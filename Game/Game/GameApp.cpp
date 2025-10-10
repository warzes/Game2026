#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3.h"
#include "NanoRender.h"
#include "NanoScene.h"
#include "GridAxis.h"
#include "Scene.h"
#include "Region.h"
//=============================================================================
namespace
{
	Scene scene;

	Camera camera;

	Entity modelTest;
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

		scene.GetDirectionalLights().push_back(DirectionalLight{ glm::vec3(-6.0f, 10.0f, 2.0f), glm::vec3(0.25f), glm::vec3(5.0f), glm::vec3(1.0f), glm::vec3(0.5f, -1.5f, -0.25f) });

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

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

			scene.BindEntity(&gRegion.model);

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