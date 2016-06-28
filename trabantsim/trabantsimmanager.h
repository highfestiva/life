
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../life/lifeclient/gameclientslavemanager.h"
#include <map>
#include "../lepra/include/socketaddress.h"
#include "../life/lifeclient/menu.h"
#include "../uicure/include/uiresourcemanager.h"
#include "../uilepra/include/uitouchdrag.h"
#include "physgfxobject.h"
#include "trabantsim.h"



namespace lepra {
class UdpSocket;
}
namespace uilepra {
namespace touch {
class TouchstickInputDevice;
}
}
namespace UiCure {
class CollisionSoundManager;
class CppContextObject;
class SoundReleaser;
}
namespace uitbc {
class Button;
}


namespace TrabantSim {



class FileServer;
class Light;
class Object;



class TrabantSimManager: public life::GameClientSlaveManager {
	typedef life::GameClientSlaveManager Parent;
public:
	typedef uilepra::touch::TouchstickInputDevice Touchstick;
	struct CollisionInfo {
		int object_id_;
		vec3 force_;
		vec3 position_;
		int other_object_id_;
	};
	struct JoystickData {
		int joystick_id_;
		float x;
		float y;
		JoystickData(int joystick_id, float px, float py);
	};
	struct TouchstickInfo {
		Touchstick* stick_;
		float x;
		float y;
		int orientation_;
		bool is_sloppy_;
		TouchstickInfo(Touchstick* stick, float px, float py, int orientation, bool is_sloppy);
	};
	struct EngineTarget {
		int instance_id_;
		float strength_;
		EngineTarget(int instance_id, float strength);
	};
	struct Drag {
		PixelCoord start_;
		PixelCoord last_;
		vec2 velocity_;
		HiResTimer timer_;
		bool is_press_;
		int button_mask_;
	};
	typedef std::list<CollisionInfo> CollisionList;
	typedef std::vector<Drag> DragList;
	typedef std::vector<PixelCoord> DragEraseList;
	typedef uilepra::touch::DragManager::DragList UiDragList;
	typedef std::vector<JoystickData> JoystickDataList;
	typedef std::vector<TouchstickInfo> TouchstickList;
	typedef std::vector<EngineTarget> EngineTargetList;
	typedef std::vector<float> FloatList;
	typedef std::vector<int> IntList;
	typedef std::vector<str> StringList;
	typedef std::vector<vec3> Vec3List;
	typedef std::vector<xform> XformList;

	TrabantSimManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~TrabantSimManager();

	virtual void Resume(bool hard);
	virtual void Suspend(bool hard);
	virtual void LoadSettings();
	virtual void RefreshOptions();

	void UserReset();
	int CreateObject(const quat& orientation, const vec3& position, const MeshObject& gfx_object, const PhysObjectArray& phys_objects, ObjectMaterial material, bool is_static, bool is_trigger);
	void CreateClones(IntList& created_object_ids, int original_id, const XformList& placements, ObjectMaterial material, bool is_static);
	void DeleteObject(int object_id);
	void DeleteAllObjects();
	void PickObjects(const vec3& position, const vec3& direction, const vec2& range, IntList& picked_object_ids, Vec3List& picked_positions);
	bool IsLoaded(int object_id);
	void Explode(const vec3& pos, const vec3& vel, float strength, float volume);
	void PlaySound(const str& sound, const vec3& pos, const vec3& vel, float volume);
	void PopCollisions(CollisionList& collision_list);
	void GetKeys(strutil::strvec& keys);
	void GetTouchDrags(DragList& drag_list);
	vec3 GetAccelerometer() const;
	vec3 GetMouseMove();
	int CreateJoystick(float x, float y, bool is_sloppy);
	JoystickDataList GetJoystickData() const;
	float GetAspectRatio() const;
	int CreateEngine(int object_id, const str& engine_type, const vec2& max_velocity, float strength, float friction, const EngineTargetList& engine_targets);
	int CreateJoint(int object_id, const str& joint_type, int other_object_id, const vec3& axis, const vec2& stop, const vec2& spring_settings);
	void Position(int object_id, bool _set, vec3& position);
	void Orientation(int object_id, bool _set, quat& orientation);
	void Velocity(int object_id, bool _set, vec3& velocity);
	void AngularVelocity(int object_id, bool _set, vec3& angular_velocity);
	void Force(int object_id, bool _set, vec3& force);
	void Torque(int object_id, bool _set, vec3& torque);
	void Mass(int object_id, bool _set, float& mass);
	void ObjectColor(int object_id, bool _set, vec3& color, float alpha);
	void EngineForce(int object_id, int engine_index, bool _set, vec3& force);
	void AddTag(int object_id, const str& tag_type, const FloatList& floats, const StringList& strings, const IntList& phys, const IntList& engines, const IntList& meshes);

	void CommandLoop();
	bool IsControlled();

	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& render_area);
	virtual bool Open();
	virtual void Close();
	bool OpenConnection();
	void CloseConnection();
	virtual void SetIsQuitting();
	virtual void SetFade(float fade_amount);

	virtual bool Paint();
	void DrawStick(Touchstick* stick, bool is_sloppy);

protected:
	typedef std::map<uilepra::InputManager::KeyCode,bool> KeyMap;

	virtual bool InitializeUniverse();
	virtual void TickInput();
	void UpdateTouchstickPlacement();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool render);

	void AddCheckerTexturing(uitbc::TriangleBasedGeometry* mesh, float scale);

	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);
	void PushCollision(cure::GameObjectId object_id1, const vec3& force, const vec3& position, cure::GameObjectId object_id2);

	void OnPauseButton(uitbc::Button*);
	void OnBackButton(uitbc::Button*);
	void OnMenuAlternative(uitbc::Button* button);

	virtual void ScriptPhysicsTick();
	virtual void MoveCamera(float frame_time);
	virtual void UpdateCameraPosition(bool update_mic_position);
	virtual void UpdateUserMessage();

	void PrintText(const wstr& s, int x, int y) const;
	void DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const;

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual void OnInput(uilepra::InputElement* element);
	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource);

	UiCure::CollisionSoundManager* collision_sound_manager_;
	std::set<cure::GameObjectId> objects_;
	CollisionList collision_list_;
	KeyMap key_map_;
	DragList drag_list_;
	DragEraseList drag_erase_list_;
	bool is_mouse_controlled_;
	bool set_focus_;
	bool set_cursor_visible_;
	bool set_cursor_invisible_;
	vec3 mouse_move_;
	HiResTimer touchstick_timer_;
	TouchstickList touchstick_list_;
	life::Menu* menu_;
	Light* light_;
	vec3 camera_angle_;
	xform camera_transform_;
	vec3 camera_velocity_;
	uitbc::Button* pause_button_;
	uitbc::Button* back_button_;
	double last_sound_volume_;
	double master_volume_;
	bool is_paused_;
	bool is_controlled_;
	bool was_controlled_;
	int hide_counter_;
	bool is_control_timeout_;
	HiResTimer startup_timer_;
	HiResTimer resend_time_of_last_packet_;
	StopWatch hide_watch_;
	double resend_intermediate_packet_time_;
	str resend_last_response_;
	SocketAddress open_local_address_;
	SocketAddress internal_local_address_;
	SocketAddress local_address_;
	SocketAddress last_remote_address_;
	SocketAddress last_accepted_address_;
	UdpSocket* command_socket_;
	MemberThread<TrabantSimManager>* command_thread_;
	FileServer* file_server_;
	uitbc::Dialog* user_info_dialog_;
	uitbc::Label* user_info_label_;
	logclass();
};



}
