
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



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



#ifndef UICHUNKYLOADER_H
#define UICHUNKYLOADER_H



#include "../../TBC/Include/ChunkyLoader.h"
#include "UiAnimatedGeometry.h"
#include "UiTBC.h"



namespace UiTbc
{



class ChunkyClass;
class TriangleBasedGeometry;



/*// Contans information on base-class stuff (structure and animation names), sound names, meshes and their attributes.
class ChunkyClassLoader: public TBC::ChunkyClassLoader
{
public:
	ChunkyClassLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyClassLoader();
	bool Load(ChunkyClass* pData);
	bool Save(const ChunkyClass* pData);
};*/

class ChunkyMeshLoader: public TBC::ChunkyLoader	// Contains the mesh. Excluded is skin weight information and materials.
{
public:
	ChunkyMeshLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyMeshLoader();
	bool Load(TriangleBasedGeometry* pMeshData);
	bool Save(const TriangleBasedGeometry* pMeshData);
};

class ChunkySkinLoader: public TBC::ChunkyLoader	// Contains mesh skin weight information. 1-1 relation to meshes.
{
public:
	ChunkySkinLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkySkinLoader();
	bool Load(AnimatedGeometry* pSkinData);
	bool Save(const AnimatedGeometry* pSkinData);

private:
	bool LoadElementCallback(TBC::ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage);
	bool SaveBoneWeightChunkArray(const AnimatedGeometry* pSkinData);
};

class ChunkyClassLoader: public TBC::ChunkyClassLoader
{
	typedef TBC::ChunkyClassLoader Parent;
public:
	ChunkyClassLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyClassLoader();

protected:
	bool LoadElementCallback(TBC::ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage);
	virtual void AddLoadElements(Parent::FileElementList& pElementList, TBC::ChunkyClass* pData);

private:
	LOG_CLASS_DECLARE();
};



}



#endif // !UICHUNKYLOADER_H
