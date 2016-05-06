
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "uidynamicrenderer.h"
#include "../../lepra/include/thread.h"
#include "uirenderer.h"



namespace uitbc {



class BillboardGeometry;



class ParticleRenderer: public DynamicRenderer {
	typedef DynamicRenderer Parent;
public:
	ParticleRenderer(Renderer* renderer, int max_light_count);
	virtual ~ParticleRenderer();

	void SetGravity(vec3 gravity);
	void SetData(int gas_texture_count, int total_texture_count, BillboardGeometry* gas, BillboardGeometry* shrapnel, BillboardGeometry* spark, BillboardGeometry* glow);

	virtual void Render();
	virtual void Tick(float time);

	void CreateFlare(const vec3& color, float strength, float time_factor, const vec3& position, const vec3& velocity);
	void CreateExplosion(const vec3& position, float strength, const vec3& velocity, float falloff, float time, const vec3& start_fire_color, const vec3& fire_color,
		const vec3& start_smoke_color, const vec3& smoke_color, const vec3& sharpnel_color, int fires, int smokes, int sparks, int shrapnels);
	void CreatePebble(float time, float scale, float angular_velocity, const vec3& color, const vec3& position, const vec3& velocity);
	void CreateFume(float time, float scale, float angular_velocity, float opacity, const vec3& position, const vec3& velocity);
	void CreateGlow(float time, float scale, const vec3& start_color, const vec3& color, float opacity, const vec3& position, const vec3& velocity);
	void RenderFireBillboard(float angle, float size, const vec3& color, float opacity, const vec3& position);

protected:
	struct Light {
		Light(const vec3& color, float strength, const vec3& position, const vec3& velocity, const vec3& target_velocity, float time_factor):
			color_(color),
			position_(position),
			velocity_(velocity),
			target_velocity_(target_velocity),
			strength_(strength),
			time_factor_(time_factor),
			render_light_id_(Renderer::INVALID_LIGHT) {
		}
		vec3 color_;
		vec3 position_;
		vec3 velocity_;
		vec3 target_velocity_;
		float strength_;
		float time_factor_;
		Renderer::LightID render_light_id_;
	};
	typedef std::vector<Light> LightArray;

	struct Billboard {
		vec3 position_;
		vec3 velocity_;
		vec3 target_velocity_;
		vec3 start_color_;
		vec3 color_;
		int texture_index_;
		float size_factor_;
		float depth_;
		float angle_;
		float angular_velocity_;
		float opacity_;
		float opacity_factor_;
		float opacity_time_;
		float time_factor_;
	};
	typedef std::vector<Billboard> BillboardArray;

	void CreateTempLight(const vec3& color, float strength, const vec3& position, const vec3& velocity, const vec3& target_velocity, float time_factor);
	void StepLights(float time, float friction);

	void CreateBillboards(const vec3& position, float strength, const vec3& velocity, const vec3& target_velocity,
		float end_turbulence, float time_factor, float size_factor, const vec3& start_color, const vec3& color,
		BillboardArray& billboards, int count);
	static void StepBillboards(BillboardArray& billboards, float time, float friction);

	Lock* lock_;
	vec3 gravity_;
	LightArray lights_;
	size_t max_light_count_;
	size_t gas_texture_count_;
	size_t total_texture_count_;
	BillboardGeometry* billboard_gas_;
	BillboardGeometry* billboard_shrapnel_;
	BillboardGeometry* billboard_spark_;
	BillboardGeometry* billboard_glow_;

	BillboardArray fires_;
	BillboardArray smokes_;
	BillboardArray sparks_;
	BillboardArray shrapnels_;
	BillboardArray temp_fires_;

	logclass();
};



}
