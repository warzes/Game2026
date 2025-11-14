#include "stdafx.h"
#include "GameApp.h"
#include "NanoLog.h"
#include "NanoWindow.h"
#include "NanoEngine.h"
#include "NanoOpenGL3Advance.h"
#include "NanoRender.h"
#include "NanoScene.h"

/*
 https://github.com/muleax/slope
 tinyfx
 https://github.com/nfginola/dx11-tech

*/




/*
idea:
- https://hardboiledcrab.itch.io/tiled-mediaval-building  - example tile house building
- карту делать из таких маленьких одноцветных кубиков - https://atapataco.itch.io/cube-tilemap-desert
		или https://wizp.itch.io/3d-tileset
*/

/*
отделить assimp от model. возможность формировать модель из даты ассимпа
вывод квада на экран (в том числе в какую-то область) - чтобы тестить рендеры в текстуру)
*/

//=============================================================================
namespace
{
}
//=============================================================================
void GameApp()
{
	try
	{
		if (!engine::Init(1600, 900, "Game"))
			return;

		while (!engine::ShouldClose())
		{
			engine::BeginFrame();

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