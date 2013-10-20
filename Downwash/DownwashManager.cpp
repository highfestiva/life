
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "DownwashManager.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/Driver.h"
#include "../Cure/Include/Elevator.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/IntAttribute.h"
#include "../Cure/Include/NetworkClient.h"
#include "../Cure/Include/Spawner.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Lepra/Include/Time.h"
#include "../Life/LifeClient/ExplodingMachine.h"
#include "../Life/LifeClient/FastProjectile.h"
#include "../Life/LifeClient/HomingProjectile.h"
#include "../Life/LifeClient/Level.h"
#include "../Life/LifeClient/MassObject.h"
#include "../Life/LifeClient/Menu.h"
#include "../Life/LifeClient/Mine.h"
#include "../Life/LifeClient/Projectile.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../Life/Explosion.h"
#include "../Life/ProjectileUtil.h"
#include "../Life/Spawner.h"
#include "../TBC/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiBurnEmitter.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiDebugRenderer.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiJetEngineEmitter.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../UiLepra/Include/UiTouchstick.h"
#include "../UiTBC/Include/GUI/UiCheckButton.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFixedLayouter.h"
#include "../UiTBC/Include/GUI/UiRadioButton.h"
#include "../UiTBC/Include/GUI/UiTextField.h"
#include "../UiTBC/Include/UiBillboardGeometry.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "AirBalloonPilot.h"
#include "Automan.h"
#include "AutoPathDriver.h"
#include "Autopilot.h"
#include "CenteredMachine.h"
#include "CanonDriver.h"
#include "DownwashConsoleManager.h"
#include "DownwashTicker.h"
#include "LandingTrigger.h"
#include "Level.h"
#include "RtVar.h"
#include "Eater.h"
#include "SimulatorDriver.h"
#include "Sunlight.h"
#include "Version.h"

#define STILL_FRAMES_UNTIL_CAM_PANS	4



namespace Downwash
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

const int ORDERED_LEVELNO[]	= { 6, 7, 8, 9, 5, 10, 0, 4, 11, 2, 1, 3, 12, 13 };
const int REVERSED_LEVELNO[]	= { 6, 10, 9, 11, 7, 4, 0, 1, 2, 3, 5, 8, 12, 13 };

}



DownwashManager::DownwashManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mAvatarId(0),
	mSetRandomChopperColor(false),
	mHadAvatar(false),
	mUpdateCameraForAvatar(false),
	mActiveWeapon(0),
	mLevel(0),
	mOldLevel(0),
	mHemisphere(0),
	mHemisphereUvTransform(0),
	mRenderHemisphere(true),
	mSunlight(0),
	mCameraTransform(QuaternionF(), Vector3DF(0, -200, 10)),
	mCameraSpeed(0),
	mZoomPlatform(false),
	mPostZoomPlatformFrameCount(100),
	mHitGroundFrameCount(STILL_FRAMES_UNTIL_CAM_PANS),
	mIsHitThisFrame(false),
	mLevelCompleted(false),
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	mPauseButton(0),
#endif // Touch or emulated touch.
	mStick(0),
	mWrongDirectionImage(0),
	mArrow(0),
	mCheckIcon(0),
	mLockIcon(0),
	mArrowBillboard(0),
	mArrowBillboardId(0),
	mArrowTotalPower(0),
	mArrowAngle(0),
	mSlowSystemCounter(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("plastic"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("thump"),		UiCure::CollisionSoundManager::SoundResourceInfo(5.0f, 0.5f, 1.0f));
	mCollisionSoundManager->AddSound(_T("gem"),		UiCure::CollisionSoundManager::SoundResourceInfo(0.1f, 0.2f, 0.1f));
	mCollisionSoundManager->PreLoadSound(_T("explosion"));

	SetConsoleManager(new DownwashConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	mMenu = new Life::Menu(mUiManager, GetResourceManager());
	mMenu->SetButtonTapSound(_T("tap.wav"), 0.3f);

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	mTouchstickTimer.ReduceTimeDiff(-5);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_STARTLEVEL, _T("level_06"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVELCOUNT, 14);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 0.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_ALLOWTOYMODE, false);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_PILOTNAME, _T("Pilot"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
}

DownwashManager::~DownwashManager()
{
	Close();

	//delete mAutopilot;	NOTE: owned by context manager.
	mAutopilot = 0;
	delete mStick;
	mStick = 0;
	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
	delete mHemisphereUvTransform;
	mHemisphereUvTransform = 0;
	delete mWrongDirectionImage;
	mWrongDirectionImage = 0;
	delete mArrow;
	mArrow = 0;
	delete mCheckIcon;
	mCheckIcon = 0;
	delete mLockIcon;
	mLockIcon = 0;
	delete mArrowBillboard;
	mArrowBillboard = 0;

}

void DownwashManager::LoadSettings()
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);

	Parent::LoadSettings();

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTHEIGHT, 30.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTFLAGS, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 30.0);

	UpdateCameraDistance();
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_CAMXOFFSET, 0.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_CAMYOFFSET, 0.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_CAMZOFFSET, 0.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_CAMXANGLE, 0.0);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_ENABLECLEAR, false);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_CTRL_STEER_LEFT3D, _T("Key.LEFT"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_CTRL_STEER_RIGHT3D, _T("Key.RIGHT"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_CTRL_STEER_UP3D, _T("Key.UP"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);

	GetConsoleManager()->ExecuteCommand(_T("bind-key F1 \"#Debug.Enable true; #Ui.3D.CamDistance 100.0\""));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F2 \"#Game.Childishness 1.0\""));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F3 \"#Game.Childishness 0.5\""));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F4 \"#Game.Childishness 0.0\""));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F5 prev-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F6 next-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F7 \"#Physics.NoClip true\""));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F8 \"#Physics.NoClip false\""));

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const str lSchtickName = _T("Touchstick");
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_UP3D, lSchtickName+_T(".AxisY-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_DOWN3D, lSchtickName+_T(".AxisY+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_LEFT3D, lSchtickName+_T(".AxisX-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_RIGHT3D, lSchtickName+_T(".AxisX+"));
#endif // Touch device or emulated touch device
}

void DownwashManager::SaveSettings()
{
#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
#endif // Computer or touch device.
}

void DownwashManager::SetRenderArea(const PixelRect& pRenderArea)
{
	Parent::SetRenderArea(pRenderArea);
	UpdateTouchstickPlacement();
}

bool DownwashManager::Open()
{
	bool lOk = Parent::Open();
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	if (lOk)
	{
		mPauseButton = ICONBTNA("grenade.png", "");
		int x = mRenderArea.GetCenterX() - 32;
		int y = mRenderArea.mBottom - 76;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(DownwashManager, OnPauseButton);
	}
#endif // Touch or emulated touch.
	if (lOk)
	{
		mWrongDirectionImage = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mWrongDirectionImage->Load(GetResourceManager(), _T("direction.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		mWinImage = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mWinImage->Load(GetResourceManager(), _T("win.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		mCheckIcon = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mCheckIcon->Load(GetResourceManager(), _T("icon_check.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		mLockIcon = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mLockIcon->Load(GetResourceManager(), _T("icon_lock.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &DownwashManager::PainterImageLoadCallback));

		mArrow = new UiCure::UserRendererImageResource(mUiManager, false);
		mArrow->Load(GetResourceManager(), _T("arrow.png"),
			UiCure::UserRendererImageResource::TypeLoadCallback(this, &DownwashManager::RendererTextureLoadCallback));
		mArrowBillboard = new UiTbc::BillboardGeometry(1/4.0f, 1);
		mArrowBillboardId = mUiManager->GetRenderer()->AddGeometry(mArrowBillboard, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	}
	return lOk;
}

void DownwashManager::Close()
{
	ScopeLock lLock(GetTickLock());
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	delete mPauseButton;
	mPauseButton = 0;
#endif // Touch or emulated touch.
	if (mSunlight)
	{
		delete mSunlight;
		mSunlight = 0;
	}
	Parent::Close();
}

void DownwashManager::SetIsQuitting()
{
	((DownwashConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void DownwashManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



bool DownwashManager::Render()
{
	if (!mHemisphere || !mHemisphere->IsLoaded() || !mLevel || !mLevel->IsLoaded())
	{
		if (!mHemisphere || !mHemisphere->IsLoaded() || !mOldLevel || !mOldLevel->IsLoaded())
		{
			mUiManager->GetRenderer()->Clear(UiTbc::Renderer::CLEAR_COLORBUFFER);
		}
		return true;
	}

	{
		double lRtrOffset;
		CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
		const bool lToyMode = lRtrOffset > 0.5;
		const double r = lToyMode? sin(mToyModeColorTimer.QueryTimeDiff()*1.0) : 0.5;
		const double g = lToyMode? sin(mToyModeColorTimer.QueryTimeDiff()*0.7) : 0.5;
		const double b = lToyMode? sin(mToyModeColorTimer.QueryTimeDiff()*0.3) : 0.5;
		const bool lEnableTexturing = lToyMode? false : true;
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_AMBIENTRED, r);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_AMBIENTGREEN, g);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_AMBIENTBLUE, b);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_ENABLETEXTURING, lEnableTexturing);
	}

	mHemisphere->GetMesh(0)->SetAlwaysVisible(false);
	if (mRenderHemisphere)
	{
		if (!mHemisphere->GetMesh(0)->GetUVAnimator())
		{
			mHemisphere->GetMesh(0)->SetUVAnimator(mHemisphereUvTransform);
			mHemisphere->GetMesh(0)->SetPreRenderCallback(TBC::GeometryBase::RenderCallback(this, &DownwashManager::DisableDepth));
			mHemisphere->GetMesh(0)->SetPostRenderCallback(TBC::GeometryBase::RenderCallback(this, &DownwashManager::EnableDepth));
		}
		Vector3DF lPosition = mCameraTransform.GetPosition();
		lPosition.x = -lPosition.x;
		lPosition.y = lPosition.z * -0.09f;
		Vector3DF lAngle = mCameraTransform.GetOrientation()*Vector3DF(0,250,0);
		lPosition.x -= lAngle.x;
		lPosition.y -= lAngle.z;
		mHemisphereUvTransform->GetBones()[0].GetRelativeBoneTransformation(0).GetPosition() = lPosition * 0.003f;

		mUiManager->GetRenderer()->RenderRelative(mHemisphere->GetMesh(0), 0);
	}

	bool lOk = Parent::Render();

	if (!lOk)
	{
		return lOk;
	}

	const UiCure::CppContextObject* lObject = (const UiCure::CppContextObject*)GetContext()->GetObject(mAvatarId);
	if (!lObject || mPostZoomPlatformFrameCount < 10)
	{
		return true;
	}
	if (mArrow->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE || lObject->GetPhysics()->GetEngineCount() < 3)
	{
		return true;
	}
	const Life::Options::Steering& s = mOptions.GetSteeringControl();
#define S(dir) s.mControl[Life::Options::Steering::CONTROL_##dir]
	const float lWantedDirection = S(RIGHT3D) - S(LEFT3D);
	const float lPower = S(UP3D) - S(DOWN3D);
	TransformationF lTransform = GetMainRotorTransform(lObject);
	float lTotalPower = ::sqrt(lWantedDirection*lWantedDirection + lPower*lPower);
	if (!lTotalPower)
	{
		return true;
	}
	lTotalPower = Math::Lerp(0.4f, 1.0f, lTotalPower);
	lTotalPower *= Math::Lerp(0.4f, 1.0f, lPower);
	mArrowTotalPower = Math::Lerp(mArrowTotalPower, lTotalPower, 0.3f);
	mArrowAngle = Math::Lerp(mArrowAngle, ::atan2(lWantedDirection, lPower), 0.3f);
	float lSize = mArrowTotalPower*0.5f;
	float lFoV;
	CURE_RTVAR_GET(lFoV, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
	lSize *= lTransform.GetPosition().GetDistance(mCameraTransform.GetPosition()) * lFoV / 2400;
	Vector3DF lPosition = lTransform.GetPosition();
	lPosition.x += ::sin(mArrowAngle) * lSize * 3.4f;
	lPosition.z += ::cos(mArrowAngle) * lSize * 3.4f;

	UiTbc::BillboardRenderInfoArray lBillboards;
	lBillboards.push_back(UiTbc::BillboardRenderInfo(mArrowAngle, lPosition, lSize, Vector3DF(1, 1, 1), 1, 0));
	mUiManager->GetRenderer()->RenderBillboards(mArrowBillboard, true, false, lBillboards);

	return true;
}

bool DownwashManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}

	if (mStick && !mMenu->IsDialogVisible())
	{
		DrawStick(mStick);
		mStick->ResetTap();
	}

	const Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar)
	{
		mUiManager->GetPainter()->SetAlphaValue(160);

		// Draw health bar first.
		const int w = mUiManager->GetDisplayManager()->GetWidth();
		const int m = w / 2;
		const float lWidth = (float)(((int)(w * 134.0f/480.0f)) & ~1);
		float lMin = m-lWidth/2;
		lMin = (lMin < 193)? 193 : lMin;
		const float lHealth = Cure::Health::Get(lAvatar);
		const uint8 r = (int8)Math::Clamp((int)((1-lHealth)*1.9f*255), 0, 255);
		const uint8 g = (int8)Math::Clamp((int)((lHealth-0.3f)*3*255), 0, 255);
		mUiManager->GetPainter()->SetColor(Color(r, g, 0), 0);
		const float lRemaining = Math::Clamp(lHealth*lWidth, 0.0f, lWidth);
		std::vector<Vector2DF> lCoords;
		lCoords.push_back(Vector2DF(lMin, 16+0.4f));
		lCoords.push_back(Vector2DF(lMin, 24+0.6f));
		lCoords.push_back(Vector2DF(lMin+lRemaining, 24+0.6f));
		lCoords.push_back(Vector2DF(lMin+lRemaining, 16+0.4f));
		lCoords.push_back(lCoords[0]);
		mUiManager->GetPainter()->DrawFan(lCoords, false);
		// Draw surrounding frame after.
		mUiManager->GetPainter()->SetColor(Color(10, 30, 40), 0);
		lCoords.clear();
		lCoords.push_back(Vector2DF(lMin-2, 16-2+0.4f));
		lCoords.push_back(Vector2DF(lMin-2, 24+2+0.6f));
		lCoords.push_back(Vector2DF(lMin+lWidth+2, 24+2+0.6f));
		lCoords.push_back(Vector2DF(lMin+lWidth+2, 16-2+0.4f));
		lCoords.push_back(lCoords[0]);
		mUiManager->GetPainter()->DrawFan(lCoords, false);

		double lRtrOffset;
		CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
		if (lRtrOffset > 0.1)
		{
			uint8 r = uint8(sin(mToyModeColorTimer.QueryTimeDiff()*3)*120+128);
			uint8 g = uint8(sin(mToyModeColorTimer.QueryTimeDiff()*5)*120+128);
			uint8 b = uint8(sin(mToyModeColorTimer.QueryTimeDiff()*7)*120+128);
			mUiManager->GetPainter()->SetColor(Color(10, 10, 10, 128));
			mUiManager->GetPainter()->PrintText(_T("Toy mode"), 11, 5);
			mUiManager->GetPainter()->SetColor(Color(r, g, b, 255));
			mUiManager->GetPainter()->PrintText(_T("Toy mode"), 10, 4);
		}
		else
		{
			const bool lIsFlying = mFlyTime.IsStarted();
			const double lTime = mFlyTime.QuerySplitTime();
			const bool lIsSloppy = (lIsFlying || !lTime);
			PrintTime(_T(""), lTime, lIsSloppy, 100, 3, Color(192, 210, 220), Color(10, 10, 10, 128));

			if (GetControlMode() != 2)
			{
				double lLevelBestTime = GetCurrentLevelBestTime(false);
				if (lLevelBestTime > 0)
				{
					PrintTime(_T("PR: "), lLevelBestTime, lIsSloppy, 100, 43, Color(10, 30, 40), Color(192, 192, 192, 128));
				}
				lLevelBestTime = GetCurrentLevelBestTime(true);
				if (lLevelBestTime > 0)
				{
					PrintTime(_T("WR: "), lLevelBestTime, lIsSloppy, 100, 83, Color(210, 40, 40, 255), Color(40, 10, 10, 128));
				}
			}
		}

		mUiManager->GetPainter()->SetAlphaValue(255);

		if (lAvatar->GetPhysics()->GetEngineCount() >= 3 && mWrongDirectionImage->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
			mLevel && mLevel->IsLoaded())
		{
			const double lImageTime = mDirectionImageTimer.QueryTimeDiff();
			if (!mDirectionImageTimer.IsStarted() && lImageTime > 2.0)
			{
				mAutopilot->AttemptCloserPathDistance();
			}
			if (lImageTime >= 0.5)
			{
				if (mDirectionImageTimer.IsStarted())
				{
					mDirectionImageTimer.PopTimeDiff();
					mDirectionImageTimer.Stop();
				}
				else
				{
					mDirectionImageTimer.Start();
				}
			}
			const float lPathDistance = mAutopilot->GetClosestPathDistance();
			//const bool lForceShowDirection = (GetCurrentLevelNumber() == 0 && mAllLoadedTimer.QueryTimeDiff() < 3.0);
			if (lPathDistance > 40.0f)
			{
				if (mDirectionImageTimer.IsStarted())
				{
					const Vector3DF lPos3d = mCameraTransform.GetPosition();
					const Vector2DF lPos(lPos3d.x, lPos3d.z);
					const Vector3DF lGoal3d = mLastLandingTriggerPosition;
					const Vector2DF lGoal(lGoal3d.x, lGoal3d.z);
					const float a = (lGoal-lPos).GetAngle() - PIF/2;
					const float lWantedSize = mUiManager->GetCanvas()->GetWidth() * 0.1f;
					const float lSize = lWantedSize;
					/*const float lSize = (float)mWrongDirectionImage->GetRamData()->GetWidth();
					while (lWantedSize >= lSize*2) lSize *= 2;
					while (lWantedSize <= lSize/2) lSize /= 2;*/
					// Find out the screen coordinate of the chopper, so we can place our arrow around that.
					const Vector3DF lCamDirection = mCameraTransform.GetOrientation() * Vector3DF(0,1,0);
					const Vector3DF lCamLookAtPointInChopperPlane = lPos3d + lCamDirection * -lPos3d.y;
					float lFoV;
					CURE_RTVAR_GET(lFoV, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
					lFoV /= 45.0f;
					const float lDistance = mUiManager->GetCanvas()->GetWidth() / 6.0f;
					const float x = mUiManager->GetCanvas()->GetWidth() /2.0f - 2*lDistance*::sin(a);
					const float y = mUiManager->GetCanvas()->GetHeight()/2.0f - lDistance*::cos(a);
					DrawImage(mWrongDirectionImage->GetData(), x, y, lSize, lSize, a);
				}
			}
			else
			{
				mDirectionImageTimer.Start();
			}
		}
	}
	if (mWinImage->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE && mWinTimer.IsStarted())
	{
		const float sf = ::sin((float)mWinTimer.QueryTimeDiff()*PIF/2.7f);
		const float f = std::min(1.0f, sf*1.3f);
		if (f < 0)
		{
			mWinTimer.Stop();
		}
		else
		{
			float x = 5;
			float s = Math::Clamp(mUiManager->GetCanvas()->GetWidth() / 4.0f, 128.0f, 256.0f);
			float y = f*s;
			y = mRenderArea.GetHeight()-y;
			x += s*0.5f;
			y += s*0.5f;
			DrawImage(mWinImage->GetData(), x, y, s, s, 0);
		}
	}

	return true;
}

void DownwashManager::PrintTime(const str pPrefix, double pTime, bool lIsSloppy, int x, int y, const Color c, const Color bg)
{
	const int lSec = (int)pTime;
	const str lIntTimeString = pPrefix + strutil::Format(_T("%i"), lSec);
	const str lTimeString = pPrefix + strutil::Format(lIsSloppy? _T("%.1f s") : _T("%.3f s"), pTime);
	int w = mUiManager->GetPainter()->GetStringWidth(lIntTimeString);
	x -= w;
	x = (x < 4)? 4 : x;
	mUiManager->GetPainter()->SetColor(bg, 0);
	mUiManager->GetPainter()->PrintText(lTimeString, x+1, y+1);
	mUiManager->GetPainter()->SetColor(c, 0);
	mUiManager->GetPainter()->PrintText(lTimeString, x, y);
}

void DownwashManager::DrawSyncDebugInfo()
{
	Parent::DrawSyncDebugInfo();

	if (GetLevel() && GetLevel()->QueryPath()->GetPath(0))
	{
		UiCure::DebugRenderer lDebugRenderer(GetVariableScope(), mUiManager, GetContext(), 0, GetTickLock());
		for (int x = 0; x < 20; ++x)
		{
			Cure::ContextPath::SplinePath* lPath = GetLevel()->QueryPath()->GetPath(x);
			if (!lPath)
			{
				break;
			}
			lDebugRenderer.RenderSpline(mUiManager, lPath);
		}
	}
}



void DownwashManager::OnLoginSuccess()
{
}



bool DownwashManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraTransform.GetPosition()) <= pDistance*pDistance);
}

Cure::GameObjectId DownwashManager::GetAvatarInstanceId() const
{
	return mAvatarId;
}



bool DownwashManager::SetAvatarEnginePower(unsigned pAspect, float pPower)
{
	deb_assert(pAspect >= 0 && pAspect < TBC::PhysicsEngine::ASPECT_COUNT);
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject && !mZoomPlatform)
	{
		return SetAvatarEnginePower(lObject, pAspect, pPower);
	}
	return SetAvatarEnginePower(lObject, pAspect, 0.0f);
}



void DownwashManager::Shoot(Cure::ContextObject* pCanon, int pAmmo)
{
	Life::FastProjectile* lProjectile = 0;
	switch (pAmmo)
	{
		default:
		{
			lProjectile = new Life::FastProjectile(GetResourceManager(), _T("bullet"), mUiManager, this);
		}
		break;
		case 1:
		{
			Life::HomingProjectile* lHoming = new Life::HomingProjectile(GetResourceManager(), _T("missile"), mUiManager, this);
			lHoming->SetTarget(mAvatarId);
			lProjectile = lHoming;
		}
		break;
	}
	AddContextObject(lProjectile, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	lProjectile->SetOwnerInstanceId(pCanon->GetInstanceId());
	TransformationF t(pCanon->GetOrientation(), pCanon->GetPosition());
	lProjectile->SetInitialTransform(t);
	lProjectile->StartLoading();

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	Vector3DF v;
	Life::ProjectileUtil::GetBarrel(lProjectile, t, v);
	lParticleRenderer->CreateFlare(Vector3DF(0.9f, 0.7f, 0.5f), 0.3f, 7.5f, t.GetPosition(), v);
}

void DownwashManager::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
{
	mCollisionSoundManager->OnCollision(pStrength, pPosition, pExplosiveGeometry, _T("explosion"));

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lKeepOnGoingFactor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	Vector3DF u = pVelocity.ProjectOntoPlane(pNormal) * (1+lKeepOnGoingFactor);
	u -= pVelocity;	// Mirror and inverse.
	u.Normalize();
	const int lParticles = Math::Lerp(8, 20, pStrength * 0.2f);
	Vector3DF lStartFireColor(1.0f, 1.0f, 0.3f);
	Vector3DF lFireColor(0.6f, 0.4f, 0.2f);
	Vector3DF lStartSmokeColor(0.4f, 0.4f, 0.4f);
	Vector3DF lSmokeColor(0.2f, 0.2f, 0.2f);
	Vector3DF lShrapnelColor(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	if (pExplosive->GetClassId().find(_T("barrel")) != str::npos)
	{
		lStartFireColor.Set(0.9f, 1.0f, 0.8f);
		lFireColor.Set(0.3f, 0.7f, 0.2f);
		lStartSmokeColor.Set(0.3f, 0.35f, 0.3f);
		lSmokeColor.Set(0.2f, 0.4f, 0.2f);
		lShrapnelColor.Set(0.5f, 0.5f, 0.1f);
	}
	lParticleRenderer->CreateExplosion(pPosition, pStrength, u, 1, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles, lParticles, lParticles/2, lParticles/3);


	// Slowmo check.
	const bool lIsAvatar = (pExplosive->GetInstanceId() == mAvatarId);
	bool lNormalDeath = true;
	if (lIsAvatar && Cure::Health::Get(pExplosive) < -5500)
	{
		if (Random::Uniform(0.0f, 1.0f) > 0.7f)
		{
			lNormalDeath = false;
			mSlowmoTimer.TryStart();
			DeleteContextObjectDelay(pExplosive, 6.5);
		}
	}
	if (lNormalDeath)
	{
		DeleteContextObjectDelay(pExplosive, 3.0);
	}
	if (lIsAvatar)
	{
		((UiCure::Machine*)pExplosive)->GetBurnEmitter()->SetFreeFlow();
	}


	// Shove!
	ScopeLock lLock(GetTickLock());
	TBC::PhysicsManager* lPhysicsManager = GetPhysicsManager();
	Cure::ContextManager::ContextObjectTable lObjectTable = GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (!lObject->IsLoaded())
		{
			continue;
		}
		float lForce = Life::Explosion::CalculateForce(lPhysicsManager, lObject, pPosition, pStrength) * 0.5f;
		if (pExplosive->GetClassId().find(_T("missile")) != str::npos)
		{
			lForce *= 5;
		}
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			Cure::FloatAttribute* lHealth = Cure::Health::GetAttribute(lObject);
			if (lHealth && !mZoomPlatform)
			{
				const float lValue = lHealth->GetValue() - lForce*Random::Normal(0.51f, 0.05f, 0.3f, 0.5f);
				lHealth->SetValue(lValue);
			}
			x->second->ForceSend();
		}
		Life::Explosion::PushObject(lPhysicsManager, lObject, pPosition, pStrength*0.1f);
	}
}

void DownwashManager::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	(void)pHitObject;

	TBC::ChunkyPhysics* lPhysics = pBullet->GetPhysics();
	if (lPhysics)
	{
		TBC::ChunkyBoneGeometry* lGeometry = lPhysics->GetBoneGeometry(0);
		mCollisionSoundManager->OnCollision(5.0f, pBullet->GetPosition(), lGeometry, lGeometry->GetMaterial());
		Cure::Health::Add(pHitObject, -0.12f, false);
	}
}



bool DownwashManager::DidFinishLevel()
{
	mLog.Headlinef(_T("Level %s done!"), mLevel->GetClassId().c_str());
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && lAvatar->GetPhysics()->GetEngineCount() >= 3)
	{
		const double lTime = mFlyTime.QuerySplitTime();
		const double lLevelBestTime = GetCurrentLevelBestTime(false);
		if (lTime > 0 && (lTime < lLevelBestTime || lLevelBestTime <= 0))
		{
			SetCurrentLevelBestTime(false, lTime);
		}

		UiCure::UserSound2dResource* lFinishSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, 1.0f, 1.0f);
		mZoomPlatform = true;
		mLevelCompleted = true;
		mWinTimer.Start();
		return true;
	}
	return false;
}

str DownwashManager::StepLevel(int pCount)
{
	if (GetContext()->GetObject(mAvatarId))
	{
		int lLevelNumber = GetCurrentLevelNumber();
		lLevelNumber += pCount;
		int lLevelCount;
		CURE_RTVAR_GET(lLevelCount, =, GetVariableScope(), RTVAR_GAME_LEVELCOUNT, 14);
		if (lLevelNumber >= lLevelCount)
		{
			lLevelNumber = 0;

			double lRtrOffset;
			CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
			lRtrOffset += 1;
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, lRtrOffset);
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0+lRtrOffset);
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_ALLOWTOYMODE, true);
		}
		if (lLevelNumber < 0)
		{
			lLevelNumber = lLevelCount-1;
		}
		lLevelNumber = ORDERED_LEVELNO[lLevelNumber];
		mOldLevel = mLevel;
		mLevelCompleted = false;
		str lNewLevelName = strutil::Format(_T("level_%.2i"), lLevelNumber);
		mLevel = (Level*)Parent::CreateContextObject(lNewLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
		mLevel->StartLoading();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_STARTLEVEL, lNewLevelName);
		return lNewLevelName;
	}
	return _T("");
}



Level* DownwashManager::GetLevel() const
{
	if (mLevel && mLevel->IsLoaded())
	{
		return mLevel;
	}
	return 0;
}

int DownwashManager::GetCurrentLevelNumber() const
{
	int lLevelNumber = 0;
	strutil::StringToInt(mLevel->GetClassId().substr(6), lLevelNumber);
	return REVERSED_LEVELNO[lLevelNumber];
}

double DownwashManager::GetCurrentLevelBestTime(bool pWorld) const
{
	const int lLevelIndex = GetCurrentLevelNumber();
	const str lRecordFormat = pWorld? _T(RTVAR_GAME_WORLDRECORD_LEVEL) _T("_%i") : _T(RTVAR_GAME_PERSONALRECORD_LEVEL) _T("_%i");
	const str lLevelTimeVarName = strutil::Format(lRecordFormat.c_str(), lLevelIndex);
	static HashedString lFastName(lLevelTimeVarName);
	if (lFastName != lLevelTimeVarName)
	{
		lFastName = lLevelTimeVarName;
	}
	return GetVariableScope()->GetDefaultValue(Cure::RuntimeVariableScope::READ_IGNORE, lFastName, 0.0);
}

void DownwashManager::SetCurrentLevelBestTime(bool pWorld, double pTime)
{
	const int lLevelIndex = GetCurrentLevelNumber();
	const str lRecordFormat = pWorld? _T(RTVAR_GAME_WORLDRECORD_LEVEL) _T("_%i") : _T(RTVAR_GAME_PERSONALRECORD_LEVEL) _T("_%i");
	const str lLevelTimeVarName = strutil::Format(lRecordFormat.c_str(), lLevelIndex);
	GetVariableScope()->SetValue(Cure::RuntimeVariable::USAGE_SYS_OVERRIDE, lLevelTimeVarName, pTime);
}

Cure::ContextObject* DownwashManager::GetAvatar() const
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	return lAvatar;
}



Cure::RuntimeVariableScope* DownwashManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool DownwashManager::Reset()	// Run when disconnected. Removes all objects and displays login GUI.
{
	ScopeLock lLock(GetTickLock());
	return Parent::Reset();
}

bool DownwashManager::InitializeUniverse()
{
	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const Vector3DF v;
	lParticleRenderer->CreateExplosion(Vector3DF(0,0,-2000), 1, v, 1, v, v, v, v, v, 1, 1, 1, 1);

	str lStartLevel;
	CURE_RTVAR_GET(lStartLevel, =, GetVariableScope(), RTVAR_GAME_STARTLEVEL, _T("level_00"));
	mMassObjectArray.clear();
	mLevel = (Level*)Parent::CreateContextObject(lStartLevel, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	mLevel->StartLoading();
	TBC::BoneHierarchy* lTransformBones = new TBC::BoneHierarchy;
	lTransformBones->SetBoneCount(1);
	lTransformBones->FinalizeInit(TBC::BoneHierarchy::TRANSFORM_NONE);
	mHemisphereUvTransform = new TBC::BoneAnimator(lTransformBones);
	mHemisphere = (UiCure::CppContextObject*)Parent::CreateContextObject(_T("hemisphere"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	mHemisphere->DisableRootShadow();
	mHemisphere->EnableMeshMove(false);
	mHemisphere->SetPhysicsTypeOverride(Cure::PHYSICS_OVERRIDE_BONES);
	mHemisphere->SetInitialTransform(TransformationF(QuaternionF(), Vector3DF(0, 25, 0)));
	mHemisphere->StartLoading();
	mSunlight = new Sunlight(mUiManager);
	mAutopilot = new Autopilot(this);
	return true;
}

void DownwashManager::CreateChopper(const str& pClassId)
{
	mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;
	mZoomPlatform = false;

	Cure::ContextObject* lAvatar = Parent::CreateContextObject(pClassId, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
	if (lSpawner)
	{
		lSpawner->PlaceObject(lAvatar, -1);
	}
	lAvatar->QuerySetChildishness(1);
	mAvatarId = lAvatar->GetInstanceId();
	lAvatar->StartLoading();
	mAutopilot->Reset();
}

void DownwashManager::UpdateChopperColor(float pLerp)
{
	UiCure::CppContextObject* lAvatar = (UiCure::CppContextObject*)GetContext()->GetObject(mAvatarId);
	if (!lAvatar || !mLevel || !mLevel->IsLoaded())
	{
		return;
	}
	const float lLevelBrightness = std::min(1.0f, mLevel->GetMesh(0)->GetBasicMaterialSettings().mDiffuse.GetLength() * 2);
	UiTbc::ChunkyClass* lClass = (UiTbc::ChunkyClass*)lAvatar->GetClass();
	const size_t lMeshCount = lClass->GetMeshCount();
	for (size_t x = 0; x < lMeshCount; ++x)
	{
		const Vector3DF d = (x == 0 && mSetRandomChopperColor)? mLastChopperColor : lClass->GetMaterial(x).mDiffuse;
		lAvatar->GetMesh(x)->GetBasicMaterialSettings().mAmbient = Math::Lerp(lAvatar->GetMesh(x)->GetBasicMaterialSettings().mAmbient, lClass->GetMaterial(x).mAmbient * lLevelBrightness, pLerp);
		lAvatar->GetMesh(x)->GetBasicMaterialSettings().mDiffuse = Math::Lerp(lAvatar->GetMesh(x)->GetBasicMaterialSettings().mDiffuse, d * lLevelBrightness, pLerp);
	}
}

void DownwashManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void DownwashManager::UpdateCameraDistance()
{
	double lCamDistance = 11.3 * mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	lCamDistance = (lCamDistance+110)/2;	// Smooth towards a sensible cam distance.
	if (mMenu->IsDialogVisible())
	{
		lCamDistance *= 0.3f;
	}
	lCamDistance = std::min(110.0, lCamDistance);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, lCamDistance);
}

void DownwashManager::UpdateTouchstickPlacement()
{
	if (mTouchstickTimer.QueryTimeDiff() < 2.0)
	{
		return;
	}
	mTouchstickTimer.ClearTimeDiff();

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const float lTouchSideScale = 1.28f;	// Inches.
	const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
	if (!mStick)
	{
		int lScreenPixelWidth;
		CURE_RTVAR_GET(lScreenPixelWidth, =, GetVariableScope(), RTVAR_UI_DISPLAY_WIDTH, 1024);
		const int lMinimumTouchRadius = (int)(lScreenPixelWidth*lTouchScale*0.17f);	// Touched area is a fraction of the required 32px/iPhone classic.
		mStick = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10),  0, lMinimumTouchRadius);
		mStick->SetUniqueIdentifier(_T("Touchstick"));
		mStick->SetValueScale(-1,+1, -1,-0.2f);
	}
	int lFingerSize = (int)(mRenderArea.GetWidth() * lTouchScale);
	if (lFingerSize > mRenderArea.GetHeight()/3)
	{
		lFingerSize = mRenderArea.GetHeight()/3;
	}
	int lJoystickSize = lFingerSize*4/3;
	PixelRect lRightStickArea(mRenderArea);
	lRightStickArea.mLeft = mRenderArea.GetWidth() - lJoystickSize;
	lRightStickArea.mTop = lRightStickArea.mBottom - lJoystickSize;
	const int ow = lRightStickArea.GetWidth();
	const int r = (int)(ow*0.27f);	// Knob radius.
	const int lMargin = (int)(r*0.2f);
	mStick->SetFingerRadius(r+lMargin);
	mStick->Move(lRightStickArea, 0);
#endif // Touch or emulated touch
}

int DownwashManager::GetControlMode() const
{
	// Three modes: easy, medium, hard.
	float lChildishness;
	CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
	if (lChildishness > 0.95f)
	{
		return 2;	// Easy.
	}
	else if (lChildishness >= 0.1f)
	{
		return 1;	// Medium.
	}
	return 0;	// Hard.
}

void DownwashManager::UpdateControlMode()
{
	Cure::CppContextObject* lAvatar = (Cure::CppContextObject*)GetContext()->GetObject(mAvatarId);
	if (!lAvatar || lAvatar->GetPhysics()->GetEngineCount() < 3)
	{
		return;
	}
	const int lHelperEngineIndex = lAvatar->GetPhysics()->GetEngineCount() - 1;
	if (GetControlMode() == 2)
	{
		// Helper engine on + upright stabilization high.
		lAvatar->GetPhysics()->GetEngine(lHelperEngineIndex)->SetStrength(1000.0f);
		((TBC::ChunkyClass::Tag*)lAvatar->GetClass()->GetTag(_T("upright_stabilizer")))->mFloatValueList[0] = 3;
	}
	else
	{
		// Engine off, low upright stabilization. Arcade mode compensates by having high autopilot and high durability.
		lAvatar->GetPhysics()->GetEngine(lHelperEngineIndex)->SetStrength(0.0f);
		((TBC::ChunkyClass::Tag*)lAvatar->GetClass()->GetTag(_T("upright_stabilizer")))->mFloatValueList[0] = 0.3f;
	}
}

void DownwashManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);

	UpdateControlMode();

	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPhysicsStepCount > 0 && mAllowMovementInput)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);

		if (lObject)
		{
			// Control steering.
			float lChildishness;
			CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
			if (mHitGroundFrameCount >= -STILL_FRAMES_UNTIL_CAM_PANS)
			{
				lChildishness = 0;	// Don't help when not even started yet.
			}
			const Vector3DF lAutoPilot3d = mAutopilot->GetSteering();
			const Vector2DF lAutoPilot(lAutoPilot3d.x, lAutoPilot3d.z);
			const Vector3DF lAutoPilotDirection3d = mAutopilot->GetClosestPathVector().GetNormalized();
			const Vector2DF lAutoPilotDirection(lAutoPilotDirection3d.x, lAutoPilotDirection3d.z);
			const Life::Options::Steering& s = mOptions.GetSteeringControl();
#define S(dir) s.mControl[Life::Options::Steering::CONTROL_##dir]
			Vector2DF lUserControls(S(RIGHT3D) - S(LEFT3D), S(UP3D) - S(DOWN3D));
			Vector2DF lUserDirection = lUserControls;
			if (lUserDirection.GetLengthSquared() < 0.01f)	// User is not controlling, AI is.
			{
				lUserControls = lAutoPilot * lChildishness;
				lUserDirection = lUserControls;
			}
			else if (lUserDirection.Dot(lAutoPilotDirection) >= 0)	// User wants to go in the same direction as AI, so AI helps.
			{
				lUserDirection = Math::Lerp(lUserDirection, lAutoPilot, lChildishness*0.35f);
			}
			else
			{
				// User wants to go in opposite direction of AI, so let him.
				// At the same time we can check if he's on to something new.
				static int cntHack = 0;
				if (++cntHack > 30)
				{
					mAutopilot->AttemptCloserPathDistance();
				}
			}
			// Pull the brakes a little bit.
			const float lCurrentFlyingDirectionX = lObject->GetVelocity().x * 0.05f;
			lUserDirection.x -= lCurrentFlyingDirectionX;
			// X follows helicopter yaw.
			float lYaw, _;
			lObject->GetOrientation().GetEulerAngles(lYaw, _, _);
			const float lPowerFwdRev = -::sin(lYaw) * lUserDirection.x;
			const float lPowerLeftRight = ::cos(lYaw) * lUserDirection.x;
			SetAvatarEnginePower(lObject, 4, lPowerFwdRev);
			SetAvatarEnginePower(lObject, 5, lPowerLeftRight);
			SetAvatarEnginePower(lObject, 7, lUserDirection.y);
			// Kids' push engine.
			if (mAutopilot)
			{
				const Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
				if (lAvatar)
				{
					const float f = std::min(1.0f, mAutopilot->GetRotorSpeed(lAvatar) / 14.0f);
					SetAvatarEnginePower(lObject,  9, lUserControls.x*f);
					SetAvatarEnginePower(lObject, 11, Math::Lerp(-0.8f, 1.0f, lUserDirection.y*f));
				}
			}

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

bool DownwashManager::SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower)
{
	if (mZoomPlatform)
	{
		pAvatar->SetEnginePower(pAspect, 0);
		return false;
	}
	return pAvatar->SetEnginePower(pAspect, pPower);
}

void DownwashManager::TickUiUpdate()
{
	if (!mIsHitThisFrame)
	{
		--mHitGroundFrameCount;
		if (mHitGroundFrameCount > 0)
		{
			mHitGroundFrameCount = 0;
		}
	}
	mIsHitThisFrame = false;

	if (GetTimeManager()->GetAffordedPhysicsStepTime() > 1.1f/PHYSICS_FPS)
	{
		if (++mSlowSystemCounter > 30)
		{
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_ENABLEMASSOBJECTS, false);
		}
	}
	else
	{
		mSlowSystemCounter = 0;
	}


	const Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (mHitGroundFrameCount <= -1 && mLevel && mLevel->IsLoaded() && lAvatar)
	{
		if (lAvatar->GetPhysics()->GetEngineCount() < 3)
		{
			mFlyTime.Stop();
		}
		else if (!mFlyTime.IsStarted() && !mLevelCompleted)
		{
			Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
			deb_assert(lSpawner);
			const float lDistanceToHome = GetContext()->GetObject(mAvatarId)->GetPosition().GetDistance(lSpawner->GetSpawnPoint().GetPosition());
			if (lDistanceToHome < 15)
			{
				mFlyTime.TryStart();
			}
			else
			{
				mFlyTime.ResumeFromLapTime();
			}
		}
	}

	((DownwashConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	mCollisionSoundManager->Tick(mCameraTransform.GetPosition());
}

bool DownwashManager::UpdateMassObjects(const Vector3DF& pPosition)
{
	if (mLevel && mLevel->IsLoaded() && mMassObjectArray.empty())
	{
		const Level::MassObjectList lMassObjects = mLevel->GetMassObjects();
		for (Level::MassObjectList::const_iterator x = lMassObjects.begin(); x != lMassObjects.end(); ++x)
		{
			const Level::MassObjectInfo& lInfo = *x;
			const TBC::PhysicsManager::BodyID lTerrainBodyId = mLevel->GetPhysics()->GetBoneGeometry(lInfo.mGroundBodyIndex)->GetBodyId();
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			Life::MassObject* lMassObject = new Life::MassObject(GetResourceManager(), lInfo.mClassId, mUiManager, lTerrainBodyId, lInfo.mCount, 120);
			lMassObject->SetSeed((unsigned)HashString(lInfo.mClassId.c_str()));
			AddContextObject(lMassObject, Cure::NETWORK_OBJECT_LOCAL_ONLY, lMassObjectId);
			lMassObject->StartLoading();
			mLevel->AddChild(lMassObject);
		}
	}

	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		Life::MassObject* lObject = (Life::MassObject*)GetContext()->GetObject(*x, true);
		deb_assert(lObject);
		lObject->SetRootPosition(pPosition);
	}

	return true;
}

void DownwashManager::SetLocalRender(bool pRender)
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

void DownwashManager::SetMassRender(bool pRender)
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



Cure::ContextObject* DownwashManager::CreateContextObject(const str& pClassId) const
{
	Cure::CppContextObject* lObject;
	if (pClassId == _T("missile"))
	{
		lObject = new Life::HomingProjectile(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
	}
	else if (pClassId == _T("grenade") || pClassId == _T("rocket"))
	{
		lObject = new Life::FastProjectile(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
	}
	else if (pClassId == _T("bomb"))
	{
		lObject = new Life::Projectile(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
	}
	else if (strutil::StartsWith(pClassId, _T("barrel")))
	{
		Life::Mine* lMine = new Life::Mine(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
		Cure::Health::Set(lMine, 0.3f);
		lMine->EnableDeleteDetonation(false);
		lMine->SetExplosiveStrength(5);
		new Cure::FloatAttribute(lMine, _T("DamageAbsorption"), 0.2f);
		new Cure::FloatAttribute(lMine, _T("DamageReduction"), 0.3f);
		lObject = lMine;
	}
	else if (pClassId == _T("stone") || pClassId == _T("cube"))
	{
		lObject = new CenteredMachine(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
		Cure::Health::DeleteAttribute(lObject);
	}
	else if (pClassId == _T("beach_ball") || pClassId == _T("dino_bubble") || pClassId == _T("clown_bubble"))
	{
		CenteredMachine* lMachine = new CenteredMachine(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
		new Cure::FloatAttribute(lMachine, _T("DamageAbsorption"), 1e-3f);
		lMachine->SetDeathFrameDelay(0);
		lMachine->SetDisappearAfterDeathDelay(0.2f);
		lObject = lMachine;
	}
	else if (strutil::StartsWith(pClassId, _T("level_")))
	{
		UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, 0.5f, 1, 10, 2);
		Level* lLevel = new Level(GetResourceManager(), pClassId, mUiManager, lGravelParticleEmitter);
		lLevel->DisableRootShadow();
		lObject = lLevel;
	}
	else if (strutil::StartsWith(pClassId, _T("helicopter_")) || strutil::StartsWith(pClassId, _T("monster")) ||
		strutil::StartsWith(pClassId, _T("fighter")))
	{
		CenteredMachine* lMachine = new CenteredMachine(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
		if (strutil::StartsWith(pClassId, _T("helicopter_")))
		{
			new Cure::FloatAttribute(lMachine, _T("DamageAbsorption"), 1);
			lMachine->SetDeathFrameDelay(2);
		}
		lMachine->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), mUiManager));
		//lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lMachine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	else if (strutil::StartsWith(pClassId, _T("forklift")) || strutil::StartsWith(pClassId, _T("corvette")) ||
		strutil::StartsWith(pClassId, _T("air_balloon")) || strutil::StartsWith(pClassId, _T("turret2")))
	{
		UiCure::Machine* lMachine = new BaseMachine(GetResourceManager(), pClassId, mUiManager, (DownwashManager*)this);
		if (strutil::StartsWith(pClassId, _T("turret")))
		{
			new Cure::FloatAttribute(lMachine, _T("DamageAbsorption"), 1);
		}
		//lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lMachine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	else
	{
		UiCure::Machine* lMachine = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
		//lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

Cure::ContextObject* DownwashManager::CreateLogicHandler(const str& pType)
{
	if (pType == _T("spawner") || pType == _T("spawner_init"))
	{
		return new Life::Spawner(GetContext());
	}
	else if (pType == _T("real_time_ratio"))
	{
		return new LandingTrigger(GetContext());
	}
	else if (pType == _T("eater"))
	{
		return new Eater(GetContext());
	}
	else if (pType == _T("context_path"))
	{
		return mLevel->QueryPath();
	}
	else if (pType == _T("trig_elevator"))
	{
		Cure::Elevator* lElevator = new Cure::Elevator(GetContext());
		lElevator->SetStopDelay(0.5f);
		return lElevator;
	}
	else if (strutil::StartsWith(pType, _T("driver:")))
	{
		return new Cure::Driver(GetContext());
	}
	return (0);
}

void DownwashManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		if (pObject->GetInstanceId() == mAvatarId)
		{
			log_volatile(mLog.Debug(_T("Yeeha! Loaded avatar!")));
			if (mSetRandomChopperColor)
			{
				Vector3DF lColor;
				do
				{
					lColor = RNDPOSVEC();
				} while (lColor.GetDistanceSquared(mLastVehicleColor) < 1);
				mLastVehicleColor = lColor;
				mLastChopperColor = lColor;
				((UiCure::CppContextObject*)pObject)->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = lColor;
			}
			UpdateChopperColor(1.0f);
			EaseDown(pObject, 0);
		}
		else if (pObject == mLevel)
		{
			OnLevelLoadCompleted();
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("monster")))
		{
			((Life::ExplodingMachine*)pObject)->SetDeathFrameDelay(5);
			Vector3DF lDirection(-1,0,0);
			new Automan(this, pObject->GetInstanceId(), lDirection);
			Vector3DF lColor;
			do
			{
				lColor = RNDPOSVEC();
			} while (lColor.GetDistanceSquared(mLastVehicleColor) < 1);
			mLastVehicleColor = lColor;
			((UiCure::CppContextObject*)pObject)->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = lColor;
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("forklift")) ||
			strutil::StartsWith(pObject->GetClassId(), _T("corvette")))
		{
			str lBaseName = strutil::Split(pObject->GetClassId(), _T("_"))[0];
			new AutoPathDriver(this, pObject->GetInstanceId(), lBaseName+_T("_path"));
			Vector3DF lColor;
			do
			{
				lColor = RNDPOSVEC();
			} while (lColor.GetDistanceSquared(mLastVehicleColor) < 1);
			mLastVehicleColor = lColor;
			((UiCure::CppContextObject*)pObject)->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = lColor;
		}
		else if (pObject->GetClassId() == _T("turret"))
		{
			new CanonDriver(this, pObject->GetInstanceId(), 0);
		}
		else if (pObject->GetClassId() == _T("turret2"))
		{
			new CanonDriver(this, pObject->GetInstanceId(), 1);
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("air_balloon")))
		{
			new AirBalloonPilot(this, pObject->GetInstanceId());
			if (mLastVehicleColor.y > 0.4f && mLastVehicleColor.x < 0.3f && mLastVehicleColor.z < 0.3f)
			{
				mLastVehicleColor = Vector3DF(0.6f, 0.2f, 0.2f);
				((UiCure::CppContextObject*)pObject)->GetMesh(2)->GetBasicMaterialSettings().mDiffuse = mLastVehicleColor;
			}
			else
			{
				mLastVehicleColor = Vector3DF(0, 1, 0);
			}
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("fighter")))
		{
			//pObject->SetEnginePower(0, 1);
			pObject->SetEnginePower(1, 1);
			//pObject->SetEnginePower(2, 1);
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("simulator")))
		{
			new SimulatorDriver(this, pObject->GetInstanceId());
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

void DownwashManager::OnLevelLoadCompleted()
{
	/*// Kickstart all elevators.
	for (int x = 0; x < 20; ++x)
	{
		mLevel->SetEnginePower(x, 1);
	}*/

	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && lAvatar->IsLoaded() && lAvatar->GetPhysics()->GetEngineCount() < 3)
	{
		// Avatar somehow crashed after landing last level... Well done, but not impossible in a dynamic world.
	}
	else if (!lAvatar)
	{
		CreateChopper(_T("helicopter_01"));
	}
	else
	{
		Cure::Health::Set(lAvatar, 1);
		Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
		deb_assert(lSpawner);
		const Vector3DF lLandingPosition = GetLandingTriggerPosition(mOldLevel);
		const Vector3DF lHeliPosition = lAvatar->GetPosition();
		const Vector3DF lHeliDelta = lHeliPosition - lLandingPosition;
		Vector3DF lNewPosition = lSpawner->GetSpawnPoint().GetPosition() + lHeliDelta;
		const float lCamAboveHeli = mCameraTransform.GetPosition().z - lHeliPosition.z;
		const Vector3DF lCamDelta = lSpawner->GetSpawnPoint().GetPosition() - lLandingPosition;

		mMassObjectArray.clear();
		GetContext()->DeleteObject(mOldLevel->GetInstanceId());
		mOldLevel = 0;

		EaseDown(lAvatar, &lNewPosition);

		mCameraTransform.GetPosition() += lCamDelta;
		mCameraTransform.GetPosition().z = lAvatar->GetPosition().z + lCamAboveHeli;
		mCameraPreviousPosition = mCameraTransform.GetPosition();
		UpdateCameraPosition(true);
	}
	if (mOldLevel)
	{
		mMassObjectArray.clear();
		GetContext()->DeleteObject(mOldLevel->GetInstanceId());
		mOldLevel = 0;
	}
	if (mLevel->GetBackgroundName().empty())
	{
		mRenderHemisphere = false;
	}
	else
	{
		mRenderHemisphere = true;
		mHemisphere->ReplaceTexture(0, mLevel->GetBackgroundName());
	}

	for (int lEngine = 30; lEngine < 40; ++lEngine)
	{
		mLevel->SetEnginePower(lEngine, 1.0f);
	}

	GetLandingTriggerPosition(mLevel);	// Update shadow landing trigger position.
	mAutopilot->Reset();
	mZoomPlatform = false;
}

void DownwashManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);

	bool lIsAvatar = (pObject1->GetInstanceId() == mAvatarId);

	float lChildishness = 0.0f;

	if (!mIsHitThisFrame && lIsAvatar)
	{
		// If it's a lever or something like that, this is not a landing!
		if (pObject2->GetPhysics()->GetPhysicsType() != TBC::ChunkyPhysics::DYNAMIC &&
			!pObject2->GetPhysics()->GetBoneGeometry(pBody2Id)->IsCollideWithSelf())
		{
			mIsHitThisFrame = true;
			if (mHitGroundFrameCount < 0)
			{
				mHitGroundFrameCount = 0;
			}
			++mHitGroundFrameCount;
		}
	}

	// Check if we're just stabilizing on starting pad.
	if (lIsAvatar && mAvatarCreateTimer.IsStarted())
	{
		return;
	}

	// Check if we're colliding with a smooth landing pad.
	bool lIsLandingPad = false;
	bool lIsLandingOnElevator = false;
	float lCollisionImpactFactor = 3;
	if (lIsAvatar && pObject2 == mLevel)
	{
		const TBC::ChunkyClass::Tag* lTag = mLevel->GetClass()->GetTag(_T("anything"));
		std::vector<int>::const_iterator x = lTag->mBodyIndexList.begin();
		for (; x != lTag->mBodyIndexList.end(); ++x)
		{
			TBC::ChunkyBoneGeometry* lBone = pObject2->GetPhysics()->GetBoneGeometry(*x);
			if (lBone->GetBodyId() == pBody2Id)
			{
				lIsLandingPad = true;
				lCollisionImpactFactor = 1;
				mFlyTime.Stop();
				if (lBone->GetJointType() != TBC::ChunkyBoneGeometry::JOINT_EXCLUDE)
				{
					lIsLandingOnElevator = true;
				}
				break;
			}
		}
	}

	const float lOrientationFactor = (pObject1->GetOrientation()*Vector3DF(0,0,1)*Vector3DF(0,0,1));

	// Don't do collisions if heli hasn't moved, such as in the case of standing on
	// an elevator.
	if (lIsLandingOnElevator && lIsAvatar && lIsLandingPad && mHitGroundFrameCount >= STILL_FRAMES_UNTIL_CAM_PANS && lOrientationFactor > 0.95f)
	{
		return;
	}

	if (lIsAvatar)
	{
		CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
	}

	// Check if it's a rotor!
	bool lIsRotor = false;
	const float lDamageFactor = pObject1->GetAttributeFloatValue(_T("DamageAbsorption"));
	if (lDamageFactor)
	{
		lCollisionImpactFactor *= lDamageFactor;
		TBC::ChunkyBoneGeometry* lGeometry = pObject1->GetStructureGeometry(pBody1Id);
		if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_HINGE &&
			lGeometry->GetGeometryType() == TBC::ChunkyBoneGeometry::GEOMETRY_BOX)
		{
			lIsRotor = true;
			TBC::ChunkyBoneGeometry* lHitBone = pObject2->GetPhysics()->GetBoneGeometry(pBody2Id);
			lCollisionImpactFactor *= Math::Lerp(1000.0f * lHitBone->GetImpactFactor(), 2.0f, lChildishness);
		}
		else if (lIsAvatar && pObject2->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC)
		{
			const float lDamageReduction = pObject2->GetAttributeFloatValue(_T("DamageReduction"));
			if (lDamageReduction)
			{
				lCollisionImpactFactor *= lDamageReduction;
			}
		}
		else if (!lIsAvatar && pObject2->GetInstanceId() == mAvatarId)
		{
			lCollisionImpactFactor *= 0.1f;
		}
	}
	else
	{
		lCollisionImpactFactor *= 0.01f;
	}

	float lForce = pForce.GetLength() * lCollisionImpactFactor;
	if (lChildishness > 0.1f)
	{
		float lUpFactor = 1 + 0.5f*(pObject1->GetOrientation()*Vector3DF(0,0,1)*Vector3DF(0,0,1));
		lUpFactor *= lUpFactor;
		lForce *= Math::Lerp(1.0f, 0.05f, lChildishness * lUpFactor);
		if (lForce < 0)
		{
			lForce = 0;
		}
	}
	// Take velocity into calculation if we're landing on a helipad (not using our rotors).
	if (lIsLandingPad && !lIsRotor)
	{
		lForce *= pForce*pObject1->GetVelocity() * (lCollisionImpactFactor / -15000);
	}
	lForce *= 5 - 4*lOrientationFactor;	// Sideways orientation means chopper not aligned.
	if (lForce > 15000)
	{
		lForce /= 30000;
		lForce *= 3 - 2*(pForce.GetNormalized()*Vector3DF(0,0,1));	// Sideways force means non-vertical landing or landing on non-flat surface.
		if (Cure::Health::Get(pObject1) > 0 && !mZoomPlatform)
		{
			float lForce2 = lForce*lForce;
			lForce2 /= pObject1->GetMass();
			Cure::Health::Add(pObject1, -lForce2, false);
		}
	}
}



Vector3DF DownwashManager::GetLandingTriggerPosition(Cure::ContextObject* pLevel) const
{
	deb_assert(pLevel);
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
					mLastLandingTriggerPosition = lTransform.GetPosition();
					return mLastLandingTriggerPosition;
				}
			}
		}
	}
	deb_assert(false);
	return Vector3DF();
}

void DownwashManager::EaseDown(Cure::ContextObject* pObject, const Vector3DF* pStartPosition)
{
	mAllLoadedTimer.Start();

	mAvatarCreateTimer.Start();
	mFlyTime.Stop();
	mFlyTime.PopTimeDiff();
	mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;
	mIsHitThisFrame = true;

	const Cure::ObjectPositionalData* lPositionalData = 0;
	pObject->UpdateFullPosition(lPositionalData);
	Cure::ObjectPositionalData* lNewPositionalData = (Cure::ObjectPositionalData*)lPositionalData->Clone();
	if (pStartPosition)
	{
		lNewPositionalData->mPosition.mTransformation.SetPosition(*pStartPosition);
	}
	lNewPositionalData->mPosition.mAcceleration = Vector3DF();
	lNewPositionalData->mPosition.mVelocity = Vector3DF();
	lNewPositionalData->mPosition.mAngularAcceleration = Vector3DF();
	lNewPositionalData->mPosition.mAngularVelocity = Vector3DF();
	bool lHasTouchedGround = false;
	float lStep = 1.0f;
	for (int x = 0; x < 100; ++x)
	{
		pObject->SetFullPosition(*lNewPositionalData, 0);
		const bool lIsColliding = GetPhysicsManager()->IsColliding(pObject->GetInstanceId());
		//mLog.Infof(_T("%s at step %f"), lIsColliding? _T("Is colliding") : _T("Not colliding"), lStep);
		if (lStep < 0.0001f && lIsColliding)
		{
			break;
		}
		lNewPositionalData->mPosition.mTransformation.GetPosition().z += lIsColliding? +lStep : -lStep;
		lHasTouchedGround |= lIsColliding;
		if (lHasTouchedGround)
		{
			lStep /= 2;
		}
	}
	pObject->SetFullPosition(*lNewPositionalData, 0);
	delete lNewPositionalData;
}

TransformationF DownwashManager::GetMainRotorTransform(const UiCure::CppContextObject* pChopper) const
{
	int lPhysIndex;
	str lMeshName;
	TransformationF lTransform;
	float lMeshScale;
	size_t lMeshCount = ((UiTbc::ChunkyClass*)pChopper->GetClass())->GetMeshCount();
	for (size_t x = 0; x < lMeshCount; ++x)
	{
		((UiTbc::ChunkyClass*)pChopper->GetClass())->GetMesh(x, lPhysIndex, lMeshName, lTransform, lMeshScale);
		if (lMeshName.find(_T("_rotor")) != str::npos)
		{
			return pChopper->GetMesh(x)->GetBaseTransformation();
		}
	}
	return lTransform;
}



void DownwashManager::OnPauseButton(UiTbc::Button*)
{
	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &DownwashManager::OnMenuAlternative), 0.8f, 0.8f);
	if (!d)
	{
		return;
	}
	d->SetColor(Color(110, 110, 110, 160), OFF_BLACK, BLACK, BLACK);

	UiTbc::FixedLayouter lLayouter(d);
	lLayouter.SetContentWidthPart(0.7f);
	lLayouter.SetContentHeightPart(0.7f);
	const int lMargin = d->GetPreferredHeight() / 30;
	lLayouter.SetContentMargin(lMargin);

	str lPilotName;
	CURE_RTVAR_GET(lPilotName, =, GetVariableScope(), RTVAR_GAME_PILOTNAME, _T("Pilot"));
	const int lDifficultyMode = GetControlMode();
	double lMasterVolume;
	CURE_RTVAR_GET(lMasterVolume, =, GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	bool lAllowToyMode;
	CURE_RTVAR_GET(lAllowToyMode, =, GetVariableScope(), RTVAR_GAME_ALLOWTOYMODE, false);
	double lRtrOffset;
	CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);

	UiTbc::TextField* lNameField = new UiTbc::TextField(0, WHITE, _T("pilot_name_field"));
	lNameField->SetText(lPilotName);
	lLayouter.AddWindow(lNameField, 0, 5, 0, 1);
	lNameField->SetHorizontalMargin(lNameField->GetPreferredHeight() / 3);
	//lNameField->SetKeyboardFocus();

	lLayouter.SetContentXMargin(0);
	UiTbc::RadioButton* lEasyButton = new UiTbc::RadioButton(Color(20, 30, 20), _T("Easy"));
	lEasyButton->SetPressColor(Color(50, 210, 40));
	lEasyButton->SetRoundedRadiusMask(0x9);
	lEasyButton->SetPressed(lDifficultyMode == 2);
	lLayouter.AddButton(lEasyButton, -2, 1, 5, 0, 3, false);
	UiTbc::RadioButton* lMediumButton = new UiTbc::RadioButton(Color(30, 30, 20), _T("Medium"));
	lMediumButton->SetPressColor(Color(170, 165, 10));
	lMediumButton->SetRoundedRadiusMask(0);
	lMediumButton->SetPressed(lDifficultyMode == 1);
	lLayouter.AddButton(lMediumButton, -3, 1, 5, 1, 3, false);
	UiTbc::RadioButton* lHardButton = new UiTbc::RadioButton(Color(30, 20, 20), _T("Hard"));
	lHardButton->SetPressColor(Color(230, 40, 30));
	lHardButton->SetRoundedRadiusMask(0x6);
	lHardButton->SetPressed(lDifficultyMode == 0);
	lLayouter.AddButton(lHardButton, -4, 1, 5, 2, 3, false);
	lLayouter.SetContentXMargin(lMargin);

	UiTbc::CheckButton* lToyModeButton = new UiTbc::CheckButton(Color(30, 70, 220), _T("Toy mode"));
	lToyModeButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	lToyModeButton->SetCheckedIcon(mCheckIcon->GetData());
	lToyModeButton->SetDisabledIcon(mLockIcon->GetData());
	lToyModeButton->Enable(lAllowToyMode);
	lToyModeButton->SetPressed(lRtrOffset > 0.1);
	lLayouter.AddButton(lToyModeButton, -5, 2, 5, 0, 2, false);

	UiTbc::CheckButton* lBedsideVolumeButton = new UiTbc::CheckButton(Color(190, 50, 180), _T("Bedside volume"));
	lBedsideVolumeButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	lBedsideVolumeButton->SetCheckedIcon(mCheckIcon->GetData());
	lBedsideVolumeButton->SetPressed(lMasterVolume < 0.5);
	lLayouter.AddButton(lBedsideVolumeButton, -6, 3, 5, 0, 2, false);
	UiTbc::Button* lHiscoreButton = new UiTbc::Button(Color(90, 50, 10), _T("High score"));
	lHiscoreButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	lLayouter.AddButton(lHiscoreButton, -7, 3, 5, 1, 2, true);

	UiTbc::Button* lRestartButton = new UiTbc::Button(Color(220, 110, 20), _T("Restart from first level"));
	lRestartButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	lLayouter.AddButton(lRestartButton, -8, 4, 5, 0, 1, true);

	UiTbc::Button* lCloseButton = new UiTbc::Button(Color(180, 60, 50), _T("X"));
	lLayouter.AddCornerButton(lCloseButton, -9);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void DownwashManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == -2)
	{
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
	}
	else if (pButton->GetTag() == -3)
	{
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 0.5);
	}
	else if (pButton->GetTag() == -4)
	{
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 0.0);
	}
	else if (pButton->GetTag() == -5)
	{
		const bool lToyMode = (pButton->GetState() == UiTbc::Button::PRESSED);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, lToyMode? 2.0 : 1.0);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, lToyMode? 1.0 : 0.0);
	}
	else if (pButton->GetTag() == -6)
	{
		const bool lBedsideVolume = (pButton->GetState() == UiTbc::Button::PRESSED);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, lBedsideVolume? 0.05 : 1.0);
	}
	else if (pButton->GetTag() == -7)
	{
		// Show hiscore!
	}
	else if (pButton->GetTag() == -8)
	{
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));
		mMenu->DismissDialog();
		HiResTimer::StepCounterShadow();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
		mSetRandomChopperColor = false;
	}
	else if (pButton->GetTag() == -9)
	{
		HiResTimer::StepCounterShadow();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
	}
}



void DownwashManager::DrawStick(Touchstick* pStick)
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (!pStick || !lAvatar)
	{
		return;
	}

	const PixelRect lArea = pStick->GetArea();
	std::vector<Vector2DF> lBoardShape;
	lBoardShape.push_back(Vector2DF((float)lArea.GetCenterX(), (float)lArea.GetCenterY()));
	lBoardShape.push_back(Vector2DF((float)lArea.mLeft, (float)lArea.mBottom));
	lBoardShape.push_back(Vector2DF((float)lArea.mRight, (float)lArea.mBottom));
	lBoardShape.push_back(Vector2DF((float)lArea.mRight, (float)lArea.mTop));
	const int lBoardRadius = lArea.GetWidth()/2;
	UiTbc::RectComponent::AddRadius(lBoardShape, lArea.mLeft+lBoardRadius, lArea.mTop+lBoardRadius, lBoardRadius, 0, +PIF/2);
	lBoardShape.push_back(lBoardShape[1]);
	mUiManager->GetPainter()->SetColor(Color(15, 10, 5));
	mUiManager->GetPainter()->SetAlphaValue(192);
	mUiManager->GetPainter()->DrawFan(lBoardShape, true);
	mUiManager->GetPainter()->SetAlphaValue(255);

	const int ow = lArea.GetWidth();
	const int r = (int)(ow*0.27f);	// Knob radius.
	const int lMargin = (int)(r*0.2f);
	float x, y;
	bool lIsPressing = false;
	pStick->GetValue(x, y, lIsPressing);
	if (!lIsPressing)
	{
		y = 0;
	}
	Vector2DF v(x, y);
	//v.Mul(2.0f * (ow - pStick->GetFingerRadius()-lMargin*2) / ow);
	const float lLength = v.GetLength();
	if (lLength > 1)
	{
		v.Div(lLength);
	}
	x = v.x;
	y = v.y;
	x = 0.5f*x + 0.5f;
	y = 0.5f*y + 0.5f;
	const int w = lArea.GetWidth()  - r*2 - lMargin*2;
	const int h = lArea.GetHeight() - r*2 - lMargin*2;
	mUiManager->GetPainter()->SetColor(lIsPressing? Color(0xD0, 0x50, 0x40) : Color(0xC0, 0x30, 0x20));
	mUiManager->GetPainter()->DrawArc(
		lArea.mLeft + lMargin + (int)(w*x),
		lArea.mTop  + lMargin + (int)(h*y),
		r*2, r*2, 0, 360, true);
}



void DownwashManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		MoveCamera();
		UpdateCameraPosition(false);
	}

	if (mAllLoadedTimer.IsStarted())
	{
		if (mAllLoadedTimer.QueryTimeDiff() > 5.0)
		{
			mAllLoadedTimer.Stop();
			strutil::strvec lResourceTypes;
			lResourceTypes.push_back(_T("RenderImg"));
			lResourceTypes.push_back(_T("Geometry"));
			lResourceTypes.push_back(_T("GeometryRef"));
			lResourceTypes.push_back(_T("Physics"));
			lResourceTypes.push_back(_T("PhysicsShared"));
			lResourceTypes.push_back(_T("RamImg"));
			lResourceTypes.push_back(_T("Sound3D"));
			lResourceTypes.push_back(_T("Sound2D"));
			GetResourceManager()->ForceFreeCache(lResourceTypes);
			GetResourceManager()->ForceFreeCache(lResourceTypes);	// Call again to release any dependent resources.
		}
	}

	if (mSlowmoTimer.IsStarted())
	{
		double lRtrOffset;
		CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
		if (mSlowmoTimer.QueryTimeDiff() < 3.5f)
		{
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 0.3*(lRtrOffset+1));
		}
		else
		{
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0+lRtrOffset);
			mSlowmoTimer.Stop();
		}
	}

	if (mAvatarCreateTimer.IsStarted())
	{
		if (mAvatarCreateTimer.QueryTimeDiff() > 0.5)
		{
			mAvatarCreateTimer.Stop();
		}
	}
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId, true);
	if (Lepra::GameTimer::GetRealTimeRatio() < 0.01f)
	{
		return;	// Wait until game runs again before resetting chopper.
	}
	if (mAvatarCreateTimer.IsStarted() || lAvatar)
	{
		mAvatarDied.Stop();
	}
	else if (mAvatarId)
	{
		mAvatarDied.TryStart();
		if (mAvatarDied.QueryTimeDiff() > 0.1f)
		{
			// If we're currently loading level, we'll load a chopper when level is complete.
			// Otherwise we just load one right away.
			if (mOldLevel)
			{
				mAvatarId = 0;
			}
			else
			{
				CreateChopper(_T("helicopter_01"));
			}
		}
	}

	Parent::ScriptPhysicsTick();
}

void DownwashManager::HandleWorldBoundaries()
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && !mTooFarAwayTimer.IsStarted() && lAvatar->GetPosition().GetLength() > 300)
	{
		mTooFarAwayTimer.Start();
		Life::HomingProjectile* lRocket = (Life::HomingProjectile*)Parent::CreateContextObject(_T("missile"), Cure::NETWORK_OBJECT_LOCAL_ONLY);
		lRocket->SetTarget(mAvatarId);
		Vector3DF lFirePosition(lAvatar->GetPosition().x, 0, 0);
		lFirePosition.Normalize(200);
		lFirePosition += lAvatar->GetPosition();
		lFirePosition.z += 50;
		lRocket->SetInitialTransform(TransformationF(QuaternionF(), lFirePosition));
		lRocket->StartLoading();
	}
	else if (mTooFarAwayTimer.IsStarted() && mTooFarAwayTimer.QueryTimeDiff() > 25.0)
	{
		mTooFarAwayTimer.Stop();
	}

	std::vector<Cure::GameObjectId> lLostObjectArray;
	typedef Cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& lObjectTable = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->IsLoaded() && lObject->GetPhysics())
		{
			const Vector3DF lPosition = lObject->GetPosition();
			if (!Math::IsInRange(lPosition.x, -1000.0f, +1000.0f) ||
				!Math::IsInRange(lPosition.y, -1000.0f, +1000.0f) ||
				!Math::IsInRange(lPosition.z, -1000.0f, +1000.0f))
			{
				lLostObjectArray.push_back(lObject->GetInstanceId());
			}
		}
	}
	if (!lLostObjectArray.empty())
	{
		ScopeLock lLock(GetTickLock());
		std::vector<Cure::GameObjectId>::const_iterator y = lLostObjectArray.begin();
		for (; y != lLostObjectArray.end(); ++y)
		{
			DeleteContextObject(*y);
		}
	}
}

void DownwashManager::MoveCamera()
{
	UpdateChopperColor(0.05f);
	UpdateCameraDistance();

	float lHalfCamDistance;
	CURE_RTVAR_GET(lHalfCamDistance, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 110.0);
	lHalfCamDistance /= 2;
	mCameraPreviousPosition = mCameraTransform.GetPosition();
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	Vector3DF lAvatarPosition = mHelicopterPosition;
	float lSpeedX = 0;
	float ax;
	CURE_RTVAR_GET(ax, =(float), GetVariableScope(), RTVAR_UI_3D_CAMXANGLE, 0.0);
	if (lAvatar && lAvatar->GetPhysics()->GetEngineCount() >= 3)
	{
		lAvatarPosition = lAvatar->GetPosition();
		float ox, oy, oz;
		CURE_RTVAR_GET(ox, =(float), GetVariableScope(), RTVAR_UI_3D_CAMXOFFSET, 0.0);
		CURE_RTVAR_GET(oy, =(float), GetVariableScope(), RTVAR_UI_3D_CAMYOFFSET, 0.0);
		CURE_RTVAR_GET(oz, =(float), GetVariableScope(), RTVAR_UI_3D_CAMZOFFSET, 0.0);
		lAvatarPosition.x += ox;
		lAvatarPosition.y += oy;
		lAvatarPosition.z += oz;
		mHelicopterPosition = lAvatarPosition;
		lSpeedX = lAvatar->GetVelocity().x;
		if (Cure::Health::Get(lAvatar) <= 0)
		{
			mSetRandomChopperColor = true;
		}
	}
	TransformationF lTargetTransform(QuaternionF(), lAvatarPosition + Vector3DF(0, -2*lHalfCamDistance, 0));
	++mPostZoomPlatformFrameCount;
	if (mZoomPlatform)
	{
		Cure::ContextObject* lLevel = mOldLevel? mOldLevel : mLevel;
		lTargetTransform.GetPosition() = GetLandingTriggerPosition(lLevel) + Vector3DF(0, 0, 10);
		mPostZoomPlatformFrameCount = 0;
	}
	else if (mHitGroundFrameCount >= -STILL_FRAMES_UNTIL_CAM_PANS)
	{
		lTargetTransform.GetPosition().z += lHalfCamDistance;
	}

	Vector3DF lCamXZPos(mCameraTransform.GetPosition());
	lCamXZPos.y = 0;
	if (lCamXZPos.GetDistance(lAvatarPosition) > 2*lHalfCamDistance)
	{
		mCameraTransform = lTargetTransform;
		mCameraSpeed = 0;
	}
	else
	{
		Vector3DF lDelta = Math::Lerp(mCameraTransform.GetPosition(), lTargetTransform.GetPosition(), 0.08f) - mCameraTransform.GetPosition();
		mCameraTransform.GetPosition().x += lDelta.x;
		//mCameraTransform.GetPosition().z += lDelta.z;
		mCameraTransform.GetPosition().z += Math::SmoothClamp(lDelta.z, -2.0f, +2.0f, 0.2f);
		mCameraTransform.GetPosition().y += Math::SmoothClamp(lDelta.y, -1.3f, +2.0f, 0.2f);
	}

	// Angle.
	const float x = lAvatarPosition.y - mCameraTransform.GetPosition().y;
	const float y = lAvatarPosition.z - mCameraTransform.GetPosition().z;
	const float lXAngle = ::atan2(y, x) + ax;
	lTargetTransform.GetOrientation().RotateAroundOwnX(lXAngle);
	if (mPostZoomPlatformFrameCount > 10)
	{
		mCameraSpeed = Math::Lerp(mCameraSpeed, lSpeedX*0.4f, 0.05f);
		//mCameraSpeed = Math::Clamp(mCameraSpeed, -3.0f, +4.0f);
		const float z = lAvatarPosition.x + mCameraSpeed - mCameraTransform.GetPosition().x;
		const int lSmoothSteps = mPostZoomPlatformFrameCount-10;
		const float lSmoothFactor = (lSmoothSteps >= 100)? 1.0f : lSmoothSteps/100.0f;
		const float lZAngle = -::atan2(z, x) * lSmoothFactor;
		lTargetTransform.GetOrientation().RotateAroundWorldZ(lZAngle);
	}
	mCameraTransform.GetOrientation().Slerp(mCameraTransform.GetOrientation(), lTargetTransform.GetOrientation(), 0.5f);
	/*float lFoV = Math::Lerp(30.0f, 60.0f, mCameraSpeed);
	lFoV = Math::SmoothClamp(lFoV, 30.0f, 60.0f, 0.4f);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, lFoV);*/

	/*CURE_RTVAR_ARITHMETIC(GetVariableScope(), "cam_ang", double, +, 0.01, 0.0, 3000.0);
	QuaternionF q;
	mCameraTransform = TransformationF(QuaternionF(), Vector3DF(0, -300, 50));
	mCameraTransform.RotateAroundAnchor(Vector3DF(), Vector3DF(1,0,1), (float)CURE_RTVAR_SLOW_TRYGET(GetVariableScope(), "cam_ang", 0.0));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);*/

	lAvatarPosition.y -= 5.0f;	// Move closer to edge to avoid jitter.
	UpdateMassObjects(lAvatarPosition);
}

void DownwashManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
		if (lAvatar && lAvatar->IsLoaded())
		{
			TransformationF lMicPos(QuaternionF(), mHelicopterPosition);
			lMicPos.GetPosition().z -= 2;
			mUiManager->SetMicrophonePosition(lMicPos, mMicrophoneSpeed);
		}
	}
}



void DownwashManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
{
	cx -= 0.5f;

	const float ca = ::cos(pAngle);
	const float sa = ::sin(pAngle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const Vector2DF c[] = { Vector2DF(x, y), Vector2DF(x+w*ca, y-w*sa), Vector2DF(x+w*ca+h*sa, y+h*ca-w*sa), Vector2DF(x+h*sa, y+h*ca) };
	const Vector2DF t[] = { Vector2DF(0, 0), Vector2DF(1, 0), Vector2DF(1, 1), Vector2DF(0, 1) };
#define V(z) std::vector<Vector2DF>(z, z+LEPRA_ARRAY_COUNT(z))
	mUiManager->GetPainter()->DrawImageFan(pImageId, V(c), V(t));
}



void DownwashManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
		UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
}

void DownwashManager::RendererTextureLoadCallback(UiCure::UserRendererImageResource* pResource)
{
	mUiManager->GetRenderer()->TryAddGeometryTexture(mArrowBillboardId, pResource->GetData());
}



void DownwashManager::DisableDepth()
{
	mUiManager->GetRenderer()->EnableAllLights(false);
	mUiManager->GetRenderer()->SetDepthWriteEnabled(false);
	mUiManager->GetRenderer()->SetDepthTestEnabled(false);
}

void DownwashManager::EnableDepth()
{
	mUiManager->GetRenderer()->EnableAllLights(true);
	mUiManager->GetRenderer()->SetDepthWriteEnabled(true);
	mUiManager->GetRenderer()->SetDepthTestEnabled(true);
}



LOG_CLASS_DEFINE(GAME, DownwashManager);



}
