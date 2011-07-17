
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Game.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../UiCure/Include/UiCppContextObject.h"
#include "../UiCure/Include/UiGameUiManager.h"



namespace GrenadeRun
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameTicker(),
	Cure::GameManager(Cure::GameTicker::GetTimeManager(), new Cure::RuntimeVariableScope(pVariableScope), pResourceManager),
	mUiManager(pUiManager)
{
}

Game::~Game()
{
	mUiManager = 0;
}

bool Game::Initialize()
{
	return true;
}



bool Game::Render()
{
	return true;
}



void Game::MoveTo(const FingerMovement& /*pMove*/)
{
}


bool Game::Tick()
{
	return true;
}

void Game::PollRoundTrip()
{
}

float Game::GetTickTimeReduction() const
{
	return 0;
}

float Game::GetPowerSaveAmount() const
{
	bool lIsMinimized = !mUiManager->GetDisplayManager()->IsVisible();
	return (lIsMinimized? 1.0f : 0);
}



void Game::OnLoadCompleted(Cure::ContextObject* /*pObject*/, bool /*pOk*/)
{
}

void Game::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	pForce;
	pTorque;
	pPosition;
	pObject1;
	pObject2;
	pBody1Id;
	pBody2Id;
}

bool Game::OnPhysicsSend(Cure::ContextObject* pObject)
{
	pObject;
	return true;
}

bool Game::OnAttributeSend(Cure::ContextObject* pObject)
{
	pObject;
	return true;
}

bool Game::IsServer()
{
	return true;
}

void Game::SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2)
{
	pObject1;
	pId1;
	pObject2;
	pId2;
}

void Game::SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	pObject1;
	pObject2;
}

void Game::TickInput()
{
}

Cure::ContextObject* Game::CreateContextObject(const str& pClassId) const
{
	pClassId;
	return 0;
}

bool Game::InitializeTerrain()
{
	bool lOk = true;
	if (lOk)
	{
		mLauncher = new UiCure::CppContextObject(GetResourceManager(), _T("launcher"), mUiManager);
		AddContextObject(mLauncher, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mLauncher != 0);
		assert(lOk);
		if (lOk)
		{
			mLauncher->StartLoading();
		}
	}
	return lOk;
}



}
