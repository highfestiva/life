/*
	Class:  GeometryBatch
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

#ifndef UIGEOMETRYBATCH_H
#define UIGEOMETRYBATCH_H

#include "../../TBC/Include/GeometryBase.h"

namespace UiTbc
{

class GeometryBatch: public TBC::GeometryBase
{
public:

	GeometryBatch(TBC::GeometryBase* pGeometry);
	virtual ~GeometryBatch();

	// Creates new instances.
	void SetInstances(const Lepra::Vector3DF* pDisplacement, 
			  int pNumInstances,
			  float pXScaleMin = 1.0f, float pXScaleMax = 1.0f,
			  float pYScaleMin = 1.0f, float pYScaleMax = 1.0f,
			  float pZScaleMin = 1.0f, float pZScaleMax = 1.0f);
	void SetInstances(const Lepra::Vector3DD* pDisplacement, 
			  int pNumInstances,
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
	virtual Lepra::uint32* GetIndexData() const;
	virtual Lepra::uint8*  GetColorData() const;

protected:
private:
	float* mVertex;
	float** mUV;
	Lepra::uint32* mIndex;
	Lepra::uint8* mColor;
	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mUVSetCount;

	TBC::GeometryBase* mGeometryInstance;
};

} // End namespace.

#endif