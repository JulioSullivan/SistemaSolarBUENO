#include "SHADER.h"

Shader::Shader()
{

}

Shader::~Shader()
{

}

bool Shader::Init(OpenGL* pOpenGL, char* sVSName, char* sFSName)
{
	if(!InitShaders(pOpenGL, sVSName, sFSName))
		return false;

	return true;
}

void Shader::Shutdown(OpenGL* pOpenGL)
{
	ShutdownShaders(pOpenGL);
}

void Shader::SetShader(OpenGL* pOpenGL)
{
	pOpenGL->glUseProgram(m_iProgramId);
}

bool Shader::SetShaderParameters(OpenGL* pOpenGL, ShaderMatrices* pShaderMatrices, mat4* pSkinnedMatrices)
{
	int iLocation = pOpenGL->glGetUniformLocation(m_iProgramId, "sampler");
	if(iLocation == -1)
		return false;

	pOpenGL->glUniform1i(iLocation, 0);

	//world
	iLocation = pOpenGL->glGetUniformLocation(m_iProgramId, "mWorld");
	if(iLocation == -1)
		return false;

	pOpenGL->glUniformMatrix4fv(iLocation, 1, false, pShaderMatrices->mWorld);

	//view
	iLocation = pOpenGL->glGetUniformLocation(m_iProgramId, "mView");
	if(iLocation == -1)
		return false;

	pOpenGL->glUniformMatrix4fv(iLocation, 1, false, pShaderMatrices->mView);

	//proj
	iLocation = pOpenGL->glGetUniformLocation(m_iProgramId, "mProj");
	if(iLocation == -1)
		return false;

	pOpenGL->glUniformMatrix4fv(iLocation, 1, false, pShaderMatrices->mProj);

	//skinnig
	iLocation = pOpenGL->glGetUniformLocation(m_iProgramId, "mSkinned");
	if(iLocation == -1)
		return false;

	pOpenGL->glUniformMatrix4fv(iLocation, MAX_BONES, false, pSkinnedMatrices[0]);

	return true;
}

const char* Shader::ReadShaderFromFile(char* sFileName)
{
	FILE* file = fopen(sFileName, "rb");
	if(!file)
		return NULL;

	int iError = fseek(file, 0, SEEK_END);
	if(iError != 0)
		return NULL;

	long lFileSize = ftell(file);
	rewind(file);

	char* pBuffer = new char[lFileSize + 1];
	if(!pBuffer)
		return NULL;

	long lCount = fread(pBuffer, sizeof(char), lFileSize, file);
	if(lCount != lFileSize)
		return NULL;

	pBuffer[lFileSize] = '\0';

	iError = fclose(file);
	if(iError != 0)
		return NULL;

	return const_cast<const char*>(pBuffer);
}

void Shader::OutputShaderError(OpenGL* pOpenGL, unsigned int iShaderId)
{
	int iSize;
	pOpenGL->glGetShaderiv(iShaderId, GL_INFO_LOG_LENGTH, &iSize);

	char* pBuffer = new char[iSize];
	if(!pBuffer)
		return;

	pOpenGL->glGetShaderInfoLog(iShaderId, iSize, 0, pBuffer);
	OutputDebugStringA(pBuffer);

	delete[] pBuffer;
	pBuffer = NULL;
}

void Shader::OutputLinkerError(OpenGL* pOpenGL, unsigned int iProgramId)
{
	int iBufferSize;
	pOpenGL->glGetProgramiv(iProgramId, GL_INFO_LOG_LENGTH, &iBufferSize);

	char* pBuffer = new char[iBufferSize];
	if(!pBuffer)
		return;

	pOpenGL->glGetProgramInfoLog(iProgramId, iBufferSize, 0, pBuffer);
	OutputDebugStringA(pBuffer);

	delete[] pBuffer;
	pBuffer = NULL;
}

bool Shader::InitShaders(OpenGL* pOpenGL, char* sVSName, char* sFSName)
{
	const char* pVertexShader = ReadShaderFromFile(sVSName);
	if(!pVertexShader)
		return false;

	const char* pFragmentShader = ReadShaderFromFile(sFSName);
	if(!pFragmentShader)
		return false;

	m_iVertexShaderId = pOpenGL->glCreateShader(GL_VERTEX_SHADER);
	m_iFragmentShaderId = pOpenGL->glCreateShader(GL_FRAGMENT_SHADER);

	pOpenGL->glShaderSource(m_iVertexShaderId, 1, &pVertexShader,  0);
	pOpenGL->glShaderSource(m_iFragmentShaderId, 1, &pFragmentShader, 0);

	pOpenGL->glCompileShader(m_iVertexShaderId);
	pOpenGL->glCompileShader(m_iFragmentShaderId);

	int iStatus;
	pOpenGL->glGetShaderiv(m_iVertexShaderId, GL_COMPILE_STATUS, &iStatus);
	if(iStatus != 1){
		OutputShaderError(pOpenGL, m_iVertexShaderId);
		return false;
	}

	pOpenGL->glGetShaderiv(m_iFragmentShaderId, GL_COMPILE_STATUS, &iStatus);
	if(iStatus != 1){
		OutputShaderError(pOpenGL, m_iFragmentShaderId);
		return false;
	}

	m_iProgramId = pOpenGL->glCreateProgram();
	pOpenGL->glAttachShader(m_iProgramId, m_iVertexShaderId);
	pOpenGL->glAttachShader(m_iProgramId, m_iFragmentShaderId);
	pOpenGL->glLinkProgram(m_iProgramId);

	pOpenGL->glGetProgramiv(m_iProgramId, GL_LINK_STATUS, &iStatus);
	if(iStatus != 1){
		OutputLinkerError(pOpenGL, m_iProgramId);
		return false;
	}

	return true;
}

void Shader::ShutdownShaders(OpenGL* pOpenGL)
{
	pOpenGL->glUseProgram(0);
	pOpenGL->glDetachShader(m_iProgramId, m_iVertexShaderId);
	pOpenGL->glDetachShader(m_iProgramId, m_iFragmentShaderId);

	pOpenGL->glDeleteShader(m_iVertexShaderId);
	pOpenGL->glDeleteShader(m_iFragmentShaderId);
	pOpenGL->glDeleteProgram(m_iProgramId);
}