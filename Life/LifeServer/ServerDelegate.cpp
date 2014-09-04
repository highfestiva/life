
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
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
