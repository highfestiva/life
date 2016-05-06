
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "pushserverdelegate.h"
#include "../../cure/include/consolemanager.h"
#include "../../cure/include/floatattribute.h"
#include "../../cure/include/intattribute.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/spawner.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/random.h"
#include "../../life/lifeserver/gameservermanager.h"
#include "../rtvar.h"
#include "../rtvar.h"
#include "../version.h"



namespace Push {



PushServerDelegate::PushServerDelegate(life::GameServerManager* game_server_manager):
	Parent(game_server_manager) {
}

PushServerDelegate::~PushServerDelegate() {
}



void PushServerDelegate::SetLevel(const str& level_name) {
	ScopeLock lock(game_server_manager_->GetTickLock());
	if (level_id_) {
		game_server_manager_->DeleteContextObject(level_id_);
	}

	cure::ContextObject* level = game_server_manager_->GameManager::CreateContextObject(level_name, cure::kNetworkObjectLocallyControlled);
	level_id_ = level->GetInstanceId();
	level->StartLoading();
}



cure::ContextObject* PushServerDelegate::CreateContextObject(const str& class_id) const {
	return new cure::CppContextObject(game_server_manager_->GetResourceManager(), class_id);
}

void PushServerDelegate::OnOpen() {
	SetLevel("level_01");
}

void PushServerDelegate::OnLogin(life::Client*) {
}

void PushServerDelegate::OnLogout(life::Client*) {
}



void PushServerDelegate::OnSelectAvatar(life::Client* client, const cure::UserAccount::AvatarId& avatar_id) {
	const cure::GameObjectId previous_avatar_id = client->GetAvatarId();
	if (previous_avatar_id) {
		log_.Info("User "+client->GetUserConnection()->GetLoginName()+" had an avatar, replacing it.");
		client->SetAvatarId(0);
		cure::ContextObject* _object = game_server_manager_->GetContext()->GetObject(previous_avatar_id);
		if (_object) {
			xform transform;
			transform.SetPosition(_object->GetPosition());
			transform.GetPosition() += vec3(0, 0, 2);
			vec3 euler_angles;
			_object->GetOrientation().GetEulerAngles(euler_angles);
			quat q;
			q.SetEulerAngles(euler_angles.x, 0, 0);
			transform.SetOrientation(q * transform.GetOrientation());
		}
		game_server_manager_->DeleteContextObject(previous_avatar_id);
	}

	cure::Spawner* spawner = game_server_manager_->GetAvatarSpawner(level_id_);
	if (!spawner) {
		log_.Error("No player spawner in level!");
		return;
	}
	log_.Info("Loading avatar '"+avatar_id+"' for user "+client->GetUserConnection()->GetLoginName()+".");
	cure::ContextObject* _object = game_server_manager_->Parent::CreateContextObject(avatar_id, cure::kNetworkObjectRemoteControlled);
	spawner->PlaceObject(_object, -1);
	client->SetAvatarId(_object->GetInstanceId());
	_object->SetExtraData((void*)(intptr_t)client->GetUserConnection()->GetAccountId());
	_object->StartLoading();
}

void PushServerDelegate::OnLoadAvatar(life::Client* client, cure::ContextObject* avatar) {
	(void)client;
	(void)avatar;
}

void PushServerDelegate::OnLoadObject(cure::ContextObject* object) {
	(void)object;
}

void PushServerDelegate::OnDeleteObject(cure::ContextObject* object) {
	(void)object;
}



bool PushServerDelegate::IsObjectLendable(life::Client* client, cure::ContextObject* object) {
	(void)client;
	(void)object;
	return true;
}



void PushServerDelegate::PreEndTick() {
}



loginstance(kGame, PushServerDelegate);



}
