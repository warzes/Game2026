#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3Advance.h"
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

	Entity2 modelPlane;
	Entity2 modelBox;
	Entity2 modelSphere;
	Entity2 modelTest;
	Entity2 modelTest2;
}
//=============================================================================
void ExampleApp002()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		scene.Init();
		camera.SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));
		//scene.SetGridAxis(22);

		modelPlane.model.Create(GeometryGenerator::CreatePlane(100, 100, 100, 100));
		modelBox.model.Create(GeometryGenerator::CreateBox());
		modelSphere.model.Create(GeometryGenerator::CreateSphere());
		modelTest.model.Load("data/models/tree.glb", ModelMaterialType::BlinnPhong);
		//modelTest.model.Load("data/models/ForgottenPlains/Forgotten_Plains_Demo.obj");

		gRegion.Init();
		
		modelTest2.model.Load("data/models/cottage/cottage_obj.obj", ModelMaterialType::BlinnPhong);
		// TRS matrix
		modelTest2.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -3.0f));
		// rotate mat
		modelTest2.modelMat = glm::scale(modelTest2.modelMat, glm::vec3(0.2f));

		scene.GetDirectionalLights().push_back(DirectionalLightO{ glm::vec3(-6.0f, 10.0f, 2.0f), glm::vec3(0.25f), glm::vec3(5.0f), glm::vec3(1.0f), glm::vec3(0.5f, -1.5f, -0.25f) });

		//auto spot = SpotLight(
		//	glm::vec3(0.0f, 0.5f, 4.5f),  // pos
		//	glm::vec3(0.025f), // amb
		//	glm::vec3(100.0f), // diff
		//	glm::vec3(1.0f), // spec
		//	glm::vec3(-4.37, -0.5f, -3.5f), // dir
		//	30.0f, // innerAngle
		//	35.0f); // outerAngle
		//scene.GetSpotLight().push_back(spot);

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

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
			scene.BindEntity(&modelTest);
			scene.BindEntity(&modelTest2);

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