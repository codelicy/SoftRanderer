#include "math.h"

const float Math_L::Pi = 3.1415926f;

//平移矩阵
Matrix4 Math_L::MatrixSetTranslate(float x, float y, float z)
{
	return Matrix4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1);
}
//缩放矩阵
Matrix4 Math_L::MatrixSetScale(float x, float y, float z)
{
	return Matrix4(
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1);
}
//绕X轴旋转矩阵
Matrix4 Math_L::MatrixSetRotationX(float angle)
{
	return Matrix4(
		1, 0, 0, 0,
		0, cos(angle), sin(angle), 0,
		0, -sin(angle), cos(angle), 0,
		0, 0, 0, 1);
}
//绕Y轴旋转矩阵
Matrix4 Math_L::MatrixSetRotationY(float angle)
{
	return Matrix4(
		cos(angle), 0, -sin(angle), 0,
		0, 1, 0, 0,
		sin(angle), 0, cos(angle), 0,
		0, 0, 0, 1);
}
//绕Z轴旋转矩阵
Matrix4 Math_L::MatrixSetRotationZ(float angle)
{
	return Matrix4(
		cos(angle), sin(angle), 0, 0,
		-sin(angle), cos(angle), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}
//获取摄像机矩阵或者视角矩阵,view matrix
Matrix4 Math_L::MatrixSetLookAt(const Vec4& look, const Vec4& at, const Vec4& up)
{
	Vec4 zaxis = (at - look).Normalize();
	Vec4 xaxis = up.Cross(zaxis).Normalize();
	Vec4 yaxis = zaxis.Cross(xaxis);
	return Matrix4(
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		-xaxis.Dot(look), -yaxis.Dot(look), -zaxis.Dot(look), 1);
}
/***********************************************
获取投影矩阵  D3DXMatrixPerspectiveFovLH
	fovY ----vertical field of view  angle in radians
	aspectRatio = width / heigth
	nearZ -----distance to near plane
	farZ -------distance to far plane
************************************************/
Matrix4 Math_L::MatrixPerspectiveFovLH(float fovY, float aspectRatio, float nearZ, float farZ)
{
	Matrix4 mat;
	mat.SetZero();
	float fac = 1.0f / tan(fovY*0.5f);
	mat._11 = fac / aspectRatio;
	mat._22 = fac;
	mat._33 = farZ / (farZ - nearZ);
	mat._34 = 1.0f;
	mat._43 = -nearZ * farZ / (farZ - nearZ);
	return mat;
}
//颜色转化为UINT, argb order
UINT Math_L::ColorToUINT(const Vec4& color)
{
	int r = int(255 * color.x);
	int g = int(255 * color.y);
	int b = int(255 * color.z);
	r = Clamp(r, 0, 255);
	g = Clamp(g, 0, 255);
	b = Clamp(b, 0, 255);
	return UINT((r << 16) | (g << 8) | (b));
}
