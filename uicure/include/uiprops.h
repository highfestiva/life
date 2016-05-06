
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uicppcontextobject.h"



namespace UiCure {



class Props: public CppContextObject {
public:
	typedef CppContextObject Parent;

	enum ParticleType {
		kParticleNone = 1,
		kParticleSolid,
		kParticleGas,
	};

	Props(cure::ResourceManager* resource_manager, const str& class_id, GameUiManager* ui_manager);
	virtual ~Props();

	void SetOpacity(float opacity);
	void StartParticle(ParticleType particle_type, const vec3& start_velocity, float scale, float angular_range, float time);
	void SetFadeOutTime(float time);

protected:
	void DispatchOnLoadMesh(UserGeometryReferenceResource* mesh_resource);
	virtual void TryAddTexture();
	void OnTick();
	void OnAlarm(int alarm_id, void* extra_data);

private:
	vec3 velocity_;
	ParticleType particle_type_;
	float scale_;
	float time_;
	float life_time_;
	float fade_out_time_;
	float opacity_;
	bool is_fading_out_;
	vec3 angular_velocity_;

	logclass();
};



}
