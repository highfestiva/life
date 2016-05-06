
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/gamemanager.h"
#include "../cure/include/gameticker.h"
#include "../uitbc/include/uirenderer.h"



#define kFps	45



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



namespace bounce {



using namespace lepra;
class Ball;
class Racket;



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
public:
	Game(UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager);
	virtual ~Game();
	UiCure::GameUiManager* GetUiManager() const;
	virtual bool Tick();

	void SetRacketForce(float lift_factor, const vec3& down);
	bool MoveRacket();
	Racket* GetRacket() const;
	Ball* GetBall() const;
	void ResetScore();
	double GetScore() const;

	bool Render();
	bool Paint();

private:
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
	Racket* racket_;
	Ball* ball_;
	float racket_lift_factor_;
	vec3 racket_down_direction_;
	double score_;

	logclass();
};



}
