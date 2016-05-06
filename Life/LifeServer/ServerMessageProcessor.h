
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../cure/include/contextmanager.h"
#include "../../cure/include/packet.h"
#include "../life.h"



namespace life {



class Client;
class GameServerManager;



class ServerMessageProcessor {
public:
	ServerMessageProcessor(GameServerManager* game_server_manager);
	virtual ~ServerMessageProcessor();

	virtual void ProcessNetworkInputMessage(Client* client, cure::Message* message);
	virtual void ProcessNumber(Client* client, cure::MessageNumber::InfoType type, int32 integer, float32 f);

protected:
	typedef cure::ContextManager::ContextObjectTable ContextTable;

	GameServerManager* game_server_manager_;

	logclass();
};



}
