
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../cure/include/contextobjectattribute.h"
#include "../lepra/include/hirestimer.h"
#include "../tbc/include/physicsmanager.h"
#include "life.h"



namespace life {



class RaceScore: public cure::ContextObjectAttribute {
	typedef cure::ContextObjectAttribute Parent;
public:
	typedef tbc::PhysicsManager::BodyID TriggerId;

	RaceScore(cure::ContextObject* context_object, const str& name, int lap_count, tbc::PhysicsManager::BodyID start_trigger);
	virtual ~RaceScore();

	TriggerId GetStartTrigger() const;
	bool IsTriggered(TriggerId trigger_id) const;
	void AddTriggered(TriggerId trigger_id);
	unsigned GetTriggedCount() const;
	int StartNewLap();
	double GetTime() const;

private:
	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* destination);
	virtual int Unpack(const uint8* source, int max_size);	// Retuns number of bytes unpacked, or -1.

	typedef std::unordered_set<tbc::PhysicsManager::BodyID> TriggerSet;

	bool is_updated_;
	int lap_count_left_;
	int physics_frame_start_;
	tbc::PhysicsManager::BodyID start_trigger_;
	TriggerSet trigger_set_;
	size_t trigged_count_;
	size_t trigger_count_;

	void operator=(const RaceScore&);
};



}
