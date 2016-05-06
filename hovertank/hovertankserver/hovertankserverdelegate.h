
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "../../life/lifeserver/serverdelegate.h"
#include "../../lepra/include/hirestimer.h"
#include "../../life/launcher.h"
#include "../hovertank.h"
#include "gameserverlogic.h"



namespace cure {
class FloatAttribute;
}



namespace HoverTank {



class HoverTankServerDelegate: public life::ServerDelegate, public GameServerLogic, public life::Launcher {
	typedef life::ServerDelegate Parent;
public:
	HoverTankServerDelegate(life::GameServerManager* game_server_manager);
	virtual ~HoverTankServerDelegate();

	virtual void SetLevel(const str& level_name);

private:
	virtual cure::ContextObject* CreateContextObject(const str& class_id) const;

	virtual void OnOpen();
	virtual void OnLogin(life::Client* client);
	virtual void OnLogout(life::Client* client);

	virtual void OnSelectAvatar(life::Client* client, const cure::UserAccount::AvatarId& avatar_id);
	virtual void OnLoadAvatar(life::Client* client, cure::ContextObject* avatar);
	virtual void OnLoadObject(cure::ContextObject* object);
	virtual void OnDeleteObject(cure::ContextObject* object);

	virtual bool IsObjectLendable(life::Client* client, cure::ContextObject* object);

	virtual void PreEndTick();

	void OrderAirStrike(const vec3& position, float fly_in_angle);

	virtual void Shoot(cure::ContextObject* avatar, int weapon);
	virtual void Detonate(cure::ContextObject* explosive, const tbc::ChunkyBoneGeometry* explosive_geometry, const vec3& position, const vec3& velocity, const vec3& normal, float strength);
	virtual void OnBulletHit(cure::ContextObject* bullet, cure::ContextObject* hit_object);

	virtual cure::ContextObject* CreateAvatarForNpc(Npc* npc);
	virtual void AddAvatarToTeam(cure::ContextObject* avatar, int team);
	virtual void RemoveAvatar(cure::ContextObject* avatar);
	virtual const AvatarIdSet& GetAvatarsInTeam(int team);

	void CreateNpc();
	void DeleteNpc();
	Npc* GetNpcByAvatar(cure::GameObjectId avatar_id) const;
	void CreateScore(const str& player_name, bool create_ping);
	void DeleteScore(const str& player_name);
	void UpdatePing();
	void AddPoint(const str& prefix, const cure::ContextObject* avatar, int points);
	void SetPoints(const str& prefix, const life::Client* client, int points);
	void DrainHealth(cure::ContextObject* explosive, cure::ContextObject* avatar, cure::FloatAttribute* health, float damage);
	void Die(cure::ContextObject* avatar);
	bool IsAvatarObject(const cure::ContextObject* object) const;
	void TickNpcGhosts();

	enum Command {
		kCommandSetLevel,
	};

	cure::GameObjectId level_id_;
	cure::GameObjectId score_info_id_;
	AvatarIdSet avatar_team_sets_[2];
	AvatarIdSet npc_set_;
	HiResTimer ping_update_timer_;

	logclass();
};



}
