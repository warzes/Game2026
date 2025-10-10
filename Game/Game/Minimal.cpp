#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3.h"
#include "NanoIO.h"
//=============================================================================
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;
//=============================================================================
void MinimalAppRun()
{
	try
	{
		if (!engine::Init(SCR_WIDTH, SCR_HEIGHT, "Game"))
			return;

		GLState state;
		state.depthState.enable = true;
		state.blendState.enable = true;
		state.blendState.srcAlpha = BlendFactor::OneMinusSrcAlpha;

		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

			// render
			BindState(state);
			glClearColor(0.5f, 0.1f, 0.8f, 1.0f);
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