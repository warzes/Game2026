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
//=============================================================================
namespace
{
	const char* shaderCodeVertex = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

out vec3 fragColor;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragTangent;

void main()
{
	gl_Position  = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	fragColor    = vertexColor;
	fragNormal   = mat3(normalMatrix) * vertexNormal;
	fragTexCoord = vertexTexCoord;
	fragTangent  = vertexTangent;
}
)";

	const char* shaderCodeFragment = R"(
#version 330 core

in vec3 fragColor;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec3 fragTangent;

uniform sampler2D diffuseTexture;

layout(location = 0) out vec4 outputColor;

const float alphaTestThreshold = 0.1f;

void main()
{
	vec4 albedo = texture(diffuseTexture, fragTexCoord);
	if (albedo.a <= alphaTestThreshold) discard;

	outputColor = albedo;
}
)";

	GLuint shader;

	Scene scene;

	Model modelPlane;
	Model modelBox;
	Model modelSphere;
	Model modelTest;
	Model modelTest2;

	Texture2D texturePlane;
	Texture2D textureBox;
}
//=============================================================================
void ExampleApp002()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		scene.Init();
		scene.GetCurrentCamera().SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));
		scene.SetGridAxis(22);

		shader = CreateShaderProgram(shaderCodeVertex, shaderCodeFragment);
		glUseProgram(shader);
		SetUniform(GetUniformLocation(shader, "diffuseTexture"), 0);

		modelPlane.Create(GeometryGenerator::CreatePlane(100, 100, 100, 100));
		modelBox.Create(GeometryGenerator::CreateBox());
		modelSphere.Create(GeometryGenerator::CreateSphere());
		modelTest.Load("data/models/tree.glb");
		modelTest2.Load("data/models/cottage/cottage_obj.obj");
		modelTest2.SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)));

		texturePlane = textures::GetDefaultDiffuse2D();
		textureBox = textures::LoadTexture2D("data/textures/temp.png", ColorSpace::sRGB, true);

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			auto& camera = scene.GetCurrentCamera();
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

			scene.Draw();
			
			glUseProgram(shader);
			SetUniform(GetUniformLocation(shader, "projectionMatrix"), scene.GetPerspective());
			SetUniform(GetUniformLocation(shader, "viewMatrix"), scene.GetCurrentCamera().GetViewMatrix());

			BindTexture2D(0, texturePlane.id);
			//modelPlane.Draw();

			//modelTest.SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)));
			modelTest.Draw(GetUniformLocation(shader, "modelMatrix"), GetUniformLocation(shader, "normalMatrix"));

			modelTest2.Draw(
				glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, -3.0f)), 
				GetUniformLocation(shader, "modelMatrix"), 
				GetUniformLocation(shader, "normalMatrix"));

			BindTexture2D(0, texturePlane.id);
			SetUniform(GetUniformLocation(shader, "modelMatrix"), glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 1.0f, 0.0f)));
			modelSphere.Draw();

			BindTexture2D(0, textureBox.id);
			SetUniform(GetUniformLocation(shader, "modelMatrix"), glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.5f, 0.0f)));
			modelBox.Draw();

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