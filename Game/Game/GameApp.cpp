#include "stdafx.h"
#include "GameApp.h"
#include "CameraOld.h"
#include "ModelTemp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3.h"
#include "NanoIO.h"
// https://github.com/Keypekss/OpenGLTechDemo
// https://github.com/vcoda/aggregated-graphics-samples
// https://github.com/Mikepicker/opengl-sandbox
// https://github.com/cforfang/opengl-shadowmapping
// https://github.com/uobirek/opengl-3D-renderer
// https://github.com/pulkitjuneja/GlEn
//=============================================================================
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
//=============================================================================
void GameAppRun()
{
	try
	{
		if (!engine::Init(SCR_WIDTH, SCR_HEIGHT, "Game"))
			return;

		GLState state;
		state.depthState.enable = true;
		state.blendState.enable = true;
		state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			// render
			BindState(state);
			glClearColor(0.1f, 0.1f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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