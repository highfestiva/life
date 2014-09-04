
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/Cure.h"



namespace Cure
{



class PositionalData
{
public:
	enum Type
	{
		TYPE_LOWEST	= 1,
		TYPE_OBJECT	= TYPE_LOWEST,
		TYPE_POSITION_6,
		TYPE_POSITION_3,
		TYPE_POSITION_2,
		TYPE_POSITION_1,
		TYPE_REAL_3,
		TYPE_REAL_1,
		TYPE_COUNT
	};

	PositionalData();
	virtual ~PositionalData();
	virtual int GetPackSize() const = 0;
	virtual int Pack(uint8* pData) const = 0;
	virtual int Unpack(const uint8* pData, int pSize) = 0;
	float GetScaledDifference(const PositionalData* pReference) const;
	void SetScale(float pScale);
	virtual Type GetType() const = 0;
	virtual void CopyData(const PositionalData* pData) = 0;
	virtual PositionalData* Clone() const = 0;

	virtual void Stop();

protected:
	virtual float GetBiasedDifference(const PositionalData* pReference) const = 0;

private:
	float mSubHierarchyScale;
};

// Six degrees of freedom in position, velocity and acceleration.
class PositionalData6: public PositionalData
{
public:
	xform mTransformation;
	vec3 mVelocity;
	vec3 mAcceleration;
	vec3 mAngularVelocity;
	vec3 mAngularAcceleration;

	static int GetStaticPackSize();
	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	virtual void Stop();

private:
	static float GetDifference(const quat& pQ1, const quat& pQ2);
	static float GetDifference(const vec3& pV1, const vec3& pV2);

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;

	logclass();
};

// Three degrees of freedom in position, velocity and acceleration.
class PositionalData3: public PositionalData
{
public:
	float mTransformation[3];
	float mVelocity[3];
	float mAcceleration[3];

	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;

	virtual void Stop();
};

// Two degrees of freedom in position, velocity and acceleration.
class PositionalData2: public PositionalData
{
public:
	float mTransformation[2];
	float mVelocity[2];
	float mAcceleration[2];

	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;

	virtual void Stop();
};

// One degree of freedom in position, velocity and acceleration.
class PositionalData1: public PositionalData
{
public:
	float mTransformation;
	float mVelocity;
	float mAcceleration;

	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;

	virtual void Stop();
};

// A container with it's own positional info.
class ObjectPositionalData: public PositionalData
{
public:
	PositionalData6 mPosition;

	typedef std::vector<PositionalData*> BodyPositionArray;
	BodyPositionArray mBodyPositionArray;

	ObjectPositionalData();
	virtual ~ObjectPositionalData();

	/// Step forward to compensate for lag. Used for network ghosts.
	void GhostStep(int pStepCount, float pFrameTime);

	void Clear();
	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;
	float GetBiasedTypeDifference(const PositionalData* pReference, bool pPositionOnly) const;

	PositionalData* GetAt(size_t pIndex) const;
	void SetAt(size_t pIndex, PositionalData* pData);
	void Trunkate(size_t pSize);

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;

	virtual void Stop();

	bool IsSameStructure(const ObjectPositionalData& pCopy) const;

private:
	ObjectPositionalData(const ObjectPositionalData&);
	void operator=(const ObjectPositionalData&);

	logclass();
};

class RealData3: public PositionalData
{
public:
	float mValue[3];

	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;
};

class RealData1: public PositionalData
{
public:
	float mValue;

	int GetPackSize() const;
	int Pack(uint8* pData) const;
	int Unpack(const uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone() const;
};



}
