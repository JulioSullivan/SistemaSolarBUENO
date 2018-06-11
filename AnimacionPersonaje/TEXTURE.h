#ifndef TEXTURE_H
#define TEXTURE_H

#include "OPENGL.h"
#include <SOIL\SOIL.h>
#pragma comment(lib, "SOIL.lib")

class Texture
{
public:
	Texture();
	~Texture();

	bool Init(OpenGL*, const char*);
	void Shutdown();
	void SetTexture(OpenGL*);

private:
	unsigned int m_iTextureId;
};

#endif //TEXTURE_H