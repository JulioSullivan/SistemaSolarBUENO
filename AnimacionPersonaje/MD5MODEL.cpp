#include "MD5MODEL.h"

MD5Model::MD5Model()
{
	m_pJointArray = NULL;
	m_pMeshArray = NULL;
	m_pFinalMeshArray = NULL;

	m_pInvBindPose = NULL;
	m_pSkinnedMatrices = NULL;
	m_pAnim = NULL;

	m_pVertexArrayIds = NULL;
	m_pVertexBufferIds = NULL;
	m_pIndexBufferIds = NULL;
	m_pNumIndices = NULL;
	m_ppTextures = NULL;
}

MD5Model::~MD5Model()
{

}

bool MD5Model::Init(OpenGL* pOpenGL, const char* filename1, const char* filename2)
{
	if(!LoadMesh(filename1))
		return false;

	if(!PrepareMeshes())
		return false;

	if(!LoadAnim(filename2))
		return false;

	//opengl
	if(!InitBuffers(pOpenGL))
		return false;

	if(!InitTextures(pOpenGL))
		return false;

	ShutdownMesh();

	return true;
}

void MD5Model::Shutdown(OpenGL* pOpenGL)
{
	ShutdownAnim();
	ShutdownBuffers(pOpenGL);
	ShutdownTextures();
}

void MD5Model::Update(float dt)
{
	m_pAnim->Update(dt);
	mat4* pAnimatedMatrices = m_pAnim->GetAnimatedMatrices();

	for(int i = 0; i < m_iNumJoints; i++)
	{
		mlMultiplyMat4_2(m_pSkinnedMatrices[i], pAnimatedMatrices[i], m_pInvBindPose[i]);
	}
}

void MD5Model::Render(OpenGL* pOpenGL)
{
	for(int i = 0; i < m_iNumMeshes; i++)
	{
		pOpenGL->glBindVertexArray(m_pVertexArrayIds[i]);

		m_ppTextures[i]->SetTexture(pOpenGL);
		glDrawElements(GL_TRIANGLES, m_pNumIndices[i], GL_UNSIGNED_INT, 0);

		pOpenGL->glBindVertexArray(0);
	}
}

mat4* MD5Model::GetSkinnedMatrices(){ return m_pSkinnedMatrices; }

bool MD5Model::LoadMesh(const char* filename)
{
	std::ifstream file(filename);
	if(!file.is_open())
		return false;

	int initpos = file.tellg();
	file.seekg(0, std::ios::end);
	int filelength = file.tellg();
	file.seekg(initpos);

	int imesh = 0;
	std::string param, junk;
	file >> param;

	while (!file.eof())
	{
		if(param == "MD5Version")
		{
			file >> m_iMD5Version;
			if(!m_iMD5Version)
				return false;

			printf("MD5Version: %i\n", m_iMD5Version);
		}
		else if(param == "numJoints")
		{
			file >> m_iNumJoints;
			m_pJointArray = new md5_joint_t[m_iNumJoints];
			if(!m_pJointArray)
				return false;

			m_pInvBindPose = new mat4[m_iNumJoints];
			if(!m_pInvBindPose)
				return false;

			m_pSkinnedMatrices = new mat4[m_iNumJoints];
			if(!m_pSkinnedMatrices)
				return false;

			printf("numJoints: %i\n", m_iNumJoints);
		}
		else if(param == "numMeshes")
		{
			file >> m_iNumMeshes;
			m_pMeshArray = new md5_mesh_t[m_iNumMeshes];
			if(!m_pMeshArray)
				return false;

			m_pFinalMeshArray = new ogl_mesh_t[m_iNumMeshes];
			if(!m_pFinalMeshArray)
				return false;

			printf("numMeshes: %i\n", m_iNumMeshes);
		}
		else if(param == "joints")
		{
			file >> junk;
			for(int i = 0; i < m_iNumJoints; i++)
			{
				md5_joint_t joint;
				file >> joint.name >> joint.parentId >> junk;
				file >> joint.position[0] >> joint.position[1] >> joint.position[2];
				file >> junk >> junk;
				file >> joint.orientation[0] >> joint.orientation[1] >> joint.orientation[2];
				file >> junk;
				file.ignore(filelength, '\n');

				RemoveQuotes(joint.name);
				ComputeQuaternionW(joint.orientation);
				m_pJointArray[i] = joint;
			}
			file >> junk;

			BuildBindPose();
		}
		else if(param == "mesh")
		{
			md5_mesh_t mesh;
			file >> junk;
			file >> param;

			while (param != "}")
			{
				if(param == "shader")
				{
					file >> mesh.shader;
					RemoveQuotes(mesh.shader);

					std::string texture = mesh.shader + '\n';
					printf(texture.c_str());
				}
				else if(param == "numverts")
				{
					file >> mesh.numverts;
					mesh.vertexArray = new md5_vertex_t[mesh.numverts];
					if(!mesh.vertexArray)
						return false;

					for(int i = 0; i < mesh.numverts; i++)
					{
						md5_vertex_t vert;
						file >> junk >> junk >> junk; //vert vertIdx (
						file >> vert.texCoord[0] >> vert.texCoord[1] >> junk; // )
						file >> vert.startWeight >> vert.numWeights;

						mesh.vertexArray[i] = vert; 
					}
				}
				else if(param == "numtris")
				{
					file >> mesh.numtris;
					mesh.triangleArray = new md5_triangle_t[mesh.numtris];
					if(!mesh.triangleArray)
						return false;

					for(int i = 0; i < mesh.numtris; i++)
					{
						md5_triangle_t tri;
						file >> junk >> junk; //tri triIdx
						file >> tri.indices[0] >> tri.indices[1] >> tri.indices[2];

						mesh.triangleArray[i] = tri;
					}
				}
				else if(param == "numweights")
				{
					file >> mesh.numweights;
					mesh.weightArray = new md5_weight_t[mesh.numweights];
					if(!mesh.weightArray)
						return false;

					for(int i = 0; i < mesh.numweights; i++)
					{
						md5_weight_t weight;
						file >> junk >> junk; //weight weightIdx
						file >> weight.jointId >> weight.bias >> junk; // (
						file >> weight.position[0] >> weight.position[1] >> weight.position[2];
						file >> junk; // )

						mesh.weightArray[i] = weight;
					}
				}

				file >> param;
			}

			m_pMeshArray[imesh] = mesh;
			imesh++;
		}

		file >> param;
	}

	return true;
}

void MD5Model::ShutdownMesh()
{
	if(m_pJointArray)
	{
		delete[] m_pJointArray;
		m_pJointArray = NULL;
	}
	if(m_pMeshArray)
	{
		for(int i = 0; i < m_iNumMeshes; i++)
		{
			delete[] m_pMeshArray[i].vertexArray;
			delete[] m_pMeshArray[i].triangleArray;
			delete[] m_pMeshArray[i].weightArray;
			m_pMeshArray[i].vertexArray = NULL;
			m_pMeshArray[i].triangleArray = NULL;
			m_pMeshArray[i].weightArray = NULL;
		}
		delete[] m_pMeshArray;
		m_pMeshArray = NULL;
	}
	if(m_pFinalMeshArray)
	{
		for(int i = 0; i < m_iNumMeshes; i++)
		{
///			delete[] m_pFinalMeshArray[i].vertexArray;
//			delete[] m_pFinalMeshArray[i].indexArray;
	//		m_pFinalMeshArray[i].vertexArray = NULL;
	//		m_pFinalMeshArray[i].indexArray = NULL;
		}
		delete[] m_pFinalMeshArray;
		m_pFinalMeshArray = NULL;
	}
}

void MD5Model::BuildBindPose()
{
	mat4 rotation, rotationInv, translationInv, finalmatrix;
	for(int i = 0; i < m_iNumJoints; i++)
	{
		md5_joint_t& joint = m_pJointArray[i];
		mlQuaternionToMat4(rotation, joint.orientation);
		mlTransposeMat4(rotationInv, rotation);
		mlTranslationMat4(translationInv, -joint.position[0], - joint.position[1], -joint.position[2]);

		mlMultiplyMat4_2(finalmatrix, rotationInv, translationInv);
		memcpy(m_pInvBindPose[i], finalmatrix, sizeof(mat4));
	}
}

void MD5Model::ComputeVertexPositionsFromWeights(md5_mesh_t* mesh, ogl_mesh_t* finalmesh)
{
	for(int i = 0; i < mesh->numverts; i++)
	{
		md5_vertex_t& vert = mesh->vertexArray[i];
		ogl_vertex_t& finalvert = finalmesh->vertexArray[i];
		memset(finalvert.position, 0, sizeof(float3));
		memcpy(finalvert.texCoord, vert.texCoord, sizeof(float2));
		memset(finalvert.indices, 0, sizeof(float4));
		memset(finalvert.weights, 0, sizeof(float4));

		for(int j = 0; j < vert.numWeights; j++)
		{
			md5_weight_t& weight = mesh->weightArray[vert.startWeight + j];
			md5_joint_t& joint = m_pJointArray[weight.jointId];
			float3 rotpos;
			mlRotateVec3ByQuaternion(rotpos, joint.orientation, weight.position);

			finalvert.position[0] += (joint.position[0] + rotpos[0]) * weight.bias;
			finalvert.position[1] += (joint.position[1] + rotpos[1]) * weight.bias;
			finalvert.position[2] += (joint.position[2] + rotpos[2]) * weight.bias;
			finalvert.indices[j] = weight.jointId;
			finalvert.weights[j] = weight.bias;
		}
	}
}

bool MD5Model::PrepareMeshes()
{
	for(int i = 0; i < m_iNumMeshes; i++)
	{
		md5_mesh_t& mesh = m_pMeshArray[i];
		ogl_mesh_t& finalmesh = m_pFinalMeshArray[i];

		finalmesh.texture = mesh.shader;
		finalmesh.numVertices = mesh.numverts;
		finalmesh.numIndices = mesh.numtris * 3;
		
		finalmesh.vertexArray = new ogl_vertex_t[finalmesh.numVertices];
		if(!finalmesh.vertexArray)
			return false;

		finalmesh.indexArray = new unsigned int[finalmesh.numIndices];
		if(!finalmesh.indexArray)
			return false;

		ComputeVertexPositionsFromWeights(&mesh, &finalmesh);

		for(int j = 0; j < mesh.numtris; j++)
		{
			finalmesh.indexArray[j * 3 + 0] = mesh.triangleArray[j].indices[0];
			finalmesh.indexArray[j * 3 + 1] = mesh.triangleArray[j].indices[1];
			finalmesh.indexArray[j * 3 + 2] = mesh.triangleArray[j].indices[2];
		}
	}

	return true;
}

bool MD5Model::LoadAnim(const char* filename)
{
	m_pAnim = new MD5Anim();
	if(!m_pAnim)
		return false;

	if(!m_pAnim->Init(filename))
		return false;

	return true;
}

void MD5Model::ShutdownAnim()
{
	if(m_pAnim)
	{
		m_pAnim->Shutdown();
		delete m_pAnim;
		m_pAnim = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
bool MD5Model::InitBuffers(OpenGL* pOpenGL)
{
	m_pVertexArrayIds = new unsigned int[m_iNumMeshes];
	if(!m_pVertexArrayIds)
		return false;

	m_pVertexBufferIds = new unsigned int[m_iNumMeshes];
	if(!m_pVertexBufferIds)
		return false;

	m_pIndexBufferIds = new unsigned int[m_iNumMeshes];
	if(!m_pIndexBufferIds)
		return false;

	m_pNumIndices = new unsigned int[m_iNumMeshes];
	if(!m_pNumIndices)
		return false;

	pOpenGL->glGenVertexArrays(m_iNumMeshes, m_pVertexArrayIds);
	pOpenGL->glGenBuffers(m_iNumMeshes, m_pVertexBufferIds);
	pOpenGL->glGenBuffers(m_iNumMeshes, m_pIndexBufferIds);

	for(int i = 0; i < m_iNumMeshes; i++)
	{
		m_pNumIndices[i] = m_pFinalMeshArray[i].numIndices;
		pOpenGL->glBindVertexArray(m_pVertexArrayIds[i]);
	
		pOpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_pVertexBufferIds[i]);
		pOpenGL->glBufferData(GL_ARRAY_BUFFER, sizeof(ogl_vertex_t) * m_pFinalMeshArray[i].numVertices, 
			                  m_pFinalMeshArray[i].vertexArray, GL_STATIC_DRAW);

		size_t textureOffset = sizeof(float3);
		size_t indicesOffset = sizeof(float2) + textureOffset;
		size_t weightsOffset = sizeof(float4) + indicesOffset;
		pOpenGL->glEnableVertexAttribArray(0);
		pOpenGL->glEnableVertexAttribArray(1);
		pOpenGL->glEnableVertexAttribArray(2);
		pOpenGL->glEnableVertexAttribArray(3);
		pOpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(ogl_vertex_t), 0);
		pOpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(ogl_vertex_t), (void*)textureOffset);
		pOpenGL->glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(ogl_vertex_t), (void*)indicesOffset);
		pOpenGL->glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(ogl_vertex_t), (void*)weightsOffset);

		pOpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pIndexBufferIds[i]);
		pOpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_pFinalMeshArray[i].numIndices, 
			                  m_pFinalMeshArray[i].indexArray, GL_STATIC_DRAW);

		pOpenGL->glBindVertexArray(0);
	}

	return true;
}

void MD5Model::ShutdownBuffers(OpenGL* pOpenGL)
{
	pOpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	pOpenGL->glDeleteBuffers(m_iNumMeshes, m_pIndexBufferIds);

	pOpenGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
	pOpenGL->glDeleteBuffers(m_iNumMeshes, m_pVertexBufferIds);

	pOpenGL->glBindVertexArray(0);
	pOpenGL->glDeleteVertexArrays(m_iNumMeshes, m_pVertexArrayIds);

	if(m_pVertexArrayIds)
	{
		delete[] m_pVertexArrayIds;
		m_pVertexArrayIds = NULL;
	}
	if(m_pIndexBufferIds)
	{
		delete[] m_pIndexBufferIds;
		m_pIndexBufferIds = NULL;
	}
	if(m_pVertexBufferIds)
	{
		delete[] m_pVertexBufferIds;
		m_pVertexBufferIds = NULL;
	}
	if(m_pNumIndices)
	{
		delete[] m_pNumIndices;
		m_pNumIndices = NULL;
	}
}

bool MD5Model::InitTextures(OpenGL* pOpenGL)
{
	m_ppTextures = new Texture*[m_iNumMeshes];
	if(!m_ppTextures)
		return false;

	for(int i = 0; i < m_iNumMeshes; i++)
	{
		m_ppTextures[i] = new Texture();
		if(!m_ppTextures[i])
			return false;

		if(!m_ppTextures[i]->Init(pOpenGL, m_pFinalMeshArray[i].texture.c_str()))
			return false;
	}

	return true;
}

void MD5Model::ShutdownTextures()
{
	if(m_ppTextures)
	{
		for(int i = 0; i < m_iNumMeshes; i++)
		{
			m_ppTextures[i]->Shutdown();
			delete m_ppTextures[i];
			m_ppTextures[i] = NULL;
		}
		delete[] m_ppTextures;
		m_ppTextures = NULL;
	}
}