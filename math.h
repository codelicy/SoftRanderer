/**********************************************
	file:math.h
	插值、矩阵变换、颜色、edgeFunction等函数///
    author：Lcy
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

	//平移矩阵
	Matrix4 MatrixSetTranslate(float x, float y, float z);

	//缩放矩阵
	Matrix4 MatrixSetScale(float x, float y, float z);

	//绕X轴旋转矩阵
	Matrix4 MatrixSetRotationX(float angle);

	//绕Y轴旋转矩阵
	Matrix4 MatrixSetRotationY(float angle);

	//绕Z轴旋转矩阵
	Matrix4 MatrixSetRotationZ(float angle);

	//获取摄像机矩阵或者视角矩阵,view matrix
	Matrix4 MatrixSetLookAt(const Vec4& look, const Vec4& at, const Vec4& up);

	//获取投影矩阵  D3DXMatrixPerspectiveFovLH
	Matrix4 MatrixPerspectiveFovLH(float fovY, float aspectRatio, float nearZ, float farZ);

	//颜色转化为UINT, argb order
	UINT ColorToUINT(const Vec4& color);

}
