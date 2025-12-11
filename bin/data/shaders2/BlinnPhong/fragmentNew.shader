#version 330 core

//==========================================
// Lights
struct PointLight
{
	vec3 pos;
	vec3 modelPos;
	vec3 color;
	float intensity;
	float constant;
	float linear;
	float quadratic;
	float att;

	bool castShadows;
	samplerCube shadowMap;
	mat4 lightViewProj;
};

struct DirectionalLight
{
	vec3 dir;
	vec3 color;
	float intensity;

	bool castShadows;
	sampler2D shadowMap;
	mat4 lightViewProj;
};

struct SpotLight
{
	vec3 pos;
	vec3 dir;
	vec3 color;
	float constant;
	float linear;
	float quadratic;
	float att;

	bool castShadows;
	sampler2D shadowMap;
	mat4 lightViewProj;
};

//==========================================
// Material

struct Material
{
	bool hasColorTex;
	bool hasSpecularTex;
	bool hasGlossTex;
	bool hasNormalTex;
	bool hasOpacityTex;

	sampler2D colorTex;
	sampler2D normalTex;
	sampler2D specularTex;
	sampler2D glossTex;
	sampler2D opacityTex;

	float opacity;
	vec3 baseColor;
	float specularity;
	float shininess;
	bool receiveShadows;
};

in VS_OUT {
	vec3 vertColor;
	vec3 pos;
	vec3 modelPos;
	vec2 texCoords;
	mat3 TBN;
	vec3 normal;
} fs_in;

const float alphaClippingThreshold = 0.1;

//Uniforms
uniform Material material;
uniform int pointsLightsNumber;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int directionalLightsNumber;
uniform DirectionalLight directionalLights[MAX_DIR_LIGHTS];
uniform int spotLightsNumber;
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform float shadowsFarPlane;
uniform float ambientStrength;
uniform vec3 ambientColor;
uniform samplerCube skybox;
uniform bool alphaTest;

//Surface properties
vec3 N;
vec3 albedo;
float specularity;
float opacity;
float shininess;

vec3 color = vec3(1, 1, 1);

layout(location = 0) out vec4 FragColor;

float computeAttenuation(vec3 lightPos, float att)
{
	float d = length(lightPos - fs_in.pos);
	float rd = 10.0f;

	return pow(att / max(d, 0.0001), 2);
	//return pow(max(1 - pow(att / max(d, 0.0001),2), 0), 2);

	//float constant = 1.0f;
	//return 1.0 / (constant + lin * d + quad * (d * d)); OLD METHOD
}

float computeShadow(sampler2D shadowMap, mat4 lightViewProj, vec3 lightDir) {

	vec4 pos_lightSpace = lightViewProj * vec4(fs_in.modelPos, 1.0);

	// perform perspective divide

	vec3 projCoords = pos_lightSpace.xyz / pos_lightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float bias = max(0.0005 * (1.0 - dot(fs_in.normal, lightDir)), 0.00005);
	//bias = 0.005;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	if (projCoords.z > 1.0)
		shadow = 0.0;

	

	return shadow;
	//return lightDir.x;


}

float computePointShadow(samplerCube shadowMap ,vec3 lightPos)
{
	// get vector between fragment position and light position
	vec3 fragToLight = fs_in.modelPos - lightPos;
	// use the light to fragment vector to sample from the depth map    
	float closestDepth = texture(shadowMap, fragToLight).r;
	// it is currently in linear range between [0,1]. Re-transform back to original value
	closestDepth *= shadowsFarPlane;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// now test for shadows
	//float bias = 0.0; //for now
	float bias = max(0.005 * (1.0 - dot(fs_in.normal, fragToLight)), 0.0005);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	// discard shadowing fragments further away
	if (currentDepth > shadowsFarPlane) shadow = 0.0;


	return shadow;
}

vec3 shadePointLight(vec3 lightPos, vec3 color, float intensity,float att, samplerCube shadowMap, vec3 worldPos, bool castShadows) {

	//Diffuse
	vec3 L = normalize(lightPos - fs_in.pos);
	vec3 diffuse = max(dot(L, N), 0.0) * color;

	//Specular
	vec3 V = normalize(-fs_in.pos);
	vec3 R = normalize(reflect(-L, N));
	float factor = max(dot(R, V), 0.0);
	vec3 specular = pow(factor, shininess) * specularity * color;

	//Attenuation
	float attenuation = computeAttenuation(lightPos, att);
	diffuse *= attenuation;
	specular *= attenuation;

	//Shadow 
	float shadow;
	material.receiveShadows ? shadow = computePointShadow(shadowMap, worldPos) : shadow = 0.0;

	vec3 result = (1.0 - shadow) * (diffuse + specular) * albedo;
	result *= intensity;
	return result;

}
vec3 shadeDirectionalLight(vec3 lightDir, vec3 color, float intensity, sampler2D shadowMap, mat4 lightViewProj, bool castShadows) {

	//Diffuse
	vec3 L = normalize(lightDir);
	vec3 diffuse = max(dot(L, N), 0.0) * color;

	//Specular
	vec3 V = normalize(-fs_in.pos);
	vec3 R = normalize(reflect(-L, N));
	float factor = max(dot(R, V), 0.0);
	vec3 specular = pow(factor, shininess) * specularity *color;


	//Shadow 
	float shadow;
	material.receiveShadows ? shadow = computeShadow(shadowMap, lightViewProj, L) : shadow = 0.0;

	vec3 result = (1.0 - shadow) * (diffuse + specular) * albedo;
	result *= intensity;
	return result;

}
vec3 shadeAmbientLight() {
	return albedo * ambientColor * ambientStrength;
}

vec3 shade() {

	vec3 result = vec3(0.0);

	//Just ambient
	result += shadeAmbientLight();

	for (int i = 0; i < pointsLightsNumber; i++) {
        result += shadePointLight(pointLights[i].pos, pointLights[i].color, pointLights[i].intensity, pointLights[i].att, pointLights[i].shadowMap, pointLights[i].modelPos,
			pointLights[i].castShadows);
	}

	for (int i = 0; i < directionalLightsNumber; i++) {
		result += shadeDirectionalLight(directionalLights[i].dir, directionalLights[i].color, directionalLights[i].intensity,
			directionalLights[i].shadowMap, directionalLights[i].lightViewProj, directionalLights[i].castShadows);
	}
	//for (int i = 0; i < spotLightsNumber; i++) {
	//	result += shadePointLight(spotLights[i].pos, spotLights[i].color, spotLights[i].intensity);
	//}

	return result;

}

void main()
{
	material.hasNormalTex ? N = normalize(fs_in.TBN * (texture(material.normalTex, fs_in.texCoords).rgb * 2.0 - 1.0)) : N = fs_in.normal;
	material.hasColorTex ? albedo = texture(material.colorTex, fs_in.texCoords).rgb : albedo = material.baseColor;
	material.hasSpecularTex ? specularity = texture(material.specularTex, fs_in.texCoords).r : specularity = material.specularity;
	material.hasGlossTex ? shininess = texture(material.glossTex, fs_in.texCoords).r : shininess = material.shininess;
	material.hasOpacityTex ? opacity = texture(material.opacityTex, fs_in.texCoords).r : opacity = material.opacity;
	if (opacity <= 0.0 && alphaTest)discard;

	vec3 r = normalize(reflect(normalize(fs_in.pos), N));
	vec3 result = mix(shade(), vec3(texture(skybox, r).rgb), 0.0);

	//result = pow(result, vec3(1.0 / 2.2));
	FragColor = vec4(result, opacity);
}