
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Life/LifeServer/ServerMessageProcessor.h"
#include "../Push.h"



namespace Push
{



class GameServerLogic;



class PushServerMessageProcessor: public Life::ServerMessageProcessor
{
	typedef Life::ServerMessageProcessor Parent;
public:
	PushServerMessageProcessor(Life::GameServerManager* pGameServerManager, GameServerLogic* pLogic);
	virtual ~PushServerMessageProcessor();

private:
	virtual void ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);

	GameServerLogic* mLogic;

	LOG_CLASS_DECLARE();
};



}
