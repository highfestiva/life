
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once



#include "../../Cure/Include/GameManager.h"
/*#include "../../Cure/Include/NetworkServer.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/UserAccountManager.h"*/
#include "../../Lepra/Include/LogListener.h"
#include "../Life.h"
//#include "../GameManager.h"
//#include "Client.h"



namespace Life
{



class GameServerManager;



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

	LOG_CLASS_DECLARE();
};



}
