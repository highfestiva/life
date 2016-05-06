
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uitbc.h"
#include "../../tbc/include/geometrybase.h"
#include "../../lepra/include/transformation.h"



namespace uitbc {



class ShadowVolume: public tbc::GeometryBase {
public:
	ShadowVolume(tbc::GeometryBase* parent_geometry);
	virtual ~ShadowVolume();

	void UpdateShadowVolume(const vec3& light_pos, float light_radius, bool directional);

	tbc::GeometryBase* GetParentGeometry();
	void SetParentGeometry(tbc::GeometryBase* parent_geometry);

	tbc::GeometryBase::GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(tbc::GeometryBase::GeometryVolatility volatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int uv_set) const;
	vtx_idx_t*     GetIndexData() const;
	lepra::uint8*  GetColorData() const;
	float*         GetNormalData() const;

protected:
	void ExtrudeEdges();
private:

	struct TriangleOrientation {
		unsigned v0_;
		unsigned v1_;
		unsigned v2_;
		bool checked_;
		bool is_front_facing_;
	};

	void InitVertices();
	void InitTO();

	float* vertex_data_;
	vtx_idx_t* index_data_;

	// Array with mNumParentTriangles elements.
	TriangleOrientation* triangle_orientation_;

	unsigned int vertex_count_;
	unsigned int triangle_count_;
	unsigned int parent_vertex_count_;
	unsigned int max_triangle_count_;

	quat previous_orientation_;

	// The geometry that cast this shadow.
	tbc::GeometryBase* parent_geometry_;
};



}
