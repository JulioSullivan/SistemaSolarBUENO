#include "GRAPHICS.h"

Graphics::Graphics()
{
	m_pOpenGL = NULL;
	m_pModel = NULL;
	m_pShader = NULL;
}

Graphics::~Graphics()
{

}

bool Graphics::Init(OpenGL* pOpenGL)
{
	m_pOpenGL = pOpenGL;

	m_pShader = new Shader();
	if(!m_pShader)
		return false;

	if(!m_pShader->Init(pOpenGL, "VertexShader.glsl", "FragmentShader.glsl"))
		return false;

	//MODEL
	m_pModel = new MD5Model();
	if(!m_pModel)
		return false;

	if(!m_pModel->Init(pOpenGL, 
		               "Assets\\Models\\boblampclean.md5mesh", 
					   "Assets\\Animations\\boblampclean.md5anim")){
		return false;
	}

	//shader matrices
	m_pShaderMatrices = new ShaderMatrices();
	if(!m_pShaderMatrices)
		return false;

	mlIdentityMat4(m_pShaderMatrices->mWorld);

	float3 eye, target, up;
	mlSetVec3(eye, 0.0f, 25.0f, 100.0f);
	mlSetVec3(target, 0.0f, 25.0f, 0.0f);
	mlSetVec3(up, 0.0f, 1.0f, 0.0f);

	mlLookAtMat4(m_pShaderMatrices->mView, eye, target, up);

	return true;
}

void Graphics::Shutdown()
{
	if(m_pShaderMatrices)
	{
		delete m_pShaderMatrices;
		m_pShaderMatrices = NULL;
	}
	if(m_pShader)
	{
		m_pShader->Shutdown(m_pOpenGL);
		delete m_pShader;
		m_pShader = NULL;
	}
	if(m_pModel)
	{
		m_pModel->Shutdown(m_pOpenGL);
		delete m_pModel;
		m_pModel = NULL;
	}
	if(m_pOpenGL)
		m_pOpenGL = NULL;
}

bool Graphics::Frame()
{
	float dt = (1.0f / 60.0f);

	mlRotateXYZMat4(m_pShaderMatrices->mWorld, -ML_PI * 0.5f, 0.0f, 0.0f);
	mlSetMat4(m_pShaderMatrices->mProj, m_pOpenGL->GetProjectionMatrix());

	m_pModel->Update(dt);

	if(!Render())
		return false;

	return true;
}

bool Graphics::Render()
{
	m_pOpenGL->BeginScene(1.0f, 1.0f, 1.0f, 1.0f);

	m_pShader->SetShader(m_pOpenGL);
	m_pShader->SetShaderParameters(m_pOpenGL, m_pShaderMatrices, m_pModel->GetSkinnedMatrices());
	m_pModel->Render(m_pOpenGL);

	m_pOpenGL->EndScene();

	return true;
}
