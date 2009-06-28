
// Author: Jonas Bystr�m
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include <vector>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Lepra/Include/Vector3D.h"



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
		TYPE_REAL_4,
		TYPE_REAL_1_BOOL,
		TYPE_REAL_1,
		TYPE_COUNT
	};

	PositionalData();
	virtual ~PositionalData();
	virtual int GetPackSize() const = 0;
	virtual int Pack(Lepra::uint8* pData) const = 0;
	virtual int Unpack(const Lepra::uint8* pData, int pSize) = 0;
	float GetScaledDifference(const PositionalData* pReference) const;
	void SetScale(float pScale);
	virtual Type GetType() const = 0;
	virtual void CopyData(const PositionalData* pData) = 0;
	virtual PositionalData* Clone() = 0;

protected:
	virtual float GetBiasedDifference(const PositionalData* pReference) const = 0;

private:
	float mScale;
};

// Six degrees of freedom in position, velocity and acceleration.
class PositionalData6: public PositionalData
{
public:
	Lepra::TransformationF mTransformation;
	Lepra::Vector3DF mVelocity;
	Lepra::Vector3DF mAcceleration;
	Lepra::Vector3DF mAngularVelocity;
	Lepra::Vector3DF mAngularAcceleration;

	static int GetStaticPackSize();
	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

private:
	static float GetDifference(const Lepra::QuaternionF& pQ1, const Lepra::QuaternionF& pQ2);
	static float GetDifference(const Lepra::Vector3DF& pV1, const Lepra::Vector3DF& pV2);

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();

	LOG_CLASS_DECLARE();
};

// Three degrees of freedom in position, velocity and acceleration.
class PositionalData3: public PositionalData
{
public:
	float mTransformation[3];
	float mVelocity[3];
	float mAcceleration[3];

	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();
};

// Two degrees of freedom in position, velocity and acceleration.
class PositionalData2: public PositionalData
{
public:
	float mTransformation[2];
	float mVelocity[2];
	float mAcceleration[2];

	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();
};

// One degree of freedom in position, velocity and acceleration.
class PositionalData1: public PositionalData
{
public:
	float mTransformation;
	float mVelocity;
	float mAcceleration;

	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();
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
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	PositionalData* GetAt(size_t pIndex) const;
	void SetAt(size_t pIndex, PositionalData* pData);
	void Trunkate(size_t pSize);

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();

	bool IsSameStructure(const ObjectPositionalData& pCopy) const;

private:
	ObjectPositionalData(const ObjectPositionalData&);
	void operator=(const ObjectPositionalData&);

	LOG_CLASS_DECLARE();
};

class RealData4: public PositionalData
{
public:
	float mValue[4];

	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();
};

class RealData1Bool: public PositionalData
{
public:
	float mValue;
	bool mBool;

	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();
};

class RealData1: public PositionalData
{
public:
	float mValue;

	int GetPackSize() const;
	int Pack(Lepra::uint8* pData) const;
	int Unpack(const Lepra::uint8* pData, int pSize);
	float GetBiasedDifference(const PositionalData* pReference) const;

	Type GetType() const;
	void CopyData(const PositionalData* pData);
	PositionalData* Clone();
};



}
