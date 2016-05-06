
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/gameclientmasterticker.h"
#include "hovertank.h"
#include "sunlight.h"



namespace life {
class GameClientSlaveManager;
}



namespace HoverTank {



class Sunlight;



class HoverTankTicker: public life::GameClientMasterTicker {
	typedef life::GameClientMasterTicker Parent;
public:
	HoverTankTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager_, float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~HoverTankTicker();

	Sunlight* GetSunlight() const;

private:
	virtual bool CreateSlave();
	virtual void OnSlavesKilled();
	virtual void OnServerCreated(life::UiGameServerManager* server);

	virtual bool Reinitialize();
	virtual bool OpenUiManager();
	void DisplaySplashLogo();
	void DisplayCompanyLogo();

	virtual void BeginRender(vec3& color);

	virtual void CloseMainMenu();
	virtual bool QueryQuit();

	static life::GameClientSlaveManager* CreateSlaveManager(life::GameClientMasterTicker* pMaster,
		cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
		cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
		int slave_index, const PixelRect& render_area);
	static life::GameClientSlaveManager* CreateViewer(life::GameClientMasterTicker* pMaster,
		cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
		cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
		int slave_index, const PixelRect& render_area);

	bool is_player_count_view_active_;
	Sunlight* sunlight_;		// TODO: remove hack and come up with something better?

	logclass();
};



}
