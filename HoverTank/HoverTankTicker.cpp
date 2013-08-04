
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#include "HoverTankTicker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../Life/LifeClient/UiGameServerManager.h"
#include "../Life/LifeServer/MasterServerConnection.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiParticleLoader.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "HoverTankServer/HoverTankServerDelegate.h"
#include "HoverTankServer/HoverTankServerMessageProcessor.h"
#include "RtVar.h"
#include "HoverTankViewer.h"



namespace HoverTank
{



HoverTankTicker::HoverTankTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mIsPlayerCountViewActive(false),
	mSunlight(0),
	mPerformanceAdjustmentTicks(0)
{
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
}

HoverTankTicker::~HoverTankTicker()
{
	CloseMainMenu();

	delete mSunlight;
	mSunlight = 0;
}



Sunlight* HoverTankTicker::GetSunlight() const
{
	return mSunlight;
}



bool HoverTankTicker::CreateSlave()
{
	return (Parent::CreateSlave(&HoverTankTicker::CreateSlaveManager));
}

void HoverTankTicker::OnSlavesKilled()
{
	DeleteServer();
	//deb_assert(!mIsPlayerCountViewActive);
	mIsPlayerCountViewActive = true;
	mSlaveTopSplit = 1.0f;
	Parent::CreateSlave(&HoverTankTicker::CreateViewer);
}

void HoverTankTicker::OnServerCreated(Life::UiGameServerManager* pServer)
{
	HoverTankServerDelegate* lDelegate = new HoverTankServerDelegate(pServer);
	pServer->SetDelegate(lDelegate);
	pServer->SetMessageProcessor(new HoverTankServerMessageProcessor(pServer, lDelegate));
}



bool HoverTankTicker::Reinitialize()
{
	delete mSunlight;
	mSunlight = 0;
	bool lOk = Parent::Reinitialize();
	mSunlight = new Sunlight(mUiManager);
	return lOk;
}

bool HoverTankTicker::OpenUiManager()
{
	bool lOk = mUiManager->OpenDraw();
	if (lOk)
	{
#ifdef LEPRA_TOUCH
		mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // Touch
		UiLepra::Core::ProcessMessages();
		mUiManager->GetPainter()->ResetClippingRect();
		mUiManager->GetPainter()->Clear(BLACK);
		DisplaySplashLogo();
	}
	if (lOk)
	{
		mUiManager->UpdateSettings();
		UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
		lRenderer->AddDynamicRenderer(_T("particle"), new UiTbc::ParticleRenderer(lRenderer, 1));
		UiCure::ParticleLoader lLoader(mResourceManager, lRenderer, _T("explosion.png"), 4, 5);
	}
	if (lOk)
	{
		UiCure::RendererImageResource* lEnvMap = new UiCure::RendererImageResource(mUiManager, mResourceManager, _T("env.png"), true);
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

void HoverTankTicker::DisplaySplashLogo()
{
	UiCure::PainterImageResource* lLogo = new UiCure::PainterImageResource(mUiManager, mResourceManager, _T("logo.png"), UiCure::PainterImageResource::RELEASE_FREE_BUFFER);
	if (lLogo->Load())
	{
		if (lLogo->PostProcess() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			//mUiManager->BeginRender(Vector3DF(0, 1, 0));
			mUiManager->PreparePaint(true);
			const Canvas* lCanvas = mUiManager->GetCanvas();
			const Canvas* lImage = lLogo->GetRamData();
			mUiManager->GetPainter()->DrawImage(lLogo->GetUserData(0), lCanvas->GetWidth()/2 - lImage->GetWidth()/2, lCanvas->GetHeight()/2 - lImage->GetHeight()/2);
			mUiManager->GetDisplayManager()->UpdateScreen();
		}
	}
	delete lLogo;
}

void HoverTankTicker::DisplayCompanyLogo()
{
	bool lShowLogo;
	CURE_RTVAR_GET(lShowLogo, =, UiCure::GetSettings(), RTVAR_GAME_ENABLESTARTLOGO, true);
	if (lShowLogo)
	{
		Cure::UserRamImageResource* lLogo = new Cure::UserRamImageResource;
		Cure::UserResourceOwner<Cure::UserRamImageResource> lLogoHolder(lLogo, mResourceManager, _T("megaphone.png"));
		UiCure::UserSound2dResource* lLogoSound = new UiCure::UserSound2dResource(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		Cure::UserResourceOwner<UiCure::UserSound2dResource> lLogoSoundHolder(lLogoSound, mResourceManager, _T("logo_trumpet.wav"));
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
			UiTbc::RectComponent lRect(lImageId, _T("logo"));
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

void HoverTankTicker::BeginRender(Vector3DF& pColor)
{
	float lRealTimeRatio;
	CURE_RTVAR_GET(lRealTimeRatio, =(float), UiCure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	float lTimeOfDayFactor;
	CURE_RTVAR_GET(lTimeOfDayFactor, =(float), UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	mSunlight->Tick(lRealTimeRatio * lTimeOfDayFactor);

	mSunlight->AddSunColor(pColor, 2);
	Parent::BeginRender(pColor);

	//Vector3DF lColor(1.2f, 1.2f, 1.2f);
	//mSunlight->AddSunColor(lColor, 1);
	const Color lFillColor = OFF_BLACK;
	//lFillColor.Set(lColor.x, lColor.y, lColor.z, 1.0f);
	mUiManager->GetRenderer()->SetOutlineFillColor(lFillColor);
}

void HoverTankTicker::PreWaitPhysicsTick()
{
	++mPerformanceAdjustmentTicks;
	if (mPerformanceAdjustmentTicks & 0x3F)
	{
		return;
	}
	const int lAdjustmentIndex = (mPerformanceAdjustmentTicks >> 7);

	bool lEnableAutoPerformance;
	CURE_RTVAR_GET(lEnableAutoPerformance, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEAUTOPERFORMANCE, true);
	if (!lEnableAutoPerformance)
	{
		return;
	}
	

	double lPerformanceLoad;
	CURE_RTVAR_TRYGET(lPerformanceLoad, =, UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_LOAD, 0.95);
	switch (lAdjustmentIndex)
	{
		default:
		{
			mPerformanceAdjustmentTicks = 0;
		}
		break;
		case 1:
		{
			bool lEnableMassObjects;
			CURE_RTVAR_GET(lEnableMassObjects, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTS, true);
			bool lEnableMassObjectFading;
			CURE_RTVAR_GET(lEnableMassObjectFading, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, true);
			if (lPerformanceLoad > 1)
			{
				CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
			}
			else if (lPerformanceLoad < 0.6 && lEnableMassObjects)
			{
				CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, true);
			}
		}
		break;
		case 2:
		{
			double lExhaustIntensity;
			CURE_RTVAR_GET(lExhaustIntensity, =, UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, 1.0);
			if (lPerformanceLoad > 0.9)
			{
				lExhaustIntensity = Math::Lerp(lExhaustIntensity, 1.9-lPerformanceLoad, 0.1);
			}
			else if (lPerformanceLoad < 0.8)
			{
				lExhaustIntensity = Math::Lerp(lExhaustIntensity, 1.0, 0.3);
			}
			CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_EXHAUSTINTENSITY, lExhaustIntensity);
		}
		break;
		case 3:
		{
			bool lEnableMassObjects;
			CURE_RTVAR_GET(lEnableMassObjects, =, UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTS, true);
			if (lPerformanceLoad > 1)
			{
				CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTS, false);
			}
			else if (lPerformanceLoad < 0.2)
			{
				CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTS, true);
			}
		}
		break;
	}
}



void HoverTankTicker::CloseMainMenu()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}

bool HoverTankTicker::QueryQuit()
{
	if (Parent::QueryQuit())
	{
		PrepareQuit();
		for (int x = 0; x < 4; ++x)
		{
			DeleteSlave(mSlaveArray[x], false);
		}
		DeleteServer();
		return (true);
	}
	return (false);
}



Life::GameClientSlaveManager* HoverTankTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	HoverTankManager* lGameManager = new HoverTankManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
	lGameManager->SetMasterServerConnection(new Life::MasterServerConnection(pMaster->GetMasterServerConnection()->GetMasterAddress()));
	return lGameManager;
}

Life::GameClientSlaveManager* HoverTankTicker::CreateViewer(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new HoverTankViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}



LOG_CLASS_DEFINE(GAME, HoverTankTicker);



}