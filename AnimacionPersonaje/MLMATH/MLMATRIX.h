#ifndef MLMATRIX_H
#define MLMATRIX_H

#include "MLVECTOR.h"

typedef float mat2[4];
typedef float mat3[9];
typedef float mat4[16];
#define IX(i, j) (i + (j * 4))

inline 
void mlSetMat4(float* out, float* in)
{
	memcpy(out, in, sizeof(float) * 16);
}

inline 
void mlSetMat4(float* out, 
               float m00, float m01, float m02, float m03,
			   float m10, float m11, float m12, float m13, 
			   float m20, float m21, float m22, float m23, 
			   float m30, float m31, float m32, float m33)
{
	out[0] = m00;
	out[1] = m10;
	out[2] = m20;
	out[3] = m30;

	out[4] = m01;
	out[5] = m11;
	out[6] = m21;
	out[7] = m31;

	out[8] = m02;
	out[9] = m12; 
	out[10] = m22;
	out[11] = m32;

	out[12] = m03;
	out[13] = m13;
	out[14] = m23;
	out[15] = m33;
}

inline 
void mlIdentityMat4(float* out)
{
	memset(out, 0, sizeof(float) * 16);
	out[0] = 1.0f;
	out[5] = 1.0f;
	out[10] = 1.0f;
	out[15] = 1.0f;
}

inline 
void mlTransposeMat4(float* O, float* M)
{
	O[0] = M[0];
	O[1] = M[4];
	O[2] = M[8];
	O[3] = M[12];

	O[4] = M[1];
	O[5] = M[5];
	O[6] = M[9];
	O[7] = M[13];

	O[8] = M[2];
	O[9] = M[6];
	O[10] = M[10];
	O[11] = M[14];

	O[12] = M[3];
	O[13] = M[7];
	O[14] = M[11];
	O[15] = M[15];
}

inline 
void mlMultiplyMat4_2(float* O, float* A, float* B)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			O[i + j * 4] = 0.0f;
			for(int k = 0; k < 4; k++)
			{
				O[i + j * 4] += A[i + k * 4] * B[k + j * 4];
			}
		}
	}
}

inline 
void mlTranslationMat4(float* M, float x, float y, float z)
{
	mlIdentityMat4(M);
	M[12] = x;
	M[13] = y;
	M[14] = z;
}

inline 
void mlTranslationMat4(float* M, float* pos)
{
	mlIdentityMat4(M);
	M[12] = pos[0];
	M[13] = pos[1];
	M[14] = pos[2];
}

inline 
void mlRotateXYZMat4(float* O, float x, float y, float z)
{
	float Cx = cosf(x);
	float Cy = cosf(y);
	float Cz = cosf(z);
	float Sx = sinf(x);
	float Sy = sinf(y);
	float Sz = sinf(z);

	mlIdentityMat4(O);

	O[IX(0, 0)] = Cy * Cz;
	O[IX(0, 1)] = -Cx * Sx + Sx * Sy * Cz;
	O[IX(0, 2)] = Sx * Sz + Cx * Sy * Cz;
	
	O[IX(1, 0)] = Cy * Sz;
	O[IX(1, 1)] = Cx * Cz + Sx * Sy * Sz;
	O[IX(1, 2)] = -Sx * Cz + Cx * Sy * Sz;

	O[IX(2, 0)] = -Sy;
	O[IX(2, 1)] = Sx * Cy;
	O[IX(2, 2)] = Cx * Cy;
}

inline
void mlLookAtMat4(float* out, float* eye, float* target, float* up)
{
	float3 xAxis, yAxis, zAxis;
	mlSubVec3_2(zAxis, eye, target);
	mlNormalizeVec3(zAxis, zAxis);

	mlCrossVec3(xAxis, up, zAxis);
	mlNormalizeVec3(xAxis, xAxis);

	mlCrossVec3(yAxis, zAxis, xAxis);
	mlNormalizeVec3(yAxis, yAxis);

	float view03 = -mlDotVec3(xAxis, eye);
	float view13 = -mlDotVec3(yAxis, eye);
	float view23 = -mlDotVec3(zAxis, eye);

	mlSetMat4(out, 
		      xAxis[0], xAxis[1], xAxis[2], view03, 
			  yAxis[0], yAxis[1], yAxis[2], view13, 
			  zAxis[0], zAxis[1], zAxis[2], view23, 
			  0.0f, 0.0f, 0.0f, 1.0f);
}

inline 
void mlPerspective(float* out, float fovy, float aspect, float zNear, float zFar)
{
	float y = 1.0f / tanf(0.5f * fovy);
	float x = y / aspect;

	mlIdentityMat4(out);
	out[0] = x;
	out[5] = y;
	out[10] = (zNear + zFar) / (zNear - zFar);
	out[14] = (2.0f * zNear * zFar) / (zNear - zFar);
	out[11] = -1.0f;
}

#endif //MLMATRIX_H