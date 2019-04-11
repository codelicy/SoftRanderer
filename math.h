/**********************************************
	file:math.h
	��ֵ������任����ɫ��edgeFunction�Ⱥ���///
    author��Lcy
	date: 2019-3-27
**********************************************/

#pragma once
//#include <vector>
#include <windows.h>
#include <algorithm>
#include "Matrix.h"


namespace Math_L
{
	extern const float Pi ;
	//MIN
	inline float Min3(const float &a,const float &b,const float &c)
	{
		return std::min<>(a, std::min<>(b, c));
	}
	//Max
	inline float Max3(const float &a, const float &b, const float &c)
	{
		return std::max<>(a, std::max<>(b, c));
	}
	//Clamp
	template<typename T>
	T Clamp(const T &x, const T &min, const T &max)
	{
		return x < min ? min : (x > max ? max : x);
	}
	// Edge function
	inline float EdgeFunc(const Vec4& a, const Vec4& b, const Vec4& p)
	{
		return ((b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x));
	}

	//degree to radians
	inline float ConvertToRadians(float fDegrees) { return fDegrees * (Pi / 180.0f); }

	//ƽ�ƾ���
	Matrix4 MatrixSetTranslate(float x, float y, float z);

	//���ž���
	Matrix4 MatrixSetScale(float x, float y, float z);

	//��X����ת����
	Matrix4 MatrixSetRotationX(float angle);

	//��Y����ת����
	Matrix4 MatrixSetRotationY(float angle);

	//��Z����ת����
	Matrix4 MatrixSetRotationZ(float angle);

	//��ȡ�������������ӽǾ���,view matrix
	Matrix4 MatrixSetLookAt(const Vec4& look, const Vec4& at, const Vec4& up);

	//��ȡͶӰ����  D3DXMatrixPerspectiveFovLH
	Matrix4 MatrixPerspectiveFovLH(float fovY, float aspectRatio, float nearZ, float farZ);

	//��ɫת��ΪUINT, argb order
	UINT ColorToUINT(const Vec4& color);

}
