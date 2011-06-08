
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiGeometry2D.h"
#include <assert.h>
#include "../Include/UiTBC.h"



namespace UiTbc
{



Geometry2D::Geometry2D(unsigned pVertexFormat, int pVertexCapacity, int pTriangleCapacity):
	mVertexFormat(pVertexFormat),
	mVertexData(0),
	mColorData(0),
	mUVData(0),
	mVertexCapacity(0),
	mTriangleCapacity(pTriangleCapacity),
	mVertexCount(0),
	mTriangleCount(0)
{
	if (IsFlagSet(VTX_INDEX16))
	{
		mTriangleData16 = new uint16[pTriangleCapacity];
	}
	else
	{
		mTriangleData32 = new uint32[pTriangleCapacity];
	}

	Init(pVertexFormat, pVertexCapacity, pTriangleCapacity);
}

Geometry2D::~Geometry2D()
{
	delete[] (float*)mVertexData;	// Cast to dumb type; we don't want any destructor anyways.
	delete[] mColorData;
	delete[] mUVData;
	if (IsFlagSet(VTX_INDEX16))
	{
		delete[] mTriangleData16;
	}
	else
	{
		delete[] mTriangleData32;
	}
}

void Geometry2D::Init(unsigned pVertexFormat, int pVertexCapacity, int pTriangleCapacity)
{
	mVertexFormat = pVertexFormat;

	if (mVertexData == 0)
	{
		if (IsFlagSet(VTX_INTERLEAVED))
		{
			unsigned lFlags = (mVertexFormat & (VTX_UV | VTX_RGB));
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
			if (IsFlagSet(VTX_UV))
			{
				mUVData = new float[pVertexCapacity * 2];
			}
			if (IsFlagSet(VTX_RGB))
			{
				mColorData = new float[pVertexCapacity * 3];
			}
		}
	}
	else
	{
		assert(!IsFlagSet(VTX_INTERLEAVED));	// Cannot handle interleaved yet.

		if (IsFlagSet(VTX_UV) != (mUVData != 0))
		{
			if (mUVData)
			{
				delete (mUVData);
				mUVData = 0;
			}
			else
			{
				mUVData = new float[pVertexCapacity * 2];
			}
		}
		if (IsFlagSet(VTX_RGB) != (mColorData != 0))
		{
			if (mColorData)
			{
				delete (mColorData);
				mColorData = 0;
			}
			else
			{
				mColorData = new float[pVertexCapacity * 3];
			}
		}
		AssureVertexCapacity(pVertexCapacity);
		AssureTriangleCapacity(pTriangleCapacity);
		Reset();
	}
}

void Geometry2D::ReallocVertexBuffers(int pVertexCapacity)
{
	if (IsFlagSet(VTX_INTERLEAVED))
	{
		unsigned lFlags = (mVertexFormat & (VTX_UV | VTX_RGB));
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
		if (mColorData)
		{
			void* lTempColor = (void*)mColorData;
			Realloc(&lTempColor, pVertexCapacity * 3 * sizeof(float), mVertexCount * 3 * sizeof(float));
			mColorData = (float*)lTempColor;
		}
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
	if (IsFlagSet(VTX_INDEX16))
	{
		uint16* lTriangleData16 = new uint16[pTriangleCapacity * 3];
		::memcpy(lTriangleData16, mTriangleData16, std::min(pTriangleCapacity, mTriangleCount) * 3 * sizeof(uint16));
		delete[] mTriangleData16;
		mTriangleData16 = lTriangleData16;
	}
	else
	{
		uint32* lTriangleData32 = new uint32[pTriangleCapacity * 3];
		::memcpy(lTriangleData32, mTriangleData32, std::min(pTriangleCapacity, mTriangleCount) * 3 * sizeof(uint32));
		delete[] mTriangleData32;
		mTriangleData32 = lTriangleData32;
	}
	mTriangleCapacity = pTriangleCapacity;
}

void Geometry2D::AssureVertexCapacity(int pVertexCapacity)
{
	if (pVertexCapacity >= mVertexCapacity)
	{
		int lNewCapacity = pVertexCapacity + mVertexCapacity / 2;
		ReallocVertexBuffers(lNewCapacity);
	}
}

void Geometry2D::AssureTriangleCapacity(int pTriangleCapacity)
{
	if (pTriangleCapacity >= mTriangleCapacity)
	{
		int lNewCapacity = pTriangleCapacity + mTriangleCapacity / 2;
		ReallocTriangleBuffer(lNewCapacity);
	}
}

uint32 Geometry2D::SetVertex(float x, float y)
{
	AssureVertexCapacity(mVertexCount + 1);

	if (IsFlagSet(VTX_INTERLEAVED))
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

uint32 Geometry2D::SetVertex(float x, float y, float u, float v)
{
	AssureVertexCapacity(mVertexCount + 1);

	if (IsFlagSet(VTX_INTERLEAVED))
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

uint32 Geometry2D::SetVertex(float x, float y, float r, float g, float b)
{
	AssureVertexCapacity(mVertexCount + 1);

	if (IsFlagSet(VTX_INTERLEAVED))
	{
		VertexXYRGB* lVertexData = (VertexXYRGB*)mVertexData;
		lVertexData[mVertexCount].x = x;
		lVertexData[mVertexCount].y = y;
		lVertexData[mVertexCount].z = 0;
		lVertexData[mVertexCount].w = 1;
		lVertexData[mVertexCount].mBlue  = (uint8)(b * 255.0f);
		lVertexData[mVertexCount].mGreen = (uint8)(g * 255.0f);
		lVertexData[mVertexCount].mRed   = (uint8)(r * 255.0f);
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

uint32 Geometry2D::SetVertex(float x, float y, float u, float v, float r, float g, float b)
{
	AssureVertexCapacity(mVertexCount + 1);

	if (IsFlagSet(VTX_INTERLEAVED))
	{
		VertexXYUVRGB* lVertexData = (VertexXYUVRGB*)mVertexData;
		lVertexData[mVertexCount].x = x;
		lVertexData[mVertexCount].y = y;
		lVertexData[mVertexCount].z = 0;
		lVertexData[mVertexCount].w = 1;
		lVertexData[mVertexCount].u = u;
		lVertexData[mVertexCount].v = v;

		lVertexData[mVertexCount].mBlue  = (uint8)(b * 255.0f);
		lVertexData[mVertexCount].mGreen = (uint8)(g * 255.0f);
		lVertexData[mVertexCount].mRed   = (uint8)(r * 255.0f);
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

void Geometry2D::SetTriangle(uint32 pV1, uint32 pV2, uint32 pV3)
{
	AssureTriangleCapacity(mTriangleCount + 1);

	int lTriangleIndex = mTriangleCount * 3;
	if (IsFlagSet(VTX_INDEX16))
	{
		mTriangleData16[lTriangleIndex + 0] = (uint16)pV1;
		mTriangleData16[lTriangleIndex + 1] = (uint16)pV2;
		mTriangleData16[lTriangleIndex + 2] = (uint16)pV3;
	}
	else
	{
		mTriangleData32[lTriangleIndex + 0] = pV1;
		mTriangleData32[lTriangleIndex + 1] = pV2;
		mTriangleData32[lTriangleIndex + 2] = pV3;
	}
	mTriangleCount++;
}

void Geometry2D::Reset()
{
	mVertexCount = 0;
	mTriangleCount = 0;
}

void Geometry2D::Realloc(void** pData, size_t pNewSize, size_t pBytesToCopy)
{
	uint8* lNewData = new uint8[pNewSize];
	::memcpy(lNewData, *pData, std::min(pNewSize, pBytesToCopy));
	delete[] (float*)(*pData);	// Cast to dumb type, we don't want any destructor anyways.
	*pData = lNewData;
}

Geometry2D::Geometry2D(const Geometry2D&)
{
	assert(false);
}

void Geometry2D::operator=(const Geometry2D&)
{
	assert(false);
}



}
