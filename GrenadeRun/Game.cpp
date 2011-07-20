
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Game.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../UiCure/Include/UiMachine.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "Grenade.h"



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
	bool lOk = true;
	if (lOk)
	{
		lOk = InitializeTerrain();
	}
	if (lOk)
	{
		mVehicle = (UiCure::CppContextObject*)Parent::CreateContextObject(_T("monster_02"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mVehicle != 0);
		assert(lOk);
		if (lOk)
		{
			TransformationF t(QuaternionF(), Vector3DF(-100, -140, -39));
			mVehicle->SetInitialTransform(t);
			mVehicle->StartLoading();
		}
	}
	if (lOk)
	{
		mLauncher = new UiCure::Props(GetResourceManager(), _T("launcher"), mUiManager);
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

bool Game::Tick()
{
	GameTicker::GetTimeManager()->Tick();
	return true;
}



UiCure::CppContextObject* Game::GetP1()
{
	return mVehicle;
}

UiCure::CppContextObject* Game::GetP2()
{
	return mLauncher;
}

bool Game::Shoot()
{
	Grenade* lGrenade = (Grenade*)Parent::CreateContextObject(_T("grenade"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	bool lOk = (lGrenade != 0);
	assert(lOk);
	if (lOk)
	{
		TransformationF t(QuaternionF(), mLauncher->GetPosition()+Vector3DF(0, 0, +1.0f));
		lGrenade->SetInitialTransform(t);
		lGrenade->Start();
		lGrenade->StartLoading();
	}
	return lOk;
}


bool Game::Render()
{
	TransformationF t(QuaternionF(), Vector3DF(-100, -140, -10));
	t.GetOrientation().RotateAroundOwnZ(-PIF/8);
	mUiManager->SetCameraPosition(t);
	const PixelRect lFullRect(0, 0, mUiManager->GetCanvas()->GetActualWidth(), mUiManager->GetCanvas()->GetActualHeight());
	PixelRect lLeftRect = lFullRect;
	lLeftRect.mRight = lLeftRect.mRight/2 - 5;
	mUiManager->Render(lLeftRect);

	t = TransformationF(QuaternionF(), Vector3DF(300, -300, 30));
	t.GetOrientation().RotateAroundOwnZ(PIF/4);
	mUiManager->SetCameraPosition(t);
	mLauncher->SetRootPosition(t.GetPosition()+Vector3DF(-0.8f, +0.8f, -0.7f));
	PixelRect lRightRect = lFullRect;
	lRightRect.mLeft = lLeftRect.mRight + 10;
	mUiManager->Render(lRightRect);
	return true;
}



/*void Game::MoveTo(const FingerMovement& pMove)
{
}*/


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
	if (strutil::StartsWith(pClassId, _T("grenade")))
	{
		return new Grenade(GetResourceManager(), pClassId, mUiManager);
	}
	return new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
}

bool Game::InitializeTerrain()
{
	bool lOk = true;
	if (lOk)
	{
		mLevel = new UiCure::Machine(GetResourceManager(), _T("level_1"), mUiManager);
		AddContextObject(mLevel, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mLevel != 0);
		assert(lOk);
		if (lOk)
		{
			mLevel->DisableRootShadow();
			mLevel->StartLoading();
		}
	}
	return lOk;
}



Cure::ContextObject* Game::CreateLogicHandler(const str&) const
{
	return 0;
}



}
