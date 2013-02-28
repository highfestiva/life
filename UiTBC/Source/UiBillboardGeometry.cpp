
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiBillboardGeometry.h"
#include "../../Lepra/Include/CyclicArray.h"



namespace UiTbc
{



BillboardGeometry::BillboardGeometry(unsigned pUVSetCount):
	mUVSetCount(pUVSetCount)
{
	const float lVertexData[] =
	{
		-1,0,+1,
		-1,0,-1,
		+1,0,-1,
		+1,0,+1,
	};
	::memcpy(mVertexData, lVertexData, sizeof(mVertexData));
	mIndexData[0] = 0;
	mIndexData[1] = 1;
	mIndexData[2] = 2;
	mIndexData[3] = 3;

	mUVData = new float[2*4*mUVSetCount];
	const float lWidth = 1.0f/mUVSetCount;
	const float lUVData[] =
	{
		0,0,
		0,1,
		1,1,
		1,0,
	};
	for (unsigned x = 0; x < mUVSetCount; ++x)
	{
		const float x0 = lWidth*x;
		int lBase = x*4*2;
		for (int y = 0; y < 4; ++y)
		{
			mUVData[lBase+y*2+0] = lUVData[y*2+0]*lWidth + x0;	// x
			mUVData[lBase+y*2+1] = lUVData[y*2+1];	// y
		}
	}
}

BillboardGeometry::~BillboardGeometry()
{
	delete mUVData;
	mUVData = 0;
}

unsigned BillboardGeometry::GetMaxVertexCount() const
{
	return LEPRA_ARRAY_COUNT(mVertexData) / 3;
}

unsigned BillboardGeometry::GetMaxIndexCount() const
{
	return LEPRA_ARRAY_COUNT(mIndexData);
}

unsigned BillboardGeometry::GetVertexCount() const
{
	return LEPRA_ARRAY_COUNT(mVertexData) / 3;
}

unsigned BillboardGeometry::GetIndexCount() const
{
	return LEPRA_ARRAY_COUNT(mIndexData);
}

unsigned BillboardGeometry::GetUVSetCount() const
{
	return mUVSetCount;
}

float* BillboardGeometry::GetVertexData() const
{
	return (float*)&mVertexData[0];
}

void BillboardGeometry::SetVertexData(const float pVertexData[12])
{
	::memcpy(mVertexData, pVertexData, sizeof(mVertexData));
}

float* BillboardGeometry::GetUVData(unsigned pUVSet) const
{
	return &mUVData[pUVSet*2*4];
}

vtx_idx_t* BillboardGeometry::GetIndexData() const
{
	return (vtx_idx_t*)&mIndexData[0];
}

uint8* BillboardGeometry::GetColorData() const
{
	return 0;
}

TBC::GeometryBase::PrimitiveType BillboardGeometry::GetPrimitiveType() const
{
	return QUADS;
}

TBC::GeometryBase::GeometryVolatility BillboardGeometry::GetGeometryVolatility() const
{
	return GEOM_STATIC;
}

void BillboardGeometry::SetGeometryVolatility(TBC::GeometryBase::GeometryVolatility)
{
}



}
