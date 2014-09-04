/*
	Class:  TerrainPatch
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#ifndef UITERRAINPATCH_H
#define UITERRAINPATCH_H

#include <math.h>
#include "../../Lepra/Include/CubicSpline.h"
#include "../../Lepra/Include/Vector2D.h"
#include "GeometryBase.h"
#include <list>

namespace Tbc
{

class TerrainPatch : public GeometryBase
{
public:
	class Modifier
	{
	public:
		virtual void ModifyVertex(const vec2& pWorldFlatPos, vec3& pVertex) const = 0;
	};
	
	// Edge resolution flags.
	// If set, the corresponding edge will have "PatchSizeMultiplier" times as many 
	// vertices generated.
	enum 
	{
		NORTH_EDGE = (1 << 0),
		SOUTH_EDGE = (1 << 1),
		EAST_EDGE  = (1 << 2),
		WEST_EDGE  = (1 << 3),
	};

	// The log2 of the patch resolution is given to guarantee a power
	// of 2.
	//
	// pPatchUnitSize is the size of the smallest patch.
	static void SetDimensions(int pPatchResLog2,
				  float pPatchUnitSize,
				  int pPatchSizeMultiplier = 3);

	// pPosition defines, in steps of "patch unit size" (see SetDimensions()),
	// what position it has relative to the center patch at position (0,0).
	// This will be used in all calculations that involve neighbour patches.
	//
	// pSizeExponent changes the size of the patch to a power of "SizeMultiplier"
	// "PatchUnitSize" units. See SetDimensions above for more details.
	// For clarity, the size of the patch is calculated as follow:
	// PatchSize = PatchUnitSize * SizeMultiplier^SizeExponent
	//
	// pHiResEdgeFlags contains flags that, if set, will increase
	// the number of vertices along the corresponding edge by a factor of 
	// "PatchSizeMultiplier".
	//
	// The hole map is simply a map of PatchRes^2 entries,
	// where each entry is a boolean telling whether the terrain is solid or not.
	// A value of 0 (= FALSE) means that there is a hole in the terrain, while
	// any other value is interpreted as solid terrain.
	TerrainPatch(const Vector2D<int>& pPosition,
		     uint8* pHoleMap = 0,
		     unsigned int pSizeExponent = 0,
		     unsigned int pHiResEdgeFlags = 0,
		     float pWestU1 = 0, float pEastU1 = 1, float pSouthV1 = 1, float pNorthV1 = 0,
		     float pWestU2 = 0, float pEastU2 = 1, float pSouthV2 = 1, float pNorthV2 = 0);

	virtual ~TerrainPatch();

	// Returns the number of vertices in one dimension.
	inline int GetVertexRes() const;
	inline float GetPatchSize() const;
	inline int GetPatchSizeMultiplier() const;

	const vec2& GetSouthWest();
	const vec2& GetNorthEast();

	bool GetEdgeFlagValue(unsigned int pEdgeFlag);

	void GetPosAndNormal(float pNormalizedX, float pNormalizedY, vec3& pPos, vec3& pNormal) const;

	void ShareVerticesWithNorthNeighbour(TerrainPatch& pNorthNeighbour);
	void ShareVerticesWithSouthNeighbour(TerrainPatch& pSouthNeighbour);
	void ShareVerticesWithEastNeighbour(TerrainPatch& pEastNeighbour);
	void ShareVerticesWithWestNeighbour(TerrainPatch& pWestNeighbour);

	void ShareNormalsWithNorthNeighbour(TerrainPatch& pNorthNeighbour);
	void ShareNormalsWithSouthNeighbour(TerrainPatch& pSouthNeighbour);
	void ShareNormalsWithEastNeighbour(TerrainPatch& pEastNeighbour);
	void ShareNormalsWithWestNeighbour(TerrainPatch& pWestNeighbour);

	void MergeNormalsWithNorthNeighbour(TerrainPatch& pNorthNeighbour);
	void MergeNormalsWithSouthNeighbour(TerrainPatch& pSouthNeighbour);
	void MergeNormalsWithEastNeighbour(TerrainPatch& pEastNeighbour);
	void MergeNormalsWithWestNeighbour(TerrainPatch& pWestNeighbour);

	void IterateOverPatch(const Modifier& pModifier, int pMinXIndex, int pMaxXIndex, int pMinYIndex, int pMaxYIndex);

	// Overloads from GeometryBase
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility pVolatility);

	unsigned int GetMaxVertexCount() const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount() const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount() const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	vtx_idx_t*     GetIndexData() const;
	uint8*         GetColorData() const;

protected:
	// Returns the pointer to the vertex data. Treats pIndex as if the vertices
	// were aligned in memory, even if the extra vertices are stored at the end
	// of the vertex array.
	float* GetSouthEdgeVertex(int pIndex);
	float* GetNorthEdgeVertex(int pIndex);
	float* GetWestEdgeVertex(int pIndex);
	float* GetEastEdgeVertex(int pIndex);

	float* GetSouthEdgeNormal(int pIndex);
	float* GetNorthEdgeNormal(int pIndex);
	float* GetWestEdgeNormal(int pIndex);
	float* GetEastEdgeNormal(int pIndex);

	// This is the function that gets the actual work done.
	float* GetEdgeVertex(int pIndex, unsigned int pEdgeFlag, int pRegularStartIndex, int pExtraStartIndex, int pPitch, float* pVertexData);

	Vector2D<int> GetSouthWestUnitPos() const;
	Vector2D<int> GetNorthEastUnitPos() const;

	void ProcessSharedEdge(TerrainPatch& pNeighbour,
			       int pEdge, int pOppositeEdge,
			       void (TerrainPatch::*pAssertAlignment)(const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&),
			       int (TerrainPatch::*pGetVertexDisplacement)(const Vector2D<int>&, const Vector2D<int>&),
			       float* (TerrainPatch::*pGetEdgeElement)(int pIndex),
			       float* (TerrainPatch::*pGetOppositeEdgeElement)(int pIndex),
			       void (TerrainPatch::*pProcessElement)(float* pDst, float* pSrs),
			       void (TerrainPatch::*pOppositeShareEdgeFunction)(TerrainPatch& pNorthNeighbour));

	void AssertNorthAlignment(const Vector2D<int>& pSouthWest,
				  const Vector2D<int>& pNorthEast,
				  const Vector2D<int>& pSouthWestNeighbour,
				  const Vector2D<int>& pNorthEastNeighbour);

	void AssertSouthAlignment(const Vector2D<int>& pSouthWest,
				  const Vector2D<int>& pNorthEast,
				  const Vector2D<int>& pSouthWestNeighbour,
				  const Vector2D<int>& pNorthEastNeighbour);

	void AssertEastAlignment(const Vector2D<int>& pSouthWest,
				 const Vector2D<int>& pNorthEast,
				 const Vector2D<int>& pSouthWestNeighbour,
				 const Vector2D<int>& pNorthEastNeighbour);

	void AssertWestAlignment(const Vector2D<int>& pSouthWest,
				 const Vector2D<int>& pNorthEast,
				 const Vector2D<int>& pSouthWestNeighbour,
				 const Vector2D<int>& pNorthEastNeighbour);

	int GetHorizontalDisplacement(const Vector2D<int>& pSouthWest, const Vector2D<int>& pSouthWestNeighbour);
	int GetVerticalDisplacement(const Vector2D<int>& pSouthWest, const Vector2D<int>& pSouthWestNeighbour);

	void CopyElement(float* pDst, float* pSrs);
	void SetElementsToMean(float* pDst, float* pSrs);

private:

	void GenerateUVData(float pWestU1, float pEastU1, float pSouthV1, float pNorthV1,
			    float pWestU2, float pEastU2, float pSouthV2, float pNorthV2);

	void GenerateIndexData(uint8* pHoleMap);
	void AddTriangles(int pTriCount, int pExtraVertexStartIndex, uint32 pCenter, uint32 pV1, uint32 pV2, unsigned int& pTriIndex, bool pSwap);

	void SetToFlatTerrainPatch();

	static void SetFlatCallback(float pFlatX, float pFlatY, vec3& pCurrentPoint);
//	void SetVertexData(const vec3* pVertexData);
//	void SetEdgeVertexData(const vec3* pVertexData, int pDstVertexIndex, int pSrcVertexStartIndex, int pPitch);

	inline bool CheckFlag(unsigned int pFlags, unsigned int pFlag) const;

	unsigned int mVertexCount;
	unsigned int mTriangleCount;
	unsigned int mSizeMultiplier;
	unsigned int mHiResEdgeFlags;

	// Indices to the extra vertices of the edges.
	unsigned int mNorthEdgeIndex;
	unsigned int mSouthEdgeIndex;
	unsigned int mEastEdgeIndex;
	unsigned int mWestEdgeIndex;

	float* mVertexData;
	float* mUVData[2];
	vtx_idx_t* mIndexData;

	// Misc data.
	vec2 mSouthWestCorner;
	vec2 mNorthEastCorner;
	Vector2D<int> mUnitPosition;

	static int smNumPatches;
	static float smPatchUnitSize;
	static int smPatchRes;
	static int smPatchSizeMultiplier;

	void operator=(const TerrainPatch&);	// Not allowed!
};

int TerrainPatch::GetVertexRes() const
{
	return smPatchRes + 1;
}

float TerrainPatch::GetPatchSize() const
{
	return smPatchUnitSize * (float)mSizeMultiplier;
}

int TerrainPatch::GetPatchSizeMultiplier() const
{
	return smPatchSizeMultiplier;
}

bool TerrainPatch::CheckFlag(unsigned int pFlags, unsigned int pFlag) const
{
	return (pFlags & pFlag) != 0;
}


} // End namespace.

#endif
