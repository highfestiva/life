/*
	Class:  Geometry2D
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../Include/UiGeometry2D.h"
#include "../Include/UiTbc.h"

namespace UiTbc
{

Geometry2D::Geometry2D(Lepra::uint16 pVertexFormat, int pVertexCapacity, int pTriangleCapacity) :
	mVertexFormat(pVertexFormat),
	mVertexData(0),
	mColorData(0),
	mUVData(0),
	mTriangleData(new Lepra::uint32[pTriangleCapacity * 3]),
	mVertexCapacity(pVertexCapacity),
	mTriangleCapacity(pTriangleCapacity),
	mVertexCount(0),
	mTriangleCount(0)
{
	if(IsFlagSet(VTX_INTERLEAVED))
	{
		Lepra::uint16 lFlags = (mVertexFormat & (VTX_UV | VTX_RGB));
		switch(lFlags)
		{
			case 0: mVertexData = new VertexXY[pVertexCapacity]; break;
			case VTX_UV: mVertexData = new VertexXYUV[pVertexCapacity]; break;
			case VTX_RGB: mVertexData = new VertexXYRGB[pVertexCapacity]; break;
			case VTX_UV | VTX_RGB: mVertexData = new VertexXYUVRGB[pVertexCapacity]; break;
		}
	}
	else
	{
		mVertexData = new float[pVertexCapacity * 2];
		if(IsFlagSet(VTX_UV))
		{
			mUVData = new float[pVertexCapacity * 2];
		}
		if(IsFlagSet(VTX_RGB))
		{
			mColorData = new float[pVertexCapacity * 3];
		}
	}
}

Geometry2D::~Geometry2D()
{
	delete[] mVertexData;
	delete[] mColorData;
	delete[] mUVData;
	delete[] mTriangleData;
}

void Geometry2D::Realloc(void** pData, size_t pNewSize, size_t pBytesToCopy)
{
	Lepra::uint8* lNewData = new Lepra::uint8[pNewSize];
	::memcpy(lNewData, *pData, min(pNewSize, pBytesToCopy));
	delete[] *pData;
	*pData = lNewData;
}

void Geometry2D::ReallocVertexBuffers(int pVertexCapacity)
{
	if(IsFlagSet(VTX_INTERLEAVED))
	{
		Lepra::uint16 lFlags = (mVertexFormat & (VTX_UV | VTX_RGB));
		switch(lFlags)
		{
			case 0: Realloc(&mVertexData, pVertexCapacity * sizeof(VertexXY), mVertexCount * sizeof(VertexXY)); break;
			case VTX_UV: Realloc(&mVertexData, pVertexCapacity * sizeof(VertexXYUV), mVertexCount * sizeof(VertexXYUV)); break;
			case VTX_RGB: Realloc(&mVertexData, pVertexCapacity * sizeof(VertexXYRGB), mVertexCount * sizeof(VertexXYRGB)); break;
			case VTX_UV | VTX_RGB: Realloc(&mVertexData, pVertexCapacity * sizeof(VertexXYUVRGB), mVertexCount * sizeof(VertexXYUVRGB)); break;
		}
	}
	else
	{
		Realloc(&mVertexData, pVertexCapacity * 2 * sizeof(float), mVertexCount * 2 * sizeof(float));
		void* lTempColor = (void*)mColorData;
		Realloc(&lTempColor, pVertexCapacity * 3 * sizeof(float), mVertexCount * 3 * sizeof(float));
		mColorData = (float*)lTempColor;
		if (mUVData)
		{
			void* lTempUV = (void*)mUVData;
			Realloc(&lTempUV, pVertexCapacity * 2 * sizeof(float), mVertexCount * 2 * sizeof(float));
			mUVData = (float*)lTempUV;
		}
	}
	mVertexCapacity = pVertexCapacity;
}

void Geometry2D::ReallocTriangleBuffer(int pTriangleCapacity)
{
	Lepra::uint32* lTriangleData = new Lepra::uint32[pTriangleCapacity * 3];
	::memcpy(lTriangleData, mTriangleData, min(pTriangleCapacity, mTriangleCount) * 3 * sizeof(Lepra::uint32));
	delete[] mTriangleData;
	mTriangleData = lTriangleData;
	mTriangleCapacity = mTriangleCapacity;
}

void Geometry2D::AssureVertexCapacity(int pVertexCapacity)
{
	if(pVertexCapacity >= mVertexCapacity)
	{
		int lNewCapacity = pVertexCapacity + mVertexCapacity / 2;
		ReallocVertexBuffers(lNewCapacity);
	}
}

void Geometry2D::AssureTriangleCapacity(int pTriangleCapacity)
{
	if(pTriangleCapacity >= mTriangleCapacity)
	{
		int lNewCapacity = pTriangleCapacity + mTriangleCapacity / 2;
		ReallocTriangleBuffer(lNewCapacity);
	}
}

Lepra::uint32 Geometry2D::SetVertex(float x, float y)
{
	AssureVertexCapacity(mVertexCount + 1);

	if(IsFlagSet(VTX_INTERLEAVED))
	{
		VertexXY* lVertexData = (VertexXY*)mVertexData;
		lVertexData[mVertexCount].x = x;
		lVertexData[mVertexCount].y = y;
	}
	else
	{
		int lVertexIndex = (mVertexCount << 1);
		float* lVertexData = (float*)mVertexData;
		lVertexData[lVertexIndex + 0] = x;
		lVertexData[lVertexIndex + 1] = y;
	}
	return mVertexCount++;
}

Lepra::uint32 Geometry2D::SetVertex(float x, float y, float u, float v)
{
	AssureVertexCapacity(mVertexCount + 1);

	if(IsFlagSet(VTX_INTERLEAVED))
	{
		VertexXYUV* lVertexData = (VertexXYUV*)mVertexData;
		lVertexData[mVertexCount].x = x;
		lVertexData[mVertexCount].y = y;
		lVertexData[mVertexCount].z = 0;
		lVertexData[mVertexCount].w = 1;
		lVertexData[mVertexCount].u = u;
		lVertexData[mVertexCount].v = v;
	}
	else
	{
		int lVertexIndex = (mVertexCount << 1);
		float* lVertexData = (float*)mVertexData;
		lVertexData[lVertexIndex + 0] = x;
		lVertexData[lVertexIndex + 1] = y;
		mUVData[lVertexIndex + 0] = u;
		mUVData[lVertexIndex + 1] = v;
	}
	return mVertexCount++;
}

Lepra::uint32 Geometry2D::SetVertex(float x, float y, float r, float g, float b)
{
	AssureVertexCapacity(mVertexCount + 1);

	if(IsFlagSet(VTX_INTERLEAVED))
	{
		VertexXYRGB* lVertexData = (VertexXYRGB*)mVertexData;
		lVertexData[mVertexCount].x = x;
		lVertexData[mVertexCount].y = y;
		lVertexData[mVertexCount].z = 0;
		lVertexData[mVertexCount].w = 1;
		lVertexData[mVertexCount].mBlue  = (Lepra::uint8)(b * 255.0f);
		lVertexData[mVertexCount].mGreen = (Lepra::uint8)(g * 255.0f);
		lVertexData[mVertexCount].mRed   = (Lepra::uint8)(r * 255.0f);
		lVertexData[mVertexCount].mAlpha = 255;
	}
	else
	{
		int lVertexIndex = (mVertexCount << 1);
		float* lVertexData = (float*)mVertexData;
		lVertexData[lVertexIndex + 0] = x;
		lVertexData[lVertexIndex + 1] = y;

		lVertexIndex += mVertexCount; // lVertexIndex = mCurrentVertex * 3.
		mColorData[lVertexIndex + 0] = r;
		mColorData[lVertexIndex + 1] = g;
		mColorData[lVertexIndex + 2] = b;
	}
	return mVertexCount++;
}

Lepra::uint32 Geometry2D::SetVertex(float x, float y, float u, float v, float r, float g, float b)
{
	AssureVertexCapacity(mVertexCount + 1);

	if(IsFlagSet(VTX_INTERLEAVED))
	{
		VertexXYUVRGB* lVertexData = (VertexXYUVRGB*)mVertexData;
		lVertexData[mVertexCount].x = x;
		lVertexData[mVertexCount].y = y;
		lVertexData[mVertexCount].z = 0;
		lVertexData[mVertexCount].w = 1;
		lVertexData[mVertexCount].u = u;
		lVertexData[mVertexCount].v = v;

		lVertexData[mVertexCount].mBlue  = (Lepra::uint8)(b * 255.0f);
		lVertexData[mVertexCount].mGreen = (Lepra::uint8)(g * 255.0f);
		lVertexData[mVertexCount].mRed   = (Lepra::uint8)(r * 255.0f);
		lVertexData[mVertexCount].mAlpha = 255;
	}
	else
	{
		int lVertexIndex = (mVertexCount << 1);
		float* lVertexData = (float*)mVertexData;
		lVertexData[lVertexIndex + 0] = x;
		lVertexData[lVertexIndex + 1] = y;
		mUVData[lVertexIndex + 0] = u;
		mUVData[lVertexIndex + 1] = v;

		lVertexIndex += mVertexCount; // lVertexIndex = mCurrentVertex * 3.
		mColorData[lVertexIndex + 0] = r;
		mColorData[lVertexIndex + 1] = g;
		mColorData[lVertexIndex + 2] = b;
	}
	return mVertexCount++;
}

void Geometry2D::SetTriangle(Lepra::uint32 pV1, Lepra::uint32 pV2, Lepra::uint32 pV3)
{
	AssureTriangleCapacity(mTriangleCount + 1);

	int lTriangleIndex = mTriangleCount * 3;
	mTriangleData[lTriangleIndex + 0] = pV1;
	mTriangleData[lTriangleIndex + 1] = pV2;
	mTriangleData[lTriangleIndex + 2] = pV3;
	mTriangleCount++;
}

void Geometry2D::Reset()
{
	mVertexCount = 0;
	mTriangleCount = 0;
}

} // End namespace.
