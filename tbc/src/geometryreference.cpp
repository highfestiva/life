
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/geometryreference.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/resourcetracker.h"



namespace tbc {



GeometryReference::GeometryReference(GeometryBase* geometry) :
	geometry_(geometry) {
	LEPRA_DEBUG_CODE(name_ = "Ref->" + geometry->name_);
	LEPRA_ACQUIRE_RESOURCE(GeometryReference);
	Copy(geometry_);
	flags_ = default_flags_;
	SetFlag(geometry_->GetFlags()&(~kValidFlagsMask) | kTransformationChanged | kRefTransformationChanged | kBigOrientationChanged);
	renderer_data_ = 0;
}

GeometryReference::~GeometryReference() {
	geometry_ = 0;
	LEPRA_RELEASE_RESOURCE(GeometryReference);
}

bool GeometryReference::IsGeometryReference() {
	return true;
}

const xform& GeometryReference::GetOffsetTransformation() const {
	return original_offset_;
}

void GeometryReference::SetOffsetTransformation(const xform& offset) {
	original_offset_ = offset;
	SetTransformationChanged(true);
}

void GeometryReference::AddOffset(const vec3& offset) {
	original_offset_.GetPosition() += offset;
	SetTransformationChanged(true);
}

const xform& GeometryReference::GetExtraOffsetTransformation() const {
	return extra_offset_;
}

void GeometryReference::SetExtraOffsetTransformation(const xform& offset) {
	extra_offset_ = offset;
	SetTransformationChanged(true);
}

const xform& GeometryReference::GetTransformation() {
	if (!CheckFlag(kRefTransformationChanged)) {
/*#ifdef LEPRA_DEBUG
		xform lReturnTransformation = GetBaseTransformation();
		lReturnTransformation.GetPosition() += lReturnTransformation.GetOrientation() * original_offset_.GetPosition();
		lReturnTransformation.GetOrientation() *= original_offset_.GetOrientation();
		deb_assert(lReturnTransformation == return_transformation_);
#endif // Debug.*/
		return return_transformation_;
	}
	ClearFlag(kRefTransformationChanged);

	return_transformation_ = GetBaseTransformation();
	vec3 delta;
	return_transformation_.GetOrientation().FastRotatedVector(
		return_transformation_.GetOrientation().GetConjugate(),
		delta,
		original_offset_.GetPosition() + extra_offset_.GetPosition());
	return_transformation_.GetPosition() += delta;
	return_transformation_.GetOrientation() *= original_offset_.GetOrientation();
	return_transformation_.GetOrientation() *= extra_offset_.GetOrientation();
	return return_transformation_;
}

GeometryBase::GeometryVolatility GeometryReference::GetGeometryVolatility() const {
	return geometry_->GetGeometryVolatility();
}

void GeometryReference::SetGeometryVolatility(GeometryVolatility volatility) {
	geometry_->SetGeometryVolatility(volatility);
}

unsigned int GeometryReference::GetMaxVertexCount()  const {
	return geometry_->GetMaxVertexCount();
}

unsigned int GeometryReference::GetMaxIndexCount() const {
	return geometry_->GetMaxIndexCount();
}

unsigned int GeometryReference::GetVertexCount()  const {
	return geometry_->GetVertexCount();
}

unsigned int GeometryReference::GetIndexCount() const {
	return geometry_->GetIndexCount();
}

unsigned int GeometryReference::GetUVSetCount()    const {
	return geometry_->GetUVSetCount();
}

float* GeometryReference::GetVertexData() const {
	return geometry_->GetVertexData();
}

float* GeometryReference::GetUVData(unsigned int uv_set) const {
	return geometry_->GetUVData(uv_set);
}

vtx_idx_t* GeometryReference::GetIndexData() const {
	return geometry_->GetIndexData();
}

uint8* GeometryReference::GetColorData() const {
	return geometry_->GetColorData();
}

float* GeometryReference::GetNormalData() const {
	return geometry_->GetNormalData();
}

GeometryBase* GeometryReference::GetParentGeometry() const {
	return geometry_;
}



}
