
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/GeometryReference.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"



namespace TBC
{



GeometryReference::GeometryReference(GeometryBase* pGeometry) :
	mGeometry(pGeometry)
{
	LEPRA_DEBUG_CODE(mName = _T("Ref->") + pGeometry->mName);
	LEPRA_ACQUIRE_RESOURCE(GeometryReference);
	SetFlag(pGeometry->GetFlags()&(~VALID_FLAGS_MASK) | TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED);
}

GeometryReference::~GeometryReference()
{
	mGeometry = 0;
	LEPRA_RELEASE_RESOURCE(GeometryReference);
}

bool GeometryReference::IsGeometryReference()
{
	return true;
}

const TransformationF& GeometryReference::GetOffsetTransformation() const
{
	return mOriginalOffset;
}

void GeometryReference::SetOffsetTransformation(const TransformationF& pOffset)
{
	mOriginalOffset = pOffset;
	SetFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED);
}

void GeometryReference::AddOffset(const Vector3DF& pOffset)
{
	mOriginalOffset.GetPosition() += pOffset;
	SetFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED);
}

const TransformationF& GeometryReference::GetExtraOffsetTransformation() const
{
	return mExtraOffset;
}

void GeometryReference::SetExtraOffsetTransformation(const TransformationF& pOffset)
{
	mExtraOffset = pOffset;
	SetFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED);
}

const TransformationF& GeometryReference::GetTransformation()
{
	if (!CheckFlag(TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED))
	{
/*#ifdef LEPRA_DEBUG
		TransformationF lReturnTransformation = GetBaseTransformation();
		lReturnTransformation.GetPosition() += lReturnTransformation.GetOrientation() * mOriginalOffset.GetPosition();
		lReturnTransformation.GetOrientation() *= mOriginalOffset.GetOrientation();
		deb_assert(lReturnTransformation == mReturnTransformation);
#endif // Debug.*/
		return mReturnTransformation;
	}
	ClearFlag(REF_TRANSFORMATION_CHANGED);

	mReturnTransformation = GetBaseTransformation();
	Vector3DF lDelta;
	mReturnTransformation.GetOrientation().FastRotatedVector(
		mReturnTransformation.GetOrientation().GetConjugate(),
		lDelta,
		mOriginalOffset.GetPosition() + mExtraOffset.GetPosition());
	mReturnTransformation.GetPosition() += lDelta;
	mReturnTransformation.GetOrientation() *= mOriginalOffset.GetOrientation();
	mReturnTransformation.GetOrientation() *= mExtraOffset.GetOrientation();
	return mReturnTransformation;
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
