/********************************
	file:DeviceL.h
	��Ⱦ�豸
	����2D���ơ��ӿڱ任���򵥲ü�����Ȳ��ԡ���դ�������ݲ�ֵ
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

	int m_width,m_height;   // ���ڿ���
	RanderMode m_randermode;  //��Ⱦģʽ1�߿�2��ɫ3����
	UINT* m_pFrameBuffer;
	//HDC screen_hdc;  
	float* m_zBuffer;  //z ����
	Matrix4 mWorld, mView, mProj,mvp,mScale;//�ռ�ת������
	float mTheta;//������
	float mPhi;
	float mRadius; 
	float m_Zoom ;//scale
	UINT m_Optimized ; //��դ���Ż�����
	Model* m_pModel;//ģ��
	TextureL* m_pTexture;
	POINT mLastMousePos;

public:
	Device_L() {}
	~Device_L() {};
	//��ʼ��
	void initialize(int w,int h,void* framebuf);
	void Destroy();//����
	void ClearBuffer(const Vec4& color);

	//����Ⱦ����
	void update();//״̬����
	void SetCameraPos(const Vec4 &look, const Vec4 &at);//�������
	void SetFrustum(float fov,float ratio,float n,float f);//������׶��
	void SetMvp();//���ñ任����
	void DrawPrimitive(Mesh* pMesh);//ͼԪ����

	bool Clip(Vec4& pos);//��βü��ռ�򵥲ü�
	void NDC2Screen(Vertex& vertexout);//͸�ӳ���&�ӿ�ӳ��
	//͸��У����ֵ
	void Interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, Vertex& pixelSamp, const Vec4& Vw);
	//void LoadBmp(char* filename);
	
	//��դ������
	//void DrawLine1(int x1, int y1, int x2, int y2, const Vec4& color);
	void DrawPoint(int x, int y, const UINT color,const float z);
	void DrawLine(int x0, int y0, int x1, int y1, const Vec4& color);
	void DrawLineX(Vec4& p0, Vec4& p1,const Vec4& color);
	bool TriangleCheck(const Vertex& v0, const Vertex& v1, const Vertex& v2, Vertex& p, Vec4& Vw);
	inline bool BackFaceCulling(const Vec4 &p0, const Vec4 &p1, const Vec4 &p2) {
		return (p0.Dot((p2 - p0).Cross(p1 - p0)) > 0);}
	void FillTriangleC(Vertex& v0,Vertex&v1,Vertex&v2);//����ɫģʽ
	void FillTriangleT(Vertex& v0, Vertex&v1, Vertex&v2);//����ģʽ
	void Rasterize(Vertex& v0,Vertex& v1,Vertex& v2);
};