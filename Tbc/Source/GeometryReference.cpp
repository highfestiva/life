
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/GeometryReference.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"



namespace Tbc
{



GeometryReference::GeometryReference(GeometryBase* pGeometry) :
	mGeometry(pGeometry)
{
	LEPRA_DEBUG_CODE(mName = _T("Ref->") + pGeometry->mName);
	LEPRA_ACQUIRE_RESOURCE(GeometryReference);
	Copy(mGeometry);
	mFlags = mDefaultFlags;
	SetFlag(mGeometry->GetFlags()&(~VALID_FLAGS_MASK) | TRANSFORMATION_CHANGED | REF_TRANSFORMATION_CHANGED | BIG_ORIENTATION_CHANGED);
	mRendererData = 0;
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

const xform& GeometryReference::GetOffsetTransformation() const
{
	return mOriginalOffset;
}

void GeometryReference::SetOffsetTransformation(const xform& pOffset)
{
	mOriginalOffset = pOffset;
	SetTransformationChanged(true);
}

void GeometryReference::AddOffset(const vec3& pOffset)
{
	mOriginalOffset.GetPosition() += pOffset;
	SetTransformationChanged(true);
}

const xform& GeometryReference::GetExtraOffsetTransformation() const
{
	return mExtraOffset;
}

void GeometryReference::SetExtraOffsetTransformation(const xform& pOffset)
{
	mExtraOffset = pOffset;
	SetTransformationChanged(true);
}

const xform& GeometryReference::GetTransformation()
{
	if (!CheckFlag(REF_TRANSFORMATION_CHANGED))
	{
/*#ifdef LEPRA_DEBUG
		xform lReturnTransformation = GetBaseTransformation();
		lReturnTransformation.GetPosition() += lReturnTransformation.GetOrientation() * mOriginalOffset.GetPosition();
		lReturnTransformation.GetOrientation() *= mOriginalOffset.GetOrientation();
		deb_assert(lReturnTransformation == mReturnTransformation);
#endif // Debug.*/
		return mReturnTransformation;
	}
	ClearFlag(REF_TRANSFORMATION_CHANGED);

	mReturnTransformation = GetBaseTransformation();
	vec3 lDelta;
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
