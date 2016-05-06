
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/gameclientmasterticker.h"
#include "fire.h"



namespace UiCure {
class MusicPlayer;
class RendererImageResource;
}
namespace life {
class GameClientSlaveManager;
}



namespace Fire {



class FireTicker: public life::GameClientMasterTicker {
	typedef life::GameClientMasterTicker Parent;
public:
	FireTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager_, float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~FireTicker();

private:
	virtual void Suspend(bool hard);
	virtual void Resume(bool hard);
	virtual bool CreateSlave();
	virtual void OnSlavesKilled();
	virtual void OnServerCreated(life::UiGameServerManager* server);

	virtual bool OpenUiManager();

	virtual void BeginRender(vec3& color);
	void PreWaitPhysicsTick();

	virtual void CloseMainMenu();
	virtual bool QueryQuit();

	static life::GameClientSlaveManager* CreateSlaveManager(life::GameClientMasterTicker* pMaster,
		cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
		cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
		int slave_index, const PixelRect& render_area);

	UiCure::RendererImageResource* env_map_;

	logclass();
};



}
