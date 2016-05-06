
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/vector3d.h"
#include "../../tbc/include/physicsmanager.h"
#include "../../uicure/include/uiresourcemanager.h"
#include "uicure.h"



namespace cure {
class ContextObject;
class GameManager;
}
namespace tbc {
class ChunkyBoneGeometry;
}



namespace UiCure {



class GameUiManager;



class CollisionSoundManager {
public:
	struct SoundResourceInfo {
		float strength_;
		float minimum_clamp_;
		float pitch_factor_;
		SoundResourceInfo();
		SoundResourceInfo(float strength, float minimum_clamp, float pitch_factor);
	//private:
		//void operator=(const SoundResourceInfo&);
	};

	CollisionSoundManager(cure::GameManager* game_manager, UiCure::GameUiManager* ui_manager);
	virtual ~CollisionSoundManager();
	void SetScale(float small_mass, float light_impact, float impact_volume_factor, float sound_cutoff_duration);

	void AddSound(const str& name, const SoundResourceInfo& info);
	void PreLoadSound(const str& name);

	void Tick(const vec3& camera_position);
	void OnCollision(const vec3& force, const vec3& torque, const vec3& position,
		cure::ContextObject* object1, cure::ContextObject* object2,
		tbc::PhysicsManager::BodyID body1_id, float max_distance, bool is_loud);
	void OnCollision(float impact, const vec3& position, const tbc::ChunkyBoneGeometry* key, const str& sound_name);

private:
	/*struct GeometryCombination {
		tbc::ChunkyBoneGeometry* geometry_;
		GeometryCombination(tbc::ChunkyBoneGeometry* pGeometryA);
		bool operator==(const GeometryCombination& other) const;
	};

	struct GeometryKeyHasher {
		size_t operator()(const GeometryCombination& __x) const {
			union __vp {
				size_t s;
				const void* p;
			};
			__vp vp;
			vp.p = __x.geometry_;
			return vp.s;
		}
	};*/

	struct SoundInfo {
		vec3 position_;
		float base_impact_;
		const SoundResourceInfo resource_info_;
		float volume_;
		float pitch_;
		UiCure::UserSound3dResource* sound_;
		SoundInfo(const SoundResourceInfo& resource_info);
		~SoundInfo();
		void UpdateImpact();
		static float GetVolume(float base_impact, const SoundResourceInfo& resource_info);
	private:
		void operator=(const SoundInfo&);
	};

	struct CollisionSoundResource: public UiCure::UserSound3dResource {
		typedef UiCure::UserSound3dResource Parent;
		SoundInfo* sound_info_;
		CollisionSoundResource(UiCure::GameUiManager* ui_manager, SoundInfo* sound_info);
	private:
		void operator=(const CollisionSoundResource&);
	};

	SoundInfo* GetPlayingSound(const tbc::ChunkyBoneGeometry* geometry_key) const;
	void PlaySound(const tbc::ChunkyBoneGeometry* geometry_key, const str& sound_name, const vec3& position, float impact);
	void OnSoundLoaded(UiCure::UserSound3dResource* sound_resource);
	void OnSoundPreLoaded(UiCure::UserSound3dResource* sound_resource);
	void UpdateSound(SoundInfo* sound_info);
	void StopSound(const tbc::ChunkyBoneGeometry* geometry_key);

	typedef std::unordered_map<const tbc::ChunkyBoneGeometry*, SoundInfo*, LEPRA_VOIDP_HASHER> SoundMap;
	typedef std::unordered_map<str, SoundResourceInfo> SoundNameMap;

	cure::GameManager* game_manager_;
	UiCure::GameUiManager* ui_manager_;
	vec3 camera_position_;
	float small_mass_;
	float light_impact_;
	float impact_volume_factor_;
	float sound_cutoff_duration_;
	SoundMap sound_map_;
	SoundNameMap sound_name_map_;
	mutable Lock lock_;

	logclass();
};



}
