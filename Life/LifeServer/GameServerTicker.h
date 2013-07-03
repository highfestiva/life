
// Author: Jonas Bystr�m
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

	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	Cure::ResourceManager* mResourceManager;
	GameServerManager* mGameManager;
	MasterServerConnection* mMasterConnection;

	LOG_CLASS_DECLARE();
};



}
