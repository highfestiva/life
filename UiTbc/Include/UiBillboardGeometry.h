
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../../Tbc/Include/GeometryBase.h"
#include "UiTbc.h"



namespace UiTbc
{



// If a billboard uses texture variations, they should be layed out equidistant horizontally on the same map.
class BillboardGeometry: public Tbc::GeometryBase
{
public:
	BillboardGeometry(float pAspectRatio, unsigned pUVSetCount);
	virtual ~BillboardGeometry();

	virtual unsigned GetMaxVertexCount() const;
	virtual unsigned GetMaxIndexCount() const;

	virtual unsigned GetVertexCount() const;
	virtual unsigned GetIndexCount() const;
	virtual unsigned GetUVSetCount() const;

	virtual float* GetVertexData() const;
	void SetVertexData(const float pVertexData[12]);
	virtual float* GetUVData(unsigned pUVSet = 0) const;
	virtual vtx_idx_t* GetIndexData() const;
	virtual uint8* GetColorData() const;

	// Overloads from Tbc::GeometryBase.
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility pVolatility);

private:
	float mVertexData[12];	// Quadruplets of (x, y, z).
	float* mUVData;		// Doublets of (u, v).
	unsigned mUVSetCount;
	vtx_idx_t mIndexData[4];
};



}