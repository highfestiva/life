/*
	Class:  AnimatedGeometry
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "../../lepra/include/vector3d.h"
#include "../../tbc/include/geometrybase.h"
#include "uitbc.h"



namespace tbc {
class BoneHierarchy;
}



namespace uitbc {

class AnimatedGeometry : public tbc::GeometryBase {
public:
	struct BoneWeightGroup {
		static const int MAX_BONES_PER_VERTEX = 4;	// TODO: move this constant out, make it more global.
		int bone_count_;
		int bone_index_array_[MAX_BONES_PER_VERTEX];
		int vector_index_count_;		// Used both for vertices and normals.
		int* vector_index_array_;	// Used both for vertices and normals.
		float* vector_weight_array_;	// Used both for vertices and normals.
	};

	AnimatedGeometry();
	virtual ~AnimatedGeometry();

	void SetGeometry(tbc::GeometryBase* geometry);

	// Takes ownership of the allocated indices and weights.
	void AddBoneWeights(BoneWeightGroup& weights);
	void FreeAllBoneWeights();
	int GetBoneWeightGroupCount() const;
	const BoneWeightGroup& GetBoneWeightGroup(int bone_weight_group_index) const;

	void SetBoneHierarchy(tbc::BoneHierarchy* bones);	// TODO: replace this with user resource object?

	void UpdateAnimatedGeometry();

	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int uv_set) const;
	vtx_idx_t*     GetIndexData() const;
	uint8*         GetColorData() const;
	float*         GetNormalData() const;

	ColorFormat GetColorFormat() const;

	tbc::GeometryBase* GetOriginalGeometry();

protected:
private:
	tbc::GeometryBase* original_geometry_;

	tbc::BoneHierarchy* bones_;

	int vertex_count_;
	float* vertex_data_;
	float* normal_data_;
	int bone_weight_group_count_;
	BoneWeightGroup* bone_weight_group_array_;
};



}
