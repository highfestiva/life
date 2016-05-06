
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uigeometry2d.h"
#include "../../lepra/include/lepraassert.h"
#include "../include/uitbc.h"



namespace uitbc {



Geometry2D::Geometry2D(unsigned vertex_format, int vertex_capacity, int triangle_capacity):
	vertex_format_(vertex_format),
	vertex_data_(0),
	color_data_(0),
	uv_data_(0),
	vertex_capacity_(0),
	triangle_capacity_(triangle_capacity),
	vertex_count_(0),
	triangle_count_(0) {
	if (IsFlagSet(kVtxIndex16)) {
		triangle_data16_ = new uint16[triangle_capacity];
	} else {
		triangle_data32_ = new uint32[triangle_capacity];
	}

	Init(vertex_format, vertex_capacity, triangle_capacity);
}

Geometry2D::~Geometry2D() {
	delete[] (float*)vertex_data_;	// Cast to dumb type; we don't want any destructor anyways.
	delete[] color_data_;
	delete[] uv_data_;
	if (IsFlagSet(kVtxIndex16)) {
		delete[] triangle_data16_;
	} else {
		delete[] triangle_data32_;
	}
}

void Geometry2D::Init(unsigned vertex_format, int vertex_capacity, int triangle_capacity) {
	vertex_format_ = vertex_format;

	if (vertex_data_ == 0) {
		if (IsFlagSet(kVtxInterleaved)) {
			unsigned flags = (vertex_format_ & (kVtxUv | kVtxRgb));
			switch(flags) {
				case 0: vertex_data_ = new VertexXY[vertex_capacity]; break;
				case kVtxUv: vertex_data_ = new VertexXYUV[vertex_capacity]; break;
				case kVtxRgb: vertex_data_ = new VertexXYRGB[vertex_capacity]; break;
				case kVtxUv | kVtxRgb: vertex_data_ = new VertexXYUVRGB[vertex_capacity]; break;
			}
		} else {
			vertex_data_ = new float[vertex_capacity * 2];
			if (IsFlagSet(kVtxUv)) {
				uv_data_ = new float[vertex_capacity * 2];
			}
			if (IsFlagSet(kVtxRgb)) {
				color_data_ = new float[vertex_capacity * 3];
			}
		}
	} else {
		deb_assert(!IsFlagSet(kVtxInterleaved));	// Cannot handle interleaved yet.

		if (IsFlagSet(kVtxUv) != (uv_data_ != 0)) {
			if (uv_data_) {
				delete (uv_data_);
				uv_data_ = 0;
			} else {
				uv_data_ = new float[vertex_capacity * 2];
			}
		}
		if (IsFlagSet(kVtxRgb) != (color_data_ != 0)) {
			if (color_data_) {
				delete (color_data_);
				color_data_ = 0;
			} else {
				color_data_ = new float[vertex_capacity * 3];
			}
		}
		AssureVertexCapacity(vertex_capacity);
		AssureTriangleCapacity(triangle_capacity);
		Reset();
	}
}

void Geometry2D::ReallocVertexBuffers(int vertex_capacity) {
	if (IsFlagSet(kVtxInterleaved)) {
		unsigned flags = (vertex_format_ & (kVtxUv | kVtxRgb));
		switch(flags) {
			case 0: Realloc(&vertex_data_, vertex_capacity * sizeof(VertexXY), vertex_count_ * sizeof(VertexXY)); break;
			case kVtxUv: Realloc(&vertex_data_, vertex_capacity * sizeof(VertexXYUV), vertex_count_ * sizeof(VertexXYUV)); break;
			case kVtxRgb: Realloc(&vertex_data_, vertex_capacity * sizeof(VertexXYRGB), vertex_count_ * sizeof(VertexXYRGB)); break;
			case kVtxUv | kVtxRgb: Realloc(&vertex_data_, vertex_capacity * sizeof(VertexXYUVRGB), vertex_count_ * sizeof(VertexXYUVRGB)); break;
		}
	} else {
		Realloc(&vertex_data_, vertex_capacity * 2 * sizeof(float), vertex_count_ * 2 * sizeof(float));
		if (color_data_) {
			void* temp_color = (void*)color_data_;
			Realloc(&temp_color, vertex_capacity * 3 * sizeof(float), vertex_count_ * 3 * sizeof(float));
			color_data_ = (float*)temp_color;
		}
		if (uv_data_) {
			void* temp_uv = (void*)uv_data_;
			Realloc(&temp_uv, vertex_capacity * 2 * sizeof(float), vertex_count_ * 2 * sizeof(float));
			uv_data_ = (float*)temp_uv;
		}
	}
	vertex_capacity_ = vertex_capacity;
}

void Geometry2D::ReallocTriangleBuffer(int triangle_capacity) {
	if (IsFlagSet(kVtxIndex16)) {
		uint16* triangle_data16 = new uint16[triangle_capacity * 3];
		::memcpy(triangle_data16, triangle_data16_, std::min(triangle_capacity, triangle_count_) * 3 * sizeof(uint16));
		delete[] triangle_data16_;
		triangle_data16_ = triangle_data16;
	} else {
		uint32* triangle_data32 = new uint32[triangle_capacity * 3];
		::memcpy(triangle_data32, triangle_data32_, std::min(triangle_capacity, triangle_count_) * 3 * sizeof(uint32));
		delete[] triangle_data32_;
		triangle_data32_ = triangle_data32;
	}
	triangle_capacity_ = triangle_capacity;
}

void Geometry2D::AssureVertexCapacity(int vertex_capacity) {
	if (vertex_capacity >= vertex_capacity_) {
		int new_capacity = vertex_capacity + vertex_capacity_ / 2;
		ReallocVertexBuffers(new_capacity);
	}
}

void Geometry2D::AssureTriangleCapacity(int triangle_capacity) {
	if (triangle_capacity >= triangle_capacity_) {
		int new_capacity = triangle_capacity + triangle_capacity_ / 2;
		ReallocTriangleBuffer(new_capacity);
	}
}

uint32 Geometry2D::SetVertex(float x, float y) {
	AssureVertexCapacity(vertex_count_ + 1);

	if (IsFlagSet(kVtxInterleaved)) {
		VertexXY* vertex_data = (VertexXY*)vertex_data_;
		vertex_data[vertex_count_].x = x;
		vertex_data[vertex_count_].y = y;
	} else {
		int vertex_index = (vertex_count_ << 1);
		float* vertex_data = (float*)vertex_data_;
		vertex_data[vertex_index + 0] = x;
		vertex_data[vertex_index + 1] = y;
	}
	return vertex_count_++;
}

uint32 Geometry2D::SetVertex(float x, float y, float u, float v) {
	AssureVertexCapacity(vertex_count_ + 1);

	if (IsFlagSet(kVtxInterleaved)) {
		VertexXYUV* vertex_data = (VertexXYUV*)vertex_data_;
		vertex_data[vertex_count_].x = x;
		vertex_data[vertex_count_].y = y;
		vertex_data[vertex_count_].z = 0;
		vertex_data[vertex_count_].w = 1;
		vertex_data[vertex_count_].u = u;
		vertex_data[vertex_count_].v = v;
	} else {
		int vertex_index = (vertex_count_ << 1);
		float* vertex_data = (float*)vertex_data_;
		vertex_data[vertex_index + 0] = x;
		vertex_data[vertex_index + 1] = y;
		uv_data_[vertex_index + 0] = u;
		uv_data_[vertex_index + 1] = v;
	}
	return vertex_count_++;
}

uint32 Geometry2D::SetVertex(float x, float y, float r, float g, float b) {
	AssureVertexCapacity(vertex_count_ + 1);

	if (IsFlagSet(kVtxInterleaved)) {
		VertexXYRGB* vertex_data = (VertexXYRGB*)vertex_data_;
		vertex_data[vertex_count_].x = x;
		vertex_data[vertex_count_].y = y;
		vertex_data[vertex_count_].z = 0;
		vertex_data[vertex_count_].w = 1;
		vertex_data[vertex_count_].blue_  = (uint8)(b * 255.0f);
		vertex_data[vertex_count_].green_ = (uint8)(g * 255.0f);
		vertex_data[vertex_count_].red_   = (uint8)(r * 255.0f);
		vertex_data[vertex_count_].alpha_ = 255;
	} else {
		int vertex_index = (vertex_count_ << 1);
		float* vertex_data = (float*)vertex_data_;
		vertex_data[vertex_index + 0] = x;
		vertex_data[vertex_index + 1] = y;

		vertex_index += vertex_count_; // vertex_index = mCurrentVertex * 3.
		color_data_[vertex_index + 0] = r;
		color_data_[vertex_index + 1] = g;
		color_data_[vertex_index + 2] = b;
	}
	return vertex_count_++;
}

uint32 Geometry2D::SetVertex(float x, float y, float u, float v, float r, float g, float b) {
	AssureVertexCapacity(vertex_count_ + 1);

	if (IsFlagSet(kVtxInterleaved)) {
		VertexXYUVRGB* vertex_data = (VertexXYUVRGB*)vertex_data_;
		vertex_data[vertex_count_].x = x;
		vertex_data[vertex_count_].y = y;
		vertex_data[vertex_count_].z = 0;
		vertex_data[vertex_count_].w = 1;
		vertex_data[vertex_count_].u = u;
		vertex_data[vertex_count_].v = v;

		vertex_data[vertex_count_].blue_  = (uint8)(b * 255.0f);
		vertex_data[vertex_count_].green_ = (uint8)(g * 255.0f);
		vertex_data[vertex_count_].red_   = (uint8)(r * 255.0f);
		vertex_data[vertex_count_].alpha_ = 255;
	} else {
		int vertex_index = (vertex_count_ << 1);
		float* vertex_data = (float*)vertex_data_;
		vertex_data[vertex_index + 0] = x;
		vertex_data[vertex_index + 1] = y;
		uv_data_[vertex_index + 0] = u;
		uv_data_[vertex_index + 1] = v;

		vertex_index += vertex_count_; // vertex_index = mCurrentVertex * 3.
		color_data_[vertex_index + 0] = r;
		color_data_[vertex_index + 1] = g;
		color_data_[vertex_index + 2] = b;
	}
	return vertex_count_++;
}

void Geometry2D::SetTriangle(uint32 v1, uint32 v2, uint32 v3) {
	AssureTriangleCapacity(triangle_count_ + 1);

	int triangle_index = triangle_count_ * 3;
	if (IsFlagSet(kVtxIndex16)) {
		triangle_data16_[triangle_index + 0] = (uint16)v1;
		triangle_data16_[triangle_index + 1] = (uint16)v2;
		triangle_data16_[triangle_index + 2] = (uint16)v3;
	} else {
		triangle_data32_[triangle_index + 0] = v1;
		triangle_data32_[triangle_index + 1] = v2;
		triangle_data32_[triangle_index + 2] = v3;
	}
	triangle_count_++;
}

void Geometry2D::Reset() {
	vertex_count_ = 0;
	triangle_count_ = 0;
}

void Geometry2D::Realloc(void** data, size_t new_size, size_t bytes_to_copy) {
	uint8* new_data = new uint8[new_size];
	::memcpy(new_data, *data, std::min(new_size, bytes_to_copy));
	delete[] (float*)(*data);	// Cast to dumb type, we don't want any destructor anyways.
	*data = new_data;
}

Geometry2D::Geometry2D(const Geometry2D&) {
	deb_assert(false);
}

void Geometry2D::operator=(const Geometry2D&) {
	deb_assert(false);
}



}
