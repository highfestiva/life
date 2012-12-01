
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "Push.h"
#include "Sunlight.h"



namespace Life
{
class GameClientSlaveManager;
}



namespace Push
{



class Sunlight;



class PushTicker: public Life::GameClientMasterTicker
{
	typedef Life::GameClientMasterTicker Parent;
public:
	PushTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~PushTicker();

	Sunlight* GetSunlight() const;

private:
	virtual bool CreateSlave();
	virtual void OnSlavesKilled();

	virtual bool Reinitialize();
	virtual bool OpenUiManager();
	void DisplaySplashLogo();
	void DisplayCompanyLogo();

	virtual void BeginRender(Vector3DF& pColor);

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
	static Life::GameClientSlaveManager* CreateDemo(Life::GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);

	bool mIsPlayerCountViewActive;
	HiResTimer* mDemoTime;
	Sunlight* mSunlight;		// TODO: remove hack and come up with something better?

	LOG_CLASS_DECLARE();
};



}
