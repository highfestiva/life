
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "hovertankserverdelegate.h"
#include "../../cure/include/consolemanager.h"
#include "../../cure/include/health.h"
#include "../../cure/include/floatattribute.h"
#include "../../cure/include/intattribute.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../cure/include/spawner.h"
#include "../../lepra/include/random.h"
#include "../../life/lifeserver/gameservermanager.h"
#include "../../life/lifeserver/serverfastprojectile.h"
#include "../../life/lifeserver/servermine.h"
#include "../../life/lifeserver/serverprojectile.h"
#include "../../life/explosion.h"
#include "../../life/projectileutil.h"
#include "../rtvar.h"
#include "../rtvar.h"
#include "../version.h"
#include "bombplane.h"
#include "npc.h"
#include "hovertankserverconsole.h"

#define kKills	"int_kills:"
#define kDeaths	"int_deaths:"
#define kPing	"int_ping:"



namespace HoverTank {



HoverTankServerDelegate::HoverTankServerDelegate(life::GameServerManager* game_server_manager):
	Parent(game_server_manager),
	level_id_(0),
	score_info_id_(0) {
	v_set(game_server_manager_->GetVariableScope(), kRtvarGameNpcskill, 0.5);
	v_set(game_server_manager_->GetVariableScope(), kRtvarDebugServerindicatehit, 0.0);
}

HoverTankServerDelegate::~HoverTankServerDelegate() {
	score_info_id_ = 0;
	level_id_ = 0;
}



void HoverTankServerDelegate::SetLevel(const str& level_name) {
	ScopeLock lock(game_server_manager_->GetTickLock());
	if (level_id_) {
		game_server_manager_->DeleteContextObject(level_id_);
	}

	cure::ContextObject* level = game_server_manager_->GameManager::CreateContextObject(level_name, cure::kNetworkObjectLocallyControlled);
	level_id_ = level->GetInstanceId();
	level->StartLoading();
}



cure::ContextObject* HoverTankServerDelegate::CreateContextObject(const str& class_id) const {
	if (strutil::StartsWith(class_id, "mine_")) {
		return new life::ServerMine(game_server_manager_->GetResourceManager(), class_id, (HoverTankServerDelegate*)this);
	} else if (strutil::StartsWith(class_id, "deltawing")) {
		return new BombPlane(game_server_manager_->GetResourceManager(), class_id, (HoverTankServerDelegate*)this, vec3());
	}
	return new cure::CppContextObject(game_server_manager_->GetResourceManager(), class_id);
}

void HoverTankServerDelegate::OnOpen() {
	new HoverTankServerConsole(this, game_server_manager_->GetConsoleManager()->GetConsoleCommandManager());

	SetLevel("level_02");

	cure::ContextObject* score_info = game_server_manager_->GameManager::CreateContextObject("score_info", cure::kNetworkObjectLocallyControlled);
	score_info_id_ = score_info->GetInstanceId();
	score_info->SetLoadResult(true);
}

void HoverTankServerDelegate::OnLogin(life::Client* client) {
#ifdef LEPRA_DEBUG
	if (game_server_manager_->GetLoggedInClientCount() == 1) {
		deb_assert(npc_set_.empty());
	}
#endif // Debug

	// Create scores.
	const str login_name = client->GetUserConnection(->GetLoginName());
	CreateScore(login_name, true);

	// Create another computer opponent, to balance teams.
	CreateNpc();
}

void HoverTankServerDelegate::OnLogout(life::Client* client) {
	// Drop scores.
	const str login_name = client->GetUserConnection(->GetLoginName());
	DeleteScore(login_name);

	// Drop a computer opponent, to balance teams.
	DeleteNpc();

#ifdef LEPRA_DEBUG
	if (game_server_manager_->GetLoggedInClientCount() == 0) {
		deb_assert(npc_set_.empty());
	}
#endif // Debug
}



void HoverTankServerDelegate::OnSelectAvatar(life::Client* client, const cure::UserAccount::AvatarId& avatar_id) {
	const cure::GameObjectId previous_avatar_id = client->GetAvatarId();
	if (previous_avatar_id) {
		log_.Info("User "+strutil::Encode(client->GetUserConnection()->GetLoginName())+" had an avatar, replacing it.");
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
		log_.AError("No player spawner in level!");
		return;
	}
	log_.Info("Loading avatar '"+avatar_id+"' for user "+strutil::Encode(client->GetUserConnection()->GetLoginName())+".");
	cure::ContextObject* _object = game_server_manager_->GameServerManager::Parent::CreateContextObject(avatar_id, cure::kNetworkObjectRemoteControlled);
	spawner->PlaceObject(_object, -1);
	client->SetAvatarId(_object->GetInstanceId());
	_object->SetExtraData((void*)(intptr_t)client->GetUserConnection()->GetAccountId());
	_object->StartLoading();
}

void HoverTankServerDelegate::OnLoadAvatar(life::Client* client, cure::ContextObject* avatar) {
	cure::Health::Set(avatar, 1);
	if (client) {
		// User avatar.
		AddAvatarToTeam(avatar, 0);
	} else {
		// NPC avatar.
		AddAvatarToTeam(avatar, 1);
	}
}

void HoverTankServerDelegate::OnLoadObject(cure::ContextObject* object) {
	(void)object;
}

void HoverTankServerDelegate::OnDeleteObject(cure::ContextObject* object) {
	if (object) {
		RemoveAvatar(object);
	}
}



bool HoverTankServerDelegate::IsObjectLendable(life::Client* client, cure::ContextObject* object) {
	(void)client;
	return !strutil::StartsWith(object->GetClassId(), "hover_tank");
}



void HoverTankServerDelegate::PreEndTick() {
	TickNpcGhosts();

	if (ping_update_timer_.QueryTimeDiff() > 5.0) {
		ping_update_timer_.ClearTimeDiff();
		UpdatePing();
	}
}



void HoverTankServerDelegate::OrderAirStrike(const vec3& position, float fly_in_angle) {
	const float plane_distance = 1000;

	cure::ContextObject* plane = new BombPlane(game_server_manager_->GetResourceManager(), "deltawing", this, position);
	game_server_manager_->AddContextObject(plane, cure::kNetworkObjectLocallyControlled, 0);
	xform t;
	t.GetPosition().Set(plane_distance*::sin(fly_in_angle), plane_distance*::cos(fly_in_angle), 30);
	t.GetPosition().x += position.x;
	t.GetPosition().y += position.y;
	t.GetOrientation().RotateAroundOwnX(PIF/2);
	t.GetOrientation().RotateAroundWorldZ(-fly_in_angle);
	plane->SetInitialTransform(t);
	plane->StartLoading();
}



void HoverTankServerDelegate::Shoot(cure::ContextObject* avatar, int weapon) {
	str ammo;
	bool is_fast = true;
	cure::NetworkObjectType network_type = cure::kNetworkObjectLocallyControlled;
	switch (weapon) {
		case 0:		ammo = "bullet";	network_type = cure::kNetworkObjectLocalOnly;	break;
		case 1:		ammo = "grenade";							break;
		case 2:		ammo = "rocket";							break;
		case -10:	ammo = "bomb";	is_fast = false;				break;
		default: deb_assert(false); return;
	}
	cure::ContextObject* projectile;
	if (is_fast) {
		projectile = new life::ServerFastProjectile(game_server_manager_->GetResourceManager(), ammo, this);
	} else {
		projectile = new life::ServerProjectile(game_server_manager_->GetResourceManager(), ammo, 0, this);
	}
	game_server_manager_->AddContextObject(projectile, network_type, 0);
	log_volatile(log_.Debugf("Shooting projectile with ID %i!", (int)projectile->GetInstanceId()));
	projectile->SetOwnerInstanceId(avatar->GetInstanceId());
	xform t;
	vec3 v;
	if (life::ProjectileUtil::GetBarrel(projectile, t, v)) {
		projectile->SetInitialTransform(t);
		projectile->StartLoading();
	}

	if (network_type == cure::kNetworkObjectLocalOnly) {
		// Transmit the shoot event rather than the projectile itself.
		life::Client* _client = game_server_manager_->GetClientByObject(avatar);
		game_server_manager_->BroadcastNumberMessage(_client, false, cure::MessageNumber::kInfoTool0, avatar->GetInstanceId(), (float)weapon);
	}
}

void HoverTankServerDelegate::Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength) {
	(void)explosive_geometry;
	(void)velocity;
	(void)normal;

	float indicate_hit;
	v_get(indicate_hit, =(float), game_server_manager_->GetVariableScope(), kRtvarDebugServerindicatehit, 0.0);
	game_server_manager_->IndicatePosition(position, indicate_hit);

	ScopeLock lock(game_server_manager_->GetTickLock());
	tbc::PhysicsManager* physics_manager = game_server_manager_->GetPhysicsManager();
	cure::ContextManager::ContextObjectTable object_table = game_server_manager_->GetContext()->GetObjectTable();
	cure::ContextManager::ContextObjectTable::iterator x = object_table.begin();
	for (; x != object_table.end(); ++x) {
		cure::ContextObject* _object = x->second;
		if (!_object->IsLoaded()) {
			continue;
		}
		const str& _class_id = _object->GetClassId();
		if (strutil::StartsWith(_class_id, "bomb")) {	// Prevent bombs from hover_tanking each other away from the target!
			continue;
		}
		float endurance_reciproc = 1;
		if (strutil::StartsWith(_class_id, "deltawing")) {	// Prevent bombers from getting their noses hover_tanked upwards when bombs go off!
			endurance_reciproc = 0.1f;
		}
		const float force = life::Explosion::CalculateForce(physics_manager, _object, position, strength * endurance_reciproc);
		if (force > 0 && _object->GetNetworkObjectType() != cure::kNetworkObjectLocalOnly) {
			cure::FloatAttribute* _health = cure::Health::GetAttribute(_object);
			if (_health) {
				DrainHealth(explosive, _object, _health, force*Random::Normal(0.51f, 0.05f, 0.3f, 0.5f));
			}
			x->second->ForceSend();
		}
		life::Explosion::PushObject(physics_manager, _object, position, strength * endurance_reciproc, 1);
	}
}

void HoverTankServerDelegate::OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object) {
	float indicate_hit;
	v_get(indicate_hit, =(float), game_server_manager_->GetVariableScope(), kRtvarDebugServerindicatehit, 0.0);
	game_server_manager_->IndicatePosition(bullet->GetPosition(), indicate_hit);

	cure::FloatAttribute* _health = cure::Health::GetAttribute(hit_object);
	if (_health) {
		DrainHealth(bullet, hit_object, _health, Random::Normal(0.17f, 0.01f, 0.1f, 0.3f));
		const float incoming_angle = 2*PIF * Random::Uniform(0.0f, 1.0f);
		OrderAirStrike(hit_object->GetPosition(), incoming_angle);
		vec3 v(27*::sin(incoming_angle), 27*::cos(incoming_angle), 2);
		quat q = quat();
		q.RotateAroundWorldZ(PIF/4);
		const vec3 r = RNDVEC(3.0f);
		OrderAirStrike(hit_object->GetPosition() + q*v+r, incoming_angle);
		q.RotateAroundWorldZ(-PIF/2);
		OrderAirStrike(hit_object->GetPosition() + q*v+r, incoming_angle);
	}
}



cure::ContextObject* HoverTankServerDelegate::CreateAvatarForNpc(Npc* npc) {
	double spawn_part;
	v_get(spawn_part, =, game_server_manager_->GetVariableScope(), kRtvarGameSpawnpart, 1.0);
	if (Random::Uniform(0.0, 0.999) >= spawn_part) {
		return 0;
	}

	cure::Spawner* spawner = game_server_manager_->GetAvatarSpawner(level_id_);
	if (!spawner) {
		log_.AError("No NPC spawner in level!");
		return 0;
	}

	cure::ContextObject* _avatar = game_server_manager_->GameManager::CreateContextObject("hover_tank_01", cure::kNetworkObjectLocallyControlled);
	spawner->PlaceObject(_avatar, -1);
	_avatar->SetExtraData((void*)-1);
	_avatar->StartLoading();
	npc->SetAvatarId(_avatar->GetInstanceId());
	return _avatar;
}

void HoverTankServerDelegate::AddAvatarToTeam(cure::ContextObject* avatar, int team) {
	deb_assert(avatar);
	deb_assert(team == 0 || team == 1);
	deb_assert(!IsAvatarObject(avatar));
	avatar_team_sets_[team].insert(avatar->GetInstanceId());
	cure::IntAttribute* _team = (cure::IntAttribute*)avatar->GetAttribute("int_team");
	if (_team) {
		_team->SetValue(team);
	} else {
		new cure::IntAttribute(avatar, "int_team", team);
	}
}

void HoverTankServerDelegate::RemoveAvatar(cure::ContextObject* avatar) {
	AvatarIdSet::iterator x;
	bool found0 = ((x = avatar_team_sets_[0].find(avatar->GetInstanceId())) != avatar_team_sets_[0].end());
	if (found0) {
		avatar_team_sets_[0].erase(x);
	}
	bool found1 = ((x = avatar_team_sets_[1].find(avatar->GetInstanceId())) != avatar_team_sets_[1].end());
	if (found1) {
		avatar_team_sets_[1].erase(x);
	}
	if (!found0 && !found1) {
		return;
	}


	Npc* _npc = GetNpcByAvatar(avatar->GetInstanceId());
	if (_npc) {
			_npc->SetAvatarId(0);
	}
}

const HoverTankServerDelegate::AvatarIdSet& HoverTankServerDelegate::GetAvatarsInTeam(int team) {
	deb_assert(team == 0 || team == 1);
	return avatar_team_sets_[team];
}



void HoverTankServerDelegate::CreateNpc() {
	Npc* _npc = new Npc(this);
	game_server_manager_->GetContext()->AddLocalObject(_npc);
	npc_set_.insert(_npc->GetInstanceId());
	_npc->StartCreateAvatar(0.1f);

	const str _player_name = strutil::Format("NPC %u", _npc->GetInstanceId());
	CreateScore(_player_name, false);
}

void HoverTankServerDelegate::DeleteNpc() {
	deb_assert(!npc_set_.empty());
	AvatarIdSet::iterator x = npc_set_.begin();
	log_.Headlinef("Deleting NPC %u.", *x);
	game_server_manager_->GetContext()->PostKillObject(*x);

	const str _player_name = strutil::Format("NPC %u", *x);
	DeleteScore(_player_name);

	npc_set_.erase(x);
}

Npc* HoverTankServerDelegate::GetNpcByAvatar(cure::GameObjectId avatar_id) const {
	for (AvatarIdSet::const_iterator x = npc_set_.begin(); x != npc_set_.end(); ++x) {
		Npc* _npc = (Npc*)game_server_manager_->GetContext()->GetObject(*x);
		if (_npc && _npc->GetAvatarId() == avatar_id) {
			return _npc;
		}
	}
	return 0;
}

void HoverTankServerDelegate::CreateScore(const str& player_name, bool create_ping) {
	deb_assert(score_info_id_);
	cure::ContextObject* score_info = game_server_manager_->GetContext()->GetObject(score_info_id_);
	deb_assert(score_info);
	new cure::IntAttribute(score_info, kKills + player_name, 0);
	new cure::IntAttribute(score_info, kDeaths + player_name, 0);
	if (create_ping) {
		new cure::IntAttribute(score_info, kPing + player_name, 0);
	}
}

void HoverTankServerDelegate::DeleteScore(const str& player_name) {
	deb_assert(score_info_id_);
	cure::ContextObject* score_info = game_server_manager_->GetContext()->GetObject(score_info_id_);
	deb_assert(score_info);
	score_info->DeleteAttribute(kKills + player_name);
	score_info->DeleteAttribute(kDeaths + player_name);
	score_info->DeleteAttribute(kPing + player_name);
}

void HoverTankServerDelegate::UpdatePing() {
	const cure::TimeManager* time_manager = game_server_manager_->GetTimeManager();
	typedef life::GameServerManager::AccountClientTable ClientTable;
	const ClientTable& clients = game_server_manager_->GetAccountClientTable();
	ClientTable::ConstIterator x = clients.First();
	for (; x != clients.End(); ++x) {
		int ping = (int)(time_manager->ConvertPhysicsFramesToSeconds((int)x.GetObject()->GetPhysicsFrameAheadCount()) * 1000);
		ping = std::abs(ping) * 2;
		SetPoints(kPing, x.GetObject(), ping);
	}
}

void HoverTankServerDelegate::AddPoint(const str& prefix, const cure::ContextObject* avatar, int points) {
	if (!avatar) {
		return;
	}
	cure::ContextObject* _avatar = (cure::ContextObject*)avatar;
	life::Client* _client = game_server_manager_->GetClientByObject(_avatar);
	str _player_name;
	if (_client) {
		_player_name = _client->GetUserConnection(->GetLoginName());
	} else {
		Npc* _npc = GetNpcByAvatar(avatar->GetInstanceId());
		if (!_npc) {
			return;
		}
		_player_name = strutil::Format("NPC %u", _npc->GetInstanceId());
	}
	deb_assert(score_info_id_);
	cure::ContextObject* score_info = game_server_manager_->GetContext()->GetObject(score_info_id_);
	deb_assert(score_info);
	cure::IntAttribute* attribute = (cure::IntAttribute*)score_info->GetAttribute(prefix+_player_name);
	if (attribute) {
		attribute->SetValue(attribute->GetValue() + points);
	}
}

void HoverTankServerDelegate::SetPoints(const str& prefix, const life::Client* client, int points) {
	const str _player_name = client->GetUserConnection(->GetLoginName());
	deb_assert(score_info_id_);
	cure::ContextObject* score_info = game_server_manager_->GetContext()->GetObject(score_info_id_);
	deb_assert(score_info);
	cure::IntAttribute* attribute = (cure::IntAttribute*)score_info->GetAttribute(prefix+_player_name);
	deb_assert(attribute);
	if (attribute) {
		attribute->SetValue(points);
	}
}

void HoverTankServerDelegate::DrainHealth(cure::ContextObject* explosive, cure::ContextObject* object, cure::FloatAttribute* health, float damage) {
	deb_assert(health);
	const float prior_health = health->GetValue();
	float remaining_health = prior_health - damage;
	if (prior_health > 0) {
		health->SetValue(remaining_health);
	}
	if (prior_health > 0 && remaining_health <= 0 && IsAvatarObject(object)) {
		cure::ContextObject* _avatar = object;
		AddPoint(kDeaths, _avatar, +1);
		if (explosive->GetOwnerInstanceId()) {
			const int _points = (explosive->GetOwnerInstanceId() == _avatar->GetInstanceId()) ? -1 : +1;	// Kills oneself?
			AddPoint(kKills, game_server_manager_->GetContext()->GetObject(explosive->GetOwnerInstanceId()), _points);
		}
		Die(_avatar);
	}
}

void HoverTankServerDelegate::Die(cure::ContextObject* avatar) {
	life::Explosion::FallApart(game_server_manager_->GetPhysicsManager(), (cure::CppContextObject*)avatar);
	game_server_manager_->GetContext()->DelayKillObject(avatar, 2);
	game_server_manager_->BroadcastNumberMessage(0, true, cure::MessageNumber::kInfoFallApart, avatar->GetInstanceId(), 0);
}

bool HoverTankServerDelegate::IsAvatarObject(const cure::ContextObject* object) const {
	return avatar_team_sets_[0].find(object->GetInstanceId()) != avatar_team_sets_[0].end() ||
		avatar_team_sets_[1].find(object->GetInstanceId()) != avatar_team_sets_[1].end();
}

void HoverTankServerDelegate::TickNpcGhosts() {
	ScopeLock lock(game_server_manager_->GetTickLock());
	const cure::ContextManager* context_manager = game_server_manager_->GetContext();
	const int step_count = game_server_manager_->GetTimeManager()->GetAffordedPhysicsStepCount();
	const float physics_frame_time = game_server_manager_->GetTimeManager()->GetAffordedPhysicsStepTime();
	for (int _team = 0; _team <= 1; ++_team) {
		AvatarIdSet::iterator x = avatar_team_sets_[_team].begin();
		for (; x != avatar_team_sets_[_team].end(); ++x) {
			cure::ContextObject* _avatar = context_manager->GetObject(*x);
			if (_avatar && _avatar->GetExtraData() == 0) {	// Exists && NPC check.
				_avatar->GetNetworkOutputGhost()->GhostStep(step_count, physics_frame_time);
				const cure::ObjectPositionalData* positional_data = 0;
				if (!_avatar->UpdateFullPosition(positional_data)) {
					continue;
				}
				if (!positional_data->IsSameStructure(*_avatar->GetNetworkOutputGhost())) {
					_avatar->GetNetworkOutputGhost()->CopyData(positional_data);
					continue;
				}
				float resync_on_diff;
				v_get(resync_on_diff, =(float), game_server_manager_->GetVariableScope(), kRtvarNetphysResyncondiffgt, 0.2);
				if (positional_data->GetScaledDifference(_avatar->GetNetworkOutputGhost()) > resync_on_diff) {
					log_volatile(log_.Debugf("NPC avatar %s (%u sending pos due to deviation."), _avatar->GetClassId().c_str(), _avatar->GetInstanceId()));
					_avatar->GetNetworkOutputGhost()->CopyData(positional_data);
					game_server_manager_->GetContext()->AddPhysicsSenderObject(_avatar);
				}
			}
		}
	}
}



loginstance(kGame, HoverTankServerDelegate);



}
