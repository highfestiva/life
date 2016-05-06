
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

//#include "../tbc/include/physicsengine.h"
#include "../life/lifeclient/gameclientslavemanager.h"
#include "../life/launcher.h"
#include "../lepra/include/gametimer.h"
#include "../uicure/include/uiresourcemanager.h"
#include "fire.h"
#include "version.h"



namespace UiCure {
class CollisionSoundManager;
class CppContextObject;
}
namespace uitbc {
class Button;
}
namespace life {
class GameClientMasterTicker;
class Menu;
}



namespace Fire {



class Autopilot;
class BaseMachine;
class Level;
class Sunlight;



class FireManager: public life::GameClientSlaveManager, private life::Launcher {
	typedef life::GameClientSlaveManager Parent;
public:
	FireManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~FireManager();
	virtual void Suspend(bool hard);
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& render_area);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount);

	PixelRect GetRenderableArea() const;
	virtual bool Render();
	virtual bool Paint();
	void PrintTime(const str prefix, double time, bool is_sloppy, int x, int y, const Color c, const Color bg);
	virtual void DrawSyncDebugInfo();

	virtual bool IsObjectRelevant(const vec3& position, float distance) const;

	virtual void Shoot(cure::ContextObject* avatar, int weapon);
	virtual void Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength);
	virtual void OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object);
	void OnLetThroughTerrorist(BaseMachine* terrorist);

	virtual bool DidFinishLevel();
	virtual str StepLevel(int count);
	str StoreLevelIndex(int level_number);

	virtual Level* GetLevel() const;
	virtual int GetCurrentLevelNumber() const;

	cure::RuntimeVariableScope* GetVariableScope() const;

	struct TargetInfo {
		TargetInfo(str villain, PixelCoord _xy, float dangerousness, float scale):
			villain_(villain),
			xy(_xy),
			dangerousness_(dangerousness),
			is_active_(true),
			time_(0),
			scale_(scale) {
		}
		str villain_;
		PixelCoord xy;
		float dangerousness_;
		bool is_active_;
		float time_;
		float scale_;
	};
	typedef std::unordered_map<void*, TargetInfo> VillainMap;
	typedef std::pair<void*, TargetInfo> VillainPair;
	typedef std::vector<VillainPair> VillainArray;

	virtual bool InitializeUniverse();
	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera();
	virtual void UpdateCameraPosition(bool update_mic_position);
	virtual void HandleShooting();
	virtual void HandleTargets(float time);

	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool render);

	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	virtual cure::ContextObject* CreateLogicHandler(const str& type);
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	virtual void OnLevelLoadCompleted();
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);

	void OnBombButton(uitbc::Button*);
	void OnPauseButton(uitbc::Button* button);
	void CreateNextLevelDialog();
	void OnMenuAlternative(uitbc::Button* button);

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);

	bool DisableAmbient();
	void EnableAmbient();

	UiCure::CollisionSoundManager* collision_sound_manager_;
	life::Menu* menu_;
	Level* level_;
	bool stepped_level_;
	Sunlight* sunlight_;
	xform camera_transform_;
	uitbc::Button* pause_button_;
	uitbc::Button* bomb_button_;
	vec3 shoot_direction_;
	StopWatch all_loaded_timer_;
	StopWatch slowmo_timer_;
	GameTimer fire_delay_timer_;
	vec3 store_ambient_;
	bool store_lights_enabled_;
	int kills_;
	int kill_limit_;
	int level_total_kills_;
	VillainMap villain_map_;

	logclass();
};



}
