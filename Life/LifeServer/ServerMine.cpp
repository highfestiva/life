
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "ServerMine.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/GameManager.h"
#include "../Launcher.h"
#include "../ProjectileUtil.h"



namespace Life
{



ServerMine::ServerMine(Cure::ResourceManager* pResourceManager, const str& pClassId, Launcher* pLauncher):
	Parent(pResourceManager, pClassId),
	mLauncher(pLauncher),
	mTicksTilFullyActivated(100),
	mTicksTilDetonation(-1),
	mIsDetonated(false)
{
	new Cure::FloatAttribute(this, _T("float_health"), 0.34f);
}

ServerMine::~ServerMine()
{
}



void ServerMine::OnTick()
{
	if (mTicksTilFullyActivated > 0)
	{
		--mTicksTilFullyActivated;
	}

	if (mTicksTilDetonation >= 0)
	{
		if (--mTicksTilDetonation == 0)
		{
			ProjectileUtil::Detonate(this, &mIsDetonated, mLauncher, GetPosition(), GetVelocity(), Vector3DF(), 1);
		}
		return;
	}

	const float lHealth = ((Cure::FloatAttribute*)GetAttribute(_T("float_health")))->GetValue();
	if (lHealth <= -0.5f)
	{
		mTicksTilDetonation = 1;
	}
	else if (lHealth <= -0.1f)
	{
		mTicksTilDetonation = 2;
	}
	else if (lHealth <= -0.05f)
	{
		mTicksTilDetonation = 6;
	}
	else if (lHealth <= 0)
	{
		mTicksTilDetonation = 12;
	}
}

void ServerMine::OnForceApplied(Cure::ContextObject* pOtherObject,
	TBC::PhysicsManager::BodyID pOwnBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque,
	const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	(void)pOtherObject;
	(void)pOwnBodyId;
	(void)pOtherBodyId;
	(void)pTorque;
	(void)pPosition;
	(void)pRelativeVelocity;

	float lForce = pForce.GetLength()/GetMass() * 0.002f;
	if (mTicksTilFullyActivated == 0 ||
		(pOtherObject->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC &&
		!dynamic_cast<ServerMine*>(pOtherObject)))
	{
		lForce *= 10;
	}
	if (lForce > 1)
	{
		Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)GetAttribute(_T("float_health"));
		const float lResult = std::max(0.0f, lHealth->GetValue() - lForce * 0.045f);
		lHealth->SetValue(lResult);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, ServerMine);



}
