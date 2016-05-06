
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/diskfile.h"
#include "../lepra/include/gametimer.h"
#include "../tbc/include/physicsengine.h"
#include "../life/lifeclient/gameclientslavemanager.h"
#include "../life/launcher.h"
#include "../uicure/include/uiresourcemanager.h"
#include "downwash.h"
#include "version.h"



namespace cure {
class HiscoreAgent;
}
namespace uilepra {
namespace touch {
class TouchstickInputDevice;
}
}
namespace UiCure {
class CollisionSoundManager;
class CppContextObject;
}
namespace uitbc {
class BillboardGeometry;
class Button;
}
namespace life {
class GameClientMasterTicker;
class Level;
class Menu;
}



namespace Downwash {



class Autopilot;
class Level;
class Sunlight;



class DownwashManager: public life::GameClientSlaveManager, private life::Launcher {
	typedef life::GameClientSlaveManager Parent;
public:
	DownwashManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~DownwashManager();
	virtual void Suspend(bool hard);
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& render_area);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount);

	virtual bool Render();
	virtual bool Paint();
	void PrintTime(const wstr prefix, double time, bool is_sloppy, int x, int y, const Color c, const Color bg);
	virtual void DrawSyncDebugInfo();

	virtual bool IsObjectRelevant(const vec3& position, float distance) const;
	cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned aspect, float power);

	virtual void Shoot(cure::ContextObject* canon, int ammo);
	virtual void Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength);
	virtual void OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object);

	virtual bool DidFinishLevel();
	virtual str StepLevel(int count);

	virtual Level* GetLevel() const;
	virtual int GetCurrentLevelNumber() const;
	virtual double GetCurrentLevelBestTime(bool world) const;
	virtual void SetLevelBestTime(int level_index, bool world, double time);
	virtual cure::ContextObject* GetAvatar() const;

	cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	typedef uilepra::touch::TouchstickInputDevice Touchstick;

	virtual bool InitializeUniverse();
	void CreateChopper(const str& class_id);
	void UpdateChopperColor(float lerp);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera();
	virtual void TickInput();

	void UpdateCameraDistance();
	void UpdateTouchstickPlacement();
	int GetControlMode() const;
	void UpdateControlMode();
	virtual void TickUiInput();
	bool SetAvatarEnginePower(cure::ContextObject* avatar, unsigned aspect, float power);
	virtual void TickUiUpdate();
	virtual bool UpdateMassObjects(const vec3& position);
	virtual void SetLocalRender(bool render);
	void SetMassRender(bool render);

	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	virtual cure::ContextObject* CreateLogicHandler(const str& type);
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	virtual void OnLevelLoadCompleted();
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);

	vec3 GetLandingTriggerPosition(cure::ContextObject* level) const;
	void EaseDown(cure::ContextObject* object, const vec3* start_position);
	xform GetMainRotorTransform(const UiCure::CppContextObject* chopper) const;

	void OnPauseButton(uitbc::Button*);
	void OnLastHiscoreButton(uitbc::Button*);
	void ShowHiscoreDialog(int direction);
	void UpdateHiscoreDialog();
	void OnMenuAlternative(uitbc::Button* button);
	void OnPreHiscoreAction(uitbc::Button* button);

	void UpdateHiscoreDialogTitle();
	wstr GetHiscoreLevelTitle() const;

	void CreateHiscoreAgent();
	void TickHiscore();

	void DrawStick(Touchstick* stick);

	virtual void UpdateCameraPosition(bool update_mic_position);

	void DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);
	void RendererTextureLoadCallback(UiCure::UserRendererImageResource* resource);

	bool DisableDepth();
	void EnableDepth();

	UiCure::CollisionSoundManager* collision_sound_manager_;
	life::Menu* menu_;

	// Network transmission and keepalive info.
	cure::GameObjectId avatar_id_;
	StopWatch all_loaded_timer_;
	StopWatch too_far_away_timer_;
	StopWatch avatar_create_timer_;
	StopWatch avatar_died_;
	GameTimer fly_time_;
	StopWatch slowmo_timer_;
	HiResTimer toy_mode_color_timer_;
	vec3 last_vehicle_color_;
	vec3 last_chopper_color_;
	bool set_random_chopper_color_;
	bool had_avatar_;
	bool update_camera_for_avatar_;
	life::options::Steering last_steering_;
	int active_weapon_;

	Level* level_;
	Level* old_level_;
	Autopilot* autopilot_;
	UiCure::CppContextObject* hemisphere_;
	tbc::BoneAnimator* hemisphere_uv_transform_;
	bool render_hemisphere_;
	Sunlight* sunlight_;
	ObjectArray mass_object_array_;

	xform camera_transform_;
	vec3 camera_previous_position_;
	vec3 helicopter_position_;
	mutable vec3 last_landing_trigger_position_;
	float camera_speed_;
	bool zoom_platform_;
	int post_zoom_platform_frame_count_;
	int hit_ground_frame_count_;
	bool is_hit_this_frame_;
	bool level_completed_;
	vec3 microphone_speed_;
	uitbc::Button* pause_button_;
	uitbc::Button* last_hiscore_button_;

	HiResTimer touchstick_timer_;
	Touchstick* stick_;

	UiCure::UserPainterKeepImageResource* wrong_direction_image_;
	UiCure::UserPainterKeepImageResource* win_image_;
	UiCure::UserPainterKeepImageResource* check_icon_;
	UiCure::UserPainterKeepImageResource* lock_icon_;
	StopWatch direction_image_timer_;
	StopWatch win_image_timer_;
	UiCure::UserRendererImageResource* arrow_;
	uitbc::BillboardGeometry* arrow_billboard_;
	uitbc::Renderer::GeometryID arrow_billboard_id_;
	float arrow_total_power_;
	float arrow_angle_;
	int slow_system_counter_;

	int hiscore_level_index_;
	int my_hiscore_index_;
	cure::HiscoreAgent* hiscore_agent_;
	StopWatch hiscore_just_uploaded_timer_;

	logclass();
};



}
