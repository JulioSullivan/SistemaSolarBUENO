#include "OPENGL.h"

OpenGL::OpenGL()
{
	m_hDeviceContext = NULL;
	m_hRenderContext = NULL;
	mlIdentityMat4(m_mProj);
}

OpenGL::~OpenGL()
{

}

bool OpenGL::Init(HWND hWnd)
{
	m_hDeviceContext = GetDC(hWnd);
	if(!m_hDeviceContext)
		return false;

	int attribList[] = 
	{
		WGL_SUPPORT_OPENGL_ARB, TRUE, 
		WGL_DRAW_TO_WINDOW_ARB, TRUE, 
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB, 
		WGL_DOUBLE_BUFFER_ARB, TRUE, 
		WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB, 
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, 
		WGL_COLOR_BITS_ARB, 24, 
		WGL_DEPTH_BITS_ARB, 24, 
		WGL_STENCIL_BITS_ARB, 8, 0
	};

	int iFormat;
	unsigned int uNumFormats;
	int iError = wglChoosePixelFormatARB(m_hDeviceContext, attribList, NULL, 1, &iFormat, &uNumFormats);
	if(iError != 1)
		return false;

	PIXELFORMATDESCRIPTOR pfDesc;
	iError = SetPixelFormat(m_hDeviceContext, iFormat, &pfDesc);
	if(iError != 1)
		return false;

	int contextAttribList[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3, 
		WGL_CONTEXT_MINOR_VERSION_ARB, 3, 0
	};

	m_hRenderContext = wglCreateContextAttribsARB(m_hDeviceContext, NULL, contextAttribList);
	if(!m_hRenderContext)
		return false;

	iError = wglMakeCurrent(m_hDeviceContext, m_hRenderContext);
	if(!iError)
		return false;

	iError = wglSwapIntervalEXT(1);
	if(iError != 1)
		return false;

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);

	return true;
}

void OpenGL::Shutdown(HWND hWnd)
{
	if(m_hRenderContext)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hRenderContext);
		m_hRenderContext = NULL;
	}
	if(m_hDeviceContext)
	{
		ReleaseDC(hWnd, m_hDeviceContext);
		m_hDeviceContext = NULL;
	}
}

bool OpenGL::InitExtensions(HWND hWnd)
{
	m_hDeviceContext = GetDC(hWnd);
	if(!m_hDeviceContext)
		return false;

	PIXELFORMATDESCRIPTOR pfDesc;
	int iError = SetPixelFormat(m_hDeviceContext, 1, &pfDesc);
	if(iError != 1)
		return false;

	m_hRenderContext = wglCreateContext(m_hDeviceContext);
	if(!m_hRenderContext)
		return false;

	iError = wglMakeCurrent(m_hDeviceContext, m_hRenderContext);
	if(iError != 1)
		return false;

	if(!LoadExtensionList())
		return false;

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(m_hRenderContext);
	m_hRenderContext = NULL;

	ReleaseDC(hWnd, m_hDeviceContext);
	m_hDeviceContext = NULL;

	return true;
}

void OpenGL::BeginScene(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL::EndScene()
{
	SwapBuffers(m_hDeviceContext);
}

void OpenGL::OnResize(unsigned short width, unsigned short height)
{
	glViewport(0, 0, width, height);

	float aspect = static_cast<float>(width) / static_cast<float>(height);
	mlPerspective(m_mProj, ML_PI*  0.25f, aspect, 0.01f, 1000.0f);
}

float* OpenGL::GetProjectionMatrix()
{
	return m_mProj;
}

bool OpenGL::LoadExtensionList()
{
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
	if(!wglChoosePixelFormatARB)
		return false;

	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if(!wglCreateContextAttribsARB)
		return false;

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
	if(!wglSwapIntervalEXT)
		return false;

	//MODEL
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	if(!glGenVertexArrays)
		return false;

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	if(!glBindVertexArray)
		return false;

	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	if(!glGenBuffers)
		return false;

	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	if(!glBindBuffer)
		return false;

	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	if(!glBufferData)
		return false;

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	if(!glEnableVertexAttribArray)
		return false;

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	if(!glDisableVertexAttribArray)
		return false;

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	if(!glVertexAttribPointer)
		return false;

	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	if(!glDeleteVertexArrays)
		return false;

	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	if(!glDeleteBuffers)
		return false;

	//SHADER
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	if(!glUseProgram)
		return false;

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	if(!glCreateShader)
		return false;

	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	if(!glDeleteShader)
		return false;

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	if(!glShaderSource)
		return false;

	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	if(!glCompileShader)
		return false;

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	if(!glCreateProgram)
		return false;

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	if(!glDeleteProgram)
		return false;
	
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	if(!glAttachShader)
		return false;
	
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	if(!glDetachShader)
		return false;

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	if(!glLinkProgram)
		return false;

	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	if(!glGetShaderiv)
		return false;

	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	if(!glGetShaderInfoLog)
		return false;

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	if(!glGetProgramiv)
		return false;

	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	if(!glGetProgramInfoLog)
		return false;

	//TEXTURE
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	if(!glActiveTexture)
		return false;

	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	if(!glGenerateMipmap)
		return false;

	//UNIFORMS
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	if(!glGetUniformLocation)
		return false;

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	if(!glUniform1i)
		return false;
	
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	if(!glUniformMatrix4fv)
		return false;

	return true;
}
