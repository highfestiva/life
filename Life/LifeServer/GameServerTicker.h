
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/LogListener.h"
#include "../Life.h"



namespace Life
{



class GameServerManager;
class MasterServerConnection;
struct ServerInfo;



class GameServerTicker: public Cure::GameTicker
{
public:
	GameServerTicker(Cure::ResourceManager* pResourceManager,
		InteractiveConsoleLogListener* pConsoleLogger);
	virtual ~GameServerTicker();

private:
	virtual bool Initialize();
	virtual bool Tick();
	virtual void PollRoundTrip();
	virtual float GetPowerSaveAmount() const;

	Cure::ResourceManager* mResourceManager;
	GameServerManager* mGameManager;
	MasterServerConnection* mMasterConnection;

	LOG_CLASS_DECLARE();
};



}
