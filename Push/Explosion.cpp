
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Explosion.h"
#include "../Cure/Include/ContextObject.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../TBC/Include/ChunkyPhysics.h"



namespace Push
{



float Explosion::PushObject(TBC::PhysicsManager* pPhysicsManager, const Cure::ContextObject* pObject, const Vector3DF& pPosition, float pStrength)
{
	TBC::ChunkyPhysics* lPhysics = pObject->ContextObject::GetPhysics();
	if (!lPhysics)
	{
		return 0;
	}
	// Dynamics only get hit in the main body, while statics gets all their dynamic sub-bodies hit.
	const Vector3DF lEpicenter = pPosition + Vector3DF(0, 0, -0.75f);
	const int lBoneStart = (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)? 0 : 1;
	const int lBoneCount = (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)? 1 : lPhysics->GetBoneCount();
	float lForce = 0;
	for (int y = lBoneStart; y < lBoneCount; ++y)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(y);
		if (lGeometry->GetBoneType() != TBC::ChunkyBoneGeometry::BONE_BODY
			|| (y != 0 && lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_EXCLUDE))
		{
			continue;
		}
		const Vector3DF lBodyCenter = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
		Vector3DF f = lBodyCenter - lEpicenter;
		float d = f.GetLength();
		if (d > 80*pStrength)
		{
			continue;
		}
		d = 1/d;
		f *= d;
		d *= 8;	// Just so we have at least some cubic strength.
		d = d*d*d;
		d = std::min(1.0f, d);
		d *= pStrength;
		mLog.Infof(_T("Explosion for %s with strength %f at (%f;%f;%f)."), pObject->GetClassId().c_str(), d, pPosition.x, pPosition.y, pPosition.z);
		lForce += d;
		const float lForceFactor = 3000.0f;	// To be able to pass a sensible strength factor to this method.
		const float ff = lForceFactor * pObject->GetMass() * d;
		if (f.z <= 0.1f)
		{
			f.z += 0.3f;
		}
		f *= ff;
		pPhysicsManager->AddForce(lGeometry->GetBodyId(), f);
	}
	return lForce;
}



LOG_CLASS_DEFINE(GAME, Explosion);



}
