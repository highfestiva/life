
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "FireManager.h"
#include <algorithm>
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/ContextPath.h"
#include "../Cure/Include/Driver.h"
#include "../Cure/Include/Elevator.h"
#include "../Cure/Include/Health.h"
#include "../Cure/Include/HiscoreAgent.h"
#include "../Cure/Include/FloatAttribute.h"
#include "../Cure/Include/IntAttribute.h"
#include "../Cure/Include/NetworkClient.h"
#include "../Cure/Include/Spawner.h"
#include "../Cure/Include/TimeManager.h"
#include "../Lepra/Include/Math.h"
#include "../Lepra/Include/Obfuxator.h"
#include "../Lepra/Include/Random.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Lepra/Include/Time.h"
#include "../Life/LifeClient/ExplodingMachine.h"
//#include "../Life/LifeClient/FastProjectile.h"
//#include "../Life/LifeClient/HomingProjectile.h"
#include "../Life/LifeClient/Level.h"
#include "../Life/LifeClient/MassObject.h"
#include "../Life/LifeClient/Menu.h"
#include "../Life/LifeClient/Mine.h"
#include "../Life/LifeClient/Projectile.h"
#include "../Life/LifeClient/UiConsole.h"
#include "../Life/Explosion.h"
#include "../Life/ProjectileUtil.h"
#include "../Life/Spawner.h"
#include "../Tbc/Include/PhysicsTrigger.h"
#include "../UiCure/Include/UiBurnEmitter.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiDebugRenderer.h"
#include "../UiCure/Include/UiExhaustEmitter.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiJetEngineEmitter.h"
#include "../UiCure/Include/UiGravelEmitter.h"
#include "../UiCure/Include/UiSoundReleaser.h"
#include "../UiLepra/Include/UiTouchDrag.h"
//#include "../UiLepra/Include/UiOpenGLExtensions.h"
#include "../UiTbc/Include/GUI/UiCheckButton.h"
#include "../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../UiTbc/Include/GUI/UiFixedLayouter.h"
#include "../UiTbc/Include/GUI/UiRadioButton.h"
#include "../UiTbc/Include/GUI/UiTextArea.h"
#include "../UiTbc/Include/GUI/UiTextField.h"
#include "../UiTbc/Include/UiBillboardGeometry.h"
#include "../UiTbc/Include/UiParticleRenderer.h"
#include "../UiTbc/Include/UiRenderer.h"
#include "../UiTbc/Include/UiTriangleBasedGeometry.h"
#include "AutoPathDriver.h"
#include "BaseMachine.h"
#include "Fire.h"
#include "FireConsoleManager.h"
#include "FireTicker.h"
#include "Level.h"
#include "RtVar.h"
#include "Eater.h"
#include "Sunlight.h"
#include "Version.h"



namespace Fire
{



#define BG_COLOR Color(40, 40, 40, 160)
const float hp = 768/1024.0f;
const int gLevels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
const float gRollOutTime = 1.5f;
const float gTargetInfoDelay = 0.1f;
struct VillainTypes
{
	str mName;
	int mCount;
};
#define PERSONS_INNOCENT_PART 0.40f
int gPersonalityCount = 0;
const VillainTypes gVillainTypes[] =
{
	{_O("JI9,,/0,5+*~29=:9,", "Terrorist leader"), 110},
	{_O("Jg9,,/-,5+*", "Terrorist"), 440},
	{_O("KX)<(9.,+5(9", "Subversive"), 110},
	{_O("X<=+;5F+*", "Fascist"), 30},
	{_O("P\\=$5", "Nazi"), 110},
	{_O("N>)*50f", "Putin"), 1},
	{_O("Qo),:9k,9,", "Murderer"), 50},
	{_O("G:65+*W29<2/'9,", "Whistleblower"), 5},
	{_O("[%6915 ;=2~]25", "Chemical Ali"), 1},
	{_O("=32q]+w+=:", "al-Assad"), 1},
	{_O("Xb)75*h5(9", "Fugitive"), 20},
	{_O("Jm,9+.}=++9,", "Trespasser"), 20},
	{_O("J]9,,/s,5+*~'=00=<9B", "Terrorist wannabe"), 30},
	{_O("]-GOR", "AWOL"), 10},
	{_O("[5/.%,z576*~(5/2=*/{,", "Copyright violator"), 1},
	{_O("]:00/%*507~7)%", "Annoying guy"), 1},
};



FireManager::FireManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mMenu(0),
	mLevel(0),
	mSteppedLevel(false),
	mSunlight(0),
	mCameraTransform(quat(), vec3()),
	mPauseButton(0),
	mBombButton(0),
	//mCheckIcon(0),
	mKills(0),
	mKillLimit(0),
	mLevelTotalKills(0)
{
	mFireDelayTimer.Start();

	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->PreLoadSound(_T("explosion"));

	SetConsoleManager(new FireConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	Tbc::GeometryBase::SetDefaultFlags(Tbc::GeometryBase::EXCLUDE_CULLING);	// Save some math during rendering, as most objects are on stage in this game.

	v_set(GetVariableScope(), RTVAR_GAME_EXPLOSIVESTRENGTH, 1.0);
	v_set(GetVariableScope(), RTVAR_GAME_FIRSTRUN, true);
	v_set(GetVariableScope(), RTVAR_GAME_FIREDELAY, 1.0);
	v_set(GetVariableScope(), RTVAR_GAME_STARTLEVEL, _T("lvl00"));
	v_set(GetVariableScope(), RTVAR_GAME_VEHICLEREMOVEDELAY, 25.0);
}

FireManager::~FireManager()
{
	Close();

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
}

void FireManager::Suspend()
{
	if (!mMenu->GetDialog())
	{
		mPauseButton->SetVisible(false);
		OnPauseButton(0);
	}
}

void FireManager::LoadSettings()
{
	v_set(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	v_set(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	v_set(GetVariableScope(), RTVAR_UI_3D_FOV, 38.8);

	Parent::LoadSettings();
	v_slowget(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);

	v_set(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);
	v_set(GetVariableScope(), RTVAR_CTRL_EMULATETOUCH, true);

	GetConsoleManager()->ExecuteCommand(_T("bind-key F2 prev-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F3 next-level"));
}

void FireManager::SaveSettings()
{
	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());
}

void FireManager::SetRenderArea(const PixelRect& pRenderArea)
{
	Parent::SetRenderArea(pRenderArea);
}

bool FireManager::Open()
{
	bool lOk = Parent::Open();
	if (lOk)
	{
		mUiManager->GetDisplayManager()->SetCaption(_T("NSAgent"));
	}
	if (lOk)
	{
		mPauseButton = ICONBTNA("btn_pause.png", "");
		int x = 12;
		int y = 12;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(FireManager, OnPauseButton);
	}
	if (lOk)
	{
		mBombButton = ICONBTNA("btn_bomb.png", "");
		int x = 12;
		int y = 12*2+64;
		mUiManager->GetDesktopWindow()->AddChild(mBombButton, x, y);
		mBombButton->SetVisible(false);
		mBombButton->SetOnClick(FireManager, OnBombButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
		mMenu->SetButtonTapSound(_T("tap.wav"), 1, 0.3f);
	}
	return lOk;
}

void FireManager::Close()
{
	ScopeLock lLock(GetTickLock());
	delete mBombButton;
	mBombButton = 0;
	delete mPauseButton;
	mPauseButton = 0;
	delete mMenu;
	mMenu = 0;
	delete mSunlight;
	mSunlight = 0;

	Parent::Close();
}

void FireManager::SetIsQuitting()
{
	((FireConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void FireManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



PixelRect FireManager::GetRenderableArea() const
{
	PixelRect lRenderArea;
	const int w = (int)(mRenderArea.GetHeight()/hp);
	lRenderArea.Set(mRenderArea.GetCenterX()-w/2, mRenderArea.mTop, mRenderArea.GetCenterX()+w/2, mRenderArea.mBottom);
	lRenderArea.mLeft = std::max(lRenderArea.mLeft, mRenderArea.mLeft);
	lRenderArea.mRight = std::min(lRenderArea.mRight, mRenderArea.mRight);
	return lRenderArea;
}

bool FireManager::Render()
{
	PixelRect lRenderArea = GetRenderableArea();
	// If we're 1024x768 (iPad), we don't need to clear.
	const bool lNeedSizeClear = (lRenderArea.GetWidth() <= mRenderArea.GetWidth()-1);
	const bool lNeedLevelClear = (!mLevel || !mLevel->IsLoaded());
	v_set(GetVariableScope(), RTVAR_UI_3D_ENABLECLEAR, (lNeedSizeClear||lNeedLevelClear));

	const PixelRect lFullRenderArea = mRenderArea;
	mRenderArea = lRenderArea;
	bool lOk = Parent::Render();
	mRenderArea = lFullRenderArea;

	return lOk;
}

static bool SortVillainsPredicate(const FireManager::VillainPair& a, const FireManager::VillainPair& b)
{
	return a.second.mScale < b.second.mScale;
}

bool FireManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}

	if (mMenu->GetDialog() || !mLevel || !mLevel->IsLoaded())
	{
		return true;	// Don't draw our terrorist indicators any more when the user is looking at a dialog.
	}

	UiTbc::Painter* lPainter = mUiManager->GetPainter();
	str lScore = strutil::Format(_T("Score: %i/%i"), mKills, mKillLimit);
	lPainter->SetColor(WHITE);
	lPainter->PrintText(lScore, 100, 21);

	const int lUnit = std::max(8, mRenderArea.GetHeight()/50);
	mUiManager->SetScaleFont(-lUnit*1.4f);
	const int r = 5;
	VillainArray lSortedVillains;
	lSortedVillains.assign(mVillainMap.begin(), mVillainMap.end());
	std::sort(lSortedVillains.begin(), lSortedVillains.end(), SortVillainsPredicate);
	VillainArray::iterator x = lSortedVillains.begin();
	for (; x != lSortedVillains.end(); ++x)
	{
		if (x->second.mTime < gTargetInfoDelay)
		{
			continue;	// If we've unrolled the indicator completely, paint no more.
		}
		const PixelCoord xy = x->second.xy;
		const float lTimePart = (x->second.mTime-gTargetInfoDelay)/gRollOutTime;
		const float lArcEndPart = 0.3f;
		const float lDiagonalEndPart = 0.48f;
		const int lArcEnd = (int)std::min(360.0f, lTimePart/lArcEndPart*360.0f);
		Color lColor;
		if (x->second.mDangerousness > 0.5f)
		{
			lColor = Color(RED, DARK_RED, (x->second.mDangerousness-0.5f)*2);
		}
		else
		{
			lColor = Color(YELLOW, RED, x->second.mDangerousness/0.5f);
		}
		const int lInfoBubbleRadius = 5;
		if (lTimePart > lArcEndPart)
		{
			const float lLinePart = std::min(1.0f, (lTimePart-lArcEndPart)/(lDiagonalEndPart-lArcEndPart));
			const int s1 = (int)(r/1.41421356f);	// Diagonal radius and sqrt(2).
			const int lAxisLength = (int)((lUnit*2-s1-lInfoBubbleRadius)*lLinePart);
			lPainter->SetColor(Color(30, 30, 30, 110));
			lPainter->SetAlphaValue(110);
			lPainter->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
			lPainter->DrawLine(xy.x+s1, xy.y-s1+1, xy.x+s1+lAxisLength, xy.y-s1-1-lAxisLength);
			lPainter->SetColor(lColor);
			lPainter->SetRenderMode(UiTbc::Painter::RM_NORMAL);
			lPainter->DrawLine(xy.x+s1, xy.y-s1+1, xy.x+s1+lAxisLength, xy.y-s1-1-lAxisLength);
		}
		lPainter->SetColor(Color(30, 30, 30, 150));
		lPainter->SetAlphaValue(150);
		lPainter->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
		lPainter->DrawArc(xy.x-r+1, xy.y-r+1, r*2, r*2, 45, 45-lArcEnd, true);
		lPainter->SetColor(lColor);
		lPainter->SetRenderMode(UiTbc::Painter::RM_NORMAL);
		lPainter->DrawArc(xy.x-r, xy.y-r, r*2, r*2, 45, 45-lArcEnd, true);
		if (lTimePart > lDiagonalEndPart)
		{
			// Cut using rect, so text will appear smoothly.
			const int r  = lInfoBubbleRadius;
			const int xl = xy.x+lUnit*2-r;
			const int yl = xy.y-lUnit*2+r;
			const int yt = yl-lPainter->GetFontHeight()-2*r;
			const int wl = lPainter->GetFontManager()->GetStringWidth(x->second.mVillain)+2+2*r;
			if (lTimePart < 1)
			{
				const float lTextPart = std::min(1.0f, (lTimePart-lDiagonalEndPart)/(1.0f-lDiagonalEndPart));
				PixelRect lRect(xl-1, yt-1, xl+1+(int)(wl*lTextPart), yl+2);
				lPainter->SetClippingRect(lRect);
			}
			lPainter->SetAlphaValue(150);
			lPainter->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
			lPainter->DrawRoundedRect(PixelRect(xl, yt, xl+wl, yl), r, 0x7, true);
			lPainter->SetColor(Color(30, 30, 30, 150));
			lPainter->SetAlphaValue(150);
			lPainter->PrintText(x->second.mVillain, xl+r+2, yt+r+1);
			lPainter->SetRenderMode(UiTbc::Painter::RM_NORMAL);
			lPainter->SetColor(WHITE);
			lPainter->PrintText(x->second.mVillain, xl+r+1, yt+r);
			if (lTimePart < 1)
			{
				lPainter->SetClippingRect(mRenderArea);	// Restore for next loop.
			}
		}
	}

	mUiManager->SetMasterFont();
	return true;
}

void FireManager::DrawSyncDebugInfo()
{
	PixelRect lRenderArea;
	const int w = (int)(mRenderArea.GetHeight()/hp);
	lRenderArea.Set(mRenderArea.GetCenterX()-w/2, mRenderArea.mTop, mRenderArea.GetCenterX()+w/2, mRenderArea.mBottom);
	lRenderArea.mLeft = std::max(lRenderArea.mLeft, mRenderArea.mLeft);
	lRenderArea.mRight = std::min(lRenderArea.mRight, mRenderArea.mRight);
	const PixelRect lFullRenderArea = mRenderArea;
	mRenderArea = lRenderArea;

	Parent::DrawSyncDebugInfo();

	ScopeLock lLock(GetTickLock());
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

	mRenderArea = lFullRenderArea;
}



bool FireManager::IsObjectRelevant(const vec3& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraTransform.GetPosition()) <= pDistance*pDistance);
}



void FireManager::Shoot(Cure::ContextObject* pAvatar, int pWeapon)
{
	(void)pAvatar;
	(void)pWeapon;

	double lFireDelay;
	v_get(lFireDelay, =, GetVariableScope(), RTVAR_GAME_FIREDELAY, 1.5);
	if (!mLevel->IsLoaded() || mFireDelayTimer.QueryTimeDiff() < lFireDelay)
	{
		return;
	}
	mFireDelayTimer.Start();
	vec3 lTargetPosition;
	if (!GetPhysicsManager()->QueryRayCollisionAgainst(mCameraTransform.GetPosition(), mShootDirection, 1000.0f, mLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), &lTargetPosition, 1) == 1)
	{
		// User aiming above ground. Find vehicle closest to that position, and adjust target range thereafter.
		float lDistance = 350.0;
		float lClosestRayDistance2 = 150.0f * 150.0f;
		Cure::ContextManager::ContextObjectTable lObjectTable = GetContext()->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			Cure::ContextObject* lObject = x->second;
			Tbc::ChunkyPhysics* lPhysics = lObject->ContextObject::GetPhysics();
			if (!lObject->IsLoaded() || !lPhysics)
			{
				continue;
			}
			const vec3 lVehiclePosition = lObject->GetPosition();
			if (lVehiclePosition.y < 30.0f || Cure::Health::Get(lObject) <= 0)
			{
				continue;
			}
			const vec3 lRayRelativePosition = lVehiclePosition.ProjectOntoPlane(mShootDirection);
			const float lDistance2 = lRayRelativePosition.GetLengthSquared();
			if (lDistance2 < lClosestRayDistance2)
			{
				lClosestRayDistance2 = lDistance2;
				lDistance = lVehiclePosition * mShootDirection;
			}
		}
		lTargetPosition = mShootDirection * lDistance;
	}
	else
	{
		Cure::ContextObject* lObject = Parent::CreateContextObject(_T("indicator"), Cure::NETWORK_OBJECT_LOCAL_ONLY);
		lObject->SetInitialTransform(xform(gIdentityQuaternionF, lTargetPosition));
		lObject->StartLoading();
		GetContext()->DelayKillObject(lObject, 1.5f);
	}

	Life::Projectile* lProjectile = new Life::Projectile(GetResourceManager(), _T("rocket"), mUiManager, this);
	lProjectile->EnableRootShadow(true);
	AddContextObject(lProjectile, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	lProjectile->SetJetEngineEmitter(new UiCure::JetEngineEmitter(GetResourceManager(), mUiManager));
	lProjectile->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
	xform t(mCameraTransform);
	t.GetPosition().x += 0.7f;
	t.GetPosition().y += 1.0f;
	t.GetPosition().z += 0.1f;
	t.GetOrientation().RotateAroundWorldX(-PIF/2);	// Tilt rocket.
	float lAcceleration = 150;
	float lTerminalVelocity = 300;
	float lGravityEffect = 1.15f;
	float lAimAbove = 1.5f;
	float lSomeRocketLength = 9.0;
	float lUpDownEffect = -0.1f;
	/*v_tryget(lAcceleration, =(float), GetVariableScope(), "shot.acceleration", 150.0);
	v_tryget(lTerminalVelocity, =(float), GetVariableScope(), "shot.terminalvelocity", 300.0);
	v_tryget(lGravityEffect, =(float), GetVariableScope(), "shot.gravityeffect", 1.15);
	v_tryget(lAimAbove, =(float), GetVariableScope(), "shot.aimabove", 1.5);
	v_tryget(lSomeRocketLength, =(float), GetVariableScope(), "shot.rocketlength", 9.0);
	v_tryget(lUpDownEffect, =(float), GetVariableScope(), "shot.updowneffect", -0.1);*/
	vec3 lDistance = lTargetPosition - t.GetPosition();
	lAimAbove = Math::Lerp(0.0f, lAimAbove, std::min(100.0f, lDistance.GetLength())/100.0f);
	lDistance.z += lAimAbove;
	const vec3 lShootDirectionEulerAngles = Life::ProjectileUtil::CalculateInitialProjectileDirection(lDistance, lAcceleration, lTerminalVelocity, GetPhysicsManager()->GetGravity()*lGravityEffect, lUpDownEffect);
	t.GetOrientation().RotateAroundWorldX(lShootDirectionEulerAngles.y);
	t.GetOrientation().RotateAroundWorldZ(lShootDirectionEulerAngles.x);
	t.mPosition.x -= lSomeRocketLength*sin(lShootDirectionEulerAngles.x);
	t.mPosition.z += lSomeRocketLength*sin(lShootDirectionEulerAngles.y);
	lProjectile->SetInitialTransform(t);
	lProjectile->StartLoading();
}

void FireManager::Detonate(Cure::ContextObject* pExplosive, const Tbc::ChunkyBoneGeometry* pExplosiveGeometry, const vec3& pPosition, const vec3& pVelocity, const vec3& pNormal, float pStrength)
{
	float lVolumeFactor = 1;
	const bool lIsRocket = (pExplosive->GetClassId() == _T("rocket"));
	if (lIsRocket)
	{
		float lExplosiveStrength;
		v_get(lExplosiveStrength, =(float), GetVariableScope(), RTVAR_GAME_EXPLOSIVESTRENGTH, 1.0);
		pStrength *= lExplosiveStrength;
		v_set(GetVariableScope(), RTVAR_GAME_EXPLOSIVESTRENGTH, 1.0);	// Reset to normal strength.
		lVolumeFactor *= (lExplosiveStrength>1)? 4 : 1;
	}
	const float lCubicStrength = 4*(::pow(pStrength+1, 1/3.0f) - 1);	// Reduce by 3D volume. Explosion spreads in all directions.
	if (!lIsRocket && !mMenu->GetDialog())
	{
		BaseMachine* lMachine = dynamic_cast<BaseMachine*>(pExplosive);
		lMachine->DeleteEngineSounds();	// Stop makin em.
		mKills += lMachine->mVillain.empty()? -1 : +1;
		++mLevelTotalKills;
		// Logic for showing super bomb icon.
		const int lShowBombLimit = 10 + GetCurrentLevelNumber();
		if (mLevelTotalKills%lShowBombLimit == 0 && mKills < mKillLimit)
		{
			if (!mBombButton->IsVisible())
			{
				mBombButton->SetVisible(true);
				UiCure::UserSound2dResource* lSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
				new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("great.wav"), lSound, 1, 1);
			}
		}
	}

	mCollisionSoundManager->OnCollision(pStrength*lVolumeFactor, pPosition, pExplosiveGeometry, _T("explosion"));

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lKeepOnGoingFactor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	vec3 u = pVelocity.ProjectOntoPlane(pNormal) * (1+lKeepOnGoingFactor);
	u -= pVelocity;	// Mirror and inverse.
	u.Normalize();
	const int lParticles = Math::Lerp(6, 10, lCubicStrength * 0.3f);
	vec3 lStartFireColor(1.0f, 1.0f, 0.6f);
	vec3 lFireColor(0.6f, 0.4f, 0.2f);
	vec3 lStartSmokeColor(0.4f, 0.4f, 0.4f);
	vec3 lSmokeColor(0.2f, 0.2f, 0.2f);
	vec3 lShrapnelColor(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	vec3 lSpritesPosition(pPosition*0.98f-pPosition.GetNormalized(2.0f));	// We just move it closer to make it less likely to be cut off by ground.
	lParticleRenderer->CreateExplosion(lSpritesPosition, lCubicStrength, u, 1, 1.5f, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles, lParticles, lParticles/2, lParticles/2);

	// Slowmo check.
	bool lNormalDeath = true;
	if (!lIsRocket && Cure::Health::Get(pExplosive) < -5500)
	{
		if (Random::Uniform(0.0f, 1.0f) > 0.7f)
		{
			lNormalDeath = false;
			mSlowmoTimer.TryStart();
		}
	}

	// Shove!
	ScopeLock lLock(GetTickLock());
	Tbc::PhysicsManager* lPhysicsManager = GetPhysicsManager();
	Cure::ContextManager::ContextObjectTable lObjectTable = GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (!lObject->IsLoaded())
		{
			continue;
		}
		float lForce = Life::Explosion::CalculateForce(lPhysicsManager, lObject, pPosition, pStrength);
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			Cure::FloatAttribute* lHealth = Cure::Health::GetAttribute(lObject);
			if (lHealth)
			{
				const float lValue = lHealth->GetValue() - lForce*Random::Normal(1.01f, 0.1f, 0.7f, 1.0f);
				lHealth->SetValue(lValue);
			}
			x->second->ForceSend();
			Life::Explosion::PushObject(lPhysicsManager, lObject, pPosition, pStrength, GetTimeManager()->GetNormalFrameTime());
		}
		BaseMachine* lMachine = dynamic_cast<BaseMachine*>(lObject);
		if (lMachine && lMachine->GetPosition().GetDistanceSquared(pPosition) < 150*150)
		{
			lMachine->AddPanic((lForce > 0.1f)? 1.0f : 0.45f);
		}
	}
}

void FireManager::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	(void)pBullet;
	(void)pHitObject;
}

void FireManager::OnLetThroughTerrorist(BaseMachine* pTerrorist)
{
	(void)pTerrorist;
	if (mMenu->GetDialog())
	{
		return;
	}

	--mKills;
	UiCure::UserSound2dResource* lSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
	new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("bad.wav"), lSound, 1, 1);
}



bool FireManager::DidFinishLevel()
{
	mLog.Headlinef(_T("Level %s done!"), mLevel->GetClassId().c_str());
	return true;
}

str FireManager::StepLevel(int pCount)
{
	mKills = 0;
	mLevelTotalKills = 0;
	int lLevelNumber = GetCurrentLevelNumber();
	lLevelNumber += pCount;
	str lNewLevelName = StoreLevelIndex(lLevelNumber);
	GetContext()->PostKillObject(mLevel->GetInstanceId());
	{
		ScopeLock lLock(GetTickLock());
		mKillLimit = 4+4*lLevelNumber;
		mLevel = (Level*)Parent::CreateContextObject(lNewLevelName, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		mLevel->StartLoading();
	}
	mSteppedLevel = true;
	return lNewLevelName;
}

str FireManager::StoreLevelIndex(int pLevelNumber)
{
	const int lLevelCount = LEPRA_ARRAY_COUNT(gLevels);
	if (pLevelNumber < 0)
	{
		pLevelNumber = lLevelCount-1;
	}
	else if (pLevelNumber >= lLevelCount)
	{
		pLevelNumber = 0;
	}
	str lNewLevelName = strutil::Format(_T("lvl%2.2i"), pLevelNumber);
	v_set(GetVariableScope(), RTVAR_GAME_STARTLEVEL, lNewLevelName);
	return lNewLevelName;
}


Level* FireManager::GetLevel() const
{
	if (mLevel && mLevel->IsLoaded())
	{
		return mLevel;
	}
	return 0;
}

int FireManager::GetCurrentLevelNumber() const
{
	int lLevelNumber = 0;
	strutil::StringToInt(mLevel->GetClassId().substr(3), lLevelNumber);
	return lLevelNumber;
}



Cure::RuntimeVariableScope* FireManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool FireManager::InitializeUniverse()
{
	mUiManager->GetRenderer()->SetLineWidth(1);
	mUiManager->UpdateSettings();

	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const vec3 v;
	lParticleRenderer->CreateExplosion(vec3(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	str lStartLevel;
	v_get(lStartLevel, =, GetVariableScope(), RTVAR_GAME_STARTLEVEL, _T("lvl00"));
	{
		ScopeLock lLock(GetTickLock());
		int lLevelIndex = 0;
		strutil::StringToInt(lStartLevel.substr(3), lLevelIndex);
		mKillLimit = 4+4*lLevelIndex;
		mLevel = (Level*)Parent::CreateContextObject(lStartLevel, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		mLevel->StartLoading();
	}
	mSunlight = new Sunlight(mUiManager);
	return true;
}

void FireManager::ScriptPhysicsTick()
{
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		MoveCamera();
		UpdateCameraPosition(false);
		HandleShooting();
		HandleTargets(lPhysicsTime);
	}

	if (mKills >= mKillLimit)
	{
		CreateNextLevelDialog();
	}

	if (mSteppedLevel && !GetResourceManager()->IsLoading())
	{
		mSteppedLevel = false;
		mAllLoadedTimer.TryStart();
	}
	if (mAllLoadedTimer.QuerySplitTime() > 10.0)
	{
		mAllLoadedTimer.Stop();
		mAllLoadedTimer.ClearTimeDiff();
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

	if (mSlowmoTimer.IsStarted())
	{
		if (mSlowmoTimer.QueryTimeDiff() < 3.5f)
		{
			v_set(GetVariableScope(), RTVAR_PHYSICS_RTR, 0.3);
		}
		else
		{
			v_set(GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0);
			mSlowmoTimer.Stop();
		}
	}

	Parent::ScriptPhysicsTick();
}

void FireManager::HandleWorldBoundaries()
{
	std::vector<Cure::GameObjectId> lLostObjectArray;
	typedef Cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& lObjectTable = GetContext()->GetObjectTable();
	ContextTable::const_iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->IsLoaded() && lObject->GetPhysics())
		{
			const vec3 lPosition = lObject->GetPosition();
			if (!Math::IsInRange(lPosition.x, -250.0f, +250.0f) ||
				!Math::IsInRange(lPosition.y, -100.0f, +550.0f) ||
				!Math::IsInRange(lPosition.z, -250.0f, +250.0f))
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

void FireManager::MoveCamera()
{
}

void FireManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, vec3());
	}
}

void FireManager::HandleShooting()
{
	if (mUiManager->CanRender())
	{
		float lFOV;
		v_get(lFOV, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 38.8);
		UpdateFrustum(lFOV);
	}

	typedef UiLepra::Touch::DragManager::DragList DragList;
	DragList& lDragList = mUiManager->GetDragManager()->GetDragList();
	for (DragList::iterator x = lDragList.begin(); x != lDragList.end(); ++x)
	{
		if (x->mIsPress && (x->mFlags&1) == 0 && !mMenu->GetDialog())
		{
			x->mFlags |= 1;
			mShootDirection = mUiManager->GetRenderer()->ScreenCoordToVector(x->mLast);
			Shoot(0, 0);
		}
	}
}

void FireManager::HandleTargets(float pTime)
{
	VillainMap::iterator y = mVillainMap.begin();
	for (; y != mVillainMap.end(); ++y)
	{
		y->second.mIsActive = false;
	}

	PixelRect lRenderArea = GetRenderableArea();
	lRenderArea.mLeft -= 30;	// Don't drop target info indicator just because the car wobbles slightly off left side of screen.
	Cure::ContextManager::ContextObjectTable lObjectTable = GetContext()->GetObjectTable();
	Cure::ContextManager::ContextObjectTable::iterator x = lObjectTable.begin();
	for (; x != lObjectTable.end(); ++x)
	{
		Cure::ContextObject* lObject = x->second;
		if (lObject->GetClassId().find(_T("lvl")) == 0 || lObject->GetClassId() == _T("indicator"))
		{
			continue;
		}
		BaseMachine* lMachine = dynamic_cast<BaseMachine*>(lObject);
		if (!lMachine || lMachine->mVillain.empty())
		{
			continue;
		}
		const vec3 lPosition = lMachine->GetPosition();
		if (lPosition.y < 35 || lPosition.y > 350.0f)
		{
			continue;
		}
		const vec2 lCoord = mUiManager->GetRenderer()->PositionToScreenCoord(lPosition, 0);
		const PixelCoord xy((int)lCoord.x, (int)lCoord.y);
		if (!lRenderArea.IsInside(xy.x, xy.y))
		{
			continue;
		}
		const float lScale = std::min(1.0f, 100.0f/lPosition.y);
		TargetInfo lTargetInfo(lMachine->mVillain, xy, lMachine->mDangerousness, lScale);
		VillainMap::iterator y = mVillainMap.find(lMachine);
		if (y == mVillainMap.end())
		{
			mVillainMap.insert(VillainMap::value_type(lMachine, lTargetInfo));
		}
		else
		{
			y->second.xy = xy;
			y->second.mIsActive = true;
			y->second.mScale = lScale;
			if (Cure::Health::Get(lMachine) > 0)
			{
				y->second.mTime += pTime;
			}
			else
			{
				if (y->second.mTime > gTargetInfoDelay+gRollOutTime)
				{
					y->second.mTime = gTargetInfoDelay+gRollOutTime;
				}
				y->second.mTime -= pTime;
			}
		}
	}
	y = mVillainMap.begin();
	while (y != mVillainMap.end())
	{
		if (!y->second.mIsActive)
		{
			y = mVillainMap.erase(y);
		}
		else
		{
			++y;
		}
	}

}



void FireManager::TickInput()
{
	OnLevelLoadCompleted();
	TickNetworkInput();
	TickUiInput();
}

void FireManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);

	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPhysicsStepCount > 0 && mAllowMovementInput)
	{
	}
}

void FireManager::TickUiUpdate()
{
	((FireConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	mCollisionSoundManager->Tick(mCameraTransform.GetPosition());
}

void FireManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



Cure::ContextObject* FireManager::CreateContextObject(const str& pClassId) const
{
	Cure::CppContextObject* lObject;
	if (strutil::StartsWith(pClassId, _T("lvl")))
	{
		UiCure::GravelEmitter* lGravelParticleEmitter = new UiCure::GravelEmitter(GetResourceManager(), mUiManager, 0.5f, 1, 10, 2);
		Level* lLevel = new Level(GetResourceManager(), pClassId, mUiManager, lGravelParticleEmitter);
		lLevel->EnableRootShadow(false);
		lObject = lLevel;
	}
	else if (strutil::StartsWith(pClassId, _T("indicator")))
	{
		lObject = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
	}
	else
	{
		BaseMachine* lMachine = new BaseMachine(GetResourceManager(), pClassId, mUiManager, (FireManager*)this);
		lMachine->mLevelSpeed = mLevel->GetLevelSpeed();
		//lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lMachine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), mUiManager));
		//lMachine->GetBurnEmitter()->SetFreeFlow();
		lMachine->SetExplosiveStrength(0.6f);
		const int c = LEPRA_ARRAY_COUNT(gVillainTypes);
		if (gPersonalityCount <= 0)
		{
			gPersonalityCount = 0;
			for (int x = 0; x < c; ++x)
			{
				gPersonalityCount += gVillainTypes[x].mCount;
			}
			gPersonalityCount = (int)(gPersonalityCount/(1-PERSONS_INNOCENT_PART));
		}
		int r = (int)Random::Uniform(0.0f, (float)gPersonalityCount+1);
		for (int x = 0; x < c; ++x)
		{
			r -= gVillainTypes[x].mCount;
			if (r <= 0)
			{
				lMachine->mVillain = gVillainTypes[x].mName;
				lMachine->mDangerousness = 1-(float)x/(c-1);
				break;
			}
		}
		lObject = lMachine;
	}
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

Cure::ContextObject* FireManager::CreateLogicHandler(const str& pType)
{
	if (pType == _T("spawner") || pType == _T("spawner_init"))
	{
		return new Life::Spawner(GetContext());
	}
	else if (pType == _T("eater"))
	{
		return new Eater(GetContext());
	}
	else if (pType == _T("context_path"))
	{
		return mLevel->QueryPath();
	}
	return (0);
}

void FireManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		if (pObject == mLevel)
		{
			OnLevelLoadCompleted();
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("rocket")))
		{
			pObject->SetEnginePower(0, 1.0f);
			//pObject->SetEnginePower(2, 1.0f);
		}
		else if (strutil::StartsWith(pObject->GetClassId(), _T("indicator")))
		{
		}
		else
		{
			new Cure::FloatAttribute(pObject, _T("float_childishness"), 1);
			new AutoPathDriver(this, pObject->GetInstanceId(), _T("input"));
			vec3 lColor = RNDPOSVEC();
			Life::ExplodingMachine* lMachine = (Life::ExplodingMachine*)pObject;
			lMachine->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = lColor;
		}
		log_volatile(mLog.Tracef(_T("Loaded object %s."), pObject->GetClassId().c_str()));
		pObject->GetPhysics()->UpdateBonesObjectTransformation(0, gIdentityTransformationF);
		((UiCure::CppContextObject*)pObject)->UiMove();
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void FireManager::OnLevelLoadCompleted()
{
	if (!mLevel || !mLevel->IsLoaded())
	{
		return;
	}
	// Update texture UV coordinates according to FoV.
	UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
	float lFOV;
	v_get(lFOV, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 38.8);
	static float lFormerFoV = 0;
	static unsigned lFormerLevelId = 0;
	if (lFormerFoV == lFOV && lFormerLevelId == mLevel->GetInstanceId())
	{
		return;
	}
	mLog.Headlinef(_T("Level %s loaded."), mLevel->GetClassId().c_str());
	bool lFirstRun;
	v_get(lFirstRun, =, GetVariableScope(), RTVAR_GAME_FIRSTRUN, false);
	if (lFirstRun)
	{
		CreateNextLevelDialog();
	}
	lRenderer->ResetClippingRect();
	lFormerFoV = lFOV;
	lFormerLevelId = mLevel->GetInstanceId();
	const float wf = +1.0f / mUiManager->GetDisplayManager()->GetWidth();
	const float hf = hp / mUiManager->GetDisplayManager()->GetHeight();
	const size_t lMeshCount = ((UiTbc::ChunkyClass*)mLevel->GetClass())->GetMeshCount();
	for (size_t x = 0; x < lMeshCount; ++x)
	{
		Tbc::GeometryReference* lMesh = (Tbc::GeometryReference*)mLevel->GetMesh(x);
		xform lTransform = lMesh->GetTransformation();
		if (lMesh->GetUVSetCount())
		{
			UiTbc::TriangleBasedGeometry* lParent = (UiTbc::TriangleBasedGeometry*)lMesh->GetParentGeometry();
			deb_assert(lParent->GetUVCountPerVertex() == 4);
			const float* xyz = lMesh->GetVertexData();
			const unsigned lVertexCount = lMesh->GetVertexCount();
			float* uvst = new float[lVertexCount*4];
			for (unsigned z = 0; z < lVertexCount; ++z)
			{
				vec3 lVector(&xyz[z*3]);
				lVector = lTransform.Transform(lVector);
				vec2 c = lRenderer->PositionToScreenCoord(lVector, 1/hp);
				const float x = c.x * wf;
				const float y = c.y * hf;
				uvst[z*4+0] = x*lVector.y;
				uvst[z*4+1] = y*lVector.y;
				uvst[z*4+2] = 0;
				uvst[z*4+3] = lVector.y;
			}
			lParent->PopUVSet();
			lParent->AddUVSet(uvst);
			lParent->SetUVDataChanged(true);
			lMesh->SetUVDataChanged(true);

			lMesh->SetPreRenderCallback(Tbc::GeometryBase::PreRenderCallback(this, &FireManager::DisableAmbient));
			lMesh->SetPostRenderCallback(Tbc::GeometryBase::PostRenderCallback(this, &FireManager::EnableAmbient));

			mUiManager->GetRenderer()->UpdateGeometry(((UiCure::GeometryReferenceResource*)mLevel->GetMeshResource(x)->GetConstResource())->GetParent()->GetData(), false);

			delete[] uvst;
		}
	}
}

void FireManager::OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);

	BaseMachine* lMachine1 = dynamic_cast<BaseMachine*>(pObject1);
	BaseMachine* lMachine2 = dynamic_cast<BaseMachine*>(pObject2);
	if (lMachine1 && lMachine2)
	{
		const vec3 v = (lMachine2->GetPosition() - lMachine1->GetPosition());
		if (lMachine1->GetForwardDirection()*v > 0.1f)
		{
			lMachine2->AddPanic(1.5f);	// 1 drives into 2.
		}
		if (lMachine2->GetForwardDirection()*(-v) > 0.1f)
		{
			lMachine1->AddPanic(1.5f);	// 2 drives into 1.
		}
	}
}



void FireManager::OnBombButton(UiTbc::Button* pButton)
{
	pButton->SetVisible(false);
	v_set(GetVariableScope(), RTVAR_GAME_EXPLOSIVESTRENGTH, 10.0);
}

void FireManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
		pButton->SetVisible(false);
	}

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &FireManager::OnMenuAlternative), 0.5f, 0.5f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);

	UiTbc::FixedLayouter lLayouter(d);
	lLayouter.SetContentYMargin(d->GetPreferredHeight()/10);

	UiTbc::Button* lRestartButton = new UiTbc::Button(Color(90, 10, 10), _T("Reset game"));
	lLayouter.AddButton(lRestartButton, -8, 0, 2, 0, 1, 1, true);

	UiTbc::Button* lRestartLevelButton = new UiTbc::Button(Color(10, 90, 10), _T("Restart level"));
	lLayouter.AddButton(lRestartLevelButton, -4, 1, 2, 0, 1, 1, true);

	UiTbc::Button* lCloseButton = new UiTbc::Button(Color(180, 60, 50), _T("X"));
	lLayouter.AddCornerButton(lCloseButton, -9);

	v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void FireManager::CreateNextLevelDialog()
{
	if (mMenu->GetDialog())
	{
		return;
	}
	mPauseButton->SetVisible(false);

	const int lFinishedLevel = GetCurrentLevelNumber();
	StoreLevelIndex(lFinishedLevel+1);

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &FireManager::OnMenuAlternative), 0.8f, 0.4f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);

	UiTbc::FixedLayouter lLayouter(d);
	lLayouter.SetContentWidthPart(0.85f);

	static const tchar* lCongratulations[] =
	{
		_T("Well done, agent!\n\nPrepare to protect other people in other parts of the world."),
		_T("Great Scott; you are good at this!\n\nRemember to relax between wet jobs."),
		_T("We sure are lucky to have you on our side.\n\nField work sure beats the office, huh?"),
		_T("On behalf of all the people in the world:\n\nTHANK YOU!!!"),
		_T("There might be a bug in our terrorist ID software.\nPlease don't worry about it while we remedy.\n\nYou should go on a mission now."),
		_T("They software guys say the bug might be fixed.\n\nGo kill!"),
		_T("Everybody dies, but it's nice to see the bad ones go first.\n\nHead out agent!"),
		_T("Rooting out vermin is your cup of tea.\n\nI'm glad that you are not in working in pesticides."),
		_T("You are the pride and joy of our agency.\n\nOh and btw: the President sends his gratitude!"),
		_T("Your persistency must be admired.\n\nThe last guy quit after just a week!"),
		_T("Your next assignment is... Haha! Just kiddin'!\nWho cares where you go when there are big guns at your disposal\nand a lot of bad people at the other end of the barrel?"),
		_T("An awful lot of bad guys out there.\n\nGood work, agent!"),
		_T("A little collateral is not a problem.\nI mean, it's like fishing: to exterminate the big\ncatch you've gotta kill innocent fish babies."),
		_T("There are almost no terrorists left in the world!\n\nHumanity is relying on you."),
		_T("You've done it, the world is cleansed!\n\nHowever, disturbing reports on the outskirts of\nyour home town tells me you should go there again."),
	};
	deb_assert(LEPRA_ARRAY_COUNT(lCongratulations) == LEPRA_ARRAY_COUNT(gLevels));
	deb_assert(lFinishedLevel < LEPRA_ARRAY_COUNT(lCongratulations));
	str lCongrats = lCongratulations[lFinishedLevel];
	bool lFirstRun;
	v_get(lFirstRun, =, GetVariableScope(), RTVAR_GAME_FIRSTRUN, false);
	if (lFirstRun)
	{
		v_set(GetVariableScope(), RTVAR_GAME_FIRSTRUN, false);
		lCongrats = _T("Our patented EnemyVisionGoggles(R) indicates villains.\nAvoid collateral damage, when possible.\n\nGood luck agent!");
	}
	else
	{
		UiCure::UserSound2dResource* lSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("level_done.wav"), lSound, 1, 1);
	}
	UiTbc::Label* lLabel = new UiTbc::Label(LIGHT_GRAY, lCongrats);
	lLabel->SetFontId(mUiManager->SetScaleFont(std::min(-14.0f, d->GetPreferredHeight()/-14.0f)));
	mUiManager->SetMasterFont();
	//lLabel->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::TextComponent::ICON_CENTER);
	lLabel->SetAdaptive(false);
	lLayouter.AddComponent(lLabel, 0, 2, 0, 1, 1);

	if (lFirstRun)
	{
		UiTbc::Button* lOkButton = new UiTbc::Button(Color(10, 90, 10), _T("OK"));
		lOkButton->SetFontId(lLabel->GetFontId());
		lLayouter.AddButton(lOkButton, -5, 3, 4, 4, 3, 7, true);
	}
	else
	{
		UiTbc::Button* lNextLevelButton = new UiTbc::Button(Color(10, 90, 10), _T("Next level"));
		lNextLevelButton->SetFontId(lLabel->GetFontId());
		lLayouter.AddButton(lNextLevelButton, -7, 3, 4, 4, 3, 7, true);

		UiTbc::Button* lRestartButton = new UiTbc::Button(Color(90, 10, 10), _T("Restart from level 1"));
		lRestartButton->SetFontId(lLabel->GetFontId());
		lLayouter.AddButton(lRestartButton, -8, 3, 4, 0, 3, 7, true);
	}
}

void FireManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == -5)
	{
		mPauseButton->SetVisible(true);
	}
	else if (pButton->GetTag() == -6)
	{
		double lBedsideVolume = (pButton->GetState() == UiTbc::Button::PRESSED)? 0.02 : 1.0;
		v_set(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, lBedsideVolume);
		mUiManager->GetSoundManager()->SetMasterVolume((float)lBedsideVolume);	// Set right away for button volume.
	}
	else if (pButton->GetTag() == -4)
	{
		mPauseButton->SetVisible(true);
		GetConsoleManager()->PushYieldCommand(strutil::Format(_T("set-level-index %i"), GetCurrentLevelNumber()));
		mMenu->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
	}
	else if (pButton->GetTag() == -7)
	{
		mPauseButton->SetVisible(true);
		GetConsoleManager()->PushYieldCommand(strutil::Format(_T("set-level-index %i"), GetCurrentLevelNumber()+1));
		mMenu->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
	}
	else if (pButton->GetTag() == -8)
	{
		mPauseButton->SetVisible(true);
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));
		mMenu->DismissDialog();
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
	}
	else if (pButton->GetTag() == -9)
	{
		mPauseButton->SetVisible(true);
		HiResTimer::StepCounterShadow();
		v_set(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
	}
}



void FireManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}
}



bool FireManager::DisableAmbient()
{
	UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
	mStoreLightsEnabled = lRenderer->GetLightsEnabled();
	if (mStoreLightsEnabled)
	{
		lRenderer->GetAmbientLight(mStoreAmbient.x, mStoreAmbient.y, mStoreAmbient.z);
		lRenderer->SetAmbientLight(1.0f, 1.0f, 1.0f);
		lRenderer->EnableAllLights(false);
	}
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return true;
}

void FireManager::EnableAmbient()
{
	UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
	if (mStoreLightsEnabled)
	{
		lRenderer->EnableAllLights(true);
		lRenderer->SetAmbientLight(mStoreAmbient.x, mStoreAmbient.y, mStoreAmbient.z);
	}
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
}



loginstance(GAME, FireManager);



}
