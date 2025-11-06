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
	GameObject modelTest;
	DirectionalLight dirLight;
	DirectionalLight dirLight2;

	GameObject sphereEntity;
	GameObject box1Entity;
	GameObject box2Entity;
	GameObject box3Entity;
	GameObject box4Entity;

	GameLight lights[3];
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

		dirLight.position = glm::vec3(-6.0f, 10.0f, 2.0f);
		//dirLight.direction = glm::vec3(0.5f, -1.5f, -0.25f);
		dirLight.direction = glm::normalize(-dirLight.position);
		dirLight.ambientStrength = glm::vec3(0.25f);
		dirLight.diffuseStrength = glm::vec3(1.0f);
		dirLight.specularStrength = glm::vec3(1.0f);

		dirLight2.position = glm::vec3(-0.0f, 10.0f, 2.0f);
		//dirLight2.direction = glm::vec3(-6.5f, -1.5f, -0.25f);
		dirLight2.direction = glm::normalize(-dirLight2.position);
		dirLight2.ambientStrength = glm::vec3(0.25f);
		dirLight2.diffuseStrength = glm::vec3(1.0f);
		dirLight2.specularStrength = glm::vec3(1.0f);

		lights[0].position = glm::vec3(0.0f, 1.0f, -2.0f);
		lights[0].color = glm::vec3(0.2f);
		lights[1].position = glm::vec3(-2.0f, 1.0f, 0.0f);
		lights[1].color = glm::vec3(0.0f, 0.2f, 0.0f);
		lights[2].position = glm::vec3(2.0f, 1.0f, 0.0f);
		lights[2].color = glm::vec3(0.2f, 0.0f, 0.0f);


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
			scene.BindGameObject(&modelTest);
			scene.BindGameObject(&sphereEntity);
			scene.BindGameObject(&box1Entity);
			scene.BindGameObject(&box2Entity);
			scene.BindGameObject(&box3Entity);
			scene.BindGameObject(&box4Entity);

			scene.BindLight(&lights[0]);
			scene.BindLight(&lights[1]);
			scene.BindLight(&lights[2]);

			scene.BindLight(&dirLight);
			scene.BindLight(&dirLight2);
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