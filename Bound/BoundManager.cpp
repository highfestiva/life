
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "BoundManager.h"
#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../UiCure/Include/UiCollisionSoundManager.h"
#include "../UiCure/Include/UiIconButton.h"
#include "../UiCure/Include/UiMachine.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFixedLayouter.h"
#include "../UiTBC/Include/UiMaterial.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "../Life/LifeClient/UiConsole.h"
#include "BoundConsoleManager.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "Version.h"

#define BG_COLOR			Color(110, 110, 110, 160)



namespace Bound
{



BoundManager::BoundManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Parent(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea),
	mCollisionSoundManager(0),
	mMenu(0),
	mSunlight(0),
	mCameraTransform(QuaternionF(), Vector3DF(0, -10, 0)),
	mLevelCompleted(false),
	mPauseButton(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("explosion"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->PreLoadSound(_T("explosion"));

	SetConsoleManager(new BoundConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_SOUND_MASTERVOLUME, 1.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
}

BoundManager::~BoundManager()
{
	Close();

	delete mCollisionSoundManager;
	mCollisionSoundManager = 0;
}

void BoundManager::Suspend()
{
	if (!mMenu->GetDialog())
	{
		mPauseButton->SetVisible(false);
		OnPauseButton(0);
	}
}

void BoundManager::LoadSettings()
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_SPAWNPART, 1.0);

	Parent::LoadSettings();

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONT, _T("Verdana"));
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_2D_FONTFLAGS, 0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, 30.0);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_NOCLIP, false);

	GetConsoleManager()->ExecuteCommand(_T("bind-key F5 prev-level"));
	GetConsoleManager()->ExecuteCommand(_T("bind-key F6 next-level"));
}

void BoundManager::SaveSettings()
{
}

bool BoundManager::Open()
{
	bool lOk = Parent::Open();
	if (lOk)
	{
		mPauseButton = ICONBTNA("btn_pause.png", "");
		int x = mRenderArea.GetCenterX() - 32;
		int y = mRenderArea.mBottom - 76;
		mUiManager->GetDesktopWindow()->AddChild(mPauseButton, x, y);
		mPauseButton->SetVisible(true);
		mPauseButton->SetOnClick(BoundManager, OnPauseButton);
	}
	if (lOk)
	{
		mMenu = new Life::Menu(mUiManager, GetResourceManager());
		mMenu->SetButtonTapSound(_T("tap.wav"), 0.3f);
	}
	return lOk;
}

void BoundManager::Close()
{
	ScopeLock lLock(GetTickLock());
	delete mPauseButton;
	mPauseButton = 0;
	delete mMenu;
	mMenu = 0;
	if (mSunlight)
	{
		delete mSunlight;
		mSunlight = 0;
	}
	Parent::Close();
}

void BoundManager::SetIsQuitting()
{
	((BoundConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void BoundManager::SetFade(float pFadeAmount)
{
	(void)pFadeAmount;
}



bool BoundManager::Render()
{
	mUiManager->GetPainter()->ResetClippingRect();
	mUiManager->GetRenderer()->SetDepthWriteEnabled(false);
	mUiManager->GetRenderer()->SetDepthTestEnabled(false);
	mUiManager->GetRenderer()->SetLightsEnabled(false);
	mUiManager->GetRenderer()->SetTexturingEnabled(false);
	mUiManager->GetPainter()->SetColor(GREEN, 0);
	mUiManager->GetPainter()->SetColor(GREEN, 1);
	mUiManager->GetPainter()->SetColor(RED, 2);
	mUiManager->GetPainter()->SetColor(BLUE, 3);
	mUiManager->GetPainter()->FillShadedRect(0, 0, mUiManager->GetCanvas()->GetWidth(), mUiManager->GetCanvas()->GetHeight());
	mUiManager->GetRenderer()->SetDepthWriteEnabled(true);
	mUiManager->GetRenderer()->SetDepthTestEnabled(true);
	mUiManager->GetRenderer()->SetLightsEnabled(true);
	mUiManager->GetRenderer()->Renderer::SetTexturingEnabled(true);

	return Parent::Render();
}

bool BoundManager::Paint()
{
	if (!Parent::Paint())
	{
		return false;
	}
	return true;
}



bool BoundManager::DidFinishLevel()
{
	int lLevel;
	CURE_RTVAR_GET(lLevel, =, GetVariableScope(), RTVAR_GAME_LEVEL, 0);
	mLog.Headlinef(_T("Level %i done!"), lLevel);
	/*Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
	if (lAvatar && lAvatar->GetPhysics()->GetEngineCount() >= 3)
	{
		const double lTime = mFlyTime.QuerySplitTime();
		const double lLevelBestTime = GetCurrentLevelBestTime(false);
		const bool lIsEasyMode = (GetControlMode() == 2);
		double lRtrOffset;
		CURE_RTVAR_GET(lRtrOffset, =, GetVariableScope(), RTVAR_PHYSICS_RTR_OFFSET, 0.0);
		const bool lIsToyMode = (lRtrOffset >= 0.5);
		if (lTime > 0 && (lTime < lLevelBestTime || lLevelBestTime <= 0) && !lIsEasyMode && !lIsToyMode)
		{
			SetLevelBestTime(GetCurrentLevelNumber(), false, lTime);

			str lPilotName;
			CURE_RTVAR_GET(lPilotName, =, GetVariableScope(), RTVAR_GAME_PILOTNAME, gDefaultPilotName);
			const bool lIsNonDefaultPilotName =  (lPilotName != gDefaultPilotName);
			if (lIsNonDefaultPilotName)
			{
				mHiscoreLevelIndex = GetCurrentLevelNumber();
				mMyHiscoreIndex = -1;
				mHiscoreJustUploadedTimer.Stop();
				CreateHiscoreAgent();
				const str lLevelName = strutil::Format(_T("level_%i"), GetCurrentLevelNumber());
				const int lNegativeTime = (int)(lTime*-1000);
				if (!mHiscoreAgent->StartUploadingScore(gPlatform, lLevelName, gVehicleName, lPilotName, lNegativeTime))
				{
					delete mHiscoreAgent;
					mHiscoreAgent = 0;
				}
			}
		}

		UiCure::UserSound2dResource* lFinishSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		new UiCure::SoundReleaser(GetResourceManager(), mUiManager, GetContext(), _T("finish.wav"), lFinishSound, 1.0f, 1.0f);
		mZoomPlatform = true;
		mLevelCompleted = true;
		mWinImageTimer.Start();
		return true;
	}*/
	return true;
}

int BoundManager::StepLevel(int pCount)
{
	/*if (GetContext()->GetObject(mAvatarId))
	{
		int lLevelNumber = GetCurrentLevelNumber();
		lLevelNumber += pCount;
		int lLevelCount;
		CURE_RTVAR_GET(lLevelCount, =, GetVariableScope(), RTVAR_GAME_LEVELCOUNT, 0);
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
		CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_LEVEL, lLevelNumber);
		return lNewLevelName;
	}
	return _T("");*/
	(void)pCount;
	return 0;
}



Cure::RuntimeVariableScope* BoundManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



bool BoundManager::InitializeUniverse()
{
	// Create dummy explosion to ensure all geometries loaded and ready, to avoid LAAAG when first exploading.
	UiTbc::ParticleRenderer* lParticleRenderer = (UiTbc::ParticleRenderer*)mUiManager->GetRenderer()->GetDynamicRenderer(_T("particle"));
	const Vector3DF v;
	lParticleRenderer->CreateExplosion(Vector3DF(0,0,-2000), 1, v, 1, v, v, v, v, v, 1, 1, 1, 1);

	Cure::ContextObject* lBall = Parent::CreateContextObject("soccerball", Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED, 0);
	lBall->StartLoading();
	mSunlight = new Sunlight(mUiManager);
	return true;
}

void BoundManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}

void BoundManager::TickUiInput()
{
	mUiManager->GetInputManager()->SetCursorVisible(true);

	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPhysicsStepCount > 0 && mAllowMovementInput)
	{
	}
}

void BoundManager::TickUiUpdate()
{
}

void BoundManager::SetLocalRender(bool pRender)
{
	(void)pRender;
}



Cure::ContextObject* BoundManager::CreateContextObject(const str& pClassId) const
{
	UiCure::Machine* lObject = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
	lObject->SetAllowNetworkLogic(true);
	return (lObject);
}

/*Cure::ContextObject* BoundManager::CreateLogicHandler(const str& pType)
{
}*/

void BoundManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	(void)pObject; (void)pOk;
	/*if (pOk)
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
	}*/
}

void BoundManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id)
{
	(void)pBody2Id;
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 5000, false);
}



void BoundManager::OnPauseButton(UiTbc::Button* pButton)
{
	if (pButton)
	{
		mMenu->OnTapSound(pButton);
		pButton->SetVisible(false);
	}

	UiTbc::Dialog* d = mMenu->CreateTbcDialog(Life::Menu::ButtonAction(this, &BoundManager::OnMenuAlternative), 0.8f, 0.8f);
	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	UiTbc::FixedLayouter lLayouter(d);

	UiTbc::Button* lPreviousLevelButton = new UiTbc::Button(Color(20, 110, 220), _T("Previous level"));
	lLayouter.AddButton(lPreviousLevelButton, -1, 0, 2, 0, 1, 1, true);

	UiTbc::Button* lRestartButton = new UiTbc::Button(Color(220, 110, 20), _T("1st level"));
	lLayouter.AddButton(lRestartButton, -2, 1, 2, 0, 1, 1, true);

	UiTbc::Button* lCloseButton = new UiTbc::Button(Color(180, 60, 50), _T("X"));
	lLayouter.AddCornerButton(lCloseButton, -9);

	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, true);
}

void BoundManager::OnMenuAlternative(UiTbc::Button* pButton)
{
	if (pButton->GetTag() == -2)
	{
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));	// Something, something...
	}
	else if (pButton->GetTag() == -2)
	{
		GetConsoleManager()->PushYieldCommand(_T("set-level-index 0"));
	}
	mPauseButton->SetVisible(true);
	HiResTimer::StepCounterShadow();
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_PHYSICS_HALT, false);
}



void BoundManager::ScriptPhysicsTick()
{
	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime > 1e-5)
	{
		MoveCamera();
		UpdateCameraPosition(false);
	}

	if (mNextLevelTimer.IsStarted())
	{
		if (mNextLevelTimer.QueryTimeDiff() > 5.0)
		{
			mNextLevelTimer.Stop();
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

	Parent::ScriptPhysicsTick();
}

void BoundManager::HandleWorldBoundaries()
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

void BoundManager::MoveCamera()
{
}

void BoundManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	mUiManager->SetCameraPosition(mCameraTransform);
	if (pUpdateMicPosition)
	{
		mUiManager->SetMicrophonePosition(mCameraTransform, Vector3DF());
	}
}



void BoundManager::DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const
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



void BoundManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource)
{
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(),
			UiTbc::GUIImageManager::CENTERED, UiTbc::GUIImageManager::ALPHABLEND, 255);
	}
}



LOG_CLASS_DEFINE(GAME, BoundManager);



}
