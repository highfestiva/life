/*
	Class:  TerrainPatch
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/terrainpatch.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/timer.h"
#include "../../lepra/include/math.h"

#include "../../lepra/include/lepraassert.h"

namespace tbc {

int   TerrainPatch::num_patches_ = 0;
float TerrainPatch::patch_unit_size_ = 1.0f;
int   TerrainPatch::patch_res_ = (1 << 4);
int   TerrainPatch::patch_size_multiplier_ = 3;

void TerrainPatch::SetDimensions(int patch_res_log2,
				 float patch_unit_size,
				 int patch_size_multiplier) {
	deb_assert(patch_res_log2 >= 0 && patch_res_log2 < 16);
	deb_assert(patch_unit_size > 0.0f);

	if (num_patches_ == 0) {
		patch_res_ = (1 << patch_res_log2);
		patch_unit_size_ = patch_unit_size;
		patch_size_multiplier_ = patch_size_multiplier;
	}
}


TerrainPatch::TerrainPatch(const Vector2D<int>& position,
			   uint8* hole_map,
			   unsigned int size_exponent,
			   unsigned int hi_res_edge_flags,
			   float west_u1, float east_u1, float south_v1, float north_v1,
			   float west_u2, float east_u2, float south_v2, float north_v2):
	vertex_count_(0),
	triangle_count_(0),
	size_multiplier_(Math::Pow(patch_size_multiplier_, size_exponent)),
	hi_res_edge_flags_(hi_res_edge_flags),
	north_edge_index_(0),
	south_edge_index_(0),
	east_edge_index_(0),
	west_edge_index_(0),
	vertex_data_(0),
	index_data_(0),
	unit_position_(position) {
	SetPrimitiveType(GeometryBase::kTriangles);

	uv_data_[0] = 0;
	uv_data_[1] = 0;

	float patch_size = (float)GetPatchSize();
	south_west_corner_.Set((float)position.x * patch_unit_size_, (float)position.y * patch_unit_size_),
	north_east_corner_.Set(south_west_corner_.x + patch_size, south_west_corner_.y + patch_size);

	vertex_count_ = (patch_res_ + 1) * (patch_res_ + 1) +
			patch_res_ * patch_res_;
	triangle_count_ = patch_res_ * patch_res_ * 4;

	north_edge_index_ = vertex_count_;
	south_edge_index_ = vertex_count_;
	east_edge_index_  = vertex_count_;
	west_edge_index_  = vertex_count_;

	int edge_count = 0;
	if (CheckFlag(hi_res_edge_flags_, kSouthEdge) == true) {
		edge_count++;
		north_edge_index_ += patch_res_ * (patch_size_multiplier_ - 1);
		east_edge_index_  += patch_res_ * (patch_size_multiplier_ - 1);
		west_edge_index_  += patch_res_ * (patch_size_multiplier_ - 1);
	}
	if (CheckFlag(hi_res_edge_flags_, kNorthEdge) == true) {
		edge_count++;
		east_edge_index_  += patch_res_ * (patch_size_multiplier_ - 1);
		west_edge_index_  += patch_res_ * (patch_size_multiplier_ - 1);
	}
	if (CheckFlag(hi_res_edge_flags_, kEastEdge) == true) {
		edge_count++;
		west_edge_index_  += patch_res_ * (patch_size_multiplier_ - 1);
	}
	if (CheckFlag(hi_res_edge_flags_, kWestEdge) == true) {
		edge_count++;
	}

	// For each square on each high-res edge, there are patch_size_multiplier_ - 1,
	// extra vertices and triangles.
	vertex_count_ += edge_count * patch_res_ * (patch_size_multiplier_ - 1);
	triangle_count_ += edge_count * patch_res_ * (patch_size_multiplier_ - 1);

	vertex_data_ = new float[vertex_count_ * 3];
	uv_data_[0] = new float[vertex_count_ * 2];
	uv_data_[1] = new float[vertex_count_ * 2];
	index_data_ = new vtx_idx_t[triangle_count_ * 3];

	//SetVertexData(vertex_data);
	SetToFlatTerrainPatch();
	GenerateIndexData(hole_map);
	GenerateUVData(west_u1, east_u1, south_v1, north_v1,
		       west_u2, east_u2, south_v2, north_v2);
	//GeometryBase::GenerateVertexNormalData();

	++num_patches_;
}

TerrainPatch::~TerrainPatch() {
	delete[] uv_data_[0];
	delete[] uv_data_[1];
	delete[] vertex_data_;
	delete[] index_data_;

	--num_patches_;
}

void TerrainPatch::GenerateUVData(float west_u1, float east_u1, float south_v1, float north_v1,
				  float west_u2, float east_u2, float south_v2, float north_v2) {
	float u1_step = (east_u1 - west_u1) / (float)patch_res_;
	float v1_step = (north_v1 - south_v1) / (float)patch_res_;
	float u2_step = (east_u2 - west_u1) / (float)patch_res_;
	float v2_step = (north_v2 - south_v1) / (float)patch_res_;

	float _v1 = south_v1;
	float _v2 = south_v2;
	for (int y = 0; y < patch_res_ + 1; y++) {
		float u1 = west_u1;
		float u2 = west_u2;
		for (int x = 0; x < patch_res_ + 1; x++) {
			int uv_index = (y * (patch_res_ + 1) + x) * 2;
			uv_data_[0][uv_index + 0] = u1;
			uv_data_[0][uv_index + 1] = _v1;
			uv_data_[1][uv_index + 0] = u2;
			uv_data_[1][uv_index + 1] = _v2;

			if (y < patch_res_ && x < patch_res_) {
				uv_index = ((patch_res_ + 1) * (patch_res_ + 1) +
						(y * patch_res_ + x)) * 2;
				uv_data_[0][uv_index + 0] = u1 + u1_step * 0.5f;
				uv_data_[0][uv_index + 1] = _v1 + v1_step * 0.5f;
				uv_data_[1][uv_index + 0] = u2 + u2_step * 0.5f;
				uv_data_[1][uv_index + 1] = _v2 + v2_step * 0.5f;
			}

			u1 += u1_step;
			u2 += u2_step;
		}

		_v1 += v1_step;
		_v2 += v2_step;
	}

	int uv_index = ((patch_res_ + 1) * (patch_res_ + 1) + patch_res_ * patch_res_) * 2;
	float u1_small_step = u1_step / (float)patch_size_multiplier_;
	float v1_small_step = v1_step / (float)patch_size_multiplier_;
	float u2_small_step = u2_step / (float)patch_size_multiplier_;
	float v2_small_step = v2_step / (float)patch_size_multiplier_;
	if (CheckFlag(hi_res_edge_flags_, kSouthEdge) == true) {
		float u1 = west_u1 + u1_small_step;
		float u2 = west_u2 + u2_small_step;
		for (int i = 0; i < patch_res_; i++) {
			for (int j = 1; j < patch_size_multiplier_; j++) {
				uv_data_[0][uv_index + 0] = u1;
				uv_data_[0][uv_index + 1] = south_v1;
				uv_data_[1][uv_index + 0] = u2;
				uv_data_[1][uv_index + 1] = south_v2;
				u1 += u1_small_step;
				u2 += u2_small_step;
				uv_index += 2;
			}
			u1 += u1_small_step;
			u2 += u2_small_step;
		}
	}
	if (CheckFlag(hi_res_edge_flags_, kNorthEdge) == true) {
		float u1 = west_u1 + u1_small_step;
		float u2 = west_u2 + u2_small_step;
		for (int i = 0; i < patch_res_; i++) {
			for (int j = 1; j < patch_size_multiplier_; j++) {
				uv_data_[0][uv_index + 0] = u1;
				uv_data_[0][uv_index + 1] = north_v1;
				uv_data_[1][uv_index + 0] = u2;
				uv_data_[1][uv_index + 1] = north_v2;
				u1 += u1_small_step;
				u2 += u2_small_step;
				uv_index += 2;
			}
			u1 += u1_small_step;
			u2 += u2_small_step;
		}
	}
	if (CheckFlag(hi_res_edge_flags_, kWestEdge) == true) {
		float __v1 = south_v1 + v1_small_step;
		float __v2 = south_v2 + v2_small_step;
		for (int i = 0; i < patch_res_; i++) {
			for (int j = 1; j < patch_size_multiplier_; j++) {
				uv_data_[0][uv_index + 0] = west_u1;
				uv_data_[0][uv_index + 1] = __v1;
				uv_data_[1][uv_index + 0] = west_u2;
				uv_data_[1][uv_index + 1] = __v2;
				__v1 += v1_small_step;
				__v2 += v2_small_step;
				uv_index += 2;
			}
			__v1 += v1_small_step;
			__v2 += v2_small_step;
		}
	}
	if (CheckFlag(hi_res_edge_flags_, kEastEdge) == true) {
		float __v1 = south_v1 + v1_small_step;
		float __v2 = south_v2 + v2_small_step;
		for (int i = 0; i < patch_res_; i++) {
			for (int j = 1; j < patch_size_multiplier_; j++) {
				uv_data_[0][uv_index + 0] = east_u1;
				uv_data_[0][uv_index + 1] = __v1;
				uv_data_[1][uv_index + 0] = east_u2;
				uv_data_[1][uv_index + 1] = __v2;
				__v1 += v1_small_step;
				__v2 += v2_small_step;
				uv_index += 2;
			}
			__v1 += v1_small_step;
			__v2 += v2_small_step;
		}
	}
}

void TerrainPatch::GenerateIndexData(uint8* hole_map) {
	int mid_vertex_start_index = (patch_res_ + 1) * (patch_res_ + 1);

	unsigned int _tri_index = 0;
	for (int y = 0; y < patch_res_; ++y) {
		int top_tri_count = 1;
		int bottom_tri_count = 1;
		if (y == 0 && CheckFlag(hi_res_edge_flags_, kSouthEdge) == true) {
			bottom_tri_count = patch_size_multiplier_;
		}
		if (y == patch_res_ - 1 && CheckFlag(hi_res_edge_flags_, kNorthEdge) == true) {
			top_tri_count = patch_size_multiplier_;
		}

		for (int x = 0; x < patch_res_; ++x) {
			if (hole_map == 0 || hole_map[y * patch_res_ + x] != 0) {
				int left_tri_count = 1;
				int right_tri_count = 1;
				if (x == 0 && CheckFlag(hi_res_edge_flags_, kWestEdge) == true) {
					left_tri_count = patch_size_multiplier_;
				}
				if (x == patch_res_ - 1 && CheckFlag(hi_res_edge_flags_, kEastEdge) == true) {
					right_tri_count = patch_size_multiplier_;
				}

				int bl_vertex = y * (patch_res_ + 1) + x;
				int br_vertex = bl_vertex + 1;
				int tl_vertex = (y + 1) * (patch_res_ + 1) + x;
				int tr_vertex = tl_vertex + 1;
				int center_vertex = mid_vertex_start_index + y * patch_res_ + x;

				// Create top triangles.
				AddTriangles(top_tri_count, north_edge_index_ + x * (patch_size_multiplier_ - 1),
					center_vertex, tl_vertex, tr_vertex, _tri_index, true);

				// Create bottom triangles.
				AddTriangles(bottom_tri_count, south_edge_index_ + x * (patch_size_multiplier_ - 1),
					center_vertex, bl_vertex, br_vertex, _tri_index, false);

				// Create left triangles.
				AddTriangles(left_tri_count, west_edge_index_ + y * (patch_size_multiplier_ - 1),
					center_vertex, bl_vertex, tl_vertex, _tri_index, true);

				// Create right triangles.
				AddTriangles(right_tri_count, east_edge_index_ + y * (patch_size_multiplier_ - 1),
					center_vertex, br_vertex, tr_vertex, _tri_index, false);
			}
		}
	}

	GeometryBase::SetIndexDataChanged(true);
	deb_assert((_tri_index % 3) == 0 && (_tri_index / 3) == triangle_count_);
}

void TerrainPatch::AddTriangles(int tri_count, int extra_vertex_start_index,
				uint32 center, uint32 v1, uint32 v2,
				unsigned int& tri_index, bool swap) {
	int _v1 = v1;
	int _v2 = tri_count == 1 ? v2 : extra_vertex_start_index;

	for (int i = 0; i < tri_count; i++) {
		if (i > 0) {
			_v1 = extra_vertex_start_index + i - 1;
			_v2 = _v1 + 1;
		}
		if (i == tri_count - 1) {
			_v2 = v2;
		}

		index_data_[tri_index + 0] = center;

		if (swap == true) {
			index_data_[tri_index + 1] = _v2;
			index_data_[tri_index + 2] = _v1;
		} else {
			index_data_[tri_index + 1] = _v1;
			index_data_[tri_index + 2] = _v2;
		}
		tri_index += 3;
	}
}

void TerrainPatch::IterateOverPatch(const Modifier& modifier, int min_x_index, int max_x_index, int min_y_index, int max_y_index) {
	float scale_x = (north_east_corner_.x - south_west_corner_.x) / (float)patch_res_;
	float scale_y = (north_east_corner_.y - south_west_corner_.y) / (float)patch_res_;

	int x;
	int y;

	// Corners...
	float _world_y = south_west_corner_.y + scale_y * (float)min_y_index;
	for (y = min_y_index; y < max_y_index; ++y) {
		float _world_x = south_west_corner_.x + scale_x * (float)min_x_index;
		for (x = min_x_index; x < max_x_index; ++x) {
			int vertex_index = (y * (patch_res_ + 1) + x) * 3;
			// TRICKY: Typecasting float-array to vec3 is risky if the implementation
			// of Vector3D changes.
			modifier.ModifyVertex(vec2(_world_x, _world_y), *((vec3*)&vertex_data_[vertex_index]));
			_world_x += scale_x;
		}

		_world_y += scale_y;
	}

	// Mid points...
	_world_y = south_west_corner_.y + scale_y * ((float)min_y_index + 0.5f);
	for (y = min_y_index; y < max_y_index - 1; ++y) {
		float _world_x = south_west_corner_.x + scale_x * ((float)min_x_index + 0.5f);
		for (x = min_x_index; x < max_x_index - 1; ++x) {
			int vertex_index = ((patch_res_ + 1) * (patch_res_ + 1) + y * patch_res_ + x) * 3;
			// TRICKY: Typecasting float-array to vec3 is risky if the implementation
			// of Vector3D changes.
			modifier.ModifyVertex(vec2(_world_x, _world_y), *((vec3*)&vertex_data_[vertex_index]));
			_world_x += scale_x;
		}

		_world_y += scale_y;
	}

	// Extra vertices...
	float small_x_step = scale_x / (float)patch_size_multiplier_;
	float small_y_step = scale_y / (float)patch_size_multiplier_;
	if (CheckFlag(hi_res_edge_flags_, kSouthEdge) == true && min_y_index == 0) {
		float _world_x = south_west_corner_.x + small_x_step;
		for (int i = min_x_index; i < max_x_index - 1; i++) {
			int vertex_index = (south_edge_index_ + i * (patch_size_multiplier_ - 1)) * 3;
			for (int j = 1; j < patch_size_multiplier_; j++) {
				// TRICKY: Typecasting float-array to vec3 is risky if the implementation
				// of Vector3D changes.
				modifier.ModifyVertex(vec2(_world_x, south_west_corner_.y), *((vec3*)&vertex_data_[vertex_index]));
				_world_x += small_x_step;
				vertex_index += 3;
			}
			_world_x += small_x_step;
		}
	}
	if (CheckFlag(hi_res_edge_flags_, kNorthEdge) == true && max_y_index == patch_res_ + 1) {
		float _world_x = south_west_corner_.x + small_x_step;
		for (int i = min_x_index; i < max_x_index - 1; i++) {
			int vertex_index = (north_edge_index_ + i * (patch_size_multiplier_ - 1)) * 3;
			for (int j = 1; j < patch_size_multiplier_; j++) {
				// TRICKY: Typecasting float-array to vec3 is risky if the implementation
				// of Vector3D changes.
				modifier.ModifyVertex(vec2(_world_x, north_east_corner_.y), *((vec3*)&vertex_data_[vertex_index]));
				_world_x += small_x_step;
				vertex_index += 3;
			}
			_world_x += small_x_step;
		}
	}
	if (CheckFlag(hi_res_edge_flags_, kWestEdge) == true && min_x_index == 0) {
		float __world_y = south_west_corner_.y + small_y_step;
		for (int i = min_y_index; i < max_y_index - 1; i++) {
			int vertex_index = (west_edge_index_ + i * (patch_size_multiplier_ - 1)) * 3;
			for (int j = 1; j < patch_size_multiplier_; j++) {
				// TRICKY: Typecasting float-array to vec3 is risky if the implementation
				// of Vector3D changes.
				modifier.ModifyVertex(vec2(south_west_corner_.x, __world_y), *((vec3*)&vertex_data_[vertex_index]));
				__world_y += small_y_step;
				vertex_index += 3;
			}
			__world_y += small_y_step;
		}
	}

	if (CheckFlag(hi_res_edge_flags_, kEastEdge) == true && max_x_index == patch_res_ + 1) {
		float __world_y = south_west_corner_.y + small_y_step;
		for (int i = min_y_index; i < max_y_index - 1; i++) {
			int vertex_index = (east_edge_index_ + i * (patch_size_multiplier_ - 1)) * 3;
			for (int j = 1; j < patch_size_multiplier_; j++) {
				// TRICKY: Typecasting float-array to vec3 is risky if the implementation
				// of Vector3D changes.
				modifier.ModifyVertex(vec2(north_east_corner_.x, __world_y), *((vec3*)&vertex_data_[vertex_index]));
				__world_y += small_y_step;
				vertex_index += 3;
			}
			__world_y += small_y_step;
		}
	}
}

class FlatModifier : public TerrainPatch::Modifier {
public:
	void ModifyVertex(const vec2& world_flat_pos, vec3& vertex) const {
		vertex.x = world_flat_pos.x;
		vertex.y = world_flat_pos.y;
		vertex.z = 0;
	}
};

void TerrainPatch::SetToFlatTerrainPatch() {
	FlatModifier _modifier;
	IterateOverPatch(_modifier, 0, GetVertexRes(), 0, GetVertexRes());
}

void TerrainPatch::SetFlatCallback(float world_x, float world_y, vec3& current_point) {
	current_point.x = world_x;
	current_point.y = world_y;
	current_point.z = 0;
}

/*
void TerrainPatch::SetVertexData(const vec3* vertex_data) {
	int x;
	int y;

	// Setup regular grid vertices.
	for (y = 0; y < patch_res_ + 1; ++y) {
		for (x = 0; x < patch_res_ + 1; ++x) {
			int lDstVertexIndex = (y * (patch_res_ + 1) + x) * 3;
			int lSrcVertexIndex = (y * (patch_res_ + 1) + x) * 2;

			vertex_data_[lDstVertexIndex + 0] = vertex_data[lSrcVertexIndex].x;
			vertex_data_[lDstVertexIndex + 1] = vertex_data[lSrcVertexIndex].y;
			vertex_data_[lDstVertexIndex + 2] = vertex_data[lSrcVertexIndex].z;
		}
	}

	// Setup center vertices.
	int lDstStartIndex = (patch_res_ + 1) * (patch_res_ + 1);
	for (y = 0; y < patch_res_; ++y) {
		for (x = 0; x < patch_res_; ++x) {
			int lDstVertexIndex = (lDstStartIndex + y * patch_res_ + x) * 3;
			int lSrcVertexIndex = ((y * (patch_res_ + 1) + x) * 2 + 1);

			vertex_data_[lDstVertexIndex + 0] = vertex_data[lSrcVertexIndex].x;
			vertex_data_[lDstVertexIndex + 1] = vertex_data[lSrcVertexIndex].y;
			vertex_data_[lDstVertexIndex + 2] = vertex_data[lSrcVertexIndex].z;
		}
	}

	// Setup extra edge vertices.
	if (CheckFlag(hi_res_edge_flags_, kSouthEdge) == true) {
		SetEdgeVertexData(vertex_data, south_edge_index_ * 3, 0, 2);
	}
	if (CheckFlag(hi_res_edge_flags_, kNorthEdge) == true) {
		SetEdgeVertexData(vertex_data, north_edge_index_ * 3, patch_res_ * (patch_res_ + 1) * 2, 2);
	}
	if (CheckFlag(hi_res_edge_flags_, kWestEdge) == true) {
		SetEdgeVertexData(vertex_data, west_edge_index_ * 3, 0, (patch_res_ + 1) * 2);
	}
	if (CheckFlag(hi_res_edge_flags_, kEastEdge) == true) {
		SetEdgeVertexData(vertex_data, east_edge_index_ * 3, (patch_res_ + 1) * 2, (patch_res_ + 1) * 2);
	}
}

void TerrainPatch::SetEdgeVertexData(const vec3* vertex_data, int pDstVertexIndex, int pSrcVertexStartIndex, int pitch) {
	for (int x = 0; x < patch_res_; ++x) {
		int lSrcVertexIndex = (pSrcVertexStartIndex + x * pitch);

		int mid = (patch_size_multiplier_ - 1) / 2;
		int i;
		for (i = 0; i < mid; i++) {
			float t = (float)(i + 1) / (float)patch_size_multiplier_;
			vertex_data_[pDstVertexIndex + 0] = Math::Lerp(vertex_data[lSrcVertexIndex].x, vertex_data[lSrcVertexIndex + pitch].x, t);// + south_west_corner_.x;
			vertex_data_[pDstVertexIndex + 1] = Math::Lerp(vertex_data[lSrcVertexIndex].y, vertex_data[lSrcVertexIndex + pitch].y, t);// + south_west_corner_.y;
			vertex_data_[pDstVertexIndex + 2] = Math::Lerp(vertex_data[lSrcVertexIndex].z, vertex_data[lSrcVertexIndex + pitch].z, t);
			pDstVertexIndex += 3;
		}
		lSrcVertexIndex += pitch;
		for (i = mid; i < patch_size_multiplier_ - 1; i++) {
			float t = (float)(i + 1) / (float)patch_size_multiplier_;
			vertex_data_[pDstVertexIndex + 0] = Math::Lerp(vertex_data[lSrcVertexIndex].x, vertex_data[lSrcVertexIndex + pitch].x, t);// + south_west_corner_.x;
			vertex_data_[pDstVertexIndex + 1] = Math::Lerp(vertex_data[lSrcVertexIndex].y, vertex_data[lSrcVertexIndex + pitch].y, t);// + south_west_corner_.y;
			vertex_data_[pDstVertexIndex + 2] = Math::Lerp(vertex_data[lSrcVertexIndex].z, vertex_data[lSrcVertexIndex + pitch].z, t);
			pDstVertexIndex += 3;
		}
	}
}
*/

float* TerrainPatch::GetSouthEdgeVertex(int index) {
	return GetEdgeVertex(index, kSouthEdge, 0, south_edge_index_, 1, vertex_data_);
}

float* TerrainPatch::GetNorthEdgeVertex(int index) {
	return GetEdgeVertex(index, kNorthEdge, (patch_res_ + 1) * patch_res_, north_edge_index_, 1, vertex_data_);
}

float* TerrainPatch::GetWestEdgeVertex(int index) {
	return GetEdgeVertex(index, kWestEdge, 0, west_edge_index_, patch_res_ + 1, vertex_data_);
}

float* TerrainPatch::GetEastEdgeVertex(int index) {
	return GetEdgeVertex(index, kEastEdge, patch_res_ + 1, east_edge_index_, patch_res_ + 1, vertex_data_);
}

float* TerrainPatch::GetSouthEdgeNormal(int index) {
	return GetEdgeVertex(index, kSouthEdge, 0, south_edge_index_, 1, GetNormalData());
}

float* TerrainPatch::GetNorthEdgeNormal(int index) {
	return GetEdgeVertex(index, kNorthEdge, (patch_res_ + 1) * patch_res_, north_edge_index_, 1, GetNormalData());
}

float* TerrainPatch::GetWestEdgeNormal(int index) {
	return GetEdgeVertex(index, kWestEdge, 0, west_edge_index_, patch_res_ + 1, GetNormalData());
}

float* TerrainPatch::GetEastEdgeNormal(int index) {
	return GetEdgeVertex(index, kEastEdge, patch_res_ + 1, east_edge_index_, patch_res_ + 1, GetNormalData());
}

float* TerrainPatch::GetEdgeVertex(int index, unsigned int edge_flag, int regular_start_index, int extra_start_index, int pitch, float* vertex_data) {
	if (CheckFlag(hi_res_edge_flags_, edge_flag) == false) {
		deb_assert(index >= 0 && index < patch_res_ + 1);
		return &vertex_data[(regular_start_index + index * pitch) * 3];
	} else {
		deb_assert(index >= 0 && index < patch_res_ * patch_size_multiplier_ + 1);

		// Index of previous regular vertex (not an "extra vertex").
		int regular_vertex_index = index / patch_size_multiplier_;
		if (index % patch_size_multiplier_ == 0) {
			// Stepping on the regular vertices.
			return &vertex_data[(regular_start_index + regular_vertex_index * pitch) * 3];
		} else {
			return &vertex_data[(extra_start_index + index - regular_vertex_index - 1) * 3];
		}
	}
}

void TerrainPatch::ShareVerticesWithNorthNeighbour(TerrainPatch& north_neighbour) {
	ProcessSharedEdge(north_neighbour, kNorthEdge, kSouthEdge,
		&TerrainPatch::AssertNorthAlignment, &TerrainPatch::GetHorizontalDisplacement,
		&TerrainPatch::GetNorthEdgeVertex, &TerrainPatch::GetSouthEdgeVertex,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithSouthNeighbour);
}

void TerrainPatch::ShareVerticesWithSouthNeighbour(TerrainPatch& south_neighbour) {
	ProcessSharedEdge(south_neighbour, kSouthEdge, kNorthEdge,
		&TerrainPatch::AssertSouthAlignment, &TerrainPatch::GetHorizontalDisplacement,
		&TerrainPatch::GetSouthEdgeVertex, &TerrainPatch::GetNorthEdgeVertex,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithNorthNeighbour);
}

void TerrainPatch::ShareVerticesWithEastNeighbour(TerrainPatch& east_neighbour) {
	ProcessSharedEdge(east_neighbour, kEastEdge, kWestEdge,
		&TerrainPatch::AssertEastAlignment, &TerrainPatch::GetVerticalDisplacement,
		&TerrainPatch::GetEastEdgeVertex, &TerrainPatch::GetWestEdgeVertex,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithWestNeighbour);
}

void TerrainPatch::ShareVerticesWithWestNeighbour(TerrainPatch& west_neighbour) {
	ProcessSharedEdge(west_neighbour, kWestEdge, kEastEdge,
		&TerrainPatch::AssertWestAlignment, &TerrainPatch::GetVerticalDisplacement,
		&TerrainPatch::GetWestEdgeVertex, &TerrainPatch::GetEastEdgeVertex,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareVerticesWithEastNeighbour);
}

void TerrainPatch::ShareNormalsWithNorthNeighbour(TerrainPatch& north_neighbour) {
	ProcessSharedEdge(north_neighbour, kNorthEdge, kSouthEdge,
		&TerrainPatch::AssertNorthAlignment, &TerrainPatch::GetHorizontalDisplacement,
		&TerrainPatch::GetNorthEdgeNormal, &TerrainPatch::GetSouthEdgeNormal,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithSouthNeighbour);
}

void TerrainPatch::ShareNormalsWithSouthNeighbour(TerrainPatch& south_neighbour) {
	ProcessSharedEdge(south_neighbour, kSouthEdge, kNorthEdge,
		&TerrainPatch::AssertSouthAlignment, &TerrainPatch::GetHorizontalDisplacement,
		&TerrainPatch::GetSouthEdgeNormal, &TerrainPatch::GetNorthEdgeNormal,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithNorthNeighbour);
}

void TerrainPatch::ShareNormalsWithEastNeighbour(TerrainPatch& east_neighbour) {
	ProcessSharedEdge(east_neighbour, kEastEdge, kWestEdge,
		&TerrainPatch::AssertEastAlignment, &TerrainPatch::GetVerticalDisplacement,
		&TerrainPatch::GetEastEdgeNormal, &TerrainPatch::GetWestEdgeNormal,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithWestNeighbour);
}

void TerrainPatch::ShareNormalsWithWestNeighbour(TerrainPatch& west_neighbour) {
	ProcessSharedEdge(west_neighbour, kWestEdge, kEastEdge,
		&TerrainPatch::AssertWestAlignment, &TerrainPatch::GetVerticalDisplacement,
		&TerrainPatch::GetWestEdgeNormal, &TerrainPatch::GetEastEdgeNormal,
		&TerrainPatch::CopyElement, &TerrainPatch::ShareNormalsWithEastNeighbour);
}

void TerrainPatch::MergeNormalsWithNorthNeighbour(TerrainPatch& north_neighbour) {
	ProcessSharedEdge(north_neighbour, kNorthEdge, kSouthEdge,
		&TerrainPatch::AssertNorthAlignment, &TerrainPatch::GetHorizontalDisplacement,
		&TerrainPatch::GetNorthEdgeNormal, &TerrainPatch::GetSouthEdgeNormal,
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithSouthNeighbour);
}

void TerrainPatch::MergeNormalsWithSouthNeighbour(TerrainPatch& south_neighbour) {
	ProcessSharedEdge(south_neighbour, kSouthEdge, kNorthEdge,
		&TerrainPatch::AssertSouthAlignment, &TerrainPatch::GetHorizontalDisplacement,
		&TerrainPatch::GetSouthEdgeNormal, &TerrainPatch::GetNorthEdgeNormal,
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithNorthNeighbour);
}

void TerrainPatch::MergeNormalsWithEastNeighbour(TerrainPatch& east_neighbour) {
	ProcessSharedEdge(east_neighbour, kEastEdge, kWestEdge,
		&TerrainPatch::AssertEastAlignment, &TerrainPatch::GetVerticalDisplacement,
		&TerrainPatch::GetEastEdgeNormal, &TerrainPatch::GetWestEdgeNormal,
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithWestNeighbour);
}

void TerrainPatch::MergeNormalsWithWestNeighbour(TerrainPatch& west_neighbour) {
	ProcessSharedEdge(west_neighbour, kWestEdge, kEastEdge,
		&TerrainPatch::AssertWestAlignment, &TerrainPatch::GetVerticalDisplacement,
		&TerrainPatch::GetWestEdgeNormal, &TerrainPatch::GetEastEdgeNormal,
		&TerrainPatch::SetElementsToMean, &TerrainPatch::ShareNormalsWithEastNeighbour);
}


void TerrainPatch::ProcessSharedEdge(TerrainPatch& neighbour,
				     int edge, int opposite_edge,
				     void (TerrainPatch::*assert_alignment)(const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&),
				     int (TerrainPatch::*get_vertex_displacement)(const Vector2D<int>&, const Vector2D<int>&),
				     float* (TerrainPatch::*get_edge_element)(int index),
				     float* (TerrainPatch::*get_opposite_edge_element)(int index),
				     void (TerrainPatch::*process_element)(float* dst, float* srs),
				     void (TerrainPatch::*opposite_share_edge_function)(TerrainPatch&)) {
	// Not used in release compilation.
	edge;
	opposite_edge;

	Vector2D<int> _south_west(GetSouthWestUnitPos());
	Vector2D<int> _north_east(GetNorthEastUnitPos());
	Vector2D<int> _south_west_neighbour(neighbour.GetSouthWestUnitPos());
	Vector2D<int> _north_east_neighbour(neighbour.GetNorthEastUnitPos());

	(this->*assert_alignment)(_south_west, _north_east, _south_west_neighbour, _north_east_neighbour);

	// The size can only differ with one step.
	deb_assert(size_multiplier_ == neighbour.size_multiplier_ ||
	       size_multiplier_ == neighbour.size_multiplier_ - patch_size_multiplier_ ||
	       size_multiplier_ == neighbour.size_multiplier_ + patch_size_multiplier_);

	// The big patch adapts to the smaller patch.
	if (size_multiplier_ >= neighbour.size_multiplier_) {
		deb_assert(neighbour.CheckFlag(neighbour.hi_res_edge_flags_, opposite_edge) == false &&
			((size_multiplier_ == neighbour.size_multiplier_ && CheckFlag(hi_res_edge_flags_, edge) == false) ||
			CheckFlag(hi_res_edge_flags_, edge) == true));

		int vertex_count = neighbour.GetVertexRes();
		int start_index = (this->*get_vertex_displacement)(_south_west, _south_west_neighbour) * vertex_count;
		int end_index = start_index + vertex_count;

		for (int i = start_index; i < end_index; i++) {
			float* _dst = (this->*get_edge_element)(i);
			float* _src = (neighbour.*get_opposite_edge_element)(i - start_index);

			(this->*process_element)(_dst, _src);
		}
	} else {
		(neighbour.*opposite_share_edge_function)(*this);
	}
}

void TerrainPatch::AssertNorthAlignment(const Vector2D<int>&, const Vector2D<int>& north_east, const Vector2D<int>& south_west_neighbour,	const Vector2D<int>&) {
	// TODO: Implement a check along the x-axis!?
	deb_assert(north_east.y == south_west_neighbour.y);
	north_east;
	south_west_neighbour;
}

void TerrainPatch::AssertSouthAlignment(const Vector2D<int>& south_west, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>& north_east_neighbour) {
	// TODO: Implement a check along the x-axis!?
	deb_assert(south_west.y == north_east_neighbour.y);
	south_west;
	north_east_neighbour;
}

void TerrainPatch::AssertEastAlignment(const Vector2D<int>&, const Vector2D<int>& north_east, const Vector2D<int>& south_west_neighbour, const Vector2D<int>&) {
	// TODO: Implement a check along the y-axis!?
	deb_assert(north_east.x == south_west_neighbour.x);
	north_east;
	south_west_neighbour;
}

void TerrainPatch::AssertWestAlignment(const Vector2D<int>& south_west, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>& north_east_neighbour) {
	// TODO: Implement a check along the y-axis!?
	deb_assert(south_west.x == north_east_neighbour.x);
	south_west;
	north_east_neighbour;
}

int TerrainPatch::GetHorizontalDisplacement(const Vector2D<int>& south_west, const Vector2D<int>& south_west_neighbour) {
	return (south_west_neighbour.x - south_west.x);
}

int TerrainPatch::GetVerticalDisplacement(const Vector2D<int>& south_west, const Vector2D<int>& south_west_neighbour) {
	return (south_west_neighbour.y - south_west.y);
}

void TerrainPatch::CopyElement(float* dst, float* src) {
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

void TerrainPatch::SetElementsToMean(float* dst, float* src) {
	vec3 v((dst[0] + src[0]) * 0.5f,
			     (dst[1] + src[1]) * 0.5f,
			     (dst[2] + src[2]) * 0.5f);
	v.Normalize();
	dst[0] = src[0] = v.x;
	dst[1] = src[1] = v.y;
	dst[2] = src[2] = v.z;
}

Vector2D<int> TerrainPatch::GetSouthWestUnitPos() const {
	return unit_position_;
}

Vector2D<int> TerrainPatch::GetNorthEastUnitPos() const {
	return unit_position_ + Vector2D<int>(size_multiplier_, size_multiplier_);
}

const vec2& TerrainPatch::GetSouthWest() {
	return south_west_corner_;
}

const vec2& TerrainPatch::GetNorthEast() {
	return north_east_corner_;
}

bool TerrainPatch::GetEdgeFlagValue(unsigned int edge_flag) {
	return CheckFlag(hi_res_edge_flags_, edge_flag);
}

void TerrainPatch::GetPosAndNormal(float normalized_x, float normalized_y, vec3& pos, vec3& normal) const {
	normalized_x; normalized_y; pos; normal;
}

GeometryBase::GeometryVolatility TerrainPatch::GetGeometryVolatility() const {
	return GeometryBase::kGeomStatic;
}

void TerrainPatch::SetGeometryVolatility(GeometryVolatility) {
}

unsigned int TerrainPatch::GetMaxVertexCount() const {
	return vertex_count_;
}

unsigned int TerrainPatch::GetMaxIndexCount() const {
	return triangle_count_ * 3;
}

unsigned int TerrainPatch::GetVertexCount() const {
	return vertex_count_;
}

unsigned int TerrainPatch::GetIndexCount() const {
	return triangle_count_ * 3;
}

unsigned int TerrainPatch::GetUVSetCount() const {
	return 2;
}

float* TerrainPatch::GetVertexData() const {
	return vertex_data_;
}

float* TerrainPatch::GetUVData(unsigned int uv_set) const {
	float* _uv_set = 0;
	if (uv_set >= 0 && uv_set < 2) {
		_uv_set = uv_data_[uv_set];
	}
	return _uv_set;
}

vtx_idx_t* TerrainPatch::GetIndexData() const {
	return index_data_;
}

uint8* TerrainPatch::GetColorData() const {
	return 0;
}

void TerrainPatch::operator=(const TerrainPatch&) {
	deb_assert(false);
}



}
