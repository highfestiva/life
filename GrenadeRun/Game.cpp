
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Game.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiMachine.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "Grenade.h"



namespace GrenadeRun
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameTicker(),
	Cure::GameManager(Cure::GameTicker::GetTimeManager(), new Cure::RuntimeVariableScope(pVariableScope), pResourceManager),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mLevel(0),
	mVehicle(0),
	mLauncher(0),
	mTime(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"), UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f));
}

Game::~Game()
{
	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
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
		mVehicle = (UiCure::CppContextObject*)Parent::CreateContextObject(_T("mobile_crane_01"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
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
	mTime += GameTicker::GetTimeManager()->GetNormalFrameTime();
	Vector3DF lPosition;
	Vector3DF lVelocity;
	if (mVehicle)
	{
		lPosition = mVehicle->GetPosition()+Vector3DF(0, 0, 20);
		lVelocity = mVehicle->GetVelocity();
	}
	else
	{
		lPosition = mLauncher->GetPosition();
	}
	mCollisionSoundManager->Tick(lPosition);
	mUiManager->SetMicrophonePosition(TransformationF(QuaternionF(), lPosition), lVelocity);
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
		TransformationF t(QuaternionF(), mLauncher->GetPosition()+Vector3DF(0, 0, +5.0f));
		lGrenade->SetInitialTransform(t);
		lGrenade->Start();
		lGrenade->StartLoading();
	}
	return lOk;
}

void Game::Blast(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, Cure::ContextObject* pObject1)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, mLevel, pObject1->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), 10000);

	UiCure::Props* lPuff = new UiCure::Props(GetResourceManager(), _T("cloud_01"), mUiManager);
	AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	//mLog.Infof(_T("Machine %i creates fume particle %i."), GetInstanceId(), lPuff->GetInstanceId());
	lPuff->DisableRootShadow();
	TransformationF lTransform(QuaternionF(), pPosition);
	lPuff->SetInitialTransform(lTransform);
	lPuff->SetOpacity(0.3f);
	lPuff->StartParticle(UiCure::Props::PARTICLE_GAS, Vector3DF(), 0.3f);
	lPuff->StartLoading();
}

bool Game::Render()
{
	TransformationF t(QuaternionF(), Vector3DF(-100, -140, -10));
	t.GetOrientation().RotateAroundOwnZ(-PIF/8);
	if (mVehicle && mVehicle->IsLoaded())
	{
		t.GetPosition() = mVehicle->GetPosition() + Vector3DF(15*sin((float)mTime), -15*cos((float)mTime), 3);
		t.GetOrientation().RotateAroundOwnZ(mTime);
	}
	mUiManager->SetCameraPosition(t);
	const PixelRect lFullRect(0, 0, mUiManager->GetCanvas()->GetActualWidth(), mUiManager->GetCanvas()->GetActualHeight());
	PixelRect lLeftRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		lLeftRect.mBottom = lLeftRect.mBottom/2 - 5;
	}
	else
	{
		lLeftRect.mRight = lLeftRect.mRight/2 - 5;
	}
	mUiManager->Render(lLeftRect);

	t = TransformationF(QuaternionF(), Vector3DF(300, -300, 30));
	t.GetOrientation().RotateAroundOwnZ(PIF/4);
	mUiManager->SetCameraPosition(t);
	mLauncher->SetRootPosition(t.GetPosition()+Vector3DF(-10.5f, +10.5f, -6.5f));
	PixelRect lRightRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		lRightRect.mTop = lLeftRect.mBottom + 10;
	}
	else
	{
		lRightRect.mLeft = lLeftRect.mRight + 10;
	}
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
