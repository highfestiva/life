
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
#include "Goal.h"
#include "Vehicle.h"
#include "VehicleElevator.h"
#include "Level.h"
#include "Spawner.h"
#include "VehicleAi.h"



#define VEHICLE_START	Vector3DF(-90, 0, 13)
#define CAM_OFFSET	Vector3DF(+2, -10, +3)



namespace TireFire
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameTicker(),
	Cure::GameManager(Cure::GameTicker::GetTimeManager(), pVariableScope, pResourceManager, 400, 4, 3),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mLightId(UiTbc::Renderer::INVALID_LIGHT),
	mLevel(0),
	mFlybyMode(FLYBY_INACTIVE),
	mFlyByTime(0),
	mVehicle(0),
	mVehicleCamPos(-80, -40, 3),
	mGoal(0),
	mVehicleAi(0),
	mFlipRenderSide(0),
	mFlipRenderSideFactor(0),
	mScore(0),
	mScoreCountingEnabled(false)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.1f, 0.2f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(0.4f, 0.1f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
}

Game::~Game()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
	mUiManager = 0;
	SetVariableScope(0);	// Not owned by us.

	mGoal = 0;
	mVehicleAi = 0;
}

UiCure::GameUiManager* Game::GetUiManager() const
{
	return mUiManager;
}

const str& Game::GetLevelName() const
{
	return mLevelName;
}

bool Game::SetLevelName(const str& pLevel)
{
	mLevelName = pLevel;
	if (!mVehicle)
	{
		return RestartLevel();
	}
	mGoal = 0;
	return InitializeTerrain();
}

bool Game::RestartLevel()
{
	return Initialize();
}

TransformationF Game::GetVehicleStart() const
{
	TransformationF t(gIdentityQuaternionF, VEHICLE_START);
	t.GetOrientation().RotateAroundOwnZ(-PIF/2);
	return t;
}

bool Game::Tick()
{
	if (!mLevel || !mLevel->IsLoaded())
	{
		return true;
	}

	if (mSlowmoTimer.IsStarted() && mSlowmoTimer.QueryTimeDiff() > 4.0f)
	{
		mSlowmoTimer.Stop();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0);
	}

	GameTicker::GetTimeManager()->Tick();

	mFlipRenderSideFactor = Math::Lerp(mFlipRenderSideFactor, (float)mFlipRenderSide, 0.15f);
	if (::fabs(mFlipRenderSideFactor-(float)mFlipRenderSide) < 0.001f)
	{
		mFlipRenderSideFactor = (float)mFlipRenderSide;
	}

	Vector3DF lPosition;
	Vector3DF lVelocity;
	if (mVehicle && mVehicle->IsLoaded())
	{
		lPosition = mVehicle->GetPosition()+Vector3DF(0, 0, +10);
		if (lPosition.z < -50 || lPosition.z > 100 /*|| isnan(lPosition.z) || isinf(lPosition.z)*/)
		{
			const float lHealth = mVehicle->GetHealth();
			const str lVehicleType = mVehicle->GetClassId();	// TRICKY: don't fetch reference!!!
			SetVehicleName(lVehicleType);
			mVehicle->DrainHealth(mVehicle->GetHealth() - lHealth);
		}
		else
		{
			lVelocity = mVehicle->GetVelocity();
			mVehicle->QueryFlip();
		}
	}
	mCollisionSoundManager->Tick(lPosition);
	mUiManager->SetMicrophonePosition(TransformationF(gIdentityQuaternionF, lPosition), lVelocity);

	/*if (win!)
	{
		AddContextObject(new UiCure::Sound(GetResourceManager(), _T("win.wav"), mUiManager), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	}
	mPreviousFrameWinnerIndex = mWinnerIndex;*/

	return true;
}

void Game::TickFlyby()
{
	const double lFrameTime = 1.0/FPS;
	mFlyByTime += lFrameTime;
}



str Game::GetVehicleName() const
{
	if (mVehicle)
	{
		return mVehicle->GetClassId();
	}
	return str();
}

void Game::SetVehicleName(const str& pVehicle)
{
	if (mVehicle && mVehicle->IsLoaded() &&
		mVehicle->GetPosition().GetDistance(GetVehicleStart().GetPosition()) < 2.0f*SCALE_FACTOR &&
		mVehicle->GetClassId() == pVehicle &&
		mVehicle->GetHealth() > 0)
	{
		mVehicle->DrainHealth(-1);
		return;
	}
	delete mVehicle;
	mVehicle = (Vehicle*)Parent::CreateContextObject(pVehicle, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	bool lOk = (mVehicle != 0);
	assert(lOk);
	if (lOk)
	{
		mVehicle->SetInitialTransform(GetVehicleStart());
		mVehicle->DrainHealth(-1);
		mVehicle->StartLoading();
	}
}

Level* Game::GetLevel() const
{
	return mLevel;
}

Vehicle* Game::GetVehicle() const
{
	return mVehicle;
}

Goal* Game::GetGoal() const
{
	return mGoal;
}

void Game::GetVehicleMotion(Vector3DF& pPosition, Vector3DF pVelocity) const
{
	if (mVehicle && mVehicle->IsLoaded())
	{
		pPosition = mVehicle->GetPosition();
		pVelocity = mVehicle->GetVelocity();
	}
}

void Game::SetThrottle(float pThrottle)
{
	if (pThrottle < 0 && mVehicle->GetForwardSpeed() > 0.5f)
	{
		mVehicle->SetEnginePower(0, 0, 0);	// Disengage throttle while braking.
		mVehicle->SetEnginePower(2, -pThrottle, 0);
		return;
	}
	mVehicle->SetEnginePower(2, 0, 0);	// Disengage brakes.
	mVehicle->SetEnginePower(0, pThrottle, 0);
}

Game::FlybyMode Game::GetFlybyMode() const
{
	if (!mVehicle || !mVehicle->IsLoaded())
	{
		return FLYBY_PAUSE;
	}
	return mFlybyMode;
}

void Game::SetFlybyMode(FlybyMode pFlybyMode)
{
	mFlybyMode = pFlybyMode;
	if (pFlybyMode == FLYBY_INTRODUCTION)
	{
		mFlyByTime = 0;
	}
}

void Game::ResetScore()
{
	mScore = 0;
}

void Game::AddScore(double pScore)
{
	if (!IsScoreCountingEnabled())
	{
		return;
	}
	mScore += pScore;
}

double Game::GetScore() const
{
	return mScore;
}

void Game::EnableScoreCounting(bool pEnable)
{
	mScoreCountingEnabled = pEnable;
}

bool Game::IsScoreCountingEnabled() const
{
	return mScoreCountingEnabled;
}

void Game::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pExplosive, Cure::ContextObject* pTarget, TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pTargetBodyId)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pExplosive, mLevel, pExplosiveBodyId, 10000, true);

	if (pTarget == mLevel)
	{
		// Stones and mud. More if hit ground, less otherwise.
		const float lScale = SCALE_FACTOR * 320 / mUiManager->GetCanvas()->GetWidth();
		const int lParticleCount = (mLevel->GetStructureGeometry((unsigned)0)->GetBodyId() == pTargetBodyId)? Random::GetRandomNumber()%50+50 : 10;
		for (int i = 0; i < lParticleCount; ++i)
		{
			UiCure::Props* lPuff = new UiCure::Props(GetResourceManager(), _T("mud_particle_01"), mUiManager);
			AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
			lPuff->DisableRootShadow();
			float x = (float)Random::Uniform(-1, 1);
			float y = (float)Random::Uniform(-1, 1);
			float z = -1;
			TransformationF lTransform(gIdentityQuaternionF, pPosition + Vector3DF(x, y, z));
			lPuff->SetInitialTransform(lTransform);
			const float lAngle = (float)Random::Uniform(0, 2*PIF);
			x = (14.0f * i/lParticleCount - 10) * cos(lAngle);
			y = (6 * (float)Random::Uniform(-1, 1)) * sin(lAngle);
			z = (17 + 8 * sin(5*PIF*i/lParticleCount) * (float)Random::Uniform(0.0, 1)) * (float)Random::Uniform(0.2f, 1.0f);
			lPuff->StartParticle(UiCure::Props::PARTICLE_SOLID, Vector3DF(x, y, z), (float)Random::Uniform(3, 7) * lScale, 0.5f, (float)Random::Uniform(3, 7));
#ifdef LEPRA_TOUCH_LOOKANDFEEL
			lPuff->SetFadeOutTime(0.3f);
#endif // Touch L&F
			lPuff->StartLoading();
		}
	}

	{
		// Release gas puffs.
		const int lParticleCount = (Random::GetRandomNumber() % 4) + 2;
		for (int i = 0; i < lParticleCount; ++i)
		{
			UiCure::Props* lPuff = new UiCure::Props(GetResourceManager(), _T("cloud_01"), mUiManager);
			AddContextObject(lPuff, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
			lPuff->DisableRootShadow();
			float x = (float)Random::Uniform(-1, 1);
			float y = (float)Random::Uniform(-1, 1);
			float z = (float)Random::Uniform(-1, 1);
			TransformationF lTransform(gIdentityQuaternionF, pPosition + Vector3DF(x, y, z));
			lPuff->SetInitialTransform(lTransform);
			const float lOpacity = (float)Random::Uniform(0.025f, 0.1f);
			lPuff->SetOpacity(lOpacity);
			x = x*12;
			y = y*12;
			z = (float)Random::Uniform(0, 7);
			lPuff->StartParticle(UiCure::Props::PARTICLE_GAS, Vector3DF(x, y, z), 0.003f / lOpacity, 0.1f, (float)Random::Uniform(1.5, 4));
			lPuff->StartLoading();
		}
	}

	float lLevelShootEasyness = 4.5f;
	if (mLevelName == _T("level_elevate"))	// Bigger and open level = easier to hit Vehicle.
	{
		lLevelShootEasyness = 3.2f;
	}
	bool lDidHitVehicle = false;
	Cure::ContextManager::ContextObjectTable lObjectTable = GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		const Cure::ContextObject* lObject = x->second;
		TBC::ChunkyPhysics* lPhysics = lObject->ContextObject::GetPhysics();
		if (!lObject->IsLoaded() || !lPhysics)
		{
			continue;
		}
		// Dynamics only get hit in the main body, while statics gets all their dynamic sub-bodies hit.
		const Vector3DF lEpicenter = pPosition + Vector3DF(0, 0, -0.75f);
		const int lBoneCount = (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)? 1 : lPhysics->GetBoneCount();
		for (int x = 0; x < lBoneCount; ++x)
		{
			const TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(x);
			if (lGeometry->GetBodyId() == TBC::INVALID_BODY)
			{
				continue;
			}
			const Vector3DF lBodyCenter = GetPhysicsManager()->GetBodyPosition(lGeometry->GetBodyId());
			Vector3DF f = lBodyCenter - lEpicenter;
			float d = f.GetLength();
			if (d > 80*SCALE_FACTOR ||
				(d > 50*SCALE_FACTOR && lObject != mVehicle))
			{
				continue;
			}
			float lDistance = d;
			d = 1/d;
			f *= d;
			d *= lLevelShootEasyness;
			double lScore = d;
			d = d*d*d;
			d = std::min(1.0f, d);
			const float lMaxForceFactor = 800.0f;
			const float ff = lMaxForceFactor * lObject->GetMass() * d;
			if (f.z <= 0.1f)
			{
				f.z += 0.3f;
			}
			f *= ff;
			GetPhysicsManager()->AddForce(lGeometry->GetBodyId(), f);
			if (lObject == mVehicle)
			{
				if (d > 0.6f)
				{
					CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 0.2);
					mSlowmoTimer.Start();
				}
				d = std::max(0.005f, d);
				mVehicle->DrainHealth(d);
				lScore *= 63;
				lScore = std::min(20000.0, lScore*lScore);
				if (lDistance < 25*SCALE_FACTOR)
				{
					lDidHitVehicle = true;
				}
				else
				{
					lScore = 0;
				}
				if (mVehicle->GetHealth() <= 0)
				{
					AddScore(-19000);
				}
				else
				{
					AddScore(-lScore);
				}
			}
		}
	}
	if (!lDidHitVehicle)
	{
		AddScore(+2000);
	}
}

void Game::OnCapture()
{
	AddScore(+10000);
}

void Game::FlipRenderSides()
{
	mFlipRenderSide = !mFlipRenderSide;
}

bool Game::IsFlipRenderSide() const
{
	return !!mFlipRenderSide;
}

void Game::EndSlowmo()
{
	mSlowmoTimer.ReduceTimeDiff(-20);
}



bool Game::Render()
{
	if (!mVehicle || !mVehicle->IsLoaded() ||
		!mLevel || !mLevel->IsLoaded() ||
		!mGoal)
	{
		return true;
	}

	const PixelRect lFullRect(0, 0, mUiManager->GetCanvas()->GetActualWidth(), mUiManager->GetCanvas()->GetActualHeight());

	if (mFlybyMode != FLYBY_INACTIVE)
	{
		return FlybyRender();
	}

	const Vector3DF lVehiclePos = mVehicle->GetPosition();
	const Vector3DF lTargetPos = Vector3DF(lVehiclePos.x, lVehiclePos.y, lVehiclePos.z) + CAM_OFFSET;
	mVehicleCamPos = Math::Lerp(mVehicleCamPos, lTargetPos, 0.5f);
	TransformationF t(gIdentityQuaternionF, mVehicleCamPos);
	const float lVehicleCamHeight = mVehicleCamPos.z - lVehiclePos.z;
	const float lCamDistance = mVehicleCamPos.GetDistance(lVehiclePos);
	t.GetOrientation().RotateAroundOwnX(-::atan(lVehicleCamHeight/lCamDistance));
/*#ifdef LEPRA_TOUCH_LOOKANDFEEL
	t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
	if (mFlipRenderSideFactor)
	{
		t.GetOrientation().RotateAroundOwnY(PIF*mFlipRenderSideFactor);
	}
#endif // Touch*/
	mUiManager->SetCameraPosition(t);
	mUiManager->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
	mUiManager->Render(lFullRect);
	mUiManager->GetRenderer()->ResetAmbientLight(true);
	return true;
}

bool Game::Paint()
{
	return true;
}



bool Game::FlybyRender()
{
	const Vector3DF lVehicle = mVehicle->GetPosition();
	const Vector3DF lGoal = mGoal->GetPosition();
	const double lTotalFlybyTime = 37.0;
	const double lTotalIntroductionTime = 75.0;
	if (mFlybyMode == FLYBY_INTRODUCTION)
	{
		if (mFlyByTime > lTotalFlybyTime)
		{
			mFlybyMode = FLYBY_INTRODUCTION_FINISHING_UP;
			return true;
		}
	}
	else if (mFlybyMode == FLYBY_INTRODUCTION_FINISHING_UP)
	{
		if (mFlyByTime > lTotalIntroductionTime)
		{
			mFlybyMode = FLYBY_INACTIVE;
			return true;
		}
	}

	TransformationF t;
	const double lSweepTime = lTotalFlybyTime * 0.25;
	const float lDistance = 100 * SCALE_FACTOR;
	if (mFlyByTime < lSweepTime || mFlybyMode == FLYBY_PAUSE)
	{
		// Sweep around the area in a circle.
		const float a = 0.8f * 2*PIF * (float)(mFlyByTime/lSweepTime);
		t.GetOrientation().RotateAroundOwnZ(a + PIF/2);
		t.GetOrientation().RotateAroundOwnX(-PIF/8);
		t.SetPosition(Vector3DF(::cos(a)*lDistance, ::sin(a)*lDistance, ::sin(a+PIF/8)*lDistance*0.1f + lDistance/3.5f));
	}
	else
	{
		// Look at level in more detail.
		const double lDetailTime = lTotalFlybyTime - lSweepTime;
		// Orientation. Treat orientation and position in different time slices, because if
		// both happen at the same time, perception of space is without a doubt lost.
		if (mFlyByTime-lSweepTime < lDetailTime * 1/12)
		{
			// Stare right at Vehicle.
			t.GetOrientation().RotateAroundOwnZ(+PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 3/12)
		{
			// Stand beside Vehicle.
			t.GetOrientation().RotateAroundOwnZ(+PIF*11/12);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 4/12)
		{
			// Look up at the goal.
			t.GetOrientation().RotateAroundOwnZ(-PIF*2/5);
			t.GetOrientation().RotateAroundOwnX(+PIF/12);
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 7/12)
		{
			// Look down at the goal.
			t.GetOrientation().RotateAroundOwnZ(-PIF*2/5);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		}
		// Position.
		if (mFlyByTime-lSweepTime < lDetailTime * 1/3)
		{
			t.SetPosition(lVehicle + Vector3DF(+1.33f, +6.67f, +3.33f));
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 2/3)
		{
			t.SetPosition(lGoal + Vector3DF(-13.33f, -10, +10));
		}
	}
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
#endif // Touch
	//mLeftCamera.Interpolate(mLeftCamera, t, 0.05f);
	//mUiManager->SetCameraPosition(mLeftCamera);
	mUiManager->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
	//mUiManager->Render(mLeftRect);
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
		assert(pObject->GetPhysics()->GetEngineCount() == 3);
		const str lName = _T("float_childishness");
		new Cure::FloatAttribute(mVehicle, lName, 0.67f);
	}
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
	if (strutil::StartsWith(pClassId, _T("monster_01")))
	{
		return new Vehicle(GetResourceManager(), pClassId, mUiManager);
	}
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
	if (lOk)
	{
		SetVehicleName(_T("monster_01"));
	}
	if (lOk)
	{
		SetFlybyMode(FLYBY_INACTIVE);
	}
	return lOk;
}

bool Game::InitializeTerrain()
{
	bool lOk = true;
	if (lOk)
	{
		delete mLevel;
		mLevel = new Level(GetResourceManager(), mLevelName, mUiManager);
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
	if (pType == _T("spawner"))
	{
		return new Spawner(GetContext());
	}
	else if (pType == _T("trig_goal"))
	{
		mGoal = new Goal(GetContext());
		return mGoal;
	}
	else if (pType == _T("trig_elevator"))
	{
		return new VehicleElevator(this);
	}
	else if (pType == _T("context_path"))
	{
		return mLevel->QueryPath();
	}
	return (0);
}



}
