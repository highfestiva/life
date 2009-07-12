
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
	void OnTrigger(TBC::PhysicsEngine::BodyID pBody1, TBC::PhysicsEngine::BodyID pBody2);
	void OnForceApplied(TBC::PhysicsEngine::ForceFeedbackListener* pOtherObject,
		const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque);

	LOG_CLASS_DECLARE();
};



class CppContextObjectFactory: public ContextObjectFactory
{
public:
	CppContextObjectFactory();
	virtual ~CppContextObjectFactory();

protected:
	ContextObject* Create(const Lepra::String& pClassId) const;
	bool CreatePhysics(ContextObject* pObject) const;

private:
	LOG_CLASS_DECLARE();
};



}
