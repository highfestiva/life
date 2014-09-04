
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/Packet.h"
#include "../Life.h"



namespace Life
{



class Client;
class GameServerManager;



class ServerMessageProcessor
{
public:
	ServerMessageProcessor(GameServerManager* pGameServerManager);
	virtual ~ServerMessageProcessor();

	virtual void ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage);
	virtual void ProcessNumber(Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);

protected:
	typedef Cure::ContextManager::ContextObjectTable ContextTable;

	GameServerManager* mGameServerManager;

	logclass();
};



}
