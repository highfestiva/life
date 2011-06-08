
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/GeometryReference.h"
#include <assert.h>



namespace TBC
{



GeometryReference::GeometryReference(GeometryBase* pGeometry) :
	mGeometry(pGeometry)
{
	LEPRA_DEBUG_CODE(mName = _T("Ref->") + pGeometry->mName);
	SetFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED);
}

GeometryReference::~GeometryReference()
{
	mGeometry = 0;
}

bool GeometryReference::IsGeometryReference()
{
	return true;
}

const TransformationF& GeometryReference::GetOffsetTransformation() const
{
	return mOffset;
}

void GeometryReference::SetOffsetTransformation(const TransformationF& pOffset)
{
	SetFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED);
	mOffset = pOffset;
}

const TransformationF& GeometryReference::GetTransformation()
{
	if (!CheckFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED))
	{
/*#ifdef LEPRA_DEBUG
		TransformationF lReturnTransformation = GetBaseTransformation();
		lReturnTransformation.GetPosition() += lReturnTransformation.GetOrientation() * mOffset.GetPosition();
		lReturnTransformation.GetOrientation() *= mOffset.GetOrientation();
		assert(lReturnTransformation == mReturnTransformation);
#endif // Debug.*/
		return (mReturnTransformation);
	}

	mReturnTransformation = GetBaseTransformation();
	mReturnTransformation.GetPosition() += mReturnTransformation.GetOrientation() * mOffset.GetPosition();
	mReturnTransformation.GetOrientation() *= mOffset.GetOrientation();
	ClearFlag(REF_TRANSFORMATION_CHANGED);
	return (mReturnTransformation);
}

GeometryBase::GeometryVolatility GeometryReference::GetGeometryVolatility() const
{
	return mGeometry->GetGeometryVolatility();
}

void GeometryReference::SetGeometryVolatility(GeometryVolatility pVolatility)
{
	mGeometry->SetGeometryVolatility(pVolatility);
}

GeometryBase::PrimitiveType GeometryReference::GetPrimitiveType() const
{
	return mGeometry->GetPrimitiveType();
}

unsigned int GeometryReference::GetMaxVertexCount()  const
{
	return mGeometry->GetMaxVertexCount();
}

unsigned int GeometryReference::GetMaxIndexCount() const
{
	return mGeometry->GetMaxIndexCount();
}

unsigned int GeometryReference::GetVertexCount()  const
{
	return mGeometry->GetVertexCount();
}

unsigned int GeometryReference::GetIndexCount() const
{
	return mGeometry->GetIndexCount();
}

unsigned int GeometryReference::GetUVSetCount()    const
{
	return mGeometry->GetUVSetCount();
}

float* GeometryReference::GetVertexData() const
{
	return mGeometry->GetVertexData();
}

float* GeometryReference::GetUVData(unsigned int pUVSet) const
{
	return mGeometry->GetUVData(pUVSet);
}

vtx_idx_t* GeometryReference::GetIndexData() const
{
	return mGeometry->GetIndexData();
}

uint8* GeometryReference::GetColorData() const
{
	return mGeometry->GetColorData();
}

float* GeometryReference::GetNormalData() const
{
	return mGeometry->GetNormalData();
}

GeometryBase* GeometryReference::GetParentGeometry() const
{
	return mGeometry;
}



}
