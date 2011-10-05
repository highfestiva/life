
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "Game.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "../TBC/Include/PhysicsEngine.h"
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
	mLightId(UiTbc::Renderer::INVALID_LIGHT),
	mLevel(0),
	mVehicle(0),
	mLauncher(0),
	mIsLaunching(false),
	mLauncherYaw(0),
	mLauncherPitch(-PIF/4),
	mTime(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"), UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f));
}

Game::~Game()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);

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
		mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(-1, 0.5f, -1.5),
			Color::Color(255, 255, 255), 2.5f, 300);
		mUiManager->GetRenderer()->EnableAllLights(true);
	}
	if (lOk)
	{
		mVehicle = (UiCure::CppContextObject*)Parent::CreateContextObject(_T("cutie"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mVehicle != 0);
		assert(lOk);
		if (lOk)
		{
			TransformationF t(QuaternionF(), Vector3DF(-173, -85, 7));
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
			mLauncher->DisableRootShadow();
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
		lPosition = mVehicle->GetPosition()+Vector3DF(0, 0, 2);
		lVelocity = mVehicle->GetVelocity();
	}
	else
	{
		lPosition = mLauncher->GetPosition();
	}
	mCollisionSoundManager->Tick(lPosition);
	mUiManager->SetMicrophonePosition(TransformationF(QuaternionF(), lPosition), lVelocity);

	if (mLauncher && mLauncher->IsLoaded())
	{
		QuaternionF lQuaternion;
		mLauncherYaw -= mLauncher->ContextObject::GetPhysics()->GetEngine(1)->GetLerpThrottle(0.2f, 0.2f) * 0.01f;
		mLauncherPitch -= mLauncher->ContextObject::GetPhysics()->GetEngine(0)->GetLerpThrottle(0.2f, 0.2f) * 0.01f;
		mLauncherYaw = Math::Clamp(mLauncherYaw, -PIF/2, PIF/2);
		mLauncherPitch = Math::Clamp(mLauncherPitch, -PIF/2*2/3, -PIF/90);
		lQuaternion.RotateAroundWorldZ(mLauncherYaw);
		lQuaternion.RotateAroundOwnX(mLauncherPitch);
		mLauncher->SetRootOrientation(lQuaternion);
	}

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

void Game::GetVehicleMotion(Vector3DF& pPosition, Vector3DF pVelocity) const
{
	if (mVehicle && mVehicle->IsLoaded())
	{
		pPosition = mVehicle->GetPosition();
		pVelocity = mVehicle->GetVelocity();
	}
}

void Game::GetLauncherTransform(TransformationF& pTransform) const
{
	if (mLauncher && mLauncher->IsLoaded())
	{
		pTransform.SetPosition(mLauncher->GetPosition());
		pTransform.SetOrientation(mLauncher->GetOrientation());
	}
}

bool Game::Shoot()
{
	if (mIsLaunching)
	{
		return false;
	}

	Grenade* lGrenade = (Grenade*)Parent::CreateContextObject(_T("grenade"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	bool lOk = (lGrenade != 0);
	assert(lOk);
	if (lOk)
	{
		TransformationF t(mLauncher->GetOrientation(), mLauncher->GetPosition()+Vector3DF(0, 0, +5.0f));
		lGrenade->SetInitialTransform(t);
		lGrenade->StartLoading();
		mIsLaunching = true;
	}
	return lOk;
}

void Game::UnlockLauncher()
{
	mIsLaunching = false;
}

void Game::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, Cure::ContextObject* pObject1)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, mLevel, pObject1->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), 10000);

	{
		const int lParticleCount = 200;
		for (int i = 0; i < lParticleCount; ++i)
		{
			UiCure::Props* lPuff = new UiCure::Props(GetResourceManager(), _T("mud_particle_01"), mUiManager);
			AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
			lPuff->DisableRootShadow();
			float x = (float)Random::Uniform(-1, 1);
			float y = (float)Random::Uniform(-1, 1);
			float z = 0;
			TransformationF lTransform(QuaternionF(), pPosition + Vector3DF(x, y, z));
			lPuff->SetInitialTransform(lTransform);
			const float lOpacity = (float)Random::Uniform(0.3f, 2.2f);
			lPuff->SetOpacity(lOpacity);
			const float lAngle = (float)Random::Uniform(0, 2*PIF);
			x = (20.0f * i/lParticleCount - 10) * cos(lAngle);
			y = (10 * (float)Random::Uniform(-1, 1)) * sin(lAngle);
			z = (30 + 12 * sin(5*PIF*i/lParticleCount) * (float)Random::Uniform(0.0, 1)) * (float)Random::Uniform(0.2f, 1.0f);
			lPuff->StartParticle(UiCure::Props::PARTICLE_SOLID, Vector3DF(x, y, z), 3.0f / lOpacity, 0.5f, 6.0f);
			lPuff->StartLoading();
		}
	}

	{
		const int lParticleCount = 10;
		for (int i = 0; i < lParticleCount; ++i)
		{
			UiCure::Props* lPuff = new UiCure::Props(GetResourceManager(), _T("cloud_01"), mUiManager);
			AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
			lPuff->DisableRootShadow();
			float x = (float)Random::Uniform(-1, 1);
			float y = (float)Random::Uniform(-1, 1);
			float z = (float)Random::Uniform(-1, 1);
			TransformationF lTransform(QuaternionF(), pPosition + Vector3DF(x, y, z));
			lPuff->SetInitialTransform(lTransform);
			const float lOpacity = (float)Random::Uniform(0.025f, 0.1f);
			lPuff->SetOpacity(lOpacity);
			x = x*17;
			y = y*17;
			z = (float)Random::Uniform(0, 10);
			lPuff->StartParticle(UiCure::Props::PARTICLE_GAS, Vector3DF(x, y, z), 0.002f / lOpacity, 0.1f, 5.0f);
			lPuff->StartLoading();
		}
	}

	if (mVehicle && mVehicle->IsLoaded())
	{
		Vector3DF v = mVehicle->GetPosition() - pPosition;
		float d = v.GetLength();
		v /= d;
		d = (d < 0.5f)? 0.5f : d;
		d *= 0.08f;
		const float lMaxForceFactor = 2000.0f;
		const float f = std::min(lMaxForceFactor, lMaxForceFactor / ::pow(d, 3.0f)) * mVehicle->GetMass();
		v *= f;
		v.z = f/5;
		const TBC::ChunkyBoneGeometry* lGeometry = mVehicle->ContextObject::GetPhysics()->GetBoneGeometry(0);
		GetPhysicsManager()->AddForce(lGeometry->GetBodyId(), v);
	}
}

bool Game::Render()
{
	TransformationF t(QuaternionF(), Vector3DF(-100, -140, -10));
	t.GetOrientation().RotateAroundOwnZ(-PIF/8);
	if (mVehicle && mVehicle->IsLoaded())
	{
		t.GetPosition() = mVehicle->GetPosition() + Vector3DF(15*sin((float)mTime), -15*cos((float)mTime), 3);
		t.GetOrientation().RotateAroundOwnZ((float)mTime);
#ifdef LEPRA_IOS
		t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
#endif // iOS
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
	mUiManager->GetRenderer()->SetViewFrustum(60, 3, 1000);
	mUiManager->Render(lLeftRect);

	const Vector3DF lLauncherPosition(0, -215, 27);

	float lRange = 150.0f;
	if (mVehicle && mVehicle->IsLoaded())
	{
		lRange = lLauncherPosition.GetDistance(mVehicle->GetPosition());
	}

	const float lCamDistance = 10;
	const Vector3DF lCamOffset(lCamDistance*sin(mLauncherYaw), -lCamDistance*cos(mLauncherYaw), 9.0f - lRange/100.0f);
	t = TransformationF(QuaternionF(), lLauncherPosition + lCamOffset);
	t.GetOrientation().RotateAroundOwnZ(mLauncherYaw*0.8f);
#ifdef LEPRA_IOS
	t.GetOrientation().RotateAroundOwnY(PIF*0.5f);
#endif // iOS
	mUiManager->SetCameraPosition(t);
	mLauncher->SetRootPosition(lLauncherPosition);
	PixelRect lRightRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		lRightRect.mTop = lLeftRect.mBottom + 10;
	}
	else
	{
		lRightRect.mLeft = lLeftRect.mRight + 10;
	}
	mUiManager->GetRenderer()->SetViewFrustum(std::min(60.0f, 9000/lRange), 3, 1000);
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



void Game::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk && pObject == mVehicle)
	{
		const str lName = _T("float_is_child");
		new Cure::FloatAttribute(mVehicle, lName, 1);
	}
	if (pOk && pObject == mLauncher)
	{
		// Create a mock engine on the launcher that we use to navigate.
		TBC::PhysicsEngine* lPitchEngine = new TBC::PhysicsEngine(TBC::PhysicsEngine::ENGINE_TILTER, 1, 1, 1, 1, 0);
		pObject->GetPhysics()->AddEngine(lPitchEngine);
		TBC::PhysicsEngine* lYawEngine = new TBC::PhysicsEngine(TBC::PhysicsEngine::ENGINE_HINGE_ROLL, 1, 1, 1, 1, 1);
		pObject->GetPhysics()->AddEngine(lYawEngine);
	}
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
		float lMuzzleVelocity = 50.0;
		if (mLauncher && mLauncher->IsLoaded() && mVehicle && mVehicle->IsLoaded())
		{
			lMuzzleVelocity = ::pow(mLauncher->GetPosition().GetDistance(mVehicle->GetPosition()), 0.5f) * 3.5f;
		}
		return new Grenade(GetResourceManager(), pClassId, mUiManager, lMuzzleVelocity);
	}
	return new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
}

bool Game::InitializeTerrain()
{
	bool lOk = true;
	if (lOk)
	{
		mLevel = new UiCure::Machine(GetResourceManager(), _T("level_2"), mUiManager);
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
