#version 330 core

#include "../pbrCore.glsl"

struct Material
{
	sampler2D albedoMap;
	sampler2D normalMap;
	sampler2D metallicRoughnessMap;
	sampler2D aoMap;
	sampler2D emissiveMap;

	float opacity;
};

// Light structures
struct DirLight
{
	vec3 direction;
	vec3 color;
	sampler2D depthMap;
	mat4 lightSpaceMatrix;
};

struct PointLight
{
	vec3 position;
	vec3 color;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;

	float cutOff;
	float outerCutOff;
};

uniform bool hasAlbedoMap;
uniform bool hasNormalMap;
uniform bool hasMetallicRoughnessMap;
uniform bool hasAOMap;
uniform bool hasEmissiveMap;

uniform Material material;

uniform int dirLightCount;
uniform DirLight dirLight[MAX_DIR_LIGHTS];

uniform int pointLightCount;
uniform PointLight pointLight[MAX_POINT_LIGHTS];

uniform vec3 camPos;

const float alphaTestThreshold = 0.1;
const float defaultMetallic = 0.0;
const float defaultRoughness = 0.5;
const float defaultAO = 1.0;

in VS_OUT {
	vec3 VertColor;
	vec3 WorldPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
} fs_in;

layout(location = 0) out vec4 FragColor;

float calculateShadow(vec4 FragPosLightSpace, DirLight light)
{
	vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

	// Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	if (projCoords.z > 1.0)
		return 0.0;
	//if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
	//	return 0.0;


	// calculate bias (based on depth map resolution and slope) on world space normals
	//float bias = max(0.0005 * (1.0 - dot(normalize(fs_in.Normal), normalize(-light.direction))), 0.0005);
	float bias = 0.0005;

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(light.depthMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{ 
			float pcfDepth = texture(light.depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
		}    
	}
	return shadow /= 9.0;
}

vec3 calculateDirLight(DirLight light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
	// Light direction
	vec3 L = normalize(-light.direction);

	// Half vector
	vec3 H = normalize(V + L);

	// Calculate radiance (no attenuation for directional lights)
	vec3 radiance = light.color;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	// Calculate specular component
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	// Energy conservation
	vec3 ks = F;
	vec3 kD = vec3(1.0) - ks;
	kD *= 1.0 - metallic; // Metals have no diffuse reflections

	// Scale by NdotL
	float NdotL = max(dot(N, L), 0.0);

	// Combine diffuse and specular
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 CalcPointLight(PointLight light, vec3 N, vec3 fragPos, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
	// Light direction
	vec3 L = normalize(light.position - fragPos);

	// Half vector
	vec3 H = normalize(V + L);

	// Caclulate distance and attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / max(distance * distance, 0.001);
	vec3 radiance = light.color * attenuation;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	// Calculate specular component
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	// kS is equal to Fresnel
	vec3 ks = F;

	// For energy conservation, the diffuse and specular light can't be above 1.0 (unless the surface is emissive)
	// So we set the diffuse component kD to 1.0 - kS
	vec3 kD = vec3(1.0) - ks;

	// Multiply kD by the inverse metalness such that only non-metals have diffuse lighting,
	// or a linear blend if partially metal (pure metals have no diffuse light)
	kD *= 1.0 - metallic;

	// Scale light by NdotL
	float NdotL = max(dot(N, L), 0.0);

	// outgoing radiance
	return (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 CalcSpotLight(SpotLight light, vec3 N, vec3 fragPos, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
	// Light direction
	vec3 L = normalize(light.position - fragPos);

	// Half vector
	vec3 H = normalize(V + L);

	// spotlight intensity
	float theta = dot(L, normalize(-light.direction)); 
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// Calculate distance and attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / max(distance * distance, 0.001);
	vec3 radiance = light.color * attenuation * intensity;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	// Calculate specular component
	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	// Energy conservation
	vec3 ks = F;
	vec3 kD = vec3(1.0) - ks;
	kD *= 1.0 - metallic; // Metals have no diffuse reflections

	// Scale by NdotL
	float NdotL = max(dot(N, L), 0.0);

	// Combine diffuse and specular
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

void main()
{
	vec4 albedo = vec4(fs_in.VertColor, 1.0);
	if (hasAlbedoMap)
	{
		albedo = texture(material.albedoMap, fs_in.TexCoords) * albedo;
	}
	// early discard
	if(albedo.a < alphaTestThreshold) discard;

	vec3 metallicRoughness = hasMetallicRoughnessMap ? texture(material.metallicRoughnessMap,  fs_in.TexCoords).rgb : vec3(0.0, defaultRoughness, defaultMetallic);
	float metallic = metallicRoughness.b;
	float roughness = metallicRoughness.g;

	float ao = hasAOMap ? texture(material.aoMap, fs_in.TexCoords).r : defaultAO;

	vec3 emission = hasEmissiveMap ? texture(material.emissiveMap, fs_in.TexCoords).rgb : vec3(0.0);

	vec3 normal;
	if (hasNormalMap)
	{
		vec3 normalMap = texture(material.normalMap, fs_in.TexCoords).rgb;
		normalMap = normalMap * 2.0 - 1.0; // Transform from [0,1] to [-1,1]
		normal = normalize(fs_in.TBN * normalMap);
	}
	else
	{
		normal = normalize(fs_in.Normal);
	}

	// View direction
	vec3 viewDir = normalize(camPos - fs_in.WorldPos);
	float NdotV = max(dot(normal, viewDir), 0.0);

	// Calculate fresnel reflectance at normal incidence
	// If dialectric (like plastic) use F0 of 0.04
	// If metal, use albedo color as F0
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo.rgb, metallic);

	vec3 currentLightColor = vec3(0.0);

	// Directional Light
	for(int i = 0; i < dirLightCount; i++)
	{
		vec4 FragPosLightSpace = dirLight[i].lightSpaceMatrix * vec4(fs_in.WorldPos, 1.0);
		// Calculate shadow
		float shadow = calculateShadow(FragPosLightSpace, dirLight[i]);
		vec3 dirLightContribution = calculateDirLight(dirLight[i], normal, viewDir, albedo.rgb, metallic, roughness, F0);
		dirLightContribution *= (1.0 - shadow); // Apply shadow to directional light

		currentLightColor += dirLightContribution;
	}

	// Point Lights
	for (int i = 0; i < pointLightCount; i++)
	{
		currentLightColor += CalcPointLight(pointLight[i], normal, fs_in.WorldPos, viewDir, albedo.rgb, metallic, roughness, F0);
	}

	// Default ambient term if not using IBL
	vec3 ambient = vec3(0.03) * albedo.rgb * ao;

	//if (useIBL)
	//{
	//}

	// Combine ambient and reflectance and emission
	vec3 color = ambient + currentLightColor + emission;

	FragColor = vec4(color, albedo.a * material.opacity);
}