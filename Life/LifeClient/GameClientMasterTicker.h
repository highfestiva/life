
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../cure/include/gameticker.h"
#include "../../lepra/include/thread.h"
#include "../../uicure/include/uilinegraph2d.h"
#include "../../uitbc/include/uifontmanager.h"
#include "../../uitbc/include/uirenderer.h"
#include "inputobserver.h"
#include "screenpart.h"



namespace cure {
class NetworkFreeAgent;
}
namespace UiCure {
class GameUiManager;
}



namespace life {



class ConsoleManager;
class GameClientSlaveManager;
class MasterServerConnection;
class RoadSignButton;
class UiGameServerManager;



class GameClientMasterTicker: public cure::GameTicker, public InputObserver, public ScreenPart {
	typedef cure::GameTicker Parent;
public:
	GameClientMasterTicker(UiCure::GameUiManager* ui_manager, cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity);
	virtual ~GameClientMasterTicker();

	virtual void AddBackedRtvar(const str& rtvar_name);
	virtual void Resume(bool hard);
	virtual void Suspend(bool hard);
	virtual void LoadRtvars(cure::RuntimeVariableScope* scope);
	virtual void SaveRtvars(cure::RuntimeVariableScope* scope);

	MasterServerConnection* GetMasterServerConnection() const;
	void SetMasterServerConnection(MasterServerConnection* connection);

	virtual bool CreateSlave() = 0;
	virtual void PrepareQuit();

	bool Tick();
	void PollRoundTrip();
	virtual void PreWaitPhysicsTick();

	bool StartResetUi();
	bool WaitResetUi();

	bool IsFirstSlave(const GameClientSlaveManager* slave) const;
	void GetSlaveInfo(const GameClientSlaveManager* slave, int& index, int& count) const;
	GameClientSlaveManager* GetSlave(int index) const;
	bool IsLocalObject(cure::GameObjectId instance_id) const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum(float fov);

	void PreLogin(const str& server_address);
	bool IsLocalServer() const;
	UiGameServerManager* GetLocalServer() const;
	void OnExit();
	void OnSetPlayerCount(int player_count);

	void DownloadServerList();

protected:
	typedef GameClientSlaveManager* (*SlaveFactoryMethod)(GameClientMasterTicker* pMaster,
		cure::TimeManager* time, cure::RuntimeVariableScope* variable_scope,
		cure::ResourceManager* resource_manager, UiCure::GameUiManager* ui_manager,
		int slave_index, const PixelRect& render_area);

	bool CreateSlave(SlaveFactoryMethod create);
	virtual void OnSlavesKilled() = 0;
	virtual void OnServerCreated(life::UiGameServerManager* server) = 0;

	cure::ContextObjectAttribute* CreateObjectAttribute(cure::ContextObject* object, const str& attribute_name);
	void AddSlave(GameClientSlaveManager* slave);
	void DeleteSlave(GameClientSlaveManager* slave, bool allow_main_menu);
	void DeleteServer();

	virtual bool Initialize();
	virtual bool Reinitialize();
	virtual bool OpenSlave(GameClientSlaveManager* slave);
	virtual bool OpenUiManager();
	void Repair();
	void UpdateSlaveLayout();
	void SlideSlaveLayout();
	int GetSlaveAnimationTarget(int slave_index) const;
	float GetSlavesVerticalAnimationTarget() const;
	void MeasureLoad();
	void Profile();
	virtual void PhysicsTick();
	virtual void WillMicroTick(float time_delta);
	virtual void DidPhysicsTick();
	virtual void BeginRender(vec3& color);
	void DrawDebugData() const;
	void DrawPerformanceLineGraph2d() const;

	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	virtual void OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity);

	int OnCommandLocal(const str& command, const strutil::strvec& parameter_vector);
	void OnCommandError(const str& command, const strutil::strvec& parameter_vector, int result);

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual void OnInput(uilepra::InputElement* element);

	virtual void CloseMainMenu() = 0;

	bool ApplyCalibration();
	void StashCalibration();

	typedef uilepra::TInputFunctor<GameClientMasterTicker> MasterInputFunctor;
	typedef std::vector<GameClientSlaveManager*> SlaveArray;

	Lock lock_;
	UiCure::GameUiManager* ui_manager_;
	cure::ResourceManager* resource_manager_;

	UiGameServerManager* server_;
	MasterServerConnection* master_connection_;
	cure::NetworkFreeAgent* free_network_agent_;

	strutil::strvec rtvars_;

	ConsoleManager* console_;
	bool restart_ui_;
	bool initialized_;
	unsigned active_width_;
	unsigned active_height_;
	SlaveArray slave_array_;
	int active_slave_count_;
	float slave_top_split_;
	float slave_bottom_split_;
	float slave_v_split_;
	float slave_fade_;
	int performance_adjustment_ticks_;
	uitbc::FontManager::FontId debug_font_id_;
	std::vector<UiCure::LineGraph2d> performance_graph_list_;
	std::unordered_set<cure::GameObjectId> local_object_set_;

	logclass();
};



}
