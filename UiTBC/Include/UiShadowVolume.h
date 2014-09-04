
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTbc.h"
#include "../../Tbc/Include/GeometryBase.h"
#include "../../Lepra/Include/Transformation.h"



namespace UiTbc
{



class ShadowVolume: public Tbc::GeometryBase
{
public:
	ShadowVolume(Tbc::GeometryBase* pParentGeometry);
	virtual ~ShadowVolume();

	void UpdateShadowVolume(const vec3& pLightPos, float pLightRadius, bool pDirectional);

	Tbc::GeometryBase* GetParentGeometry();
	void SetParentGeometry(Tbc::GeometryBase* pParentGeometry);

	Tbc::GeometryBase::GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(Tbc::GeometryBase::GeometryVolatility pVolatility);

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount() const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	vtx_idx_t*     GetIndexData() const;
	Lepra::uint8*  GetColorData() const;
	float*         GetNormalData() const;

protected:
	void ExtrudeEdges();
private:

	struct TriangleOrientation
	{
		unsigned mV0;
		unsigned mV1;
		unsigned mV2;
		bool mChecked;
		bool mIsFrontFacing;
	};

	void InitVertices();
	void InitTO();

	float* mVertexData;
	vtx_idx_t* mIndexData;

	// Array with mNumParentTriangles elements.
	TriangleOrientation* mTriangleOrientation;

	unsigned int mVertexCount;
	unsigned int mTriangleCount;
	unsigned int mParentVertexCount;
	unsigned int mMaxTriangleCount;

	quat mPreviousOrientation;

	// The geometry that cast this shadow.
	Tbc::GeometryBase* mParentGeometry;
};



}
