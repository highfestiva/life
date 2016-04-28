
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "PushServerDelegate.h"
#include "../../Cure/Include/ConsoleManager.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/IntAttribute.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/Spawner.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Random.h"
#include "../../Life/LifeServer/GameServerManager.h"
#include "../RtVar.h"
#include "../RtVar.h"
#include "../Version.h"



namespace Push
{



PushServerDelegate::PushServerDelegate(Life::GameServerManager* pGameServerManager):
	Parent(pGameServerManager)
{
}

PushServerDelegate::~PushServerDelegate()
{
}



void PushServerDelegate::SetLevel(const str& pLevelName)
{
	ScopeLock lLock(mGameServerManager->GetTickLock());
	if (mLevelId)
	{
		mGameServerManager->DeleteContextObject(mLevelId);
	}

	Cure::ContextObject* lLevel = mGameServerManager->GameManager::CreateContextObject(pLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
	mLevelId = lLevel->GetInstanceId();
	lLevel->StartLoading();
}



Cure::ContextObject* PushServerDelegate::CreateContextObject(const str& pClassId) const
{
	return new Cure::CppContextObject(mGameServerManager->GetResourceManager(), pClassId);
}

void PushServerDelegate::OnOpen()
{
	SetLevel("level_01");
}

void PushServerDelegate::OnLogin(Life::Client*)
{
}

void PushServerDelegate::OnLogout(Life::Client*)
{
}



void PushServerDelegate::OnSelectAvatar(Life::Client* pClient, const Cure::UserAccount::AvatarId& pAvatarId)
{
	const Cure::GameObjectId lPreviousAvatarId = pClient->GetAvatarId();
	if (lPreviousAvatarId)
	{
		mLog.Info("User "+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+" had an avatar, replacing it.");
		pClient->SetAvatarId(0);
		Cure::ContextObject* lObject = mGameServerManager->GetContext()->GetObject(lPreviousAvatarId);
		if (lObject)
		{
			xform lTransform;
			lTransform.SetPosition(lObject->GetPosition());
			lTransform.GetPosition() += vec3(0, 0, 2);
			vec3 lEulerAngles;
			lObject->GetOrientation().GetEulerAngles(lEulerAngles);
			quat q;
			q.SetEulerAngles(lEulerAngles.x, 0, 0);
			lTransform.SetOrientation(q * lTransform.GetOrientation());
		}
		mGameServerManager->DeleteContextObject(lPreviousAvatarId);
	}

	Cure::Spawner* lSpawner = mGameServerManager->GetAvatarSpawner(mLevelId);
	if (!lSpawner)
	{
		mLog.Error("No player spawner in level!");
		return;
	}
	mLog.Info("Loading avatar '"+pAvatarId+"' for user "+strutil::Encode(pClient->GetUserConnection()->GetLoginName())+".");
	Cure::ContextObject* lObject = mGameServerManager->Parent::CreateContextObject(pAvatarId, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	lSpawner->PlaceObject(lObject, -1);
	pClient->SetAvatarId(lObject->GetInstanceId());
	lObject->SetExtraData((void*)(intptr_t)pClient->GetUserConnection()->GetAccountId());
	lObject->StartLoading();
}

void PushServerDelegate::OnLoadAvatar(Life::Client* pClient, Cure::ContextObject* pAvatar)
{
	(void)pClient;
	(void)pAvatar;
}

void PushServerDelegate::OnLoadObject(Cure::ContextObject* pObject)
{
	(void)pObject;
}

void PushServerDelegate::OnDeleteObject(Cure::ContextObject* pObject)
{
	(void)pObject;
}



bool PushServerDelegate::IsObjectLendable(Life::Client* pClient, Cure::ContextObject* pObject)
{
	(void)pClient;
	(void)pObject;
	return true;
}



void PushServerDelegate::PreEndTick()
{
}



loginstance(GAME, PushServerDelegate);



}
