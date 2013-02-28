
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
#include "../UiCure/Include/UiExhaustEmitter.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiRuntimeVariableName.h"
#include "../UiCure/Include/UiSound.h"
#include "Ctf.h"
#include "Cutie.h"
#include "CutieElevator.h"
#include "Grenade.h"
#include "Launcher.h"
#include "LauncherAi.h"
#include "Level.h"
#include "RoboBall.h"
#include "SeeThrough.h"
#include "Spawner.h"
#include "VehicleAi.h"



#define GRENADE_RELAUNCH_DELAY	2.7f
#define CUTIE_START		Vector3DF(-57.67f, -28.33f, 2.33f)



namespace GrenadeRun
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	GameTicker(400, 4, 3),
	GameManager(GameTicker::GetTimeManager(), pVariableScope, pResourceManager),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mLightId(UiTbc::Renderer::INVALID_LIGHT),
	mLevel(0),
	mFlybyMode(FLYBY_INACTIVE),
	mFlyByTime(0),
	mVehicle(0),
	mVehicleCamPos(0, 0, 200),
	mVehicleCamHeight(15),
	mIsLauncherBarrelFree(true),
	mLauncherYaw(0),
	mLauncherPitch(-PIF/4),
	mWinnerIndex(-1),
	mPreviousFrameWinnerIndex(-1),
	mCtf(0),
	mVehicleAi(0),
	mLauncher(0),
	mLauncherAi(0),
	mComputerIndex(-1),
	mComputerDifficulty(0.5f),
	mHeartBalance(0),
	mAllowWin(false),
	mFlipRenderSide(0),
	mFlipRenderSideFactor(0),
	mScore(0),
	mScoreCountingEnabled(false),
	mRoundIndex(0)
{
	SetTicker(this);

	mPreviousCanvasAngle = mUiManager->GetCanvas()->GetOutputRotation();

	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
}

Game::~Game()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
	mUiManager = 0;
	SetVariableScope(0);	// Not owned by us.

	mCtf = 0;
	mVehicleAi = 0;
	mLauncher = 0;
	mLauncherAi = 0;
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
	mCtf = 0;
	return InitializeUniverse();
}

bool Game::RestartLevel()
{
	return Initialize();
}

TransformationF Game::GetCutieStart() const
{
	TransformationF t(gIdentityQuaternionF, CUTIE_START);
	t.GetOrientation().RotateAroundOwnZ(-PIF*0.45f);
	return t;
}

bool Game::Tick()
{
	if (!mLevel || !mLevel->IsLoaded())
	{
		return true;
	}

	if (mLaucherLockWatch.IsStarted())
	{
		mLaucherLockWatch.UpdateTimer();
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
		lPosition = mVehicle->GetPosition()+Vector3DF(0, 0, -1);
		if (lPosition.z < -50 || lPosition.z > 100 /*|| isnan(lPosition.z) || isinf(lPosition.z)*/)
		{
			const float lHealth = mVehicle->GetHealth();
			const str lVehicleType = mVehicle->GetClassId();	// TRICKY: don't fetch reference!!!
			SetVehicle(lVehicleType);
			mVehicle->DrainHealth(mVehicle->GetHealth() - lHealth);
		}
		else
		{
			lVelocity = mVehicle->GetVelocity();
			mVehicle->QueryFlip();
		}
	}
	else if (mLauncher && mLauncher->IsLoaded())
	{
		lPosition = mLauncher->GetPosition();
	}
	mCollisionSoundManager->Tick(lPosition);
	mUiManager->SetMicrophonePosition(TransformationF(gIdentityQuaternionF, lPosition), lVelocity);

	if (mLauncher && mLauncher->IsLoaded())
	{
		float lRealTimeRatio;
		CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		mLauncherYaw -= mLauncher->ContextObject::GetPhysics()->GetEngine(1)->GetLerpThrottle(0.2f, 0.2f, false) * 0.01f * lRealTimeRatio;
		mLauncherPitch -= mLauncher->ContextObject::GetPhysics()->GetEngine(0)->GetLerpThrottle(0.2f, 0.2f, false) * 0.01f * lRealTimeRatio;
		if (mLauncherYaw < -PIF)
		{
			mLauncherYaw += 2*PIF;
		}
		else if (mLauncherYaw > +PIF)
		{
			mLauncherYaw -= 2*PIF;
		}
		mLauncherPitch = Math::Clamp(mLauncherPitch, -PIF/2*0.6f, -PIF/90);
		mLauncher->SetBarrelAngle(mLauncherYaw, mLauncherPitch);
	}

	if (mPreviousFrameWinnerIndex == -1 && mWinnerIndex != -1)
	{
		if (mWinnerIndex == 0 && mComputerIndex != 0)
		{
			AddContextObject(new UiCure::Sound(GetResourceManager(), _T("win.wav"), mUiManager), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		}
		else if (mWinnerIndex == 1 && mComputerIndex != 1)
		{
			AddContextObject(new UiCure::Sound(GetResourceManager(), _T("win.wav"), mUiManager), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		}
		else
		{
			AddContextObject(new UiCure::Sound(GetResourceManager(), _T("kia.wav"), mUiManager), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		}
		mHeartBalance += (!!mWinnerIndex == mFlipRenderSide)? -1 : +1;
	}
	mPreviousFrameWinnerIndex = mWinnerIndex;

	return true;
}

void Game::TickFlyby()
{
	const double lFrameTime = 1.0/FPS;
	mFlyByTime += lFrameTime;
}



str Game::GetVehicle() const
{
	if (mVehicle)
	{
		return mVehicle->GetClassId();
	}
	return str();
}

void Game::SetVehicle(const str& pVehicle)
{
	mAllowWin = true;
	if (mVehicle && mVehicle->IsLoaded() &&
		mVehicle->GetPosition().GetDistance(GetCutieStart().GetPosition()) < 2.0f*SCALE_FACTOR &&
		mVehicle->GetClassId() == pVehicle &&
		mVehicle->GetHealth() > 0)
	{
		mVehicle->DrainHealth(-1);
		// Degrade health every other round, so it becomes increasingly difficult to extend the game.
		if (GetComputerIndex() == 1)
		{
			mVehicle->DrainHealth(1 - GetVehicleStartHealth(GetRoundIndex()));
		}
		return;
	}
	delete mVehicle;
	mVehicle = (Cutie*)GameManager::CreateContextObject(pVehicle, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	bool lOk = (mVehicle != 0);
	assert(lOk);
	if (lOk)
	{
		mVehicle->SetInitialTransform(GetCutieStart());
		mVehicle->DrainHealth(1 - GetVehicleStartHealth(GetRoundIndex()));
		mVehicle->StartLoading();
	}
}

void Game::ResetLauncher()
{
	mAllowWin = true;
	delete mLauncher;
	mLauncher = new Launcher(this);
	AddContextObject(mLauncher, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	mLauncher->DisableRootShadow();
	mLauncher->StartLoading();
	mLauncherYaw = PIF*0.24f;
	mLauncherPitch = -PIF/4;

	// Drop all grenades.
	Cure::ContextManager::ContextObjectTable lObjectTable = GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->GetClassId().find(_T("grenade")) != str::npos)
		{
			GetContext()->PostKillObject(lObject->GetInstanceId());
		}
	}
}

UiCure::CppContextObject* Game::GetP1() const
{
	return mVehicle;
}

UiCure::CppContextObject* Game::GetP2() const
{
	return mLauncher;
}

Level* Game::GetLevel() const
{
	return mLevel;
}

Cutie* Game::GetCutie() const
{
	return mVehicle;
}

Launcher* Game::GetLauncher() const
{
	return mLauncher;
}

Ctf* Game::GetCtf() const
{
	return mCtf;
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

void Game::SetThrottle(UiCure::CppContextObject* pPlayer, float pThrottle)
{
	if (pPlayer == mLauncher)
	{
		mLauncher->SetEnginePower(0, pThrottle);
		return;
	}
	if (pThrottle < 0 && mVehicle->GetForwardSpeed() > 0.5f)
	{
		mVehicle->SetEnginePower(2, -pThrottle);
		return;
	}
	mVehicle->SetEnginePower(2, 0);	// Disengage brakes.
	mVehicle->SetEnginePower(0, pThrottle);
}

bool Game::Shoot()
{
	if (GetLauncherLockPercent() < 1.0f)
	{
		return false;
	}

	Grenade* lGrenade = (Grenade*)GameManager::CreateContextObject(_T("grenade"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	bool lOk = (lGrenade != 0);
	assert(lOk);
	if (lOk)
	{
		TransformationF t(mLauncher->GetOrientation(), mLauncher->GetPosition()+Vector3DF(0, 0, +2.5f));
		lGrenade->SetInitialTransform(t);
		lGrenade->StartLoading();
		mLaucherLockWatch.Start();
		mIsLauncherBarrelFree = false;
	}
	return lOk;
}

Cure::ContextObject* Game::CreateRoboBall()
{
	RoboBall* lRoboBall = (RoboBall*)GameManager::CreateContextObject(_T("robo_ball"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	assert(lRoboBall);
	if (lRoboBall)
	{
		TransformationF t(QuaternionF(), Vector3DF(0, 0, 30));
		lRoboBall->SetInitialTransform(t);
		lRoboBall->StartLoading();
	}
	return lRoboBall;
}

float Game::GetMuzzleVelocity() const
{
	float lMuzzleVelocity = 60.0;
	if (mLauncher && mLauncher->IsLoaded() && mVehicle && mVehicle->IsLoaded())
	{
		lMuzzleVelocity = ::pow(mLauncher->GetPosition().GetDistance(mVehicle->GetPosition()), 0.5f) * 3.6f;
	}
	return lMuzzleVelocity;
}

float Game::GetLauncherLockPercent() const
{
	float lValue = 1;
	if (mLaucherLockWatch.IsStarted())
	{
		float lRealTimeRatio;
		CURE_RTVAR_GET(lRealTimeRatio, =(float), Cure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
		lValue = std::min(1.0f, (float)mLaucherLockWatch.GetTimeDiff() * lRealTimeRatio / GRENADE_RELAUNCH_DELAY);
	}
	return lValue;
}

bool Game::IsLauncherBarrelFree() const
{
	return mIsLauncherBarrelFree;
}

void Game::FreeLauncherBarrel()
{
	mIsLauncherBarrelFree = true;
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
	mRoundIndex = 0;
}

void Game::AddScore(double pCutieScore, double pLauncherScore)
{
	if (mComputerIndex == -1 || mWinnerIndex != -1 || !IsScoreCountingEnabled())
	{
		return;
	}
	// Difficulty: multiply by lerp(1, 4) ^ 2, with some factor...
	double lDifficultyFactor = (mComputerDifficulty * 3.0 + 1.0);
	lDifficultyFactor *= lDifficultyFactor;
	if (mComputerIndex == 1)
	{
		mScore += pCutieScore * lDifficultyFactor;
	}
	else
	{
		mScore += pLauncherScore * lDifficultyFactor;
	}
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
	return (mScoreCountingEnabled && mComputerIndex != -1);
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
			float x = Random::Uniform(-1.0f, 1.0f);
			float y = Random::Uniform(-1.0f, 1.0f);
			float z = -1;
			TransformationF lTransform(gIdentityQuaternionF, pPosition + Vector3DF(x, y, z));
			lPuff->SetInitialTransform(lTransform);
			const float lAngle = Random::Uniform(0.0f, 2*PIF);
			x = (14.0f * i/lParticleCount - 10) * cos(lAngle);
			y = (6 * Random::Uniform(-1.0f, 1.0f)) * sin(lAngle);
			z = (17 + 8 * sin(5*PIF*i/lParticleCount) * Random::Uniform(0.0f, 1.0f)) * Random::Uniform(0.2f, 1.0f);
			lPuff->StartParticle(UiCure::Props::PARTICLE_SOLID, Vector3DF(x, y, z), Random::Uniform(3.0f, 7.0f) * lScale, 0.5f, Random::Uniform(3.0f, 7.0f));
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
			float x = Random::Uniform(-1.0f, 1.0f);
			float y = Random::Uniform(-1.0f, 1.0f);
			float z = Random::Uniform(-1.0f, 1.0f);
			TransformationF lTransform(gIdentityQuaternionF, pPosition + Vector3DF(x, y, z));
			lPuff->SetInitialTransform(lTransform);
			const float lOpacity = Random::Uniform(0.025f, 0.1f);
			lPuff->SetOpacity(lOpacity);
			x = x*12;
			y = y*12;
			z = Random::Uniform(0.0f, 7.0f);
			lPuff->StartParticle(UiCure::Props::PARTICLE_GAS, Vector3DF(x, y, z), 0.003f / lOpacity, 0.1f, Random::Uniform(1.5f, 4.0f));
			lPuff->StartLoading();
		}
	}

	float lLevelShootEasyness = 4.5f;
	if (mLevelName == _T("level_elevate"))	// Bigger and open level = easier to hit Cutie.
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
			if (lPhysics->GetPhysicsType() == TBC::ChunkyPhysics::STATIC && lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
			{
				continue;	// No use pushing around trees, etc.
			}
			const Vector3DF lBodyCenter = GameTicker::GetPhysicsManager(true)->GetBodyPosition(lGeometry->GetBodyId());
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
			GameTicker::GetPhysicsManager(true)->AddForce(lGeometry->GetBodyId(), f);
			if (lObject == mVehicle)
			{
				if (d > 0.6f)
				{
					CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 0.2);
					mSlowmoTimer.Start();
				}
				d = std::max(0.005f, d);
				mVehicle->DrainHealth(d);
				if (mComputerIndex != -1)
				{
					// Weigh up minimum score for computer, if very bad.
					// Otherwise the car can go round, round and only
					// gain more points for every lap.
					lScore = std::max(0.18-mComputerDifficulty, lScore);
				}
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
					if (mAllowWin)
					{
						AddScore(-19000, lScore+4000);
						mWinnerIndex = (mWinnerIndex != 0)? 1 : mWinnerIndex;
					}
				}
				else
				{
					AddScore(-lScore, lScore);
				}
			}
		}
	}
	if (!lDidHitVehicle)
	{
		AddScore(+2000, -2000);
	}
}

void Game::OnCapture()
{
	if (mAllowWin)
	{
		AddScore(+10000, -10000);
		mWinnerIndex = (mWinnerIndex != 1)? 0 : mWinnerIndex;
	}
}

int Game::GetWinnerIndex() const
{
	return mWinnerIndex;
}

void Game::ResetWinnerIndex()
{
	mAllowWin = false;
	if (mCtf)
	{
		mCtf->StartSlideDown();
	}
	mWinnerIndex = -1;
	mPreviousFrameWinnerIndex = -1;
}

void Game::SetComputerIndex(int pIndex)
{
	assert(pIndex >= -1 && pIndex <= 1);
	mFlipRenderSide = 0;
	mFlipRenderSideFactor = 0;
	mComputerIndex = pIndex;
	delete mVehicleAi;
	mVehicleAi = 0;
	delete mLauncherAi;
	mLauncherAi = 0;
	if (mComputerIndex == 0)
	{
		mVehicleAi = new VehicleAi(this);
		AddContextObject(mVehicleAi, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		mVehicleAi->Init();
	}
	else if (mComputerIndex == 1)
	{
		mLauncherAi = new LauncherAi(this);
		AddContextObject(mLauncherAi, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		mLauncherAi->Init();
	}
}

int Game::GetComputerIndex() const
{
	return mComputerIndex;
}

void Game::NextComputerIndex()
{
	if (++mComputerIndex > 1)
	{
		mComputerIndex = -1;
	}
}

float Game::GetComputerDifficulty() const
{
	return mComputerDifficulty;
}

void Game::SetComputerDifficulty(float pDifficulty)
{
	assert(pDifficulty >= -1 && pDifficulty <= 1);
	mComputerDifficulty = pDifficulty;
}

int Game::GetHeartBalance() const
{
	return mHeartBalance;
}

void Game::SetHeartBalance(int pBalance)
{
	mHeartBalance = pBalance;
}

void Game::FlipRenderSides()
{
	mFlipRenderSide = !mFlipRenderSide;
}

bool Game::IsFlipRenderSide() const
{
	return !!mFlipRenderSide;
}

void Game::NextRound()
{
	++mRoundIndex;
}

int Game::GetRoundIndex() const
{
	return mRoundIndex;
}

float Game::GetVehicleStartHealth(int pRoundIndex) const
{
	if (GetComputerIndex() == -1 || pRoundIndex < 2)
	{
		return 1;
	}
	return ::pow(HEALTH_ROUND_FACTOR, pRoundIndex/2);
}

void Game::EndSlowmo()
{
	mSlowmoTimer.ReduceTimeDiff(-20);
}



void Game::SyncCameraPositions()
{
	switch (GetComputerIndex())
	{
		//case -1:	std::swap(mLeftCamera, mRightCamera);	break;
		case 0:		mLeftCamera = mRightCamera;		break;	// Computer is "left", copy player.
		case 1:		mRightCamera = mLeftCamera;		break;	// Computer is "right", copy player.
	}
}

bool Game::Render()
{
	if (!mVehicle || !mVehicle->IsLoaded() ||
		!mLevel || !mLevel->IsLoaded() ||
		!mCtf)
	{
		return true;
	}

	const PixelRect lFullRect(0, 0, mUiManager->GetCanvas()->GetActualWidth(), mUiManager->GetCanvas()->GetActualHeight());
	mLeftRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		mLeftRect.mBottom = mLeftRect.mBottom/2 - 5;
	}
	else
	{
		mLeftRect.mRight = mLeftRect.mRight/2 - 5;
	}
	mRightRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		mRightRect.mTop = mLeftRect.mBottom + 10;
	}
	else
	{
		mRightRect.mLeft = mLeftRect.mRight + 10;
	}
	if (mFlipRenderSideFactor)
	{
		if (mFlipRenderSideFactor == 1)
		{
			std::swap(mLeftRect, mRightRect);
		}
		else
		{
			const PixelRect r = mRightRect;
			mRightRect.mLeft = Math::Lerp(mRightRect.mLeft, mLeftRect.mLeft, mFlipRenderSideFactor);
			mRightRect.mRight = Math::Lerp(mRightRect.mRight, mLeftRect.mRight, mFlipRenderSideFactor);
			mRightRect.mTop = Math::Lerp(mRightRect.mTop, mLeftRect.mTop, mFlipRenderSideFactor);
			mRightRect.mBottom = Math::Lerp(mRightRect.mBottom, mLeftRect.mBottom, mFlipRenderSideFactor);
			mLeftRect.mLeft = Math::Lerp(mLeftRect.mLeft, r.mLeft, mFlipRenderSideFactor);
			mLeftRect.mRight = Math::Lerp(mLeftRect.mRight, r.mRight, mFlipRenderSideFactor);
			mLeftRect.mTop = Math::Lerp(mLeftRect.mTop, r.mTop, mFlipRenderSideFactor);
			mLeftRect.mBottom = Math::Lerp(mLeftRect.mBottom, r.mBottom, mFlipRenderSideFactor);
		}
	}
	switch (GetComputerIndex())
	{
		case -1:			break;	// Two player game.
		case 0:	mRightRect = lFullRect;	break;	// Single player, to right.
		case 1:	mLeftRect = lFullRect;	break;	// Single player, to left.
	}

	mLauncher->SetRootPosition(mLauncherPosition);

	// Yield smooth rotation when canvas orientation changed.
	if (mPreviousCanvasAngle != mUiManager->GetCanvas()->GetOutputRotation())
	{
		mPreviousCanvasAngle = mUiManager->GetCanvas()->GetOutputRotation();
		mLeftCamera.GetOrientation().RotateAroundOwnY(PIF);
		mRightCamera.GetOrientation().RotateAroundOwnY(PIF);
	}

	if (GetComputerIndex() == 0)
	{
		mLeftCamera = mRightCamera;	// For smooth transitions.
	}
	else if (GetComputerIndex() == 1)
	{
		mRightCamera = mLeftCamera;	// For smooth transitions.
	}

	if (mFlybyMode != FLYBY_INACTIVE)
	{
		return FlybyRender();
	}

	if (GetComputerIndex() != 0)
	{
		TransformationF t(gIdentityQuaternionF, Vector3DF(-50, -70, -5));
		const Vector3DF lVehiclePos = mVehicle->GetPosition();
		Vector3DF lOffset = mVehicleCamPos - lVehiclePos;
		lOffset.z = 0;
		const float lCamXYDistance = 13 * SCALE_FACTOR;
		float lCamHeight = 5 * SCALE_FACTOR;
		lOffset.Normalize(lCamXYDistance);
		float lAngle = (-lOffset).GetAngle(Vector3DF(0, lCamXYDistance, 0));
		if (lOffset.x < 0)
		{
			lAngle = -lAngle;
		}
		t.GetOrientation().RotateAroundOwnZ(lAngle);
		lOffset.z = lCamHeight;

		const TBC::PhysicsManager::BodyID lTerrainBodyId = mLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		Vector3DF lCollisionPoint;
		int x = 0;
		for (x = 0; x < 3; ++x)
		{
			const bool lIsCollision = (GameTicker::GetPhysicsManager(true)->QueryRayCollisionAgainst(
				lVehiclePos, lOffset, lOffset.GetLength(), lTerrainBodyId, &lCollisionPoint, 1) > 0);
			if (!lIsCollision)
			{
				break;
			}
			lCamHeight += 10*SCALE_FACTOR;
			lOffset.z = lCamHeight;
		}
		mVehicleCamHeight = Math::Lerp(mVehicleCamHeight, lCamHeight, 0.2f);
		lOffset.z = mVehicleCamHeight;

		t.GetOrientation().RotateAroundOwnX(-::atan(mVehicleCamHeight/lCamXYDistance) + PIF/18);
		mVehicleCamPos = lVehiclePos + lOffset;
		t.GetPosition() = mVehicleCamPos;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		// If computer runs the launcher, the vehicle should be displayed in landscape mode.
		if (GetComputerIndex() != 1)
		{
			t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		}
		if (mFlipRenderSideFactor)
		{
			t.GetOrientation().RotateAroundOwnY(PIF*mFlipRenderSideFactor);
		}
#endif // Touch
		mLeftCamera.Interpolate(mLeftCamera, t, 0.1f);
		mUiManager->SetCameraPosition(mLeftCamera);
		mUiManager->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
		mUiManager->Render(mLeftRect);
		mUiManager->GetRenderer()->ResetAmbientLight(true);
	}

	if (GetComputerIndex() != 1)
	{
		const float lLauncherHeight = 3;
		const Vector3DF lMuzzlePosition(mLauncherPosition + mLauncher->GetOrientation()*Vector3DF(0, 0, lLauncherHeight));

		float lRange = 100 * SCALE_FACTOR;
		float lLookDownAngle = -PIF/2;
		if (mVehicle && mVehicle->IsLoaded())
		{
			lRange = lMuzzlePosition.GetDistance(mVehicle->GetPosition());
			lLookDownAngle = ::sin((mVehicle->GetPosition().z-lMuzzlePosition.z)/lRange);
		}

		Vector3DF lStraightVector(mVehicle->GetPosition() - lMuzzlePosition);
		const float lCamDistance = 10 * SCALE_FACTOR;
		lStraightVector.Normalize(lCamDistance);
		lStraightVector.x = lCamDistance*sin(mLauncherYaw);
		lStraightVector.y = -lCamDistance*cos(mLauncherYaw);
		lStraightVector.z = -lStraightVector.z + lLauncherHeight*0.7f;
		TransformationF t(gIdentityQuaternionF, mLauncherPosition+lStraightVector);
		t.GetOrientation().RotateAroundOwnZ(mLauncherYaw*0.9f);
		t.GetOrientation().RotateAroundOwnX(lLookDownAngle);
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (GetComputerIndex() == -1)
		{
			// Launcher always displayed in portrait in dual play.
			const float lOrientationFactor = mFlipRenderSideFactor + 0.5f;
			t.GetOrientation().RotateAroundOwnY(PIF*lOrientationFactor);
		}
		else
		{
			// Single play in landscape mode.
			if (mFlipRenderSideFactor)
			{
				t.GetOrientation().RotateAroundOwnY(PIF*mFlipRenderSideFactor);
			}
		}
#endif // Touch
		mRightCamera.Interpolate(mRightCamera, t, 0.1f);
		mUiManager->SetCameraPosition(mRightCamera);
		const float lDistanceFoV = 5667 / ::pow(lRange, 1.2f);
		mUiManager->GetRenderer()->SetViewFrustum(std::min(60.0f, lDistanceFoV), 1.5f, 500);
		mUiManager->Render(mRightRect);
	}
	return true;
}

bool Game::Paint()
{
	return true;
}



bool Game::FlybyRender()
{
	const Vector3DF lCutie = mVehicle->GetPosition();
	const Vector3DF lGoal = mCtf->GetPosition();
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
		// Look at cutie, goal and launcher in more detail.
		const double lDetailTime = lTotalFlybyTime - lSweepTime;
		// Orientation. Treat orientation and position in different time slices, because if
		// both happen at the same time, perception of space is without a doubt lost.
		if (mFlyByTime-lSweepTime < lDetailTime * 1/12)
		{
			// Stare right at Cutie.
			t.GetOrientation().RotateAroundOwnZ(+PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 3/12)
		{
			// Stand beside Cutie.
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
		else if (mFlyByTime-lSweepTime < lDetailTime * 10/12)
		{
			// Look right at the launcher.
			t.GetOrientation().RotateAroundOwnZ(+PIF*7/8);
			t.GetOrientation().RotateAroundOwnX(-PIF/10);
		}
		else
		{
			// Stand beside the launcher.
			t.GetOrientation().RotateAroundOwnZ(+PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/4);
		}
		// Position.
		if (mFlyByTime-lSweepTime < lDetailTime * 1/3)
		{
			t.SetPosition(lCutie + Vector3DF(+1.33f, +6.67f, +3.33f));
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 2/3)
		{
			t.SetPosition(lGoal + Vector3DF(-13.33f, -10, +10));
		}
		else if (mFlyByTime-lSweepTime < lDetailTime * 10/12)
		{
			t.SetPosition(mLauncherPosition + Vector3DF(+1.67f, +5, +3.33f));	// In front of launcher.
		}
		else
		{
			t.SetPosition(mLauncherPosition + Vector3DF(+4.67f, 0, +4.67f));	// Beside launcher.
		}
	}
#ifdef LEPRA_TOUCH_LOOKANDFEEL
	// If computer runs the launcher, the vehicle should be displayed in landscape mode.
	if (GetComputerIndex() != 1)
	{
		t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
	}
#endif // Touch
	if (GetComputerIndex() != 0)
	{
		mLeftCamera.Interpolate(mLeftCamera, t, 0.05f);
		mUiManager->SetCameraPosition(mLeftCamera);
		mUiManager->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
		mUiManager->Render(mLeftRect);
	}
	if (GetComputerIndex() != 1)
	{
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		if (GetComputerIndex() == 0)
		{
			// Fly-by always in landscape when single playing.
			t.GetOrientation().RotateAroundOwnY(PIF*0.5f);
		}
		else
		{
			// The launcher in portrait for dual play.
			t.GetOrientation().RotateAroundOwnY(PIF);
		}
#endif // Touch
		mRightCamera.Interpolate(mRightCamera, t, 0.05f);
		mUiManager->SetCameraPosition(mRightCamera);
		mUiManager->GetRenderer()->SetViewFrustum(60, 1.5f, 500);
		mUiManager->Render(mRightRect);
	}
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



void Game::WillMicroTick(float pTimeDelta)
{
	MicroTick(pTimeDelta);
}

void Game::DidPhysicsTick()
{
	PostPhysicsTick();
}

void Game::OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal)
{
	GameManager::OnTrigger(pTrigger, pTriggerListenerId, pOtherBodyId, pNormal);
}

void Game::OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
	const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity)
{
	GameManager::OnForceApplied(pObjectId, pOtherObjectId, pBodyId, pOtherBodyId, pForce, pTorque, pPosition, pRelativeVelocity);
}



void Game::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk && pObject == mVehicle)
	{
		assert(pObject->GetPhysics()->GetEngineCount() == 3);
		const str lName = _T("float_childishness");
		new Cure::FloatAttribute(mVehicle, lName, 0.67f);
	}
	if (pOk && pObject == mLauncher)
	{
		// Create a mock engine on the launcher that we use to navigate.
		mLauncher->CreateEngines();
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
	UiCure::Machine* lMachine = 0;
	if (strutil::StartsWith(pClassId, _T("grenade")))
	{
		lMachine = new Grenade(GetResourceManager(), pClassId, mUiManager, GetMuzzleVelocity());
	}
	else if (strutil::StartsWith(pClassId, _T("robo_ball")))
	{
		lMachine = new RoboBall(this, pClassId);
	}
	else if (strutil::StartsWith(pClassId, _T("cutie")) ||
		strutil::StartsWith(pClassId, _T("monster")) ||
		strutil::StartsWith(pClassId, _T("corvette")) ||
		strutil::StartsWith(pClassId, _T("road_roller")))
	{
		lMachine = new Cutie(GetResourceManager(), pClassId, mUiManager);
	}
	else
	{
		lMachine = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
	}
	if (lMachine)
	{
		lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager, _T("mud_particle_01"), 3, 0.6f, 2.0f));
	}
	return lMachine;
}

bool Game::Initialize()
{
	bool lOk = true;
	if (lOk)
	{
		mWinnerIndex = -1;
		mPreviousFrameWinnerIndex = -1;

		QuaternionF lRotation;
		lRotation.RotateAroundOwnX(-PIF/4);
		lRotation.RotateAroundOwnZ(-PIF/8);
		mLeftCamera = TransformationF(lRotation, Vector3DF(-25, -50, 35));
		mRightCamera = mLeftCamera;
#ifdef LEPRA_TOUCH_LOOKANDFEEL
		mLeftCamera.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		mRightCamera.GetOrientation().RotateAroundOwnY(+PIF*0.5f);
#endif // Touch

		mLauncherPosition = Vector3DF(0, -75.2f, 10.5f);

		lOk = InitializeUniverse();
	}
	if (lOk)
	{
		const bool lPixelShadersEnabled = mUiManager->GetRenderer()->IsPixelShadersEnabled();
		mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(-1, 0.5f, -1.5),
			Vector3DF(1,1,1) * (lPixelShadersEnabled? 1.0f : 1.5f), 300);
		mUiManager->GetRenderer()->EnableAllLights(true);
	}
	if (lOk)
	{
		SetVehicle(_T("cutie"));
		ResetLauncher();
	}
	if (lOk)
	{
		SetFlybyMode(FLYBY_INACTIVE);
	}
	return lOk;
}

bool Game::InitializeUniverse()
{
	bool lOk = true;
	if (lOk)
	{
		delete mLevel;
		UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, _T("mud_particle_01"), 1, 1, 10, 2);
		mLevel = new Level(GetResourceManager(), mLevelName, mUiManager, lGravelParticleEmitter);
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
	else if (pType == _T("trig_ctf"))
	{
		mCtf = new Ctf(GetContext());
		return mCtf;
	}
	else if (pType == _T("trig_elevator"))
	{
		return new CutieElevator(this);
	}
	else if (pType == _T("context_path"))
	{
		return mLevel->QueryPath();
	}
	else if (pType == _T("see_through"))
	{
		return new SeeThrough(GetContext(), this);
	}
	else if (pType == _T("anything"))
	{
		return CreateRoboBall();
	}
	return (0);
}



}
