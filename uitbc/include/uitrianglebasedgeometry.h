
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/vector2d.h"
#include "../../tbc/include/geometrybase.h"
#include "uitbc.h"



namespace uitbc {



class TriangleBasedGeometry: public tbc::GeometryBase {
public:
	TriangleBasedGeometry();
	TriangleBasedGeometry(const TriangleBasedGeometry& geometry);

	TriangleBasedGeometry(const vec3* vertices,      // Must containt pVertexCount elements.
			      const vec3* vertex_normals, // NULL or must contain vertex_count elements.
			      const Vector2D<float>* uv,      // NULL or must contain vertex_count elements.
			      const Color* color,             // NULL or must contain vertex_count elements.
			      ColorFormat color_format,
			      const lepra::uint32* indices,
			      unsigned int vertex_count,
			      unsigned int index_count,
			      tbc::GeometryBase::PrimitiveType primitive_type,
			      tbc::GeometryBase::GeometryVolatility geom_type);

	TriangleBasedGeometry(const float* vertex_data,
			      const float* vertex_normals,
			      const float* uv_data,
			      const lepra::uint8* color_data,
			      ColorFormat color_format,
			      const lepra::uint32* indices,
			      unsigned int vertex_count,
			      unsigned int index_count,
			      tbc::GeometryBase::PrimitiveType primitive_type,
			      tbc::GeometryBase::GeometryVolatility geom_type);
	virtual ~TriangleBasedGeometry();

	void Copy(const TriangleBasedGeometry& geometry);

	void Set(const vec3* vertices,
		 const vec3* vertex_normals,
		 const Vector2D<float>* uv,
		 const Color* color,
		 ColorFormat color_format,
		 const lepra::uint32* indices,
		 unsigned int vertex_count,
		 unsigned int index_count,
		 tbc::GeometryBase::PrimitiveType primitive_type,
		 tbc::GeometryBase::GeometryVolatility geom_type);

	void Set(const float* vertex_data,
		 const float* vertex_normals,
		 const float* uv_data,
		 const lepra::uint8* color_data,
		 ColorFormat color_format,
		 const lepra::uint32* indices,
		 unsigned int vertex_count,
		 unsigned int index_count,
		 tbc::GeometryBase::PrimitiveType primitive_type,
		 tbc::GeometryBase::GeometryVolatility geom_type);

	// AddUVSet() returns the index of the uv set.
	// uv_data must contain vertex_count_ * 2 elements.
	int AddUVSet(const float* uv_data);
	int AddUVSet(const Vector2D<float>* uv_data);
	int AddEmptyUVSet();
	int DupUVSet(int uv_set);
	int PopUVSet();

	// Data access...

	virtual unsigned int GetMaxVertexCount() const;
	virtual unsigned int GetMaxIndexCount() const;

	virtual unsigned int GetVertexCount() const;
	virtual unsigned int GetIndexCount() const;
	virtual unsigned int GetUVSetCount() const;

	virtual float* GetVertexData() const;
	virtual float* GetUVData(unsigned int uv_set = 0) const;
	virtual vtx_idx_t* GetIndexData() const;
	virtual lepra::uint8* GetColorData() const;

	void SetColorData(unsigned char* color_data, ColorFormat color_format);
	void ConvertColorData(ColorFormat color_format, unsigned char alpha);
	ColorFormat GetColorFormat() const;

	// Misc work funcs.
	void ClearAll();

	// Know what you're doing when calling this internal function.
	virtual void SetIndexData(vtx_idx_t* index_data, unsigned index_count, unsigned max_index_count);

	// Will erase all current data.
	void SetPolygon(vec3* vertices, unsigned int vertex_count);

	// Flips the triangles from being clockwise to counter clockwise,
	// and vice versa. FlipTriangle() only works if the primitive type
	// is kTriangles.
	void FlipTriangles();
	void FlipTriangle(int triangle_index);

	// Generates unique vertices for each triangle. New UV-coordinate arrays and Color-
	// arrays will be created, and the vertex normals will be regenerated according
	// to the BasicMaterialSettings::smooth_. This is an irreversible operation,
	// so use it with care.
	void SplitVertices();

	void Translate(const vec3& position_offset);
	void Rotate(const RotationMatrix<float>& rotation);

	// This definitly won't work if "this" has a primitive type other than kTriangles.
	void AddGeometry(TriangleBasedGeometry* geometry);

	// Use this if IsSingleObject() returns false and you want to get the separate objects.
	// The pointer that is returned will point to an array of TriangleBasedGeometry.
	// The number of elements in this array is stored in num_objects.
	// The caller is responsible of deleting this array.
	TriangleBasedGeometry* GetSeparateObjects(int& num_objects);

	// Overloads from tbc::GeometryBase.
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility volatility);

private:

	unsigned int vertex_count_;
	unsigned int index_count_;
	unsigned int max_index_count_;
	unsigned int uv_set_count_;

	float* vertex_data_;			// Triplets of (x, y, z).
	float** uv_data_;			// Doublets of (u, v).
	unsigned char* color_data_;
	vtx_idx_t* index_data_;	// Triplets of vertex indices.

	ColorFormat color_format_;

	tbc::GeometryBase::GeometryVolatility geometry_volatility_;
	tbc::GeometryBase::PrimitiveType primitive_type_;
};



}
