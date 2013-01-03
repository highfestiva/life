
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "PushManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/NetworkClient.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/Time.h"
#include "../Life/LifeClient/ClientOptions.h"
#include "../Life/LifeClient/ClientOptions.h"
#include "../Life/LifeClient/MassObject.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiExhaustEmitter.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiLepra/Include/UiTouchstick.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "Explosion.h"
#include "Grenade.h"
#include "Level.h"
#include "PushBarrel.h"
#include "PushConsoleManager.h"
#include "PushTicker.h"
#include "RoadSignButton.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "Version.h"

#define ICONBTN(i,n)			new UiCure::IconButton(mUiManager, GetResourceManager(), i, n)
#define ICONBTNA(i,n)			ICONBTN(_T(i), _T(n))



namespace Push
{



PushManager::PushManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mAvatarId(0),
	mHadAvatar(false),
	mCamRotateExtra(0),
	mPickVehicleButton(0),
	mAvatarInvisibleCount(0),
	mRoadSignIndex(0),
	mLevelId(0),
	mLevel(0),
	mSun(0),
	mCameraPosition(0, -200, 100),
	//mCameraFollowVelocity(0, 1, 0),
	mCameraUp(0, 0, 1),
	mCameraOrientation(PIF/2, acos(mCameraPosition.z/mCameraPosition.y), 0),
	mCameraTargetXyDistance(20),
	mCameraMaxSpeed(500),
	mIsSameSteering(false),
	mSteeringLockDirection(0),
	mLoginWindow(0),
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	mFireButton(0),
#endif // Touch or emulated touch.
	mStickLeft(0),
	mStickRight(0),
	mEnginePlaybackTime(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("plastic"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));

	::memset(mEnginePowerShadow, 0, sizeof(mEnginePowerShadow));

	mCameraPivotPosition = mCameraPosition + GetCameraQuaternion() * Vector3DF(0, mCameraTargetXyDistance*3, 0);

	SetConsoleManager(new PushConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));
}

PushManager::~PushManager()
{
	Close();

	delete mStickLeft;
	mStickLeft = 0;
	delete mStickRight;
	mStickRight = 0;

#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
#endif // Computer or touch device.
}

void PushManager::LoadSettings()
{
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

void PushManager::SetRenderArea(const PixelRect& pRenderArea)
{
	Parent::SetRenderArea(pRenderArea);
	if (mLoginWindow)
	{
		mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
			mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
	}

	UpdateTouchstickPlacement();

	CURE_RTVAR_GET(mCameraTargetXyDistance, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
}

bool PushManager::Open()
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
		mFireButton->SetOnClick(PushManager, OnFireButton);
	}
#endif // Touch or emulated touch.
	return lOk;
}

void PushManager::Close()
{
	ScopeLock lLock(GetTickLock());
	ClearRoadSigns();
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	delete mFireButton;
	mFireButton = 0;
#endif // Touch or emulated touch.
	Parent::Close();
	CloseLoginGui();
}

void PushManager::SetIsQuitting()
{
	CloseLoginGui();
	((PushConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	SetRoadSignsVisible(false);

	Parent::SetIsQuitting();
}

void PushManager::SetFade(float pFadeAmount)
{
	mCameraMaxSpeed = 100000.0f;
	float lBaseDistance;
	CURE_RTVAR_GET(lBaseDistance, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	mCameraTargetXyDistance = lBaseDistance + pFadeAmount*400.0f;
}



bool PushManager::Paint()
{
	if (mStickLeft)
	{
		DrawStick(mStickLeft);
		DrawStick(mStickRight);
		mStickLeft->ResetTap();
		mStickRight->ResetTap();
	}

#ifdef LIFE_DEMO
	const double lTime = mDemoTime.QueryTimeDiff();
	if ((mSlaveIndex >= 2 || (mSlaveIndex == 1 && lTime > 10*60))
		&& !IsQuitting())
	{
		const UiTbc::FontManager::FontId lOldFontId = SetFontHeight(36.0);
		str lDemoText = strutil::Format(
			_T(" This is a free demo.\n")
			_T(" Buy the full version\n")
			_T("to loose this annoying\n")
			_T("  text for player %i."), mSlaveIndex+1);
		if ((int)lTime % 3*60 >= 3*60-2)
		{
			lDemoText =
				_T("     ])0n7 b3 B1FF\n")
				_T("g!t pwn4ge & teh kekeke\n")
				_T("     !3UYZORZ n0vv\n");
		}
		const int lTextWidth = mUiManager->GetFontManager()->GetStringWidth(lDemoText);
		const int lTextHeight = mUiManager->GetFontManager()->GetLineHeight()*4;
		const int lOffsetX = (int)(cos(lTime*4.3)*15);
		const int lOffsetY = (int)(sin(lTime*4.1)*15);
		mUiManager->GetPainter()->SetColor(Color(255, (uint8)(50*sin(lTime)+50), (uint8)(127*sin(lTime*0.9)+127), 200), 0);
		mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		mUiManager->GetPainter()->PrintText(lDemoText, mRenderArea.GetCenterX()-lTextWidth/2+lOffsetX, mRenderArea.GetCenterY()-lTextHeight/2+lOffsetY);
		mUiManager->GetFontManager()->SetActiveFont(lOldFontId);
	}
#endif // Demo
	return true;
}



void PushManager::RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken)
{
	ScopeLock lLock(GetTickLock());
	CloseLoginGui();
	Parent::RequestLogin(pServerAddress, pLoginToken);
}

void PushManager::OnLoginSuccess()
{
	ClearRoadSigns();
}



void PushManager::SelectAvatar(const Cure::UserAccount::AvatarId& pAvatarId)
{
	DropAvatar();

	log_volatile(mLog.Debugf(_T("Clicked avatar %s."), pAvatarId.c_str()));
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendStatusMessage(GetNetworkClient()->GetSocket(), 0, Cure::REMOTE_OK,
		Cure::MessageStatus::INFO_AVATAR, wstrutil::Encode(pAvatarId), lPacket);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	SetRoadSignsVisible(false);
}

void PushManager::AddLocalObjects(std::hash_set<Cure::GameObjectId>& pLocalObjectSet)
{
	if (mAvatarId)
	{
		Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
		if (mHadAvatar && !lAvatar)
		{
			DropAvatar();
		}
		else if (lAvatar)
		{
			mHadAvatar = true;
		}
	}

	Parent::AddLocalObjects(pLocalObjectSet);
}

bool PushManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraPosition) <= pDistance*pDistance);
}

Cure::GameObjectId PushManager::GetAvatarInstanceId() const
{
	return mAvatarId;
}



bool PushManager::SetAvatarEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	assert(pAspect >= 0 && pAspect < TBC::PhysicsEngine::ASPECT_COUNT);
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		return SetAvatarEnginePower(lObject, pAspect, pPower, pAngle);
	}
	return false;
}



void PushManager::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition)
{
	(void)pExplosive;

	mCollisionSoundManager->OnCollision(5.0f, pPosition, pExplosiveGeometry);

	{
		// Shattered pieces, stones or mud.
		const float lScale = VISUAL_SCALE_FACTOR * 320 / mUiManager->GetCanvas()->GetWidth();
		const int lParticleCount = 7;
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
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
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

	if (!GetMaster()->IsLocalServer())	// If local server, it will already have given us a push.
	{
		const Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
			Explosion::PushObject(GetPhysicsManager(), lObject, pPosition, 1.0f);
		}
	}
}


Cure::RuntimeVariableScope* PushManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool PushManager::Reset()	// Run when disconnected. Removes all objects and displays login GUI.
{
	ScopeLock lLock(GetTickLock());
	ClearRoadSigns();
	bool lOk = Parent::Reset();
	if (lOk)
	{
		CreateLoginView();
	}
	return (lOk);
}

void PushManager::CreateLoginView()
{
	if (!mLoginWindow && !GetNetworkClient()->IsActive())
	{
		// If first attempt (i.e. no connection problems) just skip interactivity.
		if (mDisconnectReason.empty())
		{
			str lServerName;
			CURE_RTVAR_TRYGET(lServerName, =, Cure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
			if (strutil::StartsWith(lServerName, _T("0.0.0.0")))
			{
				lServerName = lServerName.substr(7);
			}
			const str lDefaultUserName = strutil::Format(_T("User%u"), mSlaveIndex);
			str lUserName;
			CURE_RTVAR_TRYGET(lUserName, =, GetVariableScope(), RTVAR_LOGIN_USERNAME, lDefaultUserName);
                        wstr lReadablePassword = L"CarPassword";
                        const Cure::MangledPassword lPassword(lReadablePassword);
			const Cure::LoginId lLoginToken(wstrutil::Encode(lUserName), lPassword);
			RequestLogin(lServerName, lLoginToken);
		}
		else
		{
			mLoginWindow = new LoginView(this, mDisconnectReason);
			mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
			mUiManager->GetDesktopWindow()->AddChild(mLoginWindow, 0, 0, 0);
			mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
				mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
			mLoginWindow->GetChild(_T("User"), 0)->SetKeyboardFocus();
		}
	}
}

bool PushManager::InitializeTerrain()
{
	mSun = 0;
	mCloudArray.clear();

	mLevelId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, _T("mud_particle_01"), 0.5f, 1, 10, 2);
	mLevel = new Level(GetResourceManager(), _T("level_02"), mUiManager, lGravelParticleEmitter);
	AddContextObject(mLevel, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED, mLevelId);
	bool lOk = (mLevel != 0);
	assert(lOk);
	if (lOk)
	{
		mLevel->DisableRootShadow();
		mLevel->SetAllowNetworkLogic(false);
		mLevel->StartLoading();
		mSun = new UiCure::Props(GetResourceManager(), _T("sun"), mUiManager);
		AddContextObject(mSun, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mSun != 0);
		assert(lOk);
		if (lOk)
		{
			mSun->StartLoading();
		}
	}
	const int lPrimeCloudCount = 11;	// TRICKY: must be prime or clouds start moving in sync.
	for (int x = 0; lOk && x < lPrimeCloudCount; ++x)
	{
		Cure::ContextObject* lCloud = new UiCure::Props(GetResourceManager(), _T("cloud_01"), mUiManager);
		AddContextObject(lCloud, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lCloud->StartLoading();
		mCloudArray.push_back(lCloud);
	}
	mMassObjectArray.clear();
	return (lOk);
}

void PushManager::CloseLoginGui()
{
	if (mLoginWindow)
	{
		ScopeLock lLock(GetTickLock());
		mUiManager->GetDesktopWindow()->RemoveChild(mLoginWindow, 0);
		delete (mLoginWindow);
		mLoginWindow = 0;
	}
}

void PushManager::ClearRoadSigns()
{
	ScopeLock lLock(GetTickLock());

	if (mPickVehicleButton)
	{
		GetContext()->DeleteObject(mPickVehicleButton->GetInstanceId());
		mPickVehicleButton = 0;
	}

	mRoadSignIndex = 0;
	RoadSignMap::iterator x = mRoadSignMap.begin();
	for (; x != mRoadSignMap.end(); ++x)
	{
		GetContext()->DeleteObject(x->second->GetInstanceId());
	}
	mRoadSignMap.clear();
}

void PushManager::SetRoadSignsVisible(bool pVisible)
{
	if (mPickVehicleButton)
	{
		mPickVehicleButton->SetIsMovingIn(!pVisible);
	}

	RoadSignMap::iterator x = mRoadSignMap.begin();
	for (; x != mRoadSignMap.end(); ++x)
	{
		x->second->SetIsMovingIn(pVisible);
	}

	mUiManager->GetInputManager()->SetCursorVisible(pVisible);
}



void PushManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void PushManager::UpdateTouchstickPlacement()
{
	if (mTouchstickTimer.QueryTimeDiff() < 3.0)
	{
		return;
	}
	mTouchstickTimer.ClearTimeDiff();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	if (!mStickLeft)
	{
		mStickLeft  = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10),  0, 30);
		const str lLeftName = strutil::Format(_T("TouchstickLeft%i"), mSlaveIndex);
		mStickLeft->SetUniqueIdentifier(lLeftName);
		mStickRight = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10), 0, 30);
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
		lLeftStickArea.mBottom = mRenderArea.GetHeight() / 3;
		lRightStickArea.mTop = mRenderArea.GetHeight() * 2 / 3;
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
		lLeftStickArea.mRight = mRenderArea.GetWidth() / 3;
		lRightStickArea.mLeft = mRenderArea.GetWidth() * 2 / 3;
		lLeftStickArea.mTop = lLeftStickArea.mBottom - (lLeftStickArea.mRight - lLeftStickArea.mLeft);
		lRightStickArea.mTop = lLeftStickArea.mTop;
		mStickLeft->Move(lLeftStickArea, 0);
		mStickRight->Move(lRightStickArea, 0);
	}
#endif // Touch or emulated touch
}

void PushManager::TickUiInput()
{
	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPlaybackMode != PLAYBACK_PLAY && lPhysicsStepCount > 0 && mAllowMovementInput)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
			QuerySetChildishness(lObject);

			// Control steering.
			const Life::Options::Steering& s = mOptions.GetSteeringControl();
#define S(dir) s.mControl[Life::Options::Steering::CONTROL_##dir]
#if 1
			float lLeftPowerFwdRev = S(FORWARD) - S(BREAKANDBACK);
			//float lRightPowerFwdRev = S(FORWARD3D) - S(BACKWARD3D);
			float lLeftPowerLR = S(RIGHT)-S(LEFT);
			float lRightPowerLR = (S(RIGHT3D) - S(LEFT3D)) * 0.65f;
			lRightPowerLR *= Math::Lerp(0.8f, 2.0f, std::abs(lLeftPowerFwdRev));
			static int cnt = 0;
			if (++cnt < 30)
			{
				return;
			}
			cnt = 0;

			SetAvatarEnginePower(lObject, 0, lLeftPowerFwdRev+lRightPowerLR, mCameraOrientation.x);
			SetAvatarEnginePower(lObject, 1, lLeftPowerLR, mCameraOrientation.x);
			SetAvatarEnginePower(lObject, 4, lLeftPowerFwdRev-lRightPowerLR, mCameraOrientation.x);
			SetAvatarEnginePower(lObject, 5, lLeftPowerLR, mCameraOrientation.x);
			SetAvatarEnginePower(lObject, 8, lRightPowerLR, mCameraOrientation.x);
#else
			const bool lIsMovingForward = lObject->GetForwardSpeed() > 3.0f;
			float lPowerFwdRev = lForward - std::max(lBack, lIsMovingForward? 0.0f : lBreakAndBack);
			SetAvatarEnginePower(lObject, 0, lPowerFwdRev, mCameraOrientation.x);
			float lPowerLR = S(RIGHT)-S(LEFT);
			SetAvatarEnginePower(lObject, 1, lPowerLR, mCameraOrientation.x);
			float lPower = S(HANDBRAKE) - std::max(S(BREAK), lIsMovingForward? lBreakAndBack : 0.0f);
			if (!SetAvatarEnginePower(lObject, 2, lPower, mCameraOrientation.x) &&
				lBreakAndBack > 0 && Math::IsEpsEqual(lBack, 0.0f, 0.01f))
			{
				// Someone is apparently trying to stop/break, but no engine configured for breaking.
				// Just apply it as a reverse motion.
				lPowerFwdRev = lForward - lBreakAndBack;
				SetAvatarEnginePower(lObject, 0, lPowerFwdRev, mCameraOrientation.x);
			}
			lPower = S(UP)-S(DOWN);
			SetAvatarEnginePower(lObject, 3, lPower, mCameraOrientation.x);
			lPower = S(FORWARD3D) - S(BACKWARD3D);
			SetAvatarEnginePower(lObject, 4, lPower, mCameraOrientation.x);
			lPower = S(RIGHT3D) - S(LEFT3D);
			SetAvatarEnginePower(lObject, 5, lPower, mCameraOrientation.x);
			// Engine aspect 6 is not currently in use (3D handbraking). Might come in useful some day though.
			lPower = S(UP3D) - S(DOWN3D);
			SetAvatarEnginePower(lObject, 7, lPower, mCameraOrientation.x);
#endif
			const float lSteeringChange = mLastSteering-s;
			if (lSteeringChange > 0.5f)
			{
				mInputExpireAlarm.Set();
			}
			else if (!Math::IsEpsEqual(lSteeringChange, 0.0f, 0.01f))
			{
				mInputExpireAlarm.Push(0.1f);
			}
			mLastSteering = s;

			// Control camera.
			const float lScale = 50.0f * GetTimeManager()->GetAffordedPhysicsTotalTime();
			const Life::Options::CamControl& c = mOptions.GetCamControl();
#define C(dir) c.mControl[Life::Options::CamControl::CAMDIR_##dir]
			float lCamPower = C(UP)-C(DOWN);
			CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, double, +, lCamPower*lScale, -5.0, 30.0);
			mCamRotateExtra = (C(RIGHT)-C(LEFT)) * lScale;
			lCamPower = C(BACKWARD)-C(FORWARD);
			CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, double, +, lCamPower*lScale, 3.0, 100.0);

			// Control fire.
			const Life::Options::FireControl& f = mOptions.GetFireControl();
#define F(alt) f.mControl[Life::Options::FireControl::FIRE##alt]
			if (F(0) > 0.5f)
			{
				Fire();
			}

			mAvatarInvisibleCount = 0;
		}
		else if (++mAvatarInvisibleCount > 60)
		{
			SetRoadSignsVisible(true);
			mAvatarInvisibleCount = -100000;
		}
	}
}

bool PushManager::SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower, float pAngle)
{
	bool lSet = pAvatar->SetEnginePower(pAspect, pPower, pAngle);

	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	if (lPlaybackMode == PLAYBACK_RECORD)
	{
		if (!Math::IsEpsEqual(mEnginePowerShadow[pAspect].mPower, pPower)
			//|| !Math::IsEpsEqual(mEnginePowerShadow[pAspect].mAngle, pAngle, 0.3f)
			)
		{
			mEnginePowerShadow[pAspect].mPower = pPower;
			mEnginePowerShadow[pAspect].mAngle = pAngle;
			if (!mEnginePlaybackFile.IsOpen())
			{
				mEnginePlaybackFile.Open(_T("Data/Steering.rec"), DiskFile::MODE_TEXT_WRITE);
				wstr lComment = wstrutil::Format(L"// Recording %s at %s.\n", pAvatar->GetClassId().c_str(), Time().GetDateTimeAsString().c_str());
				mEnginePlaybackFile.WriteString(lComment);
				mEnginePlaybackFile.WriteString(wstrutil::Encode("#" RTVAR_STEERING_PLAYBACKMODE " 2\n"));
			}
			const float lTime = GetTimeManager()->GetAbsoluteTime();
			if (lTime != mEnginePlaybackTime)
			{
				wstr lCommand = wstrutil::Format(L"sleep %g\n", Cure::TimeManager::GetAbsoluteTimeDiff(lTime, mEnginePlaybackTime));
				mEnginePlaybackFile.WriteString(lCommand);
				mEnginePlaybackTime = lTime;
			}
			wstr lCommand = wstrutil::Format(L"set-avatar-engine-power %u %g %g\n", pAspect, pPower, pAngle);
			mEnginePlaybackFile.WriteString(lCommand);
		}
	}
	else if (lPlaybackMode == PLAYBACK_NONE)
	{
		if (mEnginePlaybackFile.IsOpen())
		{
			if (mEnginePlaybackFile.IsInMode(File::WRITE_MODE))
			{
				mEnginePlaybackFile.WriteString(wstrutil::Encode("#" RTVAR_STEERING_PLAYBACKMODE " 0\n"));
			}
			mEnginePlaybackFile.Close();
		}
		mEnginePlaybackTime = GetTimeManager()->GetAbsoluteTime();
		mEnginePowerShadow[pAspect].mPower = 0;
		mEnginePowerShadow[pAspect].mAngle = 0;
	}

	return lSet;
}

void PushManager::TickUiUpdate()
{
	((PushConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();

	mCollisionSoundManager->Tick(mCameraPosition);

	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime < 1e-5)
	{
		return;
	}

#if 1
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		mCameraPivotPosition = lObject->GetPosition();
		UpdateMassObjects(mCameraPivotPosition);

		const Vector3DF lForward3d = lObject->GetForwardDirection();
		//const Vector3DF lRight3d = lForward3d.Cross(Vector3DF(0, 0, 1));
		Vector3DF lBackward2d = -lForward3d.ProjectOntoPlane(Vector3DF(0, 0, 1));
		//Vector3DF lBackward2d = lRight3d.ProjectOntoPlane(Vector3DF(0, 0, 1));
		lBackward2d.Normalize(mCameraTargetXyDistance);
		float lCamHeight;
		CURE_RTVAR_GET(lCamHeight, =(float), GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
		lBackward2d.z = lCamHeight;
		mCameraPreviousPosition = mCameraPosition;
		mCameraPosition = Math::Lerp(mCameraPosition, mCameraPivotPosition + lBackward2d, 0.2f);
	}

	Vector3DF lPivotXyPosition = mCameraPivotPosition;
	lPivotXyPosition.z = mCameraPosition.z;
	const float lNewTargetCameraXyDistance = mCameraPosition.GetDistance(lPivotXyPosition);
	Vector3DF lTargetCameraOrientation(::asin((mCameraPosition.x-lPivotXyPosition.x)/lNewTargetCameraXyDistance) + PIF/2, 4*PIF/7, 0);
	if (lPivotXyPosition.y-mCameraPosition.y < 0)
	{
		lTargetCameraOrientation.x = -lTargetCameraOrientation.x;
	}
	Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
	Math::RangeAngles(mCameraOrientation.y, lTargetCameraOrientation.y);
	float lYawChange = (lTargetCameraOrientation.x-mCameraOrientation.x) * 0.5f;
	lYawChange = Math::Clamp(lYawChange, -PIF*3/7, +PIF*3/7);
	lTargetCameraOrientation.z = -lYawChange;
	Math::RangeAngles(mCameraOrientation.z, lTargetCameraOrientation.z);
	mCameraOrientation = Math::Lerp<Vector3DF, float>(mCameraOrientation, lTargetCameraOrientation, 0.4f);

	float lRotationFactor;
	CURE_RTVAR_GET(lRotationFactor, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	lRotationFactor += mCamRotateExtra;
	if (lRotationFactor)
	{
		mCameraPivotVelocity.x += lRotationFactor;
		TransformationF lTransform(GetCameraQuaternion(), mCameraPosition);
		lTransform.RotateAroundAnchor(mCameraPivotPosition, Vector3DF(0, 0, 1), mCameraPivotVelocity.x * lPhysicsTime);
		mCameraPosition = lTransform.GetPosition();
		float lTheta;
		float lPhi;
		float lGimbal;
		lTransform.GetOrientation().GetEulerAngles(lTheta, lPhi, lGimbal);
		mCameraOrientation.x = lTheta+PIF/2;
		mCameraOrientation.y = PIF/2-lPhi;
		mCameraOrientation.z = lGimbal;
	}
#else
	// TODO: remove camera hack (camera position should be context object controlled).
	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	Vector3DF lAvatarPosition = mCameraPivotPosition;
	float lCameraPivotSpeed = 0;
	if (lObject)
	{
		// Target position is <cam> distance from the avatar along a straight line
		// (in the XY plane) to where the camera currently is.
		lAvatarPosition = lObject->GetPosition();
		mCameraPivotPosition = lAvatarPosition;
		Vector3DF lAvatarVelocity = lObject->GetVelocity();
		lAvatarVelocity.z *= 0.2f;	// Don't take very much action on the up/down speed, that has it's own algo.
		mCameraPivotVelocity = Math::Lerp(mCameraPivotVelocity, lAvatarVelocity, 0.5f*lPhysicsTime/0.1f);
		mCameraPivotPosition += mCameraPivotVelocity * 0.6f;	// Look to where the avatar will be in a while.
		lCameraPivotSpeed = mCameraPivotVelocity.GetLength();

		UpdateMassObjects(mCameraPivotPosition);
	}
	const Vector3DF lPivotXyPosition(mCameraPivotPosition.x, mCameraPivotPosition.y, mCameraPosition.z);
	Vector3DF lTargetCameraPosition(mCameraPosition-lPivotXyPosition);
	const float lCurrentCameraXyDistance = lTargetCameraPosition.GetLength();
	const float lSpeedDependantCameraXyDistance = mCameraTargetXyDistance + lCameraPivotSpeed*0.6f;
	lTargetCameraPosition = lPivotXyPosition + lTargetCameraPosition*(lSpeedDependantCameraXyDistance/lCurrentCameraXyDistance);
	float lCamHeight;
	CURE_RTVAR_GET(lCamHeight, =(float), GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	lTargetCameraPosition.z = mCameraPivotPosition.z + lCamHeight;

	if (lObject)
	{
		/* Almost tried out "stay behind velocity". Was too jerky, since velocity varies too much.
		Vector3DF lVelocity = lObject->GetVelocity();
		mCameraFollowVelocity = lVelocity;
		float lSpeed = lVelocity.GetLength();
		if (lSpeed > 0.1f)
		{
			lVelocity.Normalize();
			mCameraFollowVelocity = Math::Lerp(mCameraFollowVelocity, lVelocity, 0.1f).GetNormalized();
		}
		// Project previous "camera up" onto plane orthogonal to the velocity to get new "up".
		Vector3DF lCameraUp = mCameraUp.ProjectOntoPlane(mCameraFollowVelocity) + Vector3DF(0, 0, 0.01f);
		if (lCameraUp.GetLengthSquared() > 0.1f)
		{
			mCameraUp = lCameraUp;
		}
		lSpeed *= 0.05f;
		lSpeed = (lSpeed > 0.4f)? 0.4f : lSpeed;
		mCameraUp.Normalize();
		lTargetCameraPosition = Math::Lerp(lTargetCameraPosition, mCameraPivotPosition - 
			mCameraFollowVelocity * mCameraTargetXyDistance +
			mCameraUp * mCameraTargetXyDistance * 0.3f, 0.0f);*/

		/*// Temporary: changed to "cam stay behind" mode.
		lTargetCameraPosition = lObject->GetOrientation() *
			Vector3DF(0, -mCameraTargetXyDistance, mCameraTargetXyDistance/4) +
			mCameraPivotPosition;*/
	}

	lTargetCameraPosition.x = Math::Clamp(lTargetCameraPosition.x, -1000.0f, 1000.0f);
	lTargetCameraPosition.y = Math::Clamp(lTargetCameraPosition.y, -1000.0f, 1000.0f);
	lTargetCameraPosition.z = Math::Clamp(lTargetCameraPosition.z, -20.0f, 200.0f);

	// Now that we've settled where we should be, it's time to check where we actually can see our avatar.
	// TODO: currently only checks against terrain. Add a ray to world, that we can use for this kinda thing.
	if (mLevel)
	{
		const float lCameraAboveGround = 0.3f;
		lTargetCameraPosition.z -= lCameraAboveGround;
		const TBC::PhysicsManager::BodyID lTerrainBodyId = mLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		Vector3DF lCollisionPoint;
		float lStepSize = (lTargetCameraPosition - lAvatarPosition).GetLength() * 0.5f;
		for (int y = 0; y < 5; ++y)
		{
			int x;
			for (x = 0; x < 2; ++x)
			{
				const Vector3DF lRay = lTargetCameraPosition - lAvatarPosition;
				const bool lIsCollision = (GetPhysicsManager()->QueryRayCollisionAgainst(
					lAvatarPosition, lRay, lRay.GetLength(), lTerrainBodyId, &lCollisionPoint, 1) > 0);
				if (lIsCollision)
				{
					lTargetCameraPosition.z += lStepSize;
				}
				else
				{
					if (x != 0)
					{
						lTargetCameraPosition.z -= lStepSize;
					}
					break;
				}
			}
			if (x == 0 && y == 0)
			{
				break;
			}
			lStepSize *= 1/3.0f;
			//lTargetCameraPosition.z += lStepSize;
		}
		lTargetCameraPosition.z += lCameraAboveGround;
	}

	const float lHalfDistanceTime = 0.1f;	// Time it takes to half the distance from where it is now to where it should be.
	float lMovingAveragePart = 0.5f*lPhysicsTime/lHalfDistanceTime;
	if (lMovingAveragePart > 0.8f)
	{
		lMovingAveragePart = 0.8f;
	}
	//lMovingAveragePart = 1;
	const Vector3DF lNewPosition = Math::Lerp<Vector3DF, float>(mCameraPosition,
		lTargetCameraPosition, lMovingAveragePart);
	const Vector3DF lDirection = lNewPosition-mCameraPosition;
	const float lDistance = lDirection.GetLength();
	if (lDistance > mCameraMaxSpeed*lPhysicsTime)
	{
		mCameraPosition += lDirection*(mCameraMaxSpeed*lPhysicsTime/lDistance);
	}
	else
	{
		mCameraPosition = lNewPosition;
	}
	if (lNewPosition.z > mCameraPosition.z)	// Dolly cam up pretty quick to avoid looking "through the ground."
	{
		mCameraPosition.z = Math::Lerp(mCameraPosition.z, lNewPosition.z, lHalfDistanceTime);
	}

	// "Roll" camera towards avatar.
	const float lNewTargetCameraXyDistance = mCameraPosition.GetDistance(lPivotXyPosition);
	const float lNewTargetCameraDistance = mCameraPosition.GetDistance(mCameraPivotPosition);
	Vector3DF lTargetCameraOrientation;
	lTargetCameraOrientation.Set(::asin((mCameraPosition.x-lPivotXyPosition.x)/lNewTargetCameraXyDistance) + PIF/2,
		::acos((mCameraPivotPosition.z-mCameraPosition.z)/lNewTargetCameraDistance), 0);
	if (lPivotXyPosition.y-mCameraPosition.y < 0)
	{
		lTargetCameraOrientation.x = -lTargetCameraOrientation.x;
	}
	Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
	float lYawChange = (lTargetCameraOrientation.x-mCameraOrientation.x)*3;
	lYawChange = Math::Clamp(lYawChange, -PIF*3/7, +PIF*3/7);
	lTargetCameraOrientation.z = -lYawChange;
	Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
	Math::RangeAngles(mCameraOrientation.y, lTargetCameraOrientation.y);
	Math::RangeAngles(mCameraOrientation.z, lTargetCameraOrientation.z);
	mCameraOrientation = Math::Lerp<Vector3DF, float>(mCameraOrientation, lTargetCameraOrientation, lMovingAveragePart);

	float lRotationFactor;
	CURE_RTVAR_GET(lRotationFactor, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	lRotationFactor += mCamRotateExtra;
	if (lRotationFactor)
	{
		TransformationF lTransform(GetCameraQuaternion(), mCameraPosition);
		lTransform.RotateAroundAnchor(mCameraPivotPosition, Vector3DF(0, 0, 1), lRotationFactor * lPhysicsTime);
		mCameraPosition = lTransform.GetPosition();
		float lTheta;
		float lPhi;
		float lGimbal;
		lTransform.GetOrientation().GetEulerAngles(lTheta, lPhi, lGimbal);
		mCameraOrientation.x = lTheta+PIF/2;
		mCameraOrientation.y = PIF/2-lPhi;
		mCameraOrientation.z = lGimbal;
	}
#endif
}

bool PushManager::UpdateMassObjects(const Vector3DF& pPosition)
{
	bool lOk = true;

	if (mLevel && mMassObjectArray.empty())
	{
#if 0
		const TBC::PhysicsManager::BodyID lTerrainBodyId = mLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		if (lOk)
		{
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			Cure::ContextObject* lFlowers = new Life::MassObject(GetResourceManager(), _T("flower"), mUiManager, lTerrainBodyId, 600, 170);
			AddContextObject(lFlowers, Cure::NETWORK_OBJECT_LOCAL_ONLY, lMassObjectId);
			lFlowers->StartLoading();
		}
		if (lOk)
		{
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			Cure::ContextObject* lBushes = new Life::MassObject(GetResourceManager(), _T("bush_01"), mUiManager, lTerrainBodyId, 150, 290);
			AddContextObject(lBushes, Cure::NETWORK_OBJECT_LOCAL_ONLY, lMassObjectId);
			lBushes->StartLoading();
		}
#endif 
	}

	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		Life::MassObject* lObject = (Life::MassObject*)GetContext()->GetObject(*x, true);
		assert(lObject);
		lObject->SetRootPosition(pPosition);
	}
	return lOk;
}

void PushManager::SetLocalRender(bool pRender)
{
	if (pRender)
	{
		// Update light and sun according to this slave's camera.
		Sunlight* lSunlight = ((PushTicker*)GetMaster())->GetSunlight();
		const float lSunDistance = 1700;
		mSun->SetRootPosition(mCameraPosition + lSunDistance * lSunlight->GetDirection());

		const float lCloudDistance = 600;
		size_t x = 0;
		for (; x < mCloudArray.size(); ++x)
		{
			Cure::ContextObject* lCloud = mCloudArray[x];
			float lTod = lSunlight->GetTimeOfDay();
			lTod += x / (float)mCloudArray.size();
			lTod *= 2 * PIF;
			const float x = sin(lTod*2) * lCloudDistance;
			const float y = cos(lTod) * lCloudDistance;
			const float z = cos(lTod*3) * lCloudDistance * 0.2f + lCloudDistance * 0.4f;
			lCloud->SetRootPosition(Vector3DF(x, y, z));
		}

		bool lMass;
		CURE_RTVAR_GET(lMass, =, GetVariableScope(), RTVAR_UI_3D_ENABLEMASSOBJECTS, false);
		SetMassRender(lMass);
	}
	else
	{
		SetMassRender(false);
	}
}

void PushManager::SetMassRender(bool pRender)
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



void PushManager::ProcessNetworkInputMessage(Cure::Message* pMessage)
{
	Parent::ProcessNetworkInputMessage(pMessage);

	Cure::MessageType lType = pMessage->GetType();
	switch (lType)
	{
		case Cure::MESSAGE_TYPE_DELETE_OBJECT:
		{
			Cure::MessageDeleteObject* lMessageDeleteObject = (Cure::MessageDeleteObject*)pMessage;
			Cure::GameObjectId lId = lMessageDeleteObject->GetObjectId();
			if (lId == mAvatarId)
			{
				DropAvatar();
			}
		}
		break;
	}
}

void PushManager::ProcessNetworkStatusMessage(Cure::MessageStatus* pMessage)
{
	switch (pMessage->GetInfo())
	{
		case Cure::MessageStatus::INFO_AVATAR:
		{
			wstr lAvatarName;
			pMessage->GetMessageString(lAvatarName);
			Cure::UserAccount::AvatarId lAvatarId = strutil::Encode(lAvatarName);
			log_adebug("Status: INFO_AVATAR...");
			str lTextureId = strutil::Format(_T("road_sign_%s.png"), lAvatarId.c_str());
			if (!GetResourceManager()->QueryFileExists(lTextureId))
			{
				lTextureId = _T("road_sign_car.png");
			}
			RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(),
				mUiManager, lAvatarId, _T("road_sign_01"), lTextureId, RoadSignButton::SHAPE_ROUND);
			GetContext()->AddLocalObject(lButton);
			const int SIGN_COUNT_X = 5;
			const int SIGN_COUNT_Y = 5;
			const float lDeltaX = 1 / (float)SIGN_COUNT_X;
			const float lDeltaY = 1 / (float)SIGN_COUNT_Y;
			const float x = (mRoadSignIndex % SIGN_COUNT_X) * lDeltaX - 0.5f + 0.5f*lDeltaX;
			const float y = (mRoadSignIndex / SIGN_COUNT_X) * lDeltaY - 0.5f + 0.5f*lDeltaY;
			++mRoadSignIndex;
			lButton->SetTrajectory(Vector2DF(x, y), 8);
			lButton->GetButton().SetOnClick(PushManager, OnAvatarSelect);
			mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
			lButton->StartLoading();
		}
		return;
	}
	Parent::ProcessNetworkStatusMessage(pMessage);
}

void PushManager::ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	switch (pType)
	{
		case Cure::MessageNumber::INFO_AVATAR:
		{
			mAvatarId = pInteger;
			mOwnedObjectList.insert(mAvatarId);
			mLog.Infof(_T("Got control over avatar with ID %i."), pInteger);
		}
		return;
	}
	Parent::ProcessNumber(pType, pInteger, pFloat);
}

Cure::ContextObject* PushManager::CreateContextObject(const str& pClassId) const
{
	Cure::CppContextObject* lObject;
	if (pClassId == _T("stone") || pClassId == _T("cube"))
	{
		lObject = new UiCure::CppContextObject(GetResourceManager(), pClassId, mUiManager);
	}
	else if (pClassId == _T("grenade"))
	{
		lObject = new Grenade(GetResourceManager(), mUiManager, (Launcher*)this);
	}
	else
	{
		UiCure::Machine* lMachine = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
		lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager, _T("mud_particle_01"), 3, 0.6f, 2.0f));
		lObject = lMachine;
	}
	lObject->SetAllowNetworkLogic(false);	// Only server gets to control logic.
	return (lObject);
}

void PushManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		if (pObject->GetInstanceId() == mAvatarId)
		{
			log_volatile(mLog.Debug(_T("Yeeha! Loaded avatar!")));
		}
		else
		{
			log_volatile(mLog.Tracef(_T("Loaded object %s."), pObject->GetClassId().c_str()));
		}
		pObject->GetPhysics()->UpdateBonesObjectTransformation(0, gIdentityTransformationF);
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void PushManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 200, false);

	const bool lObject1Dynamic = (pObject1->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	const bool lObject2Dynamic = (pObject2->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	if (!lObject1Dynamic || !lObject2Dynamic)
	{
		return;
	}

	if (IsOwned(pObject1->GetInstanceId()))
	{
		if (pObject1->GetImpact(GetPhysicsManager()->GetGravity(), pForce, pTorque) >= 2.0f)
		{
			pObject1->QueryResendTime(0, false);
		}
		mCollisionExpireAlarm.SetIfNotSet();
	}
	else if (pObject2->GetInstanceId() == mAvatarId &&
		pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_REMOTE_CONTROLLED)
	{
		if (!GetMaster()->IsLocalObject(pObject1->GetInstanceId()) &&
			pObject1->GetImpact(GetPhysicsManager()->GetGravity(), pForce, pTorque) >= 0.5f)
		{
			if (pObject1->QueryResendTime(1.0, false))
			{
				GetNetworkAgent()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
					Cure::MessageNumber::INFO_REQUEST_LOAN, pObject1->GetInstanceId(), 0, 0);
				log_adebug("Sending loan request to server.");
			}
		}
	}
}



void PushManager::OnFireButton(UiTbc::Button*)
{
	Fire();
}

void PushManager::Fire()
{
	if (mFireTimeout.QueryTimeDiff() < 0.2f)
	{
		return;
	
	}

	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (!lAvatar)
	{
		return;
	}

	mFireTimeout.ClearTimeDiff();
	GetNetworkClient()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
						Cure::MessageNumber::INFO_APPLICATION_0, 0, 0);
}

void PushManager::GetBarrel(TransformationF& pTransform, Vector3DF& pVelocity) const
{
	PushBarrel::GetInfo(this, mAvatarId, pTransform, pVelocity);
}



void PushManager::CancelLogin()
{
	CloseLoginGui();
	SetIsQuitting();
}

void PushManager::OnVehicleSelect(UiTbc::Button* pButton)
{
	(void)pButton;
	SetRoadSignsVisible(true);
}

void PushManager::OnAvatarSelect(UiTbc::Button* pButton)
{
	Cure::UserAccount::AvatarId lAvatarId = pButton->GetName();
	SelectAvatar(lAvatarId);

	if (!mPickVehicleButton)
	{
		mPickVehicleButton = new RoadSignButton(this, GetResourceManager(), mUiManager, _T("PickVehicle"),
			_T("road_sign_01"), _T("road_sign_car.png"), RoadSignButton::SHAPE_ROUND);
		GetContext()->AddLocalObject(mPickVehicleButton);
		mPickVehicleButton->SetTrajectory(Vector2DF(0, 0.45f), 20);
		mPickVehicleButton->SetTrajectoryAngle(-PIF/2);
		mPickVehicleButton->GetButton().SetOnClick(PushManager, OnVehicleSelect);
		mPickVehicleButton->StartLoading();
	}
	mPickVehicleButton->SetIsMovingIn(true);
}

void PushManager::DropAvatar()
{
	mOwnedObjectList.erase(mAvatarId);
	mAvatarId = 0;
	mHadAvatar = false;
}



void PushManager::DrawStick(Touchstick* pStick)
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



void PushManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	TransformationF lCameraTransform(GetCameraQuaternion(), mCameraPosition);
	mUiManager->SetCameraPosition(lCameraTransform);
	if (pUpdateMicPosition)
	{
		const float lFrameTime = GetTimeManager()->GetNormalFrameTime();
		if (lFrameTime > 1e-4)
		{
			Vector3DF lVelocity = (mCameraPosition-mCameraPreviousPosition) / lFrameTime;
			const float lMicrophoneMaxVelocity = 100.0f;
			if (lVelocity.GetLength() > lMicrophoneMaxVelocity)
			{
				lVelocity.Normalize(lMicrophoneMaxVelocity);
			}
			const float lLerpTime = Math::GetIterateLerpTime(0.9f, lFrameTime);
			mMicrophoneSpeed = Math::Lerp(mMicrophoneSpeed, lVelocity, lLerpTime);
			mUiManager->SetMicrophonePosition(lCameraTransform, mMicrophoneSpeed);
		}
	}
}

QuaternionF PushManager::GetCameraQuaternion() const
{
	const float lTheta = mCameraOrientation.x;
	const float lPhi = mCameraOrientation.y;
	const float lGimbal = mCameraOrientation.z;
	QuaternionF lOrientation;
	lOrientation.SetEulerAngles(lTheta-PIF/2, PIF/2-lPhi, lGimbal);

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	int lIndex = 0;
	int lCount = 0;
	GetMaster()->GetSlaveInfo(this, lIndex, lCount);
	if (lCount == 2)
	{
		if (lIndex == 0)
		{
			lOrientation.RotateAroundOwnY(-PIF/2);
		}
		else
		{
			lOrientation.RotateAroundOwnY(+PIF/2);
		}
	}
#endif // Touch or emulated touch.

	return (lOrientation);
}



LOG_CLASS_DEFINE(GAME, PushManager);



}
