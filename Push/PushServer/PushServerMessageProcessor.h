
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Life/LifeServer/ServerMessageProcessor.h"
#include "../Push.h"



namespace Push
{



class PushServerMessageProcessor: public Life::ServerMessageProcessor
{
	typedef Life::ServerMessageProcessor Parent;
public:
	PushServerMessageProcessor(Life::GameServerManager* pGameServerManager);
	virtual ~PushServerMessageProcessor();

private:
	virtual void ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);

	LOG_CLASS_DECLARE();
};



}
