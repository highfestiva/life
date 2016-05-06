
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../lepra/include/diskfile.h"
#include "../tbc/include/physicsengine.h"
#include "../life/lifeclient/gameclientslavemanager.h"
#include "../life/launcher.h"
#include "loginview.h"
#include "hovertank.h"
#include "version.h"



namespace uilepra {
namespace touch {
class TouchstickInputDevice;
}
}
namespace UiCure {
class CollisionSoundManager;
}
namespace life {
class GameClientMasterTicker;
class Level;
}



namespace HoverTank {



class RoadSignButton;



class HoverTankManager: public life::GameClientSlaveManager, private ClientLoginObserver, private life::Launcher {
	typedef life::GameClientSlaveManager Parent;
public:
	HoverTankManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~HoverTankManager();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& render_area);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount);

	virtual bool Paint();

	virtual void RequestLogin(const str& server_address, const cure::LoginId& login_token);
	virtual void OnLoginSuccess();

	void SelectAvatar(const cure::UserAccount::AvatarId& avatar_id);
	void AddLocalObjects(std::unordered_set<cure::GameObjectId>& local_object_set);
	virtual bool IsObjectRelevant(const vec3& position, float distance) const;
	cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned aspect, float power);

	virtual void Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength);
	virtual void OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object);

protected:
	typedef std::unordered_map<cure::GameObjectId, RoadSignButton*> RoadSignMap;
	typedef uilepra::touch::TouchstickInputDevice Touchstick;

	cure::RuntimeVariableScope* GetVariableScope() const;

	virtual bool Reset();
	virtual void CreateLoginView();
	virtual bool InitializeUniverse();
	void CloseLoginGui();
	void ClearRoadSigns();
	void SetRoadSignsVisible(bool visible);

	virtual void ScriptPhysicsTick();
	virtual void MoveCamera();
	virtual void TickInput();

	void UpdateTouchstickPlacement();
	virtual void TickUiInput();
	bool SetAvatarEnginePower(cure::ContextObject* avatar, unsigned aspect, float power);
	virtual void TickUiUpdate();
	virtual bool UpdateMassObjects(const vec3& position);
	virtual void SetLocalRender(bool render);
	void SetMassRender(bool render);

	virtual void ProcessNetworkInputMessage(cure::Message* message);
	virtual void ProcessNetworkStatusMessage(cure::MessageStatus* message);
	virtual void ProcessNumber(cure::MessageNumber::InfoType type, int32 integer, float32 f);
	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);

	void OnFireButton(uitbc::Button*);
	void AvatarShoot();
	void Shoot(cure::ContextObject* avatar, int weapon);

	void CancelLogin();
	void OnVehicleSelect(uitbc::Button* button);
	void OnAvatarSelect(uitbc::Button* button);
	void DropAvatar();

	void DrawStick(Touchstick* stick);
	void DrawScore();

	virtual void UpdateCameraPosition(bool update_mic_position);
	quat GetCameraQuaternion() const;

	UiCure::CollisionSoundManager* collision_sound_manager_;

	// Network transmission and keepalive info.
	cure::GameObjectId avatar_id_;
	bool had_avatar_;
	bool update_camera_for_avatar_;
	life::options::Steering last_steering_;
	float cam_rotate_extra_;
	int active_weapon_;
	HiResTimer fire_timeout_;

	RoadSignButton* pick_vehicle_button_;
	int avatar_invisible_count_;
	int road_sign_index_;
	RoadSignMap road_sign_map_;

	cure::GameObjectId level_id_;
	life::Level* level_;
	ObjectArray mass_object_array_;
	cure::ContextObject* sun_;
	std::vector<cure::ContextObject*> cloud_array_;

	cure::GameObjectId score_info_id_;

	vec3 camera_position_;
	vec3 camera_previous_position_;
	vec3 camera_up_;
	vec3 camera_orientation_;
	vec3 camera_pivot_position_;
	vec3 camera_pivot_velocity_;
	float camera_target_xy_distance_;
	float camera_max_speed_;
	float camera_mouse_angle_;
	HiResTimer camera_mouse_angle_timer_;
	float camera_target_angle_;
	float camera_target_angle_factor_;
	vec3 microphone_speed_;
	uitbc::Window* login_window_;
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	uitbc::Button* fire_button_;
#endif // touch or emulated touch.

	HiResTimer touchstick_timer_;
	Touchstick* stick_left_;
	Touchstick* stick_right_;

	struct EnginePower {	// Used for recording vechile steering playback.
		float power_;
	};
	DiskFile engine_playback_file_;	// Used for recording vechile steering playback.
	float engine_playback_time_;	// Used for recording vechile steering playback.
	EnginePower engine_power_shadow_[tbc::PhysicsEngine::kAspectCount];	// Used for recording vechile steering playback.

	logclass();
};



}
