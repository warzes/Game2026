#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3Advance.h"
#include "NanoRender.h"
#include "NanoScene.h"
#include "GridAxis.h"
#include "GameSceneO.h"
//=============================================================================
namespace
{
	GameSceneO scene;

	Camera camera;
	GameObjectO modelTest;
	DirectionalLight dirLight;

	PointLight pointLights[10];


	GameObjectO sphereEntity;
	GameObjectO box1Entity;
	GameObjectO box2Entity;
	GameObjectO box3Entity;
	GameObjectO box4Entity;
}
//=============================================================================
void OldGameApp()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		scene.Init();

		camera.SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));

		modelTest.model.Load("data/models/ForgottenPlains/Forgotten_Plains_Demo.obj", ModelMaterialType::PBR);
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

		dirLight.direction = glm::vec3(0.3f, -0.7f, -0.4f);
		dirLight.color = glm::vec3(5.0f, 4.5f, 4.0f);

		for (int i = 0; i < 10; ++i)
		{
			float x = rand() % 20 - 10;
			float y = rand() % 20 - 10;

			pointLights[i].position = glm::vec3(x, 1.0f, y);
			pointLights[i].color = glm::vec3(5.0f, 0.8f, 4.5f);
		}

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

			scene.BindCamera(&camera);
			scene.BindGameObject(&modelTest);
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