/*
	File:   ShadowVolume.cpp
	Class:  ShadowVolume
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../Include/UiShadowVolume.h"

#include <crtdbg.h>

namespace UiTbc
{

ShadowVolume::ShadowVolume() :
	mVertexData(0),
	mIndexData(0),
	mTriangleOrientation(0),
	mVertexCount(0),
	mTriangleCount(0),
	mParentVertexCount(0),
	mMaxTriangleCount(0),
	mRed(0),
	mGreen(0),
	mBlue(0),
	mLastFrameVisible(0),
	mTransformationChanged(false),
	mParentGeometry(0)
{
}

ShadowVolume::ShadowVolume(TBC::GeometryBase* pParentGeometry) :
	mVertexData(0),
	mIndexData(0),
	mTriangleOrientation(0),
	mVertexCount(0),
	mTriangleCount(0),
	mParentVertexCount(0),
	mMaxTriangleCount(0),
	mRed(0),
	mGreen(0),
	mBlue(0),
	mLastFrameVisible(0),
	mTransformationChanged(false),
	mParentGeometry(pParentGeometry)
{
	if (mParentGeometry->GetEdgeData() == 0)
	{
		mParentGeometry->GenerateEdgeData();
	}

	mMaxTriangleCount = mParentGeometry->GetMaxTriangleCount() * 2 + mParentGeometry->GetEdgeCount() * 2;
	mTriangleCount = 0;
}

ShadowVolume::~ShadowVolume()
{
	if (mVertexData != 0)
		delete[] mVertexData;

	if (mIndexData != 0)
		delete[] mIndexData;

	if (mTriangleOrientation != 0)
		delete[] mTriangleOrientation;
}

TBC::GeometryBase* ShadowVolume::GetParentGeometry()
{
	return mParentGeometry;
}

void ShadowVolume::SetParentGeometry(TBC::GeometryBase* pParentGeometry)
{
	mParentGeometry = pParentGeometry;

	if (mParentGeometry->GetEdgeData() == 0)
	{
		mParentGeometry->GenerateEdgeData();
	}

	mMaxTriangleCount = mParentGeometry->GetMaxTriangleCount() * 2 + mParentGeometry->GetEdgeCount() * 2;
	mTriangleCount = 0;
}

unsigned int ShadowVolume::GetMaxVertexCount() const
{
	return mParentGeometry->GetMaxVertexCount() * 2;
}

unsigned int ShadowVolume::GetMaxIndexCount() const
{
	return mMaxTriangleCount * 3;
}

unsigned int ShadowVolume::GetVertexCount() const
{
	return mVertexCount;
}

unsigned int ShadowVolume::GetIndexCount() const
{
	return mTriangleCount * 3;
}

unsigned int ShadowVolume::GetUVSetCount() const
{
	return 0;
}

float* ShadowVolume::GetVertexData() const
{
	return mVertexData;
}

float* ShadowVolume::GetUVData(unsigned int) const
{
	return 0;
}

uint32* ShadowVolume::GetIndexData() const
{
	return mIndexData;
}

uint8* ShadowVolume::GetColorData() const
{
	return 0;
}

float* ShadowVolume::GetNormalData() const
{
	return 0;
}

TBC::GeometryBase::ColorFormat ShadowVolume::GetColorFormat() const
{
	return TBC::GeometryBase::COLOR_RGB;
}


void ShadowVolume::SetLastFrameVisible(unsigned int pLastFrameVisible)
{
	mLastFrameVisible = pLastFrameVisible;
}

unsigned int ShadowVolume::GetLastFrameVisible() const
{
	return mLastFrameVisible;
}

void ShadowVolume::GetReplacementColor(float& pRed, float& pGreen, float& pBlue) const
{
	pRed   = mRed;
	pGreen = mGreen;
	pBlue  = mBlue;
}

void ShadowVolume::SetReplacementColor(float pRed, float pGreen, float pBlue)
{
	mRed   = pRed;
	mGreen = pGreen;
	mBlue  = pBlue;
}

TBC::GeometryBase::GeometryVolatility ShadowVolume::GetGeometryVolatility() const
{
	if (mParentGeometry)
	{
		return (mParentGeometry->GetGeometryVolatility());
	}

	return (TBC::GeometryBase::GEOM_VOLATILE);
}

void ShadowVolume::SetGeometryVolatility(TBC::GeometryBase::GeometryVolatility pVolatility)
{
	if (mParentGeometry)
	{
		mParentGeometry->SetGeometryVolatility(pVolatility);
	}
}

TBC::GeometryBase::PrimitiveType ShadowVolume::GetPrimitiveType() const
{
	return TBC::GeometryBase::TRIANGLES;
}

void ShadowVolume::InitVertices()
{
	bool lInitVertices = mParentGeometry->GetVertexDataChanged();

	if (mParentGeometry->GetVertexCount() > mParentVertexCount)
	{
		if (mVertexData != 0)
			delete[] mVertexData;

		mParentVertexCount  = mParentGeometry->GetVertexCount();
		mVertexCount  = mParentVertexCount * 2;
		mVertexData = new float[mVertexCount * 3];

		lInitVertices = true;
	}

	if (lInitVertices == true)
	{
		// Copy vertex data from parent, twice.
		float* lParentVertexData = mParentGeometry->GetVertexData();
		::memcpy(mVertexData, lParentVertexData, mParentVertexCount * 3 * sizeof(float));
		::memcpy(&mVertexData[mParentVertexCount * 3], lParentVertexData, mParentVertexCount * 3 * sizeof(float));
	}
}

void ShadowVolume::InitTO()
{
	unsigned int lNumMaxTriangles = mParentGeometry->GetMaxTriangleCount() * 2 + mParentGeometry->GetEdgeCount() * 2;

	if (lNumMaxTriangles > mMaxTriangleCount)
	{
		if (mTriangleOrientation != 0)
		{
			delete[] mTriangleOrientation;
			mTriangleOrientation = 0;
		}

		if (mIndexData != 0)
		{
			delete[] mIndexData;
			mIndexData = 0;
		}

		mMaxTriangleCount = lNumMaxTriangles;
	}

	if (mTriangleOrientation == 0)
	{
		mTriangleOrientation = new TriangleOrientation[mMaxTriangleCount];
	}

	if (mIndexData == 0)
	{
		mIndexData = new uint32[mMaxTriangleCount * 3];
	}
}

#define MACRO_ADDFRONTCAPTRIANGLE(to) \
{ \
	if (to.mChecked == false) \
	{ \
		mIndexData[lIndex + 0] = to.mV0; \
		mIndexData[lIndex + 1] = to.mV1; \
		mIndexData[lIndex + 2] = to.mV2; \
		mTriangleCount++; \
		lIndex += 3; \
 \
		to.mChecked = true; \
	} \
}

#define MACRO_ADDBACKCAPTRIANGLE(to) \
{ \
	if (to.mChecked == false) \
	{ \
		mIndexData[lIndex + 0] = to.mV0 + mParentVertexCount; \
		mIndexData[lIndex + 1] = to.mV1 + mParentVertexCount; \
		mIndexData[lIndex + 2] = to.mV2 + mParentVertexCount; \
		mTriangleCount++; \
		lIndex += 3; \
 \
		to.mChecked = true; \
	} \
}

#define MACRO_ADDBACKCAPTRIANGLE_FLIPPED(to) \
{ \
	if (to.mChecked == false) \
	{ \
		mIndexData[lIndex + 0] = to.mV2 + mParentVertexCount; \
		mIndexData[lIndex + 1] = to.mV1 + mParentVertexCount; \
		mIndexData[lIndex + 2] = to.mV0 + mParentVertexCount; \
		mTriangleCount++; \
		lIndex += 3; \
 \
		to.mChecked = true; \
	} \
}

void ShadowVolume::UpdateShadowVolume(const Vector3DF& pLightPos, float pShadowRange, bool pDirectional)
{
	TBC::GeometryBase::SetLastFrameVisible(mParentGeometry->GetLastFrameVisible());

	SetTransformation(mParentGeometry->GetTransformation());

	InitVertices();
	InitTO();

	mTriangleCount = 0;

	//
	// Transform the light position in object space.
	//

	Vector3DF lLightPos;
	
	if (pDirectional == true)
	{
		lLightPos = mParentGeometry->GetTransformation().GetOrientation().GetInverseRotatedVector(pLightPos);
		lLightPos.Normalize(pShadowRange);
	}
	else
	{
		lLightPos = mParentGeometry->GetTransformation().InverseTransform(pLightPos);
	}

	//
	// Calculate the shadow meshs' vertex positions.
	//

	mParentGeometry->GenerateSurfaceNormalData();
	float* lSurfaceNormalData = mParentGeometry->GetSurfaceNormalData();
	float* lVertexData = mParentGeometry->GetVertexData();

	unsigned int i;
	unsigned int lTriangleCount = mParentGeometry->GetTriangleCount();
	if (pDirectional == true)
	{
		// Calculate triangle orientations relative to light source.
		for (i = 0; i < lTriangleCount; i++)
		{
			uint32 lVertexIndex[3];
			mParentGeometry->GetTriangleIndices(i, lVertexIndex);
			
			unsigned int lTriIndex = i * 3;
			Vector3DF lSurfaceNormal(lSurfaceNormalData[lTriIndex + 0],
							 lSurfaceNormalData[lTriIndex + 1],
							 lSurfaceNormalData[lTriIndex + 2]);

			// Get the vector between one corner of the triangle and the light source.
			mTriangleOrientation[i].mV0 = lVertexIndex[0];
			mTriangleOrientation[i].mV1 = lVertexIndex[1];
			mTriangleOrientation[i].mV2 = lVertexIndex[2];
			mTriangleOrientation[i].mChecked = false;

			// Light position is now treated as a direction instead.
			if (lLightPos.Dot(lSurfaceNormal) <= 0.0f)
			{
				// Front towards light source.
				mTriangleOrientation[i].mTO = TO_FRONT;
			}
			else
			{
				// Back towards light source.
				mTriangleOrientation[i].mTO = TO_BACK;
			}
		}

		// Move vertex twins away from lightsource.
		for (i = 0; i < mParentVertexCount; i++)
		{
			// Read original vertex.
			int lIndex0 = i * 3;
			int lIndex1 = (i + mParentVertexCount) * 3;

			mVertexData[lIndex1 + 0] = mVertexData[lIndex0 + 0] + (float)lLightPos.x;
			mVertexData[lIndex1 + 1] = mVertexData[lIndex0 + 1] + (float)lLightPos.y;
			mVertexData[lIndex1 + 2] = mVertexData[lIndex0 + 2] + (float)lLightPos.z;
		}
	}
	else // if light is a point light (pDirectional == false).
	{
		// Calculate triangle orientations relative to light source.
		for (i = 0; i < lTriangleCount; i++)
		{
			uint32 lVertexIndex[3];
			mParentGeometry->GetTriangleIndices(i, lVertexIndex);
			
			unsigned int lTriIndex = i * 3;
			Vector3DF lSurfaceNormal(lSurfaceNormalData[lTriIndex + 0],
							 lSurfaceNormalData[lTriIndex + 1],
							 lSurfaceNormalData[lTriIndex + 2]);

			// Get the vector between one corner of the triangle and the light source.
			unsigned int lIndex = lVertexIndex[0] * 3;
			Vector3DF lVector(lVertexData[lIndex + 0] - lLightPos.x,
						  lVertexData[lIndex + 1] - lLightPos.y,
						  lVertexData[lIndex + 2] - lLightPos.z);

			mTriangleOrientation[i].mV0 = lVertexIndex[0];
			mTriangleOrientation[i].mV1 = lVertexIndex[1];
			mTriangleOrientation[i].mV2 = lVertexIndex[2];
			mTriangleOrientation[i].mChecked = false;

			if (lVector.Dot(lSurfaceNormal) <= 0.0f)
			{
				// Front towards light source.
				mTriangleOrientation[i].mTO = TO_FRONT;
			}
			else
			{
				// Back towards light source.
				mTriangleOrientation[i].mTO = TO_BACK;
			}
		}

		float lShadowRangeSquared = pShadowRange * pShadowRange;
		// Move vertex twins away from lightsource.
		for (i = 0; i < mParentVertexCount; i++)
		{
			// Read original vertex.
			int lIndex0 = i * 3;
			Vector3DF lVector(mVertexData[lIndex0 + 0] - lLightPos.x,
						  mVertexData[lIndex0 + 1] - lLightPos.y,
						  mVertexData[lIndex0 + 2] - lLightPos.z);

			// Move, and write to its "twin vertex".
			if (lVector.GetLengthSquared() < lShadowRangeSquared)
			{
				lVector.Normalize(pShadowRange);

				int lIndex1 = (i + mParentVertexCount) * 3;
				mVertexData[lIndex1 + 0] = (float)(lLightPos.x + lVector.x);
				mVertexData[lIndex1 + 1] = (float)(lLightPos.y + lVector.y);
				mVertexData[lIndex1 + 2] = (float)(lLightPos.z + lVector.z);
			}
		}
	}


	//
	// Generate triangles.
	//

	if (mParentGeometry->GetEdgeData() == 0)
	{
		mParentGeometry->GenerateEdgeData();
	}

	TBC::GeometryBase::Edge* lEdges = mParentGeometry->GetEdgeData();

	for (i = 0; i < mParentGeometry->GetEdgeCount(); i++)
	{
		Edge& lEdge = lEdges[i];

		//_ASSERT(lEdge.mTriangleCount == 1 || lEdge.mTriangleCount == 2);
		//_ASSERT(lEdge.mVertex[0] != lEdge.mVertex[1]);
		//_ASSERT(lEdge.mTriangle[0] != lEdge.mTriangle[1]);

		// Set this to true if we have a silhouette edge.
		bool lExtrudeEdge = false;
		TriangleOrientation* lFT = 0;
		unsigned int lIndex = 0;

		if (lEdge.mTriangleCount == 2)
		{
			TriangleOrientation& lT0 = mTriangleOrientation[lEdge.mTriangle[0]];
			TriangleOrientation& lT1 = mTriangleOrientation[lEdge.mTriangle[1]];

			//_ASSERT(lT0.mTO != TO_INVALID);
			//_ASSERT(lT1.mTO != TO_INVALID);

			lIndex = mTriangleCount * 3;

			if (lT0.mTO == TO_FRONT)
			{
				// Add front cap triangle.
				MACRO_ADDFRONTCAPTRIANGLE(lT0);

				if (lT1.mTO == TO_FRONT)
				{
					// Add front cap triangle.
					MACRO_ADDFRONTCAPTRIANGLE(lT1);
				}
				else
				{
					lFT = &lT0;
					lExtrudeEdge = true;

					// Add back cap triangle.
					MACRO_ADDBACKCAPTRIANGLE(lT1);
				}
			}
			else
			{
				// Add back cap triangle.
				MACRO_ADDBACKCAPTRIANGLE(lT0);

				if (lT1.mTO == TO_FRONT)
				{
					lFT = &lT1;
					lExtrudeEdge = true;

					// Add front cap triangle.
					MACRO_ADDFRONTCAPTRIANGLE(lT1);
				}
				else
				{
					// Add back cap triangle.
					MACRO_ADDBACKCAPTRIANGLE(lT1);
				}
			}
		}
		else // lEdge.mTriangleCount == 1
		{
			TriangleOrientation& lT0 = mTriangleOrientation[lEdge.mTriangle[0]];

			_ASSERT(lT0.mTO != TO_INVALID);

			lIndex = mTriangleCount * 3;

			if (lT0.mTO == TO_FRONT)
			{
				// Add front cap triangle.
				MACRO_ADDFRONTCAPTRIANGLE(lT0);
				MACRO_ADDBACKCAPTRIANGLE_FLIPPED(lT0);
				lFT = &lT0;
				lExtrudeEdge = true;
			}
		}

		if (lExtrudeEdge == true)
		{
			int lV0;
			int lV1;
			int lV2;

			if ((int)lFT->mV0 != lEdge.mVertex[0] &&
			   (int)lFT->mV0 != lEdge.mVertex[1])
			{
				lV0 = lFT->mV2;
				lV1 = lFT->mV1;
				lV2 = lFT->mV0;
			}
			else if((int)lFT->mV1 != lEdge.mVertex[0] &&
				(int)lFT->mV1 != lEdge.mVertex[1])
			{
				lV0 = lFT->mV0;
				lV1 = lFT->mV2;
				lV2 = lFT->mV1;
			}
			else if((int)lFT->mV2 != lEdge.mVertex[0] &&
				(int)lFT->mV2 != lEdge.mVertex[1])
			{
				lV0 = lFT->mV1;
				lV1 = lFT->mV0;
				lV2 = lFT->mV2;
			}
			else
			{
				lV0 = 0;
				lV1 = 0;
				lV2 = 0;
			}

			// Create two triangles that are extruded from the edge.
			mIndexData[lIndex + 0] = lV0;
			mIndexData[lIndex + 1] = lV1;
			mIndexData[lIndex + 2] = lV0 + mParentVertexCount;
			mIndexData[lIndex + 3] = lV1;
			mIndexData[lIndex + 4] = lV1 + mParentVertexCount;
			mIndexData[lIndex + 5] = lV0 + mParentVertexCount;

			mTriangleCount += 2;

			_ASSERT(mTriangleCount < mMaxTriangleCount);
		}
	}

	TBC::GeometryBase::SetVertexDataChanged(true);
	TBC::GeometryBase::SetIndexDataChanged(true);
}

} // End namespace.
