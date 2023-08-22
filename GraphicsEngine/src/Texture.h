#pragma once
#include <iostream>
#include "GL/glew.h"


class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(unsigned int slot = 0);
	void UnBind();

	int GetWidth();
	int GetHeight();

private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width;
	int m_Height;
	int m_BPP;
};
