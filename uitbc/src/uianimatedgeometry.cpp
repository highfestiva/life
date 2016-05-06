/*
	Class:  AnimatedGeometry
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uianimatedgeometry.h"
#include "../../tbc/include/bones.h"
#include "../../lepra/include/lepraassert.h"

namespace uitbc {

AnimatedGeometry::AnimatedGeometry() :
	original_geometry_(0),
	vertex_count_(0),
	vertex_data_(0),
	normal_data_(0),
	bone_weight_group_count_(0),
	bone_weight_group_array_(0) {
}

AnimatedGeometry::~AnimatedGeometry() {
	vertex_count_ = 0;
	delete[] vertex_data_;
	vertex_data_ = 0;
	delete[] normal_data_;
	normal_data_ = 0;

	FreeAllBoneWeights();
}



void AnimatedGeometry::SetGeometry(tbc::GeometryBase* geometry) {
	original_geometry_ = geometry;
	SetPrimitiveType(original_geometry_->GetPrimitiveType());

	vertex_count_ = original_geometry_->GetMaxVertexCount();
	vertex_data_ = new float[vertex_count_ * 3];
	normal_data_ = new float[vertex_count_ * 3];

	original_geometry_->GenerateVertexNormalData();

	const float* original_v_data = original_geometry_->GetVertexData();
	const float* original_n_data = original_geometry_->GetNormalData();
	for (int i = 0; i < vertex_count_; i++) {
		int index = i * 3;
		vertex_data_[index + 0] = original_v_data[index + 0];
		vertex_data_[index + 1] = original_v_data[index + 1];
		vertex_data_[index + 2] = original_v_data[index + 2];

		normal_data_[index + 0] = original_n_data[index + 0];
		normal_data_[index + 1] = original_n_data[index + 1];
		normal_data_[index + 2] = original_n_data[index + 2];
	}
}



void AnimatedGeometry::AddBoneWeights(BoneWeightGroup& weights) {
	int new_count = bone_weight_group_count_ + 1;

	BoneWeightGroup* old_weight_group_array = bone_weight_group_array_;
	bone_weight_group_array_ = new BoneWeightGroup[new_count];
	::memcpy(bone_weight_group_array_, old_weight_group_array, sizeof(BoneWeightGroup)*bone_weight_group_count_);
	delete [] (old_weight_group_array);

	bone_weight_group_array_[bone_weight_group_count_] = weights;
	bone_weight_group_count_ = new_count;
}

void AnimatedGeometry::FreeAllBoneWeights() {
	for (int x = 0; x < bone_weight_group_count_; ++x) {
		delete[] (bone_weight_group_array_[x].vector_index_array_);
		bone_weight_group_array_[x].vector_index_array_ = 0;
		delete[] (bone_weight_group_array_[x].vector_weight_array_);
		bone_weight_group_array_[x].vector_weight_array_ = 0;
	}
	delete[] (bone_weight_group_array_);
	bone_weight_group_count_ = 0;
}

int AnimatedGeometry::GetBoneWeightGroupCount() const {
	return (bone_weight_group_count_);
}

const AnimatedGeometry::BoneWeightGroup& AnimatedGeometry::GetBoneWeightGroup(int bone_weight_group_index) const {
	return (bone_weight_group_array_[bone_weight_group_index]);
}



void AnimatedGeometry::SetBoneHierarchy(tbc::BoneHierarchy* bones) {
	bones_ = bones;
}



void AnimatedGeometry::UpdateAnimatedGeometry() {
	// TODO: this whole thing should usually be run on the GPU or any available vector processors.
	// Alex: ...but to do so while maintaining the goal of scalability, the following must be
	//       done...
	//
	//       Having two levels of materials (standard pipeline and pixel shaded materials), a
	//       third layer needs to be added - skinned materials. These materials only differ
	//       from the "standard" pixel shaded materials by their vertex shaders.
	//
	//       BUT!!! The code below can't be translated into a vertex shader!!! Why? Because
	//       there are no loops in shaders! (Not old version shaders, at least). We need to hard
	//       code things like how many bones there are per vertex. And the outer loop needs to
	//       be an outer loop in the renderer as well. This means one of three things:
	//
	//       1. We need to render the skinned material using several rendering passes (one
	//          pass per group), and between the passes we need to store the intermediate
	//          results somewhere, which means we need to use framebuffers. This CAN be faster
	//          than doing it on the CPU, but I wonder...
	//       2. OR we simply disallow more than one BoneWeightGroup.
	//       3. OR we write some weird translation routine that picks as many bones from
	//          different groups as the vertex shaders supports.
	//
	//       Is it worth the trouble? We might just stick with the code below and exploit the
	//       fact that all future CPU:s have at least two cores.

	const float* original_v_data = original_geometry_->GetVertexData();
	const float* original_n_data = original_geometry_->GetNormalData();

	for (int x = 0; x < bone_weight_group_count_; ++x) {
		const BoneWeightGroup& group = bone_weight_group_array_[x];
		deb_assert(group.bone_count_ <= bones_->GetBoneCount());

		unsigned vector_weight_index = 0;
		for (int y = 0; y < group.vector_index_count_; ++y) {
			const unsigned vector_index = group.vector_index_array_[y] * 3;
			deb_assert(vector_index+2 < original_geometry_->GetVertexCount()*3);

			const vec3 v(original_v_data[vector_index+0], original_v_data[vector_index+1], original_v_data[vector_index+2]);
			const vec3 n(original_n_data[vector_index+0], original_n_data[vector_index+1], original_n_data[vector_index+2]);
			vec3 weighted_vertex;
			vec3 weighted_normal;
			for (int z = 0; z < group.bone_count_; ++z, ++vector_weight_index) {
				const int bone_index = group.bone_index_array_[z];
				const xform& transform = bones_->GetRelativeBoneTransformation(bone_index);
				const float weight = group.vector_weight_array_[vector_weight_index];
				weighted_vertex += transform.Transform(v)*weight;
				weighted_normal += transform.Transform(n)*weight;
			}
			vertex_data_[vector_index+0] = weighted_vertex.x;
			vertex_data_[vector_index+1] = weighted_vertex.y;
			vertex_data_[vector_index+2] = weighted_vertex.z;
			normal_data_[vector_index+0] = weighted_normal.x;
			normal_data_[vector_index+1] = weighted_normal.y;
			normal_data_[vector_index+2] = weighted_normal.z;
		}
	}

	GeometryBase::SetVertexDataChanged(true);
}

tbc::GeometryBase::GeometryVolatility AnimatedGeometry::GetGeometryVolatility() const {
	return tbc::GeometryBase::kGeomVolatile;
}

void AnimatedGeometry::SetGeometryVolatility(GeometryVolatility) {
}

unsigned int AnimatedGeometry::GetMaxVertexCount() const {
	return original_geometry_->GetMaxVertexCount();
}

unsigned int AnimatedGeometry::GetMaxIndexCount() const {
	return original_geometry_->GetMaxIndexCount();
}

unsigned int AnimatedGeometry::GetVertexCount() const {
	return original_geometry_->GetVertexCount();
}

unsigned int AnimatedGeometry::GetIndexCount() const {
	return original_geometry_->GetIndexCount();
}

unsigned int AnimatedGeometry::GetUVSetCount() const {
	return original_geometry_->GetUVSetCount();
}

float* AnimatedGeometry::GetVertexData() const {
	return vertex_data_;
}

float* AnimatedGeometry::GetUVData(unsigned int uv_set) const {
	return original_geometry_->GetUVData(uv_set);
}

vtx_idx_t* AnimatedGeometry::GetIndexData() const {
	return original_geometry_->GetIndexData();
}

uint8* AnimatedGeometry::GetColorData() const {
	return original_geometry_->GetColorData();
}

float* AnimatedGeometry::GetNormalData() const {
	return normal_data_;
}

tbc::GeometryBase::ColorFormat AnimatedGeometry::GetColorFormat() const {
	return original_geometry_->GetColorFormat();
}

tbc::GeometryBase* AnimatedGeometry::GetOriginalGeometry() {
	return original_geometry_;
}



}
