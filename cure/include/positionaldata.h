
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "../../lepra/include/log.h"
#include "../../lepra/include/transformation.h"
#include "../../lepra/include/vector3d.h"
#include "../include/cure.h"



namespace cure {



class PositionalData {
public:
	enum Type {
		kTypeLowest	= 1,
		kTypeObject	= kTypeLowest,
		kTypePosition6,
		kTypePosition3,
		kTypePosition2,
		kTypePosition1,
		kTypeReal3,
		kTypeReal1,
		kTypeCount
	};

	PositionalData();
	virtual ~PositionalData();
	virtual int GetPackSize() const = 0;
	virtual int Pack(uint8* data) const = 0;
	virtual int Unpack(const uint8* data, int size) = 0;
	float GetScaledDifference(const PositionalData* reference) const;
	void SetScale(float scale);
	virtual Type GetType() const = 0;
	virtual void CopyData(const PositionalData* data) = 0;
	virtual PositionalData* Clone() const = 0;

	virtual void Stop();

protected:
	virtual float GetBiasedDifference(const PositionalData* reference) const = 0;

private:
	float sub_hierarchy_scale_;
};

// Six degrees of freedom in position, velocity and acceleration.
class PositionalData6: public PositionalData {
public:
	xform transformation_;
	vec3 velocity_;
	vec3 acceleration_;
	vec3 angular_velocity_;
	vec3 angular_acceleration_;

	static int GetStaticPackSize();
	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;

	virtual void Stop();

private:
	static float GetDifference(const quat& q1, const quat& q2);
	static float GetDifference(const vec3& v1, const vec3& v2);

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;

	logclass();
};

// Three degrees of freedom in position, velocity and acceleration.
class PositionalData3: public PositionalData {
public:
	float transformation_[3];
	float velocity_[3];
	float acceleration_[3];

	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;

	virtual void Stop();
};

// Two degrees of freedom in position, velocity and acceleration.
class PositionalData2: public PositionalData {
public:
	float transformation_[2];
	float velocity_[2];
	float acceleration_[2];

	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;

	virtual void Stop();
};

// One degree of freedom in position, velocity and acceleration.
class PositionalData1: public PositionalData {
public:
	float transformation_;
	float velocity_;
	float acceleration_;

	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;

	virtual void Stop();
};

// A container with it's own positional info.
class ObjectPositionalData: public PositionalData {
public:
	PositionalData6 position_;

	typedef std::vector<PositionalData*> BodyPositionArray;
	BodyPositionArray body_position_array_;

	ObjectPositionalData();
	virtual ~ObjectPositionalData();

	/// Step forward to compensate for lag. Used for network ghosts.
	void GhostStep(int step_count, float frame_time);

	void Clear();
	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;
	float GetBiasedTypeDifference(const PositionalData* reference, bool position_only) const;

	PositionalData* GetAt(size_t index) const;
	void SetAt(size_t index, PositionalData* data);
	void Trunkate(size_t size);

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;

	virtual void Stop();

	bool IsSameStructure(const ObjectPositionalData& copy) const;

private:
	ObjectPositionalData(const ObjectPositionalData&);
	void operator=(const ObjectPositionalData&);

	logclass();
};

class RealData3: public PositionalData {
public:
	float value_[3];

	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;
};

class RealData1: public PositionalData {
public:
	float value_;

	int GetPackSize() const;
	int Pack(uint8* data) const;
	int Unpack(const uint8* data, int size);
	float GetBiasedDifference(const PositionalData* reference) const;

	Type GetType() const;
	void CopyData(const PositionalData* data);
	PositionalData* Clone() const;
};



}
