#include "stdafx.h"
#include "GameApp.h"
#include "GameScene.h"
#include "GeomMap.h"
#include "MapGrid.h"
#include "Map.h"
//=============================================================================
namespace
{
	GameScene scene;
	Camera camera;
	GameModel modelLevel;

	Map map;
	MapChunk geommaps;
}
//=============================================================================
void GameApp()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		if (!scene.Init())
			return;

		if (!geommaps.Init(map))
			return;

		camera.MovementSpeed = 10.0f;
		camera.SetPosition(glm::vec3(0.0f, 2.5f, -1.0f));

		modelLevel.model.Load("data/models/ForgottenPlains/Forgotten_Plains_Demo.obj", ModelMaterialType::BlinnPhong);
		modelLevel.modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(-30.0f, -10.0f, 15.0f));

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

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

			// cursor
			if (input::IsMouseDown(RGFW_mouseLeft))
			{
				auto mpos = input::GetCursorPos();
				glm::vec3 rayOrigin = camera.Position;
				glm::vec3 rayDirection = GetRayFromScreen(
					mpos.x, mpos.y, window::GetWidth(), window::GetHeight(), 
					camera.GetViewMatrix(), 
					glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f));

				auto selectedTile = map.RaycastTile(rayOrigin, rayDirection);

				if (selectedTile.tile != NoTile)
				{
					Print("x=" + std::to_string(selectedTile.x) + ";y=" + std::to_string(selectedTile.y) + ";z=" + std::to_string(selectedTile.z));
				}
			}



			scene.Bind(&camera);
			scene.Bind(geommaps.GetModel());
			scene.Draw();

			// ui

			{
				if (const ImGuiViewport* v = ImGui::GetMainViewport())
				{
					ImGui::SetNextWindowPos({ v->WorkPos.x + 15.0f, v->WorkPos.y + 15.0f }, ImGuiCond_Always, { 0.0f, 0.0f });
				}
				ImGui::SetNextWindowBgAlpha(0.30f);
				ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize("VertexCount : __________").x, 0));
				if (ImGui::Begin("##Map", nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
					ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
				{
					ImGui::Text("Map Info :");
					ImGui::Text("VertexCount : %i", (int)geommaps.GetVertexCount());
					ImGui::Text("IndexCount  : %i", (int)geommaps.GetIndexCount());
				}
				ImGui::End();
			}

			engine::DrawFPS();

			engine::EndFrame();
		}
	}
	catch (const std::exception& exc)
	{
		puts(exc.what());
	}

	geommaps.Close();
	engine::Close();
}
//=============================================================================