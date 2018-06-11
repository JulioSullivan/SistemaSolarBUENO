#ifndef OPENGL_H
#define OPENGL_H

#include <Windows.h>
#include <GL\gl.H>
#include <GL\glext.h>
#include <GL\wglext.h>
#pragma comment(lib, "opengl32.lib")
#include "MLMATH\MLMATRIX.h"

class OpenGL
{
public:
	OpenGL();
	~OpenGL();

	bool Init(HWND);
	void Shutdown(HWND);
	bool InitExtensions(HWND);

	void BeginScene(float, float, float, float);
	void EndScene();

	void OnResize(unsigned short, unsigned short);
	float* GetProjectionMatrix();

private:
	bool LoadExtensionList();

private:
	mat4 m_mProj;
	HDC m_hDeviceContext;
	HGLRC m_hRenderContext;
	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

public:
	//MODEL
	PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
	PFNGLGENBUFFERSPROC glGenBuffers;
	PFNGLBINDBUFFERPROC glBindBuffer;
	PFNGLBUFFERDATAPROC glBufferData;
	PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
	PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
	PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
	PFNGLDELETEBUFFERSPROC glDeleteBuffers;

	//SHADER
	PFNGLUSEPROGRAMPROC glUseProgram;
	PFNGLCREATESHADERPROC glCreateShader;
	PFNGLDELETESHADERPROC glDeleteShader;
	PFNGLSHADERSOURCEPROC glShaderSource;
	PFNGLCOMPILESHADERPROC glCompileShader;
	PFNGLCREATEPROGRAMPROC glCreateProgram;
	PFNGLDELETEPROGRAMPROC glDeleteProgram;
	PFNGLATTACHSHADERPROC glAttachShader;
	PFNGLDETACHSHADERPROC glDetachShader;
	PFNGLLINKPROGRAMPROC glLinkProgram;
	PFNGLGETSHADERIVPROC glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
	PFNGLGETPROGRAMIVPROC glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

	//TEXTURE
	PFNGLACTIVETEXTUREPROC glActiveTexture;
	PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

	//UNIFORMS
	PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
	PFNGLUNIFORM1IPROC glUniform1i;
	PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
};

#endif //OPENGL_H