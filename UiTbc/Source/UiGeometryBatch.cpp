
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uigeometrybatch.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/resourcetracker.h"



namespace uitbc {



GeometryBatch::GeometryBatch(tbc::GeometryBase* geometry):
	vertex_(0),
	uv_(0),
	index_(0),
	color_(0),
	vertex_count_(0),
	index_count_(0),
	max_index_count_(0),
	uv_set_count_(0),
	geometry_instance_(geometry) {
	LEPRA_ACQUIRE_RESOURCE(GeometryBatch);
	SetPrimitiveType(tbc::GeometryBase::kTriangles);
	SetBasicMaterialSettings(geometry_instance_->GetBasicMaterialSettings());
}

GeometryBatch::~GeometryBatch() {
	ClearAllInstances();
	LEPRA_RELEASE_RESOURCE(GeometryBatch);
}

void GeometryBatch::SetInstances(const xform* displacement, const vec3& root_offset,
	int num_instances, uint32 random_seed,
	float x_scale_min, float x_scale_max,
	float y_scale_min, float y_scale_max,
	float z_scale_min, float z_scale_max) {
	ClearAllInstances();

	if (num_instances <= 0) {
		return;
	}

	vertex_count_ = geometry_instance_->GetVertexCount()  * num_instances;
	max_index_count_ = index_count_ = geometry_instance_->GetTriangleCount() * num_instances * 3;
	uv_set_count_ = geometry_instance_->GetUVSetCount();

	unsigned int i;
	unsigned int j;
	unsigned int k;

	if (vertex_count_ > 0) {
		//
		// Allocate memory...
		//

		vertex_ = new float[vertex_count_ * 3];

		if (uv_set_count_ > 0) {
			uv_ = new float*[uv_set_count_];
			for (i = 0; i < uv_set_count_; i++) {
				uv_[i] = new float[vertex_count_ * 2];
			}
		}

		if (index_count_ > 0) {
			index_ = new vtx_idx_t[index_count_];
		}

		if (geometry_instance_->GetColorData() != 0) {
			color_ = new uint8[vertex_count_ * 3];
		}

		//
		// Copy data.
		//

		unsigned int vertex_count  = geometry_instance_->GetVertexCount();
		unsigned int triangle_count = geometry_instance_->GetTriangleCount();
		float* vertex = geometry_instance_->GetVertexData();
		unsigned char* color = geometry_instance_->GetColorData();

		int v_index = 0;
		int t_index = 0;
		int uv_index = 0;
		for (i = 0; i < (unsigned int)num_instances; i++) {
			float x_scale = Random::Uniform(random_seed, x_scale_min, x_scale_max);
			float y_scale = Random::Uniform(random_seed, y_scale_min, y_scale_max);
			float z_scale = Random::Uniform(random_seed, z_scale_min, z_scale_max);

			const xform& transform = displacement[i];
			int index = 0;
			int uv_index2 = 0;
			for (j = 0; j < vertex_count; j++) {
				vec3 pos(
					(vertex[index + 0] + root_offset.x) * x_scale,
					(vertex[index + 1] + root_offset.y) * y_scale,
					(vertex[index + 2] + root_offset.z) * z_scale);
				pos = transform.GetOrientation()*pos + transform.GetPosition();
				vertex_[v_index + 0] = pos.x;
				vertex_[v_index + 1] = pos.y;
				vertex_[v_index + 2] = pos.z;

				if (color != 0) {
					color_[v_index + 0] = color[index + 0];
					color_[v_index + 1] = color[index + 1];
					color_[v_index + 2] = color[index + 2];
				}

				for (k = 0; k < uv_set_count_; k++) {
					float* uv = geometry_instance_->GetUVData(k);
					uv_[k][uv_index + 0] = uv[uv_index2 + 0];
					uv_[k][uv_index + 1] = uv[uv_index2 + 1];
				}

				v_index += 3;
				index += 3;
				uv_index += 2;
				uv_index2 += 2;
			}

			unsigned int ti_offset = vertex_count * i;
			index = 0;
			for (j = 0; j < triangle_count; j++) {
				uint32 tri_index[4];
				geometry_instance_->GetTriangleIndices(j, tri_index);
				index_[t_index + 0] = tri_index[0] + ti_offset;
				index_[t_index + 1] = tri_index[1] + ti_offset;
				index_[t_index + 2] = tri_index[2] + ti_offset;
				t_index += 3;
				index += 3;
			}
		}
	}
}

void GeometryBatch::ClearAllInstances() {
	if (vertex_ != 0) {
		delete[] vertex_;
		vertex_ = 0;
	}

	if (uv_ != 0) {
		for (unsigned int i = 0; i < uv_set_count_; i++) {
			delete[] uv_[i];
		}

		delete[] uv_;
		uv_ = 0;
	}

	if (index_ != 0) {
		delete[] index_;
		index_ = 0;
	}

	if (color_ != 0) {
		delete[] color_;
		color_ = 0;
	}

	vertex_count_ = 0;
	index_count_ = 0;
	max_index_count_ = 0;
	uv_set_count_ = 0;
}

void GeometryBatch::SetGeometryVolatility(GeometryVolatility) {
}

tbc::GeometryBase::GeometryVolatility GeometryBatch::GetGeometryVolatility() const {
	return tbc::GeometryBase::kGeomStatic;
}

tbc::GeometryBase::ColorFormat GeometryBatch::GetColorFormat() const {
	return tbc::GeometryBase::kColorRgb;
}

unsigned int GeometryBatch::GetMaxVertexCount()  const {
	return vertex_count_;
}

unsigned int GeometryBatch::GetMaxIndexCount() const {
	return max_index_count_;
}

unsigned int GeometryBatch::GetVertexCount()  const {
	return vertex_count_;
}

unsigned int GeometryBatch::GetIndexCount() const {
	return index_count_;
}

unsigned int GeometryBatch::GetUVSetCount()    const {
	return uv_set_count_;
}

float* GeometryBatch::GetVertexData() const {
	return vertex_;
}

float* GeometryBatch::GetUVData(unsigned int uv_set) const {
	if (uv_set < uv_set_count_) {
		return uv_[uv_set];
	}
	return 0;
}

vtx_idx_t* GeometryBatch::GetIndexData() const {
	return index_;
}

void GeometryBatch::SetIndexData(vtx_idx_t* index_data, unsigned index_count, unsigned max_index_count) {
	index_ = index_data;
	index_count_ = index_count;
	max_index_count_ = max_index_count;
}

uint8* GeometryBatch::GetColorData() const {
	return color_;
}



}
