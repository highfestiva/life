
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/gamemanager.h"
#include "../cure/include/gameticker.h"
#include "../uitbc/include/uirenderer.h"



#ifdef LEPRA_TOUCH
#define LEPRA_TOUCH_LOOKANDFEEL
#endif // iOS
#define LEPRA_TOUCH_LOOKANDFEEL
#define kFps			20
#define SCALE_FACTOR		1.0f
#define HEALTH_ROUND_FACTOR	0.8f



namespace cure {
class ContextObject;
class ResourceManager;
class RuntimeVariableScope;
}

namespace UiCure {
class CollisionSoundManager;
class CppContextObject;
class GameUiManager;
}



namespace tirefire {



using namespace lepra;
class Goal;
class Vehicle;
class Level;
class VehicleAi;



struct FingerMovement {
	int start_x_;
	int start_y_;
	int last_x_;
	int last_y_;
	int delta_x_;
	int delta_y_;
	int moved_distance_;
	bool is_press_;
	int tag_;
	HiResTimer timer_;

	inline FingerMovement(int x, int y):
		start_x_(x),
		start_y_(y),
		last_x_(x),
		last_y_(y),
		delta_x_(0),
		delta_y_(0),
		moved_distance_(0),
		is_press_(true),
		tag_(0),
		timer_(false) {
	}

	inline bool Update(int last_x, int last_y, int new_x, int new_y) {
		if (std::abs(last_x_-last_x) < 44 && std::abs(last_y_-last_y) < 44) {
			delta_x_ += std::abs(last_x_-new_x);
			delta_y_ += std::abs(last_y_-new_y);
			last_x_ = new_x;
			last_y_ = new_y;
			return true;
		}
		return false;
	}

	inline void UpdateDistance() {
		moved_distance_ = delta_x_ + delta_y_;
	}
};
typedef std::list<FingerMovement> FingerMoveList;



class Game: public cure::GameTicker, public cure::GameManager {
	typedef cure::GameTicker GameTicker;
	typedef cure::GameManager GameManager;
public:
	enum FlybyMode {
		kFlybyInactive = 1,
		kFlybyIntroduction,
		kFlybyIntroductionFinishingUp,
		kFlybyPause,
	};

	Game(UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager);
	virtual ~Game();
	UiCure::GameUiManager* GetUiManager() const;
	const str& GetLevelName() const;
	bool SetLevelName(const str& level);
	bool RestartLevel();
	xform GetVehicleStart() const;
	virtual bool Tick();
	void TickFlyby();

	str GetVehicleName() const;
	void SetVehicleName(const str& vehicle);
	Level* GetLevel() const;
	Vehicle* GetVehicle() const;
	Goal* GetGoal() const;
	void GetVehicleMotion(vec3& position, vec3 velocity) const;
	void SetThrottle(float throttle);
	FlybyMode GetFlybyMode() const;
	void SetFlybyMode(FlybyMode flyby_mode);
	void ResetScore();
	void AddScore(double score);
	double GetScore() const;
	void EnableScoreCounting(bool enable);
	bool IsScoreCountingEnabled() const;
	void Detonate(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* explosive, cure::ContextObject* target, tbc::PhysicsManager::BodyID explosive_body_id, tbc::PhysicsManager::BodyID target_body_id);
	void OnCapture();	// CTF
	int GetHeartBalance() const;	// 0 = Heart is 2 love, -1 = P1 leads with 1 point, +2 P2 leads with 2 points...
	void SetHeartBalance(int balance);
	void FlipRenderSides();
	bool IsFlipRenderSide() const;
	void NextRound();
	int GetRoundIndex() const;
	void EndSlowmo();

	bool Render();
	bool Paint();

	//void MoveTo(const FingerMovement& move);

private:
	bool FlybyRender();

	virtual void PollRoundTrip();	// Polls network for any incoming to yield lower latency.
	virtual float GetTickTimeReduction() const;	// Returns how much quicker the tick loop should be; can be negative.
	virtual float GetPowerSaveAmount() const;

	virtual void WillMicroTick(float time_delta);
	virtual void DidPhysicsTick();
	virtual void OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal);
	virtual void OnForceApplied(int object_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, tbc::PhysicsManager::BodyID other_body_id,
		const vec3& force, const vec3& torque, const vec3& position, const vec3& relative_velocity);

	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	virtual void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id);
	virtual bool OnPhysicsSend(cure::ContextObject* object);
	virtual bool OnAttributeSend(cure::ContextObject* object);
	virtual bool IsServer();
	virtual void SendAttach(cure::ContextObject* object1, unsigned id1, cure::ContextObject* object2, unsigned id2);
	virtual void SendDetach(cure::ContextObject* object1, cure::ContextObject* object2);
	virtual void TickInput();
	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;
	virtual bool Initialize();
	virtual bool InitializeUniverse();

	cure::ContextObject* CreateLogicHandler(const str& type);

	UiCure::GameUiManager* ui_manager_;
	UiCure::CollisionSoundManager* collision_sound_manager_;
	uitbc::Renderer::LightID light_id_;
	Level* level_;
	str level_name_;
	FlybyMode flyby_mode_;
	double fly_by_time_;
	Vehicle* vehicle_;
	vec3 vehicle_cam_pos_;

	mutable Goal* goal_;
	VehicleAi* vehicle_ai_;
	int flip_render_side_;
	float flip_render_side_factor_;
	double score_;
	bool score_counting_enabled_;
	StopWatch slowmo_timer_;
};



}
