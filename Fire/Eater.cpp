
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "Eater.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "BaseMachine.h"



namespace Fire
{



Eater::Eater(Cure::ContextManager* pManager):
	Parent(pManager->GetGameManager()->GetResourceManager(), _T("Eater"))
{
	pManager->AddLocalObject(this);
}

Eater::~Eater()
{
}



void Eater::OnTrigger(Tbc::PhysicsManager::BodyID, ContextObject* pOtherObject, Tbc::PhysicsManager::BodyID pBodyId, const vec3&, const vec3&)
{
	if (pOtherObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId() == pBodyId)	// Only if whole car enters "goal", not only wheel.
	{
		BaseMachine* lMachine = dynamic_cast<BaseMachine*>(pOtherObject);
		if (lMachine && !lMachine->mDidGetToTown)
		{
			lMachine->mDidGetToTown = true;
			if (!lMachine->mVillain.empty() && Cure::Health::Get(lMachine) > 0)
			{
				((FireManager*)GetManager()->GetGameManager())->OnLetThroughTerrorist(lMachine);
			}
		}

		//GetManager()->PostKillObject(pOtherObject->GetInstanceId());
	}
}



loginstance(GAME_CONTEXT_CPP, Eater);




}
