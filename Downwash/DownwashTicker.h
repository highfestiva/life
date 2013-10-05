
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "Downwash.h"



namespace UiCure
{
class MusicPlayer;
class RendererImageResource;
}
namespace Life
{
class GameClientSlaveManager;
}



namespace Downwash
{



class DownwashTicker: public Life::GameClientMasterTicker
{
	typedef Life::GameClientMasterTicker Parent;
public:
	DownwashTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~DownwashTicker();

private:
	void Suspend();
	void Resume();
	virtual bool CreateSlave();
	virtual void OnSlavesKilled();
	virtual void OnServerCreated(Life::UiGameServerManager* pServer);

	virtual bool OpenUiManager();

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
	/*static Life::GameClientSlaveManager* CreateViewer(Life::GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);*/

	bool mIsPlayerCountViewActive;
	int mPerformanceAdjustmentTicks;
	UiCure::MusicPlayer* mMusicPlayer;
	UiCure::RendererImageResource* mEnvMap;

	LOG_CLASS_DECLARE();
};



}
