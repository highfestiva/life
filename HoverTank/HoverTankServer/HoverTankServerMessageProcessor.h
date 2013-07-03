
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Life/LifeServer/ServerMessageProcessor.h"
#include "../HoverTank.h"



namespace HoverTank
{



class GameServerLogic;



class HoverTankServerMessageProcessor: public Life::ServerMessageProcessor
{
	typedef Life::ServerMessageProcessor Parent;
public:
	HoverTankServerMessageProcessor(Life::GameServerManager* pGameServerManager, GameServerLogic* pLogic);
	virtual ~HoverTankServerMessageProcessor();

private:
	virtual void ProcessNumber(Life::Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);

	GameServerLogic* mLogic;

	LOG_CLASS_DECLARE();
};



}
