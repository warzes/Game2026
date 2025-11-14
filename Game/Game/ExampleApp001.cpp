#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3Advance.h"
#include "NanoRender.h"
#include "NanoScene.h"
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

out vec3 fragColor;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec3 fragTangent;

void main()
{
	gl_Position  = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	fragColor    = vertexColor;
	fragNormal   = mat3(transpose(inverse(modelMatrix))) * vertexNormal;
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

void main()
{
	outputColor = texture(diffuseTexture, fragTexCoord);
}
)";

	GLState state;
	GLuint shader;
	Camera camera;
	Model modelPlane;
	Model modelBox;
	Model modelSphere;
	Model modelTest;

	Texture2D texturePlane;
	Texture2D textureBox;
}
//=============================================================================
void ExampleApp001()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		state.depthState.enable = true;
		state.blendState.enable = true;
		state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

		shader = CreateShaderProgram(shaderCodeVertex, shaderCodeFragment);
		glUseProgram(shader);
		SetUniform(GetUniformLocation(shader, "diffuseTexture"), 0);

		camera.SetPosition(glm::vec3(0.0f, 0.5f, 4.5f));

		modelPlane.Create(GeometryGenerator::CreatePlane(100, 100, 100, 100));
		modelBox.Create(GeometryGenerator::CreateBox());
		modelSphere.Create(GeometryGenerator::CreateSphere());

		//modelTest.Load("data/models/simple_scene.glb");

		texturePlane = textures::GetDefaultDiffuse2D();
		textureBox = textures::LoadTexture2D("data/textures/temp.png", ColorSpace::sRGB, true);

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			if (input::IsKeyDown(RGFW_w)) 
				camera.ProcessKeyboard(CameraForward, engine::GetDeltaTime());
			if (input::IsKeyDown(RGFW_s)) 
				camera.ProcessKeyboard(CameraBackward, engine::GetDeltaTime());
			if (input::IsKeyDown(RGFW_a)) 
				camera.ProcessKeyboard(CameraLeft, engine::GetDeltaTime());
			if (input::IsKeyDown(RGFW_d)) 
				camera.ProcessKeyboard(CameraRight, engine::GetDeltaTime());

			if (input::IsMouseDown(RGFW_mouseRight))
			{
				input::SetCursorVisible(false);
				camera.ProcessMouseMovement(input::GetCursorOffset().x, input::GetCursorOffset().y);
			}
			else if (input::IsMouseReleased(RGFW_mouseRight))
			{
				input::SetCursorVisible(true);
			}

			BindState(state);
			glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 perspective = glm::perspective(glm::radians(60.0f), window::GetAspect(), 0.01f, 1000.0f);

			glActiveTexture(GL_TEXTURE0);


			glUseProgram(shader);
			SetUniform(GetUniformLocation(shader, "projectionMatrix"), perspective);
			SetUniform(GetUniformLocation(shader, "viewMatrix"), camera.GetViewMatrix());

			glBindTexture(GL_TEXTURE_2D, texturePlane.id);
			SetUniform(GetUniformLocation(shader, "modelMatrix"), glm::mat4(1.0f));
			modelPlane.tDraw();

			//SetUniform(GetUniformLocation(shader, "modelMatrix"), glm::scale(glm::mat4(1.0f), glm::vec3(0.6f)));
			//modelTest.Draw();

			glBindTexture(GL_TEXTURE_2D, texturePlane.id);
			SetUniform(GetUniformLocation(shader, "modelMatrix"), glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 1.0f, 0.0f)));
			modelSphere.tDraw();

			glBindTexture(GL_TEXTURE_2D, textureBox.id);
			SetUniform(GetUniformLocation(shader, "modelMatrix"), glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.5f, 0.0f)));
			modelBox.tDraw();

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