
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uibillboardgeometry.h"
#include "../../lepra/include/cyclicarray.h"



namespace uitbc {



BillboardGeometry::BillboardGeometry(float aspect_ratio, unsigned uv_set_count):
	uv_set_count_(uv_set_count) {
	SetAlwaysVisible(false);
	SetPrimitiveType(kTriangleStrip);

	const float tri_strip[] =
	{
		-1,0,+1/aspect_ratio,
		+1,0,+1/aspect_ratio,
		-1,0,-1/aspect_ratio,
		+1,0,-1/aspect_ratio,
	};
	::memcpy(vertex_data_, tri_strip, sizeof(vertex_data_));
	index_data_[0] = 0;
	index_data_[1] = 1;
	index_data_[2] = 2;
	index_data_[3] = 3;

	uv_data_ = new float[2*4*uv_set_count_];
	const float width = 1.0f/uv_set_count_;
	const float margin_x = 1/256.0;
	const float uv_tri_strip_data[] =
	{
		margin_x,   0,
		1-margin_x, 0,
		margin_x,   1,
		1-margin_x, 1,
	};
	for (unsigned x = 0; x < uv_set_count_; ++x) {
		const float x0 = width*x;
		int base = x*4*2;
		for (int y = 0; y < 4; ++y) {
			uv_data_[base+y*2+0] = uv_tri_strip_data[y*2+0]*width + x0;	// x
			uv_data_[base+y*2+1] = uv_tri_strip_data[y*2+1];	// y
		}
	}
}

BillboardGeometry::~BillboardGeometry() {
	delete uv_data_;
	uv_data_ = 0;
}

unsigned BillboardGeometry::GetMaxVertexCount() const {
	return LEPRA_ARRAY_COUNT(vertex_data_) / 3;
}

unsigned BillboardGeometry::GetMaxIndexCount() const {
	return LEPRA_ARRAY_COUNT(index_data_);
}

unsigned BillboardGeometry::GetVertexCount() const {
	return LEPRA_ARRAY_COUNT(vertex_data_) / 3;
}

unsigned BillboardGeometry::GetIndexCount() const {
	return LEPRA_ARRAY_COUNT(index_data_);
}

unsigned BillboardGeometry::GetUVSetCount() const {
	return uv_set_count_;
}

float* BillboardGeometry::GetVertexData() const {
	return (float*)&vertex_data_[0];
}

void BillboardGeometry::SetVertexData(const float vertex_data[12]) {
	::memcpy(vertex_data_, vertex_data, sizeof(vertex_data_));
}

float* BillboardGeometry::GetUVData(unsigned uv_set) const {
	return &uv_data_[uv_set*2*4];
}

vtx_idx_t* BillboardGeometry::GetIndexData() const {
	return (vtx_idx_t*)&index_data_[0];
}

uint8* BillboardGeometry::GetColorData() const {
	return 0;
}

tbc::GeometryBase::GeometryVolatility BillboardGeometry::GetGeometryVolatility() const {
	return kGeomStatic;
}

void BillboardGeometry::SetGeometryVolatility(tbc::GeometryBase::GeometryVolatility) {
}



}
