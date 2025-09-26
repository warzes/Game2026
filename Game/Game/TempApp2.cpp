#include "stdafx.h"
#include "GameApp.h"
#include "CameraOld.h"
#include "ModelTemp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoOpenGL3.h"
#include "NanoIO.h"
#include "Scene.h"
//=============================================================================
namespace
{
	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 900;
	// timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
}
//=============================================================================
void TempAppRun2()
{
	try
	{
		if (!window::Init(SCR_WIDTH, SCR_HEIGHT, "Game"))
			return;

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		//GLState state;
		//state.depthState.enable = true;
		//state.blendState.enable = true;
		//state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

		Scene scene = Scene();

		scene.Init();
		
		while (!window::WindowShouldClose())
		{
			float currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			if (glfwGetKey(window::handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window::handle, true);
			
			//BindState(state);
			//glClearColor(0.3f, 0.1f, 0.8f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			scene.Render();

			window::Swap();
		}
	}
	catch (const std::exception& exc)
	{
		puts(exc.what());
	}
	window::Close();
}
//=============================================================================