#version 330 core

struct Light
{
	int type; // 0 => directional, 1 => spot
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
	mat4 lightSpaceMatrix;
};

struct Material
{
	vec3 color_diffuse;
	vec3 color_specular;
	vec3 color_ambient;
	float opacity;
	float shininess;
	int hasDiffuse;
	int hasSpecular;
	int hasNormal;
	int nbTextures;
};

struct Camera
{
	vec3 viewPos;
};

in vec3 fsColor;
in vec3 fsNormal;
in vec2 fsTexCoord;
in vec3 fsFragPos;
in mat3 fsTBN;

uniform Camera cam;

uniform int solid;
uniform int shadowOn;
uniform int shadowMethod;
uniform float bias;
uniform int lightCount;
uniform Light light[10];

uniform Material material;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform sampler2D depthMap[10];


layout(location = 0) out vec4 fragColor;

const float alphaTestThreshold = 0.1;

float linearizeDepth(float depth)
{
	float near = 0.1;
	float far = 100.0;
	float ndc = depth * 2.0f - 1.0f;
	float z = (2.0f * near * far) / (far + near - ndc * (far - near));
	return z / far;
}

vec3 calculateDiffuse(vec3 lightDir, vec3 diffuseStrength, vec3 objColor)
{
	vec3 norm; // TODO: вычислить один раз
	if(material.hasNormal == 1)
	{
		norm = texture(normalTexture, fsTexCoord).rgb;
		norm = normalize(norm * 2.0f - 1.0f); // преобразуем из [0,1] в [-1,1]
		norm = normalize(fsTBN * norm);
	}
	else
		norm = normalize(fsNormal);

	float diff = max(dot(norm, lightDir), 0.0);
	return diff * diffuseStrength * objColor;
}

vec3 calculateSpecular(vec3 viewPos, vec3 lightDir, vec3 specularStrength, vec3 objColor)
{
	vec3 norm; // TODO: вычислить один раз
	if(material.hasNormal == 1)
	{
		norm = texture(normalTexture, fsTexCoord).rgb;
		norm = normalize(norm * 2.0f - 1.0f);
		norm = normalize(fsTBN * norm);
	}
	else
	{
		norm = normalize(fsNormal);
	}
	vec3 viewDir = normalize(viewPos - fsFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	return spec * specularStrength * objColor;
}

// Функция вычисления тени с PCF
float calculateShadow(vec4 fragPosLightSpace, int l)
{
	// Переводим из clip space в NDC, затем в [0,1] (texture coordinates)
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; // из [-1,1] в [0,1]
	// Проверяем, находится ли фрагмент в пределах теневой карты
	if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
		return 0.0;

    // Получаем размер одного пикселя теневой карты
	float texelSize = 1.0 / textureSize(depthMap[l], 0).x;
	
	float shadow = 0.0;
	// PCF: усредняем результаты в 3x3 сетке
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depthMap[l], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += (projCoords.z - bias) > pcfDepth ? 1.0 : 0.0;
		}
	}
	
	return shadow / 9.0; // усредняем
}

float percentIllumination(int l)
{
	float illumCount = 0.0f;
	float virtualIllumCount = 0.0f;
	vec2 texelSize = 1.0 / textureSize(depthMap[l], 0);

	// get pixel world coordinates
	vec3 pW = fsFragPos;

	// do normal depth comparisons
	vec4 pL = light[l].lightSpaceMatrix * vec4(pW, 1.0f);
	vec3 point = pL.xyz / pL.w;
	point = (point * 0.5) + 0.5;
	
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float depth = texture(depthMap[l], point.xy + vec2(x, y) * texelSize).r;
			if(solid == 1)
				illumCount += point.z <= depth ? 1.0f : 0.0f;
			else if(solid == 0)
				illumCount += point.z <= (depth + bias) ? 1.0f : 0.0f;
		}
	}

	if(illumCount < 9.0f && illumCount > 0.0f)
	{
		// get normal at pixel in world coordinates
		vec3 nW = fsNormal;
		float dW = -(nW.x * pW.x + nW.y * pW.y + nW.z * pW.z);
		// get tangent plane in lightsource coordinates
		vec4 tL = transpose(inverse(light[l].lightSpaceMatrix)) * vec4(nW, dW);

		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float xL = x * texelSize.x + point.x;
				float yL = y * texelSize.y + point.y;
				float depth = (-tL.x * xL) / tL.z + (-tL.y * yL) / tL.z + (-tL.w / tL.z);
				if(solid == 1)
					illumCount += point.z <= depth ? 1.0f : 0.0f;
				else if(solid == 0)
					illumCount += point.z <= (depth + bias) ? 1.0f : 0.0f;
			}
		}

		return illumCount / 9.0f;
	}
	else
		return illumCount;
}

void main()
{
	// early discard
	if(material.nbTextures > 0)
	{
		if(texture(diffuseTexture, fsTexCoord).a == 0.0)
			discard;
	}

	// start
	vec3 color = vec3(0.0);

	for(int l = 0; l < lightCount; ++l)
	{
		// calculate shadow
		float illumination = 1.0;
		if(shadowOn == 1)
			illumination = (shadowMethod == 0) 
				? 1.0f - calculateShadow(light[l].lightSpaceMatrix * vec4(fsFragPos, 1.0f), l) 
				: percentIllumination(l);

		vec3 lightPos = light[l].position;
		vec3 fragPos = fsFragPos;
		vec3 viewPos = cam.viewPos;
		if(material.hasNormal == 1)
		{
			lightPos = fsTBN * light[l].position;
			fragPos = fsTBN * fsFragPos;
			viewPos = fsTBN * cam.viewPos;
		}

		vec3 lightDir;
		float dist = length(lightPos - fragPos);
		float theta = 0.0;
		float epsilon = 0.0;
		float intensity = 1.0;

		if(light[l].type == 0)
		{
			lightDir = normalize(-light[l].direction);
		}
		else if(light[l].type == 1)
		{
			lightDir = normalize(light[l].position - fsFragPos);
			if(material.hasNormal == 1)
			{
				lightDir = fsTBN * lightDir;
			}

			theta = dot(lightDir, normalize(light[l].direction));
			epsilon = light[l].cutOff - light[l].outerCutOff;
			intensity = clamp((theta - light[l].outerCutOff) / epsilon, 0.0f, 1.0f);
		}

		if(material.nbTextures > 0)
		{
			// ambient
			vec3 ambient = light[l].ambientStrength * texture(diffuseTexture, fsTexCoord).rgb;

			if(light[l].type == 1 && theta > light[l].outerCutOff)
			{
				// diffuse
				vec3 diffuse = calculateDiffuse(lightDir, light[l].diffuseStrength, texture(diffuseTexture, fsTexCoord).rgb);

				// specular
				vec3 specular;
				if(material.hasSpecular == 1)
					specular = calculateSpecular(viewPos, lightDir, light[l].specularStrength, texture(specularTexture, fsTexCoord).rgb);
				else
					specular = calculateSpecular(viewPos, lightDir, light[l].specularStrength, material.color_specular);
				
				diffuse = diffuse * intensity * illumination;
				specular = specular * intensity * illumination;
			
				// putting it all together
				color += ambient + diffuse + specular;
			}
			else if(light[l].type == 1)
			{
				color += ambient;
			}
			else
			{
				vec3 difTex = texture(diffuseTexture, fsTexCoord).rgb;

				// diffuse
				vec3 diffuse = calculateDiffuse(lightDir, light[l].diffuseStrength, difTex);

				// specular
				vec3 specular;
				if(material.hasSpecular == 1)
					specular = calculateSpecular(viewPos, lightDir, light[l].specularStrength, texture(specularTexture, fsTexCoord).rgb);
				else
					specular = calculateSpecular(viewPos, lightDir, light[l].specularStrength, material.color_specular);
				
				color += (ambient + illumination * (diffuse + specular)) * difTex;
			}
		}
		else
		{
			// ambient
			vec3 ambient = light[l].ambientStrength * material.color_ambient;

			if(light[l].type == 1 && theta > light[l].outerCutOff)
			{
				// diffuse
				vec3 diffuse = calculateDiffuse(lightDir, light[l].diffuseStrength, material.color_diffuse) * illumination;

				// specular
				vec3 specular = calculateSpecular(viewPos, lightDir, light[l].specularStrength, material.color_specular) * illumination;
				
				diffuse = diffuse * intensity;
				specular = specular * intensity;
			
				// putting it all together
				color += ambient + diffuse + specular;
			}
			else if(light[l].type == 1)
			{
				color += ambient;
			}
			else
			{
				// diffuse
				vec3 diffuse = calculateDiffuse(lightDir, light[l].diffuseStrength, material.color_diffuse) * illumination;

				// specular
				vec3 specular = calculateSpecular(viewPos, lightDir, light[l].specularStrength, material.color_specular) * illumination;

				// putting it all together
				color += (ambient + diffuse + specular);
			}
		}
	}

	float alpha = (material.hasDiffuse == 1) ? texture(diffuseTexture, fsTexCoord).a * material.opacity : material.opacity;
	fragColor = vec4(color, alpha);
}