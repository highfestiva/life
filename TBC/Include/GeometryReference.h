
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "GeometryBase.h"
#include "../../Lepra/Include/Transformation.h"



namespace TBC
{



class GeometryReference: public GeometryBase
{
	typedef GeometryBase Parent;
public:
	
	GeometryReference(GeometryBase* pGeometry);
	virtual ~GeometryReference();

	bool IsGeometryReference();

	void SetOffsetTransformation(const Lepra::TransformationF& pOffset);

	// Overloads from GeometryBase.
	const Lepra::TransformationF& GetTransformation() const;
	GeometryVolatility GetGeometryVolatility() const;
	void SetGeometryVolatility(GeometryVolatility pVolatility);

	PrimitiveType GetPrimitiveType() const;

	unsigned int GetMaxVertexCount()  const;
	unsigned int GetMaxIndexCount() const;

	unsigned int GetVertexCount()  const;
	unsigned int GetIndexCount()  const;
	unsigned int GetUVSetCount()    const;

	float*         GetVertexData() const;
	float*         GetUVData(unsigned int pUVSet) const;
	Lepra::uint32* GetIndexData() const;
	Lepra::uint8*  GetColorData() const;
	float*         GetNormalData() const;

	GeometryBase* GetParentGeometry() const;

protected:
private:
	GeometryBase* mGeometry;

	Lepra::TransformationF mOffset;
	mutable Lepra::TransformationF mReturnTransformation;
};



}
