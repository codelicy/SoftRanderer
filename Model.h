/*************************************
	file:Mesh.h

	ģ��mesh
	��װ����ṹ����������
	author:Lcy
	date:2019-3-27
**************************************/
#pragma once
#include <vector>
#include <windows.h>
#include "VectorL.h"

struct Vertex
{
	Vec4 pos, color,uv;
	Vertex(const Vec4& p,const Vec4& c):pos(p),color(c){}
	Vertex(const Vec4&p,const Vec4& c,const Vec4& tc):pos(p),color(c),uv(tc){}
	Vertex(float px,float py,float pz,const Vec4& c,float pw=1):pos(px,py,pz,pw),color(c) {}
	Vertex() {}
};
struct Mesh
{
	std::vector<Vertex> VertexBuffer;
	std::vector<UINT> IndexBuffer;
	//std::vector<Mesh*> ModelList;

};
struct Model
{
	std::vector<Mesh*> ModelList;

	//LoadMesh��������ȡģ��mesh�ĵ�
	//Mesh* LoadMesh(std::string filename)
	//����������mesh
	void CreateTriangle3D();
	//��������mesh
	void CreateBox3D();

};



