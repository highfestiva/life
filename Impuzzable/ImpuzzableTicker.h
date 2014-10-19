
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Life/LifeClient/GameClientMasterTicker.h"
#include "Impuzzable.h"



namespace UiCure
{
class MusicPlayer;
class RendererImageResource;
}
namespace Life
{
class GameClientSlaveManager;
}



namespace Impuzzable
{



class ImpuzzableTicker: public Life::GameClientMasterTicker
{
	typedef Life::GameClientMasterTicker Parent;
public:
	ImpuzzableTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~ImpuzzableTicker();

private:
	void Suspend();
	void Resume();
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

	bool mIsPlayerCountViewActive;
	UiCure::MusicPlayer* mMusicPlayer;
	UiCure::RendererImageResource* mEnvMap;

	logclass();
};



}
