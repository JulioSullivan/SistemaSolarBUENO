#include "TEXTURE.h"

Texture::Texture()
{

}

Texture::~Texture()
{

}

bool Texture::Init(OpenGL* pOpenGL, const char* texName)
{
	pOpenGL->glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &m_iTextureId);
	glBindTexture(GL_TEXTURE_2D, m_iTextureId);

	int iWidth, iHeight;
	unsigned char* pPixels = SOIL_load_image(texName, &iWidth, &iHeight, 0, SOIL_LOAD_RGBA);
	if(!pPixels){
		return false;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pPixels);
	SOIL_free_image_data(pPixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	pOpenGL->glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

void Texture::Shutdown()
{
	glDeleteTextures(1, &m_iTextureId);
}

void Texture::SetTexture(OpenGL* pOpenGL)
{
	pOpenGL->glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_iTextureId);
}