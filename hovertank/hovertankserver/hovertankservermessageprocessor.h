
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../life/lifeserver/servermessageprocessor.h"
#include "../hovertank.h"



namespace HoverTank {



class GameServerLogic;



class HoverTankServerMessageProcessor: public life::ServerMessageProcessor {
	typedef life::ServerMessageProcessor Parent;
public:
	HoverTankServerMessageProcessor(life::GameServerManager* game_server_manager, GameServerLogic* logic);
	virtual ~HoverTankServerMessageProcessor();

private:
	virtual void ProcessNumber(life::Client* client, cure::MessageNumber::InfoType type, int32 integer, float32 f);

	GameServerLogic* logic_;

	logclass();
};



}
