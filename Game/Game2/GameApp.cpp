#include "stdafx.h"
#include "GameApp.h"
#include "GameScene.h"

/*

arthurian legends


https://www.youtube.com/shorts/ksffC38Jg_o
https://www.youtube.com/@retro-dev2798/videos
http://www.3dcpptutorials.sk/index.php?id=46
https://github.com/AEspinosaDev/OpenGL-Renderer
https://github.com/mharrys/omni
*/
/*
 https://github.com/muleax/slope
 tinyfx
 https://github.com/nfginola/dx11-tech
*/
/*
idea:
- https://hardboiledcrab.itch.io/tiled-mediaval-building  - example tile house building
- карту делать из таких маленьких одноцветных кубиков - https://atapataco.itch.io/cube-tilemap-desert
		или https://wizp.itch.io/3d-tileset
*/
/*
отделить assimp от model. возможность формировать модель из даты ассимпа
вывод квада на экран (в том числе в какую-то область) - чтобы тестить рендеры в текстуру)
*/

//=============================================================================
namespace
{
	GameScene scene;

	GameCamera cameraGame;
	GameModel modelLevel;
	GameDirectionalLight* directionalLight;
	GamePointLight* pointLight1;
	GamePointLight* pointLight2;

	Camera camera;
	OldGameObject modelTest;
	DirectionalLight dirLight;

	PointLight pointLights[10];

	AmbientSphereLight ambientSphereLight;

	OldGameObject sphereEntity;
	OldGameObject box1Entity;
	OldGameObject box2Entity;
	OldGameObject box3Entity;
	OldGameObject box4Entity;
}
//=============================================================================
void GameApp()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		scene.Init();

		cameraGame.SetFOV(60.0f);
		cameraGame.SetFar(150);
		cameraGame.SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));

		modelLevel.LoadModel("data/models/ForgottenPlains/Forgotten_Plains_Demo.obj");
		modelLevel.SetPosition(glm::vec3(-30.0f, 0.0f, 15.0f));

		directionalLight = new GameDirectionalLight(glm::vec3(-5.0f, -5.0f, 5.0f), glm::vec3(1.0f, 0.8f, 0.8f), 2.0f);
		directionalLight->SetPosition(glm::vec3(-5.0f, -5.0f, 5.0f));

		pointLight1 = new GamePointLight(glm::vec3(-2.0f, 5.0f, 3.5f), glm::vec3(1.0f, 0.2f, 0.3f), 0.6f, 10);
		pointLight2 = new GamePointLight(glm::vec3(2.0f, 5.0f, 3.5f), glm::vec3(0.0f, 0.3f, 1.5f), 0.4f, 10);

		camera.SetPosition(glm::vec3(0.0f, 0.5f, 0.5f));

		modelTest.model.Load("data/models/ForgottenPlains/Forgotten_Plains_Demo.obj", ModelMaterialType::BlinnPhong);
		modelTest.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, 0.0f, 15.0f));

		sphereEntity.model.Create(GeometryGenerator::CreateSphere(0.5f, 16, 16));
		sphereEntity.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));

		box1Entity.model.Create(GeometryGenerator::CreateBox());
		box1Entity.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 0.0f));
		box2Entity.model.Create(GeometryGenerator::CreateBox());
		box2Entity.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 2.0f, 0.0f));
		box3Entity.model.Create(GeometryGenerator::CreateBox());
		box3Entity.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -1.0f));
		box4Entity.model.Create(GeometryGenerator::CreateBox());
		box4Entity.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		dirLight.direction = glm::vec3(-0.2f, -0.6f, -0.2f);
		dirLight.color = glm::vec3(1.0f, 0.9f, 0.9f);
		dirLight.luminosity = 3;

		for (int i = 0; i < 10; ++i)
		{
			float x = rand() % 20 - 10;
			float y = rand() % 20 - 10;

			pointLights[i].position = glm::vec3(x, 1.0f, y);
			pointLights[i].color = glm::vec3(1.0f, 0.0f, 1.0f);
		}

		ambientSphereLight.position = glm::vec3(2.0f, 0.0f, 0.0f);
		ambientSphereLight.radius = { 2.0f };
		ambientSphereLight.intensity = 10.0f;
		ambientSphereLight.color = glm::vec3(1.0f, 0.0, 0.0f);

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			// input
			{
				if (input::IsKeyDown(RGFW_w)) camera.ProcessKeyboard(CameraForward, engine::GetDeltaTime());
				if (input::IsKeyDown(RGFW_s)) camera.ProcessKeyboard(CameraBackward, engine::GetDeltaTime());
				if (input::IsKeyDown(RGFW_a)) camera.ProcessKeyboard(CameraLeft, engine::GetDeltaTime());
				if (input::IsKeyDown(RGFW_d)) camera.ProcessKeyboard(CameraRight, engine::GetDeltaTime());

				if (input::IsMouseDown(RGFW_mouseRight))
				{
					input::SetCursorVisible(false);
					camera.ProcessMouseMovement(input::GetCursorOffset().x, input::GetCursorOffset().y);
				}
				else if (input::IsMouseReleased(RGFW_mouseRight))
				{
					input::SetCursorVisible(true);
				}
			}

			scene.Bind(&cameraGame);
			scene.Bind(&modelLevel);
			scene.Bind(directionalLight);
			scene.Bind(pointLight1);
			scene.Bind(pointLight2);

			scene.BindCamera(&camera);

			scene.BindGameObject(&sphereEntity);
			scene.BindGameObject(&box1Entity);
			scene.BindGameObject(&box2Entity);
			scene.BindGameObject(&box3Entity);
			scene.BindGameObject(&box4Entity);

			scene.BindLight(&dirLight);
			for (size_t i = 0; i < 10; i++)
			{
				scene.BindLight(&pointLights[i]);
			}
			scene.BindLight(&ambientSphereLight);

			scene.Draw();

			engine::DrawFPS();

			engine::EndFrame();
		}
	}
	catch (const std::exception& exc)
	{
		puts(exc.what());
	}
	engine::Close();
}
//=============================================================================