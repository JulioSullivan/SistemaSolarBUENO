#ifndef MD5MODEL_H
#define MD5MODEL_H

#include "OPENGL.h"
#include "TEXTURE.h"
#include "MLMATH\MLMATRIX.h"
#include "MLMATH\MLQUATERNION.h"
#include "MD5ANIM.h"
#include "HELPERS.h"
#include <string>
#include <fstream>

struct ogl_vertex_t
{
	float3 position;
	float2 texCoord;
	float4 indices;
	float4 weights;
};

struct ogl_mesh_t
{
	std::string texture;
	ogl_vertex_t* vertexArray;
	unsigned int* indexArray;
	unsigned int numVertices;
	unsigned int numIndices;
};

struct md5_vertex_t
{
	float2 texCoord;
	int startWeight;
	int numWeights;
};

struct md5_triangle_t
{
	int indices[3];
};

struct md5_weight_t
{
	int jointId;
	float bias;
	float3 position;
};

struct md5_joint_t
{
	std::string name;
	int parentId;
	float3 position;
	float4 orientation;
};

struct md5_mesh_t
{
	std::string shader;
	md5_vertex_t* vertexArray;
	md5_triangle_t* triangleArray;
	md5_weight_t* weightArray;
	int numverts;
	int numtris;
	int numweights;
};

class MD5Model
{
public:
	MD5Model();
	~MD5Model();

	bool Init(OpenGL*, const char*, const char*);
	void Shutdown(OpenGL*);
	void Update(float);
	void Render(OpenGL*);
	mat4* GetSkinnedMatrices();

private:
	bool LoadMesh(const char*);
	void ShutdownMesh();
	void BuildBindPose();
	void ComputeVertexPositionsFromWeights(md5_mesh_t*, ogl_mesh_t*);
	bool PrepareMeshes();

	bool LoadAnim(const char*);
	void ShutdownAnim();

private:
	bool InitBuffers(OpenGL*);
	void ShutdownBuffers(OpenGL*);
	bool InitTextures(OpenGL*);
	void ShutdownTextures();

private:
	int m_iMD5Version;
	int m_iNumJoints;
	int m_iNumMeshes;

	md5_joint_t* m_pJointArray;
	md5_mesh_t* m_pMeshArray;
	ogl_mesh_t* m_pFinalMeshArray;

	mat4* m_pInvBindPose;
	mat4* m_pSkinnedMatrices;
	MD5Anim* m_pAnim;

	//graphics
	unsigned int* m_pVertexArrayIds;
	unsigned int* m_pVertexBufferIds;
	unsigned int* m_pIndexBufferIds;
	unsigned int* m_pNumIndices;

	Texture** m_ppTextures;
};


#endif //MD5MODEL_H