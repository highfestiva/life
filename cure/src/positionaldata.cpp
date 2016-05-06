
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/positionaldata.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/packer.h"



namespace cure {



#define CHECK_SIZE(size)		\
	if (_size < (int)(size))	\
	{				\
		deb_assert(false);		\
		return (-1);		\
	}

#define CHECK_TYPE(type)			\
	if (type != (Type)data[__size++])	\
	{					\
		deb_assert(false);			\
		return (-1);			\
	}



PositionalData::PositionalData():
	sub_hierarchy_scale_(1) {
}

PositionalData::~PositionalData() {
}

float PositionalData::GetScaledDifference(const PositionalData* reference) const {
	return (sub_hierarchy_scale_*GetBiasedDifference(reference));
}

void PositionalData::SetScale(float scale) {
	sub_hierarchy_scale_ = scale;
}

void PositionalData::Stop() {
	// To stop or not to stop...
}



int PositionalData6::GetStaticPackSize() {
	return (1+sizeof(xform::BaseType)*7 + sizeof(vec3::BaseType)*3*4);
}

int PositionalData6::GetPackSize() const {
	return (GetStaticPackSize());
}

int PositionalData6::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypePosition6;
	__size += PackerTransformation::Pack(&data[__size], transformation_);
	__size += PackerVector::Pack(&data[__size], velocity_);
	__size += PackerVector::Pack(&data[__size], acceleration_);
	__size += PackerVector::Pack(&data[__size], angular_velocity_);
	__size += PackerVector::Pack(&data[__size], angular_acceleration_);
	return (__size);
}

int PositionalData6::Unpack(const uint8* data, int _size) {
	CHECK_SIZE(1+sizeof(float)*7+sizeof(float)*3*4);
	int __size = 0;
	CHECK_TYPE(kTypePosition6);
	__size += PackerTransformation::Unpack(transformation_, &data[__size], sizeof(float)*7);
	__size += PackerVector::Unpack(velocity_, &data[__size], sizeof(float)*3);
	__size += PackerVector::Unpack(acceleration_, &data[__size], sizeof(float)*3);
	__size += PackerVector::Unpack(angular_velocity_, &data[__size], sizeof(float)*3);
	__size += PackerVector::Unpack(angular_acceleration_, &data[__size], sizeof(float)*3);
	return (__size);
}

float PositionalData6::GetBiasedDifference(const PositionalData* reference) const {
	const PositionalData6& _reference = (const PositionalData6&)*reference;
	float weighted_difference_sum =
		(GetDifference(transformation_.GetOrientation(), _reference.transformation_.GetOrientation())*1.0f +
		GetDifference(transformation_.GetPosition(), _reference.transformation_.GetPosition())*1.0f +
		GetDifference(velocity_, _reference.velocity_)*1.0f +
		GetDifference(acceleration_, _reference.acceleration_)*3.0f +	// Linear acceleration is by far the most important one when doing some simple "CrocketBalls".
		GetDifference(angular_velocity_, _reference.angular_velocity_)*1.0f +
		GetDifference(angular_acceleration_, _reference.angular_acceleration_)*1.0f);
	return weighted_difference_sum / 8;
}

void PositionalData6::Stop() {
	velocity_.Set(0, 0, 0);
	acceleration_.Set(0, 0, 0);
	angular_velocity_.Set(0, 0, 0);
	angular_acceleration_.Set(0, 0, 0);
}

float PositionalData6::GetDifference(const quat& q1, const quat& q2) {
	float weighted_difference_sum =
		(::fabs(q1.a-q2.a) +
		::fabs(q1.b-q2.b) +
		::fabs(q1.c-q2.c) +
		::fabs(q1.d-q2.d));
	return (weighted_difference_sum);
}

float PositionalData6::GetDifference(const vec3& v1, const vec3& v2) {
	float weighted_difference_sum =
		(::fabs(v1.x-v2.x) +
		::fabs(v1.y-v2.y) +
		::fabs(v1.z-v2.z));
	return (weighted_difference_sum);
}

PositionalData::Type PositionalData6::GetType() const {
	return (kTypePosition6);
}

void PositionalData6::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const PositionalData6& _copy = *(PositionalData6*)data;
	*this = _copy;
}

PositionalData* PositionalData6::Clone() const {
	return (new PositionalData6(*this));
}

loginstance(kNetwork, PositionalData6);



int PositionalData3::GetPackSize() const {
	return (1+sizeof(float)*3*3);
}

int PositionalData3::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypePosition3;
	__size += PackerReal::Pack(&data[__size], transformation_[0]);
	__size += PackerReal::Pack(&data[__size], transformation_[1]);
	__size += PackerReal::Pack(&data[__size], transformation_[2]);
	__size += PackerReal::Pack(&data[__size], velocity_[0]);
	__size += PackerReal::Pack(&data[__size], velocity_[1]);
	__size += PackerReal::Pack(&data[__size], velocity_[2]);
	__size += PackerReal::Pack(&data[__size], acceleration_[0]);
	__size += PackerReal::Pack(&data[__size], acceleration_[1]);
	__size += PackerReal::Pack(&data[__size], acceleration_[2]);
	return (__size);
}

int PositionalData3::Unpack(const uint8* data, int _size) {
	CHECK_SIZE(1+sizeof(transformation_[0])*3*3);
	int __size = 0;
	CHECK_TYPE(kTypePosition3);
	__size += PackerReal::Unpack(transformation_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(transformation_[1], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(transformation_[2], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(velocity_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(velocity_[1], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(velocity_[2], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(acceleration_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(acceleration_[1], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(acceleration_[2], &data[__size], sizeof(float));
	return (__size);
}

float PositionalData3::GetBiasedDifference(const PositionalData* reference) const {
	const PositionalData3& _reference = (const PositionalData3&)*reference;
	float weighted_difference_sum =
		::fabs(transformation_[0]-_reference.transformation_[0]) +
		::fabs(transformation_[1]-_reference.transformation_[1]) +
		::fabs(transformation_[2]-_reference.transformation_[2]) +
		::fabs(velocity_[0]-_reference.velocity_[0])*3.0f +		// Linear acceleration seems by far the most important one.
		::fabs(velocity_[1]-_reference.velocity_[1]) +
		::fabs(velocity_[2]-_reference.velocity_[2]) +
		::fabs(acceleration_[0]-_reference.acceleration_[0]) +
		::fabs(acceleration_[1]-_reference.acceleration_[1]) +
		::fabs(acceleration_[2]-_reference.acceleration_[2]);
	return weighted_difference_sum / 11;
}

PositionalData::Type PositionalData3::GetType() const {
	return (kTypePosition3);
}

void PositionalData3::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const PositionalData3& _copy = *(PositionalData3*)data;
	*this = _copy;
}

PositionalData* PositionalData3::Clone() const {
	return (new PositionalData3(*this));
}

void PositionalData3::Stop() {
	velocity_[0] = velocity_[1] = velocity_[2] = 0;
	acceleration_[0] = acceleration_[1] = acceleration_[2] = 0;
}



int PositionalData2::GetPackSize() const {
	return (1+sizeof(float)*2*3);
}

int PositionalData2::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypePosition2;
	__size += PackerReal::Pack(&data[__size], transformation_[0]);
	__size += PackerReal::Pack(&data[__size], transformation_[1]);
	__size += PackerReal::Pack(&data[__size], velocity_[0]);
	__size += PackerReal::Pack(&data[__size], velocity_[1]);
	__size += PackerReal::Pack(&data[__size], acceleration_[0]);
	__size += PackerReal::Pack(&data[__size], acceleration_[1]);
	return (__size);
}

int PositionalData2::Unpack(const uint8* data, int _size) {
	CHECK_SIZE(1+sizeof(transformation_[0])*2*3);
	int __size = 0;
	CHECK_TYPE(kTypePosition2);
	__size += PackerReal::Unpack(transformation_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(transformation_[1], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(velocity_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(velocity_[1], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(acceleration_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(acceleration_[1], &data[__size], sizeof(float));
	return (__size);
}

float PositionalData2::GetBiasedDifference(const PositionalData* reference) const {
	const PositionalData2& _reference = (const PositionalData2&)*reference;
	float weighted_difference_sum =
		::fabs(transformation_[0]-_reference.transformation_[0]) +
		::fabs(transformation_[1]-_reference.transformation_[1]) +
		::fabs(velocity_[0]-_reference.velocity_[0])*3.0f +		// Linear acceleration seems by far the most important one.
		::fabs(velocity_[1]-_reference.velocity_[1]) +
		::fabs(acceleration_[0]-_reference.acceleration_[0]) +
		::fabs(acceleration_[1]-_reference.acceleration_[1]);
	return weighted_difference_sum / 8;
}

PositionalData::Type PositionalData2::GetType() const {
	return (kTypePosition2);
}

void PositionalData2::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const PositionalData2& _copy = *(PositionalData2*)data;
	*this = _copy;
}

PositionalData* PositionalData2::Clone() const {
	return (new PositionalData2(*this));
}

void PositionalData2::Stop() {
	velocity_[0] = velocity_[1] = 0;
	acceleration_[0] = acceleration_[1] = 0;
}



int PositionalData1::GetPackSize() const {
	return (1+sizeof(float)*1*3);
}

int PositionalData1::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypePosition1;
	__size += PackerReal::Pack(&data[__size], transformation_);
	__size += PackerReal::Pack(&data[__size], velocity_);
	__size += PackerReal::Pack(&data[__size], acceleration_);
	return (__size);
}

int PositionalData1::Unpack(const uint8* data, int _size) {
	CHECK_SIZE(1+sizeof(transformation_)*1*3);
	int __size = 0;
	CHECK_TYPE(kTypePosition1);
	__size += PackerReal::Unpack(transformation_, &data[__size], sizeof(float));
	__size += PackerReal::Unpack(velocity_, &data[__size], sizeof(float));
	__size += PackerReal::Unpack(acceleration_, &data[__size], sizeof(float));
	return (__size);
}

float PositionalData1::GetBiasedDifference(const PositionalData* reference) const {
	const PositionalData1& _reference = (const PositionalData1&)*reference;
	float weighted_difference_sum =
		::fabs(transformation_-_reference.transformation_) +
		::fabs(velocity_-_reference.velocity_)*3.0f +		// Linear acceleration seems by far the most important one.
		::fabs(acceleration_-_reference.acceleration_);
	return weighted_difference_sum / 5;
}

PositionalData::Type PositionalData1::GetType() const {
	return (kTypePosition1);
}

void PositionalData1::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const PositionalData1& _copy = *(PositionalData1*)data;
	*this = _copy;
}

PositionalData* PositionalData1::Clone() const {
	return (new PositionalData1(*this));
}

void PositionalData1::Stop() {
	velocity_ = 0;
	acceleration_ = 0;
}



ObjectPositionalData::ObjectPositionalData() {
}

ObjectPositionalData::~ObjectPositionalData() {
	Clear();
}

void ObjectPositionalData::GhostStep(int pFrameCount, float delta_time) {
	if (pFrameCount < 0) {
		// Simply reverse delta time factor if going backwards.
		delta_time = -delta_time;
		pFrameCount = -pFrameCount;
	}
	for (int x = 0; x < pFrameCount; ++x) {
		// We just add acceleration to velocity, and velocity to position. An improvement
		// here might save us a few bytes of network data.
		position_.transformation_.GetPosition().Add(position_.velocity_*delta_time);
		position_.velocity_.Add(position_.acceleration_*delta_time);

		/*quat q;
		q.RotateAroundWorldX(position_.angular_velocity_.x*delta_time);
		q.RotateAroundWorldY(position_.angular_velocity_.y*delta_time);
		q.RotateAroundWorldZ(position_.angular_velocity_.z*delta_time);
		position_.transformation_.GetOrientation() *= q;
		position_.angular_velocity_.Add(position_.angular_acceleration_*delta_time);*/
	}
}

void ObjectPositionalData::Clear() {
	BodyPositionArray::iterator x = body_position_array_.begin();
	for (; x != body_position_array_.end(); ++x) {
		PositionalData* position = *x;
		delete (position);
	}
	body_position_array_.clear();
}

int ObjectPositionalData::GetPackSize() const {
	int __size = 1+sizeof(int16)+position_.GetPackSize();
	BodyPositionArray::const_iterator x = body_position_array_.begin();
	for (; x != body_position_array_.end(); ++x) {
		__size += (*x)->GetPackSize();
	}
	return (__size);
}

int ObjectPositionalData::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypeObject;
	__size += PackerInt16::Pack(&data[__size], GetPackSize()-1-sizeof(int16));
	__size += position_.Pack(&data[__size]);
	BodyPositionArray::const_iterator x = body_position_array_.begin();
	for (; x != body_position_array_.end(); ++x) {
		__size += (*x)->Pack(&data[__size]);
	}
	return (__size);
}

int ObjectPositionalData::Unpack(const uint8* data, int _size) {
	CHECK_SIZE((int)(1+sizeof(int16)+PositionalData6::GetStaticPackSize()));
	int __size = 0;
	CHECK_TYPE(kTypeObject);
	deb_assert(body_position_array_.size() == 0);
	int data_size;
	int sub_size = PackerInt16::Unpack(data_size, &data[__size], _size-__size);
	if (sub_size <= 0) {
		deb_assert(false);
		return (-1);
	}
	__size += sub_size;
	if (data_size < PositionalData6::GetStaticPackSize() || data_size > _size-__size) {
		deb_assert(false);
		return (-1);
	}
	_size = data_size+__size;
	sub_size = position_.Unpack(&data[__size], _size-__size);
	if (sub_size <= 0) {
		deb_assert(false);
		return (-1);
	}
	__size += sub_size;
	while (__size < _size) {
		PositionalData* position = 0;
		switch (data[__size]) {
			case kTypePosition6:	position = new PositionalData6;	break;
			case kTypePosition3:	position = new PositionalData3;	break;
			case kTypePosition2:	position = new PositionalData2;	break;
			case kTypePosition1:	position = new PositionalData1;	break;
			case kTypeReal3:	position = new RealData3;		break;
			case kTypeReal1:	position = new RealData1;		break;
		}
		if (!position) {
			deb_assert(false);
			__size = -1;
			break;
		}
		body_position_array_.push_back(position);
		sub_size = position->Unpack(&data[__size], _size-__size);
		if (sub_size > 0) {
			__size += sub_size;
		} else {
			__size = -1;
			break;
		}
	}
	return (__size);
}

float ObjectPositionalData::GetBiasedDifference(const PositionalData* reference) const {
	return GetBiasedTypeDifference(reference, false);
}

float ObjectPositionalData::GetBiasedTypeDifference(const PositionalData* reference, bool position_only) const {
	deb_assert(GetType() == reference->GetType());
	const ObjectPositionalData& _reference = (const ObjectPositionalData&)*reference;
	deb_assert(IsSameStructure(_reference));
	float diff = 0;
	diff += position_.GetBiasedDifference(&_reference.position_);
	BodyPositionArray::const_iterator x = body_position_array_.begin();
	BodyPositionArray::const_iterator y = _reference.body_position_array_.begin();
	int body_count = 1 + body_position_array_.size();
	for (; x != body_position_array_.end(); ++x, ++y) {
		deb_assert(y != _reference.body_position_array_.end());
		if (position_only) {
			const Type __type = (*x)->GetType();
			if (__type != kTypePosition6 && __type != kTypePosition3 && __type != kTypePosition2 && __type != kTypePosition1) {
				--body_count;
				continue;
			}

		}
		diff += (*x)->GetScaledDifference(*y);
	}
	deb_assert((x == body_position_array_.end()) && (y == _reference.body_position_array_.end()));
	return diff / body_count;
}

PositionalData* ObjectPositionalData::GetAt(size_t index) const {
	if (index >= body_position_array_.size()) {
		return (0);
	}
	return (body_position_array_[index]);
}

void ObjectPositionalData::SetAt(size_t index, PositionalData* data) {
	if (body_position_array_.size() >= index) {
		if (body_position_array_.size() == index) {
			body_position_array_.push_back(data);
		} else {
			delete (body_position_array_[index]);
			body_position_array_[index] = data;
		}
	} else {
		log_.Error("Trying to set positional data out of range!");
		deb_assert(false);
	}
}

void ObjectPositionalData::Trunkate(size_t _size) {
	for (size_t x = _size; x < body_position_array_.size(); ++x) {
		delete (body_position_array_[x]);
	}
	body_position_array_.resize(_size);
}

PositionalData::Type ObjectPositionalData::GetType() const {
	return (kTypeObject);
}

void ObjectPositionalData::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const ObjectPositionalData& _copy = *(ObjectPositionalData*)data;
	if (IsSameStructure(_copy)) {
		position_ = _copy.position_;
		BodyPositionArray::iterator x = body_position_array_.begin();
		BodyPositionArray::iterator y = ((ObjectPositionalData*)data)->body_position_array_.begin();
		for (; x != body_position_array_.end(); ++x, ++y) {
			PositionalData* position = *x;
			position->CopyData(*y);
		}
	} else {
		Clear();
		position_ = _copy.position_;
		BodyPositionArray::const_iterator x = _copy.body_position_array_.begin();
		for (; x != _copy.body_position_array_.end(); ++x) {
			body_position_array_.push_back((*x)->Clone());
		}
	}
}

PositionalData* ObjectPositionalData::Clone() const {
	ObjectPositionalData* _data = new ObjectPositionalData;
	_data->position_ = position_;
	BodyPositionArray::const_iterator x = body_position_array_.begin();
	for (; x != body_position_array_.end(); ++x) {
		_data->body_position_array_.push_back((*x)->Clone());
	}
	return (_data);
}

void ObjectPositionalData::Stop() {
	position_.Stop();
	BodyPositionArray::const_iterator x = body_position_array_.begin();
	for (; x != body_position_array_.end(); ++x) {
		(*x)->Stop();
	}
}

bool ObjectPositionalData::IsSameStructure(const ObjectPositionalData& copy) const {
	if (body_position_array_.size() != copy.body_position_array_.size()) {
		return (false);
	}
	BodyPositionArray::const_iterator x = body_position_array_.begin();
	BodyPositionArray::const_iterator y = copy.body_position_array_.begin();
	for (; x != body_position_array_.end(); ++x, ++y) {
		if ((*x)->GetType() != (*y)->GetType()) {
			return (false);
		}
	}
	return (true);
}

ObjectPositionalData::ObjectPositionalData(const ObjectPositionalData&) {
	deb_assert(false);
}

void ObjectPositionalData::operator=(const ObjectPositionalData&) {
	deb_assert(false);
}

loginstance(kNetwork, ObjectPositionalData);



int RealData3::GetPackSize() const {
	return (1+sizeof(value_[0])*3);
}

int RealData3::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypeReal3;
	__size += PackerReal::Pack(&data[__size], value_[0]);
	__size += PackerReal::Pack(&data[__size], value_[1]);
	__size += PackerReal::Pack(&data[__size], value_[2]);
	return (__size);
}

int RealData3::Unpack(const uint8* data, int _size) {
	CHECK_SIZE(1+sizeof(value_[0])*3);
	int __size = 0;
	CHECK_TYPE(kTypeReal3);
	__size += PackerReal::Unpack(value_[0], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(value_[1], &data[__size], sizeof(float));
	__size += PackerReal::Unpack(value_[2], &data[__size], sizeof(float));
	return (__size);
}

float RealData3::GetBiasedDifference(const PositionalData* reference) const {
	const RealData3& _reference = (const RealData3&)*reference;
	float weighted_difference_sum =
		::fabs(value_[0]-_reference.value_[0]) +
		::fabs(value_[1]-_reference.value_[1]) +
		::fabs(value_[2]-_reference.value_[2]);
	return weighted_difference_sum / 3;
}

PositionalData::Type RealData3::GetType() const {
	return kTypeReal3;
}

void RealData3::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const RealData3& _copy = *(RealData3*)data;
	*this = _copy;
}

PositionalData* RealData3::Clone() const {
	return (new RealData3(*this));
}



int RealData1::GetPackSize() const {
	return (1+sizeof(value_));
}

int RealData1::Pack(uint8* data) const {
	int __size = 0;
	data[__size++] = (uint8)kTypeReal1;
	__size += PackerReal::Pack(&data[__size], value_);
	return (__size);
}

int RealData1::Unpack(const uint8* data, int _size) {
	CHECK_SIZE(1+sizeof(value_));
	int __size = 0;
	CHECK_TYPE(kTypeReal1);
	__size += PackerReal::Unpack(value_, &data[__size], sizeof(float));
	return (__size);
}

float RealData1::GetBiasedDifference(const PositionalData* reference) const {
	const RealData1& _reference = (const RealData1&)*reference;
	float weighted_difference_sum =
		::fabs(value_-_reference.value_)*3;
	return weighted_difference_sum / 1;
}

PositionalData::Type RealData1::GetType() const {
	return (kTypeReal1);
}

void RealData1::CopyData(const PositionalData* data) {
	deb_assert(data->GetType() == GetType());
	const RealData1& _copy = *(RealData1*)data;
	*this = _copy;
}

PositionalData* RealData1::Clone() const {
	return (new RealData1(*this));
}



}
