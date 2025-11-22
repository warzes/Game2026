#include "shadow.glsl"

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
};
uniform int pointLightCount;
uniform PointLight pointLight[MAX_POINT_LIGHTS];

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;

	float cutOff;
	float outerCutOff;
};

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

vec4 ComputeBlinnPhongLighting(vec2 texCoords, vec3 normal, vec3 viewPos, vec3 fragPos, vec4 diffuse, vec3 specular, sampler2D specularMap, float shininess)
{
    vec3 viewDir = normalize(viewPos - fragPos);
    vec4 specularTexel = texture(specularMap, texCoords) * vec4(specular, 1.0);

    vec3 lightAccumulation = vec3(0.0);

	for (int i = 0; i < dirLightCount; ++i)
	{
		lightAccumulation += ComputeDirectionalLight(dirLight[i], fragPos, diffuse, specularTexel, normal, viewDir, shininess);
	}

    //for (int i = 0; i < ssbo_Lights.length(); ++i)
    //{
    //    const mat4 light = ssbo_Lights[i];
    //    const int lightType = int(light[3][0]);

    //    switch(lightType)
    //    {
    //        case 0: lightAccumulation += ComputePointLight(light, fragPos, diffuse, specularTexel, normal, viewDir, shininess); break;
    //        case 1: lightAccumulation += ComputeDirectionalLight(light, fragPos, diffuse, specularTexel, normal, viewDir, shininess, shadowMap, lightSpaceMatrix); break;
    //        case 2: lightAccumulation += ComputeSpotLight(light, fragPos, diffuse, specularTexel, normal, viewDir, shininess); break;
    //        case 3: lightAccumulation += ComputeAmbientBoxLight(light, fragPos, diffuse); break;
    //        case 4: lightAccumulation += ComputeAmbientSphereLight(light, fragPos, diffuse); break;
    //    }
    //}
	//lightAccumulation.rgb = diffuse.rgb;

    return vec4(lightAccumulation, diffuse.a);
}