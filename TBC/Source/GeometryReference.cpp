
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Include/GeometryReference.h"



namespace TBC
{



GeometryReference::GeometryReference(GeometryBase* pGeometry) :
	mGeometry(pGeometry)
{
}

GeometryReference::~GeometryReference()
{
	mGeometry = 0;
}

bool GeometryReference::IsGeometryReference()
{
	return true;
}

void GeometryReference::SetOffsetTransformation(const Lepra::TransformationF& pOffset)
{
	mOffset = pOffset;
}

const Lepra::TransformationF& GeometryReference::GetTransformation() const
{
	mReturnTransformation = GetBaseTransformation();
	mReturnTransformation.GetPosition() += mReturnTransformation.GetOrientation() * mOffset.GetPosition();
	mReturnTransformation.GetOrientation() *= mOffset.GetOrientation();
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

Lepra::uint32* GeometryReference::GetIndexData() const
{
	return mGeometry->GetIndexData();
}

Lepra::uint8* GeometryReference::GetColorData() const
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
