/*
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

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



#include "../../TBC/Include/GeometryBase.h"
#include "../Include/UiTBC.h"



namespace UiTbc
{



class GeometryBatch: public TBC::GeometryBase
{
public:

	GeometryBatch(TBC::GeometryBase* pGeometry);
	virtual ~GeometryBatch();

	// Creates new instances.
	void SetInstances(const TransformationF* pDisplacement, const Vector3DF& pRootOffset,
		int pNumInstances, uint32 pRandomSeed,
		float pXScaleMin = 1.0f, float pXScaleMax = 1.0f,
		float pYScaleMin = 1.0f, float pYScaleMax = 1.0f,
		float pZScaleMin = 1.0f, float pZScaleMax = 1.0f);
	void ClearAllInstances();

	virtual void SetGeometryVolatility(GeometryVolatility pVolatility);
	virtual GeometryVolatility GetGeometryVolatility() const;

	virtual PrimitiveType GetPrimitiveType() const;
	virtual ColorFormat GetColorFormat() const;

	virtual unsigned int GetMaxVertexCount()  const;
	virtual unsigned int GetMaxIndexCount() const;

	virtual unsigned int GetVertexCount()  const;
	virtual unsigned int GetIndexCount() const;
	virtual unsigned int GetUVSetCount()    const;

	virtual float*         GetVertexData() const;
	virtual float*         GetUVData(unsigned int pUVSet) const;
	virtual uint32* GetIndexData() const;
	virtual uint8*  GetColorData() const;

protected:
private:
	float* mVertex;
	float** mUV;
	uint32* mIndex;
	uint8* mColor;
	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mUVSetCount;

	TBC::GeometryBase* mGeometryInstance;
};



}
