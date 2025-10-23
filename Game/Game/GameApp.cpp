#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3.h"
#include "NanoRender.h"
#include "NanoScene.h"
#include "GridAxis.h"
#include "GameScene.h"
//=============================================================================
namespace
{
	GameScene scene;

	Camera camera;
	Entity modelTest;
	DirectionalLight dirLight;
	//DirectionalLight dirLight2;

	Entity sphereEntity;
	Entity box1Entity;
	Entity box2Entity;
	Entity box3Entity;
	Entity box4Entity;
}
//=============================================================================
void GameApp()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		scene.Init();

		camera.SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));

		modelTest.model.Load("data/models/ForgottenPlains/Forgotten_Plains_Demo.obj");
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

		dirLight.position = glm::vec3(-6.0f, 10.0f, 2.0f);
		dirLight.direction = glm::vec3(0.5f, -1.5f, -0.25f);
		dirLight.ambientStrength = glm::vec3(0.25f);
		dirLight.diffuseStrength = glm::vec3(1.0f);
		dirLight.specularStrength = glm::vec3(1.0f);

		/*dirLight2.position = glm::vec3(0.0f, 10.0f, 0.0f);
		dirLight2.direction = glm::vec3(0.0f, 9.0f, 10.0f);
		dirLight2.ambientStrength = glm::vec3(0.25f);
		dirLight2.diffuseStrength = glm::vec3(1.0f);
		dirLight2.specularStrength = glm::vec3(1.0f);*/

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			// input
			{
				if (input::IsKeyDown(GLFW_KEY_W)) camera.ProcessKeyboard(CameraForward, engine::GetDeltaTime());
				if (input::IsKeyDown(GLFW_KEY_S)) camera.ProcessKeyboard(CameraBackward, engine::GetDeltaTime());
				if (input::IsKeyDown(GLFW_KEY_A)) camera.ProcessKeyboard(CameraLeft, engine::GetDeltaTime());
				if (input::IsKeyDown(GLFW_KEY_D)) camera.ProcessKeyboard(CameraRight, engine::GetDeltaTime());

				if (input::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
				{
					input::SetCursorVisible(false);
					camera.ProcessMouseMovement(input::GetCursorOffset().x, input::GetCursorOffset().y);
				}
				else if (input::IsMouseReleased(GLFW_MOUSE_BUTTON_RIGHT))
				{
					input::SetCursorVisible(true);
				}
			}

			scene.BindCamera(&camera);
			scene.BindEntity(&modelTest);
			scene.BindEntity(&sphereEntity);
			scene.BindEntity(&box1Entity);
			scene.BindEntity(&box2Entity);
			scene.BindEntity(&box3Entity);
			scene.BindEntity(&box4Entity);

			scene.BindLight(&dirLight);
			//scene.BindLight(&dirLight2);
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