
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "Fire.h"



namespace UiCure
{
class MusicPlayer;
class RendererImageResource;
}
namespace Life
{
class GameClientSlaveManager;
}



namespace Fire
{



class FireTicker: public Life::GameClientMasterTicker
{
	typedef Life::GameClientMasterTicker Parent;
public:
	FireTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~FireTicker();

private:
	virtual void Suspend(bool pHard);
	virtual void Resume(bool pHard);
	virtual bool CreateSlave();
	virtual void OnSlavesKilled();
	virtual void OnServerCreated(Life::UiGameServerManager* pServer);

	virtual bool OpenUiManager();

	virtual void BeginRender(vec3& pColor);
	void PreWaitPhysicsTick();

	virtual void CloseMainMenu();
	virtual bool QueryQuit();

	static Life::GameClientSlaveManager* CreateSlaveManager(Life::GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);

	UiCure::RendererImageResource* mEnvMap;

	logclass();
};



}
