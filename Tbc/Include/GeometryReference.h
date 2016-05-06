
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "geometrybase.h"
#include "../../lepra/include/transformation.h"



namespace tbc {



class GeometryReference: public GeometryBase {
	typedef GeometryBase Parent;
public:

	GeometryReference(GeometryBase* geometry);
	virtual ~GeometryReference();

	bool IsGeometryReference();

	const xform& GetOffsetTransformation() const;
	void SetOffsetTransformation(const xform& offset);
	void AddOffset(const vec3& offset);
	const xform& GetExtraOffsetTransformation() const;
	void SetExtraOffsetTransformation(const xform& offset);

	// Overloads from GeometryBase.
	const xform& GetTransformation();
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility volatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount()  const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int uv_set) const;
	vtx_idx_t*     GetIndexData() const;
	uint8*         GetColorData() const;
	float*         GetNormalData() const;

	GeometryBase* GetParentGeometry() const;

protected:
private:
	GeometryBase* geometry_;

	xform original_offset_;
	xform extra_offset_;
	xform return_transformation_;
};



}
