#pragma once

#include "GameModel.h"

class MapChunk final
{
public:
	bool Init();
	void Close();

	GameModel* GetModel() noexcept { return &m_model; }
private:
	void generateBufferMap();
	GameModel m_model;
};