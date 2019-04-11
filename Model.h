/*************************************
	file:Mesh.h

	模型mesh
	封装顶点结构、顶点索引
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

	//LoadMesh函数，读取模型mesh文档
	//Mesh* LoadMesh(std::string filename)
	//创建三角形mesh
	void CreateTriangle3D();
	//创建方块mesh
	void CreateBox3D();

};



