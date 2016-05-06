
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

#include "../../tbc/include/chunkyloader.h"
#include "uianimatedgeometry.h"
#include "uitbc.h"



namespace uitbc {



class ChunkyClass;
class TriangleBasedGeometry;



class ChunkyMeshLoader: public tbc::ChunkyLoader {	// Contains the mesh. Excluded is skin weight information and materials.
public:
	ChunkyMeshLoader(File* file, bool is_file_owner);
	virtual ~ChunkyMeshLoader();
	bool Load(TriangleBasedGeometry* mesh_data, int& casts_shadows);
	bool Save(const TriangleBasedGeometry* mesh_data, int casts_shadows);
};

class ChunkySkinLoader: public tbc::ChunkyLoader {	// Contains mesh skin weight information. 1-1 relation to meshes.
public:
	ChunkySkinLoader(File* file, bool is_file_owner);
	virtual ~ChunkySkinLoader();
	bool Load(AnimatedGeometry* skin_data);
	bool Save(const AnimatedGeometry* skin_data);

private:
	bool LoadElementCallback(tbc::ChunkyType type, uint32 size, int64 chunk_end_position, void* storage);
	bool SaveBoneWeightChunkArray(const AnimatedGeometry* skin_data);
};

class ChunkyClassLoader: public tbc::ChunkyClassLoader {
	typedef tbc::ChunkyClassLoader Parent;
public:
	ChunkyClassLoader(File* file, bool is_file_owner);
	virtual ~ChunkyClassLoader();

protected:
	virtual void AddLoadElements(Parent::FileElementList& element_list, tbc::ChunkyClass* data);
	bool LoadElementCallback(tbc::ChunkyType type, uint32 size, int64 chunk_end_position, void* storage);

private:
	logclass();
};



}
