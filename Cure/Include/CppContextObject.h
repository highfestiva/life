
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games

#pragma once

#include "ContextObject.h"



namespace Cure
{


class GameManager;



class CppContextObject: public ContextObject
{
public:
	CppContextObject(const Lepra::String& pClassId);
	virtual ~CppContextObject();

private:
	void OnTick(float pFrameTime);
	void OnAlarm(int pAlarmId);
	void OnBodyInside(TBC::PhysicsEngine::BodyID pBody);
	void OnForceApplied(TBC::PhysicsEngine::ForceFeedbackListener* pOtherObject,
		const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque);
};



class CppContextObjectFactory: public ContextObjectFactory
{
public:
	CppContextObjectFactory(unsigned pPhysicsFps);
	virtual ~CppContextObjectFactory();

protected:
	ContextObject* Create(const Lepra::String& pClassId) const;
	bool CreatePhysics(ContextObject* pObject, ContextObject* pTriggerListener) const;

private:
	unsigned mPhysicsFps;

	LOG_CLASS_DECLARE();
};



}
