
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "servermessageprocessor.h"
#include "gameservermanager.h"



namespace life {



const float NETWORK_POSITIONAL_RESEND_INTERVAL = 0.6f;
const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;



ServerMessageProcessor::ServerMessageProcessor(GameServerManager* game_server_manager):
	game_server_manager_(game_server_manager) {
}

ServerMessageProcessor::~ServerMessageProcessor() {
	game_server_manager_ = 0;
}



void ServerMessageProcessor::ProcessNetworkInputMessage(Client* client, cure::Message* message) {
	cure::MessageType _type = message->GetType();
	switch (_type) {
		case cure::kMessageTypeStatus: {
			cure::MessageStatus* status = (cure::MessageStatus*)message;
			if (status->GetRemoteStatus() == cure::kRemoteOk) {
				switch (status->GetInfo()) {
					case cure::MessageStatus::kInfoChat: {
						log_debug("Chat...");
					} break;
					case cure::MessageStatus::kInfoAvatar: {
						log_debug("Avatar selected...");
						str avatar_name;
						status->GetMessageString(avatar_name);
						const cure::UserAccount::AvatarId avatar_id = avatar_name;
						game_server_manager_->OnSelectAvatar(client, avatar_id);
					} break;
					default: {
						deb_assert(false);
					} break;
				}
			} else { //if (status->GetRemoteStatus() == cure::kRemoteNoConnection)
				log_trace("User disconnects.");
				game_server_manager_->GetNetworkServer()->Disconnect(client->GetUserConnection()->GetAccountId(), "", false);
			}
		} break;
		case cure::kMessageTypeObjectPosition: {
			cure::MessageObjectPosition* movement = (cure::MessageObjectPosition*)message;
			// TODO: make sure client is authorized to control object with given ID.
			cure::GameObjectId instance_id = movement->GetObjectId();
			cure::ContextObject* object = game_server_manager_->GetContext()->GetObject(instance_id);
			if (object && (client->GetAvatarId() == instance_id || client->GetAvatarId() == object->GetBorrowerInstanceId())) {
				int32 client_frame_index = movement->GetFrameIndex();
				game_server_manager_->AdjustClientSimulationSpeed(client, client_frame_index);
				// Pass on to other clients.
				const cure::ObjectPositionalData& position = movement->GetPositionalData();
				game_server_manager_->BroadcastObjectPosition(instance_id, position, client, false);
				game_server_manager_->StoreMovement(client_frame_index, movement);
			} else {
				log_.Warningf("Client %s tried to control instance ID %i, but was not in possession.",
					client->GetUserConnection()->GetLoginName().c_str(),
					instance_id);
			}
		} break;
		case cure::kMessageTypeNumber: {
			cure::MessageNumber* message_number = (cure::MessageNumber*)message;
			ProcessNumber(client, message_number->GetInfo(), message_number->GetInteger(), message_number->GetFloat());
		} break;
		case cure::kMessageTypeObjectAttribute: {
			cure::MessageObjectAttribute* message_attrib = (cure::MessageObjectAttribute*)message;
			cure::GameObjectId object_id = message_attrib->GetObjectId();
			unsigned byte_size = 0;
			const uint8* buffer = message_attrib->GetReadBuffer(byte_size);
			game_server_manager_->GetContext()->UnpackObjectAttribute(object_id, buffer, byte_size);
		} break;
		default: {
			log_.Error("Got bad message type from client.");
		} break;
	}
}

void ServerMessageProcessor::ProcessNumber(Client* client, cure::MessageNumber::InfoType type, int32 integer, float32 _f) {
	(void)_f;

	switch (type) {
		case cure::MessageNumber::kInfoPing: {
			game_server_manager_->GetNetworkServer()->SendNumberMessage(false, client->GetUserConnection()->GetSocket(),
				cure::MessageNumber::kInfoPong, integer, client->GetPhysicsFrameAheadCount());
			log_volatile(log_.Debugf("Replying to PING (PONG) to %s (ptr=%p).",
				client->GetUserConnection()->GetSocket()->GetTargetAddress().GetAsString().c_str(),
				client->GetUserConnection()->GetSocket()));
		} break;
		case cure::MessageNumber::kInfoRecreateObject: {
			const cure::GameObjectId instance_id = integer;
			cure::ContextObject* object = game_server_manager_->GetContext()->GetObject(instance_id);
			if (object) {
				ContextTable table;
				table.insert(ContextTable::value_type(instance_id, object));
				game_server_manager_->SendObjects(client, true, table);
				log_volatile(log_.Debugf("Recreating %s (%i) on client.",
					object->GetClassId().c_str(),
					instance_id));
			} else {
				log_volatile(log_.Debugf("User %s tried to fetch unknown object with ID %i.", client->GetUserConnection()->GetLoginName().c_str(), instance_id));
			}
		} break;
		case cure::MessageNumber::kInfoRequestLoan: {
			const cure::GameObjectId instance_id = integer;
			game_server_manager_->LoanObject(client, instance_id);
		} break;
		default: {
			log_.Error("Received an invalid MessageNumber from client.");
		} break;
	}
}



loginstance(kGame, ServerMessageProcessor);



}
