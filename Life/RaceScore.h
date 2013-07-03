
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Cure/Include/ContextObjectAttribute.h"
#include "../Lepra/Include/HiResTimer.h"
#include "../TBC/Include/PhysicsManager.h"
#include "Life.h"



namespace Life
{



class RaceScore: public Cure::ContextObjectAttribute
{
	typedef Cure::ContextObjectAttribute Parent;
public:
	typedef TBC::PhysicsManager::TriggerID TriggerId;

	RaceScore(Cure::ContextObject* pContextObject, const str& pName, int pLapCount, TBC::PhysicsManager::TriggerID pStartTrigger);
	virtual ~RaceScore();

	TriggerId GetStartTrigger() const;
	bool IsTriggered(TriggerId pTriggerId) const;
	void AddTriggered(TriggerId pTriggerId);
	unsigned GetTriggedCount() const;
	int StartNewLap();
	double GetTime() const;

private:
	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* pDestination);
	virtual int Unpack(const uint8* pSource, int pMaxSize);	// Retuns number of bytes unpacked, or -1.

	typedef std::hash_set<TBC::PhysicsManager::TriggerID> TriggerSet;

	bool mIsUpdated;
	int mLapCountLeft;
	int mPhysicsFrameStart;
	TBC::PhysicsManager::TriggerID mStartTrigger;
	TriggerSet mTriggerSet;
	size_t mTriggedCount;
	size_t mTriggerCount;

	void operator=(const RaceScore&);
};



}
