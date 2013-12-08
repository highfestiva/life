
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiShadowVolume.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../Lepra/Include/Thread.h"



namespace UiTbc
{



ShadowVolume::ShadowVolume(TBC::GeometryBase* pParentGeometry):
	mVertexData(0),
	mIndexData(0),
	mTriangleOrientation(0),
	mVertexCount(0),
	mTriangleCount(0),
	mParentVertexCount(0),
	mMaxTriangleCount(0),
	mParentGeometry(pParentGeometry)
{
	LEPRA_ACQUIRE_RESOURCE(ShadowVolume);

	LEPRA_DEBUG_CODE(mName = _T("Shdw->") + pParentGeometry->mName);

	if (mParentGeometry->GetEdgeData() == 0)
	{
		mParentGeometry->GenerateEdgeData();
	}

	SetScale(pParentGeometry->GetScale());

	mMaxTriangleCount = mParentGeometry->GetMaxTriangleCount() * 2 + mParentGeometry->GetEdgeCount() * 2;
	mTriangleCount = 0;
}

ShadowVolume::~ShadowVolume()
{
	delete[] mVertexData;
	mVertexData = 0;
	delete[] mIndexData;
	mIndexData = 0;
	delete[] mTriangleOrientation;
	mTriangleOrientation = 0;
	LEPRA_ADD_RESOURCE(ShadowVolume_Triangles, -(int)mMaxTriangleCount);

	LEPRA_RELEASE_RESOURCE(ShadowVolume);
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

unsigned ShadowVolume::GetMaxVertexCount() const
{
	return mParentGeometry->GetMaxVertexCount() * 2;
}

unsigned ShadowVolume::GetMaxIndexCount() const
{
	return mMaxTriangleCount * 3;
}

unsigned ShadowVolume::GetVertexCount() const
{
	return mVertexCount;
}

unsigned ShadowVolume::GetIndexCount() const
{
	return mTriangleCount * 3;
}

unsigned ShadowVolume::GetUVSetCount() const
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

vtx_idx_t* ShadowVolume::GetIndexData() const
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
		mParentVertexCount  = mParentGeometry->GetVertexCount();
		mVertexCount  = mParentVertexCount * 2;
		delete[] mVertexData;
		mVertexData = new float[mVertexCount * 3];
		lInitVertices = true;
	}

	if (lInitVertices == true)
	{
		// Copy vertex data from parent, twice.
		const float* lParentVertexData = mParentGeometry->GetVertexData();
		::memcpy(mVertexData, lParentVertexData, mParentVertexCount * 3 * sizeof(float));
		::memcpy(&mVertexData[mParentVertexCount * 3], lParentVertexData, mParentVertexCount * 3 * sizeof(float));
	}
}

void ShadowVolume::InitTO()
{
	unsigned lNumMaxTriangles = mParentGeometry->GetMaxTriangleCount() * 2 + mParentGeometry->GetEdgeCount() * 2;

	if (lNumMaxTriangles > mMaxTriangleCount)
	{
		delete[] mTriangleOrientation;
		mTriangleOrientation = 0;
		LEPRA_ADD_RESOURCE(ShadowVolume_Triangles, -(int)mMaxTriangleCount);
		delete[] mIndexData;
		mIndexData = 0;

		mMaxTriangleCount = lNumMaxTriangles;
	}

	if (mTriangleOrientation == 0)
	{
		mTriangleOrientation = new TriangleOrientation[mMaxTriangleCount];
		LEPRA_ADD_RESOURCE(ShadowVolume_Triangles, +(int)mMaxTriangleCount);
	}

	if (mIndexData == 0)
	{
		mIndexData = new vtx_idx_t[mMaxTriangleCount * 3];
	}
}

#define MACRO_ADDFRONTCAPTRIANGLE(to) \
{ \
	if (to.mChecked == false) \
	{ \
		*lIndexData++ = to.mV0; \
		*lIndexData++ = to.mV1; \
		*lIndexData++ = to.mV2; \
		++mTriangleCount; \
		to.mChecked = true; \
	} \
}

#define MACRO_ADDBACKCAPTRIANGLE(to) \
{ \
	if (to.mChecked == false) \
	{ \
		*lIndexData++ = to.mV0 + mParentVertexCount; \
		*lIndexData++ = to.mV1 + mParentVertexCount; \
		*lIndexData++ = to.mV2 + mParentVertexCount; \
		++mTriangleCount; \
		to.mChecked = true; \
	} \
}

#define MACRO_ADDBACKCAPTRIANGLE_FLIPPED(to) \
{ \
	if (to.mChecked == false) \
	{ \
		*lIndexData++ = to.mV2 + mParentVertexCount; \
		*lIndexData++ = to.mV1 + mParentVertexCount; \
		*lIndexData++ = to.mV0 + mParentVertexCount; \
		++mTriangleCount; \
		to.mChecked = true; \
	} \
}

void ShadowVolume::UpdateShadowVolume(const Vector3DF& pLightPos, float pShadowRange, const bool pDirectional)
{
	SetTransformation(mParentGeometry->GetTransformation());

	InitVertices();
	InitTO();

	// Transform the light position in object space.
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

	// Calculate the shadow meshes' vertex positions.
	mParentGeometry->GenerateSurfaceNormalData();
	const vtx_idx_t* lIndices = mParentGeometry->GetIndexData();
	const float* lSurfaceNormalData = mParentGeometry->GetSurfaceNormalData();
	const float* lVertexData = mParentGeometry->GetVertexData();

	// Warning: optimized code has no support for strips or other primitives!
	const unsigned lTriangleCount = mParentGeometry->GetTriangleCount();
	if (pDirectional)
	{
		// Calculate triangle orientations relative to light source.
		TriangleOrientation* lT = mTriangleOrientation;
		TriangleOrientation* lEnd = lT + lTriangleCount;
		for (; lT != lEnd; ++lT)
		{
			// Get the vector between one corner of the triangle and the light source.
			lT->mV0 = *lIndices++;
			lT->mV1 = *lIndices++;
			lT->mV2 = *lIndices++;
			lT->mChecked = false;

			// Light position is now treated as a direction instead.
			lT->mIsFrontFacing = (lLightPos.Dot(lSurfaceNormalData[0], lSurfaceNormalData[1], lSurfaceNormalData[2]) < -1e-8f);
			lSurfaceNormalData += 3;
		}

		// Move vertex twins away from lightsource.
		const float* lSource = mVertexData;
		float* lTarget = &mVertexData[mParentVertexCount*3];
		const float* lEndSource = lTarget;
		while (lSource != lEndSource)
		{
			*lTarget++ = lLightPos.x + *lSource++;
			*lTarget++ = lLightPos.y + *lSource++;
			*lTarget++ = lLightPos.z + *lSource++;
		}
	}
	else // Point or spot light.
	{
		const vtx_idx_t* lIndices = mParentGeometry->GetIndexData();
		// Calculate triangle orientations relative to light source.
		for (unsigned i = 0; i < lTriangleCount; i++)
		{
			const unsigned lTriIndex = i * 3;

			//if (mParentGeometry->GetPrimitiveType() == TBC::GeometryBase::TRIANGLES)
			{
				mTriangleOrientation[i].mV0 = lIndices[lTriIndex + 0];
				mTriangleOrientation[i].mV1 = lIndices[lTriIndex + 1];
				mTriangleOrientation[i].mV2 = lIndices[lTriIndex + 2];
			}
			/*else
			{
				uint32 lVertexIndex[3];
				mParentGeometry->GetTriangleIndices(i, lVertexIndex);
				mTriangleOrientation[i].mV0 = lVertexIndex[0];
				mTriangleOrientation[i].mV1 = lVertexIndex[1];
				mTriangleOrientation[i].mV2 = lVertexIndex[2];
			}*/
			
			Vector3DF lSurfaceNormal(lSurfaceNormalData[lTriIndex + 0],
							 lSurfaceNormalData[lTriIndex + 1],
							 lSurfaceNormalData[lTriIndex + 2]);

			// Get the vector between one corner of the triangle and the light source.
			const unsigned lIndex = mTriangleOrientation[i].mV0 * 3;
			Vector3DF lVector(lVertexData[lIndex + 0] - lLightPos.x,
						  lVertexData[lIndex + 1] - lLightPos.y,
						  lVertexData[lIndex + 2] - lLightPos.z);

			mTriangleOrientation[i].mChecked = false;
			mTriangleOrientation[i].mIsFrontFacing = (lVector.Dot(lSurfaceNormal) <= 0.0f);
		}

		const float lShadowRangeSquared = pShadowRange * pShadowRange;
		// Move vertex twins away from lightsource.
		for (unsigned i = 0; i < mParentVertexCount; i++)
		{
			// Read original vertex.
			const int lIndex0 = i * 3;
			Vector3DF lVector(mVertexData[lIndex0 + 0] - lLightPos.x,
						  mVertexData[lIndex0 + 1] - lLightPos.y,
						  mVertexData[lIndex0 + 2] - lLightPos.z);

			// Move, and write to its "twin vertex".
			if (lVector.GetLengthSquared() < lShadowRangeSquared)
			{
				lVector.Normalize(pShadowRange);

				const int lIndex1 = (i + mParentVertexCount) * 3;
				mVertexData[lIndex1 + 0] = (float)(lLightPos.x + lVector.x);
				mVertexData[lIndex1 + 1] = (float)(lLightPos.y + lVector.y);
				mVertexData[lIndex1 + 2] = (float)(lLightPos.z + lVector.z);
			}
		}
	}

	TBC::GeometryBase::SetVertexDataChanged(true);


	// Check if we actually need to update triangle definitions, or if vertex data will suffice.
	if (mTriangleCount != 0)
	{
		// We check if orientation has changed.
		const QuaternionF& lCasterOrientation = mParentGeometry->GetTransformation().GetOrientation();
		const float lOrientationDiff = (mPreviousOrientation-lCasterOrientation).GetNorm();
		if (lOrientationDiff < 0.03f)
		{
			return;	// We only need to generate triangles if orientation has changed significantly.
		}
		mPreviousOrientation = lCasterOrientation;
	}


	// Generate triangles.
	mTriangleCount = 0;
	if (mParentGeometry->GetEdgeData() == 0)
	{
		mParentGeometry->GenerateEdgeData();
	}
	const TBC::GeometryBase::Edge* lEdges = mParentGeometry->GetEdgeData();
	const unsigned lEdgeCount = mParentGeometry->GetEdgeCount();
	vtx_idx_t* lIndexData = mIndexData;
	for (unsigned i = 0; i < lEdgeCount; ++i)
	{
		const Edge& lEdge = lEdges[i];
		const TriangleOrientation* lFT = 0;	// Set this if we have a silhouette edge.
		TriangleOrientation& lT0 = mTriangleOrientation[lEdge.mTriangle[0]];
		if (lEdge.mTriangleCount == 2)
		{
			TriangleOrientation& lT1 = mTriangleOrientation[lEdge.mTriangle[1]];

			if (lT0.mIsFrontFacing)
			{
				MACRO_ADDFRONTCAPTRIANGLE(lT0);
				if (!lT1.mIsFrontFacing)
				{
					lFT = &lT0;
				}
			}
			else
			{
				MACRO_ADDBACKCAPTRIANGLE(lT0);
			}
			if (lT1.mIsFrontFacing)
			{
				MACRO_ADDFRONTCAPTRIANGLE(lT1);
				if (!lT0.mIsFrontFacing)
				{
					lFT = &lT1;
				}
			}
			else
			{
				MACRO_ADDBACKCAPTRIANGLE(lT1);
			}
		}
		else if (lT0.mIsFrontFacing)	// Assume only one triangle on edge.
		{
			// Add front+back cap triangle.
			MACRO_ADDFRONTCAPTRIANGLE(lT0);
			MACRO_ADDBACKCAPTRIANGLE_FLIPPED(lT0);
			lFT = &lT0;
		}

		if (lFT)
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
			*lIndexData++ = lV0;
			*lIndexData++ = lV1;
			*lIndexData++ = lV0 + mParentVertexCount;
			*lIndexData++ = lV1;
			*lIndexData++ = lV1 + mParentVertexCount;
			*lIndexData++ = lV0 + mParentVertexCount;

			mTriangleCount += 2;
			deb_assert(mTriangleCount < mMaxTriangleCount);
		}
	}

	TBC::GeometryBase::SetIndexDataChanged(true);
}



}
