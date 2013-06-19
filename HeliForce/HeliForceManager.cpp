
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "HeliForceManager.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/Elevator.h"
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
#include "../Life/LifeClient/HomingProjectile.h"
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
#include "../UiCure/Include/UiDebugRenderer.h"
#include "../UiCure/Include/UiExhaustEmitter.h"
#include "../UiCure/Include/UiJetEngineEmitter.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiProps.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../UiLepra/Include/UiTouchstick.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiBillboardGeometry.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "../UiTBC/Include/UiRenderer.h"
#include "CenteredMachine.h"
#include "Autopilot.h"
#include "HeliForceConsoleManager.h"
#include "HeliForceTicker.h"
#include "LandingTrigger.h"
#include "Level.h"
#include "RtVar.h"
#include "StoneEater.h"
#include "Sunlight.h"
#include "Version.h"

#define LAST_LEVEL			3
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
	mHemisphere(0),
	mHemisphereUvTransform(0),
	mRenderHemisphere(true),
	mSunlight(0),
	mCameraTransform(QuaternionF(), Vector3DF(0, -200, 100)),
	mCameraSpeed(0),
	mZoomPlatform(false),
	mPostZoomPlatformFrameCount(100),
	mHitGroundFrameCount(STILL_FRAMES_UNTIL_CAM_PANS),
	mIsHitThisFrame(false),
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	mFireButton(0),
#endif // Touch or emulated touch.
	mStick(0),
	mStickImage(0),
	mDirectionImage(0),
	mArrow(0),
	mArrowBillboard(0),
	mArrowBillboardId(0),
	mArrowTotalPower(0),
	mArrowAngle(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("plastic"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("thump"),		UiCure::CollisionSoundManager::SoundResourceInfo(5.0f, 0.5f, 1.0f));

	SetConsoleManager(new HeliForceConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, -0.1f, 0.2f);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTHEIGHT, 30.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTFLAGS, 1);
}

HeliForceManager::~HeliForceManager()
{
	Close();

	delete mStick;
	mStick = 0;
}

void HeliForceManager::LoadSettings()
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);

	Parent::LoadSettings();

	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);

	//CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 0.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_ENABLECLEAR, false);

#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	const str lSchtickName = _T("Touchstick");
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_UP3D, lSchtickName+_T(".AxisY-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_DOWN3D, lSchtickName+_T(".AxisY+"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_LEFT3D, lSchtickName+_T(".AxisX-"));
	CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_CTRL_STEER_RIGHT3D, lSchtickName+_T(".AxisX+"));
#endif // Touch device or emulated touch device
}

void HeliForceManager::SaveSettings()
{
#ifndef EMULATE_TOUCH
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
#endif // Computer or touch device.
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
	if (lOk)
	{
		mStickImage = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mStickImage->Load(GetResourceManager(), _T("stick.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &HeliForceManager::PainterImageLoadCallback));

		mDirectionImage = new UiCure::UserPainterKeepImageResource(mUiManager, UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
		mDirectionImage->Load(GetResourceManager(), _T("direction.png"),
			UiCure::UserPainterKeepImageResource::TypeLoadCallback(this, &HeliForceManager::PainterImageLoadCallback));

		mArrow = new UiCure::UserRendererImageResource(mUiManager, false);
		mArrow->Load(GetResourceManager(), _T("arrow.png"),
			UiCure::UserRendererImageResource::TypeLoadCallback(this, &HeliForceManager::RendererTextureLoadCallback));
		mArrowBillboard = new UiTbc::BillboardGeometry(1/4.0f, 1);
		mArrowBillboardId = mUiManager->GetRenderer()->AddGeometry(mArrowBillboard, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::FORCE_NO_SHADOWS);
	}
	return lOk;
}

void HeliForceManager::Close()
{
	ScopeLock lLock(GetTickLock());
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	delete mFireButton;
	mFireButton = 0;
#endif // Touch or emulated touch.
	if (mSunlight)
	{
		delete mSunlight;
		mSunlight = 0;
	}
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



bool HeliForceManager::Render()
{
	if (mHemisphere && mHemisphere->IsLoaded() && mRenderHemisphere)
	{
		if (!mHemisphere->GetMesh(0)->GetUVAnimator())
		{
			mHemisphere->GetMesh(0)->SetUVAnimator(mHemisphereUvTransform);
			mHemisphere->GetMesh(0)->SetAlwaysVisible(false);
			mHemisphere->GetMesh(0)->SetPreRenderCallback(TBC::GeometryBase::RenderCallback(this, &HeliForceManager::DisableDepth));
			mHemisphere->GetMesh(0)->SetPostRenderCallback(TBC::GeometryBase::RenderCallback(this, &HeliForceManager::EnableDepth));
		}
		Vector3DF lPosition = mCameraTransform.GetPosition();
		lPosition.x = -lPosition.x;
		lPosition.y = 0;
		mHemisphereUvTransform->GetBones()[0].GetRelativeBoneTransformation(0).GetPosition() = lPosition * 0.003f;

		mUiManager->GetRenderer()->RenderRelative(mHemisphere->GetMesh(0), 0);
		mUiManager->GetRenderer()->SetDepthTestEnabled(true);
		mUiManager->GetRenderer()->SetDepthWriteEnabled(true);
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

bool HeliForceManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}

	if (mStick)
	{
		DrawStick(mStick);
		mStick->ResetTap();
	}

	const Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar)
	{

		Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)lAvatar->GetAttribute(_T("float_health"));
		const str lInfo = lHealth? strutil::DoubleToString(lHealth->GetValue()*100, 0) : _T("");
		mUiManager->GetPainter()->SetColor(Color(255, 0, 0, 255), 0);
		mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		mUiManager->GetPainter()->PrintText(lInfo, mRenderArea.mLeft + 200, 10);

		const bool lIsFlying = mFlyTime.IsStarted();
		const double lTime = mFlyTime.QuerySplitTime();
		const int lSec = (int)lTime;
		const str lIntTimeString = strutil::Format(_T("%i"), lSec);
		const str lTimeString = strutil::Format((lIsFlying || !lTime)? _T("%.1f s") : _T("%.3f s"), lTime);
		int w = mUiManager->GetPainter()->GetStringWidth(lIntTimeString);
		mUiManager->GetPainter()->PrintText(lTimeString, 50 - w, 3);

		if (lAvatar->GetPhysics()->GetEngineCount() >= 3 && mDirectionImage->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
			mLevel && mLevel->IsLoaded())
		{
			if (mDirectionImageTimer.QueryTimeDiff() >= 0.5)
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
			if (lPathDistance > 40.0f)
			{
				if (mDirectionImageTimer.IsStarted())
				{
					const Vector3DF lPos3d = mCameraTransform.GetPosition();
					const Vector2DF lPos(lPos3d.x, lPos3d.z);
					const Vector3DF lGoal3d = mLastLandingTriggerPosition;
					const Vector2DF lGoal(lGoal3d.x, lGoal3d.z);
					const float a = (lGoal-lPos).GetAngle() - PIF/2;
					const float lTouchSideScale = 1.28f;	// Inches.
					const float lTouchScale = lTouchSideScale / (float)mUiManager->GetDisplayManager()->GetPhysicalScreenSize();
					const float lWantedSize = mUiManager->GetCanvas()->GetWidth() * lTouchScale * 2;
					float lSize = (float)mDirectionImage->GetRamData()->GetWidth();
					while (lWantedSize >= lSize*2) lSize *= 2;
					while (lWantedSize <= lSize/2) lSize /= 2;
					// Find out the screen coordinate of the chopper, so we can place our arrow around that.
					const Vector3DF lCamDirection = mCameraTransform.GetOrientation() * Vector3DF(0,1,0);
					const Vector3DF lCamLookAtPointInChopperPlane = lPos3d + lCamDirection * -lPos3d.y;
					float lFoV;
					CURE_RTVAR_GET(lFoV, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
					lFoV /= 45.0f;
					const Vector3DF lDelta = (mAutopilot->GetLastAvatarPosition() - lPos3d) / (-lPos3d.y * lFoV);
					const float lDistance = mUiManager->GetCanvas()->GetWidth() / 6.0f;
					const float x = mUiManager->GetCanvas()->GetWidth() /2.0f - lDistance*::sin(a);
					const float y = mUiManager->GetCanvas()->GetHeight()/2.0f - lDistance*::cos(a);
					DrawImage(mDirectionImage->GetData(), x, y, lSize, lSize, a);
				}
			}
			else
			{
				mDirectionImageTimer.Start();
			}
		}
	}

	return true;
}

void HeliForceManager::DrawSyncDebugInfo()
{
	Parent::DrawSyncDebugInfo();

	if (GetLevel() && GetLevel()->QueryPath()->GetPath(0))
	{
		UiCure::DebugRenderer lDebugRenderer(GetVariableScope(), GetContext(), 0, GetTickLock());
		lDebugRenderer.RenderSpline(mUiManager, GetLevel()->QueryPath()->GetPath(0));
	}
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
	if (lObject && !mZoomPlatform)
	{
		return SetAvatarEnginePower(lObject, pAspect, pPower);
	}
	return SetAvatarEnginePower(lObject, pAspect, 0.0f);
}



void HeliForceManager::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
{
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
	if (pExplosive->GetClassId().find(_T("mine")) != str::npos)
	{
		lStartFireColor.Set(0.9f, 1.0f, 0.8f);
		lFireColor.Set(0.3f, 0.7f, 0.2f);
		lStartSmokeColor.Set(0.3f, 0.35f, 0.3f);
		lSmokeColor.Set(0.2f, 0.4f, 0.2f);
		lShrapnelColor.Set(0.5f, 0.5f, 0.1f);
	}
	lParticleRenderer->CreateExplosion(pPosition, pStrength * 1.5f, u, 1, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles*2, lParticles*2, lParticles, lParticles/2);

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
		const float lForce = Life::Explosion::CalculateForce(lPhysicsManager, lObject, pPosition, pStrength);
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)lObject->GetAttribute(_T("float_health"));
			if (lHealth && !mZoomPlatform)
			{
				lHealth->SetValue(lHealth->GetValue() - lForce*Random::Normal(0.51f, 0.05f, 0.3f, 0.5f));
			}
			x->second->ForceSend();
		}
		Life::Explosion::PushObject(lPhysicsManager, lObject, pPosition, pStrength*0.1f);
	}
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



bool HeliForceManager::DidFinishLevel()
{
	mLog.AHeadline("Level done!");
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && lAvatar->GetPhysics()->GetEngineCount() >= 3)
	{
		UiCure::UserSound3dResource* lFinishSound = new UiCure::UserSound3dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, mCameraTransform.GetPosition(), Vector3DF(), 5.0f, 1.0f);
		mZoomPlatform = true;
		return true;
	}
	return false;
}

void HeliForceManager::NextLevel()
{
	if (GetContext()->GetObject(mAvatarId))
	{
		mOldLevel = mLevel;
		int lLevelNumber = 0;
		strutil::StringToInt(mOldLevel->GetClassId().substr(6), lLevelNumber);
		++lLevelNumber;
		if (lLevelNumber > LAST_LEVEL)
		{
			lLevelNumber = 0;
		}
		str lNewLevelName = strutil::Format(_T("level_%.2i"), lLevelNumber);
		mLevel = (Level*)Parent::CreateContextObject(lNewLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
		mLevel->StartLoading();
	}
}



Level* HeliForceManager::GetLevel() const
{
	if (mLevel && mLevel->IsLoaded())
	{
		return mLevel;
	}
	return 0;
}

Cure::ContextObject* HeliForceManager::GetAvatar() const
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	return lAvatar;
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
	mLevel = (Level*)Parent::CreateContextObject(_T("level_04"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
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

void HeliForceManager::CreateChopper(const str& pClassId)
{
	mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;
	mZoomPlatform = false;

	Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
	assert(lSpawner);
	Cure::ContextObject* lAvatar = Parent::CreateContextObject(pClassId, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lAvatar->QuerySetChildishness(1);
	lSpawner->PlaceObject(lAvatar);
	mAvatarId = lAvatar->GetInstanceId();
	mAvatarCreateTimer.Start();
	lAvatar->StartLoading();
}

void HeliForceManager::UpdateChopperColor(float pLerp)
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
		lAvatar->GetMesh(x)->GetBasicMaterialSettings().mAmbient = Math::Lerp(lAvatar->GetMesh(x)->GetBasicMaterialSettings().mAmbient, lClass->GetMaterial(x).mAmbient * lLevelBrightness, pLerp);
		lAvatar->GetMesh(x)->GetBasicMaterialSettings().mDiffuse = Math::Lerp(lAvatar->GetMesh(x)->GetBasicMaterialSettings().mDiffuse, lClass->GetMaterial(x).mDiffuse * lLevelBrightness, pLerp);
	}
}

void HeliForceManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void HeliForceManager::UpdateTouchstickPlacement()
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
		mStick  = new Touchstick(mUiManager->GetInputManager(), Touchstick::MODE_RELATIVE_CENTER, PixelRect(0, 0, 10, 10),  0, lMinimumTouchRadius);
		mStick->SetUniqueIdentifier(_T("Touchstick"));
	}
	PixelRect lRightStickArea(mRenderArea);
	int lFingerSize = (int)(lRightStickArea.GetWidth() * lTouchScale);
	lRightStickArea.mLeft = mRenderArea.GetWidth() - lFingerSize;
	lRightStickArea.mTop = lRightStickArea.mBottom - lFingerSize;
	lRightStickArea.mBottom += (int)(lFingerSize*1.1f);
	mStick->Move(lRightStickArea, 0);
#endif // Touch or emulated touch
}

void HeliForceManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);

	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPlaybackMode != PLAYBACK_PLAY && lPhysicsStepCount > 0 && mAllowMovementInput)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);

		if (lObject)
		{
			// Control steering.
			float lChildishness;
			CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
			if (mFlyTime.GetTimeDiff() < 0.01f)
			{
				lChildishness = 0;	// Don't help when not even started yet.
			}
			const float lChildSteerFactor = 1 - lChildishness * 0.5f;
			const Vector3DF lAutoPilot = mAutopilot->GetSteering() * lChildishness;
			const Life::Options::Steering& s = mOptions.GetSteeringControl();
#define S(dir) s.mControl[Life::Options::Steering::CONTROL_##dir]
			float lYaw, _;
			lObject->GetOrientation().GetEulerAngles(lYaw, _, _);
			const float lUserWantedDirection = (S(RIGHT3D) - S(LEFT3D)) * lChildSteerFactor;
			const float lWantedDirection = lUserWantedDirection + lAutoPilot.x;
			const float lCurrentDirection = lObject->GetVelocity().x * 0.05f;
			const float lWantedChange = lWantedDirection-lCurrentDirection;
			const float lPowerFwdRev = -::sin(lYaw) * lWantedChange;
			const float lPowerLeftRight = ::cos(lYaw) * lWantedChange;
			SetAvatarEnginePower(lObject, 4, lPowerFwdRev);
			SetAvatarEnginePower(lObject, 5, lPowerLeftRight);
			const float lUserPower = (S(UP3D) - S(DOWN3D)) * lChildSteerFactor;
			const float lPower = lUserPower + lAutoPilot.z;
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
	if (mZoomPlatform)
	{
		pAvatar->SetEnginePower(pAspect, 0);
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

	const Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (mHitGroundFrameCount <= 0 && mLevel && mLevel->IsLoaded() && lAvatar)
	{
		if (lAvatar->GetPhysics()->GetEngineCount() < 3)
		{
			mFlyTime.Stop();
		}
		else if (!mFlyTime.IsStarted())
		{
			Cure::Spawner* lSpawner = GetAvatarSpawner(mLevel->GetInstanceId());
			assert(lSpawner);
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
	if (pClassId == _T("missile"))
	{
		lObject = new Life::HomingProjectile(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
	}
	else if (pClassId == _T("grenade") || pClassId == _T("rocket"))
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
		lObject = new CenteredMachine(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
		lObject->DeleteAttribute(_T("float_health"));
	}
	else if (strutil::StartsWith(pClassId, _T("level_")))
	{
		UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, 0.5f, 1, 10, 2);
		Level* lLevel = new Level(GetResourceManager(), pClassId, mUiManager, lGravelParticleEmitter);
		lLevel->DisableRootShadow();
		lObject = lLevel;
	}
	else if (strutil::StartsWith(pClassId, _T("helicopter_")))
	{
		UiCure::Machine* lMachine = new CenteredMachine(GetResourceManager(), pClassId, mUiManager, (HeliForceManager*)this);
		lMachine->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), mUiManager));
		lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	else
	{
		UiCure::Machine* lMachine = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
		lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lObject = lMachine;
	}
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

Cure::ContextObject* HeliForceManager::CreateLogicHandler(const str& pType)
{
	if (pType == _T("spawner"))
	{
		return new Life::Spawner(GetContext());
	}
	else if (pType == _T("real_time_ratio"))
	{
		return new LandingTrigger(GetContext());
	}
	else if (pType == _T("stone_eater"))
	{
		return new StoneEater(GetContext());
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
	return (0);
}

void HeliForceManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		if (pObject->GetInstanceId() == mAvatarId)
		{
			log_volatile(mLog.Debug(_T("Yeeha! Loaded avatar!")));
			UpdateChopperColor(1.0f);
			EaseDown(pObject, 0);
			mHitGroundFrameCount = STILL_FRAMES_UNTIL_CAM_PANS;
		}
		else if (pObject == mLevel)
		{
			OnLevelLoadCompleted();
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

void HeliForceManager::OnLevelLoadCompleted()
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
	if (mLevel->GetBackgroundName().empty())
	{
		mRenderHemisphere = false;
	}
	else
	{
		mRenderHemisphere = true;
		mHemisphere->ReplaceTexture(0, mLevel->GetBackgroundName());
	}
	GetLandingTriggerPosition(mLevel);	// Update shadow landing trigger position.
	mZoomPlatform = false;
}

void HeliForceManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);

	bool lIsAvatar = (pObject1->GetInstanceId() == mAvatarId);

	float lChildishness = 0.0f;

	if (!mIsHitThisFrame && lIsAvatar)
	{
		mIsHitThisFrame = true;
		++mHitGroundFrameCount;
	}

	// Check if we're just stabilizing on starting pad.
	if (lIsAvatar && mAvatarCreateTimer.IsStarted())
	{
		return;
	}

	// Check if we're colliding with a smooth landing pad.
	float lCollisionImpactFactor = 3;
	if (lIsAvatar && pObject2 == mLevel)
	{
		const TBC::ChunkyClass::Tag* lTag = mLevel->GetClass()->GetTag(_T("anything"));
		std::vector<int>::const_iterator x = lTag->mBodyIndexList.begin();
		for (; x != lTag->mBodyIndexList.end(); ++x)
		{
			if (pObject2->GetPhysics()->GetBoneGeometry(*x)->GetBodyId() == pBody2Id)
			{
				lCollisionImpactFactor = 1;
				mFlyTime.Stop();
				break;
			}
		}
	}

	if (lIsAvatar)
	{
		CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
	}

	// Check if it's a rotor!
	if (pObject1->GetClassId().find(_T("helicopter_")) != str::npos)
	{
		TBC::ChunkyBoneGeometry* lGeometry = pObject1->GetStructureGeometry(pBody1Id);
		if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_HINGE &&
			lGeometry->GetGeometryType() == TBC::ChunkyBoneGeometry::GEOMETRY_BOX)
		{
			lCollisionImpactFactor *= Math::Lerp(1000.0f, 2.0f, lChildishness);
		}
	}

	float lForce = pForce.GetLength() * lCollisionImpactFactor;
	if (lChildishness > 0.1f)
	{
		float lUpFactor = 1 + 0.5f*(pObject1->GetOrientation()*Vector3DF(0,0,1)*Vector3DF(0,0,1));
		lUpFactor *= lUpFactor;
		lForce *= Math::Lerp(1.0f, 0.05f, lChildishness * lUpFactor);
	}
	if (lForce > 15000)
	{
		float lForce2 = lForce;
		lForce2 /= 3000;
		lForce2 *= 3 - 2*(pForce.GetNormalized()*Vector3DF(0,0,1));	// Sideways force means non-vertical landing or landing on non-flat surface.
		lForce2 *= 10 - 9*(pObject1->GetOrientation()*Vector3DF(0,0,1)*Vector3DF(0,0,1));	// Sideways orientation means chopper not aligned.
		Cure::FloatAttribute* lHealth = (Cure::FloatAttribute*)pObject1->GetAttribute(_T("float_health"));
		if (lHealth && lHealth->GetValue() > 0 && !mZoomPlatform)
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
					mLastLandingTriggerPosition = lTransform.GetPosition();
					return mLastLandingTriggerPosition;
				}
			}
		}
	}
	assert(false);
	return Vector3DF();
}

float HeliForceManager::EaseDown(Cure::ContextObject* pObject, const Vector3DF* pStartPosition)
{
	mFlyTime.Stop();
	mFlyTime.PopTimeDiff();

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

TransformationF HeliForceManager::GetMainRotorTransform(const UiCure::CppContextObject* pChopper) const
{
	int lPhysIndex;
	str lMeshName;
	TransformationF lTransform;
	size_t lMeshCount = ((UiTbc::ChunkyClass*)pChopper->GetClass())->GetMeshCount();
	for (size_t x = 0; x < lMeshCount; ++x)
	{
		((UiTbc::ChunkyClass*)pChopper->GetClass())->GetMesh(x, lPhysIndex, lMeshName, lTransform);
		if (lMeshName.find(_T("_rotor")) != str::npos)
		{
			return pChopper->GetMesh(x)->GetBaseTransformation();
		}
	}
	return lTransform;
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
	if (!pStick || mStickImage->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE  || !lAvatar)
	{
		return;
	}

	PixelRect lArea = pStick->GetArea();
	lArea.mBottom = lArea.mTop + lArea.GetWidth();
	lArea.Shrink(8);
	DrawImage(mStickImage->GetData(), (float)lArea.GetCenterX(), (float)lArea.GetCenterY(), (float)lArea.GetWidth(), (float)lArea.GetHeight(), 0);
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

	if (mAvatarCreateTimer.IsStarted())
	{
		if (mAvatarCreateTimer.QueryTimeDiff() > 2.0)
		{
			mAvatarCreateTimer.Stop();
		}
	}
	if (mAvatarCreateTimer.IsStarted() || GetContext()->GetObject(mAvatarId))
	{
		mAvatarDied.Stop();
	}
	else if (mAvatarId)
	{
		mAvatarDied.TryStart();
		if (mAvatarDied.QueryTimeDiff() > 0.1f)
		{
			CreateChopper(_T("helicopter_01"));
		}
	}

	Parent::ScriptPhysicsTick();
}

void HeliForceManager::HandleWorldBoundaries()
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && !mTooFarAwayTimer.IsStarted() && lAvatar->GetPosition().GetLength() > 300)
	{
		mTooFarAwayTimer.Start();
		Life::HomingProjectile* lRocket = (Life::HomingProjectile*)Parent::CreateContextObject(_T("missile"), Cure::NETWORK_OBJECT_LOCAL_ONLY);
		lRocket->SetTarget(mAvatarId);
		Vector3DF lFirePosition(lAvatar->GetPosition().x, 0, 0);
		lFirePosition.Normalize(500);
		lFirePosition += lAvatar->GetPosition();
		lFirePosition.z += 200;
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

void HeliForceManager::MoveCamera()
{
	Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar)
	{
		UpdateChopperColor(0.1f);

		mCameraPreviousPosition = mCameraTransform.GetPosition();
		Vector3DF lAvatarPosition = lAvatar->GetPosition();
		TransformationF lTargetTransform(QuaternionF(), lAvatarPosition + Vector3DF(0, -60, 0));
		++mPostZoomPlatformFrameCount;
		if (lAvatar->GetAttributeFloatValue(_T("float_health")) <= 0)
		{
			return;
		}
		else if (mZoomPlatform)
		{
			Cure::ContextObject* lLevel = mOldLevel? mOldLevel : mLevel;
			lTargetTransform.GetPosition() = GetLandingTriggerPosition(lLevel) + Vector3DF(0, 0, 10);
			mPostZoomPlatformFrameCount = 0;
		}
		else if (mHitGroundFrameCount >= STILL_FRAMES_UNTIL_CAM_PANS)
		{
			lTargetTransform.GetPosition().z += 30;
		}
		Vector3DF lDelta = Math::Lerp(mCameraTransform.GetPosition(), lTargetTransform.GetPosition(), 0.08f) - mCameraTransform.GetPosition();

		const float lCamNormalizedDistance = mCameraTransform.GetPosition().GetDistance(lAvatarPosition) / 40;
		if (lCamNormalizedDistance < 2.5f)
		{
			const float lMaxCamSpeed = Math::SmoothClamp(lCamNormalizedDistance, 0.1f, 5.0f, 0.3f);
			if (lDelta.GetLength() > lMaxCamSpeed)
			{
				lDelta.Normalize(lMaxCamSpeed);
			}
			mCameraTransform.GetPosition() += lDelta;
		}
		else
		{
			mCameraTransform = lTargetTransform;
		}

		// Angle.
		const float x = lAvatarPosition.y - mCameraTransform.GetPosition().y;
		const float y = lAvatarPosition.z - mCameraTransform.GetPosition().z;
		const float lXAngle = ::atan2(y, x);
		lTargetTransform.GetOrientation().RotateAroundOwnX(lXAngle);
		if (mPostZoomPlatformFrameCount > 10)
		{
			const float z = lAvatarPosition.x - mCameraTransform.GetPosition().x;
			const int lSmoothSteps = mPostZoomPlatformFrameCount-10;
			const float lSmoothFactor = (lSmoothSteps >= 100)? 1.0f : lSmoothSteps/100.0f;
			const float lZAngle = -::atan2(z, x) * lSmoothFactor;
			lTargetTransform.GetOrientation().RotateAroundWorldZ(lZAngle);
		}
		mCameraTransform.GetOrientation().Slerp(mCameraTransform.GetOrientation(), lTargetTransform.GetOrientation(), 0.5f);
		mCameraSpeed = Math::Lerp(mCameraSpeed, lAvatar->GetVelocity().GetLength()/10, 0.02f);
		float lFoV = Math::Lerp(30.0f, 60.0f, mCameraSpeed);
		lFoV = Math::SmoothClamp(lFoV, 30.0f, 60.0f, 0.4f);
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, lFoV);

		/*CURE_RTVAR_ARITHMETIC(GetVariableScope(), "cam_ang", double, +, 0.01, 0.0, 3000.0);
		QuaternionF q;
		mCameraTransform = TransformationF(QuaternionF(), Vector3DF(0, -300, 50));
		mCameraTransform.RotateAroundAnchor(Vector3DF(), Vector3DF(1,0,1), (float)CURE_RTVAR_SLOW_TRYGET(GetVariableScope(), "cam_ang", 0.0));
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);*/

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



void HeliForceManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
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



void HeliForceManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	(void)pResource;
}

void HeliForceManager::RendererTextureLoadCallback(UiCure::UserRendererImageResource* pResource)
{
	mUiManager->GetRenderer()->TryAddGeometryTexture(mArrowBillboardId, pResource->GetData());
}



void HeliForceManager::DisableDepth()
{
	mUiManager->GetRenderer()->EnableAllLights(false);
	mUiManager->GetRenderer()->SetDepthWriteEnabled(false);
	mUiManager->GetRenderer()->SetDepthTestEnabled(false);
}

void HeliForceManager::EnableDepth()
{
	mUiManager->GetRenderer()->EnableAllLights(true);
	mUiManager->GetRenderer()->SetDepthWriteEnabled(true);
	mUiManager->GetRenderer()->SetDepthTestEnabled(true);
}



LOG_CLASS_DEFINE(GAME, HeliForceManager);



}
