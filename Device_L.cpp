/*file:Device_L.cpp*/

#include"DeviceL.h"

void Device_L::Destroy()
{
	if (m_pModel)
		delete m_pModel;
	if (m_pFrameBuffer)
		delete m_pFrameBuffer;
	if (m_zBuffer)
		delete[] m_zBuffer;
}
void Device_L::initialize(int w,int h,void* framebuf)
{
	m_width = w;
	m_height = h;

	m_pFrameBuffer = static_cast<UINT*>(framebuf);

	m_zBuffer = new float[m_height*m_width];
	for (int i = 0; i < m_height*m_width; i++)
		m_zBuffer[i] = 1000.f;

	m_randermode = Rander_State_WireFrame;
	mTheta = 1.5f * Math_L::Pi;
	mPhi  = 0.4f * Math_L::Pi;
	mRadius = 5.0f;
	m_Optimized = 1;//默认开启光栅化优化
	m_pModel=new Model();
	m_pTexture = new TextureL();

	mWorld.Identity();//世界空间矩阵
	mView.Identity();//视椎体矩阵
	mProj.Identity();//投影矩阵
	//mRotY.Identity();//Y轴自旋矩阵
	mScale.Identity();//缩放矩阵

	//初始相机位置由WM_PAINT消息device->update()函数根据鼠标位置提供,这里不需要提供
	//SetCameraPos({ 0.f,1.f,-5.f,1.f }, { 0.f,0.f,0.f,1.f });//设置初始相机位置,相机矩阵mView
	m_Zoom = 0.5f;//缩放系数初始值
	mScale = Math_L::MatrixSetScale(m_Zoom, m_Zoom,m_Zoom);
	//设置投影矩阵mProj
	SetFrustum(Math_L::Pi / 4, (float)m_width / m_height, 1.f, 1000.f);//mProj
}
//清空屏幕
void Device_L::ClearBuffer(const Vec4& color)
{
	for (int x = 0; x < m_width; ++x)
		for (int y = 0; y < m_height; ++y)
		{
			m_pFrameBuffer[m_width*y+x] = Math_L::ColorToUINT(color);
			m_zBuffer[m_width*y+x] = 1000.f;// Initialize depth buffer to far clipping plane
		}
}
//画点
void Device_L::DrawPoint(int x, int y, const UINT color,const float z)
{
		if (x >= 0 && x < m_width&&y >= 0 && y < m_height)
		{
			m_pFrameBuffer[m_width*y + x] = color;
			m_zBuffer[m_width*y + x] = z; // write z buffer;
		}
}

void Device_L::DrawLineX(Vec4& p0, Vec4& p1,const Vec4& color)
{
	int x0 = (int)std::floor(p0.x), x1 = (int)std::floor(p1.x), y0 = (int)std::floor(p0.y), y1 = (int)std::floor(p1.y);
	DrawLine(x0, y0, x1, y1,color);
}
// Bresenham直线算法
// ref:http://www.cnblogs.com/gamesky/archive/2012/08/21/2648623.html
void Device_L::DrawLine(int x0, int y0, int x1, int y1, const Vec4& color)
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int ux = (dx > 0) ? 1 : -1;//x方向步进值
	int uy = (dy > 0) ? 1 : -1;//y方向步进值
	dx = abs(x1 - x0); dy = abs(y1 - y0);

	if (dx > dy) // 斜率k<1
	{
		int e = 2 * dy - dx; //e为累计误差
		int y = y0;
		for (int x = x0; x !=x1; x+=ux)
		{
			DrawPoint(x, y, Math_L::ColorToUINT(color),0);
			e += dy;
			if (e >= 0)
			{
				y +=uy;
				e -= dx;
			}
		}//end for
	}
	else //斜率k>1
	{
		//if (y0 >y1) { std::swap(x0, x1); std::swap(y0, y1); }
		int e = 2 * dx - dy;
		int x = x0;
		for (int y = y0; y != y1; y+=uy)
		{
			DrawPoint(x, y, Math_L::ColorToUINT(color),0);
			e += dx;
			if (e >= 0)
			{
				x+=ux;
				e -= dy;
			}
		}
	}//else
}//end DrawLine

//裁剪,区间-w<x<w,-w<y<w,0<z<w,顶点处于奇次裁剪空间
//使用了D3D透视矩阵，变换后，Z值保存在W中，z区间[0,1]
//openGL下CVV空间，Z值在[-1,1]之间
bool Device_L::Clip(Vec4& pos)
{
	//float w = pos.w;
	if (pos.x >= -pos.w && pos.x <= pos.w &&
		pos.y >= -pos.w && pos.y <= pos.w &&
		pos.z >= 0.f && pos.z <= pos.w)
	{
		return true;
	}
	return false;
}

//透视除法/w,从裁剪空间转至NDC设备规范化空间
//NDC到视口变换,映射X,Y坐标区间[-1，1]至屏幕[m_width,m_height],UV不变
void Device_L::NDC2Screen(Vertex& vertexout)
{
	float reciprocalw = 1.f / vertexout.pos.w; //透视变换后，z值保留在w中
	vertexout.pos.x = (vertexout.pos.x*reciprocalw + 1)*0.5f*m_width;//映射至屏幕
	vertexout.pos.y = (vertexout.pos.y *reciprocalw+ 1)*0.5f*m_height;
	vertexout.pos.z = vertexout.pos.w;//z
	vertexout.pos.w = reciprocalw;  //保存1/z在W中,插值时使用
}

//设置相机
void Device_L::SetCameraPos(const Vec4 &look, const Vec4 &at)
{
	mView = Math_L::MatrixSetLookAt(look, at, { 0,1,0,0 });
}
//设置投影视椎体
void Device_L::SetFrustum(float fov, float ratio, float n, float f)
{
	mProj = Math_L::MatrixPerspectiveFovLH(fov, ratio, n, f);
}
void Device_L::SetMvp()
{
	//注意矩阵乘法顺序，不支持交换律
	mvp = mWorld * mView*mProj*mScale;
}
//状态变更
void Device_L::update()
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	Vec4 eyePos(x, y, z, 1.f);
	Vec4 at(0.f, 0.f, 0.f, 1.f);
	Vec4 up(0.f, 1.f, 0.f, 0.f);

	SetCameraPos(eyePos, at);//mView
}
//绘制图元
void Device_L::DrawPrimitive(Mesh* m_pMesh)
{
	Vertex v0, v1, v2;

	for (UINT i = 0; i < m_pMesh->IndexBuffer.size(); i += 3)
	{
		v0 = m_pMesh->VertexBuffer[m_pMesh->IndexBuffer[i]];
		v1 = m_pMesh->VertexBuffer[m_pMesh->IndexBuffer[i + 1]];
		v2 = m_pMesh->VertexBuffer[m_pMesh->IndexBuffer[i + 2]];
		//对顶点进行MVP投影变换，投影后，W中保存着原Z值
		v0.pos = mvp.MultVec(v0.pos);
		v1.pos = mvp.MultVec(v1.pos);
		v2.pos = mvp.MultVec(v2.pos);
		//判断是否通过裁剪
		if (Clip(v0.pos) == false || Clip(v1.pos) == false || Clip(v2.pos) == false)
		{
			continue;
		}
		if(m_randermode==Rander_State_Color)//线框模式不用背面消隐
		{	//背面消隐
			if (BackFaceCulling(v0.pos, v1.pos, v2.pos))
				continue;
		}
		//透视除法转至NDC空间，视口映射，准备光栅化
		NDC2Screen(v0);
		NDC2Screen(v1);
		NDC2Screen(v2);
		//光栅化
		Rasterize(v0, v1, v2);
	}
}
//光栅化
void Device_L::Rasterize(Vertex& v0, Vertex& v1, Vertex& v2)
{
	//线框模式
	if (m_randermode == Rander_State_WireFrame)
	{
		DrawLineX(v0.pos, v1.pos, { 1.f,1.f,1.f,1.f });
		DrawLineX(v0.pos, v2.pos, { 1.f,1.f,1.f,1.f });
		DrawLineX(v1.pos, v2.pos, { 1.f,1.f,1.f,1.f });
	}
	//顶点色模式
	else if (m_randermode == Rander_State_Color)
		FillTriangleC(v0, v1, v2);
	//纹理模式
	else
		FillTriangleT(v0,v1,v2);
}

//顶点色模式
void Device_L::FillTriangleC(Vertex& v0, Vertex& v1, Vertex& v2)
{
	//get the triangle bounding box in float
	float xmax = Math_L::Max3(v0.pos.x, v1.pos.x, v2.pos.x);
	float ymax = Math_L::Max3(v0.pos.y, v1.pos.y, v2.pos.y);
	float xmin = Math_L::Min3(v0.pos.x, v1.pos.x, v2.pos.x);
	float ymin = Math_L::Min3(v0.pos.y, v1.pos.y, v2.pos.y);
	//to int
	int x0 = std::max<>(0, (int)std::floor(xmin));
	int y0 = std::max<>(0, (int)std::floor(ymin));
	int x1 = std::min<>(m_width - 1, (int)std::floor(xmax));
	int y1 = std::min<>(m_height - 1, (int)std::floor(ymax));
	//weight stored in vector
	Vec4 Vweight = { 0,0,0,Math_L::EdgeFunc(v0.pos, v1.pos, v2.pos) };

	//start inside-outside test
	if (m_Optimized) //edgefunction光栅化伪优化算法
	{
		//关于edgefunction优化算法
		//ref:http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-practical-implementation
		//这里只进行了部分优化，循环中还是带有2个乘法，FPS比优化前略有提升

		float w0base = (v2.pos.x - v1.pos.x)*(y0 + 0.5f - v1.pos.y) - (v2.pos.y - v1.pos.y)*(x0 + 0.5f - v1.pos.x);
		float w1base = (v0.pos.x - v2.pos.x)*(y0 + 0.5f - v2.pos.y) - (v0.pos.y - v2.pos.y)*(x0 + 0.5f - v2.pos.x);
		float w2base = (v1.pos.x - v0.pos.x)*(y0 + 0.5f - v0.pos.y) - (v1.pos.y - v0.pos.y)*(x0 + 0.5f - v0.pos.x);
		float  w0_stepX = -(v2.pos.y - v1.pos.y);
		float  w0_stepY = (v2.pos.x - v1.pos.x);
		float  w1_stepX = -(v0.pos.y - v2.pos.y);
		float  w1_stepY = (v0.pos.x - v2.pos.x);
		float  w2_stepX = -(v1.pos.y - v0.pos.y);
		float  w2_stepY = (v1.pos.x - v0.pos.x);

		//start 
		for (int x = x0, sx = 0; x <= x1; x++, sx++)
			for (int y = y0, sy = 0; y <= y1; y++, sy++)
			{
				Vertex SamplePixel = { {x + 0.5f ,y + 0.5f ,0.f,0.f},{} }; //采样点初值,颜色通过顶点插值获取

				float w0 = w0base + w0_stepX * sx + w0_stepY * sy;
				float w1 = w1base + w1_stepX * sx + w1_stepY * sy;
				float w2 = w2base + w2_stepX * sx + w2_stepY * sy;

				if (!(w0 >= 0 && w1 >= 0 && w2 >= 0))//skip  pixel outside
					continue;

				Vweight.x = w0 * v0.pos.w / Vweight.w;//pos.w=1/z,查看NDC2Screen
				Vweight.y = w1 * v1.pos.w / Vweight.w;
				Vweight.z = w2 * v2.pos.w / Vweight.w;//存储 lambda/z 至Vweight

				//透视校正插值,包括XY坐标,颜色
				Interpolate(v0, v1, v2, SamplePixel, Vweight);

				DrawPoint(x, y, Math_L::ColorToUINT(SamplePixel.color), SamplePixel.pos.z);
			}
	}//end optimized
	else//未优化,edgefunction初始算法
	{
		//start inside outside test or coverage test using original edge function
		for (int x = x0; x <= x1; x++)
			for (int y = y0; y <= y1; y++)
			{
				Vertex SamplePixel = { {x + 0.5f, y + 0.5f, 0.f,0.f},{} };//采样点初值,颜色通过顶点插值获取
				// SamplePixel is outside the triangle
				if (!TriangleCheck(v0, v1, v2, SamplePixel, Vweight))
					continue;//now vector Vweight has the coefficient "lambda"

				//透视校正插值perspective correct interpolation
				Interpolate(v0, v1, v2, SamplePixel, Vweight);

				/*		
				// Z-Test 
				//m_zBuffer[y*m_width+x] = SamplePixel.pos.z;
				if (SamplePixel.pos.z >= m_zBuffer[y*m_width + x])
					continue;
				*/
				DrawPoint(x, y, Math_L::ColorToUINT(SamplePixel.color), SamplePixel.pos.z);
			}//end for
	}//end else

}//end if

//纹理模式
void Device_L::FillTriangleT(Vertex& v0, Vertex& v1, Vertex& v2)
{
	//get the triangle bounding box in float
	float xmax = Math_L::Max3(v0.pos.x, v1.pos.x, v2.pos.x);
	float ymax = Math_L::Max3(v0.pos.y, v1.pos.y, v2.pos.y);
	float xmin = Math_L::Min3(v0.pos.x, v1.pos.x, v2.pos.x);
	float ymin = Math_L::Min3(v0.pos.y, v1.pos.y, v2.pos.y);
	//to int
	int x0 = std::max<>(0, (int)std::floor(xmin));
	int y0 = std::max<>(0, (int)std::floor(ymin));
	int x1 = std::min<>(m_width - 1, (int)std::floor(xmax));
	int y1 = std::min<>(m_height - 1, (int)std::floor(ymax));
	//weight stored in vector
	Vec4 Vweight = { 0,0,0,Math_L::EdgeFunc(v0.pos, v1.pos, v2.pos) };
	float w0base = (v2.pos.x - v1.pos.x)*(y0 + 0.5f - v1.pos.y) - (v2.pos.y - v1.pos.y)*(x0 + 0.5f - v1.pos.x);
	float w1base = (v0.pos.x - v2.pos.x)*(y0 + 0.5f - v2.pos.y) - (v0.pos.y - v2.pos.y)*(x0 + 0.5f - v2.pos.x);
	float w2base = (v1.pos.x - v0.pos.x)*(y0 + 0.5f - v0.pos.y) - (v1.pos.y - v0.pos.y)*(x0 + 0.5f - v0.pos.x);
	float  w0_stepX = -(v2.pos.y - v1.pos.y);
	float  w0_stepY = (v2.pos.x - v1.pos.x);
	float  w1_stepX = -(v0.pos.y - v2.pos.y);
	float  w1_stepY = (v0.pos.x - v2.pos.x);
	float  w2_stepX = -(v1.pos.y - v0.pos.y);
	float  w2_stepY = (v1.pos.x - v0.pos.x);
	//start 
	for (int x = x0, sx = 0; x <= x1; x++, sx++)
		for (int y = y0, sy = 0; y <= y1; y++, sy++)
		{
			Vertex SamplePixel = { {x + 0.5f ,y + 0.5f ,0.f,0.f},{} }; //采样点初值,颜色,UV坐标通过插值获取

			float w0 = w0base + w0_stepX * sx + w0_stepY * sy;
			float w1 = w1base + w1_stepX * sx + w1_stepY * sy;
			float w2 = w2base + w2_stepX * sx + w2_stepY * sy;

			if (!(w0 >= 0 && w1 >= 0 && w2 >= 0))//skip  pixel outside
				continue;
			Vweight.x = w0 * v0.pos.w / Vweight.w;//pos.w=1/z,查看NDC2Screen
			Vweight.y = w1 * v1.pos.w / Vweight.w;
			Vweight.z = w2 * v2.pos.w / Vweight.w;//存储 lambda/z 至Vweight
			//透视校正插值
			Interpolate(v0, v1, v2, SamplePixel, Vweight);
			//
			DrawPoint(x, y, m_pTexture->SampColor(SamplePixel.uv), SamplePixel.pos.z);
		}
}
//check the pixel if it inside the triangle,
//also store the coefficient ‘lambda’ in Vector Vw
//we will use this lambda to interpolate
bool Device_L::TriangleCheck(const Vertex& v0,const Vertex& v1, const Vertex& v2, Vertex& p, Vec4& Vw)
{
	// lambda0,lambda1,lambda2 stored in Vector weight,//multiply 1 / Z which stored in pos.w
	//注意三个顶点的传入顺序将影响颜色分量的位置分布	
	//float w0 = Math_L::EdgeFunc(v1.pos, v2.pos, p.pos);
	//float w1 = Math_L::EdgeFunc(v2.pos, v0.pos, p.pos);
	//float w2 = Math_L::EdgeFunc(v0.pos, v1.pos, p.pos);
	Vw.x = Math_L::EdgeFunc(v1.pos, v2.pos, p.pos) *v0.pos.w / Vw.w; //lambda0/z0
	Vw.y = Math_L::EdgeFunc(v2.pos, v0.pos, p.pos) *v1.pos.w/ Vw.w;//lambda1/z1	
	Vw.z = Math_L::EdgeFunc(v0.pos, v1.pos, p.pos) *v2.pos.w / Vw.w;//lambda2/z2

	return (Vw.x >= 0 && Vw.y >= 0 && Vw.z >= 0);
}//return true if pixel inside the triangle

//perspective correct interpolation
void Device_L::Interpolate(const Vertex& v0, const Vertex& v1, const Vertex& v2, Vertex& pixelSamp, const Vec4& Vw)
{
	// Find z-coordinate,treat as the interpolation coefficient
	//according to formula :	1/p.z = lambda0/v0.z+lambda1/v1.z+lambda2/v2.z
	//ref:http://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
	pixelSamp.pos.z = 1.f / (Vw.x + Vw.y + Vw.z);
	//XY坐标循环中获取,这里可略去pos插值
	//pixelSamp.pos = (v0.pos*Vw.x + v1.pos*Vw.y + v2.pos*Vw.z)*pixelSamp.pos.z;//
	if(m_randermode==Rander_State_Color)//顶点色插值
		pixelSamp.color = (v0.color*Vw.x + v1.color*Vw.y + v2.color*Vw.z)*pixelSamp.pos.z;//
	else//纹理插值
		pixelSamp.uv = (v0.uv*Vw.x + v1.uv*Vw.y + v2.uv*Vw.z)*pixelSamp.pos.z;//
}//插值所有顶点属性

/*Bresenham直线算法版本2,参考
void Device_L::DrawLine1(int x1, int y1, int x2, int y2,const Vec4& color)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int stepx = 1;
	int stepy = 1;

	if (dx >= 0)
	{
		stepx = 1;
	}
	else
	{
		stepx = -1;
		dx = abs(dx);
	}

	if (dy >= 0)
	{
		stepy = 1;
	}
	else
	{
		stepy = -1;
		dy = abs(dy);
	}

	int deltaX = 2 * dx;
	int deltaY = 2 * dy;
	if (dx > dy)
	{
		int error = deltaY - dx;
		for (int i = 0; i <= dx; ++i)
		{
			DrawPoint(x1, y1, color);
			if (error >= 0)
			{
				error -= deltaX;
				y1 += stepy;
			}
			error += deltaY;
			x1 += stepx;
		}
	}
	else
	{
		int error = deltaX - dy;
		for (int i = 0; i <= dy; i++)
		{
			DrawPoint(x1, y1, color);
			if (error >= 0)
			{
				error -= deltaY;
				x1 += stepx;
			}
			error += deltaX;
			y1 += stepy;
		}
	}
}
*/