
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "HoverTankManager.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/IntAttribute.h"
#include "../Cure/Include/NetworkClient.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
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
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiExhaustEmitter.h"
#include "../UiCure/Include/UiJetEngineEmitter.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiLepra/Include/UiTouchstick.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "HoverTankConsoleManager.h"
#include "HoverTankTicker.h"
#include "RoadSignButton.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "Version.h"

#define ICONBTN(i,n)			new UiCure::IconButton(mUiManager, GetResourceManager(), i, n)
#define ICONBTNA(i,n)			ICONBTN(_T(i), _T(n))



namespace HoverTank
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



HoverTankManager::HoverTankManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mAvatarId(0),
	mHadAvatar(false),
	mUpdateCameraForAvatar(false),
	mCamRotateExtra(0),
	mActiveWeapon(0),
	mPickVehicleButton(0),
	mAvatarInvisibleCount(0),
	mRoadSignIndex(0),
	mLevelId(0),
	mLevel(0),
	mSun(0),
	mScoreInfoId(0),
	mCameraPosition(0, -200, 100),
	mCameraUp(0, 0, 1),
	mCameraOrientation(PIF/2, acos(mCameraPosition.z/mCameraPosition.y), 0),
	mCameraTargetXyDistance(20),
	mCameraMaxSpeed(500),
	mCameraMouseAngle(0),
	mCameraTargetAngle(0),
	mCameraTargetAngleFactor(0),
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

	SetConsoleManager(new HoverTankConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));
}

HoverTankManager::~HoverTankManager()
{
	Close();

	delete mStickLeft;
	mStickLeft = 0;
	delete mStickRight;
	mStickRight = 0;
}

void HoverTankManager::LoadSettings()
{
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_GAME_DRAWSCORE, false);

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

#ifdef LEPRA_TOUCH
	// TODO: remove hard-coding!
	//CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, _T("pixeldoctrine.dyndns.org:16650"));
	//CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, true);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
#endif // Touch device
}

void HoverTankManager::SaveSettings()
{
#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
#endif // Computer or touch device.
}

void HoverTankManager::SetRenderArea(const PixelRect& pRenderArea)
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

bool HoverTankManager::Open()
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
		mFireButton->SetOnClick(HoverTankManager, OnFireButton);
	}
#endif // Touch or emulated touch.
	return lOk;
}

void HoverTankManager::Close()
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

void HoverTankManager::SetIsQuitting()
{
	CloseLoginGui();
	((HoverTankConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	SetRoadSignsVisible(false);

	Parent::SetIsQuitting();
}

void HoverTankManager::SetFade(float pFadeAmount)
{
	mCameraMaxSpeed = 100000.0f;
	float lBaseDistance;
	CURE_RTVAR_GET(lBaseDistance, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	mCameraTargetXyDistance = lBaseDistance + pFadeAmount*400.0f;
}



bool HoverTankManager::Paint()
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
		Cure::FloatAttribute* lHealth = Cure::Health::GetAttribute(lObject);
		const str lInfo = lHealth? strutil::DoubleToString(lHealth->GetValue()*100, 0) : _T("");
		mUiManager->GetPainter()->SetColor(Color(255, 0, 0, 255), 0);
		mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		mUiManager->GetPainter()->PrintText(lInfo, mRenderArea.mLeft + 10, 10);
	}

	bool lDrawScore;
	CURE_RTVAR_GET(lDrawScore, =, GetVariableScope(), RTVAR_GAME_DRAWSCORE, false);
	if (lDrawScore)
	{
		DrawScore();
	}
	return true;
}



void HoverTankManager::RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken)
{
	ScopeLock lLock(GetTickLock());
	CloseLoginGui();
	Parent::RequestLogin(pServerAddress, pLoginToken);
}

void HoverTankManager::OnLoginSuccess()
{
	ClearRoadSigns();
}



void HoverTankManager::SelectAvatar(const Cure::UserAccount::AvatarId& pAvatarId)
{
	DropAvatar();

	log_volatile(mLog.Debugf(_T("Clicked avatar %s."), pAvatarId.c_str()));
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendStatusMessage(GetNetworkClient()->GetSocket(), 0, Cure::REMOTE_OK,
		Cure::MessageStatus::INFO_AVATAR, wstrutil::Encode(pAvatarId), lPacket);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	SetRoadSignsVisible(false);
}

void HoverTankManager::AddLocalObjects(std::unordered_set<Cure::GameObjectId>& pLocalObjectSet)
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

bool HoverTankManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraPosition) <= pDistance*pDistance);
}

Cure::GameObjectId HoverTankManager::GetAvatarInstanceId() const
{
	return mAvatarId;
}



bool HoverTankManager::SetAvatarEnginePower(unsigned pAspect, float pPower)
{
	deb_assert(pAspect >= 0 && pAspect < TBC::PhysicsEngine::ASPECT_COUNT);
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		return SetAvatarEnginePower(lObject, pAspect, pPower);
	}
	return false;
}



void HoverTankManager::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
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
	lParticleRenderer->CreateExplosion(pPosition, pStrength * 1.5f, u, 1, 1, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles*2, lParticles*2, lParticles, lParticles/2);

	/*if (!GetMaster()->IsLocalServer())	// If local server, it will already have given us a hover_tank.
	{
		const Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
			Explosion::HoverTankObject(GetPhysicsManager(), lObject, pPosition, 1.0f);
		}
	}*/
}

void HoverTankManager::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	(void)pHitObject;

	TBC::ChunkyPhysics* lPhysics = pBullet->GetPhysics();
	if (lPhysics)
	{
		TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(0);
		mCollisionSoundManager->OnCollision(5.0f, pBullet->GetPosition(), lGeometry, lGeometry->GetMaterial());
	}
}

Cure::RuntimeVariableScope* HoverTankManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool HoverTankManager::Reset()	// Run when disconnected. Removes all objects and displays login GUI.
{
	ScopeLock lLock(GetTickLock());
	mScoreInfoId = 0;
	ClearRoadSigns();
	bool lOk = Parent::Reset();
	if (lOk)
	{
		CreateLoginView();
	}
	return (lOk);
}

void HoverTankManager::CreateLoginView()
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

bool HoverTankManager::InitializeUniverse()
{
	mSun = 0;
	mCloudArray.clear();

	bool lOk = true;
	if (lOk)
	{
		mSun = new UiCure::Props(GetResourceManager(), _T("sun"), mUiManager);
		AddContextObject(mSun, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mSun != 0);
		deb_assert(lOk);
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

void HoverTankManager::CloseLoginGui()
{
	if (mLoginWindow)
	{
		ScopeLock lLock(GetTickLock());
		mUiManager->GetDesktopWindow()->RemoveChild(mLoginWindow, 0);
		delete (mLoginWindow);
		mLoginWindow = 0;
	}
}

void HoverTankManager::ClearRoadSigns()
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

void HoverTankManager::SetRoadSignsVisible(bool pVisible)
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



void HoverTankManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void HoverTankManager::UpdateTouchstickPlacement()
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
		const int lMinimumTouchRadius = (int)(lScreenPixelWidth*lTouchScale*0.17f);	// 30 pixels in iPhone classic.
		mStickLeft  = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10),  0, lMinimumTouchRadius);
		const str lLeftName = strutil::Format(_T("TouchstickLeft%i"), mSlaveIndex);
		mStickLeft->SetUniqueIdentifier(lLeftName);
		mStickRight = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10), 0, lMinimumTouchRadius);
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

void HoverTankManager::TickUiInput()
{
	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPlaybackMode != PLAYBACK_PLAY && lPhysicsStepCount > 0 && mAllowMovementInput)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);

		// Show billboard.
		CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_GAME_DRAWSCORE, !lObject || mOptions.GetShowScore());

		if (lObject)
		{
			float lChildishness;
			CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
			lObject->QuerySetChildishness(lChildishness);

			// Control steering.
			const Life::Options::Steering& s = mOptions.GetSteeringControl();
#define S(dir) s.mControl[Life::Options::Steering::CONTROL_##dir]
			Vector2DF lLeftPower(S(RIGHT)-S(LEFT), S(FORWARD)-S(BRAKEANDBACK));
			Vector2DF lRightPower(S(RIGHT3D)-S(LEFT3D), S(FORWARD3D)-S(BACKWARD3D));
			if (lLeftPower*lRightPower > 0.2f)
			{
				// Pointing somewhat in the same direction, so let's assume that's what the user is trying to accomplish.
				lLeftPower = (lLeftPower+lRightPower) * 0.5f;
				lRightPower = lLeftPower;
			}
			/*else if (std::abs(lLeftPower.x) > std::abs(lLeftPower.y)*2 && lRightPower.GetLengthSquared() < 0.05f)
			{
				// Left stick moving left/right. Simulate left or right rotation instead of the logically expected one-sided strafe.
				lLeftPower.y = lLeftPower.x;
				lRightPower.y = -lLeftPower.x;
				lLeftPower.x = 0;
			}
			else if (std::abs(lRightPower.x) > std::abs(lRightPower.y)*2 && lLeftPower.GetLengthSquared() < 0.05f)
			{
				// Right stick moving left/right. Simulate left or right rotation instead of the logically expected one-sided strafe.
				lRightPower.y = -lRightPower.x;
				lLeftPower.y = lRightPower.x;
				lRightPower.x = 0;
			}*/
			//const float lSteeringPower = std::abs(lLeftPowerFwdRev);
			//lRightPowerLR *= Math::Lerp(0.8f, 2.0f, lSteeringPower);
			deb_assert(lLeftPower.y  >= -3 && lLeftPower.y  <= +3);
			deb_assert(lRightPower.y >= -3 && lRightPower.y <= +3);
			deb_assert(lLeftPower.x  >= -3 &&  lLeftPower.x <= +3);
			deb_assert(lRightPower.x >= -3 && lRightPower.y <= +3);

			// Mouse controls yaw angle.
			const float lAngleDelta = S(YAW_ANGLE) * 0.05f;
			const Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
			const bool lIsUpdatingYaw = !!lAngleDelta;
			const bool lIsTimedYawUpdate = (mCameraMouseAngleTimer.QueryTimeDiff() < 1.5f);
			if (lAvatar && mUpdateCameraForAvatar)
			{
				mUpdateCameraForAvatar = false;
				float lCurrentAngle = 0;
				float _;
				lAvatar->GetOrientation().GetEulerAngles(lCurrentAngle, _, _);
				//lCurrentAngle = +PIF/2 - lCurrentAngle;
				//const Vector3DF fwd = lAvatar->GetForwardDirection();
				//mLog.Infof(_T("Setting cam from avatar fwd vec %f, %f, %f"), fwd.x, fwd.y, fwd.z);
				//const QuaternionF r = lAvatar->GetOrientation();
				//mLog.Infof(_T("Setting cam from Q=%f, %f, %f, %f"), r.mA, r.mB, r.mC, r.mD);
				mCameraMouseAngle = lCurrentAngle;
				mCameraTargetAngle = lCurrentAngle;
				mCameraOrientation.x = lCurrentAngle + PIF/2;
			}
			/*if (lAvatar)
			{
				static int pc = 0;
				if (++pc > 20)
				{
					pc = 0;
					//const Vector3DF fwd = lAvatar->GetForwardDirection();
					//mLog.Infof(_T("Avatar fwd vec %f, %f, %f"), fwd.x, fwd.y, fwd.z);
					//const QuaternionF q = lAvatar->GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
					//mLog.Infof(_T("Avatar original Q=%f, %f, %f, %f"), q.mA, q.mB, q.mC, q.mD);
					const QuaternionF r = lAvatar->GetOrientation();
					mLog.Infof(_T("Avatar Q=%f, %f, %f, %f"), r.mA, r.mB, r.mC, r.mD);
				}
			}*/
			if (lAvatar && (lIsUpdatingYaw || lIsTimedYawUpdate))
			{
				float lCurrentAngle = 0;
				float _;
				lAvatar->GetOrientation().GetEulerAngles(lCurrentAngle, _, _);
				const bool lIsFirstYawUpdate = !lIsTimedYawUpdate;
				if (lIsFirstYawUpdate)
				{
					mCameraMouseAngle = lCurrentAngle;
				}
				if (lIsUpdatingYaw)
				{
					mCameraMouseAngleTimer.ClearTimeDiff();
				}
				mCameraMouseAngle -= lAngleDelta;
				Math::RangeAngles(lCurrentAngle, mCameraMouseAngle);
				mCameraTargetAngle = mCameraMouseAngle;
				mCameraTargetAngleFactor = 1;
				float lAngleDiff = lCurrentAngle - mCameraMouseAngle;
				const Vector3DF lRotationVelocity = lAvatar->GetAngularVelocity();
				float lRotationFriction = -lRotationVelocity.z * 0.2f;
				if ((lAngleDelta < 0) == (lAngleDiff > 0))
				{
					lRotationFriction = 0;
				}
				/*static int pc = 0;
				if (++pc > 5)
				{
					pc = 0;
					mLog.Infof(_T("angle=%f, delta=%f, current=%f, diff=%f, friction=%f"), mCameraMouseAngle, lAngleDelta, lCurrentAngle, lCurrentAngle - mCameraMouseAngle, lRotationFriction);
				}*/
				const float lStrength = Math::Lerp(1.0f, 2.0f, lLeftPower.GetLength());
				lAngleDiff *= 4;
				lAngleDiff -= lRotationFriction * lStrength;
				lRightPower.x += lAngleDiff;
			}
			else
			{
				mCameraTargetAngleFactor *= 0.5f;
			}

			SetAvatarEnginePower(lObject, 0,  lLeftPower.y);
			SetAvatarEnginePower(lObject, 1,  lLeftPower.x);
			SetAvatarEnginePower(lObject, 4, lRightPower.y);
			SetAvatarEnginePower(lObject, 5, lRightPower.x);
			SetAvatarEnginePower(lObject, 8, lLeftPower.GetDistance(lRightPower)*0.5f);
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
				AvatarShoot();
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

bool HoverTankManager::SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower)
{
	bool lSet = pAvatar->SetEnginePower(pAspect, pPower);

	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	if (lPlaybackMode == PLAYBACK_RECORD)
	{
		if (!Math::IsEpsEqual(mEnginePowerShadow[pAspect].mPower, pPower)
			//|| !Math::IsEpsEqual(mEnginePowerShadow[pAspect].mAngle, pAngle, 0.3f)
			)
		{
			mEnginePowerShadow[pAspect].mPower = pPower;
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
			wstr lCommand = wstrutil::Format(L"set-avatar-engine-power %u %g\n", pAspect, pPower);
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
	}

	return lSet;
}

void HoverTankManager::TickUiUpdate()
{
	((HoverTankConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	mCollisionSoundManager->Tick(mCameraPosition);
}

bool HoverTankManager::UpdateMassObjects(const Vector3DF& pPosition)
{
	bool lOk = true;
	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		Life::MassObject* lObject = (Life::MassObject*)GetContext()->GetObject(*x, true);
		deb_assert(lObject);
		lObject->SetRootPosition(pPosition);
	}
	return lOk;
}

void HoverTankManager::SetLocalRender(bool pRender)
{
	if (pRender)
	{
		// Update light and sun according to this slave's camera.
		Sunlight* lSunlight = ((HoverTankTicker*)GetMaster())->GetSunlight();
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

void HoverTankManager::SetMassRender(bool pRender)
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



void HoverTankManager::ProcessNetworkInputMessage(Cure::Message* pMessage)
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
			mOwnedObjectList.erase(lId);
		}
		break;
	}
}

void HoverTankManager::ProcessNetworkStatusMessage(Cure::MessageStatus* pMessage)
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
			lButton->GetButton().SetOnClick(HoverTankManager, OnAvatarSelect);
			mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
			lButton->StartLoading();
		}
		return;
	}
	Parent::ProcessNetworkStatusMessage(pMessage);
}

void HoverTankManager::ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	switch (pType)
	{
		case Cure::MessageNumber::INFO_AVATAR:
		{
			mAvatarId = pInteger;
			mOwnedObjectList.insert(mAvatarId);
			mUpdateCameraForAvatar = true;
			mCameraMouseAngleTimer.ReduceTimeDiff(-10.0f);
			mCameraTargetAngleFactor = 0;
			log_volatile(mLog.Debugf(_T("Got control over avatar with ID %i."), pInteger));
		}
		return;
		case Cure::MessageNumber::INFO_FALL_APART:
		{
			const Cure::GameObjectId lInstanceId = pInteger;
			UiCure::CppContextObject* lObject = (UiCure::CppContextObject*)GetContext()->GetObject(lInstanceId);
			if (lObject)
			{
				Life::Explosion::FallApart(GetPhysicsManager(), lObject);
				lObject->CenterMeshes();
				log_volatile(mLog.Debugf(_T("Object %i falling apart."), pInteger));
			}
		}
		return;
		case Cure::MessageNumber::INFO_TOOL_0:
		{
			const Cure::GameObjectId lAvatarId = pInteger;
			UiCure::CppContextObject* lAvatar = (UiCure::CppContextObject*)GetContext()->GetObject(lAvatarId);
			if (lAvatar)
			{
				Shoot(lAvatar, (int)pFloat);
			}
		}
		return;
	}
	Parent::ProcessNumber(pType, pInteger, pFloat);
}

Cure::ContextObject* HoverTankManager::CreateContextObject(const str& pClassId) const
{
	Cure::CppContextObject* lObject;
	if (pClassId == _T("grenade") || pClassId == _T("rocket"))
	{
		lObject = new Life::FastProjectile(GetResourceManager(), pClassId, mUiManager, (HoverTankManager*)this);
	}
	else if (pClassId == _T("bomb"))
	{
		lObject = new Life::Projectile(GetResourceManager(), pClassId, mUiManager, (HoverTankManager*)this);
	}
	else if (strutil::StartsWith(pClassId, _T("mine")))
	{
		lObject = new Life::Mine(GetResourceManager(), pClassId, mUiManager, (HoverTankManager*)this);
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
	else if (pClassId == _T("score_info"))
	{
		lObject = new UiCure::CppContextObject(GetResourceManager(), _T("score_info"), mUiManager);
		lObject->SetLoadResult(true);
	}
	else if (strutil::StartsWith(pClassId, _T("hover_tank")) ||
		strutil::StartsWith(pClassId, _T("deltawing")))
	{
		UiCure::Machine* lMachine = new Life::ExplodingMachine(GetResourceManager(), pClassId, mUiManager, (HoverTankManager*)this);
		lMachine->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	else
	{
		UiCure::Machine* lMachine = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
		lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	lObject->SetAllowNetworkLogic(false);	// Only server gets to control logic.
	return (lObject);
}

void HoverTankManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
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
		((UiCure::CppContextObject*)pObject)->UiMove();
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void HoverTankManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
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



void HoverTankManager::OnFireButton(UiTbc::Button*)
{
	AvatarShoot();
}

void HoverTankManager::AvatarShoot()
{
	if (mFireTimeout.QueryTimeDiff() < 0.15f)
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
		Cure::MessageNumber::INFO_TOOL_0, 0, (float)mActiveWeapon);

	if (mActiveWeapon == 0)
	{
		Shoot(lAvatar, mActiveWeapon);
	}

	++mActiveWeapon;
	mActiveWeapon %= 3;
}

void HoverTankManager::Shoot(Cure::ContextObject* pAvatar, int pWeapon)
{
	str lAmmo;
	switch (pWeapon)
	{
		case 0:	lAmmo = _T("bullet");	break;
		default: deb_assert(false); return;
	}
	Life::FastProjectile* lProjectile = new Life::FastProjectile(GetResourceManager(), lAmmo, mUiManager, this);
	AddContextObject(lProjectile, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	lProjectile->SetOwnerInstanceId(pAvatar->GetInstanceId());
	TransformationF t(pAvatar->GetOrientation(), pAvatar->GetPosition());
	lProjectile->SetInitialTransform(t);
	lProjectile->StartLoading();

	if (pWeapon >= 0)
	{
		UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
		TransformationF t;
		Vector3DF v;
		Life::ProjectileUtil::GetBarrel(lProjectile, t, v);
		lParticleRenderer->CreateFlare(Vector3DF(0.9f, 0.7f, 0.5f), 0.3f, 7.5f, t.GetPosition(), v);
	}
}



void HoverTankManager::CancelLogin()
{
	CloseLoginGui();
	SetIsQuitting();
}

void HoverTankManager::OnVehicleSelect(UiTbc::Button* pButton)
{
	(void)pButton;
	SetRoadSignsVisible(true);
}

void HoverTankManager::OnAvatarSelect(UiTbc::Button* pButton)
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
		mPickVehicleButton->GetButton().SetOnClick(HoverTankManager, OnVehicleSelect);
		mPickVehicleButton->StartLoading();
	}
	mPickVehicleButton->SetIsMovingIn(true);
}

void HoverTankManager::DropAvatar()
{
	mOwnedObjectList.erase(mAvatarId);
	mAvatarId = 0;
	mHadAvatar = false;
}



void HoverTankManager::DrawStick(Touchstick* pStick)
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

void HoverTankManager::DrawScore()
{
	typedef Cure::ContextObject::AttributeArray AttributeArray;
	if (!mScoreInfoId)
	{
		const Cure::ContextManager::ContextObjectTable& lObjectTable = GetContext()->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			Cure::ContextObject* lObject = x->second;
			const AttributeArray& lAttributeArray = lObject->GetAttributes();
			AttributeArray::const_iterator y = lAttributeArray.begin();
			for (; y != lAttributeArray.end(); ++y)
			{
				Cure::ContextObjectAttribute* lAttribute = *y;
				if (strutil::StartsWith(lAttribute->GetName(), _T("int_kills:")))
				{
					mScoreInfoId = lObject->GetInstanceId();
					return;	// Better luck next time.
				}
			}
		}
	}

	Cure::ContextObject* lScoreInfo = GetContext()->GetObject(mScoreInfoId);
	if (!lScoreInfo)
	{
		mScoreInfoId = 0;
		return;
	}

	typedef std::unordered_map<str, Score*> ScoreMap;
	typedef std::vector<Score> ScoreArray;
	ScoreMap lScoreMap;
	ScoreArray lScoreArray;
	const AttributeArray& lAttributeArray = lScoreInfo->GetAttributes();
	AttributeArray::const_iterator y = lAttributeArray.begin();
	for (; y != lAttributeArray.end(); ++y)
	{
		Cure::ContextObjectAttribute* lAttribute = *y;
		str lName;
		int lValue = 0;
		int lMode = 0;
		if (strutil::StartsWith(lAttribute->GetName(), _T("int_kills:")))
		{
			lName = lAttribute->GetName().substr(10);
			lValue = ((Cure::IntAttribute*)lAttribute)->GetValue();
			lMode = 0;
		}
		else if (strutil::StartsWith(lAttribute->GetName(), _T("int_deaths:")))
		{
			lName = lAttribute->GetName().substr(11);
			lValue = ((Cure::IntAttribute*)lAttribute)->GetValue();
			lMode = 1;
		}
		else if (strutil::StartsWith(lAttribute->GetName(), _T("int_ping:")))
		{
			lName = lAttribute->GetName().substr(9);
			lValue = ((Cure::IntAttribute*)lAttribute)->GetValue();
			lMode = 2;
		}
		if (!lName.empty())
		{
			ScoreMap::iterator x = lScoreMap.find(lName);
			if (x == lScoreMap.end())
			{
				Score s;
				s.mName = lName;
				s.mKills = 0;
				s.mDeaths = 0;
				s.mPing = 0;
				lScoreArray.push_back(s);
				x = lScoreMap.insert(ScoreMap::value_type(lName, &lScoreArray.back())).first;
			}
			switch (lMode)
			{
				case 0:	x->second->mKills	= lValue;	break;
				case 1:	x->second->mDeaths	= lValue;	break;
				case 2:	x->second->mPing	= lValue;	break;
			}
		}
	}
	std::sort(lScoreArray.begin(), lScoreArray.end(), gDeathsAscendingOrder);
	std::sort(lScoreArray.begin(), lScoreArray.end(), gKillsDecendingOrder);
	mUiManager->GetPainter()->SetTabSize(140);
	str lScore = _T("Name\tKills\tDeaths\tPing");
	ScoreArray::iterator x = lScoreArray.begin();
	for (; x != lScoreArray.end(); ++x)
	{
		lScore += strutil::Format(_T("\n%s\t%i\t%i\t%i"), x->mName.c_str(), x->mKills, x->mDeaths, x->mPing);
	}
	mUiManager->GetPainter()->SetColor(Color(1, 1, 1, 190), 0);
	mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
	mUiManager->GetPainter()->SetAlphaValue(140);
	int lHeight = mUiManager->GetFontManager()->GetStringHeight(lScore);
	mUiManager->GetPainter()->FillRect(mRenderArea.mLeft + 10, 30, mRenderArea.mLeft + 450, 30+lHeight+20);
	mUiManager->GetPainter()->SetColor(Color(140, 140, 140, 255), 0);
	mUiManager->GetPainter()->SetAlphaValue(255);
	mUiManager->GetPainter()->PrintText(lScore, mRenderArea.mLeft + 20, 40);
}



void HoverTankManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		MoveCamera();
		UpdateCameraPosition(false);
	}

	Parent::ScriptPhysicsTick();
}

void HoverTankManager::MoveCamera()
{
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		mCameraPivotPosition = lObject->GetPosition();
		UpdateMassObjects(mCameraPivotPosition);

		if (lObject->GetAngularVelocity().GetLengthSquared() > 30.0f)
		{
			Vector3DF lTarget = mCameraPivotPosition  - GetCameraQuaternion() * Vector3DF(0, mCameraTargetXyDistance, 0);
			mCameraPosition = Math::Lerp(mCameraPosition, lTarget, 0.2f);
			return;
		}

		const Vector3DF lForward3d = lObject->GetForwardDirection();
		//const Vector3DF lRight3d = lForward3d.Cross(Vector3DF(0, 0, 1));
		Vector3DF lBackward2d = -lForward3d.ProjectOntoPlane(Vector3DF(0, 0, 1));
		//Vector3DF lBackward2d = lRight3d.ProjectOntoPlane(Vector3DF(0, 0, 1));
		lBackward2d.Normalize(mCameraTargetXyDistance);
		float lCamHeight;
		CURE_RTVAR_GET(lCamHeight, =(float), GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
		lBackward2d.z = lCamHeight;
		{
			float lRotationFactor;
			CURE_RTVAR_GET(lRotationFactor, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
			lRotationFactor += mCamRotateExtra * 0.06f;
			mCameraPivotVelocity.x += lRotationFactor;
			lBackward2d = QuaternionF(mCameraPivotVelocity.x, Vector3DF(0,0,1)) * lBackward2d;
		}
		mCameraPreviousPosition = mCameraPosition;
		mCameraPosition = Math::Lerp(mCameraPosition, mCameraPivotPosition + lBackward2d, 0.4f);
	}

	Vector3DF lPivotXyPosition = mCameraPivotPosition;
	lPivotXyPosition.z = mCameraPosition.z;
	const float lNewTargetCameraXyDistance = mCameraPosition.GetDistance(lPivotXyPosition);
	Vector3DF lTargetCameraOrientation(::asin((mCameraPosition.x-lPivotXyPosition.x)/lNewTargetCameraXyDistance) + PIF/2, 4*PIF/7, 0);
	if (lPivotXyPosition.y-mCameraPosition.y < 0)
	{
		lTargetCameraOrientation.x = -lTargetCameraOrientation.x;
	}
	{
		float lCameraTargetAngle = mCameraTargetAngle + PIF/2;
		Math::RangeAngles(lTargetCameraOrientation.x, mCameraTargetAngle);
		lTargetCameraOrientation.x = Math::Lerp(lTargetCameraOrientation.x, lCameraTargetAngle, mCameraTargetAngleFactor);
		if (mCameraTargetAngleFactor < 0.1f)
		{
			mCameraTargetAngle = lTargetCameraOrientation.x - PIF/2;
		}
	}
	Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
	Math::RangeAngles(mCameraOrientation.y, lTargetCameraOrientation.y);
	float lYawChange = (lTargetCameraOrientation.x-mCameraOrientation.x) * 0.5f;
	lYawChange = Math::Clamp(lYawChange, -PIF*3/7, +PIF*3/7);
	lTargetCameraOrientation.z = -lYawChange;
	Math::RangeAngles(mCameraOrientation.z, lTargetCameraOrientation.z);
	mCameraOrientation = Math::Lerp<Vector3DF, float>(mCameraOrientation, lTargetCameraOrientation, 0.4f);
}

void HoverTankManager::UpdateCameraPosition(bool pUpdateMicPosition)
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

QuaternionF HoverTankManager::GetCameraQuaternion() const
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



LOG_CLASS_DEFINE(GAME, HoverTankManager);



}
