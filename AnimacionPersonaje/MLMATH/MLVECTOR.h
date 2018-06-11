#ifndef MLVECTOR_H
#define MLVECTOR_H

#include <math.h>
#include <iostream>

typedef float float2[2];
typedef float float3[3];
typedef float float4[4];
#define ML_PI 3.141592
#define X 0
#define Y 1
#define Z 2
#define W 3

inline 
void mlSetVec3(float* out, float* in)
{
	memcpy(out, in, sizeof(float) * 3);
}

inline 
void mlSetVec3(float* out, float x, float y, float z)
{
	out[0] = x;  
	out[1] = y;  
	out[2] = z;
}

inline 
void mlAddVec3_2(float* out, float* a, float* b)
{
	for(int i = 0; i < 3; i++)
		out[i] = a[i] + b[i];
}

inline 
void mlSubVec3_2(float* out, float* a, float* b)
{
	out[0] = a[0] - b[0];
	out[1] = a[1] - b[1];
	out[2] = a[2] - b[2];
}

inline 
float mlDotVec3(float* a, float* b)
{ 
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline 
float mlLengthVec3(float* a)
{
	return sqrtf(mlDotVec3(a, a));
}

inline 
void mlCrossVec3(float* out, float* a, float* b)
{
	out[X] = a[Y] * b[Z] - a[Z] * b[Y];
	out[Y] = a[Z] * b[X] - a[X] * b[Z];
	out[Z] = a[X] * b[Y] - a[Y] * b[X];
}

inline 
void mlNormalizeVec3(float* out, float* a)
{
	float invLength = 1.0f / mlLengthVec3(a);

	out[0] = a[0] * invLength;
	out[1] = a[1] * invLength;
	out[2] = a[2] * invLength;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
inline 
void mlSetVec4(float4 out, float x, float y, float z, float w)
{
	out[0] = x;
	out[1] = y;
	out[2] = z;
	out[3] = w;
}

#endif //MLVECTOR_H