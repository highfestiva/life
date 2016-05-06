
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"
#include "../../lepra/include/gametimer.h"
#include "../../uitbc/include/uichunkyclass.h"



namespace UiCure {



class BurnEmitter;
class ExhaustEmitter;
class JetEngineEmitter;



class Machine: public CppContextObject {
public:
	typedef CppContextObject Parent;

	Machine(cure::ResourceManager* resource_manager, const str& class_id, GameUiManager* ui_manager);
	virtual ~Machine();
	void SetJetEngineEmitter(JetEngineEmitter* emitter);
	void SetExhaustEmitter(ExhaustEmitter* emitter);
	void SetBurnEmitter(BurnEmitter* emitter);
	BurnEmitter* GetBurnEmitter() const;
	void DeleteEngineSounds();

protected:
	void OnTick();

private:
	void HandleTagAmbientSounds(const uitbc::ChunkyClass::Tag& tag, float real_time_ratio);
	void HandleTagEye(const uitbc::ChunkyClass::Tag& tag, const tbc::PhysicsManager* physics_manager, bool is_child);
	void HandleTagBrakeLight(const uitbc::ChunkyClass::Tag& tag);
	void HandleTagEngineLight(const uitbc::ChunkyClass::Tag& tag, float frame_time);
	void HandleTagBlinkLight(const uitbc::ChunkyClass::Tag& tag, float frame_time);
	void HandleTagEngineSound(const uitbc::ChunkyClass::Tag& tag, const tbc::PhysicsManager* physics_manager, const vec3& velocity,
		float frame_time, float real_time_ratio, size_t& engine_sound_index);
	void HandleTagEngineMeshOffset(const uitbc::ChunkyClass::Tag& tag, float frame_time);
	void HandleTagMeshOffset(const uitbc::ChunkyClass::Tag& tag, float frame_time);
	void HandleTagMeshRotate(const uitbc::ChunkyClass::Tag& tag, float frame_time);

	float CalculateMeshOffset(const str& function, float start_time, float duration, float frequency, float frame_time, float& mesh_time) const;

	void LoadPlaySound3d(UserSound3dResource* sound_resource);

	typedef std::unordered_map<const uitbc::ChunkyClass::Tag*, UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;
	typedef std::vector<float> TagSoundIntensityArray;

	JetEngineEmitter* jet_engine_emitter_;
	ExhaustEmitter* exhaust_emitter_;
	BurnEmitter* burn_emitter_;
	TagSoundTable engine_sound_table_;
	TagSoundIntensityArray engine_sound_intensity_;
	GameTimer ambient_sound_timer_;
	float blink_time_;
	float mesh_offset_time_;
	float mesh_rotate_time_;
	bool prevent_engine_sounds_;

	logclass();
};



}
