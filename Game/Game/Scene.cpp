#include "stdafx.h"
#include "Scene.h"
#include "NanoWindow.h"

static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
const float toRadians = 3.14159265f / 180.0f;

using namespace std;

Scene::Scene()
{
	uniformProjection = 0;
	uniformModel = 0;
	uniformView = 0;
	uniformEyePosition = 0;
	uniformSpecularIntensity = 0;
	uniformShininess = 0;
	uniformFogColour = 0;

	pointLightCount = 0; spotLightCount = 0;

	deltaTime = 0.0f;
	lastTime = 0.0f;

	cout << "Created\n";
}

void Scene::calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void Scene::CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		// x     y      z	  u	    v	  nx	ny    nz
		-1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f, -10.0f, 1.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f,  10.0f, 0.0f, 1.0f,	0.0f, -1.0f, 0.0f,
		 10.0f, 0.0f,  10.0f, 1.0f, 1.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int backWallIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat backWallVertices[] = {
		-10.0f,  0.0f, -10.0f, 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		 10.0f,  0.0f, -10.0f, 1.0f, 0.0f,	0.0f, 0.0f, -1.0f,
		-10.0f, 10.0f, -10.0f, 0.0f, 1.0f,	0.0f, 0.0f, -1.0f,
		 10.0f, 10.0f, -10.0f, 1.0f, 1.0f,	0.0f, 0.0f, -1.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesho2* obj1 = new Mesho2();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesho2* obj2 = new Mesho2();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesho2* obj3 = new Mesho2();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesho2* obj4 = new Mesho2();
	obj4->CreateMesh(backWallVertices, backWallIndices, 32, 6);
	meshList.push_back(obj4);
}

void Scene::TransformAndRenderModel(Modelo2* m, Materialo2* mat, GLfloat transX, GLfloat transY, GLfloat transZ, GLfloat scale, GLfloat rotX, GLfloat rotY, GLfloat rotZ)
{
	// First translate, rotate, then scale so that it's executed as scale, rotate, translate
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(transX, transY, transZ));
	model = glm::rotate(model, rotX * toRadians, glm::vec3(1, 0, 0));
	model = glm::rotate(model, rotY * toRadians, glm::vec3(0, 1, 0));
	model = glm::rotate(model, rotZ * toRadians, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(scale, scale, scale));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	mat->UseMaterial(uniformSpecularIntensity, uniformShininess);
	m->RenderModel();
}

void Scene::TransformAndRenderMesh(Mesho2* m, Materialo2* mat, GLfloat transX, GLfloat transY, GLfloat transZ, GLfloat scale, GLfloat rotX, GLfloat rotY, GLfloat rotZ)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(transX, transY, transZ));
	model = glm::rotate(model, rotX * toRadians, glm::vec3(1, 0, 0));
	model = glm::rotate(model, rotY * toRadians, glm::vec3(0, 1, 0));
	model = glm::rotate(model, rotZ * toRadians, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(scale, scale, scale));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	marbleTexture.UseTexture();
	mat->UseMaterial(uniformSpecularIntensity, uniformShininess);
	m->RenderMesh();
}

void Scene::CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
	directionalShadowShader.CreateFromFiles("shaders/directionalShadowMap.vert", "shaders/directionalShadowMap.frag");
}

void Scene::RenderScene()
{
	TransformAndRenderMesh(meshList[2], &dullMaterial, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	TransformAndRenderMesh(meshList[3], &dullMaterial, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	TransformAndRenderMesh(meshList[3], &dullMaterial, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, -90.0f, 0.0f);
	TransformAndRenderMesh(meshList[3], &dullMaterial, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 90.0f, 0.0f);

	TransformAndRenderModel(&rook, &shinyMaterial, 8.0f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&knight, &shinyMaterial, 6.0f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&bishop, &shinyMaterial, 3.5f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&queen, &shinyMaterial, 1.0f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&king, &shinyMaterial, -1.5f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&bishop, &shinyMaterial, -4.0f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&knight, &shinyMaterial, -6.0f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&rook, &shinyMaterial, -8.0f, 0.0f, -5.0f, 1.0f, -90.0f, 0.0f, 180.0f);

	TransformAndRenderModel(&pawn, &shinyMaterial, 8.0f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, 6.0f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, 3.5f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, 1.0f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, -1.5f, 2.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, -4.0f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, -6.0f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
	TransformAndRenderModel(&pawn, &shinyMaterial, -8.0f, 0.0f, -3.0f, 1.0f, -90.0f, 0.0f, 180.0f);
}


void Scene::DirectionalShadowMapPass()
{
	directionalShadowShader.UseShader();

	glViewport(0, 0, mainLight.GetShadowMap()->GetShadowWidth(), mainLight.GetShadowMap()->GetShadowHeight());

	mainLight.GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();
	glm::mat4 temp = mainLight.CalculateLightTransform();
	directionalShadowShader.SetDirectionalLightTransform(&temp);

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::RenderPass(glm::mat4 viewMatrix)
{
	shaderList[0].UseShader();

	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformModel = shaderList[0].GetModelLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShininess = shaderList[0].GetShininessLocation();
	uniformFogColour = shaderList[0].GetFogColourLocation();

	glViewport(0, 0, window::GetWidth(), window::GetHeight());

	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
	glUniform4f(uniformFogColour, 0.4f, 0.4f, 0.4f, 1.0f);

	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount);
	shaderList[0].SetSpotLights(spotLights, spotLightCount);
	glm::mat4 temp = mainLight.CalculateLightTransform();
	shaderList[0].SetDirectionalLightTransform(&temp);

	mainLight.GetShadowMap()->Read(GL_TEXTURE1);
	shaderList[0].SetTexture(0);
	shaderList[0].SetDirectionalShadowMap(1);

	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;

	RenderScene();
}

void Scene::Render()
{
	GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
	deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
	lastTime = now;


	// Get + Handle User Input
	glfwPollEvents();

	camera.keyControl(window::GetsKeys(), deltaTime);
	camera.mouseControl(window::GetXChange(), window::GetYChange());

	DirectionalShadowMapPass();
	RenderPass(camera.calculateViewMatrix());
}

void Scene::Init()
{
	CreateObjects();
	CreateShaders();

	camera = CameraO2(glm::vec3(2.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = TextureO2("textures/brick.png");
	brickTexture.LoadTextureA();
	marbleTexture = TextureO2("textures/marble.jpg");
	marbleTexture.LoadTexture();
	plainTexture = TextureO2("textures/plain.png");
	plainTexture.LoadTextureA();

	shinyMaterial = Materialo2(4.0f, 256);
	dullMaterial = Materialo2(0.3f, 4);

	bishop = Modelo2();
	bishop.LoadModel("models/bishop.obj");

	king = Modelo2();
	king.LoadModel("models/king.obj");

	queen = Modelo2();
	queen.LoadModel("models/queen.obj");

	rook = Modelo2();
	rook.LoadModel("models/rook.obj");

	knight = Modelo2();
	knight.LoadModel("models/knight.obj");

	pawn = Modelo2();
	pawn.LoadModel("models/pawn.obj");

	mainLight = DirectionalLight(2048, 2048,
		1.0f, 1.0f, 1.0f,
		0.1f, 0.3f,
		0.0f, -15.0f, -10.0f);

	pointLights[0] = PointLight(1.0f, 1.0f, .8f,
		0.0f, 0.1f,
		0.0f, 0.0f, 0.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	pointLights[1] = PointLight(1.0f, 1.0f, 0.8f,
		0.0f, 0.1f,
		-4.0f, 2.0f, 0.0f,
		0.3f, 0.1f, 0.1f);
	pointLightCount++;

	spotLights[0] = SpotLight(1.0f, 1.0f, .8f,
		-.0f, 1.0f,
		-1.0f, 5.0f, -7.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		40.0f);
	spotLightCount++;
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.5f,
		0.0f, -1.5f, 0.0f,
		-10.0f, 2.0f, -7.0f,
		1.0f, 0.0f, 0.0f,
		30.0f);
	spotLightCount++;
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 0.5f,
		0.0f, 1.5f, 0.0f,
		10.0f, -1.0f, -8.0f,
		1.0f, 0.0f, 0.0f,
		30.0f);
	spotLightCount++;

	projection = glm::perspective(45.0f, window::GetAspect(), 0.1f, 100.0f);
}