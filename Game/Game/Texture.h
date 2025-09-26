#pragma once

class TextureO2
{
public:
	TextureO2();
	TextureO2(const char* fileLoc);

	bool LoadTexture();
	bool LoadTextureA();

	void UseTexture();
	void ClearTexture();

	~TextureO2();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};
