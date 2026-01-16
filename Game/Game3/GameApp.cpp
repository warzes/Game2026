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

glm::vec3 mouseToWorldOnPlane(float mouseX, float mouseY, int windowWidth, int windowHeight, const glm::mat4& view, const glm::mat4& projection)
{
	// Преобразование экранных координат в нормализованные координаты устройства (NDC)
	// NDC координаты находятся в диапазоне [-1, 1], где (-1,-1) - левый нижний угол, (1,1) - правый верхний
	float ndcX = (2.0f * mouseX) / windowWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY) / windowHeight; // Инвертируем Y, так как ось Y в экране направлена вниз,
	// а в NDC ось Y направлена вверх

	// Создаем луч в пространстве NDC
	// rayClip - это точка в clip space с координатами (ndcX, ndcY, -1, 1)
	// Мы используем z=-1, чтобы получить точку на ближней плоскости отсечения
	glm::vec4 rayClip = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);

	// Преобразуем луч из Clip Space в Eye Space (видовое пространство)
	// Для этого умножаем на обратную матрицу проекции
	glm::mat4 inverseProjection = glm::inverse(projection);
	glm::vec4 rayEye = inverseProjection * rayClip;
	// В clip space w-компонента важна для перспективного деления, но после умножения на обратную
	// матрицу проекции мы получаем координаты в eye space, где w уже не равен 1
	// Для вектора направления устанавливаем w=0, чтобы игнорировать влияние переноса
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Устанавливаем w=0 для вектора направления

	// Преобразуем луч из Eye Space в World Space
	// Для этого умножаем на обратную матрицу вида
	glm::mat4 inverseView = glm::inverse(view);
	glm::vec3 rayWorld = glm::vec3(inverseView * rayEye);
	rayWorld = glm::normalize(rayWorld); // Нормализуем вектор направления луча

	// Получаем позицию камеры из матрицы вида
	// Последний столбец матрицы вида содержит информацию о положении камеры
	glm::vec3 cameraPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);

	// Решаем уравнение пересечения луча с плоскостью Y=0
	// Уравнение плоскости: Y = 0
	// Уравнение луча: P(t) = cameraPos + t * rayWorld
	// Подставляем в уравнение плоскости: cameraPos.y + t * rayWorld.y = 0
	// Отсюда: t = -(cameraPos.y - 0) / rayWorld.y
	// Формула: t = (плоскость_y - начальная_точка_y) / направление_y
	float t = -(cameraPos.y - 0) / rayWorld.y; // Для плоскости Y=0

	// Вычисляем точку пересечения
	glm::vec3 intersectionPoint = cameraPos + t * rayWorld;

	// Возвращаем точку с Y=0 (гарантированно лежит на плоскости Y=0)
	return glm::vec3(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
}

glm::vec3 roundCoordinates(const glm::vec3& coords)
{
	return glm::vec3(
		(coords.x >= 0.0f) ? static_cast<float>(static_cast<int>(coords.x + 0.5f)) : static_cast<float>(static_cast<int>(coords.x - 0.5f)),
		(coords.y >= 0.0f) ? static_cast<float>(static_cast<int>(coords.y + 0.5f)) : static_cast<float>(static_cast<int>(coords.y - 0.5f)),
		(coords.z >= 0.0f) ? static_cast<float>(static_cast<int>(coords.z + 0.5f)) : static_cast<float>(static_cast<int>(coords.z - 0.5f))
	);
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
			{
				auto mpos = input::GetCursorPos();	
				glm::vec3 worldCoords = mouseToWorldOnPlane(mpos.x, mpos.y, window::GetWidth(), window::GetHeight(), camera.GetViewMatrix(), glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f));
				worldCoords = roundCoordinates(worldCoords);
				gEditorCursor->SetPosition(worldCoords);
			}

			if (input::IsMouseDown(RGFW_mouseLeft))
			{
				//auto mpos = input::GetCursorPos();
				//glm::vec3 rayOrigin = camera.Position;
				//glm::vec3 rayDirection = GetRayFromScreen(
				//	mpos.x, mpos.y, window::GetWidth(), window::GetHeight(), 
				//	camera.GetViewMatrix(), 
				//	glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f));

				//auto selectedTile = map.RaycastTile(rayOrigin, rayDirection);

				//glm::vec3 worldCoords = mouseToWorldOnPlane(mpos.x, mpos.y, window::GetWidth(), window::GetHeight(), camera.GetViewMatrix(), glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f));
				////worldCoords.x -= 0.5f;
				////worldCoords.z -= 0.5f;

				//Print("x=" + std::to_string(worldCoords.x) + ";y=" + std::to_string(worldCoords.y) + ";z=" + std::to_string(worldCoords.z));
				//worldCoords = roundCoordinates(worldCoords);
				//Print("x=" + std::to_string(worldCoords.x) + ";y=" + std::to_string(worldCoords.y) + ";z=" + std::to_string(worldCoords.z));

				//gEditorCursor->SetPosition(worldCoords);

				//if (selectedTile.tile != NoTile)
				//{
				//	Print("x=" + std::to_string(selectedTile.x) + ";y=" + std::to_string(selectedTile.y) + ";z=" + std::to_string(selectedTile.z));
				//}
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