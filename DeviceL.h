/********************************
	file:DeviceL.h
	渲染设备
	包含2D绘制、视口变换、简单裁剪、深度测试、光栅化、数据插值
	author: Lcy
	date:2019-3-27
*********************************/
#pragma once
#include "math.h"
#include "Model.h"
#include "Texture.h"
#include <memory>   // std::unique_ptr support
//struct Device_L;
//extern Device_L *device;

enum RanderMode
{
	Rander_State_WireFrame = 0,
	Rander_State_Color,
	Rander_State_Texture
};

struct Device_L
{

	int m_width,m_height;   // 窗口宽、高
	RanderMode m_randermode;  //渲染模式1线框2颜色3纹理
	UINT* m_pFrameBuffer;
	//HDC screen_hdc;  
	float* m_zBuffer;  //z 缓存
	Matrix4 mWorld, mView, mProj,mvp,mScale;//空间转换矩阵
	float mTheta;//鼠标控制
	float mPhi;
	float mRadius; 
	float m_Zoom ;//scale
	UINT m_Optimized ; //光栅化优化开关
	Model* m_pModel;//模型
	TextureL* m_pTexture;
	POINT mLastMousePos;

public:
	Device_L() {}
	~Device_L() {};
	//初始化
	void initialize(int w,int h,void* framebuf);
	void Destroy();//销毁
	void ClearBuffer(const Vec4& color);

	//主渲染函数
	void update();//状态更新
	void SetCameraPos(const Vec4 &look, const Vec4 &at);//设置相机
	void SetFrustum(float fov,float ratio,float n,float f);//设置视锥体
	void SetMvp();//设置变换矩阵
	void DrawPrimitive(Mesh* pMesh);//图元绘制

	bool Clip(Vec4& pos);//奇次裁剪空间简单裁剪
	void NDC2Screen(Vertex& vertexout);//透视除法&视口映射
	//透视校正插值
	void Interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, Vertex& pixelSamp, const Vec4& Vw);
	//void LoadBmp(char* filename);
	
	//光栅化函数
	//void DrawLine1(int x1, int y1, int x2, int y2, const Vec4& color);
	void DrawPoint(int x, int y, const UINT color,const float z);
	void DrawLine(int x0, int y0, int x1, int y1, const Vec4& color);
	void DrawLineX(Vec4& p0, Vec4& p1,const Vec4& color);
	bool TriangleCheck(const Vertex& v0, const Vertex& v1, const Vertex& v2, Vertex& p, Vec4& Vw);
	inline bool BackFaceCulling(const Vec4 &p0, const Vec4 &p1, const Vec4 &p2) {
		return (p0.Dot((p2 - p0).Cross(p1 - p0)) > 0);}
	void FillTriangleC(Vertex& v0,Vertex&v1,Vertex&v2);//顶点色模式
	void FillTriangleT(Vertex& v0, Vertex&v1, Vertex&v2);//纹理模式
	void Rasterize(Vertex& v0,Vertex& v1,Vertex& v2);
};