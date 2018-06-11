#ifndef MLQUATERNION_H
#define MLQUATERNION_H

#include "MLVECTOR.h"

inline 
float mlDotVec4(float* a, float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

inline 
float mlLengthVec4(float* in)
{
	return sqrtf(mlDotVec4(in, in));
}

inline 
void mlNormalizeVec4(float* finalq, float* in)
{
	float invLength = 1.0f / mlLengthVec4(in);
	finalq[0] = in[0] * invLength;
	finalq[1] = in[1] * invLength;
	finalq[2] = in[2] * invLength;
	finalq[3] = in[3] * invLength;
}

inline
void mlMultiplyQuaternion_2(float* q, float* a, float* b)
{
	float3 va = {a[X], a[Y], a[Z]};
	float3 vb = {b[X], b[Y], b[Z]};
	float wa = a[W]; 
	float wb = b[W];  

	float3 va_X_vb;
	mlCrossVec3(va_X_vb, va, vb);
	q[0] = va[0] * wb  + vb[0] * wa + va_X_vb[0];
	q[1] = va[1] * wb  + vb[1] * wa + va_X_vb[1];
	q[2] = va[2] * wb  + vb[2] * wa + va_X_vb[2];
	q[3] = wa * wb - mlDotVec3(va, vb);
}

inline
void mlRotateVec3ByQuaternion(float* finalpos, float* q, float* pos)
{
	float4 pos_quat = {pos[X], pos[Y], pos[Z], 0.0f}; 
	float4 q_star = {-q[X], -q[Y], -q[Z], q[W]};

	float4 result, temp;
	mlMultiplyQuaternion_2(temp, pos_quat, q_star);
	mlMultiplyQuaternion_2(result, q, temp);

	finalpos[X] = result[X];
	finalpos[Y] = result[Y];
	finalpos[Z] = result[Z];
}

inline
void mlAxisAgle(float* q, float* n, float theta)
{
	float cos_ht = cosf(theta * 0.5f);
	float sin_ht = sinf(theta * 0.5f);

	q[0] = n[0] * sin_ht;
	q[1] = n[1] * sin_ht;
	q[2] = n[2] * sin_ht;
	q[3] = cos_ht; 
}

inline
void mlSlerp(float* q, float* a, float* b, float t)
{
	float cosOmega = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
	if(cosOmega < 0.0f)
	{
		a[0] = -a[0];
		a[1] = -a[1];
		a[2] = -a[2];
		a[3] = -a[3];
		cosOmega = -cosOmega;
	}
	
	float k0, k1;
	if(cosOmega > 0.9999f)
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		float sinOmega = sqrtf(1.0f - cosOmega * cosOmega);
		float omega = atan2f(sinOmega, cosOmega);
		float oneOverSinOmega = 1.0f / sinOmega;

		k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sinf(t * omega) * oneOverSinOmega;
	}

	q[0] = a[0] * k0 + b[0] * k1;
	q[1] = a[1] * k0 + b[1] * k1;
	q[2] = a[2] * k0 + b[2] * k1;
	q[3] = a[3] * k0 + b[3] * k1;
} 

inline
void mlQuaternionToMat4(float* M, float* Q)
{
	M[0] = 1.0f - 2.0f * (Q[Y] * Q[Y]  + Q[Z] * Q[Z]);
	M[1] = 2.0f * (Q[X] * Q[Y] + Q[W] * Q[Z]);
	M[2] = 2.0f * (Q[X] * Q[Z] - Q[W] * Q[Y]);
	M[3] = 0.0f;

	M[4] = 2.0f * (Q[X] * Q[Y] - Q[W] * Q[Z]);
	M[5] = 1.0f - 2.0f * (Q[X] * Q[X]  + Q[Z] * Q[Z]);
	M[6] = 2.0f * (Q[Y] * Q[Z] + Q[W] * Q[X]);
	M[7] = 0.0f;

	M[8] = 2.0f * (Q[X] * Q[Z] + Q[W] * Q[Y]);
	M[9] = 2.0f * (Q[Y] * Q[Z] - Q[W] * Q[X]);
	M[10] = 1.0f - 2.0f * (Q[X] * Q[X] + Q[Y] * Q[Y]);
	M[11] = 0.0f;

	M[12] = 0.0f;
	M[13] = 0.0f;
	M[14] = 0.0f;
	M[15] = 1.0f;
}

#endif //MLQUATERNION_H