
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "HoverTank.h"
#include "Sunlight.h"



namespace Life
{
class GameClientSlaveManager;
}



namespace HoverTank
{



class Sunlight;



class HoverTankTicker: public Life::GameClientMasterTicker
{
	typedef Life::GameClientMasterTicker Parent;
public:
	HoverTankTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~HoverTankTicker();

	Sunlight* GetSunlight() const;

private:
	virtual bool CreateSlave();
	virtual void OnSlavesKilled();
	virtual void OnServerCreated(Life::UiGameServerManager* pServer);

	virtual bool Reinitialize();
	virtual bool OpenUiManager();
	void DisplaySplashLogo();
	void DisplayCompanyLogo();

	virtual void BeginRender(Vector3DF& pColor);
	void PreWaitPhysicsTick();

	virtual void CloseMainMenu();
	virtual bool QueryQuit();

	typedef Life::GameClientSlaveManager* (*SlaveFactoryMethod)(Life::GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);
	static Life::GameClientSlaveManager* CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);
	static Life::GameClientSlaveManager* CreateViewer(Life::GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);

	bool mIsPlayerCountViewActive;
	Sunlight* mSunlight;		// TODO: remove hack and come up with something better?
	int mPerformanceAdjustmentTicks;

	LOG_CLASS_DECLARE();
};



}
