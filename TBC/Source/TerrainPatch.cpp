/*
	Class:  TerrainPatch
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "../Include/TerrainPatch.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/Timer.h"
#include "../../Lepra/Include/Math.h"

#include "../../Lepra/Include/LepraAssert.h"

namespace TBC
{

int   TerrainPatch::smNumPatches = 0;
float TerrainPatch::smPatchUnitSize = 1.0f;
int   TerrainPatch::smPatchRes = (1 << 4);
int   TerrainPatch::smPatchSizeMultiplier = 3;

void TerrainPatch::SetDimensions(int pPatchResLog2,
				 float pPatchUnitSize,
				 int pPatchSizeMultiplier)
{
	deb_assert(pPatchResLog2 >= 0 && pPatchResLog2 < 16);
	deb_assert(pPatchUnitSize > 0.0f);

	if (smNumPatches == 0)
	{
		smPatchRes = (1 << pPatchResLog2);
		smPatchUnitSize = pPatchUnitSize;
		smPatchSizeMultiplier = pPatchSizeMultiplier;
	}
}


TerrainPatch::TerrainPatch(const Vector2D<int>& pPosition,
			   uint8* pHoleMap,
			   unsigned int pSizeExponent,
			   unsigned int pHiResEdgeFlags,
			   float pWestU1, float pEastU1, float pSouthV1, float pNorthV1,
			   float pWestU2, float pEastU2, float pSouthV2, float pNorthV2):
	mVertexCount(0),
	mTriangleCount(0),
	mSizeMultiplier(Math::Pow(smPatchSizeMultiplier, pSizeExponent)),
	mHiResEdgeFlags(pHiResEdgeFlags),
	mNorthEdgeIndex(0),
	mSouthEdgeIndex(0),
	mEastEdgeIndex(0),
	mWestEdgeIndex(0),
	mVertexData(0),
	mIndexData(0),
	mUnitPosition(pPosition)
{
	mUVData[0] = 0;
	mUVData[1] = 0;

	float lPatchSize = (float)GetPatchSize();
	mSouthWestCorner.Set((float)pPosition.x * smPatchUnitSize, (float)pPosition.y * smPatchUnitSize),
	mNorthEastCorner.Set(mSouthWestCorner.x + lPatchSize, mSouthWestCorner.y + lPatchSize);

	mVertexCount = (smPatchRes + 1) * (smPatchRes + 1) +
			smPatchRes * smPatchRes;
	mTriangleCount = smPatchRes * smPatchRes * 4;

	mNorthEdgeIndex = mVertexCount;
	mSouthEdgeIndex = mVertexCount;
	mEastEdgeIndex  = mVertexCount;
	mWestEdgeIndex  = mVertexCount;

	int lEdgeCount = 0;
	if (CheckFlag(mHiResEdgeFlags, SOUTH_EDGE) == true)
	{
		lEdgeCount++;
		mNorthEdgeIndex += smPatchRes * (smPatchSizeMultiplier - 1);
		mEastEdgeIndex  += smPatchRes * (smPatchSizeMultiplier - 1);
		mWestEdgeIndex  += smPatchRes * (smPatchSizeMultiplier - 1);
	}
	if (CheckFlag(mHiResEdgeFlags, NORTH_EDGE) == true)
	{
		lEdgeCount++;
		mEastEdgeIndex  += smPatchRes * (smPatchSizeMultiplier - 1);
		mWestEdgeIndex  += smPatchRes * (smPatchSizeMultiplier - 1);
	}
	if (CheckFlag(mHiResEdgeFlags, EAST_EDGE) == true)
	{
		lEdgeCount++;
		mWestEdgeIndex  += smPatchRes * (smPatchSizeMultiplier - 1);
	}
	if (CheckFlag(mHiResEdgeFlags, WEST_EDGE) == true)
	{
		lEdgeCount++;
	}

	// For each square on each high-res edge, there are smPatchSizeMultiplier - 1,
	// extra vertices and triangles.
	mVertexCount += lEdgeCount * smPatchRes * (smPatchSizeMultiplier - 1);
	mTriangleCount += lEdgeCount * smPatchRes * (smPatchSizeMultiplier - 1);

	mVertexData = new float[mVertexCount * 3];
	mUVData[0] = new float[mVertexCount * 2];
	mUVData[1] = new float[mVertexCount * 2];
	mIndexData = new vtx_idx_t[mTriangleCount * 3];

	//SetVertexData(pVertexData);
	SetToFlatTerrainPatch();
	GenerateIndexData(pHoleMap);
	GenerateUVData(pWestU1, pEastU1, pSouthV1, pNorthV1,
		       pWestU2, pEastU2, pSouthV2, pNorthV2);
	//GeometryBase::GenerateVertexNormalData();

	++smNumPatches;
}

TerrainPatch::~TerrainPatch()
{
	delete[] mUVData[0];
	delete[] mUVData[1];
	delete[] mVertexData;
	delete[] mIndexData;

	--smNumPatches;
}

void TerrainPatch::GenerateUVData(float pWestU1, float pEastU1, float pSouthV1, float pNorthV1,
				  float pWestU2, float pEastU2, float pSouthV2, float pNorthV2)
{
	float lU1Step = (pEastU1 - pWestU1) / (float)smPatchRes;
	float lV1Step = (pNorthV1 - pSouthV1) / (float)smPatchRes;
	float lU2Step = (pEastU2 - pWestU1) / (float)smPatchRes;
	float lV2Step = (pNorthV2 - pSouthV1) / (float)smPatchRes;

	float lV1 = pSouthV1;
	float lV2 = pSouthV2;
	for (int y = 0; y < smPatchRes + 1; y++)
	{
		float lU1 = pWestU1;
		float lU2 = pWestU2;
		for (int x = 0; x < smPatchRes + 1; x++)
		{
			int lUVIndex = (y * (smPatchRes + 1) + x) * 2;
			mUVData[0][lUVIndex + 0] = lU1;
			mUVData[0][lUVIndex + 1] = lV1;
			mUVData[1][lUVIndex + 0] = lU2;
			mUVData[1][lUVIndex + 1] = lV2;

			if (y < smPatchRes && x < smPatchRes)
			{
				lUVIndex = ((smPatchRes + 1) * (smPatchRes + 1) +
						(y * smPatchRes + x)) * 2;
				mUVData[0][lUVIndex + 0] = lU1 + lU1Step * 0.5f;
				mUVData[0][lUVIndex + 1] = lV1 + lV1Step * 0.5f;
				mUVData[1][lUVIndex + 0] = lU2 + lU2Step * 0.5f;
				mUVData[1][lUVIndex + 1] = lV2 + lV2Step * 0.5f;
			}

			lU1 += lU1Step;
			lU2 += lU2Step;
		}

		lV1 += lV1Step;
		lV2 += lV2Step;
	}

	int lUVIndex = ((smPatchRes + 1) * (smPatchRes + 1) + smPatchRes * smPatchRes) * 2;
	float lU1SmallStep = lU1Step / (float)smPatchSizeMultiplier;
	float lV1SmallStep = lV1Step / (float)smPatchSizeMultiplier;
	float lU2SmallStep = lU2Step / (float)smPatchSizeMultiplier;
	float lV2SmallStep = lV2Step / (float)smPatchSizeMultiplier;
	if (CheckFlag(mHiResEdgeFlags, SOUTH_EDGE) == true)
	{
		float lU1 = pWestU1 + lU1SmallStep;
		float lU2 = pWestU2 + lU2SmallStep;
		for (int i = 0; i < smPatchRes; i++)
		{
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				mUVData[0][lUVIndex + 0] = lU1;
				mUVData[0][lUVIndex + 1] = pSouthV1;
				mUVData[1][lUVIndex + 0] = lU2;
				mUVData[1][lUVIndex + 1] = pSouthV2;
				lU1 += lU1SmallStep;
				lU2 += lU2SmallStep;
				lUVIndex += 2;
			}
			lU1 += lU1SmallStep;
			lU2 += lU2SmallStep;
		}
	}
	if (CheckFlag(mHiResEdgeFlags, NORTH_EDGE) == true)
	{
		float lU1 = pWestU1 + lU1SmallStep;
		float lU2 = pWestU2 + lU2SmallStep;
		for (int i = 0; i < smPatchRes; i++)
		{
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				mUVData[0][lUVIndex + 0] = lU1;
				mUVData[0][lUVIndex + 1] = pNorthV1;
				mUVData[1][lUVIndex + 0] = lU2;
				mUVData[1][lUVIndex + 1] = pNorthV2;
				lU1 += lU1SmallStep;
				lU2 += lU2SmallStep;
				lUVIndex += 2;
			}
			lU1 += lU1SmallStep;
			lU2 += lU2SmallStep;
		}
	}
	if (CheckFlag(mHiResEdgeFlags, WEST_EDGE) == true)
	{
		float lV1 = pSouthV1 + lV1SmallStep;
		float lV2 = pSouthV2 + lV2SmallStep;
		for (int i = 0; i < smPatchRes; i++)
		{
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				mUVData[0][lUVIndex + 0] = pWestU1;
				mUVData[0][lUVIndex + 1] = lV1;
				mUVData[1][lUVIndex + 0] = pWestU2;
				mUVData[1][lUVIndex + 1] = lV2;
				lV1 += lV1SmallStep;
				lV2 += lV2SmallStep;
				lUVIndex += 2;
			}
			lV1 += lV1SmallStep;
			lV2 += lV2SmallStep;
		}
	}
	if (CheckFlag(mHiResEdgeFlags, EAST_EDGE) == true)
	{
		float lV1 = pSouthV1 + lV1SmallStep;
		float lV2 = pSouthV2 + lV2SmallStep;
		for (int i = 0; i < smPatchRes; i++)
		{
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				mUVData[0][lUVIndex + 0] = pEastU1;
				mUVData[0][lUVIndex + 1] = lV1;
				mUVData[1][lUVIndex + 0] = pEastU2;
				mUVData[1][lUVIndex + 1] = lV2;
				lV1 += lV1SmallStep;
				lV2 += lV2SmallStep;
				lUVIndex += 2;
			}
			lV1 += lV1SmallStep;
			lV2 += lV2SmallStep;
		}
	}
}

void TerrainPatch::GenerateIndexData(uint8* pHoleMap)
{
	int lMidVertexStartIndex = (smPatchRes + 1) * (smPatchRes + 1);

	unsigned int lTriIndex = 0;
	for (int y = 0; y < smPatchRes; ++y)
	{
		int lTopTriCount = 1;
		int lBottomTriCount = 1;
		if (y == 0 && CheckFlag(mHiResEdgeFlags, SOUTH_EDGE) == true)
		{
			lBottomTriCount = smPatchSizeMultiplier;
		}
		if (y == smPatchRes - 1 && CheckFlag(mHiResEdgeFlags, NORTH_EDGE) == true)
		{
			lTopTriCount = smPatchSizeMultiplier;
		}

		for (int x = 0; x < smPatchRes; ++x)
		{
			if (pHoleMap == 0 || pHoleMap[y * smPatchRes + x] != 0)
			{
				int lLeftTriCount = 1;
				int lRightTriCount = 1;
				if (x == 0 && CheckFlag(mHiResEdgeFlags, WEST_EDGE) == true)
				{
					lLeftTriCount = smPatchSizeMultiplier;
				}
				if (x == smPatchRes - 1 && CheckFlag(mHiResEdgeFlags, EAST_EDGE) == true)
				{
					lRightTriCount = smPatchSizeMultiplier;
				}

				int lBLVertex = y * (smPatchRes + 1) + x;
				int lBRVertex = lBLVertex + 1;
				int lTLVertex = (y + 1) * (smPatchRes + 1) + x;
				int lTRVertex = lTLVertex + 1;
				int lCenterVertex = lMidVertexStartIndex + y * smPatchRes + x;

				// Create top triangles.
				AddTriangles(lTopTriCount, mNorthEdgeIndex + x * (smPatchSizeMultiplier - 1),
					lCenterVertex, lTLVertex, lTRVertex, lTriIndex, true);

				// Create bottom triangles.
				AddTriangles(lBottomTriCount, mSouthEdgeIndex + x * (smPatchSizeMultiplier - 1),
					lCenterVertex, lBLVertex, lBRVertex, lTriIndex, false);

				// Create left triangles.
				AddTriangles(lLeftTriCount, mWestEdgeIndex + y * (smPatchSizeMultiplier - 1),
					lCenterVertex, lBLVertex, lTLVertex, lTriIndex, true);

				// Create right triangles.
				AddTriangles(lRightTriCount, mEastEdgeIndex + y * (smPatchSizeMultiplier - 1),
					lCenterVertex, lBRVertex, lTRVertex, lTriIndex, false);
			}
		}
	}

	GeometryBase::SetIndexDataChanged(true);
	deb_assert((lTriIndex % 3) == 0 && (lTriIndex / 3) == mTriangleCount);
}

void TerrainPatch::AddTriangles(int pTriCount, int pExtraVertexStartIndex,
				uint32 pCenter, uint32 pV1, uint32 pV2, 
				unsigned int& pTriIndex, bool pSwap)
{
	int lV1 = pV1;
	int lV2 = pTriCount == 1 ? pV2 : pExtraVertexStartIndex;

	for (int i = 0; i < pTriCount; i++)
	{
		if (i > 0)
		{
			lV1 = pExtraVertexStartIndex + i - 1;
			lV2 = lV1 + 1;
		}
		if (i == pTriCount - 1)
		{
			lV2 = pV2;
		}

		mIndexData[pTriIndex + 0] = pCenter;

		if (pSwap == true)
		{
			mIndexData[pTriIndex + 1] = lV2;
			mIndexData[pTriIndex + 2] = lV1;
		}
		else
		{
			mIndexData[pTriIndex + 1] = lV1;
			mIndexData[pTriIndex + 2] = lV2;
		}
		pTriIndex += 3;
	}
}

void TerrainPatch::IterateOverPatch(const Modifier& pModifier, int pMinXIndex, int pMaxXIndex, int pMinYIndex, int pMaxYIndex)
{
	float lScaleX = (mNorthEastCorner.x - mSouthWestCorner.x) / (float)smPatchRes;
	float lScaleY = (mNorthEastCorner.y - mSouthWestCorner.y) / (float)smPatchRes;

	int x;
	int y;

	// Corners...
	float lWorldY = mSouthWestCorner.y + lScaleY * (float)pMinYIndex;
	for (y = pMinYIndex; y < pMaxYIndex; ++y)
	{
		float lWorldX = mSouthWestCorner.x + lScaleX * (float)pMinXIndex;
		for (x = pMinXIndex; x < pMaxXIndex; ++x)
		{
			int lVertexIndex = (y * (smPatchRes + 1) + x) * 3;
			// TRICKY: Typecasting float-array to Vector3DF is risky if the implementation
			// of Vector3D changes.
			pModifier.ModifyVertex(Vector2DF(lWorldX, lWorldY), *((Vector3DF*)&mVertexData[lVertexIndex]));
			lWorldX += lScaleX;
		}

		lWorldY += lScaleY;
	}

	// Mid points...
	lWorldY = mSouthWestCorner.y + lScaleY * ((float)pMinYIndex + 0.5f);
	for (y = pMinYIndex; y < pMaxYIndex - 1; ++y)
	{
		float lWorldX = mSouthWestCorner.x + lScaleX * ((float)pMinXIndex + 0.5f);
		for (x = pMinXIndex; x < pMaxXIndex - 1; ++x)
		{
			int lVertexIndex = ((smPatchRes + 1) * (smPatchRes + 1) + y * smPatchRes + x) * 3;
			// TRICKY: Typecasting float-array to Vector3DF is risky if the implementation
			// of Vector3D changes.
			pModifier.ModifyVertex(Vector2DF(lWorldX, lWorldY), *((Vector3DF*)&mVertexData[lVertexIndex]));
			lWorldX += lScaleX;
		}

		lWorldY += lScaleY;
	}

	// Extra vertices...
	float lSmallXStep = lScaleX / (float)smPatchSizeMultiplier;
	float lSmallYStep = lScaleY / (float)smPatchSizeMultiplier;
	if (CheckFlag(mHiResEdgeFlags, SOUTH_EDGE) == true && pMinYIndex == 0)
	{
		float lWorldX = mSouthWestCorner.x + lSmallXStep;
		for (int i = pMinXIndex; i < pMaxXIndex - 1; i++)
		{
			int lVertexIndex = (mSouthEdgeIndex + i * (smPatchSizeMultiplier - 1)) * 3;
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				// TRICKY: Typecasting float-array to Vector3DF is risky if the implementation
				// of Vector3D changes.
				pModifier.ModifyVertex(Vector2DF(lWorldX, mSouthWestCorner.y), *((Vector3DF*)&mVertexData[lVertexIndex]));
				lWorldX += lSmallXStep;
				lVertexIndex += 3;
			}
			lWorldX += lSmallXStep;
		}
	}
	if (CheckFlag(mHiResEdgeFlags, NORTH_EDGE) == true && pMaxYIndex == smPatchRes + 1)
	{
		float lWorldX = mSouthWestCorner.x + lSmallXStep;
		for (int i = pMinXIndex; i < pMaxXIndex - 1; i++)
		{
			int lVertexIndex = (mNorthEdgeIndex + i * (smPatchSizeMultiplier - 1)) * 3;
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				// TRICKY: Typecasting float-array to Vector3DF is risky if the implementation
				// of Vector3D changes.
				pModifier.ModifyVertex(Vector2DF(lWorldX, mNorthEastCorner.y), *((Vector3DF*)&mVertexData[lVertexIndex]));
				lWorldX += lSmallXStep;
				lVertexIndex += 3;
			}
			lWorldX += lSmallXStep;
		}
	}
	if (CheckFlag(mHiResEdgeFlags, WEST_EDGE) == true && pMinXIndex == 0)
	{
		float lWorldY = mSouthWestCorner.y + lSmallYStep;
		for (int i = pMinYIndex; i < pMaxYIndex - 1; i++)
		{
			int lVertexIndex = (mWestEdgeIndex + i * (smPatchSizeMultiplier - 1)) * 3;
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				// TRICKY: Typecasting float-array to Vector3DF is risky if the implementation
				// of Vector3D changes.
				pModifier.ModifyVertex(Vector2DF(mSouthWestCorner.x, lWorldY), *((Vector3DF*)&mVertexData[lVertexIndex]));
				lWorldY += lSmallYStep;
				lVertexIndex += 3;
			}
			lWorldY += lSmallYStep;
		}
	}

	if (CheckFlag(mHiResEdgeFlags, EAST_EDGE) == true && pMaxXIndex == smPatchRes + 1)
	{
		float lWorldY = mSouthWestCorner.y + lSmallYStep;
		for (int i = pMinYIndex; i < pMaxYIndex - 1; i++)
		{
			int lVertexIndex = (mEastEdgeIndex + i * (smPatchSizeMultiplier - 1)) * 3;
			for (int j = 1; j < smPatchSizeMultiplier; j++)
			{
				// TRICKY: Typecasting float-array to Vector3DF is risky if the implementation
				// of Vector3D changes.
				pModifier.ModifyVertex(Vector2DF(mNorthEastCorner.x, lWorldY), *((Vector3DF*)&mVertexData[lVertexIndex]));
				lWorldY += lSmallYStep;
				lVertexIndex += 3;
			}
			lWorldY += lSmallYStep;
		}
	}
}

class FlatModifier : public TerrainPatch::Modifier
{
public:
	void ModifyVertex(const Vector2DF& pWorldFlatPos, Vector3DF& pVertex) const
	{
		pVertex.x = pWorldFlatPos.x;
		pVertex.y = pWorldFlatPos.y;
		pVertex.z = 0;
	}
};

void TerrainPatch::SetToFlatTerrainPatch()
{
	FlatModifier lModifier;
	IterateOverPatch(lModifier, 0, GetVertexRes(), 0, GetVertexRes());
}

void TerrainPatch::SetFlatCallback(float pWorldX, float pWorldY, Vector3DF& pCurrentPoint)
{
	pCurrentPoint.x = pWorldX;
	pCurrentPoint.y = pWorldY;
	pCurrentPoint.z = 0;
}

/*
void TerrainPatch::SetVertexData(const Vector3DF* pVertexData)
{
	int x;
	int y;
	
	// Setup regular grid vertices.
	for (y = 0; y < smPatchRes + 1; ++y)
	{
		for (x = 0; x < smPatchRes + 1; ++x)
		{
			int lDstVertexIndex = (y * (smPatchRes + 1) + x) * 3;
			int lSrcVertexIndex = (y * (smPatchRes + 1) + x) * 2;

			mVertexData[lDstVertexIndex + 0] = pVertexData[lSrcVertexIndex].x;
			mVertexData[lDstVertexIndex + 1] = pVertexData[lSrcVertexIndex].y;
			mVertexData[lDstVertexIndex + 2] = pVertexData[lSrcVertexIndex].z;
		}
	}

	// Setup center vertices.
	int lDstStartIndex = (smPatchRes + 1) * (smPatchRes + 1);
	for (y = 0; y < smPatchRes; ++y)
	{
		for (x = 0; x < smPatchRes; ++x)
		{
			int lDstVertexIndex = (lDstStartIndex + y * smPatchRes + x) * 3;
			int lSrcVertexIndex = ((y * (smPatchRes + 1) + x) * 2 + 1);

			mVertexData[lDstVertexIndex + 0] = pVertexData[lSrcVertexIndex].x;
			mVertexData[lDstVertexIndex + 1] = pVertexData[lSrcVertexIndex].y;
			mVertexData[lDstVertexIndex + 2] = pVertexData[lSrcVertexIndex].z;
		}
	}

	// Setup extra edge vertices.
	if (CheckFlag(mHiResEdgeFlags, SOUTH_EDGE) == true)
	{
		SetEdgeVertexData(pVertexData, mSouthEdgeIndex * 3, 0, 2);
	}
	if (CheckFlag(mHiResEdgeFlags, NORTH_EDGE) == true)
	{
		SetEdgeVertexData(pVertexData, mNorthEdgeIndex * 3, smPatchRes * (smPatchRes + 1) * 2, 2);
	}
	if (CheckFlag(mHiResEdgeFlags, WEST_EDGE) == true)
	{
		SetEdgeVertexData(pVertexData, mWestEdgeIndex * 3, 0, (smPatchRes + 1) * 2);
	}
	if (CheckFlag(mHiResEdgeFlags, EAST_EDGE) == true)
	{
		SetEdgeVertexData(pVertexData, mEastEdgeIndex * 3, (smPatchRes + 1) * 2, (smPatchRes + 1) * 2);
	}
}

void TerrainPatch::SetEdgeVertexData(const Vector3DF* pVertexData, int pDstVertexIndex, int pSrcVertexStartIndex, int pPitch)
{
	for (int x = 0; x < smPatchRes; ++x)
	{
		int lSrcVertexIndex = (pSrcVertexStartIndex + x * pPitch);

		int lMid = (smPatchSizeMultiplier - 1) / 2;
		int i;
		for (i = 0; i < lMid; i++)
		{
			float t = (float)(i + 1) / (float)smPatchSizeMultiplier;
			mVertexData[pDstVertexIndex + 0] = Math::Lerp(pVertexData[lSrcVertexIndex].x, pVertexData[lSrcVertexIndex + pPitch].x, t);// + mSouthWestCorner.x;
			mVertexData[pDstVertexIndex + 1] = Math::Lerp(pVertexData[lSrcVertexIndex].y, pVertexData[lSrcVertexIndex + pPitch].y, t);// + mSouthWestCorner.y;
			mVertexData[pDstVertexIndex + 2] = Math::Lerp(pVertexData[lSrcVertexIndex].z, pVertexData[lSrcVertexIndex + pPitch].z, t);
			pDstVertexIndex += 3;
		}
		lSrcVertexIndex += pPitch;
		for (i = lMid; i < smPatchSizeMultiplier - 1; i++)
		{
			float t = (float)(i + 1) / (float)smPatchSizeMultiplier;
			mVertexData[pDstVertexIndex + 0] = Math::Lerp(pVertexData[lSrcVertexIndex].x, pVertexData[lSrcVertexIndex + pPitch].x, t);// + mSouthWestCorner.x;
			mVertexData[pDstVertexIndex + 1] = Math::Lerp(pVertexData[lSrcVertexIndex].y, pVertexData[lSrcVertexIndex + pPitch].y, t);// + mSouthWestCorner.y;
			mVertexData[pDstVertexIndex + 2] = Math::Lerp(pVertexData[lSrcVertexIndex].z, pVertexData[lSrcVertexIndex + pPitch].z, t);
			pDstVertexIndex += 3;
		}
	}
}
*/

float* TerrainPatch::GetSouthEdgeVertex(int pIndex)
{
	return GetEdgeVertex(pIndex, SOUTH_EDGE, 0, mSouthEdgeIndex, 1, mVertexData);
}

float* TerrainPatch::GetNorthEdgeVertex(int pIndex)
{
	return GetEdgeVertex(pIndex, NORTH_EDGE, (smPatchRes + 1) * smPatchRes, mNorthEdgeIndex, 1, mVertexData);
}

float* TerrainPatch::GetWestEdgeVertex(int pIndex)
{
	return GetEdgeVertex(pIndex, WEST_EDGE, 0, mWestEdgeIndex, smPatchRes + 1, mVertexData);
}

float* TerrainPatch::GetEastEdgeVertex(int pIndex)
{
	return GetEdgeVertex(pIndex, EAST_EDGE, smPatchRes + 1, mEastEdgeIndex, smPatchRes + 1, mVertexData);
}

float* TerrainPatch::GetSouthEdgeNormal(int pIndex)
{
	return GetEdgeVertex(pIndex, SOUTH_EDGE, 0, mSouthEdgeIndex, 1, GetNormalData());
}

float* TerrainPatch::GetNorthEdgeNormal(int pIndex)
{
	return GetEdgeVertex(pIndex, NORTH_EDGE, (smPatchRes + 1) * smPatchRes, mNorthEdgeIndex, 1, GetNormalData());
}

float* TerrainPatch::GetWestEdgeNormal(int pIndex)
{
	return GetEdgeVertex(pIndex, WEST_EDGE, 0, mWestEdgeIndex, smPatchRes + 1, GetNormalData());
}

float* TerrainPatch::GetEastEdgeNormal(int pIndex)
{
	return GetEdgeVertex(pIndex, EAST_EDGE, smPatchRes + 1, mEastEdgeIndex, smPatchRes + 1, GetNormalData());
}

float* TerrainPatch::GetEdgeVertex(int pIndex, unsigned int pEdgeFlag, int pRegularStartIndex, int pExtraStartIndex, int pPitch, float* pVertexData)
{
	if (CheckFlag(mHiResEdgeFlags, pEdgeFlag) == false)
	{
		deb_assert(pIndex >= 0 && pIndex < smPatchRes + 1);
		return &pVertexData[(pRegularStartIndex + pIndex * pPitch) * 3];
	}
	else
	{
		deb_assert(pIndex >= 0 && pIndex < smPatchRes * smPatchSizeMultiplier + 1);

		// Index of previous regular vertex (not an "extra vertex").
		int lRegularVertexIndex = pIndex / smPatchSizeMultiplier;
		if (pIndex % smPatchSizeMultiplier == 0)
		{
			// Stepping on the regular vertices.
			return &pVertexData[(pRegularStartIndex + lRegularVertexIndex * pPitch) * 3];
		}
		else
		{
			return &pVertexData[(pExtraStartIndex + pIndex - lRegularVertexIndex - 1) * 3];
		}
	}
}

void TerrainPatch::ShareVerticesWithNorthNeighbour(TerrainPatch& pNorthNeighbour)
{
	ProcessSharedEdge(pNorthNeighbour, NORTH_EDGE, SOUTH_EDGE, 
		&TerrainPatch::AssertNorthAlignment, &TerrainPatch::GetHorizontalDisplacement, 
		&TerrainPatch::GetNorthEdgeVertex, &TerrainPatch::GetSouthEdgeVertex, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithSouthNeighbour);
}

void TerrainPatch::ShareVerticesWithSouthNeighbour(TerrainPatch& pSouthNeighbour)
{
	ProcessSharedEdge(pSouthNeighbour, SOUTH_EDGE, NORTH_EDGE, 
		&TerrainPatch::AssertSouthAlignment, &TerrainPatch::GetHorizontalDisplacement, 
		&TerrainPatch::GetSouthEdgeVertex, &TerrainPatch::GetNorthEdgeVertex, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithNorthNeighbour);
}

void TerrainPatch::ShareVerticesWithEastNeighbour(TerrainPatch& pEastNeighbour)
{
	ProcessSharedEdge(pEastNeighbour, EAST_EDGE, WEST_EDGE, 
		&TerrainPatch::AssertEastAlignment, &TerrainPatch::GetVerticalDisplacement, 
		&TerrainPatch::GetEastEdgeVertex, &TerrainPatch::GetWestEdgeVertex, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithWestNeighbour);
}

void TerrainPatch::ShareVerticesWithWestNeighbour(TerrainPatch& pWestNeighbour)
{
	ProcessSharedEdge(pWestNeighbour, WEST_EDGE, EAST_EDGE, 
		&TerrainPatch::AssertWestAlignment, &TerrainPatch::GetVerticalDisplacement, 
		&TerrainPatch::GetWestEdgeVertex, &TerrainPatch::GetEastEdgeVertex, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithEastNeighbour);
}

void TerrainPatch::ShareNormalsWithNorthNeighbour(TerrainPatch& pNorthNeighbour)
{
	ProcessSharedEdge(pNorthNeighbour, NORTH_EDGE, SOUTH_EDGE, 
		&TerrainPatch::AssertNorthAlignment, &TerrainPatch::GetHorizontalDisplacement, 
		&TerrainPatch::GetNorthEdgeNormal, &TerrainPatch::GetSouthEdgeNormal, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithSouthNeighbour);
}

void TerrainPatch::ShareNormalsWithSouthNeighbour(TerrainPatch& pSouthNeighbour)
{
	ProcessSharedEdge(pSouthNeighbour, SOUTH_EDGE, NORTH_EDGE, 
		&TerrainPatch::AssertSouthAlignment, &TerrainPatch::GetHorizontalDisplacement, 
		&TerrainPatch::GetSouthEdgeNormal, &TerrainPatch::GetNorthEdgeNormal, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithNorthNeighbour);
}

void TerrainPatch::ShareNormalsWithEastNeighbour(TerrainPatch& pEastNeighbour)
{
	ProcessSharedEdge(pEastNeighbour, EAST_EDGE, WEST_EDGE, 
		&TerrainPatch::AssertEastAlignment, &TerrainPatch::GetVerticalDisplacement, 
		&TerrainPatch::GetEastEdgeNormal, &TerrainPatch::GetWestEdgeNormal, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithWestNeighbour);
}

void TerrainPatch::ShareNormalsWithWestNeighbour(TerrainPatch& pWestNeighbour)
{
	ProcessSharedEdge(pWestNeighbour, WEST_EDGE, EAST_EDGE, 
		&TerrainPatch::AssertWestAlignment, &TerrainPatch::GetVerticalDisplacement, 
		&TerrainPatch::GetWestEdgeNormal, &TerrainPatch::GetEastEdgeNormal, 
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithEastNeighbour);
}

void TerrainPatch::MergeNormalsWithNorthNeighbour(TerrainPatch& pNorthNeighbour)
{
	ProcessSharedEdge(pNorthNeighbour, NORTH_EDGE, SOUTH_EDGE, 
		&TerrainPatch::AssertNorthAlignment, &TerrainPatch::GetHorizontalDisplacement, 
		&TerrainPatch::GetNorthEdgeNormal, &TerrainPatch::GetSouthEdgeNormal, 
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithSouthNeighbour);
}

void TerrainPatch::MergeNormalsWithSouthNeighbour(TerrainPatch& pSouthNeighbour)
{
	ProcessSharedEdge(pSouthNeighbour, SOUTH_EDGE, NORTH_EDGE, 
		&TerrainPatch::AssertSouthAlignment, &TerrainPatch::GetHorizontalDisplacement, 
		&TerrainPatch::GetSouthEdgeNormal, &TerrainPatch::GetNorthEdgeNormal, 
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithNorthNeighbour);
}

void TerrainPatch::MergeNormalsWithEastNeighbour(TerrainPatch& pEastNeighbour)
{
	ProcessSharedEdge(pEastNeighbour, EAST_EDGE, WEST_EDGE, 
		&TerrainPatch::AssertEastAlignment, &TerrainPatch::GetVerticalDisplacement, 
		&TerrainPatch::GetEastEdgeNormal, &TerrainPatch::GetWestEdgeNormal, 
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithWestNeighbour);
}

void TerrainPatch::MergeNormalsWithWestNeighbour(TerrainPatch& pWestNeighbour)
{
	ProcessSharedEdge(pWestNeighbour, WEST_EDGE, EAST_EDGE, 
		&TerrainPatch::AssertWestAlignment, &TerrainPatch::GetVerticalDisplacement, 
		&TerrainPatch::GetWestEdgeNormal, &TerrainPatch::GetEastEdgeNormal, 
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithEastNeighbour);
}


void TerrainPatch::ProcessSharedEdge(TerrainPatch& pNeighbour,
				     int pEdge, int pOppositeEdge,
				     void (TerrainPatch::*pAssertAlignment)(const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&),
				     int (TerrainPatch::*pGetVertexDisplacement)(const Vector2D<int>&, const Vector2D<int>&),
				     float* (TerrainPatch::*pGetEdgeElement)(int pIndex),
				     float* (TerrainPatch::*pGetOppositeEdgeElement)(int pIndex),
				     void (TerrainPatch::*pProcessElement)(float* pDst, float* pSrs),
				     void (TerrainPatch::*pOppositeShareEdgeFunction)(TerrainPatch&))
{
	// Not used in release compilation.
	pEdge;
	pOppositeEdge;

	Vector2D<int> lSouthWest(GetSouthWestUnitPos());
	Vector2D<int> lNorthEast(GetNorthEastUnitPos());
	Vector2D<int> lSouthWestNeighbour(pNeighbour.GetSouthWestUnitPos());
	Vector2D<int> lNorthEastNeighbour(pNeighbour.GetNorthEastUnitPos());

	(this->*pAssertAlignment)(lSouthWest, lNorthEast, lSouthWestNeighbour, lNorthEastNeighbour);

	// The size can only differ with one step.
	deb_assert(mSizeMultiplier == pNeighbour.mSizeMultiplier ||
	       mSizeMultiplier == pNeighbour.mSizeMultiplier - smPatchSizeMultiplier ||
	       mSizeMultiplier == pNeighbour.mSizeMultiplier + smPatchSizeMultiplier);

	// The big patch adapts to the smaller patch.
	if (mSizeMultiplier >= pNeighbour.mSizeMultiplier)
	{
		deb_assert(pNeighbour.CheckFlag(pNeighbour.mHiResEdgeFlags, pOppositeEdge) == false &&
			((mSizeMultiplier == pNeighbour.mSizeMultiplier && CheckFlag(mHiResEdgeFlags, pEdge) == false) ||
			CheckFlag(mHiResEdgeFlags, pEdge) == true));

		int lVertexCount = pNeighbour.GetVertexRes();
		int lStartIndex = (this->*pGetVertexDisplacement)(lSouthWest, lSouthWestNeighbour) * lVertexCount;
		int lEndIndex = lStartIndex + lVertexCount;

		for (int i = lStartIndex; i < lEndIndex; i++)
		{
			float* lDst = (this->*pGetEdgeElement)(i);
			float* lSrc = (pNeighbour.*pGetOppositeEdgeElement)(i - lStartIndex);

			(this->*pProcessElement)(lDst, lSrc);
		}
	}
	else
	{
		(pNeighbour.*pOppositeShareEdgeFunction)(*this);
	}
}

void TerrainPatch::AssertNorthAlignment(const Vector2D<int>&, const Vector2D<int>& pNorthEast, const Vector2D<int>& pSouthWestNeighbour,	const Vector2D<int>&)
{
	// TODO: Implement a check along the x-axis!?
	deb_assert(pNorthEast.y == pSouthWestNeighbour.y);
	pNorthEast;
	pSouthWestNeighbour;
}

void TerrainPatch::AssertSouthAlignment(const Vector2D<int>& pSouthWest, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>& pNorthEastNeighbour)
{
	// TODO: Implement a check along the x-axis!?
	deb_assert(pSouthWest.y == pNorthEastNeighbour.y);
	pSouthWest;
	pNorthEastNeighbour;
}

void TerrainPatch::AssertEastAlignment(const Vector2D<int>&, const Vector2D<int>& pNorthEast, const Vector2D<int>& pSouthWestNeighbour, const Vector2D<int>&)
{
	// TODO: Implement a check along the y-axis!?
	deb_assert(pNorthEast.x == pSouthWestNeighbour.x);
	pNorthEast;
	pSouthWestNeighbour;
}

void TerrainPatch::AssertWestAlignment(const Vector2D<int>& pSouthWest, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>& pNorthEastNeighbour)
{
	// TODO: Implement a check along the y-axis!?
	deb_assert(pSouthWest.x == pNorthEastNeighbour.x);
	pSouthWest;
	pNorthEastNeighbour;
}

int TerrainPatch::GetHorizontalDisplacement(const Vector2D<int>& pSouthWest, const Vector2D<int>& pSouthWestNeighbour)
{
	return (pSouthWestNeighbour.x - pSouthWest.x);
}

int TerrainPatch::GetVerticalDisplacement(const Vector2D<int>& pSouthWest, const Vector2D<int>& pSouthWestNeighbour)
{
	return (pSouthWestNeighbour.y - pSouthWest.y);
}

void TerrainPatch::CopyElement(float* pDst, float* pSrc)
{
	pDst[0] = pSrc[0];
	pDst[1] = pSrc[1];
	pDst[2] = pSrc[2];
}

void TerrainPatch::SetElementsToMean(float* pDst, float* pSrc)
{
	Vector3DF lV((pDst[0] + pSrc[0]) * 0.5f,
			     (pDst[1] + pSrc[1]) * 0.5f,
			     (pDst[2] + pSrc[2]) * 0.5f);
	lV.Normalize();
	pDst[0] = pSrc[0] = lV.x;
	pDst[1] = pSrc[1] = lV.y;
	pDst[2] = pSrc[2] = lV.z;
}

Vector2D<int> TerrainPatch::GetSouthWestUnitPos() const
{
	return mUnitPosition;
}

Vector2D<int> TerrainPatch::GetNorthEastUnitPos() const
{
	return mUnitPosition + Vector2D<int>(mSizeMultiplier, mSizeMultiplier);
}

const Vector2DF& TerrainPatch::GetSouthWest()
{
	return mSouthWestCorner;
}

const Vector2DF& TerrainPatch::GetNorthEast()
{
	return mNorthEastCorner;
}

bool TerrainPatch::GetEdgeFlagValue(unsigned int pEdgeFlag)
{
	return CheckFlag(mHiResEdgeFlags, pEdgeFlag);
}

void TerrainPatch::GetPosAndNormal(float pNormalizedX, float pNormalizedY, Vector3DF& pPos, Vector3DF& pNormal) const
{
	pNormalizedX; pNormalizedY; pPos; pNormal;
}

GeometryBase::GeometryVolatility TerrainPatch::GetGeometryVolatility() const
{
	return GeometryBase::GEOM_STATIC;
}

void TerrainPatch::SetGeometryVolatility(GeometryVolatility)
{
}

GeometryBase::PrimitiveType TerrainPatch::GetPrimitiveType() const
{
	return GeometryBase::TRIANGLES;
}

unsigned int TerrainPatch::GetMaxVertexCount() const
{
	return mVertexCount;
}

unsigned int TerrainPatch::GetMaxIndexCount() const
{
	return mTriangleCount * 3;
}

unsigned int TerrainPatch::GetVertexCount() const
{
	return mVertexCount;
}

unsigned int TerrainPatch::GetIndexCount() const
{
	return mTriangleCount * 3;
}

unsigned int TerrainPatch::GetUVSetCount() const
{
	return 2;
}

float* TerrainPatch::GetVertexData() const
{
	return mVertexData;
}

float* TerrainPatch::GetUVData(unsigned int pUVSet) const
{
	float* lUVSet = 0;
	if (pUVSet >= 0 && pUVSet < 2)
	{
		lUVSet = mUVData[pUVSet];
	}
	return lUVSet;
}

vtx_idx_t* TerrainPatch::GetIndexData() const
{
	return mIndexData;
}

uint8* TerrainPatch::GetColorData() const
{
	return 0;
}

void TerrainPatch::operator=(const TerrainPatch&)
{
	deb_assert(false);
}



}
