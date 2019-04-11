#pragma once
#include <vector>
#include <fstream>
#include <string>
#include "VectorL.h"
#include "math.h"

struct TextureL
{
	int width;
	int height;
	UINT* data;

	//����BMP
	bool LoadBmp(const std::string& filename);
	//����ӳ��
	UINT SampColor(Vec4& tc);
};

