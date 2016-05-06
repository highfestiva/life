
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "hovertankserverticker.h"
#include "../../life/lifeserver/gameservermanager.h"
#include "hovertankserverdelegate.h"
#include "hovertankservermessageprocessor.h"



namespace HoverTank {



HoverTankServerTicker::HoverTankServerTicker(cure::ResourceManager* resource_manager, float physics_radius, int physics_levels, float physics_sensitivity):
	Parent(resource_manager, physics_radius, physics_levels, physics_sensitivity) {
	HoverTankServerDelegate* delegate = new HoverTankServerDelegate(game_manager_);
	game_manager_->SetDelegate(delegate);
	game_manager_->SetMessageProcessor(new HoverTankServerMessageProcessor(game_manager_, delegate));
}

HoverTankServerTicker::~HoverTankServerTicker() {
}



}
