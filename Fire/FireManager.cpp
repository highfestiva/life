
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
#include "../UiLepra/Include/UiTouchDrag.h"
#include "../UiTBC/Include/GUI/UiCheckButton.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFixedLayouter.h"
#include "../UiTBC/Include/GUI/UiRadioButton.h"
#include "../UiTBC/Include/GUI/UiTextArea.h"
#include "../UiTBC/Include/GUI/UiTextField.h"
#include "../UiTBC/Include/UiBillboardGeometry.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "../UiTBC/Include/UiRenderer.h"
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

#define BG_COLOR			Color(110, 110, 110, 160)



namespace Fire
{



FireManager::FireManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mMenu(0),
	mLevel(0),
	mSunlight(0),
	mCameraTransform(QuaternionF(), Vector3DF()),
	mPauseButton(0)
	//mCheckIcon(0),
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

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_FIREDELAY, 0.5);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_STARTLEVEL, _T("lvl00"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVELCOUNT, 14);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
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
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTFLAGS, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 40.0);

	Parent::LoadSettings();

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_ENABLECLEAR, false);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);

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
		mPauseButton = ICONBTNA("btn_pause.png", "");
		int x = 12;
		int y = 12;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(FireManager, OnPauseButton);
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



bool FireManager::Render()
{
	// TODO: render background image.

	return Parent::Render();
}

bool FireManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}

	// TODO: draw targeting crosshairs.

	return true;
}

void FireManager::DrawSyncDebugInfo()
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



bool FireManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraTransform.GetPosition()) <= pDistance*pDistance);
}



void FireManager::Shoot(Cure::ContextObject* pAvatar, int pWeapon)
{
	(void)pAvatar;
	(void)pWeapon;

	double lFireDelay;
	CURE_RTVAR_GET(lFireDelay, =, GetVariableScope(), RTVAR_GAME_FIREDELAY, 0.5);
	if (!mLevel->IsLoaded() || mFireDelayTimer.QueryTimeDiff() < lFireDelay)
	{
		return;
	}
	mFireDelayTimer.Start();
	Vector3DF lTargetPosition;
	if (!GetPhysicsManager()->QueryRayCollisionAgainst(mCameraTransform.GetPosition(), mShootDirection, 1000.0f, mLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), &lTargetPosition, 1) == 1)
	{
		return;
	}

	{
		Cure::ContextObject* lObject = Parent::CreateContextObject(_T("indicator"), Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
		lObject->SetInitialTransform(TransformationF(gIdentityQuaternionF, lTargetPosition));
		lObject->StartLoading();
		DeleteContextObjectDelay(lObject, 1.5f);
	}

	Life::Projectile* lProjectile = new Life::Projectile(GetResourceManager(), _T("rocket"), mUiManager, this);
	AddContextObject(lProjectile, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
	TransformationF t(mCameraTransform);
	t.GetPosition().x += 0.4f;
	t.GetPosition().y -= 0.7f;
	t.GetOrientation().RotateAroundWorldX(-PIF/2);	// Tilt rocket.
	const Vector3DF lDistance = lTargetPosition - t.GetPosition();
	float lAcceleration;
	float lTerminalVelocity;
	CURE_RTVAR_TRYGET(lAcceleration, =(float), GetVariableScope(), "shot.acceleration", 140.0);
	CURE_RTVAR_TRYGET(lTerminalVelocity, =(float), GetVariableScope(), "shot.terminalvelocity", 110.0);
	const Vector3DF lShootDirectionEulerAngles = Life::ProjectileUtil::CalculateInitialProjectileDirection(lDistance, lAcceleration, lTerminalVelocity, Vector3DF(0,0,-9.82f));
	t.GetOrientation().RotateAroundWorldX(lShootDirectionEulerAngles.y);
	t.GetOrientation().RotateAroundWorldZ(lShootDirectionEulerAngles.x);
	lProjectile->SetInitialTransform(t);
	lProjectile->StartLoading();
}

void FireManager::Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength)
{
	mCollisionSoundManager->OnCollision(pStrength, pPosition, pExplosiveGeometry, _T("explosion"));

	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const float lKeepOnGoingFactor = 0.5f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	Vector3DF u = pVelocity.ProjectOntoPlane(pNormal) * (1+lKeepOnGoingFactor);
	u -= pVelocity;	// Mirror and inverse.
	u.Normalize();
	const int lParticles = Math::Lerp(4, 8, pStrength * 0.2f);
	Vector3DF lStartFireColor(1.0f, 1.0f, 0.3f);
	Vector3DF lFireColor(0.6f, 0.4f, 0.2f);
	Vector3DF lStartSmokeColor(0.4f, 0.4f, 0.4f);
	Vector3DF lSmokeColor(0.2f, 0.2f, 0.2f);
	Vector3DF lShrapnelColor(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	lParticleRenderer->CreateExplosion(pPosition, pStrength, u, 1, 1, lStartFireColor, lFireColor, lStartSmokeColor, lSmokeColor, lShrapnelColor, lParticles, lParticles, lParticles/2, lParticles/2);

	// Slowmo check.
	bool lNormalDeath = true;
	if (Cure::Health::Get(pExplosive) < -5500)
	{
		if (Random::Uniform(0.0f, 1.0f) > 0.7f)
		{
			lNormalDeath = false;
			mSlowmoTimer.TryStart();
		}
	}

	// Shove!
	const bool lIsExplosive = (pExplosive->GetClassId().find(_T("rocket")) != str::npos);
	const float lExplosionFactor = lIsExplosive? 3.0f : 1.0f;
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
		float lForce = Life::Explosion::CalculateForce(lPhysicsManager, lObject, pPosition, pStrength * lExplosionFactor) * 0.3f;
		if (lForce > 0 && lObject->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY)
		{
			Cure::FloatAttribute* lHealth = Cure::Health::GetAttribute(lObject);
			if (lHealth)
			{
				const float lValue = lHealth->GetValue() - lForce*Random::Normal(0.51f, 0.05f, 0.3f, 0.5f);
				lHealth->SetValue(lValue);
			}
			x->second->ForceSend();
		}
		Life::Explosion::PushObject(lPhysicsManager, lObject, pPosition, pStrength*lExplosionFactor*0.1f);
	}
}

void FireManager::OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject)
{
	(void)pBullet;
	(void)pHitObject;
}



bool FireManager::DidFinishLevel()
{
	mLog.Headlinef(_T("Level %s done!"), mLevel->GetClassId().c_str());
	return true;
}

str FireManager::StepLevel(int pCount)
{
	int lLevelNumber = GetCurrentLevelNumber();
	lLevelNumber += pCount;
	int lLevelCount;
	CURE_RTVAR_GET(lLevelCount, =, GetVariableScope(), RTVAR_GAME_LEVELCOUNT, 14);
	if (lLevelNumber >= lLevelCount)
	{
		lLevelNumber = 0;
	}
	if (lLevelNumber < 0)
	{
		lLevelNumber = lLevelCount-1;
	}
	str lNewLevelName = strutil::Format(_T("lvl%2.2i"), lLevelNumber);
	mLevel = (Level*)Parent::CreateContextObject(lNewLevelName, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	mLevel->StartLoading();
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_STARTLEVEL, lNewLevelName);
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
	strutil::StringToInt(mLevel->GetClassId().substr(6), lLevelNumber);
	return lLevelNumber;
}



Cure::RuntimeVariableScope* FireManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool FireManager::InitializeUniverse()
{
	mUiManager->GetRenderer()->SetLineWidth(1);

	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const Vector3DF v;
	lParticleRenderer->CreateExplosion(Vector3DF(0,0,-2000), 1, v, 1, 1, v, v, v, v, v, 1, 1, 1, 1);

	str lStartLevel;
	CURE_RTVAR_GET(lStartLevel, =, GetVariableScope(), RTVAR_GAME_STARTLEVEL, _T("lvl00"));
	mLevel = (Level*)Parent::CreateContextObject(lStartLevel, Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	mLevel->StartLoading();
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
	}

	if (!GetResourceManager()->IsLoading())
	{
		mAllLoadedTimer.TryStart();
	}
	if (mAllLoadedTimer.QuerySplitTime() > 5.0)
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
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 0.3);
		}
		else
		{
			CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR, 1.0);
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
			const Vector3DF lPosition = lObject->GetPosition();
			if (!Math::IsInRange(lPosition.x, -250.0f, +250.0f) ||
				!Math::IsInRange(lPosition.y, -10.0f, +350.0f) ||
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
		mUiManager->SetMicrophonePosition(mCameraTransform, Vector3DF());
	}
}

void FireManager::HandleShooting()
{
	float lFOV;
	CURE_RTVAR_GET(lFOV, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 40.0);
	UpdateFrustum(lFOV);

	typedef UiLepra::Touch::DragManager::DragList DragList;
	const DragList& lDragList = mUiManager->GetDragManager()->GetDragList();
	for (DragList::const_iterator x = lDragList.begin(); x != lDragList.end(); ++x)
	{
		if (x->mIsNew && !mMenu->GetDialog())
		{
			mShootDirection = mUiManager->GetRenderer()->ScreenCoordToVector(x->mLast);
			Shoot(0, 0);
		}
	}
}



void FireManager::TickInput()
{
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
		lLevel->DisableRootShadow();
		lObject = lLevel;
	}
	else if (strutil::StartsWith(pClassId, _T("indicator")))
	{
		lObject = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
	}
	else
	{
		Life::ExplodingMachine* lMachine = new BaseMachine(GetResourceManager(), pClassId, mUiManager, (FireManager*)this);
		//lMachine->SetExhaustEmitter(new UiCure::ExhaustEmitter(GetResourceManager(), mUiManager));
		lMachine->SetBurnEmitter(new UiCure::BurnEmitter(GetResourceManager(), mUiManager));
		lMachine->SetDisappearAfterDeathDelay(30.0);
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
			Vector3DF lColor = RNDPOSVEC();
			((UiCure::CppContextObject*)pObject)->GetMesh(0)->GetBasicMaterialSettings().mDiffuse = lColor;
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
}

void FireManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);
}



void FireManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
		pButton->SetVisible(false);
	}

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &FireManager::OnMenuAlternative), 0.8f, 0.8f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);

	UiTbc::FixedLayouter lLayouter(d);

	double lMasterVolume;
	CURE_RTVAR_GET(lMasterVolume, =, GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);

	lLayouter.SetContentXMargin(0);
	UiTbc::RadioButton* lEasyButton = new UiTbc::RadioButton(Color(20, 30, 20), _T("Easy"));
	lEasyButton->SetPressColor(Color(50, 210, 40));
	lEasyButton->SetRoundedRadiusMask(0x9);
	lLayouter.AddButton(lEasyButton, -2, 1, 5, 0, 1, 3, false);
	UiTbc::RadioButton* lMediumButton = new UiTbc::RadioButton(Color(30, 30, 20), _T("Medium"));
	lMediumButton->SetPressColor(Color(170, 165, 10));
	lMediumButton->SetRoundedRadiusMask(0);
	lLayouter.AddButton(lMediumButton, -3, 1, 5, 1, 1, 3, false);
	UiTbc::RadioButton* lHardButton = new UiTbc::RadioButton(Color(30, 20, 20), _T("Hard"));
	lHardButton->SetPressColor(Color(230, 40, 30));
	lHardButton->SetRoundedRadiusMask(0x6);
	lLayouter.AddButton(lHardButton, -4, 1, 5, 2, 1, 3, false);
	lLayouter.SetContentXMargin(lLayouter.GetContentYMargin());

	UiTbc::CheckButton* lBedsideVolumeButton = new UiTbc::CheckButton(Color(190, 50, 180), _T("Bedside volume"));
	lBedsideVolumeButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	//lBedsideVolumeButton->SetCheckedIcon(mCheckIcon->GetData());
	lBedsideVolumeButton->SetPressed(lMasterVolume < 0.5);
	lLayouter.AddButton(lBedsideVolumeButton, -6, 3, 5, 0, 1, 2, false);
	UiTbc::Button* lHiscoreButton = new UiTbc::Button(Color(90, 50, 10), _T("High score"));
	lHiscoreButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	lLayouter.AddButton(lHiscoreButton, -7, 3, 5, 1, 1, 2, true);

	UiTbc::Button* lRestartButton = new UiTbc::Button(Color(220, 110, 20), _T("Restart from first level"));
	lRestartButton->SetIcon(UiTbc::Painter::INVALID_IMAGEID, UiTbc::Button::ICON_RIGHT);
	lLayouter.AddButton(lRestartButton, -8, 4, 5, 0, 1, 1, true);

	UiTbc::Button* lCloseButton = new UiTbc::Button(Color(180, 60, 50), _T("X"));
	lLayouter.AddCornerButton(lCloseButton, -9);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void FireManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == -6)
	{
		double lBedsideVolume = (pButton->GetState() == UiTbc::Button::PRESSED)? 0.02 : 1.0;
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, lBedsideVolume);
		mUiManager->GetSoundManager()->SetMasterVolume((float)lBedsideVolume);	// Set right away for button volume.
	}
	else if (pButton->GetTag() == -8)
	{
		mPauseButton->SetVisible(true);
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));
		mMenu->DismissDialog();
		HiResTimer::StepCounterShadow();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
	}
	else if (pButton->GetTag() == -9)
	{
		mPauseButton->SetVisible(true);
		HiResTimer::StepCounterShadow();
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
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



LOG_CLASS_DEFINE(GAME, FireManager);



}
