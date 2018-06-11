#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "OPENGL.h"
#include "SHADER.h"
#include "MD5MODEL.h"

class Graphics
{
public:
	Graphics();
	~Graphics();

	bool Init(OpenGL*);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	OpenGL* m_pOpenGL;
	MD5Model* m_pModel;
	Shader* m_pShader;

	ShaderMatrices* m_pShaderMatrices;
};

#endif //GRAPHICS_H