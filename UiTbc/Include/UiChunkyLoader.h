
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



// Mesh format:
// - File tag
//   |- Vertices
//   |-
//   |- Volatility

// Skin format:
//   |- Bone weight group
//      |- Bone indices
//      |- Vertex indices
//      |- Weights (WeightCount = BoneCount*VertexCount)
//      |- ...
//      |- Bone indices
//      |- Vertex indices
//      |- Weights (WeightCount = BoneCount*VertexCount)



#pragma once

#include "../../Tbc/Include/ChunkyLoader.h"
#include "UiAnimatedGeometry.h"
#include "UiTbc.h"



namespace UiTbc
{



class ChunkyClass;
class TriangleBasedGeometry;



class ChunkyMeshLoader: public Tbc::ChunkyLoader	// Contains the mesh. Excluded is skin weight information and materials.
{
public:
	ChunkyMeshLoader(File* pFile, bool pIsFileOwner);
	virtual ~ChunkyMeshLoader();
	bool Load(TriangleBasedGeometry* pMeshData, int& pCastsShadows);
	bool Save(const TriangleBasedGeometry* pMeshData, int pCastsShadows);
};

class ChunkySkinLoader: public Tbc::ChunkyLoader	// Contains mesh skin weight information. 1-1 relation to meshes.
{
public:
	ChunkySkinLoader(File* pFile, bool pIsFileOwner);
	virtual ~ChunkySkinLoader();
	bool Load(AnimatedGeometry* pSkinData);
	bool Save(const AnimatedGeometry* pSkinData);

private:
	bool LoadElementCallback(Tbc::ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage);
	bool SaveBoneWeightChunkArray(const AnimatedGeometry* pSkinData);
};

class ChunkyClassLoader: public Tbc::ChunkyClassLoader
{
	typedef Tbc::ChunkyClassLoader Parent;
public:
	ChunkyClassLoader(File* pFile, bool pIsFileOwner);
	virtual ~ChunkyClassLoader();

protected:
	virtual void AddLoadElements(Parent::FileElementList& pElementList, Tbc::ChunkyClass* pData);
	bool LoadElementCallback(Tbc::ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage);

private:
	logclass();
};



}
