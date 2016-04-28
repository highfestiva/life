
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Lepra/Include/LepraTarget.h"
#include "PushTicker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeClient/UiGameServerManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../Life/LifeServer/ServerMessageProcessor.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiParticleLoader.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../UiTbc/Include/UiParticleRenderer.h"
#include "PushServer/PushServerDelegate.h"
#include "RtVar.h"
#include "PushDemo.h"
#include "PushViewer.h"



namespace Push
{



PushTicker::PushTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mIsPlayerCountViewActive(false),
	mDemoTime(0),
	mSunlight(0)
{
	v_override(UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	v_override(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
}

PushTicker::~PushTicker()
{
	CloseMainMenu();

	delete mSunlight;
	mSunlight = 0;
}



Sunlight* PushTicker::GetSunlight() const
{
	return mSunlight;
}



bool PushTicker::CreateSlave()
{
	return (Parent::CreateSlave(&PushTicker::CreateSlaveManager));
}

void PushTicker::OnSlavesKilled()
{
	DeleteServer();
	//deb_assert(!mIsPlayerCountViewActive);
	mIsPlayerCountViewActive = true;
	mSlaveTopSplit = 1.0f;
	Parent::CreateSlave(&PushTicker::CreateViewer);
}

void PushTicker::OnServerCreated(Life::UiGameServerManager* pServer)
{
	PushServerDelegate* lDelegate = new PushServerDelegate(pServer);
	pServer->SetDelegate(lDelegate);
	pServer->SetMessageProcessor(new Life::ServerMessageProcessor(pServer));
}



bool PushTicker::Reinitialize()
{
	delete mSunlight;
	mSunlight = 0;
	bool lOk = Parent::Reinitialize();
	mSunlight = new Sunlight(mUiManager);
	return lOk;
}

bool PushTicker::OpenUiManager()
{
	bool lOk = mUiManager->OpenDraw();
	if (lOk)
	{
		UiLepra::Core::ProcessMessages();
		mUiManager->GetPainter()->ResetClippingRect();
		mUiManager->GetPainter()->Clear(BLACK);
		DisplaySplashLogo();
	}
	if (lOk)
	{
		mUiManager->UpdateSettings();
		UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
		lRenderer->AddDynamicRenderer("particle", new UiTbc::ParticleRenderer(lRenderer, 1));
		UiCure::ParticleLoader lLoader(mResourceManager, lRenderer, "explosion.png", 4, 5);
	}
	if (lOk)
	{
		UiCure::RendererImageResource* lEnvMap = new UiCure::RendererImageResource(mUiManager, mResourceManager, "env.png", UiCure::ImageProcessSettings(Canvas::RESIZE_FAST, true));
		if (lEnvMap->Load())
		{
			if (lEnvMap->PostProcess() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				UiTbc::Renderer::TextureID lTextureId = lEnvMap->GetUserData(0);
				mUiManager->GetRenderer()->SetEnvironmentMap(lTextureId);
			}
		}
	}
	if (lOk)
	{
		lOk = mUiManager->OpenRest();
	}
	if (lOk)
	{
		mUiManager->GetDesktopWindow()->CreateLayer(new UiTbc::FloatingLayout());
		DisplayCompanyLogo();
	}
	return lOk;
}

void PushTicker::DisplaySplashLogo()
{
	UiCure::PainterImageResource* lLogo = new UiCure::PainterImageResource(mUiManager, mResourceManager, "logo.png", UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
	if (lLogo->Load())
	{
		if (lLogo->PostProcess() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			//mUiManager->BeginRender(vec3(0, 1, 0));
			mUiManager->PreparePaint(true);
			const Canvas* lCanvas = mUiManager->GetCanvas();
			const Canvas* lImage = lLogo->GetRamData();
			mUiManager->GetPainter()->DrawImage(lLogo->GetUserData(0), lCanvas->GetWidth()/2 - lImage->GetWidth()/2, lCanvas->GetHeight()/2 - lImage->GetHeight()/2);
			mUiManager->GetDisplayManager()->UpdateScreen();
		}
	}
	delete lLogo;
}

void PushTicker::DisplayCompanyLogo()
{
	bool lShowLogo;
	v_get(lShowLogo, =, UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, true);
	if (lShowLogo)
	{
		Cure::UserRamImageResource* lLogo = new Cure::UserRamImageResource;
		Cure::UserResourceOwner<Cure::UserRamImageResource> lLogoHolder(lLogo, mResourceManager, "megaphone.png");
		UiCure::UserSound2dResource* lLogoSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		Cure::UserResourceOwner<UiCure::UserSound2dResource> lLogoSoundHolder(lLogoSound, mResourceManager, "logo_trumpet.wav");
		for (int x = 0; x < 1000; ++x)
		{
			mResourceManager->Tick();
			if (lLogo->GetLoadState() != Cure::RESOURCE_LOAD_IN_PROGRESS &&
				lLogoSound->GetLoadState() != Cure::RESOURCE_LOAD_IN_PROGRESS)
			{
				break;
			}
			Thread::Sleep(0.001);
		}
		if (lLogo->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
			lLogoSound->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			mUiManager->GetSoundManager()->Play(lLogoSound->GetData(), 1, 1);

			UiLepra::Canvas& lCanvas = *lLogo->GetRamData();
			const UiTbc::Painter::ImageID lImageId = mUiManager->GetDesktopWindow()->GetImageManager()->AddImage(lCanvas, UiTbc::GUIImageManager::STRETCHED, UiTbc::GUIImageManager::NO_BLEND, 255);
			UiTbc::RectComponent lRect(lImageId);
			mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
			mUiManager->GetDesktopWindow()->AddChild(&lRect, 0, 0, 0);
			const unsigned lWidth = mUiManager->GetDisplayManager()->GetWidth();
			const unsigned lHeight = mUiManager->GetDisplayManager()->GetHeight();
			lRect.SetPreferredSize(lCanvas.GetWidth(), lCanvas.GetHeight());
			const unsigned lTargetX = lWidth/2 - lCanvas.GetWidth()/2;
			const unsigned lTargetY = lHeight/2 - lCanvas.GetHeight()/2;
			mUiManager->GetRenderer()->ResetClippingRect();
			Color lColor;
			mUiManager->GetRenderer()->SetClearColor(Color());
			mUiManager->GetDisplayManager()->SetVSyncEnabled(true);

			const float lMin = 0;
			const float lMax = 26;
			const int lStepCount = 50;
			const float lBaseStep = (lMax-lMin)/(float)lStepCount;
			float lBase = -lMax;
			int lCount = 0;
			const int lTotalFrameCount = 600;
			for (lCount = 0; lCount <= lTotalFrameCount && SystemManager::GetQuitRequest() == 0; ++lCount)
			{
				if (lCount < lStepCount || lCount > lTotalFrameCount-lStepCount)
				{
					lBase += lBaseStep;
				}
				int lMovement = (int)(::fabs(lBase)*lBase*3);
				lRect.SetPos(lTargetX+lMovement, lTargetY);

				mUiManager->GetRenderer()->Clear();
				mUiManager->Paint(false);
				mUiManager->GetDisplayManager()->UpdateScreen();

				Thread::Sleep(0.01);
				mUiManager->InputTick();
				//lOk = (SystemManager::GetQuitRequest() <= 0);

				if (lCount == lStepCount)
				{
					lBase = 0;
				}
				else if (lCount == lTotalFrameCount-lStepCount)
				{
					lBase = lMin;
				}
			}
			mUiManager->GetDesktopWindow()->RemoveChild(&lRect, 0);
			mUiManager->GetDesktopWindow()->GetImageManager()->RemoveImage(lImageId);
		}
	}
	mResourceManager->ForceFreeCache();

}

void PushTicker::BeginRender(vec3& pColor)
{
	float lRealTimeRatio;
	v_get(lRealTimeRatio, =(float), UiCure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	float lTimeOfDayFactor;
	v_get(lTimeOfDayFactor, =(float), UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	mSunlight->Tick(lRealTimeRatio * lTimeOfDayFactor);

	mSunlight->AddSunColor(pColor, 2);
	Parent::BeginRender(pColor);

	vec3 lColor(1.2f, 1.2f, 1.2f);
	mSunlight->AddSunColor(lColor, 1);
	Color lFillColor;
	lFillColor.Set(lColor.x, lColor.y, lColor.z, 1.0f);
	mUiManager->GetRenderer()->SetOutlineFillColor(lFillColor);
}



void PushTicker::CloseMainMenu()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}

bool PushTicker::QueryQuit()
{
	if (mDemoTime)
	{
		// We quit if user tried quitting twice or more, or demo time is over.
		return (SystemManager::GetQuitRequest() >= 2 || mDemoTime->QueryTimeDiff() > 30.0f);
	}

	if (Parent::QueryQuit())
	{
		PrepareQuit();
		for (int x = 0; x < 4; ++x)
		{
			DeleteSlave(mSlaveArray[x], false);
		}
		DeleteServer();
#ifdef PUSH_DEMO
		if (!mUiManager->CanRender())
		{
			return true;
		}
		CreateSlave(&PushTicker::CreateDemo);
		mDemoTime = new HiResTimer;
#else // !Demo
		return (true);
#endif // Demo / !Demo
	}
	return (false);
}



Life::GameClientSlaveManager* PushTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	PushManager* lGameManager = new PushManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
	lGameManager->SetMasterServerConnection(new Life::MasterServerConnection(pMaster->GetMasterServerConnection()->GetMasterAddress()));
	return lGameManager;
}

Life::GameClientSlaveManager* PushTicker::CreateViewer(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new PushViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

Life::GameClientSlaveManager* PushTicker::CreateDemo(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
#ifdef PUSH_DEMO
	return new PushDemo(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#else // !Demo
	return new PushViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
#endif // Demo / !Demo
}



loginstance(GAME, PushTicker);



}
