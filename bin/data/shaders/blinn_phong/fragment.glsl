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

const float alphaTestThreshold = 0.0;
vec3 Normal;

void calculateNormal()
{
	if(material.hasNormal == 1)
	{
		Normal = texture(normalTexture, fsTexCoord).rgb;
		Normal = normalize(Normal * 2.0f - 1.0f); // преобразуем из [0,1] в [-1,1]
		Normal = normalize(fsTBN * Normal);
	}
	else
		Normal = normalize(fsNormal);
}

vec3 calculateDiffuse(vec3 lightDir, vec3 diffuseStrength, vec3 objColor)
{
	float diff = max(dot(Normal, -lightDir), 0.0);
	return diff * diffuseStrength * objColor;
}

vec3 calculateSpecular(vec3 viewPos, vec3 fragPos, vec3 lightDir, vec3 specularStrength, vec3 objColor)
{
	vec3 viewDir = normalize(viewPos - fragPos);

	// if (blinnPhong){
	vec3 halfwayDir = normalize(-lightDir + viewDir);
	float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.shininess * 4);
	// else
	//vec3 reflectDir = reflect(lightDir, Normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

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

void main()
{
	vec4 rawDiffuse = 
		((material.hasDiffuse == 1) ? texture(diffuseTexture, fsTexCoord) : vec4(material.color_diffuse, 1.0)) 
		* vec4(fsColor, 1.0);

	// early discard
	if(rawDiffuse.a == alphaTestThreshold) discard;

	vec4 rawSpecular = ((material.hasSpecular == 1) ? texture(specularTexture, fsTexCoord) : vec4(material.color_specular, 1.0));

	calculateNormal();

	vec3 fragPos = fsFragPos;
	vec3 viewPos = cam.viewPos;
	if(material.hasNormal == 1)
	{
		fragPos = fsTBN * fsFragPos;
		viewPos = fsTBN * cam.viewPos;
	}

	vec3 currentColor = vec3(0.0);
	for(int l = 0; l < lightCount; ++l)
	{
		//vec3 lightPos = (material.hasNormal == 1) ? fsTBN * light[l].position : light[l].position;

		// ambient
		vec3 ambient = light[l].ambientStrength * rawDiffuse.rgb;
		
		if(light[l].type == 0)
		{
			// diffuse
			vec3 lightDir = (material.hasNormal == 1) ? normalize(fsTBN * light[l].direction) : normalize(light[l].direction);	
			vec3 diffuse = calculateDiffuse(lightDir, light[l].diffuseStrength, rawDiffuse.rgb);

			// specular
			vec3 specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, rawSpecular.rgb);

			// calculate shadow
			float illumination = 1.0;
			if(shadowOn == 1)
				illumination = 1.0 - calculateShadow(light[l].lightSpaceMatrix * vec4(fragPos, 1.0), l);

			// result
			currentColor += (ambient + illumination * (diffuse + specular)) * rawDiffuse.rgb;
		}
		else if(light[l].type == 1)
		{
		}
	}

	float alpha = rawDiffuse.a * material.opacity;
	fragColor = vec4(currentColor, alpha);
}

void main222()
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
			illumination = 1.0f - calculateShadow(light[l].lightSpaceMatrix * vec4(fsFragPos, 1.0f), l);

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
					specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, texture(specularTexture, fsTexCoord).rgb);
				else
					specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, material.color_specular);
				
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
					specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, texture(specularTexture, fsTexCoord).rgb);
				else
					specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, material.color_specular);
				
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
				vec3 specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, material.color_specular) * illumination;
				
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
				vec3 specular = calculateSpecular(viewPos, fragPos, lightDir, light[l].specularStrength, material.color_specular) * illumination;

				// putting it all together
				color += (ambient + diffuse + specular);
			}
		}
	}

	float alpha = (material.hasDiffuse == 1) ? texture(diffuseTexture, fsTexCoord).a * material.opacity : material.opacity;
	fragColor = vec4(color, alpha);
}