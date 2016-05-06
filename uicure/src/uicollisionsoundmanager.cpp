
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uicollisionsoundmanager.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/hashutil.h"
#include "../../cure/include/delayeddeleter.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../include/uigameuimanager.h"



namespace UiCure {



#define MINIMUM_PLAYED_VOLUME_FACTOR	0.1f



CollisionSoundManager::CollisionSoundManager(cure::GameManager* game_manager, UiCure::GameUiManager* ui_manager):
	game_manager_(game_manager),
	ui_manager_(ui_manager) {
	SetScale(50, 0.5f, 1, 0.2f);
}

CollisionSoundManager::~CollisionSoundManager() {
	ui_manager_ = 0;
	game_manager_ = 0;
}

void CollisionSoundManager::SetScale(float small_mass, float light_impact, float impact_volume_factor, float sound_cutoff_duration) {
	small_mass_ = small_mass;
	light_impact_ = light_impact;
	impact_volume_factor_ = impact_volume_factor;
	sound_cutoff_duration_ = sound_cutoff_duration;
}

void CollisionSoundManager::AddSound(const str& name, const SoundResourceInfo& info) {
	ScopeLock lock(&lock_);
	sound_name_map_.insert(SoundNameMap::value_type(name, info));
}

void CollisionSoundManager::PreLoadSound(const str& name) {
	CollisionSoundResource* sound = new CollisionSoundResource(ui_manager_, 0);
	sound->Load(game_manager_->GetResourceManager(), "collision_"+name+".wav",
		UiCure::UserSound3dResource::TypeLoadCallback(this, &CollisionSoundManager::OnSoundPreLoaded));
}


void CollisionSoundManager::Tick(const vec3& camera_position) {
	ScopeLock lock(&lock_);

	camera_position_ = camera_position;

	float real_time_ratio;
	v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);

	SoundMap::iterator x = sound_map_.begin();
	while (x != sound_map_.end()) {
		SoundInfo* _sound_info = x->second;
		bool is_playing = false;
		bool one_is_playing_or_loading = false;
		if (_sound_info->sound_) {
			if (_sound_info->sound_->GetLoadState() == cure::kResourceLoadComplete) {
				is_playing = ui_manager_->GetSoundManager()->IsPlaying(_sound_info->sound_->GetData());
				one_is_playing_or_loading |= is_playing;
			} else if (_sound_info->sound_->GetLoadState() == cure::kResourceLoadInProgress) {
				one_is_playing_or_loading = true;
			}
		}
		if (one_is_playing_or_loading) {
			if (is_playing && real_time_ratio != 1) {
				ui_manager_->GetSoundManager()->SetPitch(_sound_info->sound_->GetData(), _sound_info->pitch_*real_time_ratio);
			}
			++x;
		} else {
			sound_map_.erase(x);
			delete _sound_info;
			break;	// Map disallows further interation after erase, update next loop.
		}
	}
}

void CollisionSoundManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, float max_distance, bool is_loud) {
	if (!object1 || !object2) {
		return;
	}
	if (position.GetDistanceSquared(camera_position_) > max_distance*max_distance) {
		return;
	}
	/*if (object1->GetVelocity().GetDistanceSquared(object2->GetVelocity()) < light_impact_) {
		log_.Infof("Not playing sound due to low velocity diff. Force=(%g,%g,%g, light impact %g, v1=(%g,%g,%g), v2=(%g,%g,%g)"),
				force.x, force.y, force.z, light_impact_, object1->GetVelocity().x, object1->GetVelocity().y, object1->GetVelocity().z,
				object2->GetVelocity().x, object2->GetVelocity().y, object2->GetVelocity().z);
		return;
	}*/
	float _impact = object1->GetImpact(game_manager_->GetPhysicsManager()->GetGravity(), force, torque*0.01f, small_mass_);
	if (_impact < light_impact_) {
		if (!is_loud) {
			/*log_.Infof("Not playing sound due to light impact %g (light impact %g. Force (%g,%g,%g), torque (%g,%g,%g)"),
					_impact, light_impact_, force.x, force.y, force.z, torque.x, torque.y, torque.z);*/
			return;
		}
		_impact = light_impact_;
	}
	/*log_.Infof("Playing sound. Impact %g (light impact %g. Force (%g,%g,%g), torque (%g,%g,%g)"),
			_impact, light_impact_, force.x, force.y, force.z, torque.x, torque.y, torque.z);*/
	const float volume_impact = std::min(2.0f, _impact*impact_volume_factor_);
	const tbc::ChunkyBoneGeometry* _key = object1->GetStructureGeometry(body1_id);
	OnCollision(volume_impact, position, _key, _key->GetMaterial());
}

void CollisionSoundManager::OnCollision(float impact, const vec3& position, const tbc::ChunkyBoneGeometry* key, const str& sound_name) {
	ScopeLock lock(&lock_);
	SoundInfo* _sound_info = GetPlayingSound(key);
	if (_sound_info) {
		const double time = (_sound_info->sound_->GetLoadState() == cure::kResourceLoadComplete)? ui_manager_->GetSoundManager()->GetStreamTime(_sound_info->sound_->GetRamData()) : 0;
		if (time < sound_cutoff_duration_ && strutil::StartsWith(_sound_info->sound_->GetName(), "collision_"+sound_name)) {
			if (impact > _sound_info->base_impact_ * 1.2f) {
				// We are louder! Use our impact instead!
				_sound_info->base_impact_ = impact;
				UpdateSound(_sound_info);
			}
		} else {
			// We are newer or different, or the sound has already stopped.
			if (time > 100 || impact > _sound_info->base_impact_ * 0.7f) {
				// ... and we almost as load! Play us instead!
				StopSound(key);
				PlaySound(key, sound_name, position, impact);
			} else {
				// We must play both sounds at once. This hack (using key+1) will allow us to fire-and-forget.
				OnCollision(impact, position, key+1, sound_name);
			}
		}
	} else {
		PlaySound(key, sound_name, position, impact);
	}
}



CollisionSoundManager::SoundInfo* CollisionSoundManager::GetPlayingSound(const tbc::ChunkyBoneGeometry* geometry_key) const {
	return HashUtil::FindMapObject(sound_map_, geometry_key);
}

void CollisionSoundManager::PlaySound(const tbc::ChunkyBoneGeometry* geometry_key, const str& sound_name, const vec3& position, float impact) {
	SoundResourceInfo resource;
	bool got_sound = HashUtil::TryFindMapObject(sound_name_map_, sound_name, resource);
	got_sound &= (SoundInfo::GetVolume(impact, resource) >= light_impact_*MINIMUM_PLAYED_VOLUME_FACTOR);
	if (!got_sound) {
		//log_.Warningf("Unable to play sound %s.", sound_name.c_str());
		return;
	}

	if (GetPlayingSound(geometry_key)) {
		log_.Warningf("Already playing sound %s, can't play it again!", sound_name.c_str());
		return;
	}
	SoundInfo* _sound_info = new SoundInfo(resource);
	_sound_info->position_ = position;
	_sound_info->base_impact_ = impact;
	_sound_info->sound_ = new CollisionSoundResource(ui_manager_, _sound_info);
	sound_map_.insert(SoundMap::value_type(geometry_key, _sound_info));
	_sound_info->sound_->Load(game_manager_->GetResourceManager(), "collision_"+sound_name+".wav",
		UiCure::UserSound3dResource::TypeLoadCallback(this, &CollisionSoundManager::OnSoundLoaded));
}

void CollisionSoundManager::OnSoundLoaded(UiCure::UserSound3dResource* sound_resource) {
	ScopeLock lock(&lock_);
	SoundInfo* _sound_info = ((CollisionSoundResource*)sound_resource)->sound_info_;
	//deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		ui_manager_->GetSoundManager()->SetSoundPosition(sound_resource->GetData(),
			_sound_info->position_, vec3());
		_sound_info->UpdateImpact();
		ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), _sound_info->volume_, _sound_info->pitch_ * real_time_ratio);
	}
}

void CollisionSoundManager::OnSoundPreLoaded(UiCure::UserSound3dResource* sound_resource) {
	new cure::DelayedDeleter<UiCure::UserSound3dResource>(game_manager_->GetResourceManager(), game_manager_->GetContext(), sound_resource);
}

void CollisionSoundManager::UpdateSound(SoundInfo* sound_info) {
	//for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(sound_info->sound_); ++x)
	{
		if (!sound_info->sound_ || sound_info->sound_->GetLoadState() != cure::kResourceLoadComplete) {
			return;
		}
		float real_time_ratio;
		v_get(real_time_ratio, =(float), cure::GetSettings(), kRtvarPhysicsRtr, 1.0);
		sound_info->UpdateImpact();
		ui_manager_->GetSoundManager()->SetVolume(sound_info->sound_->GetData(), sound_info->volume_);
		ui_manager_->GetSoundManager()->SetPitch(sound_info->sound_->GetData(), sound_info->pitch_ * real_time_ratio);
	}
}

void CollisionSoundManager::StopSound(const tbc::ChunkyBoneGeometry* geometry_key) {
	SoundMap::iterator x = sound_map_.find(geometry_key);
	if (x == sound_map_.end()) {
		return;
	}
	SoundInfo* _sound_info = x->second;
	sound_map_.erase(x);
	if (_sound_info->sound_->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->Stop(_sound_info->sound_->GetData());
	}
	delete _sound_info;
}



CollisionSoundManager::SoundResourceInfo::SoundResourceInfo():
	strength_(0),
	minimum_clamp_(0) {
}

CollisionSoundManager::SoundResourceInfo::SoundResourceInfo(float strength, float minimum_clamp, float pitch_factor):
	strength_(strength),
	minimum_clamp_(minimum_clamp),
	pitch_factor_(pitch_factor) {
}



CollisionSoundManager::SoundInfo::SoundInfo(const SoundResourceInfo& resource_info):
	base_impact_(0),
	resource_info_(resource_info),
	volume_(0),
	pitch_(0),
	sound_(0) {
}

CollisionSoundManager::SoundInfo::~SoundInfo() {
	//for (unsigned x = 0; x < LEPRA_ARRAY_COUNT(sound_); ++x)
	{
		delete sound_;
		sound_ = 0;
	}
}

void CollisionSoundManager::SoundInfo::UpdateImpact() {
	volume_ = GetVolume(base_impact_, resource_info_);
	if (!resource_info_.pitch_factor_) {
		pitch_ = 1;
	} else {
		const float target_pitch = Math::Clamp(base_impact_, resource_info_.minimum_clamp_, 1.0f);
		pitch_ = Math::Lerp(1.0f, target_pitch, resource_info_.pitch_factor_);
	}
}

float CollisionSoundManager::SoundInfo::GetVolume(float base_impact, const SoundResourceInfo& resource_info) {
	return std::max(base_impact * resource_info.strength_, resource_info.minimum_clamp_);
}

void CollisionSoundManager::SoundInfo::operator=(const SoundInfo&) {
	deb_assert(false);
}



CollisionSoundManager::CollisionSoundResource::CollisionSoundResource(UiCure::GameUiManager* ui_manager,
	SoundInfo* sound_info):
	Parent(ui_manager, uilepra::SoundManager::kLoopNone),
	sound_info_(sound_info) {
}

void CollisionSoundManager::CollisionSoundResource::operator=(const CollisionSoundResource&) {
	deb_assert(false);
}



loginstance(kGame, CollisionSoundManager);



}
