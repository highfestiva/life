/*
	Class:  TerrainPatch
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include <math.h>
#include "../../lepra/include/cubicspline.h"
#include "../../lepra/include/vector2d.h"
#include "geometrybase.h"
#include <list>

namespace tbc {

class TerrainPatch : public GeometryBase {
public:
	class Modifier {
	public:
		virtual void ModifyVertex(const vec2& world_flat_pos, vec3& vertex) const = 0;
	};

	// Edge resolution flags.
	// If set, the corresponding edge will have "PatchSizeMultiplier" times as many
	// vertices generated.
	enum {
		kNorthEdge = (1 << 0),
		kSouthEdge = (1 << 1),
		kEastEdge  = (1 << 2),
		kWestEdge  = (1 << 3),
	};

	// The log2 of the patch resolution is given to guarantee a power
	// of 2.
	//
	// patch_unit_size is the size of the smallest patch.
	static void SetDimensions(int patch_res_log2,
				  float patch_unit_size,
				  int patch_size_multiplier = 3);

	// position defines, in steps of "patch unit size" (see SetDimensions()),
	// what position it has relative to the center patch at position (0,0).
	// This will be used in all calculations that involve neighbour patches.
	//
	// size_exponent changes the size of the patch to a power of "SizeMultiplier"
	// "PatchUnitSize" units. See SetDimensions above for more details.
	// For clarity, the size of the patch is calculated as follow:
	// PatchSize = PatchUnitSize * SizeMultiplier^SizeExponent
	//
	// hi_res_edge_flags contains flags that, if set, will increase
	// the number of vertices along the corresponding edge by a factor of
	// "PatchSizeMultiplier".
	//
	// The hole map is simply a map of PatchRes^2 entries,
	// where each entry is a boolean telling whether the terrain is solid or not.
	// A value of 0 (= FALSE) means that there is a hole in the terrain, while
	// any other value is interpreted as solid terrain.
	TerrainPatch(const Vector2D<int>& position,
		     uint8* hole_map = 0,
		     unsigned int size_exponent = 0,
		     unsigned int hi_res_edge_flags = 0,
		     float west_u1 = 0, float east_u1 = 1, float south_v1 = 1, float north_v1 = 0,
		     float west_u2 = 0, float east_u2 = 1, float south_v2 = 1, float north_v2 = 0);

	virtual ~TerrainPatch();

	// Returns the number of vertices in one dimension.
	inline int GetVertexRes() const;
	inline float GetPatchSize() const;
	inline int GetPatchSizeMultiplier() const;

	const vec2& GetSouthWest();
	const vec2& GetNorthEast();

	bool GetEdgeFlagValue(unsigned int edge_flag);

	void GetPosAndNormal(float normalized_x, float normalized_y, vec3& pos, vec3& normal) const;

	void ShareVerticesWithNorthNeighbour(TerrainPatch& north_neighbour);
	void ShareVerticesWithSouthNeighbour(TerrainPatch& south_neighbour);
	void ShareVerticesWithEastNeighbour(TerrainPatch& east_neighbour);
	void ShareVerticesWithWestNeighbour(TerrainPatch& west_neighbour);

	void ShareNormalsWithNorthNeighbour(TerrainPatch& north_neighbour);
	void ShareNormalsWithSouthNeighbour(TerrainPatch& south_neighbour);
	void ShareNormalsWithEastNeighbour(TerrainPatch& east_neighbour);
	void ShareNormalsWithWestNeighbour(TerrainPatch& west_neighbour);

	void MergeNormalsWithNorthNeighbour(TerrainPatch& north_neighbour);
	void MergeNormalsWithSouthNeighbour(TerrainPatch& south_neighbour);
	void MergeNormalsWithEastNeighbour(TerrainPatch& east_neighbour);
	void MergeNormalsWithWestNeighbour(TerrainPatch& west_neighbour);

	void IterateOverPatch(const Modifier& modifier, int min_x_index, int max_x_index, int min_y_index, int max_y_index);

	// Overloads from GeometryBase
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility volatility);

	unsigned int GetMaxVertexCount() const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount() const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount() const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int uv_set) const;
	vtx_idx_t*     GetIndexData() const;
	uint8*         GetColorData() const;

protected:
	// Returns the pointer to the vertex data. Treats index as if the vertices
	// were aligned in memory, even if the extra vertices are stored at the end
	// of the vertex array.
	float* GetSouthEdgeVertex(int index);
	float* GetNorthEdgeVertex(int index);
	float* GetWestEdgeVertex(int index);
	float* GetEastEdgeVertex(int index);

	float* GetSouthEdgeNormal(int index);
	float* GetNorthEdgeNormal(int index);
	float* GetWestEdgeNormal(int index);
	float* GetEastEdgeNormal(int index);

	// This is the function that gets the actual work done.
	float* GetEdgeVertex(int index, unsigned int edge_flag, int regular_start_index, int extra_start_index, int pitch, float* vertex_data);

	Vector2D<int> GetSouthWestUnitPos() const;
	Vector2D<int> GetNorthEastUnitPos() const;

	void ProcessSharedEdge(TerrainPatch& neighbour,
			       int edge, int opposite_edge,
			       void (TerrainPatch::*assert_alignment)(const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&),
			       int (TerrainPatch::*get_vertex_displacement)(const Vector2D<int>&, const Vector2D<int>&),
			       float* (TerrainPatch::*get_edge_element)(int index),
			       float* (TerrainPatch::*get_opposite_edge_element)(int index),
			       void (TerrainPatch::*process_element)(float* dst, float* srs),
			       void (TerrainPatch::*opposite_share_edge_function)(TerrainPatch& north_neighbour));

	void AssertNorthAlignment(const Vector2D<int>& south_west,
				  const Vector2D<int>& north_east,
				  const Vector2D<int>& south_west_neighbour,
				  const Vector2D<int>& north_east_neighbour);

	void AssertSouthAlignment(const Vector2D<int>& south_west,
				  const Vector2D<int>& north_east,
				  const Vector2D<int>& south_west_neighbour,
				  const Vector2D<int>& north_east_neighbour);

	void AssertEastAlignment(const Vector2D<int>& south_west,
				 const Vector2D<int>& north_east,
				 const Vector2D<int>& south_west_neighbour,
				 const Vector2D<int>& north_east_neighbour);

	void AssertWestAlignment(const Vector2D<int>& south_west,
				 const Vector2D<int>& north_east,
				 const Vector2D<int>& south_west_neighbour,
				 const Vector2D<int>& north_east_neighbour);

	int GetHorizontalDisplacement(const Vector2D<int>& south_west, const Vector2D<int>& south_west_neighbour);
	int GetVerticalDisplacement(const Vector2D<int>& south_west, const Vector2D<int>& south_west_neighbour);

	void CopyElement(float* dst, float* srs);
	void SetElementsToMean(float* dst, float* srs);

private:

	void GenerateUVData(float west_u1, float east_u1, float south_v1, float north_v1,
			    float west_u2, float east_u2, float south_v2, float north_v2);

	void GenerateIndexData(uint8* hole_map);
	void AddTriangles(int tri_count, int extra_vertex_start_index, uint32 center, uint32 v1, uint32 v2, unsigned int& tri_index, bool swap);

	void SetToFlatTerrainPatch();

	static void SetFlatCallback(float flat_x, float flat_y, vec3& current_point);
//	void SetVertexData(const vec3* vertex_data);
//	void SetEdgeVertexData(const vec3* vertex_data, int pDstVertexIndex, int pSrcVertexStartIndex, int pitch);

	inline bool CheckFlag(unsigned int flags, unsigned int pFlag) const;

	unsigned int vertex_count_;
	unsigned int triangle_count_;
	unsigned int size_multiplier_;
	unsigned int hi_res_edge_flags_;

	// Indices to the extra vertices of the edges.
	unsigned int north_edge_index_;
	unsigned int south_edge_index_;
	unsigned int east_edge_index_;
	unsigned int west_edge_index_;

	float* vertex_data_;
	float* uv_data_[2];
	vtx_idx_t* index_data_;

	// Misc data.
	vec2 south_west_corner_;
	vec2 north_east_corner_;
	Vector2D<int> unit_position_;

	static int num_patches_;
	static float patch_unit_size_;
	static int patch_res_;
	static int patch_size_multiplier_;

	void operator=(const TerrainPatch&);	// Not allowed!
};

int TerrainPatch::GetVertexRes() const {
	return patch_res_ + 1;
}

float TerrainPatch::GetPatchSize() const {
	return patch_unit_size_ * (float)size_multiplier_;
}

int TerrainPatch::GetPatchSizeMultiplier() const {
	return patch_size_multiplier_;
}

bool TerrainPatch::CheckFlag(unsigned int flags, unsigned int pFlag) const {
	return (flags & pFlag) != 0;
}


}
