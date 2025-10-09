#pragma once

#include "Scene.h"

class Region final
{
public:
	void Init();
	void Close();

	Entity model;
};

extern Region gRegion;