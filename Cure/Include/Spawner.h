
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "cppcontextobject.h"
#include "../../lepra/include/hirestimer.h"
#include "cure.h"



namespace cure {



class Spawner: public CppContextObject {
	typedef CppContextObject Parent;
public:
	Spawner(ContextManager* manager);
	virtual ~Spawner();

	void PlaceObject(ContextObject* object, int spawn_point_index);
	xform GetSpawnPoint() const;
	static void EaseDown(tbc::PhysicsManager* physics_manager, ContextObject* object, const vec3* start_position);

protected:
	virtual void OnTick();
	virtual void OnAlarm(int alarm_id, void* extra_data);
	void OnCreate(float create_interval, bool has_recreate);
	void OnDestroy(float destroy_interval);
	void OnRecreate(float recreate_interval);
	void Create();
	virtual int GetSpawnCount() const;

private:
	typedef std::vector<GameObjectId> GameObjectIdArray;

	StopWatch recreate_timer_;
	int spawn_point_index_;
	GameObjectIdArray ease_down_objects_;

	logclass();
};



}
