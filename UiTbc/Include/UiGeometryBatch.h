/*
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	The vegetation patch is a group of objects sharing the same
	geometry. Thus, a tree can be duplicated into a hundred copies 
	of the same tree-mesh, and stored as only one single geometry 
	instance.

	This increases rendering performance in cases where thousand of
	instances of the same geometry should be rendered, and especially
	if this geometry is a low-poly mesh.
*/



#pragma once



#include "../../Tbc/Include/GeometryBase.h"
#include "../Include/UiTbc.h"



namespace UiTbc
{



class GeometryBatch: public Tbc::GeometryBase
{
public:

	GeometryBatch(Tbc::GeometryBase* pGeometry);
	virtual ~GeometryBatch();

	// Creates new instances.
	void SetInstances(const xform* pDisplacement, const vec3& pRootOffset,
		int pNumInstances, uint32 pRandomSeed,
		float pXScaleMin, float pXScaleMax,
		float pYScaleMin, float pYScaleMax,
		float pZScaleMin, float pZScaleMax);
	void ClearAllInstances();

	virtual void SetGeometryVolatility(GeometryVolatility pVolatility);
	virtual GeometryVolatility GetGeometryVolatility() const;

	virtual ColorFormat GetColorFormat() const;

	virtual unsigned int GetMaxVertexCount()  const;
	virtual unsigned int GetMaxIndexCount() const;

	virtual unsigned int GetVertexCount()  const;
	virtual unsigned int GetIndexCount() const;
	virtual unsigned int GetUVSetCount()    const;

	virtual float*         GetVertexData() const;
	virtual float*         GetUVData(unsigned int pUVSet) const;
	virtual vtx_idx_t*     GetIndexData() const;
	virtual void           SetIndexData(vtx_idx_t* pIndexData, unsigned pIndexCount, unsigned pMaxIndexCount);
	virtual uint8*         GetColorData() const;

protected:
private:
	float* mVertex;
	float** mUV;
	vtx_idx_t* mIndex;
	uint8* mColor;
	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mMaxIndexCount;
	unsigned int mUVSetCount;

	Tbc::GeometryBase* mGeometryInstance;
};



}
