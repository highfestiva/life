
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ChunkyBoneGeometry.h"



namespace TBC
{



ChunkyBoneGeometry::ChunkyBoneGeometry(PhysicsEngine* pPhysics):
	mPhysics(pPhysics),
	mBodyId(INVALID_BODY),
	mTriggerId(INVALID_TRIGGER)
{
}

ChunkyBoneGeometry::~ChunkyBoneGeometry()
{
	Remove();
}

void ChunkyBoneGeometry::Remove()
{
	if (mBodyId != INVALID_BODY)
	{
		mPhysics->DeleteBody(mBodyId);
		mBodyId = INVALID_BODY;
	}
	if (mTriggerId != INVALID_TRIGGER)
	{
		mPhysics->DeleteTrigger(mTriggerId);
		mTriggerId = INVALID_TRIGGER;
	}
}



/*ChunkyBoneCapsule::ChunkyBoneCapsule(PhysicsEngine* pPhysics, Lepra::float32 pMass, Lepra::float32 pRadius,
	Lepra::float32 pLength, Lepra::float32 pFriction, Lepra::float32 pBounce):
	ChunkyBoneGeometry(pPhysics),
	mMass(pMass),
	mRadius(pRadius),
	mLength(pLength),
	mFriction(pFriction),
	mBounce(pBounce)
{
}

PhysicsEngine::BodyID ChunkyBoneCapsule::AddPhyicsBody(PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform)
{
	Remove();
	mBodyId = mPhysics->CreateCapsule(pTransform, mMass, mRadius, mLength, pType, mFriction, mBounce);
	return (mBodyId);
}

PhysicsEngine::TriggerID ChunkyBoneCapsule::AddTrigger(const Lepra::TransformationF& pTransform)
{
	Remove();
	mTriggerId = mPhysics->CreateCapsuleTrigger(pTransform, mRadius, mLength, 0);
	return (mTriggerId);
}

ChunkyType ChunkyBoneCapsule::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_CAPSULE);
}

unsigned ChunkyBoneCapsule::GetChunkySize() const
{
	return (sizeof(Lepra::float32)*5);
}

void ChunkyBoneCapsule::SaveChunkyData(void* pData) const
{
	float* lData = (float*)pData;
	lData[0] = mMass;
	lData[1] = mRadius;
	lData[2] = mLength;
	lData[3] = mFriction;
	lData[4] = mBounce;
}

void ChunkyBoneCapsule::LoadChunkyData(const void* pData)
{
	const float* lData = (const float*)pData;
	mMass = lData[0];
	mRadius = lData[1];
	mLength = lData[2];
	mFriction = lData[3];
	mBounce = lData[4];
}



ChunkyBoneBox::ChunkyBoneBox(PhysicsEngine* pPhysics, Lepra::float32 pMass, const Lepra::Vector3DF& pSize,
	Lepra::float32 pFriction, Lepra::float32 pBounce):
	ChunkyBoneGeometry(pPhysics),
	mMass(pMass),
	mSize(pSize),
	mFriction(pFriction),
	mBounce(pBounce)
{
}

PhysicsEngine::BodyID ChunkyBoneBox::AddPhyicsBody(PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform)
{
	Remove();
	mBodyId = mPhysics->CreateBox(pTransform, mMass, mSize, pType, mFriction, mBounce);
	return (mBodyId);
}

PhysicsEngine::TriggerID ChunkyBoneBox::AddTrigger(const Lepra::TransformationF& pTransform)
{
	Remove();
	mTriggerId = mPhysics->CreateBoxTrigger(pTransform, mSize, 0);
	return (mTriggerId);
}

ChunkyType ChunkyBoneBox::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_BOX);
}

unsigned ChunkyBoneBox::GetChunkySize() const
{
	return (sizeof(Lepra::float32)*6);
}

void ChunkyBoneBox::SaveChunkyData(void* pData) const
{
	float* lData = (float*)pData;
	lData[0] = mMass;
	lData[1] = mSize.x;
	lData[2] = mSize.y;
	lData[3] = mSize.z;
	lData[4] = mFriction;
	lData[5] = mBounce;
}

void ChunkyBoneBox::LoadChunkyData(const void* pData)
{
	const float* lData = (const float*)pData;
	mMass = lData[0];
	mSize.x = lData[1];
	mSize.x = lData[2];
	mSize.x = lData[3];
	mFriction = lData[4];
	mBounce = lData[5];
}*/



}
