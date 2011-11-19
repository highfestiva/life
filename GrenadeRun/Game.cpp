
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "Game.h"
#include "../Cure/Include/ContextManager.h"
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
#include "Ctf.h"
#include "Cutie.h"
#include "Grenade.h"
#include "Launcher.h"
#include "LauncherAi.h"
#include "Spawner.h"



namespace GrenadeRun
{



Game::Game(UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
	Cure::GameTicker(),
	Cure::GameManager(Cure::GameTicker::GetTimeManager(), pVariableScope, pResourceManager, 400, 3, 3),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mLightId(UiTbc::Renderer::INVALID_LIGHT),
	mLevel(0),
	mIsFlyingBy(true),
	mFlyByTime(0),
	mVehicle(0),
	mVehicleCamPos(0, 0, 200),
	mVehicleCamHeight(15),
	mIsLaunching(false),
	mLauncherYaw(0),
	mLauncherPitch(-PIF/4),
	mWinnerIndex(-1),
	mPreviousFrameWinnerIndex(-1),
	mCtf(0),
	mLauncher(0),
	mLauncherAi(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"), UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f));
}

Game::~Game()
{
	mUiManager->GetRenderer()->RemoveLight(mLightId);

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
	mUiManager = 0;
	SetVariableScope(0);	// Not owned by us.

	mCtf = 0;
	mLauncher = 0;
	mLauncherAi = 0;
}

UiCure::GameUiManager* Game::GetUiManager() const
{
	return mUiManager;
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
		mLeftCamera = TransformationF(lRotation, Vector3DF(-50, -100, 70));
		mRightCamera = mLeftCamera;
#ifdef LEPRA_IOS_LOOKANDFEEL
		mLeftCamera.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		mRightCamera.GetOrientation().RotateAroundOwnY(+PIF*0.5f);
#endif // iOS

		lOk = InitializeTerrain();
	}
	if (lOk)
	{
		mLightId = mUiManager->GetRenderer()->AddDirectionalLight(
			UiTbc::Renderer::LIGHT_MOVABLE, Vector3DF(-1, 0.5f, -1.5),
			Color::Color(255, 255, 255), 1.0f, 300);
		mUiManager->GetRenderer()->EnableAllLights(true);
	}
	if (lOk)
	{
		mVehicle = (Cutie*)Parent::CreateContextObject(_T("cutie"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mVehicle != 0);
		assert(lOk);
		if (lOk)
		{
			TransformationF t(QuaternionF(), Vector3DF(-173, -85, 7));
			t.GetOrientation().RotateAroundOwnZ(-PIF*0.6f);
			mVehicle->SetInitialTransform(t);
			mVehicle->StartLoading();
		}
	}
	if (lOk)
	{
		mLauncher = new Launcher(this);
		AddContextObject(mLauncher, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mLauncher != 0);
		assert(lOk);
		if (lOk)
		{
			mLauncher->DisableRootShadow();
			mLauncher->StartLoading();
		}
	}
	if (lOk)
	{
		/*mLauncherAi = new LauncherAi(this);
		AddContextObject(mLauncherAi, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		mLauncherAi->Init();*/
		mComputerIndex = 0;
	}
	if (lOk)
	{
		mIsFlyingBy = true;
		mFlyByTime = 0;
	}
	return lOk;
}

bool Game::Tick()
{
	GameTicker::GetTimeManager()->Tick();

	Vector3DF lPosition;
	Vector3DF lVelocity;
	if (mVehicle)
	{
		lPosition = mVehicle->GetPosition()+Vector3DF(0, 0, -2);
		lVelocity = mVehicle->GetVelocity();

		mVehicle->QueryFlip();
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
		mLauncherPitch = Math::Clamp(mLauncherPitch, -PIF/2*0.6f, -PIF/90);
		lQuaternion.RotateAroundWorldZ(mLauncherYaw);
		lQuaternion.RotateAroundOwnX(mLauncherPitch);
		mLauncher->SetRootOrientation(lQuaternion);
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
	}
	mPreviousFrameWinnerIndex = mWinnerIndex;

	return true;
}



UiCure::CppContextObject* Game::GetP1() const
{
	return mVehicle;
}

UiCure::CppContextObject* Game::GetP2() const
{
	return mLauncher;
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
		mLauncher->SetEnginePower(0, pThrottle, 0);
		return;
	}
	if (pThrottle < 0 && mVehicle->GetForwardSpeed() > 0.5f)
	{
		mVehicle->SetEnginePower(2, -pThrottle, 0);
		return;
	}
	mVehicle->SetEnginePower(2, 0, 0);	// Disengage brakes.
	mVehicle->SetEnginePower(0, pThrottle, 0);
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

float Game::GetMuzzleVelocity() const
{
	float lMuzzleVelocity = 50.0;
	if (mLauncher && mLauncher->IsLoaded() && mVehicle && mVehicle->IsLoaded())
	{
		lMuzzleVelocity = ::pow(mLauncher->GetPosition().GetDistance(mVehicle->GetPosition()), 0.5f) * 3.5f;
	}
	return lMuzzleVelocity;
}

bool Game::IsLauncherLocked() const
{
	return mIsLaunching;
}

void Game::UnlockLauncher()
{
	mIsLaunching = false;
}

bool Game::IsFlyingBy() const
{
	return mIsFlyingBy;
}

void Game::Detonate(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pExplosive, Cure::ContextObject* pTarget, TBC::PhysicsManager::BodyID pExplosiveBodyId, TBC::PhysicsManager::BodyID pTargetBodyId)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pExplosive, mLevel, pExplosiveBodyId, 10000, true);

	if (pTarget == mLevel)
	{
		// Stones and mud. More if hit ground, less otherwise.
		const int lParticleCount = (mLevel->GetStructureGeometry((unsigned)0)->GetBodyId() == pTargetBodyId)? 100 : 10;
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
			const float lAngle = (float)Random::Uniform(0, 2*PIF);
			x = (20.0f * i/lParticleCount - 10) * cos(lAngle);
			y = (8 * (float)Random::Uniform(-1, 1)) * sin(lAngle);
			z = (30 + 12 * sin(5*PIF*i/lParticleCount) * (float)Random::Uniform(0.0, 1)) * (float)Random::Uniform(0.2f, 1.0f);
			lPuff->StartParticle(UiCure::Props::PARTICLE_SOLID, Vector3DF(x, y, z), (float)Random::Uniform(6, 12), 0.5f, (float)Random::Uniform(3, 7));
			lPuff->StartLoading();
		}
	}

	{
		// Release gas puffs.
		const int lParticleCount = 5;
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
			lPuff->StartParticle(UiCure::Props::PARTICLE_GAS, Vector3DF(x, y, z), 0.003f / lOpacity, 0.1f, (float)Random::Uniform(2, 6));
			lPuff->StartLoading();
		}
	}

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
		const Vector3DF lEpicenter = pPosition + Vector3DF(0, 0, -1.5f);
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
			if (d > 50*3)
			{
				continue;
			}
			d = 1/d;
			f *= d;
			d *= 12;
			d = d*d*d;
			d = std::min(1.0f, d);
			const float lMaxForceFactor = 2000.0f;
			const float ff = lMaxForceFactor * lObject->GetMass() * d;
			if (f.z <= 0.1f)
			{
				f.z += 0.3f;
			}
			f *= ff;
			GetPhysicsManager()->AddForce(lGeometry->GetBodyId(), f);
			if (lObject == mVehicle)
			{
				mVehicle->DrainHealth(d * 0.7f);
				if (mVehicle->GetHealth() <= 0)
				{
					mWinnerIndex = (mWinnerIndex != 0)? 1 : mWinnerIndex;
				}
			}
		}
	}
}

void Game::OnCapture()
{
	mWinnerIndex = (mWinnerIndex != 1)? 0 : mWinnerIndex;
}

int Game::GetWinnerIndex() const
{
	return mWinnerIndex;
}

int Game::GetComputerIndex() const
{
	return mComputerIndex;
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
	PixelRect lLeftRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		lLeftRect.mBottom = lLeftRect.mBottom/2 - 5;
	}
	else
	{
		lLeftRect.mRight = lLeftRect.mRight/2 - 5;
	}
	PixelRect lRightRect = lFullRect;
	if (lFullRect.mRight < lFullRect.mBottom)	// Portrait?
	{
		lRightRect.mTop = lLeftRect.mBottom + 10;
	}
	else
	{
		lRightRect.mLeft = lLeftRect.mRight + 10;
	}
	switch (GetComputerIndex())
	{
		case -1:			break;	// Two player game.
		case 0:	lRightRect = lFullRect;	break;	// Single player, to right.
		case 1:	lLeftRect = lFullRect;	break;	// Single player, to left.
	}

	const Vector3DF lLauncherPosition(0, -215, 27);
	mLauncher->SetRootPosition(lLauncherPosition);

	if (mIsFlyingBy)
	{
		const Vector3DF lCutie = mVehicle->GetPosition();
		const Vector3DF lGoal = mCtf->GetPosition();
		const double lTotalTime = 5.0;
		const double lFrameTime = 1.0/FPS;
		mFlyByTime += lFrameTime;
		if (mFlyByTime > lTotalTime)
		{
			mIsFlyingBy = false;
			return true;
		}

		TransformationF t;
		const double lSweepTime = lTotalTime * 0.25;
		if (mFlyByTime < lSweepTime)
		{
			// Sweep around the area in a circle.
			const float a = 0.8f * 2*PIF * (float)(mFlyByTime/lSweepTime);
			t.GetOrientation().RotateAroundOwnZ(a + PIF/2);
			t.GetOrientation().RotateAroundOwnX(-PIF/8);
			t.SetPosition(Vector3DF(::cos(a)*240, ::sin(a)*240, ::sin(a+PIF/8)*50 + 80));
		}
		else
		{
			// Look at cutie, goal and launcher in more detail.
			const double lDetailTime = lTotalTime - lSweepTime;
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
				t.SetPosition(lCutie + Vector3DF(+4, +20, +10));
			}
			else if (mFlyByTime-lSweepTime < lDetailTime * 2/3)
			{
				t.SetPosition(lGoal + Vector3DF(-40, -30, +30));
			}
			else if (mFlyByTime-lSweepTime < lDetailTime * 10/12)
			{
				t.SetPosition(lLauncherPosition + Vector3DF(+5, +15, +10));	// In front of launcher.
			}
			else
			{
				t.SetPosition(lLauncherPosition + Vector3DF(+14, 0, +14));	// Beside launcher.
			}
		}
#ifdef LEPRA_IOS_LOOKANDFEEL
		// If computer runs the launcher, the vehicle should be displayed in landscape mode.
		if (GetComputerIndex() != 1)
		{
			t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		}
#endif // iOS
		if (GetComputerIndex() != 0)
		{
			mLeftCamera.Interpolate(mLeftCamera, t, 0.05f);
			mUiManager->SetCameraPosition(mLeftCamera);
			mUiManager->GetRenderer()->SetViewFrustum(60, 3, 1000);
			mUiManager->Render(lLeftRect);
		}
		if (GetComputerIndex() != 1)
		{
#ifdef LEPRA_IOS_LOOKANDFEEL
			// The launcher is always displayed in portrait, both for single and dual play.
			t.GetOrientation().RotateAroundOwnY(PIF);
#endif // iOS
			mRightCamera.Interpolate(mRightCamera, t, 0.05f);
			mUiManager->SetCameraPosition(mRightCamera);
			mUiManager->GetRenderer()->SetViewFrustum(60, 3, 1000);
			mUiManager->Render(lRightRect);
		}
		return true;
	}

	if (GetComputerIndex() != 0)
	{
		TransformationF t(QuaternionF(), Vector3DF(-100, -140, -10));
		const Vector3DF lVehiclePos = mVehicle->GetPosition();
		Vector3DF lOffset = mVehicleCamPos - lVehiclePos;
		lOffset.z = 0;
		const float lCamXYDistance = 40;
		float lCamHeight = 15;
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
			const bool lIsCollision = (GetPhysicsManager()->QueryRayCollisionAgainst(
				lVehiclePos, lOffset, lOffset.GetLength(), lTerrainBodyId, &lCollisionPoint, 1) > 0);
			if (!lIsCollision)
			{
				break;
			}
			lCamHeight += 15;
			lOffset.z = lCamHeight;
		}
		mVehicleCamHeight = Math::Lerp(mVehicleCamHeight, lCamHeight, 0.2f);
		lOffset.z = mVehicleCamHeight;

		t.GetOrientation().RotateAroundOwnX(-::atan(mVehicleCamHeight/lCamXYDistance) + PIF/18);
		mVehicleCamPos = lVehiclePos + lOffset;
		t.GetPosition() = mVehicleCamPos;
#ifdef LEPRA_IOS_LOOKANDFEEL
		// If computer runs the launcher, the vehicle should be displayed in landscape mode.
		if (GetComputerIndex() != 1)
		{
			t.GetOrientation().RotateAroundOwnY(-PIF*0.5f);
		}
#endif // iOS
		mLeftCamera.Interpolate(mLeftCamera, t, 0.1f);
		mUiManager->SetCameraPosition(mLeftCamera);
		mUiManager->GetRenderer()->SetViewFrustum(60, 3, 1000);
		mUiManager->Render(lLeftRect);
	}

	if (GetComputerIndex() != 1)
	{
		const float lLauncherHeight = 6;
		const Vector3DF lMuzzlePosition(lLauncherPosition + mLauncher->GetOrientation()*Vector3DF(0, 0, lLauncherHeight));

		float lRange = 150.0f;
		float lLookDownAngle = -PIF/2;
		if (mVehicle && mVehicle->IsLoaded())
		{
			lRange = lMuzzlePosition.GetDistance(mVehicle->GetPosition());
			lLookDownAngle = ::sin((mVehicle->GetPosition().z-lMuzzlePosition.z)/lRange);
		}

		Vector3DF lStraightVector(mVehicle->GetPosition() - lMuzzlePosition);
		const float lCamDistance = 10;
		lStraightVector.Normalize(lCamDistance);
		lStraightVector.x = lCamDistance*sin(mLauncherYaw);
		lStraightVector.y = -lCamDistance*cos(mLauncherYaw);
		lStraightVector.z = -lStraightVector.z + lLauncherHeight*0.7f;
		TransformationF t(QuaternionF(), lLauncherPosition+lStraightVector);
		t.GetOrientation().RotateAroundOwnZ(mLauncherYaw*0.9f);
		t.GetOrientation().RotateAroundOwnX(lLookDownAngle);
#ifdef LEPRA_IOS_LOOKANDFEEL
		// The launcher is always displayed in portrait, both for single and dual play.
		t.GetOrientation().RotateAroundOwnY(PIF*0.5f);
#endif // iOS
		mRightCamera.Interpolate(mRightCamera, t, 0.1f);
		mUiManager->SetCameraPosition(mRightCamera);
		mUiManager->GetRenderer()->SetViewFrustum(std::min(60.0f, 9000/lRange), 3, 1000);
		mUiManager->Render(lRightRect);
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
	if (strutil::StartsWith(pClassId, _T("grenade")))
	{
		return new Grenade(GetResourceManager(), pClassId, mUiManager, GetMuzzleVelocity());
	}
	else if (strutil::StartsWith(pClassId, _T("cutie")))
	{
		return new Cutie(GetResourceManager(), pClassId, mUiManager);
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



Cure::ContextObject* Game::CreateLogicHandler(const str& pType) const
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
	assert(false);
	return (0);
}



}
