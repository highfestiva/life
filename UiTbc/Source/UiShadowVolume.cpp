
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uishadowvolume.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/resourcetracker.h"
#include "../../lepra/include/thread.h"



namespace uitbc {



ShadowVolume::ShadowVolume(tbc::GeometryBase* parent_geometry):
	vertex_data_(0),
	index_data_(0),
	triangle_orientation_(0),
	vertex_count_(0),
	triangle_count_(0),
	parent_vertex_count_(0),
	max_triangle_count_(0),
	parent_geometry_(parent_geometry) {
	LEPRA_ACQUIRE_RESOURCE(ShadowVolume);

	LEPRA_DEBUG_CODE(name_ = "Shdw->" + parent_geometry->name_);

	SetPrimitiveType(tbc::GeometryBase::kTriangles);

	if (parent_geometry_->GetEdgeData() == 0) {
		parent_geometry_->GenerateEdgeData();
	}

	SetScale(parent_geometry->GetScale());

	max_triangle_count_ = parent_geometry_->GetMaxTriangleCount() * 2 + parent_geometry_->GetEdgeCount() * 2;
	triangle_count_ = 0;
}

ShadowVolume::~ShadowVolume() {
	delete[] vertex_data_;
	vertex_data_ = 0;
	delete[] index_data_;
	index_data_ = 0;
	delete[] triangle_orientation_;
	triangle_orientation_ = 0;
	LEPRA_ADD_RESOURCE(ShadowVolume_Triangles, -(int)max_triangle_count_);

	LEPRA_RELEASE_RESOURCE(ShadowVolume);
}

tbc::GeometryBase* ShadowVolume::GetParentGeometry() {
	return parent_geometry_;
}

void ShadowVolume::SetParentGeometry(tbc::GeometryBase* parent_geometry) {
	parent_geometry_ = parent_geometry;

	if (parent_geometry_->GetEdgeData() == 0) {
		parent_geometry_->GenerateEdgeData();
	}

	max_triangle_count_ = parent_geometry_->GetMaxTriangleCount() * 2 + parent_geometry_->GetEdgeCount() * 2;
	triangle_count_ = 0;
}

unsigned ShadowVolume::GetMaxVertexCount() const {
	return parent_geometry_->GetMaxVertexCount() * 2;
}

unsigned ShadowVolume::GetMaxIndexCount() const {
	return max_triangle_count_ * 3;
}

unsigned ShadowVolume::GetVertexCount() const {
	return vertex_count_;
}

unsigned ShadowVolume::GetIndexCount() const {
	return triangle_count_ * 3;
}

unsigned ShadowVolume::GetUVSetCount() const {
	return 0;
}

float* ShadowVolume::GetVertexData() const {
	return vertex_data_;
}

float* ShadowVolume::GetUVData(unsigned int) const {
	return 0;
}

vtx_idx_t* ShadowVolume::GetIndexData() const {
	return index_data_;
}

uint8* ShadowVolume::GetColorData() const {
	return 0;
}

float* ShadowVolume::GetNormalData() const {
	return 0;
}

tbc::GeometryBase::GeometryVolatility ShadowVolume::GetGeometryVolatility() const {
	if (parent_geometry_) {
		return (parent_geometry_->GetGeometryVolatility());
	}

	return (tbc::GeometryBase::kGeomVolatile);
}

void ShadowVolume::SetGeometryVolatility(tbc::GeometryBase::GeometryVolatility volatility) {
	if (parent_geometry_) {
		parent_geometry_->SetGeometryVolatility(volatility);
	}
}

void ShadowVolume::InitVertices() {
	bool init_vertices = parent_geometry_->GetVertexDataChanged();

	if (parent_geometry_->GetVertexCount() > parent_vertex_count_) {
		parent_vertex_count_  = parent_geometry_->GetVertexCount();
		vertex_count_  = parent_vertex_count_ * 2;
		delete[] vertex_data_;
		vertex_data_ = new float[vertex_count_ * 3];
		init_vertices = true;
	}

	if (init_vertices == true) {
		// Copy vertex data from parent, twice.
		const float* parent_vertex_data = parent_geometry_->GetVertexData();
		::memcpy(vertex_data_, parent_vertex_data, parent_vertex_count_ * 3 * sizeof(float));
		::memcpy(&vertex_data_[parent_vertex_count_ * 3], parent_vertex_data, parent_vertex_count_ * 3 * sizeof(float));
	}
}

void ShadowVolume::InitTO() {
	unsigned num_max_triangles = parent_geometry_->GetMaxTriangleCount() * 2 + parent_geometry_->GetEdgeCount() * 2;

	if (num_max_triangles > max_triangle_count_) {
		delete[] triangle_orientation_;
		triangle_orientation_ = 0;
		LEPRA_ADD_RESOURCE(ShadowVolume_Triangles, -(int)max_triangle_count_);
		delete[] index_data_;
		index_data_ = 0;

		max_triangle_count_ = num_max_triangles;
	}

	if (triangle_orientation_ == 0) {
		triangle_orientation_ = new TriangleOrientation[max_triangle_count_];
		LEPRA_ADD_RESOURCE(ShadowVolume_Triangles, +(int)max_triangle_count_);
	}

	if (index_data_ == 0) {
		index_data_ = new vtx_idx_t[max_triangle_count_ * 3];
	}
}

#define MACRO_ADDFRONTCAPTRIANGLE(to) \
{ \
	if (to.checked_ == false) \
	{ \
		*index_data++ = to.v0_; \
		*index_data++ = to.v1_; \
		*index_data++ = to.v2_; \
		++triangle_count_; \
		to.checked_ = true; \
	} \
}

#define MACRO_ADDBACKCAPTRIANGLE(to) \
{ \
	if (to.checked_ == false) \
	{ \
		*index_data++ = to.v0_ + parent_vertex_count_; \
		*index_data++ = to.v1_ + parent_vertex_count_; \
		*index_data++ = to.v2_ + parent_vertex_count_; \
		++triangle_count_; \
		to.checked_ = true; \
	} \
}

#define MACRO_ADDBACKCAPTRIANGLE_FLIPPED(to) \
{ \
	if (to.checked_ == false) \
	{ \
		*index_data++ = to.v2_ + parent_vertex_count_; \
		*index_data++ = to.v1_ + parent_vertex_count_; \
		*index_data++ = to.v0_ + parent_vertex_count_; \
		++triangle_count_; \
		to.checked_ = true; \
	} \
}

void ShadowVolume::UpdateShadowVolume(const vec3& light_pos, float shadow_range, const bool directional) {
	SetTransformation(parent_geometry_->GetTransformation());

	InitVertices();
	InitTO();

	// Transform the light position in object space.
	vec3 _light_pos;
	if (directional == true) {
		_light_pos = parent_geometry_->GetTransformation().orientation_.GetInverseRotatedVector(light_pos);
		_light_pos.Normalize(shadow_range);
	} else {
		_light_pos = parent_geometry_->GetTransformation().InverseTransform(light_pos);
	}

	// Calculate the shadow meshes' vertex positions.
	parent_geometry_->GenerateSurfaceNormalData();
	const vtx_idx_t* indices = parent_geometry_->GetIndexData();
	const float* surface_normal_data = parent_geometry_->GetSurfaceNormalData();
	const float* vertex_data = parent_geometry_->GetVertexData();

	// Warning: optimized code has no support for strips or other primitives!
	const unsigned triangle_count = parent_geometry_->GetTriangleCount();
	if (directional) {
		// Calculate triangle orientations relative to light source.
		TriangleOrientation* t = triangle_orientation_;
		TriangleOrientation* end = t + triangle_count;
		for (; t != end; ++t, indices+=3, surface_normal_data+=3) {
			// Get the vector between one corner of the triangle and the light source.
			t->v0_ = indices[0];
			t->v1_ = indices[1];
			t->v2_ = indices[2];
			t->checked_ = false;

			float dx = _light_pos.data_[0] * surface_normal_data[0];
			float dy = _light_pos.data_[1] * surface_normal_data[1];
			float dz = _light_pos.data_[2] * surface_normal_data[2];
			float dot = dx+dy+dz;
			// Light position is now treated as a direction instead.
			t->is_front_facing_ = (dot < -1e-8f);
		}

		// Move vertex twins away from lightsource.
		const float* source = vertex_data_;
		float* target = &vertex_data_[parent_vertex_count_*3];
		const float* end_source = target;
		while (source != end_source) {
			*target++ = _light_pos.x + *source++;
			*target++ = _light_pos.y + *source++;
			*target++ = _light_pos.z + *source++;
		}
	} else { // Point or spot light.
		const vtx_idx_t* indices = parent_geometry_->GetIndexData();
		// Calculate triangle orientations relative to light source.
		for (unsigned i = 0; i < triangle_count; i++, indices+=3, surface_normal_data+=3) {
			//if (parent_geometry_->GetPrimitiveType() == tbc::GeometryBase::kTriangles)
			{
				triangle_orientation_[i].v0_ = indices[0];
				triangle_orientation_[i].v1_ = indices[1];
				triangle_orientation_[i].v2_ = indices[2];
			}
			/*else {
				uint32 vertex_index[4];
				parent_geometry_->GetTriangleIndices(i, vertex_index);
				triangle_orientation_[i].v0_ = vertex_index[0];
				triangle_orientation_[i].v1_ = vertex_index[1];
				triangle_orientation_[i].v2_ = vertex_index[2];
			}*/

			// Get the vector between one corner of the triangle and the light source.
			const unsigned index = triangle_orientation_[i].v0_ * 3;
			// Optimized dot product.
			float dx = (vertex_data[index + 0] - _light_pos.data_[0]) * surface_normal_data[0];
			float dy = (vertex_data[index + 1] - _light_pos.data_[1]) * surface_normal_data[1];
			float dz = (vertex_data[index + 2] - _light_pos.data_[2]) * surface_normal_data[2];
			float dot = dx+dy+dz;
			triangle_orientation_[i].checked_ = false;
			triangle_orientation_[i].is_front_facing_ = (dot < -1e-8f);
		}

		const float shadow_range_squared = shadow_range * shadow_range;
		// Move vertex twins away from lightsource.
		for (unsigned i = 0; i < parent_vertex_count_; i++) {
			// Read original vertex.
			const int index0 = i * 3;
			vec3 vector(vertex_data_[index0 + 0] - _light_pos.x,
						  vertex_data_[index0 + 1] - _light_pos.y,
						  vertex_data_[index0 + 2] - _light_pos.z);

			// Move, and write to its "twin vertex".
			if (vector.GetLengthSquared() < shadow_range_squared) {
				vector.Normalize(shadow_range);

				const int index1 = (i + parent_vertex_count_) * 3;
				vertex_data_[index1 + 0] = (float)(_light_pos.x + vector.x);
				vertex_data_[index1 + 1] = (float)(_light_pos.y + vector.y);
				vertex_data_[index1 + 2] = (float)(_light_pos.z + vector.z);
			}
		}
	}

	tbc::GeometryBase::SetVertexDataChanged(true);


	// Check if we actually need to update triangle definitions, or if vertex data will suffice.
	if (triangle_count_ != 0) {
		// We check if orientation has changed.
		const quat& caster_orientation = parent_geometry_->GetTransformation().GetOrientation();
		const float orientation_diff = (previous_orientation_-caster_orientation).GetNorm();
		if (orientation_diff < 0.03f) {
			return;	// We only need to generate triangles if orientation has changed significantly.
		}
		previous_orientation_ = caster_orientation;
	}


	// Generate triangles.
	triangle_count_ = 0;
	if (parent_geometry_->GetEdgeData() == 0) {
		parent_geometry_->GenerateEdgeData();
	}
	const tbc::GeometryBase::Edge* edges = parent_geometry_->GetEdgeData();
	const unsigned edge_count = parent_geometry_->GetEdgeCount();
	vtx_idx_t* index_data = index_data_;
	for (unsigned i = 0; i < edge_count; ++i) {
		const Edge& edge = edges[i];
		const TriangleOrientation* ft = 0;	// Set this if we have a silhouette edge.
		TriangleOrientation& t0 = triangle_orientation_[edge.triangle_[0]];
		if (edge.triangle_count_ == 2) {
			TriangleOrientation& t1 = triangle_orientation_[edge.triangle_[1]];

			if (t0.is_front_facing_) {
				MACRO_ADDFRONTCAPTRIANGLE(t0);
				if (!t1.is_front_facing_) {
					ft = &t0;
				}
			} else {
				MACRO_ADDBACKCAPTRIANGLE(t0);
			}
			if (t1.is_front_facing_) {
				MACRO_ADDFRONTCAPTRIANGLE(t1);
				if (!t0.is_front_facing_) {
					ft = &t1;
				}
			} else {
				MACRO_ADDBACKCAPTRIANGLE(t1);
			}
		} else if (t0.is_front_facing_) {	// Assume only one triangle on edge.
			// Add front+back cap triangle.
			MACRO_ADDFRONTCAPTRIANGLE(t0);
			MACRO_ADDBACKCAPTRIANGLE_FLIPPED(t0);
			ft = &t0;
		}

		if (ft) {
			int v0;
			int v1;
			int v2;

			if ((int)ft->v0_ != edge.vertex_[0] &&
			   (int)ft->v0_ != edge.vertex_[1]) {
				v0 = ft->v2_;
				v1 = ft->v1_;
				v2 = ft->v0_;
			} else if((int)ft->v1_ != edge.vertex_[0] &&
				(int)ft->v1_ != edge.vertex_[1]) {
				v0 = ft->v0_;
				v1 = ft->v2_;
				v2 = ft->v1_;
			} else if((int)ft->v2_ != edge.vertex_[0] &&
				(int)ft->v2_ != edge.vertex_[1]) {
				v0 = ft->v1_;
				v1 = ft->v0_;
				v2 = ft->v2_;
			} else {
				v0 = 0;
				v1 = 0;
				v2 = 0;
			}

			// Create two triangles that are extruded from the edge.
			*index_data++ = v0;
			*index_data++ = v1;
			*index_data++ = v0 + parent_vertex_count_;
			*index_data++ = v1;
			*index_data++ = v1 + parent_vertex_count_;
			*index_data++ = v0 + parent_vertex_count_;

			triangle_count_ += 2;
			deb_assert(triangle_count_ < max_triangle_count_);
		}
	}

	tbc::GeometryBase::SetIndexDataChanged(true);
}



}
