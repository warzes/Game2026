#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

class Scene
{
public:
	std::vector<Mesho2*> meshList;
	std::vector<Shader> shaderList;
	Shader directionalShadowShader;

	std::vector<Modelo2> modelList;
	Materialo2 shinyMaterial, dullMaterial;
	Modelo2 bishop, king, queen, rook, knight, pawn;

	CameraO2 camera;

	TextureO2 brickTexture;
	TextureO2 marbleTexture;
	TextureO2 plainTexture;

	DirectionalLight mainLight;
	PointLight pointLights[MAX_POINT_LIGHTS];
	SpotLight spotLights[MAX_SPOT_LIGHTS];

	GLfloat deltaTime;
	GLfloat lastTime;

	glm::mat4 projection;

	unsigned int pointLightCount, spotLightCount;

	GLuint uniformProjection, uniformModel, uniformView, uniformEyePosition, uniformSpecularIntensity, uniformShininess, uniformFogColour;

	Scene();
	void Init();

	void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
		unsigned int vLength, unsigned int normalOffset);
	void CreateObjects();
	void CreateShaders();

	void TransformAndRenderModel(Modelo2* m, Materialo2* mat, GLfloat transX, GLfloat transY, GLfloat transZ, GLfloat scale, GLfloat rotX, GLfloat rotY, GLfloat rotZ);
	void TransformAndRenderMesh(Mesho2* m, Materialo2* mat, GLfloat transX, GLfloat transY, GLfloat transZ, GLfloat scale, GLfloat rotX, GLfloat rotY, GLfloat rotZ);
	void RenderScene();
	void DirectionalShadowMapPass();
	void RenderPass(glm::mat4 viewMatrix);
	void Render();
};