
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiGeometryBatch.h"
#include "../../Lepra/Include/Random.h"



namespace UiTbc
{



GeometryBatch::GeometryBatch(TBC::GeometryBase* pGeometry):
	mVertex(0),
	mUV(0),
	mIndex(0),
	mColor(0),
	mVertexCount(0),
	mIndexCount(0),
	mUVSetCount(0),
	mGeometryInstance(pGeometry)
{
	SetBasicMaterialSettings(mGeometryInstance->GetBasicMaterialSettings());
}

GeometryBatch::~GeometryBatch()
{
	ClearAllInstances();
}

void GeometryBatch::SetInstances(const Vector3DF* pDisplacement, 
				 int pNumInstances,
				 float pXScaleMin, float pXScaleMax,
				 float pYScaleMin, float pYScaleMax,
				 float pZScaleMin, float pZScaleMax)
{
	ClearAllInstances();

	if (pNumInstances <= 0)
	{
		return;
	}

	mVertexCount = mGeometryInstance->GetVertexCount()  * pNumInstances;
	mIndexCount  = mGeometryInstance->GetTriangleCount() * pNumInstances * 3;
	mUVSetCount   = mGeometryInstance->GetUVSetCount();

	unsigned int i;
	unsigned int j;
	unsigned int k;

	if (mVertexCount > 0)
	{
		//
		// Allocate memory...
		//

		mVertex = new float[mVertexCount * 3];

		if (mUVSetCount > 0)
		{
			mUV = new float*[mUVSetCount];
			for (i = 0; i < mUVSetCount; i++)
			{
				mUV[i] = new float[mVertexCount * 2];
			}
		}

		if (mIndexCount > 0)
		{
			mIndex = new uint32[mIndexCount];
		}

		if (mGeometryInstance->GetColorData() != 0)
		{
			mColor = new uint8[mVertexCount * 3];
		}

		//
		// Copy data.
		//

		unsigned int lVertexCount  = mGeometryInstance->GetVertexCount();
		unsigned int lTriangleCount = mGeometryInstance->GetTriangleCount();
		float* lVertex = mGeometryInstance->GetVertexData();
		unsigned char* lColor = mGeometryInstance->GetColorData();

		int lVIndex = 0;
		int lTIndex = 0;
		int lUVIndex = 0;
		for (i = 0; i < (unsigned int)pNumInstances; i++)
		{
			float lXScale = (float)Random::Uniform(pXScaleMin, pXScaleMax);
			float lYScale = (float)Random::Uniform(pYScaleMin, pYScaleMax);
			float lZScale = (float)Random::Uniform(pZScaleMin, pZScaleMax);

			const Vector3DF& lPos = pDisplacement[i];
			int lIndex = 0;
			int lUVIndex2 = 0;
			for (j = 0; j < lVertexCount; j++)
			{
				mVertex[lVIndex + 0] = lVertex[lIndex + 0] * lXScale + lPos.x;
				mVertex[lVIndex + 1] = lVertex[lIndex + 1] * lYScale + lPos.y;
				mVertex[lVIndex + 2] = lVertex[lIndex + 2] * lZScale + lPos.z;

				if (lColor != 0)
				{
					mColor[lVIndex + 0] = lColor[lIndex + 0];
					mColor[lVIndex + 1] = lColor[lIndex + 1];
					mColor[lVIndex + 2] = lColor[lIndex + 2];
				}

				for (k = 0; k < mUVSetCount; k++)
				{
					float* lUV = mGeometryInstance->GetUVData(k);
					mUV[k][lUVIndex + 0] = lUV[lUVIndex2 + 0];
					mUV[k][lUVIndex + 1] = lUV[lUVIndex2 + 1];
				}

				lVIndex += 3;
				lIndex += 3;
				lUVIndex += 2;
				lUVIndex2 += 2;
			}

			unsigned int lTIOffset = lVertexCount * i;
			lIndex = 0;
			for (j = 0; j < lTriangleCount; j++)
			{
				uint32 lTriIndex[3];
				mGeometryInstance->GetTriangleIndices(j, lTriIndex);
				mIndex[lTIndex + 0] = lTriIndex[0] + lTIOffset;
				mIndex[lTIndex + 1] = lTriIndex[1] + lTIOffset;
				mIndex[lTIndex + 2] = lTriIndex[2] + lTIOffset;
				lTIndex += 3;
				lIndex += 3;
			}
		}
	}
}

void GeometryBatch::SetInstances(const Vector3DD* pDisplacement, 
				 int pNumInstances,
				 float pXScaleMin, float pXScaleMax,
				 float pYScaleMin, float pYScaleMax,
				 float pZScaleMin, float pZScaleMax)
{
	ClearAllInstances();

	if (pNumInstances <= 0)
	{
		return;
	}

	Vector3DF* lVTemp = new Vector3DF[pNumInstances];

	for (int i = 0; i < pNumInstances; i++)
	{
		lVTemp[i].x = (float)pDisplacement[i].x;
		lVTemp[i].y = (float)pDisplacement[i].y;
		lVTemp[i].z = (float)pDisplacement[i].z;
	}

	SetInstances(lVTemp, pNumInstances,
		     pXScaleMin, pXScaleMax,
		     pYScaleMin, pYScaleMax,
		     pZScaleMin, pZScaleMax);

	delete[] lVTemp;
}

void GeometryBatch::ClearAllInstances()
{
	if (mVertex != 0)
	{
		delete[] mVertex;
		mVertex = 0;
	}

	if (mUV != 0)
	{
		for (unsigned int i = 0; i < mUVSetCount; i++)
		{
			delete[] mUV[i];
		}

		delete[] mUV;
		mUV = 0;
	}

	if (mIndex != 0)
	{
		delete[] mIndex;
		mIndex = 0;
	}

	if (mColor != 0)
	{
		delete[] mColor;
		mColor = 0;
	}

	mVertexCount = 0;
	mIndexCount  = 0;
	mUVSetCount   = 0;
}

void GeometryBatch::SetGeometryVolatility(GeometryVolatility)
{
}

TBC::GeometryBase::GeometryVolatility GeometryBatch::GetGeometryVolatility() const
{
	return TBC::GeometryBase::GEOM_STATIC;
}

TBC::GeometryBase::PrimitiveType GeometryBatch::GetPrimitiveType() const
{
	return TBC::GeometryBase::TRIANGLES;
}

TBC::GeometryBase::ColorFormat GeometryBatch::GetColorFormat() const
{
	return TBC::GeometryBase::COLOR_RGB;
}

unsigned int GeometryBatch::GetMaxVertexCount()  const
{
	return mVertexCount;
}

unsigned int GeometryBatch::GetMaxIndexCount() const
{
	return mIndexCount;
}

unsigned int GeometryBatch::GetVertexCount()  const
{
	return mVertexCount;
}

unsigned int GeometryBatch::GetIndexCount() const
{
	return mIndexCount;
}

unsigned int GeometryBatch::GetUVSetCount()    const
{
	return mUVSetCount;
}

float* GeometryBatch::GetVertexData() const
{
	return mVertex;
}

float* GeometryBatch::GetUVData(unsigned int pUVSet) const
{
	if (pUVSet < mUVSetCount)
	{
		return mUV[pUVSet];
	}
	return 0;
}

uint32* GeometryBatch::GetIndexData() const
{
	return mIndex;
}

uint8* GeometryBatch::GetColorData() const
{
	return mColor;
}



}
