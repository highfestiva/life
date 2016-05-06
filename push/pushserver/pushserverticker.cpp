
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "pushserverticker.h"
#include "../../life/lifeserver/gameservermanager.h"
#include "../../life/lifeserver/servermessageprocessor.h"
#include "pushserverdelegate.h"



namespace Push {



PushServerTicker::PushServerTicker(cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(resource_manager, physics_radius, physics_levels, physics_sensitivity) {
	PushServerDelegate* delegate = new PushServerDelegate(game_manager_);
	game_manager_->SetDelegate(delegate);
	game_manager_->SetMessageProcessor(new life::ServerMessageProcessor(game_manager_));
}

PushServerTicker::~PushServerTicker() {
}



}
