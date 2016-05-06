
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/vector3d.h"
#include "../../lepra/include/vector2d.h"
#include "../../tbc/include/geometrybase.h"
#include "uitbc.h"



namespace uitbc {



// If a billboard uses texture variations, they should be layed out equidistant horizontally on the same map.
class BillboardGeometry: public tbc::GeometryBase {
public:
	BillboardGeometry(float aspect_ratio, unsigned uv_set_count);
	virtual ~BillboardGeometry();

	virtual unsigned GetMaxVertexCount() const;
	virtual unsigned GetMaxIndexCount() const;

	virtual unsigned GetVertexCount() const;
	virtual unsigned GetIndexCount() const;
	virtual unsigned GetUVSetCount() const;

	virtual float* GetVertexData() const;
	void SetVertexData(const float vertex_data[12]);
	virtual float* GetUVData(unsigned uv_set = 0) const;
	virtual vtx_idx_t* GetIndexData() const;
	virtual uint8* GetColorData() const;

	// Overloads from tbc::GeometryBase.
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility volatility);

private:
	float vertex_data_[12];	// Quadruplets of (x, y, z).
	float* uv_data_;		// Doublets of (u, v).
	unsigned uv_set_count_;
	vtx_idx_t index_data_[4];
};



}
