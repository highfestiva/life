
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "ServerDelegate.h"



namespace Life
{



ServerDelegate::ServerDelegate(GameServerManager* pGameServerManager):
	mGameServerManager(pGameServerManager)
{
}

ServerDelegate::~ServerDelegate()
{
	mGameServerManager = 0;
}



}
