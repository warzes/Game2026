vec2 TileAndOffsetTexCoords(vec2 texCoords, vec2 tiling, vec2 offset)
{
	return texCoords * tiling + offset;
}

bool IsOrthographic(mat4 projectionMatrix)
{
	// In an orthographic projection matrix, the [3][3] element is 1.0
	// In a perspective projection matrix, it's 0.0
	return projectionMatrix[3][3] > 0.5;
}

bool IsParallaxOutOfBounds(vec2 texCoords, vec2 tiling, vec2 offset, mat4 projectionMatrix)
{
	vec2 adjustedCoords = texCoords - offset;

	return
		!IsOrthographic(projectionMatrix) && // No clipping in orthographic projection (not supported)
		(adjustedCoords.x < 0.0 || adjustedCoords.x > tiling.x || adjustedCoords.y < 0.0 || adjustedCoords.y > tiling.y);
}

// Expects a height map with values in the range [0, 1].
// 1.0 means the height is at the maximum depth, 0.0 means the height is at the minimum depth.
vec2 ApplyParallaxOcclusionMapping(vec2 texCoords, sampler2D heightMap, vec3 tangentViewPos, vec3 tangentFragPos, float heightScale, int minLayers, int maxLayers)
{
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);

	// calculate optimal layer count
	minLayers = max(minLayers, 1); // Ensure minLayers is at least 1
	maxLayers = min(maxLayers, 8192); // Limit maxLayers to a reasonable value (2^13)
	maxLayers = max(maxLayers, minLayers); // Ensure maxLayers is at least minLayers

	// clamp alpha to prevent extrapolating (which could make the fragment very expensive, and potentially result in a crash).
	float alpha = clamp(abs(dot(vec3(0.0, 0.0, 1.0), viewDir)), 0.0, 1.0);
	float numLayers = mix(maxLayers, minLayers, alpha);

	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * heightScale; 
	vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = 1.0 - texture(heightMap, currentTexCoords).r;
	float currentLayerDepth = 0.0;

	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = 1.0 - texture(heightMap, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = 1.0 - texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}