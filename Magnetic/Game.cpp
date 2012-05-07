
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#include "Game.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"
#include "../UiCure/Include/UiSound.h"
#include "Level.h"



#define GRENADE_RELAUNCH_DELAY	2.7f
#define CUTIE_START		Vector3DF(-57.67f, -28.33f, 2.33f)



namespace Magnetic
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameTicker(),
	Cure::GameManager(Cure::GameTicker::GetTimeManager(), pVariableScope, pResourceManager, 400, 4, 3),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mLightId(UiTbc::Renderer::INVALID_LIGHT),
	mLevel(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"), UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f));

	Initialize();
}

Game::~Game()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
	mUiManager = 0;
	SetVariableScope(0);	// Not owned by us.
}

UiCure::GameUiManager* Game::GetUiManager() const
{
	return mUiManager;
}

bool Game::Tick()
{
	if (!mLevel || !mLevel->IsLoaded())
	{
		return true;
	}

	GameTicker::GetTimeManager()->Tick();

	Vector3DF lPosition;
	Vector3DF lVelocity;
	mCollisionSoundManager->Tick(lPosition);
	mUiManager->SetMicrophonePosition(TransformationF(gIdentityQuaternionF, lPosition), lVelocity);

	return true;
}

bool Game::Render()
{
	mUiManager->SetCameraPosition(TransformationF(QuaternionF(), Vector3DF(0, -30, 0)));
	const PixelRect lFullRect(0, 0, mUiManager->GetCanvas()->GetActualWidth(), mUiManager->GetCanvas()->GetActualHeight());
	mUiManager->Render(lFullRect);
	return true;
}

bool Game::Paint()
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



void Game::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	(void)pObject;
	(void)pOk;
}

void Game::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 2000, false);
}

bool Game::OnPhysicsSend(Cure::ContextObject* pObject)
{
	(void)pObject;
	return true;
}

bool Game::OnAttributeSend(Cure::ContextObject* pObject)
{
	(void)pObject;
	return true;
}

bool Game::IsServer()
{
	return true;
}

void Game::SendAttach(Cure::ContextObject* pObject1, unsigned pId1, Cure::ContextObject* pObject2, unsigned pId2)
{
	(void)pObject1;
	(void)pId1;
	(void)pObject2;
	(void)pId2;
}

void Game::SendDetach(Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	(void)pObject1;
	(void)pObject2;
}

void Game::TickInput()
{
}

Cure::ContextObject* Game::CreateContextObject(const str& pClassId) const
{
	return new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
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
		const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
		mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(-1, 0.5f, -1.5),
			Color::Color(255, 255, 255), lPixelShadersEnabled? 1.0f : 1.5f, 300);
		mUiManager->GetRenderer()->EnableAllLights(true);
	}
	return lOk;
}

bool Game::InitializeTerrain()
{
	bool lOk = true;
	if (lOk)
	{
		delete mLevel;
		mLevel = new Level(GetResourceManager(), _T("magnetic"), mUiManager);
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



Cure::ContextObject* Game::CreateLogicHandler(const str& pType)
{
	(void)pType;
	return 0;
}



}
