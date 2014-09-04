
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Cure/Include/GameTicker.h"
#include "../../Lepra/Include/LogListener.h"
#include "../Life.h"



namespace Life
{



class GameServerManager;
class MasterServerConnection;
struct ServerInfo;



class GameServerTicker: public Cure::GameTicker
{
	typedef Cure::GameTicker Parent;
public:
	GameServerTicker(Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~GameServerTicker();
	virtual void StartConsole(InteractiveConsoleLogListener* pConsoleLogger);
	void SetMasterServerConnection(MasterServerConnection* pConnection);

protected:
	virtual bool Initialize();
	virtual bool Tick();
	virtual void PollRoundTrip();
	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

	virtual void WillMicroTick(float pTimeDelta);
	virtual void DidPhysicsTick();

	virtual void OnTrigger(Tbc::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherObjectId, Tbc::PhysicsManager::BodyID pBodyId, const vec3& pNormal);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, Tbc::PhysicsManager::BodyID pBodyId, Tbc::PhysicsManager::BodyID pOtherBodyId,
		const vec3& pForce, const vec3& pTorque, const vec3& pPosition, const vec3& pRelativeVelocity);

	Cure::ResourceManager* mResourceManager;
	GameServerManager* mGameManager;
	MasterServerConnection* mMasterConnection;

	logclass();
};



}
