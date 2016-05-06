
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "serverdelegate.h"



namespace life {



ServerDelegate::ServerDelegate(GameServerManager* game_server_manager):
	game_server_manager_(game_server_manager) {
}

ServerDelegate::~ServerDelegate() {
	game_server_manager_ = 0;
}



}
