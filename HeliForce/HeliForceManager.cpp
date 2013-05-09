
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "HeliForceManager.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/IntAttribute.h"
#include "../Cure/Include/NetworkClient.h"
#include "../Cure/Include/Spawner.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Lepra/Include/Time.h"
#include "../Life/LifeClient/ClientOptions.h"
#include "../Life/LifeClient/ClientOptions.h"
#include "../Life/LifeClient/ExplodingMachine.h"
#include "../Life/LifeClient/FastProjectile.h"
#include "../Life/LifeClient/Level.h"
#include "../Life/LifeClient/MassObject.h"
#include "../Life/LifeClient/Mine.h"
#include "../Life/LifeClient/Projectile.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../Life/Explosion.h"
#include "../Life/ProjectileUtil.h"
#include "../Life/Spawner.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiExhaustEmitter.h"
#include "../UiCure/Include/UiJetEngineEmitter.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../UiLepra/Include/UiTouchstick.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "CenteredMachine.h"
#include "HeliForceConsoleManager.h"
#include "HeliForceTicker.h"
#include "LandingTrigger.h"
#include "RtVar.h"
#include "Version.h"

#define ICONBTN(i,n)			new UiCure::IconButton(mUiManager, GetResourceManager(), i, n)
#define ICONBTNA(i,n)			ICONBTN(_T(i), _T(n))
#define STILL_FRAMES_UNTIL_CAM_PANS	2



namespace HeliForce
{



namespace
{

struct Score
{
	str mName;
	int mKills;
	int mDeaths;
	int mPing;
};

struct DeathsAscendingOrder
{
	bool operator() (const Score& a, const Score& b) { return a.mDeaths < b.mDeaths; }
}
gDeathsAscendingOrder;

struct KillsDecendingOrder
{
	bool operator() (const Score& a, const Score& b) { return a.mKills > b.mKills; }
}
gKillsDecendingOrder;

}



HeliForceManager::HeliForceManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mAvatarId(0),
	mHadAvatar(false),
	mUpdateCameraForAvatar(false),
	mActiveWeapon(0),
	mLevel(0),
	mOldLevel(0),
	mCameraTransform(QuaternionF(), Vector3DF(0, -200, 100)),
	mCameraSpeed(0),
	mZoomHeli(false),
	mHitGroundFrameCount(STILL_FRAMES_UNTIL_CAM_PANS),
	mIsHitThisFrame(false),
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	mFireButton(0),
#endif // Touch or emulated touch.
	mStickLeft(0),
	mStickRight(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("plastic"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));

	SetConsoleManager(new HeliForceConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.005f, 0.0f, 0.2f);
}

HeliForceManager::~HeliForceManager()
{
	Close();

	delete mStickLeft;
	mStickLeft = 0;
	delete mStickRight;
	mStickRight = 0;
}

void HeliForceManager::LoadSettings()
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);

	Parent::LoadSettings();

	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const str lLeftName  = strutil::Format(_T("TouchstickLeft%i"), mSlaveIndex);
	const str lRightName = strutil::Format(_T("TouchstickRight%i"), mSlaveIndex);
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_FWD, lLeftName+_T(".AxisY-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_BRKBACK, lLeftName+_T(".AxisY+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_LEFT, lLeftName+_T(".AxisX-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_RIGHT, lLeftName+_T(".AxisX+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_FWD3D, lRightName+_T(".AxisY-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_BACK3D, lRightName+_T(".AxisY+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_LEFT3D, lRightName+_T(".AxisX-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_RIGHT3D, lRightName+_T(".AxisX+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_UP3D, lLeftName+_T(".AxisY-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_DOWN3D, lLeftName+_T(".AxisY+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_UP, lRightName+_T(".AxisY-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_DOWN, lRightName+_T(".AxisY+"));
#endif // Touch device or emulated touch device
}

void HeliForceManager::SetRenderArea(const PixelRect& pRenderArea)
{
	Parent::SetRenderArea(pRenderArea);
	UpdateTouchstickPlacement();
}

bool HeliForceManager::Open()
{
	bool lOk = Parent::Open();
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	if (lOk)
	{
		mFireButton = ICONBTNA("grenade.png", "");
		int x = mRenderArea.GetCenterX() - 32;
		int y = mRenderArea.mBottom - 76;
		mUiManager->GetDesktopWindow()->AddChild(mFireButton, x, y);
		mFireButton->SetVisible(true);
		mFireButton->SetOnClick(HeliForceManager, OnFireButton);
	}
#endif // Touch or emulated touch.
	return lOk;
}

void HeliForceManager::Close()
{
	ScopeLock lLock(GetTickLock());
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	delete mFireButton;
	mFireButton = 0;
#endif // Touch or emulated touch.
	Parent::Close();
}

void HeliForceManager::SetIsQuitting()
{
	((HeliForceConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void HeliForceManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



bool HeliForceManager::Paint()
{
	if (mStickLeft)
	{
		DrawStick(mStickLeft);
		DrawStick(mStickRight);
		mStickLeft->ResetTap();
		mStickRight->ResetTap();
	}

	const Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)lObject->GetAttribute(_T("float_health"));
		const str lInfo = lHealth? strutil::DoubleToString(lHealth->GetValue()*100, 0) : _T("");
		mUiManager->GetPainter()->SetColor(Color(255, 0, 0, 255), 0);
		mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		mUiManager->GetPainter()->PrintText(lInfo, mRenderArea.mLeft + 10, 10);
	}

	return true;
}



void HeliForceManager::OnLoginSuccess()
{
}



bool HeliForceManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraTransform.GetPosition()) <= pDistance*pDistance);
}

Cure::GameObjectId HeliForceManager::GetAvatarInstanceId() const
{
	return mAvatarId;
}



bool HeliForceManager::SetAvatarEnginePower(unsigned pAspect, float pPower)
{
	assert(pAspect >= 0 && pAspect < TBC::PhysicsEngine::ASPECT_COUNT);
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		return SetAvatarEnginePower(lObject, pAspect, pPower);
	}
	return false;
}



void HeliForceManager::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
{
	(void)pExplosive;

	mCollisionSoundManager->OnCollision(5.0f * pStrength, pPosition, pExplosiveGeometry, _T("explosion"));

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	//mLog.Infof(_T("Hit object normal is (%.1f; %.1f; %.1f)"), pNormal.x, pNormal.y, pNormal.z);
	const float lKeepOnGoingFactor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	Vector3DF u = pVelocity.ProjectOntoPlane(pNormal) * (1+lKeepOnGoingFactor);
	u -= pVelocity;	// Mirror and inverse.
	u.Normalize();
	const int lParticles = Math::Lerp(4, 10, pStrength * 0.2f);
	Vector3DF lStartFireColor(1.0f, 1.0f, 0.3f);
	Vector3DF lFireColor(0.6f, 0.4f, 0.2f);
	Vector3DF lStartSmokeColor(0.4f, 0.4f, 0.4f);
	Vector3DF lSmokeColor(0.2f, 0.2f, 0.2f);
	Vector3DF lShrapnelColor(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	if (dynamic_cast<Life::Mine*>(pExplosive))
	{
		lStartFireColor.Set(0.9f, 1.0f, 0.8f);
		lFireColor.Set(0.3f, 0.7f, 0.2f);
		lStartSmokeColor.Set(0.3f, 0.35f, 0.3f);
		lSmokeColor.Set(0.2f, 0.4f, 0.2f);
		lShrapnelColor.Set(0.5f, 0.5f, 0.1f);
	}
	lParticleRenderer->CreateExplosion(pPosition, pStrength * 1.5f, u, 1, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles*2, lParticles*2, lParticles, lParticles/2);
}

void HeliForceManager::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	(void)pHitObject;

	TBC::ChunkyPhysics* lPhysics = pBullet->GetPhysics();
	if (lPhysics)
	{
		TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(0);
		mCollisionSoundManager->OnCollision(5.0f, pBullet->GetPosition(), lGeometry, lGeometry->GetMaterial());
	}
}



void HeliForceManager::DidFinishLevel()
{
	mLog.AHeadline("Level done!");
	if (GetContext()->GetObject(mAvatarId))
	{
		UiCure::UserSound3dResource* lFinishSound = new UiCure::UserSound3dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, mCameraTransform.GetPosition(), Vector3DF(), 5.0f, 1.0f);
		mZoomHeli = true;
	}
}

void HeliForceManager::NextLevel()
{
	if (GetContext()->GetObject(mAvatarId))
	{
		mOldLevel = mLevel;
		const str lLevelName = (mOldLevel->GetClassId() == _T("level_00"))? _T("level_01") : _T("level_00");
		mLevel = (Life::Level*)Parent::CreateContextObject(lLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
		mLevel->StartLoading();
	}
}



Cure::RuntimeVariableScope* HeliForceManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool HeliForceManager::Reset()	// Run when disconnected. Removes all objects and displays login GUI.
{
	ScopeLock lLock(GetTickLock());
	return Parent::Reset();
}

bool HeliForceManager::InitializeUniverse()
{
	mMassObjectArray.clear();
	mLevel = (Life::Level*)Parent::CreateContextObject(_T("level_00"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	mLevel->StartLoading();
	return true;
}

void HeliForceManager::CreateChopper(const str& pClassId)
{
	mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;
	mZoomHeli = false;

	Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
	assert(lSpawner);
	Cure::ContextObject* lAvatar = Parent::CreateContextObject(pClassId, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lSpawner->PlaceObject(lAvatar);
	mAvatarId = lAvatar->GetInstanceId();
	mAvatarCreateTimer.Start();
	lAvatar->StartLoading();
}

void HeliForceManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void HeliForceManager::UpdateTouchstickPlacement()
{
	if (mTouchstickTimer.QueryTimeDiff() < 3.0)
	{
		return;
	}
	mTouchstickTimer.ClearTimeDiff();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	if (!mStickLeft)
	{
		int lScreenPixelWidth;
		CURE_RTVAR_GET(lScreenPixelWidth, =, GetVariableScope(), RTVAR_UI_DISPLAY_WIDTH, 1024);
		const int lFingerPixels = (int)(lScreenPixelWidth*lTouchScale*0.17f);	// 30 pixels in iPhone classic.
		mStickLeft  = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10),  0, lFingerPixels);
		const str lLeftName = strutil::Format(_T("TouchstickLeft%i"), mSlaveIndex);
		mStickLeft->SetUniqueIdentifier(lLeftName);
		mStickRight = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10), 0, lFingerPixels);
		const str lRightName = strutil::Format(_T("TouchstickRight%i"), mSlaveIndex);
		mStickRight->SetUniqueIdentifier(lRightName);
	}
	int lIndex = 0;
	int lCount = 0;
	GetMaster()->GetSlaveInfo(this, lIndex, lCount);
	if (lCount == 2)
	{
		PixelRect lLeftStickArea(mRenderArea);
		PixelRect lRightStickArea(mRenderArea);
		lLeftStickArea.mBottom = mRenderArea.GetHeight() * lTouchScale;
		lRightStickArea.mTop = mRenderArea.GetHeight() * (1-lTouchScale);
		lLeftStickArea.mRight = lLeftStickArea.mLeft + lLeftStickArea.GetHeight();
		lRightStickArea.mRight = lLeftStickArea.mRight;

		if (lIndex == 0)
		{
			mStickLeft->Move(lLeftStickArea, -90);
			mStickRight->Move(lRightStickArea, -90);
		}
		else
		{
			lLeftStickArea.mLeft += mRenderArea.GetWidth() - lLeftStickArea.GetWidth();
			lRightStickArea.mLeft = lLeftStickArea.mLeft;
			lLeftStickArea.mRight = lLeftStickArea.mLeft + lLeftStickArea.GetHeight();
			lRightStickArea.mRight = lLeftStickArea.mRight;
			std::swap(lLeftStickArea, lRightStickArea);
			mStickLeft->Move(lLeftStickArea, +90);
			mStickRight->Move(lRightStickArea, +90);
		}
	}
	else
	{
		PixelRect lLeftStickArea(mRenderArea);
		PixelRect lRightStickArea(mRenderArea);
		lLeftStickArea.mRight = mRenderArea.GetWidth() * lTouchScale;
		lRightStickArea.mLeft = mRenderArea.GetWidth() * (1-lTouchScale);
		lLeftStickArea.mTop = lLeftStickArea.mBottom - (lLeftStickArea.mRight - lLeftStickArea.mLeft);
		lRightStickArea.mTop = lLeftStickArea.mTop;
		mStickLeft->Move(lLeftStickArea, 0);
		mStickRight->Move(lRightStickArea, 0);
	}
#endif // Touch or emulated touch
}

void HeliForceManager::TickUiInput()
{
	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPlaybackMode != PLAYBACK_PLAY && lPhysicsStepCount > 0 && mAllowMovementInput)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);

		// Show billboard.
		if (lObject)
		{
			QuerySetChildishness(lObject);

			// Control steering.
			const Life::Options::Steering& s = mOptions.GetSteeringControl();
#define S(dir) s.mControl[Life::Options::Steering::CONTROL_##dir]
			float lYaw, _;
			lObject->GetOrientation().GetEulerAngles(lYaw, _, _);
			const float lWantedDirection = S(RIGHT3D) - S(LEFT3D);
			const float lCurrentDirection = lObject->GetVelocity().x * 0.05f;
			const float lWantedChange = lWantedDirection-lCurrentDirection;
			const float lPowerFwdRev = -::sin(lYaw) * lWantedChange;
			const float lPowerLeftRight = ::cos(lYaw) * lWantedChange;
			SetAvatarEnginePower(lObject, 4, lPowerFwdRev);
			SetAvatarEnginePower(lObject, 5, lPowerLeftRight);
			float lPower = S(UP3D) - S(DOWN3D);
			lPower = Math::Lerp(0.0f, 1.0f, lPower);
			SetAvatarEnginePower(lObject, 7, lPower);

			// Control fire.
			const Life::Options::FireControl& f = mOptions.GetFireControl();
#define F(alt) f.mControl[Life::Options::FireControl::FIRE##alt]
			if (F(0) > 0.5f)
			{
				//AvatarShoot();
			}
		}
	}
}

bool HeliForceManager::SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower)
{
	if (mZoomHeli)
	{
		return false;
	}
	return pAvatar->SetEnginePower(pAspect, pPower);
}

void HeliForceManager::TickUiUpdate()
{
	if (!mIsHitThisFrame)
	{
		mHitGroundFrameCount = 0;
	}
	mIsHitThisFrame = false;

	((HeliForceConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	mCollisionSoundManager->Tick(mCameraTransform.GetPosition());
}

bool HeliForceManager::UpdateMassObjects(const Vector3DF& pPosition)
{
	bool lOk = true;
	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		Life::MassObject* lObject = (Life::MassObject*)GetContext()->GetObject(*x, true);
		assert(lObject);
		lObject->SetRootPosition(pPosition);
	}
	return lOk;
}

void HeliForceManager::SetLocalRender(bool pRender)
{
	if (pRender)
	{
		bool lMass;
		CURE_RTVAR_GET(lMass, =, GetVariableScope(), RTVAR_UI_3D_ENABLEMASSOBJECTS, false);
		SetMassRender(lMass);
	}
	else
	{
		SetMassRender(false);
	}
}

void HeliForceManager::SetMassRender(bool pRender)
{
	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		Life::MassObject* lObject = (Life::MassObject*)GetContext()->GetObject(*x);
		if (lObject)
		{
			lObject->SetRender(pRender);
		}
	}
}



Cure::ContextObject* HeliForceManager::CreateContextObject(const str& pClassId) const
{
	Cure::CppContextObject* lObject;
	if (pClassId == _T("grenade") || pClassId == _T("rocket"))
	{
		lObject = new Life::FastProjectile(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
	}
	else if (pClassId == _T("bomb"))
	{
		lObject = new Life::Projectile(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
	}
	else if (strutil::StartsWith(pClassId, _T("mine")))
	{
		lObject = new Life::Mine(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
	}
	else if (pClassId == _T("stone") || pClassId == _T("cube"))
	{
		lObject = new UiCure::CppContextObject(GetResourceManager(), pClassId, mUiManager);
	}
	else if (strutil::StartsWith(pClassId, _T("level_")))
	{
		UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, 0.5f, 1, 10, 2);
		Life::Level* lLevel = new Life::Level(GetResourceManager(), pClassId, mUiManager, lGravelParticleEmitter);
		lLevel->DisableRootShadow();
		lObject = lLevel;
	}
	else if (strutil::StartsWith(pClassId, _T("helicopter_")))
	{
		UiCure::Machine* lMachine = new CenteredMachine(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
		lMachine->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	else
	{
		UiCure::Machine* lMachine = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
		lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager, 3, 0.6f, 2.0f));
		lObject = lMachine;
	}
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

Cure::ContextObject* HeliForceManager::CreateLogicHandler(const str& pType)
{
	/*if (pType == _T("trig_elevator"))
	{
		return new Cure::Elevator(GetContext());
	}
	else*/
	if (pType == _T("spawner"))
	{
		return new Life::Spawner(GetContext());
	}
	else if (pType == _T("real_time_ratio"))
	{
		return new LandingTrigger(GetContext());
	}
	/*else if (pType == _T("race_timer"))
	{
		return new RaceTimer(GetContext());
	}*/
	return (0);
}

void HeliForceManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		if (pObject->GetInstanceId() == mAvatarId)
		{
			log_volatile(mLog.Debug(_T("Yeeha! Loaded avatar!")));
			EaseDown(pObject, 0);
			mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;
		}
		else if (pObject == mLevel)
		{
			Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
			if (!lAvatar)
			{
				CreateChopper(_T("helicopter_01"));
			}
			else
			{
				Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)lAvatar->GetAttribute(_T("float_health"));
				lHealth->SetValue(1.0f);
				mAvatarCreateTimer.Start();
				Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
				assert(lSpawner);
				const Vector3DF lLandingPosition = GetLandingTriggerPosition(mOldLevel);
				const Vector3DF lHeliPosition = lAvatar->GetPosition();
				const Vector3DF lHeliDelta = lHeliPosition - lLandingPosition;
				const Vector3DF lNewPosition = lSpawner->GetSpawnPoint().GetPosition() + lHeliDelta;
				const float lCamAboveHeli = mCameraTransform.GetPosition().z - lHeliPosition.z;
				const Vector3DF lCamDelta = lSpawner->GetSpawnPoint().GetPosition() - lLandingPosition;

				GetContext()->DeleteObject(mOldLevel->GetInstanceId());
				mOldLevel = 0;

				EaseDown(lAvatar, &lNewPosition);
				mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;

				mCameraTransform.GetPosition() += lCamDelta;
				mCameraTransform.GetPosition().z = lAvatar->GetPosition().z + lCamAboveHeli;
				mCameraPreviousPosition = mCameraTransform.GetPosition();
				UpdateCameraPosition(true);
			}
			mZoomHeli = false;
		}
		else
		{
			log_volatile(mLog.Tracef(_T("Loaded object %s."), pObject->GetClassId().c_str()));
		}
		pObject->GetPhysics()->UpdateBonesObjectTransformation(0, gIdentityTransformationF);
		((UiCure::CppContextObject*)pObject)->UiMove();
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void HeliForceManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 200, false);

	if (!mIsHitThisFrame && pObject1->GetInstanceId() == mAvatarId)
	{
		mIsHitThisFrame = true;
		++mHitGroundFrameCount;
	}

	if (pObject1->GetInstanceId() == mAvatarId && mAvatarCreateTimer.IsStarted())
	{
		if (mAvatarCreateTimer.QueryTimeDiff() > 3.0)
		{
			mAvatarCreateTimer.Stop();
		}
		return;
	}

	// Check if we're colliding with a smooth landing pad.
	float lCollisionImpactFactor = 3;
	if (pObject1->GetInstanceId() == mAvatarId && pObject2 == mLevel)
	{
		const TBC::ChunkyClass::Tag* lTag = mLevel->GetClass()->GetTag(_T("anything"));
		std::vector<int>::const_iterator x = lTag->mBodyIndexList.begin();
		for (; x != lTag->mBodyIndexList.end(); ++x)
		{
			if (pObject2->GetPhysics()->GetBoneGeometry(*x)->GetBodyId() == pBody2Id)
			{
				lCollisionImpactFactor = 1;
				break;
			}
		}
	}

	const float lForce = pForce.GetLength() * lCollisionImpactFactor;
	if (lForce > 15000)
	{
		float lForce2 = lForce;
		lForce2 /= 3000;
		lForce2 *= 3 - 2*(pForce.GetNormalized()*Vector3DF(0,0,1));	// Sideways force means non-vertical landing or landing on non-flat surface.
		lForce2 *= 10 - 9*(pObject1->GetOrientation()*Vector3DF(0,0,1)*Vector3DF(0,0,1));	// Sideways orientation means chopper not aligned.
		Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)pObject1->GetAttribute(_T("float_health"));
		if (lHealth && lHealth->GetValue() > 0)
		{
			lForce2 *= lForce2;
			lForce2 /= pObject1->GetMass();
			lHealth->SetValue(lHealth->GetValue() - lForce2);
		}
	}
}



Vector3DF HeliForceManager::GetLandingTriggerPosition(Cure::ContextObject* pLevel) const
{
	assert(pLevel);
	Cure::ContextObject::Array::const_iterator x = pLevel->GetChildArray().begin();
	for (; x != pLevel->GetChildArray().end(); ++x)
	{
		LandingTrigger* lLandingTrigger = dynamic_cast<LandingTrigger*>(*x);
		if (lLandingTrigger)
		{
			const int lTriggerCount = pLevel->GetPhysics()->GetTriggerCount();
			for (int x = 0; x < lTriggerCount; ++x)
			{
				const TBC::PhysicsTrigger* lTrigger = pLevel->GetPhysics()->GetTrigger(x);
				if (pLevel->GetTrigger(lTrigger->GetPhysicsTriggerId(0)) == lLandingTrigger)
				{
					TransformationF lTransform;
					GetPhysicsManager()->GetTriggerTransform(lTrigger->GetPhysicsTriggerId(0), lTransform);
					return lTransform.GetPosition();
				}
			}
		}
	}
	assert(false);
	return Vector3DF();
}

float HeliForceManager::EaseDown(Cure::ContextObject* pObject, const Vector3DF* pStartPosition)
{
	const Cure::ObjectPositionalData* lPositionalData = 0;
	pObject->UpdateFullPosition(lPositionalData);
	Cure::ObjectPositionalData* lNewPositionalData = (Cure::ObjectPositionalData*)lPositionalData->Clone();
	if (pStartPosition)
	{
		lNewPositionalData->mPosition.mTransformation.SetPosition(*pStartPosition);
	}
	float lDistanceToGround = 0;
	const float lStep = 0.1f;
	for (int x = 0; x < 100; ++x)
	{
		pObject->SetFullPosition(*lNewPositionalData, 0);
		if (GetPhysicsManager()->IsColliding(pObject->GetInstanceId()))
		{
			break;
		}
		lNewPositionalData->mPosition.mTransformation.GetPosition().z -= lStep;
		lDistanceToGround += lStep;
	}
	delete lNewPositionalData;
	return lDistanceToGround;
}

void HeliForceManager::Shoot(Cure::ContextObject*, int)
{
}



void HeliForceManager::OnFireButton(UiTbc::Button*)
{
}



void HeliForceManager::DrawStick(Touchstick* pStick)
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (!pStick || !lAvatar)
	{
		return;
	}

	PixelRect lArea = pStick->GetArea();
	const int ow = lArea.GetWidth();
	const int lMargin = pStick->GetFingerRadius() / 8;
	const int r = pStick->GetFingerRadius() - lMargin;
	lArea.Shrink(lMargin*2);
	mUiManager->GetPainter()->DrawArc(lArea.mLeft, lArea.mTop, lArea.GetWidth(), lArea.GetHeight(), 0, 360, false);
	float x;
	float y;
	bool lIsPressing;
	pStick->GetValue(x, y, lIsPressing);
	if (lIsPressing)
	{
		Vector2DF v(x, y);
		v.Mul((ow+lMargin*2) / (float)ow);
		const float lLength = v.GetLength();
		if (lLength > 1)
		{
			v.Div(lLength);
		}
		x = v.x;
		y = v.y;
		x = 0.5f*x + 0.5f;
		y = 0.5f*y + 0.5f;
		const int w = lArea.GetWidth()  - r*2;
		const int h = lArea.GetHeight() - r*2;
		mUiManager->GetPainter()->DrawArc(
			lArea.mLeft + (int)(w*x),
			lArea.mTop  + (int)(h*y),
			r*2, r*2, 0, 360, true);
	}
}



void HeliForceManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		MoveCamera();
		UpdateCameraPosition(false);
	}

	if (mAvatarCreateTimer.IsStarted() || (mAvatarId && GetContext()->GetObject(mAvatarId)))
	{
		mAvatarDied.Stop();
	}
	else if (mAvatarId)
	{
		mAvatarDied.TryStart();
		if (mAvatarDied.QueryTimeDiff() > 0)
		{
			CreateChopper(_T("helicopter_01"));
		}
	}

	Parent::ScriptPhysicsTick();
}

void HeliForceManager::MoveCamera()
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar)
	{
		mCameraPreviousPosition = mCameraTransform.GetPosition();
		Vector3DF lAvatarPosition = lAvatar->GetPosition();
		TransformationF lTargetTransform(QuaternionF(), lAvatarPosition + Vector3DF(0, -60, 0));
		if (mZoomHeli)
		{
			Cure::ContextObject* lLevel = mOldLevel? mOldLevel : mLevel;
			lTargetTransform.GetPosition() = GetLandingTriggerPosition(lLevel) + Vector3DF(0, 0, 10);
		}
		else if (mHitGroundFrameCount >= STILL_FRAMES_UNTIL_CAM_PANS)
		{
			lTargetTransform.GetPosition().z += 30;
		}
		Vector3DF lDelta = Math::Lerp(mCameraTransform.GetPosition(), lTargetTransform.GetPosition(), 0.08f) - mCameraTransform.GetPosition();

		const float lCamDistance = mCameraTransform.GetPosition().GetDistance(lAvatarPosition);
		const float lMaxCamSpeed = Math::SmoothClamp(lCamDistance/40, 0.1f, 5.0f, 0.3f);
		if (lDelta.GetLength() > lMaxCamSpeed)
		{
			lDelta.Normalize(lMaxCamSpeed);
		}
		mCameraTransform.GetPosition() += lDelta;

		// Angle.
		const float x = lAvatarPosition.y - mCameraTransform.GetPosition().y;
		const float y = lAvatarPosition.z - mCameraTransform.GetPosition().z;
		const float lAngle = ::atan2(y, x);
		lTargetTransform.GetOrientation().RotateAroundOwnX(lAngle);
		mCameraTransform.GetOrientation().Slerp(mCameraTransform.GetOrientation(), lTargetTransform.GetOrientation(), 1.0f);
		mCameraSpeed = Math::Lerp(mCameraSpeed, lAvatar->GetVelocity().GetLength()/10, 0.02f);
		float lFoV = Math::Lerp(30.0f, 60.0f, mCameraSpeed);
		lFoV = Math::SmoothClamp(lFoV, 30.0f, 60.0f, 0.4f);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, lFoV);

		UpdateMassObjects(mCameraTransform.GetPosition());
	}
}

void HeliForceManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, mMicrophoneSpeed);
	}
}



LOG_CLASS_DEFINE(GAME, HeliForceManager);



}
