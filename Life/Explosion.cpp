
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "Explosion.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/CppContextObject.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../TBC/Include/ChunkyPhysics.h"



namespace Life
{



float Explosion::CalculateForce(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength)
{
	return Force(pPhysicsManager, pObject, pPosition, pStrength, false);
}

float Explosion::PushObject(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength)
{
	return Force(pPhysicsManager, pObject, pPosition, pStrength, true);
}

void Explosion::FallApart(TBC::PhysicsManager* pPhysicsManager, Cure::CppContextObject* pObject)
{
	TBC::ChunkyPhysics* lPhysics = pObject->ContextObject::GetPhysics();
	const int lBoneCount = lPhysics->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(x);
		if (lGeometry->GetBoneType() != TBC::ChunkyBoneGeometry::BONE_BODY)
		{
			continue;
		}
		if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
		{
			if (pObject->GetClass()->IsPhysRoot(x))
			{
				pPhysicsManager->DetachToDynamic(lGeometry->GetBodyId(), lGeometry->GetMass());
			}
		}
		else if (lGeometry->GetJointId() != TBC::INVALID_JOINT)
		{
			pPhysicsManager->DeleteJoint(lGeometry->GetJointId());
			lGeometry->ResetJointId();
		}
		// This is so that the different parts of the now broken object can collide with each other.
		pPhysicsManager->EnableCollideWithSelf(lGeometry->GetBodyId(), true);
	}

	lPhysics->ClearEngines();

	pObject->QuerySetChildishness(0);
}



float Explosion::Force(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength, bool pApplyForce)
{
	TBC::ChunkyPhysics* lPhysics = pObject->ContextObject::GetPhysics();
	if (!lPhysics)
	{
		return 0;
	}
	const float lExplosiveDig = 0.75f;	// How much below the collision point the explosion "digs" things up. Adds angular velocity to objects.
	// Dynamics only get hit in the main body, while statics gets all their dynamic sub-bodies hit.
	const Vector3DF lEpicenter = pPosition + Vector3DF(0, 0, -lExplosiveDig);
	const bool lIsDynamic = (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	const int lBoneStart = lIsDynamic? 0 : 1;
	const int lBoneCount = lPhysics->GetBoneCount();
	float lForce = 0;
	int y;
	for (y = lBoneStart; y < lBoneCount; ++y)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(y);
		if (lGeometry->GetBoneType() != TBC::ChunkyBoneGeometry::BONE_BODY
			|| pPhysicsManager->IsStaticBody(lGeometry->GetBodyId()))
		{
			continue;
		}
		Vector3DF lBodyCenter = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
		Vector3DF f = lBodyCenter - lEpicenter;
		f.z += lExplosiveDig*2;	// Multiply by two, to end up above center, so we'll cause angular rotation.
		float d = f.GetLength();
		if (d > 80*pStrength)
		{
			continue;
		}
		d = 1/d;
		f *= d;
		d *= 8;	// Just so we have at least some cubic strength. This affects the blast radius. Don't change!
		d = d*d*d;
		d = std::min(1.0f, d);
		d *= pStrength;
		//mLog.Infof(_T("Explosion for %s with strength %f at (%f;%f;%f)."), pObject->GetClassId().c_str(), d, pPosition.x, pPosition.y, pPosition.z);
		lForce += d;

		if (pApplyForce)
		{
			const float lForceFactor = 430.0f;	// To be able to pass a sensible strength factor to this method.
			const float ff = lForceFactor * lGeometry->GetMass() * d;
			f *= ff;
			pPhysicsManager->AddForceAtPos(lGeometry->GetBodyId(), f, lEpicenter);
		}
	}
	lForce /= (lBoneCount-lBoneStart);
	return lForce;
}



LOG_CLASS_DEFINE(GAME, Explosion);



}
