
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#ifndef CHUNKYBONEGEOMETRY_H
#define CHUNKYBONEGEOMETRY_H



#include "../../Lepra/Include/Vector3D.h"
#include "ChunkyLoader.h"
#include "PhysicsEngine.h"



namespace TBC
{



class ChunkyBoneGeometry
{
public:
	ChunkyBoneGeometry(PhysicsEngine* pPhysics);
	virtual ~ChunkyBoneGeometry();
	// Retains ownership of returned phyics object.
	virtual PhysicsEngine::BodyID AddPhyicsBody(PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform) = 0;
	// Retains ownership of returned phyics object.
	virtual PhysicsEngine::TriggerID AddTrigger(const Lepra::TransformationF& pTransform) = 0;
	void Remove();

	virtual ChunkyType GetChunkyType() const = 0;
	virtual unsigned GetChunkySize() const = 0;
	virtual void SaveChunkyData(void* pData) const = 0;
	virtual void LoadChunkyData(const void* pData) = 0;

protected:
	PhysicsEngine* mPhysics;
	PhysicsEngine::BodyID mBodyId;
	PhysicsEngine::TriggerID mTriggerId;
};



/*class ChunkyBoneCapsule: public ChunkyBoneGeometry
{
public:
	ChunkyBoneCapsule(PhysicsEngine* pPhyics, Lepra::float32 pMass, Lepra::float32 pRadius,
		Lepra::float32 pLength, Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0);
	PhysicsEngine::BodyID AddPhyicsBody(PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform);
	PhysicsEngine::TriggerID AddTrigger(const Lepra::TransformationF& pTransform);

	ChunkyType GetChunkyType() const;
	unsigned GetChunkySize() const;
	void SaveChunkyData(void* pData) const;
	void LoadChunkyData(const void* pData);

private:
	Lepra::float32 mMass;
	Lepra::float32 mRadius;
	Lepra::float32 mLength;
	Lepra::float32 mFriction;
	Lepra::float32 mBounce;
};



class ChunkyBoneBox: public ChunkyBoneGeometry
{
public:
	ChunkyBoneBox(PhysicsEngine* pPhyics, Lepra::float32 pMass, const Lepra::Vector3DF& pSize,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0);
	PhysicsEngine::BodyID AddPhyicsBody(PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform);
	PhysicsEngine::TriggerID AddTrigger(const Lepra::TransformationF& pTransform);

	ChunkyType GetChunkyType() const;
	unsigned GetChunkySize() const;
	void SaveChunkyData(void* pData) const;
	void LoadChunkyData(const void* pData);

private:
	Lepra::float32 mMass;
	Lepra::Vector3DF mSize;
	Lepra::float32 mFriction;
	Lepra::float32 mBounce;
};*/



}



#endif // CHUNKYBONEGEOMETRY_H
