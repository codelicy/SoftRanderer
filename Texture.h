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

	//º”‘ÿbmp
	bool LoadBmp(const std::string& filename);
	//Œ∆¿Ì”≥…‰
	UINT SampColor(Vec4& tc);
};

