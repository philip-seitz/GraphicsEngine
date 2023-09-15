#pragma once
#include <iostream>
#include "GL/glew.h"


class Texture
{
public:
	Texture(const std::string& path, const int& slot, const unsigned short& wrap_mode);
	~Texture();

	void Bind();
	void UnBind();

	int GetWidth();
	int GetHeight();

private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	unsigned char* m_LocalBuffer;
	short m_Slot;
	int m_Width;
	int m_Height;
	int m_BPP;
};
