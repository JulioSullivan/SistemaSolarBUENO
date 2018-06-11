#ifndef SHADER_H
#define SHADER_H

#include "OPENGL.h"
#include <fstream>
#include "MLMATH\MLMATRIX.h"
#define MAX_BONES 33

struct ShaderMatrices
{
	mat4 mWorld;
	mat4 mView;
	mat4 mProj;
};

class Shader
{
public:
	Shader();
	~Shader();

	bool Init(OpenGL*, char*, char*);
	void Shutdown(OpenGL*);
	void SetShader(OpenGL*);
	bool SetShaderParameters(OpenGL*, ShaderMatrices*, mat4*);

private:
	const char* ReadShaderFromFile(char*);
	void OutputShaderError(OpenGL*, unsigned int);
	void OutputLinkerError(OpenGL*, unsigned int);

private:
	bool InitShaders(OpenGL*, char*, char*);
	void ShutdownShaders(OpenGL*);

private:
	unsigned int m_iProgramId;
	unsigned int m_iVertexShaderId;
	unsigned int m_iFragmentShaderId;
};

#endif //SHADER_H