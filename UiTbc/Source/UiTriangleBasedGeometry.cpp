
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uitrianglebasedgeometry.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/resourcetracker.h"



namespace uitbc {



TriangleBasedGeometry::TriangleBasedGeometry() :
	vertex_count_(0),
	index_count_(0),
	max_index_count_(0),
	uv_set_count_(0),
	vertex_data_(0),
	uv_data_(0),
	color_data_(0),
	index_data_(0),
	color_format_(kColorRgb),
	geometry_volatility_(tbc::GeometryBase::kGeomStatic) {
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);
	SetPrimitiveType(tbc::GeometryBase::kTriangles);
}

TriangleBasedGeometry::TriangleBasedGeometry(const TriangleBasedGeometry& geometry) :
	vertex_count_(0),
	index_count_(0),
	max_index_count_(0),
	uv_set_count_(0),
	vertex_data_(0),
	uv_data_(0),
	color_data_(0),
	index_data_(0),
	color_format_(kColorRgb),
	geometry_volatility_(tbc::GeometryBase::kGeomStatic) {
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);
	SetPrimitiveType(tbc::GeometryBase::kTriangles);
	Copy(geometry);
}

TriangleBasedGeometry::TriangleBasedGeometry(const vec3* vertices,
                                             const vec3* vertex_normals,
                                             const Vector2D<float>* uv,
                                             const Color* color,
                                             ColorFormat color_format,
                                             const uint32* indices,
                                             unsigned int vertex_count,
                                             unsigned int index_count,
					     tbc::GeometryBase::PrimitiveType primitive_type,
                                             tbc::GeometryBase::GeometryVolatility geom_type) :
	vertex_count_(0),
	index_count_(0),
	max_index_count_(0),
	uv_set_count_(0),
	vertex_data_(0),
	uv_data_(0),
	color_data_(0),
	index_data_(0),
	color_format_(kColorRgb),
	geometry_volatility_(tbc::GeometryBase::kGeomStatic) {
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);

	Set(vertices,
		vertex_normals,
		uv,
		color,
		color_format,
		indices,
		vertex_count,
		index_count,
		primitive_type,
		geom_type);
}

TriangleBasedGeometry::TriangleBasedGeometry(const float* vertex_data,
                                             const float* vertex_normals,
                                             const float* uv_data,
                                             const uint8* color_data,
                                             ColorFormat color_format,
                                             const uint32* indices,
                                             unsigned int vertex_count,
                                             unsigned int index_count,
					     tbc::GeometryBase::PrimitiveType primitive_type,
                                             tbc::GeometryBase::GeometryVolatility geom_type) :
	vertex_count_(0),
	index_count_(0),
	max_index_count_(0),
	uv_set_count_(0),
	vertex_data_(0),
	uv_data_(0),
	color_data_(0),
	index_data_(0),
	color_format_(kColorRgb),
	geometry_volatility_(tbc::GeometryBase::kGeomStatic) {
	LEPRA_ACQUIRE_RESOURCE(TriangleBasedGeometry);

	Set(vertex_data,
		vertex_normals,
		uv_data,
		color_data,
		color_format,
		indices,
		vertex_count,
		index_count,
		primitive_type,
		geom_type);
}

void TriangleBasedGeometry::Set(const vec3* vertices,
                                const vec3* vertex_normals,
                                const Vector2D<float>* uv,
                                const Color* color,
                                ColorFormat color_format,
                                const uint32* indices,
                                unsigned int vertex_count,
                                unsigned int index_count,
				tbc::GeometryBase::PrimitiveType primitive_type,
                                tbc::GeometryBase::GeometryVolatility geom_type) {
	ClearAll();

	SetPrimitiveType(primitive_type);
	geometry_volatility_ = geom_type;

	if (vertex_count > 0 && vertices != 0) {
		unsigned int i;
		int index;

		vertex_count_ = vertex_count;
		index_count_ = index_count;
		max_index_count_ = index_count;

		// Copy vertex data.
		vertex_data_ = new float[vertex_count_ * 3];
		for (i = 0, index = 0; i < vertex_count_; i++, index += 3) {
			vertex_data_[index + 0] = vertices[i].x;
			vertex_data_[index + 1] = vertices[i].y;
			vertex_data_[index + 2] = vertices[i].z;
		}

		if (vertex_normals != 0) {
			// Copy normal data.
			tbc::GeometryBase::AllocVertexNormalData();
			float* vertex_normal_data = tbc::GeometryBase::GetNormalData();
			for (i = 0, index = 0; i < vertex_count_; i++, index += 3) {
				vertex_normal_data[index + 0] = vertex_normals[i].x;
				vertex_normal_data[index + 1] = vertex_normals[i].y;
				vertex_normal_data[index + 2] = vertex_normals[i].z;
			}
		}

		if (uv != 0) {
			AddUVSet(uv);
		}

		if (color != 0) {
			// Copy color data.
			color_format_ = color_format;

			int size = 4;
			if (color_format_ == kColorRgb)
				size = 3;

			color_data_ = new uint8[vertex_count_ * size];
			for (i = 0, index = 0; i < vertex_count_; i++, index += size) {
				color_data_[index + 0] = color[i].red_;
				color_data_[index + 1] = color[i].green_;
				color_data_[index + 2] = color[i].blue_;

				if (color_format_ == kColorRgba)
					color_data_[index + 3] = color[i].alpha_;
			}
		}

		if (indices != 0) {
			// Copy index data.
			index_data_ = new vtx_idx_t[index_count_];
			for (i = 0; i < index_count_; i++) {
				index_data_[i] = (vtx_idx_t)indices[i];
			}
		}
	}

	tbc::GeometryBase::CalculateBoundingRadius();
}

void TriangleBasedGeometry::Set(const float* vertex_data,
                                const float* vertex_normals,
                                const float* uv_data,
                                const uint8* color_data,
                                ColorFormat color_format,
                                const uint32* indices,
                                unsigned int vertex_count,
                                unsigned int index_count,
				tbc::GeometryBase::PrimitiveType primitive_type,
                                tbc::GeometryBase::GeometryVolatility geom_type) {
	ClearAll();

	SetPrimitiveType(primitive_type);
	geometry_volatility_ = geom_type;

	if (vertex_count > 0 && vertex_data != 0) {
		unsigned int i;
		int index;

		vertex_count_ = vertex_count;
		index_count_ = index_count;
		max_index_count_ = index_count;

		// Copy vertex data.
		vertex_data_ = new float[vertex_count_ * 3];
		for (i = 0, index = 0; i < vertex_count_; i++, index += 3) {
			vertex_data_[index + 0] = vertex_data[index + 0];
			vertex_data_[index + 1] = vertex_data[index + 1];
			vertex_data_[index + 2] = vertex_data[index + 2];
		}

		if (vertex_normals != 0) {
			// Copy normal data.
			SetVertexNormalData(vertex_normals, vertex_count_);
		}

		if (uv_data != 0) {
			AddUVSet(uv_data);
		}

		if (color_data != 0) {
			// Copy color data.
			color_format_ = color_format;

			int size = 4;
			if (color_format_ == kColorRgb)
				size = 3;

			color_data_ = new uint8[vertex_count_ * size];

			for (i = 0, index = 0; i < vertex_count_; i++, index += size) {
				color_data_[index + 0] = color_data[index + 0];
				color_data_[index + 1] = color_data[index + 1];
				color_data_[index + 2] = color_data[index + 2];

				if (color_format_ == kColorRgba)
					color_data_[index + 3] = color_data[index + 3];
			}
		}

		if (indices != 0) {
			// Copy index data.
			index_data_ = new vtx_idx_t[index_count_];
			for (i = 0; i < index_count_; i++) {
				index_data_[i] = indices[i];
			}
		}
	}

	tbc::GeometryBase::CalculateBoundingRadius();
}

void TriangleBasedGeometry::Copy(const TriangleBasedGeometry& geometry) {
	ClearAll();

	if (geometry.vertex_count_ > 0 && geometry.vertex_data_ != 0) {
		unsigned int i;
		int index;

		TriangleBasedGeometry& _geometry = (TriangleBasedGeometry&)geometry;

		vertex_count_ = geometry.GetVertexCount();
		index_count_ = geometry.GetIndexCount();
		max_index_count_ = geometry.GetMaxIndexCount();

		geometry_volatility_ = geometry.GetGeometryVolatility();

		tbc::GeometryBase::SetBasicMaterialSettings(geometry.GetBasicMaterialSettings());

		tbc::GeometryBase::SetBoundingRadius(geometry.GetBoundingRadius());

		// Copy vertex data.
		const float* _vertex_data = _geometry.GetVertexData();
		vertex_data_ = new float[vertex_count_ * 3];
		for (i = 0, index = 0; i < vertex_count_; i++, index += 3) {
			vertex_data_[index + 0] = _vertex_data[index + 0];
			vertex_data_[index + 1] = _vertex_data[index + 1];
			vertex_data_[index + 2] = _vertex_data[index + 2];
		}

		if (_geometry.GetNormalData() != 0) {
			// Copy normal data.
			tbc::GeometryBase::SetVertexNormalData(_geometry.GetNormalData(), vertex_count_);
		}

		if (_geometry.GetSurfaceNormalData() != 0) {
			tbc::GeometryBase::SetSurfaceNormalData(_geometry.GetSurfaceNormalData());
		}

		if (_geometry.GetUVData() != 0) {
			uv_data_ = new float*[_geometry.GetUVSetCount()];
			const int uv_count_per_vertex = _geometry.GetUVCountPerVertex();

			for (unsigned int j = 0; j < _geometry.GetUVSetCount(); j++) {
				// Copy UV data.
				uv_data_[j] = new float[vertex_count_ * uv_count_per_vertex];
				::memcpy(uv_data_[j], _geometry.GetUVData(j), vertex_count_*uv_count_per_vertex*sizeof(float));
			}
		}

		if (_geometry.GetColorData() != 0) {
			// Copy color data.
			color_format_ = _geometry.GetColorFormat();

			int size = 4;
			if (color_format_ == kColorRgb)
				size = 3;

			const uint8* _color_data = _geometry.GetColorData();
			color_data_ = new uint8[vertex_count_ * size];
			for (i = 0, index = 0; i < vertex_count_; i++, index += size) {
				color_data_[index + 0] = _color_data[index + 0];
				color_data_[index + 1] = _color_data[index + 1];
				color_data_[index + 2] = _color_data[index + 2];

				if (color_format_ == kColorRgba)
					color_data_[index + 3] = _color_data[index + 3];
			}
		}

		if (_geometry.GetIndexData() != 0) {
			// Copy index data.
			const vtx_idx_t* _indices = _geometry.GetIndexData();
			index_data_ = new vtx_idx_t[max_index_count_];
			for (i = 0; i < max_index_count_; i++) {
				index_data_[i] = _indices[i];
			}
		}

		tbc::GeometryBase::Copy(&_geometry);
	}
}

TriangleBasedGeometry::~TriangleBasedGeometry() {
	LEPRA_RELEASE_RESOURCE(TriangleBasedGeometry);

	ClearAll();
}

void TriangleBasedGeometry::ClearAll() {
	if (vertex_data_ != 0) {
		delete[] vertex_data_;
		vertex_data_ = 0;
	}

	tbc::GeometryBase::ClearSurfaceNormalData();
	tbc::GeometryBase::ClearVertexNormalData();

	if (uv_set_count_ > 0) {
		for (unsigned int i = 0; i < uv_set_count_; i++) {
			delete[] uv_data_[i];
		}
		delete[] uv_data_;
		uv_data_ = 0;
	}

	if (color_data_ != 0) {
		delete[] color_data_;
		color_data_ = 0;
	}

	if (index_data_ != 0) {
		delete[] index_data_;
		index_data_ = 0;
	}

	vertex_count_ = 0;
	index_count_ = 0;
	max_index_count_ = 0;

	tbc::GeometryBase::ClearAll();
	SetVertexDataChanged(true);
	SetUVDataChanged(true);
	SetColorDataChanged(true);
	SetIndexDataChanged(true);
}

void TriangleBasedGeometry::SetIndexData(vtx_idx_t* index_data, unsigned index_count, unsigned max_index_count) {
	index_data_ = index_data;
	index_count_ = index_count;
	max_index_count_ = max_index_count;
}

void TriangleBasedGeometry::SetPolygon(vec3* vertices, unsigned int vertex_count) {
	ClearAll();
	vertex_data_ = new float[vertex_count * 3];

	for (unsigned int i = 0; i < vertex_count; i++) {
		vertex_data_[i * 3 + 0] = vertices[i].x;
		vertex_data_[i * 3 + 1] = vertices[i].y;
		vertex_data_[i * 3 + 2] = vertices[i].z;
	}

	vertex_count_ = vertex_count;

	tbc::GeometryBase::CalculateBoundingRadius();
}

int TriangleBasedGeometry::AddUVSet(const float* uv_data) {
	unsigned int uv_set_count = uv_set_count_ + 1;
	float** _uv_data = new float*[uv_set_count];

	unsigned int uv_set_index;

	// Copy present data.
	for (uv_set_index = 0; uv_set_index < uv_set_count_; uv_set_index++) {
		_uv_data[uv_set_index] = new float[vertex_count_ * GetUVCountPerVertex()];
		::memcpy(_uv_data[uv_set_index], uv_data_[uv_set_index], vertex_count_*GetUVCountPerVertex()*sizeof(float));
		delete[] uv_data_[uv_set_index];
	}

	delete[] uv_data_;

	_uv_data[uv_set_index] = new float[vertex_count_ * GetUVCountPerVertex()];

	// Add new data at uv_set_index.
	::memcpy(_uv_data[uv_set_index], uv_data, vertex_count_*GetUVCountPerVertex()*sizeof(float));

	uv_data_ = _uv_data;
	uv_set_count_ = uv_set_count;

	return uv_set_count_ - 1;
}

int TriangleBasedGeometry::AddUVSet(const Vector2D<float>* uv_data) {
	unsigned int uv_set_count = uv_set_count_ + 1;
	float** _uv_data = new float*[uv_set_count];

	unsigned int uv_set_index;
	unsigned int i;

	// Copy present data.
	const int uv_count_per_vertex = GetUVCountPerVertex();
	for (uv_set_index = 0; uv_set_index < uv_set_count_; uv_set_index++) {
		_uv_data[uv_set_index] = new float[vertex_count_ * uv_count_per_vertex];
		::memcpy(_uv_data[uv_set_index], uv_data_[uv_set_index], vertex_count_*uv_count_per_vertex*sizeof(float));
		delete[] uv_data_[uv_set_index];
	}

	delete[] uv_data_;

	_uv_data[uv_set_index] = new float[vertex_count_ * uv_count_per_vertex];
	::memset(_uv_data[uv_set_index], 0, vertex_count_*uv_count_per_vertex*sizeof(float));

	// Add new data at uv_set_index.
	for (i = 0; i < vertex_count_; i++) {
		_uv_data[uv_set_index][i * uv_count_per_vertex + 0] = uv_data[i].x;
		_uv_data[uv_set_index][i * uv_count_per_vertex + 1] = uv_data[i].y;
	}

	uv_data_ = _uv_data;
	uv_set_count_ = uv_set_count;

	return uv_set_count_ - 1;
}

int TriangleBasedGeometry::AddEmptyUVSet() {
	unsigned int uv_set_count = uv_set_count_ + 1;
	float** _uv_data = new float*[uv_set_count];

	unsigned int uv_set_index;

	// Copy present data.
	const int uv_count_per_vertex = GetUVCountPerVertex();
	for (uv_set_index = 0; uv_set_index < uv_set_count_; uv_set_index++) {
		_uv_data[uv_set_index] = new float[vertex_count_ * uv_count_per_vertex];
		::memcpy(_uv_data[uv_set_index], uv_data_[uv_set_index], vertex_count_*uv_count_per_vertex*sizeof(float));
		delete[] uv_data_[uv_set_index];
	}

	delete[] uv_data_;

	_uv_data[uv_set_index] = new float[vertex_count_ * uv_count_per_vertex];
	::memset(_uv_data[uv_set_index], 0, vertex_count_*uv_count_per_vertex*sizeof(float));

	uv_data_ = _uv_data;
	uv_set_count_ = uv_set_count;

	return uv_set_count_ - 1;
}

int TriangleBasedGeometry::DupUVSet(int uv_set) {
	int new_uv_set = -1;
	if (uv_set >= 0 && uv_set < (int)uv_set_count_) {
		unsigned int uv_set_count = uv_set_count_ + 1;
		float** _uv_data = new float*[uv_set_count];

		unsigned int uv_set_index;

		// Copy present data.
		const int uv_count_per_vertex = GetUVCountPerVertex();
		for (uv_set_index = 0; uv_set_index < uv_set_count_; uv_set_index++) {
			_uv_data[uv_set_index] = new float[vertex_count_ * uv_count_per_vertex];
			::memcpy(_uv_data[uv_set_index], uv_data_[uv_set_index], vertex_count_*uv_count_per_vertex*sizeof(float));
			delete[] uv_data_[uv_set_index];
		}

		delete[] uv_data_;

		_uv_data[uv_set_index] = new float[vertex_count_ * uv_count_per_vertex];
		::memcpy(_uv_data[uv_set_index], _uv_data[uv_set], vertex_count_*uv_count_per_vertex*sizeof(float));

		uv_data_ = _uv_data;
		uv_set_count_ = uv_set_count;

		return uv_set_count_ - 1;
	}
	return new_uv_set;
}

int TriangleBasedGeometry::PopUVSet() {
	deb_assert(uv_set_count_ > 0);
	if (uv_set_count_ > 0) {
		--uv_set_count_;
		delete[] uv_data_[uv_set_count_];
		uv_data_[uv_set_count_] = 0;
		if (uv_set_count_ == 0) {
			delete[] uv_data_;
			uv_data_ = 0;
		}
	}
	return uv_set_count_;
}

void TriangleBasedGeometry::SetColorData(uint8* color_data, ColorFormat color_format) {
	if (color_data_ != 0) {
		delete[] color_data_;
		color_data_ = 0;
	}

	if (color_data != 0) {
		// Copy color data.
		color_format_ = color_format;

		int size = 4;
		if (color_format_ == kColorRgb)
			size = 3;

		color_data_ = new uint8[vertex_count_ * size];

		unsigned int i;
		unsigned int index;
		for (i = 0, index = 0; i < vertex_count_; i++, index += size) {
			color_data_[index + 0] = color_data[index + 0];
			color_data_[index + 1] = color_data[index + 1];
			color_data_[index + 2] = color_data[index + 2];

			if (color_format_ == kColorRgba)
				color_data_[index + 3] = color_data[index + 3];
		}
	}
}

void TriangleBasedGeometry::ConvertColorData(ColorFormat color_format, uint8 alpha) {
	if (color_format_ != color_format) {
		int size = 4;
		if (color_format == kColorRgb)
			size = 3;

		uint8* _color_data = new uint8[vertex_count_ * size];
		unsigned int i;
		unsigned int index;
		for (i = 0, index = 0; i < vertex_count_; i++, index += size) {
			_color_data[index + 0] = color_data_[index + 0];
			_color_data[index + 1] = color_data_[index + 1];
			_color_data[index + 2] = color_data_[index + 2];

			if (color_format == kColorRgba)
				_color_data[index + 3] = alpha;
		}

		if (color_data_ != 0)
			delete[] color_data_;

		color_data_ = _color_data;

		color_format_ = color_format;
	}
}

void TriangleBasedGeometry::FlipTriangles() {
	if (index_data_ != 0) {
		if (primitive_type_ == tbc::GeometryBase::kTriangles) {
			for (unsigned int i = 0; i < index_count_; i += 3) {
				vtx_idx_t temp = index_data_[i + 0];
				index_data_[i + 0] = index_data_[i + 1];
				index_data_[i + 1] = temp;
			}
		} else if (primitive_type_ == tbc::GeometryBase::kTriangleStrip && index_count_ > 2) {
			// We only need to flip the first one, and the rest will follow.
			vtx_idx_t temp = index_data_[1];
			index_data_[1] = index_data_[2];
			index_data_[2] = temp;
		} else {
			deb_assert(false);
		}
	}

	tbc::GeometryBase::SetConvexVolumeCheckValid(false);
}

void TriangleBasedGeometry::FlipTriangle(int triangle_index) {
	if (primitive_type_ == tbc::GeometryBase::kTriangles &&
	    index_data_ != 0 &&
	    triangle_index >= 0 &&
	    triangle_index < (int)GetTriangleCount()) {
		int i = triangle_index * 3;

		vtx_idx_t temp = index_data_[i + 0];
		index_data_[i + 0] = index_data_[i + 1];
		index_data_[i + 1] = temp;

		tbc::GeometryBase::SetConvexVolumeCheckValid(false);
	} else {
		deb_assert(false);
	}
}

void TriangleBasedGeometry::SplitVertices() {
	if (index_data_ != 0) {
		const tbc::GeometryBase::BasicMaterialSettings& material = GetBasicMaterialSettings();
		bool smooth = material.smooth_;
		float* original_normal_data = 0;
		if (smooth == true) {
			GenerateVertexNormalData();
			original_normal_data = GetNormalData();
		} else {
			GenerateSurfaceNormalData();
			original_normal_data = GetSurfaceNormalData();
		}

		//
		// First allocate new memory for everything.
		//

		unsigned int _vertex_count = GetTriangleCount() * 3;
		unsigned int uv_set_count = GetUVSetCount();
		uint32* _indices = new uint32[_vertex_count];
		float* _vertex_data = new float[_vertex_count * 3];
		float* normal_data = new float[_vertex_count * 3];
		float** _uv_data = 0;
		uint8* _color_data = 0;
		unsigned int i;

		if (uv_set_count > 0) {
			_uv_data = new float*[uv_set_count];
			for (i = 0; i < uv_set_count; i++) {
				_uv_data[i] = new float[_vertex_count * 2];
			}
		}

		int color_size = (color_format_ == kColorRgb ? 3 : 4);
		if (color_data_ != 0) {
			_color_data = new uint8[_vertex_count * color_size];
		}

		//
		// Second, copy & set everything...
		//

		int v_index  = 0;
		int uv_index = 0;
		int color_index = 0;

		for (i = 0; i < GetTriangleCount(); i++) {
			unsigned long t = i * 3;

			_indices[t + 0] = t + 0;
			_indices[t + 1] = t + 1;
			_indices[t + 2] = t + 2;

			unsigned long v1 = index_data_[t + 0];
			unsigned long v2 = index_data_[t + 1];
			unsigned long v3 = index_data_[t + 2];

			unsigned long v1_i = v1 * 3;
			unsigned long v2_i = v2 * 3;
			unsigned long v3_i = v3 * 3;

			unsigned long c1_i = v1 * color_size;
			unsigned long c2_i = v2 * color_size;
			unsigned long c3_i = v3 * color_size;

			// Copy vertex coordinates for 3 vertices.
			_vertex_data[v_index + 0] = vertex_data_[v1_i + 0];
			_vertex_data[v_index + 1] = vertex_data_[v1_i + 1];
			_vertex_data[v_index + 2] = vertex_data_[v1_i + 2];
			_vertex_data[v_index + 3] = vertex_data_[v2_i + 0];
			_vertex_data[v_index + 4] = vertex_data_[v2_i + 1];
			_vertex_data[v_index + 5] = vertex_data_[v2_i + 2];
			_vertex_data[v_index + 6] = vertex_data_[v3_i + 0];
			_vertex_data[v_index + 7] = vertex_data_[v3_i + 1];
			_vertex_data[v_index + 8] = vertex_data_[v3_i + 2];

			if (smooth == true) {
				// Copy vertex normals...
				normal_data[v_index + 0] = original_normal_data[v1_i + 0];
				normal_data[v_index + 1] = original_normal_data[v1_i + 1];
				normal_data[v_index + 2] = original_normal_data[v1_i + 2];
				normal_data[v_index + 3] = original_normal_data[v2_i + 0];
				normal_data[v_index + 4] = original_normal_data[v2_i + 1];
				normal_data[v_index + 5] = original_normal_data[v2_i + 2];
				normal_data[v_index + 6] = original_normal_data[v3_i + 0];
				normal_data[v_index + 7] = original_normal_data[v3_i + 1];
				normal_data[v_index + 8] = original_normal_data[v3_i + 2];
			} else {
				// Copy surface normals...
				normal_data[v_index + 0] = original_normal_data[t + 0];
				normal_data[v_index + 1] = original_normal_data[t + 1];
				normal_data[v_index + 2] = original_normal_data[t + 2];
				normal_data[v_index + 3] = original_normal_data[t + 0];
				normal_data[v_index + 4] = original_normal_data[t + 1];
				normal_data[v_index + 5] = original_normal_data[t + 2];
				normal_data[v_index + 6] = original_normal_data[t + 0];
				normal_data[v_index + 7] = original_normal_data[t + 1];
				normal_data[v_index + 8] = original_normal_data[t + 2];
			}

			if (color_data_ != 0) {
				_color_data[color_index++] = color_data_[c1_i + 0];
				_color_data[color_index++] = color_data_[c1_i + 1];
				_color_data[color_index++] = color_data_[c1_i + 2];
				if (color_size == 4)
					_color_data[color_index++] = color_data_[c1_i + 3];
				_color_data[color_index++] = color_data_[c2_i + 0];
				_color_data[color_index++] = color_data_[c2_i + 1];
				_color_data[color_index++] = color_data_[c2_i + 2];
				if (color_size == 4)
					_color_data[color_index++] = color_data_[c2_i + 3];
				_color_data[color_index++] = color_data_[c3_i + 0];
				_color_data[color_index++] = color_data_[c3_i + 1];
				_color_data[color_index++] = color_data_[c3_i + 2];
				if (color_size == 4)
					_color_data[color_index++] = color_data_[c3_i + 3];
			}

			v1_i = v1 * 2;
			v2_i = v2 * 2;
			v3_i = v3 * 2;
			for (unsigned int j = 0; j < uv_set_count; j++) {
				_uv_data[j][uv_index + 0] = uv_data_[j][v1 + 0];
				_uv_data[j][uv_index + 1] = uv_data_[j][v1 + 1];
				_uv_data[j][uv_index + 2] = uv_data_[j][v2 + 0];
				_uv_data[j][uv_index + 3] = uv_data_[j][v2 + 1];
				_uv_data[j][uv_index + 4] = uv_data_[j][v3 + 0];
				_uv_data[j][uv_index + 5] = uv_data_[j][v3 + 1];
			}

			v_index += 9;
			uv_index += 6;
		}

		float* _uv_set = 0;
		if (uv_set_count > 0)
			_uv_set = _uv_data[0];

		//
		// Third, update the geometry.
		//

		Set(_vertex_data,
		    normal_data,
		    _uv_set,
		    _color_data,
		    color_format_,
		    _indices,
		    _vertex_count,
		    _vertex_count,
		    tbc::GeometryBase::kTriangles,
		    geometry_volatility_);

		for (i = 1; i < uv_set_count; i++) {
			AddUVSet(_uv_data[i]);
		}

		//
		// Fourth, deallocate all memory.
		//
		delete[] _indices;
		delete[] _vertex_data;
		delete[] normal_data;

		if (uv_set_count > 0) {
			for (i = 0; i < uv_set_count; i++) {
				delete[] _uv_data[i];
			}
			delete[] _uv_data;
		}

		if (_color_data != 0) {
			delete[] _color_data;
		}
	}

}

void TriangleBasedGeometry::Translate(const vec3& position_offset) {
	if (vertex_data_ != 0 && vertex_count_ > 0) {
		const int max_count = vertex_count_ * 3;
		for (int i = 0; i < max_count; i += 3) {
			vertex_data_[i + 0] += position_offset.x;
			vertex_data_[i + 1] += position_offset.y;
			vertex_data_[i + 2] += position_offset.z;
		}
	}

	tbc::GeometryBase::CalculateBoundingRadius();
}

void TriangleBasedGeometry::Rotate(const RotationMatrix<float>& rotation) {
	vec3 axis_x(rotation.GetInverseAxisX());
	vec3 axis_y(rotation.GetInverseAxisY());
	vec3 axis_z(rotation.GetInverseAxisZ());

	if (vertex_count_ > 0) {
		const int max_count = vertex_count_ * 3;

		if (vertex_data_ != 0) {
			for (int i = 0; i < max_count; i += 3) {
				float __x = vertex_data_[i + 0];
				float __y = vertex_data_[i + 1];
				float __z = vertex_data_[i + 2];

				float rot_x = axis_x.x * __x + axis_x.y * __y + axis_x.z * __z;
				float rot_y = axis_y.x * __x + axis_y.y * __y + axis_y.z * __z;
				float rot_z = axis_z.x * __x + axis_z.y * __y + axis_z.z * __z;

				vertex_data_[i + 0] = rot_x;
				vertex_data_[i + 1] = rot_y;
				vertex_data_[i + 2] = rot_z;
			}
		}

		GenerateVertexNormalData();
		float* vertex_normal_data = GetNormalData();
		if (vertex_normal_data != 0) {
			for (int i = 0; i < max_count; i += 3) {
				float __x = vertex_normal_data[i + 0];
				float __y = vertex_normal_data[i + 1];
				float __z = vertex_normal_data[i + 2];

				float rot_x = axis_x.x * __x + axis_x.y * __y + axis_x.z * __z;
				float rot_y = axis_y.x * __x + axis_y.y * __y + axis_y.z * __z;
				float rot_z = axis_z.x * __x + axis_z.y * __y + axis_z.z * __z;

				vertex_normal_data[i + 0] = rot_x;
				vertex_normal_data[i + 1] = rot_y;
				vertex_normal_data[i + 2] = rot_z;
			}
		}
	}

	if (GetTriangleCount() > 0 && tbc::GeometryBase::GetSurfaceNormalData() != 0) {
		const int max_count = GetTriangleCount() * 3;
		float* surface_normal_data = tbc::GeometryBase::GetSurfaceNormalData();

		for (int i = 0; i < max_count; i += 3) {
			float __x = surface_normal_data[i + 0];
			float __y = surface_normal_data[i + 1];
			float __z = surface_normal_data[i + 2];

			float rot_x = axis_x.x * __x + axis_x.y * __y + axis_x.z * __z;
			float rot_y = axis_y.x * __x + axis_y.y * __y + axis_y.z * __z;
			float rot_z = axis_z.x * __x + axis_z.y * __y + axis_z.z * __z;

			surface_normal_data[i + 0] = rot_x;
			surface_normal_data[i + 1] = rot_y;
			surface_normal_data[i + 2] = rot_z;
		}
	}
}

void TriangleBasedGeometry::AddGeometry(TriangleBasedGeometry* geometry) {
	if (primitive_type_ != kTriangles) {
		deb_assert(false);
		return;
	}

	bool generate_edge_data = false;

	tbc::GeometryBase::ClearSurfaceNormalData();
	tbc::GeometryBase::ClearVertexNormalData();

	if (tbc::GeometryBase::GetEdgeData() != 0) {
		generate_edge_data = true;
		tbc::GeometryBase::ClearEdgeData();
	}

	unsigned int _vertex_count = vertex_count_ + geometry->vertex_count_;
	unsigned int num_triangles = GetTriangleCount() + geometry->GetTriangleCount();

	float* _vertex_data = new float[_vertex_count * 3];
	vtx_idx_t* _indices = new vtx_idx_t[num_triangles * 3];

	unsigned int i;
	int index = 0;
	for (i = 0; i < vertex_count_; i++, index += 3) {
		_vertex_data[index + 0] = vertex_data_[index + 0];
		_vertex_data[index + 1] = vertex_data_[index + 1];
		_vertex_data[index + 2] = vertex_data_[index + 2];
	}
	int src_index = 0;
	for (i = 0; i < geometry->vertex_count_; i++, index += 3, src_index += 3) {
		_vertex_data[index + 0] = geometry->vertex_data_[src_index + 0];
		_vertex_data[index + 1] = geometry->vertex_data_[src_index + 1];
		_vertex_data[index + 2] = geometry->vertex_data_[src_index + 2];
	}

	index = 0;
	for (i = 0; i < GetTriangleCount(); i++, index += 3) {
		_indices[index + 0] = index_data_[index + 0];
		_indices[index + 1] = index_data_[index + 1];
		_indices[index + 2] = index_data_[index + 2];
	}

	for (i = 0; i < geometry->GetTriangleCount(); i++, index += 3) {
		uint32 tri_index[4];
		geometry->GetTriangleIndices(i, tri_index);

		_indices[index + 0] = tri_index[0] + vertex_count_;
		_indices[index + 1] = tri_index[1] + vertex_count_;
		_indices[index + 2] = tri_index[2] + vertex_count_;
	}

	delete[] vertex_data_;
	delete[] index_data_;

	vertex_data_ = _vertex_data;
	index_data_ = _indices;

	vertex_count_ = _vertex_count;
	index_count_ = num_triangles * 3;
	max_index_count_ = num_triangles * 3;

	if (uv_data_ != 0) {
		float** _uv_data = new float*[uv_set_count_];
		for (unsigned int _uv_set = 0; _uv_set < uv_set_count_; _uv_set++) {
			_uv_data[_uv_set] = new float[vertex_count_ * 2];

			index = 0;
			for (i = 0; i < vertex_count_; i++, index += 2) {
				_uv_data[_uv_set][index + 0] = uv_data_[_uv_set][index + 0];
				_uv_data[_uv_set][index + 1] = uv_data_[_uv_set][index + 1];
			}
			src_index = 0;

			if (_uv_set < geometry->uv_set_count_) {
				for (i = 0; i < geometry->vertex_count_; i++, index += 2, src_index += 2) {
					_uv_data[_uv_set][index + 0] = geometry->uv_data_[_uv_set][src_index + 0];
					_uv_data[_uv_set][index + 1] = geometry->uv_data_[_uv_set][src_index + 1];
				}
			} else {
				for (i = 0; i < geometry->vertex_count_; i++, index += 2) {
					_uv_data[_uv_set][index + 0] = 0;
					_uv_data[_uv_set][index + 1] = 0;
				}
			}

			delete[] uv_data_[_uv_set];
		}
		delete[] uv_data_;
		uv_data_ = _uv_data;
	}

	if (color_data_ != 0) {
		uint8* _color_data = new uint8[vertex_count_ * 3];

		index = 0;
		for (i = 0; i < vertex_count_; i++, index += 3) {
			_color_data[index + 0] = color_data_[index + 0];
			_color_data[index + 1] = color_data_[index + 1];
			_color_data[index + 2] = color_data_[index + 2];
		}
		src_index = 0;

		if (geometry->color_data_ != 0) {
			for (i = 0; i < geometry->vertex_count_; i++, index += 3, src_index += 3) {
				_color_data[index + 0] = geometry->color_data_[src_index + 0];
				_color_data[index + 1] = geometry->color_data_[src_index + 1];
				_color_data[index + 2] = geometry->color_data_[src_index + 2];
			}
		} else {
			for (i = 0; i < geometry->vertex_count_; i++, index += 3) {
				_color_data[index + 0] = 255;
				_color_data[index + 1] = 255;
				_color_data[index + 2] = 255;
			}
		}

		delete[] color_data_;
		color_data_ = _color_data;
	}

	GenerateVertexNormalData();

	if (generate_edge_data == true) {
		tbc::GeometryBase::GenerateEdgeData();
	}

	tbc::GeometryBase::SetSolidVolumeCheckValid(false);
	tbc::GeometryBase::SetSingleObjectCheckValid(false);
	tbc::GeometryBase::SetConvexVolumeCheckValid(false);

	tbc::GeometryBase::CalculateBoundingRadius();
}

unsigned int TriangleBasedGeometry::GetMaxVertexCount() const {
	return vertex_count_;
}

unsigned int TriangleBasedGeometry::GetMaxIndexCount() const {
	return max_index_count_;
}

unsigned int TriangleBasedGeometry::GetVertexCount() const {
	return vertex_count_;
}

unsigned int TriangleBasedGeometry::GetIndexCount() const {
	return index_count_;
}

unsigned int TriangleBasedGeometry::GetUVSetCount() const {
	return uv_set_count_;
}

float* TriangleBasedGeometry::GetVertexData() const {
	return vertex_data_;
}

float* TriangleBasedGeometry::GetUVData(unsigned int uv_set) const {
	if (uv_data_ == 0)
		return 0;

	return uv_data_[uv_set];
}

uint8* TriangleBasedGeometry::GetColorData() const {
	return color_data_;
}

vtx_idx_t* TriangleBasedGeometry::GetIndexData() const {
	return index_data_;
}

tbc::GeometryBase::GeometryVolatility TriangleBasedGeometry::GetGeometryVolatility() const {
	//deb_assert(geometry_volatility_ == kGeomStatic);
	return (geometry_volatility_);
}

void TriangleBasedGeometry::SetGeometryVolatility(tbc::GeometryBase::GeometryVolatility volatility) {
	geometry_volatility_ = volatility;
	//deb_assert(geometry_volatility_ == kGeomStatic);
}

TriangleBasedGeometry::ColorFormat TriangleBasedGeometry::GetColorFormat() const {
	deb_assert(color_format_ == kColorRgb || color_format_ == kColorRgba);
	return color_format_;
}



}
