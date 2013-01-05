
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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
