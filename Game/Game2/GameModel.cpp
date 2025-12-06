#include "stdafx.h"
#include "GameModel.h"
//=============================================================================
bool GameModel::LoadModel(const std::string& fileName)
{
	if (!m_data.model.Load(fileName, ModelMaterialType::BlinnPhong))
		return false;

	return true;
}
//=============================================================================
void GameModel::SetupParameters(ProgramHandle program)
{
	switch (m_data.faceVisibility)
	{
	case FaceVisibility::Front:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		break;
	case FaceVisibility::Back:
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		break;
	case FaceVisibility::Double:
		glDisable(GL_CULL_FACE);
		break;
	default:
		break;
	}

	if (m_data.transparency)
	{
		glEnable(GL_BLEND);
		if (m_data.blendingType == BlendingType::Normal)
		{
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		}
		else if (m_data.blendingType == BlendingType::Additive)
		{
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO); // TODO: Change to additive blending
		}
	}
	else
	{
		glDisable(GL_BLEND);
	}

	SetUniform(GetUniformLocation(program, "TileU"), m_data.tileU);
	SetUniform(GetUniformLocation(program, "TileV"), m_data.tileV);
	SetUniform(GetUniformLocation(program, "AlphaTest"), m_data.alphaTest);

	SetUniform(GetUniformLocation(program, "DiffuseColor"), m_data.diffuseColor);
	SetUniform(GetUniformLocation(program, "Specularity"), m_data.specularity);
	SetUniform(GetUniformLocation(program, "Shininess"), m_data.shininess);
	SetUniform(GetUniformLocation(program, "Opacity"), m_data.opacity);
	SetUniform(GetUniformLocation(program, "ReceiveShadows"), m_data.receiveShadows);
}
//=============================================================================