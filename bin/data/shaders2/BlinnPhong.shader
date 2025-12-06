struct DirLight
{
	vec3 direction;
	vec3 color;
	float luminosity;
	sampler2D depthMap;
	mat4 lightSpaceMatrix;
};
uniform int dirLightCount;
uniform DirLight dirLight[MAX_DIR_LIGHTS];

struct PointLight
{
	vec3 position;
	vec3 color;
	vec3 attenuation;
	float intensity;
};
uniform int pointLightCount;
uniform PointLight pointLight[MAX_POINT_LIGHTS];

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;
	
	vec3 attenuation;
	float intensity;
	float cutOff;
	float outerCutOff;
};
uniform int spotLightCount;
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

struct AmbientBoxLight
{
	vec3 size;
	vec3 position;
	vec3 color;
	float intensity;
};
uniform int ambientBoxLightCount;
uniform AmbientBoxLight ambientBoxLight[MAX_AMBIENT_BOX_LIGHTS];

struct AmbientSphereLight
{
	vec3 position;
	vec3 color;
	float intensity;
	float radius;
};
uniform int ambientSphereLightCount;
uniform AmbientSphereLight ambientSphereLight[MAX_AMBIENT_SPHERE_LIGHTS];

vec3 BlinnPhong(vec3 lightDir, vec3 lightColor, float luminosity, vec4 diffuseTexel, vec4 specularTexel, vec3 normal, vec3 viewDir, float shininess)
{
    vec3  halfwayDir          = normalize(lightDir + viewDir);
    float diffuseCoefficient  = max(dot(normal, lightDir), 0.0);
    float specularCoefficient = pow(max(dot(normal, halfwayDir), 0.0), shininess * 2.0);

    return lightColor * diffuseTexel.rgb * diffuseCoefficient * luminosity + ((luminosity > 0.0) ? (lightColor * specularTexel.rgb * specularCoefficient * luminosity) : vec3(0.0));
}

vec3 ComputeDirectionalLight(DirLight light, vec3 fragPos, vec4 diffuseTexel, vec4 specularTexel, vec3 normal, vec3 viewDir, float shininess)
{
	vec3 lightDir = -light.direction;
	vec3 blinnPhong = BlinnPhong(lightDir, light.color, light.luminosity, diffuseTexel, specularTexel, normal, viewDir, shininess);

	vec4 fragPosLightSpace = light.lightSpaceMatrix * vec4(fragPos, 1.0);
	float shadow = CalculateShadow(fragPosLightSpace, light.depthMap, normal, lightDir);
	blinnPhong *= 1.0 - shadow;

	return blinnPhong;
}

float PointLuminosityFromAttenuation(PointLight light, vec3 fragPos)
{
	float constant = light.attenuation.x;
	float linear = light.attenuation.y;
	float quadratic = light.attenuation.z;

	float distanceToLight = length(light.position - fragPos);
	float attenuation = (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));

	return 1.0 / attenuation;
}

float SpotLuminosityFromAttenuation(SpotLight light, vec3 fragPos)
{
	float constant = light.attenuation.x;
	float linear = light.attenuation.y;
	float quadratic = light.attenuation.z;

	float distanceToLight = length(light.position - fragPos);
	float attenuation = (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));

	return 1.0 / attenuation;
}

vec3 ComputePointLight(PointLight light, vec3 fragPos, vec4 diffuseTexel, vec4 specularTexel, vec3 normal, vec3 viewDir, float shininess)
{
	vec3 lightDirection = normalize(light.position - fragPos);
	float luminosity = PointLuminosityFromAttenuation(light, fragPos);

	return BlinnPhong(lightDirection, light.color, light.intensity * luminosity, diffuseTexel, specularTexel, normal, viewDir, shininess);
}

vec3 ComputeSpotLight(SpotLight light, vec3 fragPos, vec4 diffuseTexel, vec4 specularTexel, vec3 normal, vec3 viewDir, float shininess)
{
	float cutOff = cos(light.cutOff); // cos ???
	float outerCutOff = cos(radians(light.cutOff + light.outerCutOff));

	vec3 lightDirection = normalize(light.position - fragPos);
	float luminosity = SpotLuminosityFromAttenuation(light, fragPos);

	/* Calculate the spot intensity */
	float theta = dot(lightDirection, normalize(-light.direction));
	float epsilon = cutOff - outerCutOff;
	float spotIntensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

	return BlinnPhong(lightDirection, light.color, light.intensity * spotIntensity * luminosity, diffuseTexel, specularTexel, normal, viewDir, shininess);
}

bool IsPointInAABB(vec3 point, vec3 aabbCenter, vec3 aabbHalfSize)
{
	return
		point.x > aabbCenter.x - aabbHalfSize.x && point.x < aabbCenter.x + aabbHalfSize.x &&
		point.y > aabbCenter.y - aabbHalfSize.y && point.y < aabbCenter.y + aabbHalfSize.y &&
		point.z > aabbCenter.z - aabbHalfSize.z && point.z < aabbCenter.z + aabbHalfSize.z;
}

bool IsPointInSphere(vec3 point, vec3 sphereCenter, float sphereRadius)
{
	return distance(point, sphereCenter) <= sphereRadius;
}


vec3 ComputeAmbientBoxLight(AmbientBoxLight light, vec3 fragPos, vec4 diffuseTexel)
{
	return IsPointInAABB(fragPos, light.position, light.size) ? diffuseTexel.rgb * light.color * light.intensity : vec3(0.0);
}

vec3 ComputeAmbientSphereLight(AmbientSphereLight light, vec3 fragPos, vec4 diffuseTexel)
{
	return IsPointInSphere(fragPos, light.position, light.radius) ? diffuseTexel.rgb * light.color * light.intensity : vec3(0.0);
}

vec4 ComputeBlinnPhongLighting(vec2 texCoords, vec3 normal, vec3 viewPos, vec3 fragPos, vec4 diffuse, vec3 specular, sampler2D specularMap, float shininess)
{
	vec3 viewDir = normalize(viewPos - fragPos);
	vec4 specularTexel = texture(specularMap, texCoords) * vec4(specular, 1.0);

	vec3 lightAccumulation = vec3(0.0);

	for (int i = 0; i < dirLightCount; ++i)
	{
		lightAccumulation += ComputeDirectionalLight(dirLight[i], fragPos, diffuse, specularTexel, normal, viewDir, shininess);
	}
	for (int i = 0; i < pointLightCount; ++i)
	{
		lightAccumulation += ComputePointLight(pointLight[i], fragPos, diffuse, specularTexel, normal, viewDir, shininess);
	}
	for (int i = 0; i < spotLightCount; ++i)
	{
		lightAccumulation += ComputeSpotLight(spotLight[i], fragPos, diffuse, specularTexel, normal, viewDir, shininess);
	}
	for (int i = 0; i < ambientBoxLightCount; ++i)
	{
		lightAccumulation += ComputeAmbientBoxLight(ambientBoxLight[i], fragPos, diffuse);
	}
	for (int i = 0; i < ambientSphereLightCount; ++i)
	{
		lightAccumulation += ComputeAmbientSphereLight(ambientSphereLight[i], fragPos, diffuse);
	}

	return vec4(lightAccumulation, diffuse.a);
}