
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "hovertankservermessageprocessor.h"
#include "../../life/lifeserver/gameservermanager.h"
#include "gameserverlogic.h"
#include "hovertankserverdelegate.h"



namespace HoverTank {



HoverTankServerMessageProcessor::HoverTankServerMessageProcessor(life::GameServerManager* game_server_manager, GameServerLogic* logic):
	Parent(game_server_manager),
	logic_(logic) {
}

HoverTankServerMessageProcessor::~HoverTankServerMessageProcessor() {
	logic_ = 0;
}



void HoverTankServerMessageProcessor::ProcessNumber(life::Client* client, cure::MessageNumber::InfoType type, int32 integer, float32 f) {
	switch (type) {
		case cure::MessageNumber::kInfoTool0: {
			cure::ContextObject* avatar = game_server_manager_->GetContext()->GetObject(client->GetAvatarId());
			if (avatar) {
				logic_->Shoot(avatar, (int)f);
			}
		}
		return;
	}
	Parent::ProcessNumber(client, type, integer, f);
}



loginstance(kGame, HoverTankServerMessageProcessor);



}
