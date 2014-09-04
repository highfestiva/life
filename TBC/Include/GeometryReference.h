
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "GeometryBase.h"
#include "../../Lepra/Include/Transformation.h"



namespace Tbc
{



class GeometryReference: public GeometryBase
{
	typedef GeometryBase Parent;
public:
	
	GeometryReference(GeometryBase* pGeometry);
	virtual ~GeometryReference();

	bool IsGeometryReference();

	const xform& GetOffsetTransformation() const;
	void SetOffsetTransformation(const xform& pOffset);
	void AddOffset(const vec3& pOffset);
	const xform& GetExtraOffsetTransformation() const;
	void SetExtraOffsetTransformation(const xform& pOffset);

	// Overloads from GeometryBase.
	const xform& GetTransformation();
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility pVolatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount()  const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	vtx_idx_t*     GetIndexData() const;
	uint8*         GetColorData() const;
	float*         GetNormalData() const;

	GeometryBase* GetParentGeometry() const;

protected:
private:
	GeometryBase* mGeometry;

	xform mOriginalOffset;
	xform mExtraOffset;
	xform mReturnTransformation;
};



}
