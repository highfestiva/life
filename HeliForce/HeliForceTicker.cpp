
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#include "HeliForceTicker.h"
#include "../Lepra/Include/SystemManager.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiParticleLoader.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../UiTBC/Include/UiParticleRenderer.h"
#include "RtVar.h"
#include "HeliForceManager.h"



namespace HeliForce
{



HeliForceTicker::HeliForceTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity):
	Parent(pUiManager, pResourceManager, pPhysicsRadius, pPhysicsLevels, pPhysicsSensitivity),
	mIsPlayerCountViewActive(false),
	mPerformanceAdjustmentTicks(0),
	mEnvMap(0)
{
	CURE_RTVAR_SYS_OVERRIDE(UiCure::GetSettings(), RTVAR_UI_3D_ENABLEMASSOBJECTFADING, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_PHYSICS_ISFIXEDFPS, false);
}

HeliForceTicker::~HeliForceTicker()
{
	CloseMainMenu();
	delete mEnvMap;
	mEnvMap = 0;
}



bool HeliForceTicker::CreateSlave()
{
	return (Parent::CreateSlave(&HeliForceTicker::CreateSlaveManager));
}

void HeliForceTicker::OnSlavesKilled()
{
	/*DeleteServer();
	//deb_assert(!mIsPlayerCountViewActive);
	mIsPlayerCountViewActive = true;
	mSlaveTopSplit = 1.0f;
	Parent::CreateSlave(&HeliForceTicker::CreateViewer);*/
	CreateSlave();
}

void HeliForceTicker::OnServerCreated(Life::UiGameServerManager*)
{
}



bool HeliForceTicker::OpenUiManager()
{
	bool lOk = mUiManager->OpenDraw();
	if (lOk)
	{
#ifdef LEPRA_TOUCH
		mUiManager->GetCanvas()->SetOutputRotation(90);
#endif // Touch
		UiLepra::Core::ProcessMessages();
		//mUiManager->GetPainter()->ResetClippingRect();
		//mUiManager->GetPainter()->Clear(BLACK);
		//DisplaySplashLogo();
	}
	if (lOk)
	{
		mUiManager->UpdateSettings();
		UiTbc::Renderer* lRenderer = mUiManager->GetRenderer();
		lRenderer->AddDynamicRenderer(_T("particle"), new UiTbc::ParticleRenderer(lRenderer, 0));
		UiCure::ParticleLoader lLoader(mResourceManager, lRenderer, _T("explosion.png"), 4, 5);
	}
	if (lOk)
	{
		mEnvMap = new UiCure::RendererImageResource(mUiManager, mResourceManager, _T("env.png"), true);
		if (mEnvMap->Load())
		{
			if (mEnvMap->PostProcess() == Cure::RESOURCE_LOAD_COMPLETE)
			{
				UiTbc::Renderer::TextureID lTextureId = mEnvMap->GetUserData(0);
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
	}
	return lOk;
}

void HeliForceTicker::BeginRender(Vector3DF& pColor)
{
	Parent::BeginRender(pColor);
	mUiManager->GetRenderer()->SetOutlineFillColor(OFF_BLACK);
}

void HeliForceTicker::PreWaitPhysicsTick()
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



void HeliForceTicker::CloseMainMenu()
{
	if (mIsPlayerCountViewActive)
	{
		DeleteSlave(mSlaveArray[0], false);
		mIsPlayerCountViewActive = false;
	}
}

bool HeliForceTicker::QueryQuit()
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



Life::GameClientSlaveManager* HeliForceTicker::CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new HeliForceManager(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}

/*Life::GameClientSlaveManager* HeliForceTicker::CreateViewer(Life::GameClientMasterTicker* pMaster,
	Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
	int pSlaveIndex, const PixelRect& pRenderArea)
{
	return new HeliForceViewer(pMaster, pTime, pVariableScope, pResourceManager, pUiManager, pSlaveIndex, pRenderArea);
}*/



LOG_CLASS_DEFINE(GAME, HeliForceTicker);



}