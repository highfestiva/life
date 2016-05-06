
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiparticlerenderer.h"
#include "../../lepra/include/random.h"
#include "../include/uibillboardgeometry.h"

#define RNDCOL(col, lo, hi)		vec3(Random::Uniform(col.x*lo, col.x*hi), Random::Uniform(col.y*lo, col.y*hi), Random::Uniform(col.z*lo, col.z*hi))
#define OPACITY_FADE_IN_TIME_OFFSET	(PIF*0.25f + 0.03f)
#define PARTICLE_TIME			(PIF+OPACITY_FADE_IN_TIME_OFFSET*2)



namespace uitbc {



ParticleRenderer::ParticleRenderer(Renderer* renderer, int max_light_count):
	Parent(renderer),
	lock_(new Lock),
	gravity_(0,0,-9.8f),
	max_light_count_(max_light_count),
	gas_texture_count_(1),
	total_texture_count_(1),
	billboard_gas_(0),
	billboard_shrapnel_(0),
	billboard_spark_(0),
	billboard_glow_(0) {
}

ParticleRenderer::~ParticleRenderer() {
	delete billboard_gas_;
	billboard_gas_ = 0;
	delete billboard_shrapnel_;
	billboard_shrapnel_ = 0;
	delete billboard_spark_;
	billboard_spark_ = 0;
	delete billboard_glow_;
	billboard_glow_ = 0;
	delete lock_;
	lock_ = 0;
}



void ParticleRenderer::SetGravity(vec3 gravity) {
	gravity_ = gravity;
}

void ParticleRenderer::SetData(int gas_texture_count, int total_texture_count, BillboardGeometry* gas, BillboardGeometry* shrapnel, BillboardGeometry* spark, BillboardGeometry* glow) {
	gas_texture_count_ = gas_texture_count;
	total_texture_count_ = total_texture_count;
	deb_assert(total_texture_count_ >= gas_texture_count_);
	deb_assert(total_texture_count_ > 0);
	billboard_gas_ = gas;
	billboard_shrapnel_ = shrapnel;
	billboard_spark_ = spark;
	billboard_glow_ = glow;
}

void ParticleRenderer::Render() {
	if (!billboard_gas_) {
		return;
	}

	ScopeLock lock(lock_);

	const float q = 1;
	BillboardArray::iterator x;
	BillboardRenderInfoArray _billboards;
	x = smokes_.begin();
	for (; x != smokes_.end(); ++x) {
		const float s = (q + x->opacity_time_) * x->size_factor_;
		const float r = Math::Lerp(x->start_color_.x, x->color_.x, x->opacity_time_/PARTICLE_TIME);
		const float g = Math::Lerp(x->start_color_.y, x->color_.y, x->opacity_time_/PARTICLE_TIME);
		const float b = Math::Lerp(x->start_color_.z, x->color_.z, x->opacity_time_/PARTICLE_TIME);
		_billboards.push_back(BillboardRenderInfo(x->angle_, x->position_, s, vec3(r, g, b), x->opacity_, x->texture_index_));
	}
	renderer_->RenderBillboards(billboard_gas_, true, false, _billboards);

	_billboards.clear();
	x = shrapnels_.begin();
	for (; x != shrapnels_.end(); ++x) {
		const float s = x->size_factor_;
		_billboards.push_back(BillboardRenderInfo(x->angle_, x->position_, s, x->color_, x->opacity_, x->texture_index_));
	}
	renderer_->RenderBillboards(billboard_shrapnel_, false, false, _billboards);

	_billboards.clear();
	const quat& cam_orientation_inverse = renderer_->GetCameraOrientationInverse();
	const vec3 camera_xz_plane(0,1,0);	// In cam space, that is.
	x = sparks_.begin();
	for (; x != sparks_.end(); ++x) {
		const vec3 angle_vector = (cam_orientation_inverse * x->velocity_).ProjectOntoPlane(camera_xz_plane);
		const float _angle = PIF/2 - angle_vector.GetPolarCoordAngleY();
		const float s = x->size_factor_;
		const float r  = Math::Lerp(1.0f, 0.6f, x->opacity_time_/PARTICLE_TIME);
		const float gb = Math::Lerp(1.0f, 0.3f, x->opacity_time_/PARTICLE_TIME);
		_billboards.push_back(BillboardRenderInfo(_angle, x->position_, s, vec3(r, gb, gb), x->opacity_, x->texture_index_));
	}
	renderer_->RenderBillboards(billboard_spark_, false, true, _billboards);

	_billboards.clear();
	x = fires_.begin();
	for (; x != fires_.end(); ++x) {
		const float s = (q + x->opacity_time_) * x->size_factor_;
		const float r = Math::Lerp(x->start_color_.x, x->color_.x, x->opacity_time_/PARTICLE_TIME);
		const float g = Math::Lerp(x->start_color_.y, x->color_.y, x->opacity_time_/PARTICLE_TIME);
		const float b = Math::Lerp(x->start_color_.z, x->color_.z, x->opacity_time_/PARTICLE_TIME);
		_billboards.push_back(BillboardRenderInfo(x->angle_, x->position_, s, vec3(r, g, b), x->opacity_, x->texture_index_));
	}
	renderer_->RenderBillboards(billboard_gas_, true, true, _billboards);

	_billboards.clear();
	x = temp_fires_.begin();
	for (; x != temp_fires_.end(); ++x) {
		_billboards.push_back(BillboardRenderInfo(x->angle_, x->position_, x->size_factor_, x->color_, x->opacity_, x->texture_index_));
	}
	renderer_->RenderBillboards(billboard_glow_, true, true, _billboards);

	// Update lights.
	LightArray::iterator y = lights_.begin();
	for (; y < lights_.end(); ++y) {
		if (y->render_light_id_ != Renderer::INVALID_LIGHT) {
			renderer_->SetLightPosition(y->render_light_id_, y->position_);
			renderer_->SetLightColor(y->render_light_id_, vec3(0.6f, 0.4f, 0.2f) * y->strength_ * 30.0f);
			deb_assert(y->strength_ >= 0 && y->strength_ < 1000);
		}
	}
}

void ParticleRenderer::Tick(float time) {
	ScopeLock lock(lock_);

	StepLights(time, 6);

	StepBillboards(fires_, time, 6);
	StepBillboards(smokes_, time, 5);
	StepBillboards(sparks_, time, -0.4f);
	StepBillboards(shrapnels_, time, -0.2f);
	temp_fires_.clear();
}

void ParticleRenderer::CreateFlare(const vec3& color, float strength, float time_factor, const vec3& position, const vec3& velocity) {
	ScopeLock lock(lock_);
	CreateTempLight(color, strength, position, velocity, velocity, time_factor);
}

void ParticleRenderer::CreateExplosion(const vec3& position, float strength, const vec3& velocity, float falloff, float time, const vec3& start_fire_color, const vec3& fire_color,
	const vec3& start_smoke_color, const vec3& smoke_color, const vec3& sharpnel_color, int fires, int smokes, int sparks, int shrapnels) {
	ScopeLock lock(lock_);

	const float random_xy_end_speed = 1.0f;
	const float strength2 = (strength>1)? ::sqrt(strength) : strength*strength;
	const float particle_size = strength2*0.5f;
	const float speed = velocity.GetLength() * 0.01f;
	CreateBillboards(position,  7*strength2+ 1*speed,      velocity, Math::Lerp(velocity,gravity_*-0.2f,falloff), random_xy_end_speed, 5.3f/time, particle_size,      start_fire_color, fire_color, fires_, fires);
	CreateBillboards(position,  8*strength2+ 1*speed,      velocity, Math::Lerp(velocity,gravity_*+0.2f,falloff), random_xy_end_speed,    3/time, particle_size*2,    start_smoke_color, smoke_color, smokes_, smokes);
	CreateBillboards(position, 20*strength2+20*speed, 1.2f*velocity, Math::Lerp(velocity,gravity_*+0.8f,falloff), random_xy_end_speed, 4.5f/time, particle_size*0.4f, vec3(), vec3(), sparks_, sparks);
	CreateBillboards(position,  9*strength2+10*speed, 1.1f*velocity, Math::Lerp(velocity,gravity_*+1.1f,falloff), random_xy_end_speed, 1.1f/time, particle_size*0.5f, sharpnel_color, sharpnel_color, shrapnels_, shrapnels);

	const float min_spark_velocity2 = strength2*100;
	const vec3 cam_plane = renderer_->GetCameraTransformation().GetOrientation() * vec3(0,1,0);
	BillboardArray::reverse_iterator x = sparks_.rbegin();
	for (int y = 0; y < sparks; ++y, ++x) {
		x->velocity_ = x->velocity_.ProjectOntoPlane(cam_plane);
		float speed2 = x->velocity_.GetLengthSquared();
		if (speed2 < min_spark_velocity2) {
			x->velocity_.Mul(::sqrt(min_spark_velocity2/speed2));
		}
	}

	if (fires > 0) {
		const Billboard& fire = fires_.back();
		CreateTempLight(fire_color, strength, fire.position_, fire.velocity_, fire.target_velocity_, fire.time_factor_);
	}
}

void ParticleRenderer::CreatePebble(float time, float scale, float angular_velocity, const vec3& color, const vec3& position, const vec3& velocity) {
	const float _time_factor = PARTICLE_TIME*0.5f/time;	// Split in two, as we're only using latter half of sine curve (don't fade into existance).
	const float random_xy_end_speed = 1.0f;

	ScopeLock lock(lock_);

	CreateBillboards(position, 0, vec3(), vec3(0,0,-10), random_xy_end_speed, _time_factor, scale*0.1f, vec3(), color, shrapnels_, 1);
	Billboard& pebble_billboard = shrapnels_.back();
	pebble_billboard.velocity_ = velocity;
	pebble_billboard.angular_velocity_ = Random::Uniform(-angular_velocity, +angular_velocity);
	pebble_billboard.opacity_ = 1;
	pebble_billboard.opacity_time_ += PIF/2;	// Move to "fully opaque" time in sine curve.
}

void ParticleRenderer::CreateFume(float time, float scale, float angular_velocity, float opacity, const vec3& position, const vec3& velocity) {
	const float _time_factor = PARTICLE_TIME/time;
	const float random_xy_end_speed = 0.5f;

	ScopeLock lock(lock_);

	CreateBillboards(position, 0, vec3(), vec3(0,0,2), random_xy_end_speed, _time_factor, scale*0.1f, vec3(0.4f,0.4f,0.4f), vec3(), smokes_, 1);
	Billboard& pebble_billboard = smokes_.back();
	pebble_billboard.velocity_ = velocity;
	pebble_billboard.angular_velocity_ = Random::Uniform(-angular_velocity, +angular_velocity);
	pebble_billboard.opacity_factor_ = opacity;
	//pebble_billboard.opacity_time_ += OPACITY_FADE_IN_TIME_OFFSET * 0.5f;	// Move forward some in time to ensure that we're very visible when we come out of the pipe.
}

void ParticleRenderer::CreateGlow(float time, float scale, const vec3& start_color, const vec3& color, float opacity, const vec3& position, const vec3& velocity) {
	ScopeLock lock(lock_);

	const float _time_factor = PARTICLE_TIME/time;
	CreateBillboards(position, 0, vec3(), vec3(), 0, _time_factor, scale, start_color, color, fires_, 1);
	Billboard& glow_billboard = fires_.back();
	glow_billboard.opacity_factor_ = opacity;
	glow_billboard.velocity_ = velocity;
	glow_billboard.target_velocity_ = velocity;
}

void ParticleRenderer::RenderFireBillboard(float angle, float _size, const vec3& color, float opacity, const vec3& position) {
	ScopeLock lock(lock_);

	temp_fires_.push_back(Billboard());
	Billboard& billboard = temp_fires_.back();
	billboard.position_ = position;
	billboard.color_ = color;
	billboard.texture_index_ = total_texture_count_-1;
	billboard.depth_ = 1000.0f;
	billboard.size_factor_ = _size;
	billboard.angle_ = angle;
	billboard.opacity_ = opacity;
}



void ParticleRenderer::CreateTempLight(const vec3& color, float strength, const vec3& position, const vec3& velocity, const vec3& target_velocity, float time_factor) {
	if (lights_.size() < max_light_count_) {
		lights_.push_back(Light(color, strength, position, velocity, target_velocity, time_factor));
		//log_.Infof("Creating new light with strength %f", strength);
	} else {
		int darkest_light_index = -1;
		float darkest_light_strength = 1e15f;
		int i = 0;
		LightArray::iterator x = lights_.begin();
		for (; x < lights_.end(); ++x, ++i) {
			if (x->strength_ < darkest_light_strength) {
				darkest_light_index = i;
				darkest_light_strength = x->strength_;
			}
		}
		if (strength >= darkest_light_strength && darkest_light_index >= 0) {
			//mLog.Infof("Overtaking light with render ID %i (had strength %f, got strength %f"), mLights[lDarkestLightIndex].mRenderLightId, mLights[lDarkestLightIndex].mStrength, pStrength);
			// TRICKY: don't overwrite! We must not leak the previosly allocated hardware light!
			lights_[darkest_light_index].color_ = color;
			lights_[darkest_light_index].strength_ = strength;
			lights_[darkest_light_index].position_ = position;
			lights_[darkest_light_index].velocity_ = velocity;
			lights_[darkest_light_index].target_velocity_ = target_velocity;
			lights_[darkest_light_index].time_factor_ = time_factor;
		}
	}
}

void ParticleRenderer::StepLights(float time, float friction) {
	(void)friction;

	LightArray::iterator x = lights_.begin();
	while (x != lights_.end()) {
		x->velocity_ = Math::Lerp(x->velocity_, x->target_velocity_, friction*time);
		x->position_ += x->velocity_ * time;

		x->strength_ -= time * x->time_factor_ * 2 / PIF;
		if (x->strength_ <= 0.1f) {
			if (x->render_light_id_ != Renderer::INVALID_LIGHT) {
				//log_.Infof("Dropping light with render ID %i.", x->render_light_id_);
				renderer_->RemoveLight(x->render_light_id_);
			}
			x = lights_.erase(x);
		} else {
			if (x->render_light_id_ == Renderer::INVALID_LIGHT) {
				x->render_light_id_ = renderer_->AddPointLight(Renderer::kLightMovable, x->position_, x->color_*x->strength_*10, x->strength_*10, 0);
				//log_.Infof("Creating render ID %i for light with strength %f", x->render_light_id_, x->strength_);
			}
			++x;
		}
	}
}

void ParticleRenderer::CreateBillboards(const vec3& position, float strength, const vec3& velocity, const vec3& target_velocity,
	float end_turbulence, float time_factor, float size_factor, const vec3& start_color, const vec3& color, BillboardArray& billboards, int count) {
	for (int x = 0; x < count; ++x) {
		billboards.push_back(Billboard());
		Billboard& billboard = billboards.back();
		billboard.velocity_ = velocity + RNDVEC(strength);
		const vec3 this_particles_target_velocity = Math::Lerp(target_velocity*0.8f, target_velocity, Random::Uniform(0.0f, 2.0f));
		billboard.target_velocity_ = RNDVEC(end_turbulence) + this_particles_target_velocity;
		billboard.position_ = position;
		billboard.start_color_ = RNDCOL(start_color, 0.9f, 1.1f);
		billboard.color_ = RNDCOL(color, 0.7f, 1.3f);
		billboard.texture_index_ = Random::GetRandomNumber() % gas_texture_count_;
		billboard.depth_ = 1000.0f;
		billboard.size_factor_ = Random::Uniform(size_factor*0.7f, size_factor*1.4f);
		billboard.angle_ = Random::Uniform(0.0f, 2*PIF);
		billboard.angular_velocity_ = Random::Uniform(-5.0f, +5.0f);
		billboard.opacity_ = 0;
		billboard.opacity_factor_ = 1;
		billboard.opacity_time_ = Random::Uniform(0.0f, 0.3f);
		deb_assert(time_factor > 0);
		billboard.time_factor_ = Random::Uniform(time_factor*0.7f, time_factor*1.3f);
	}
}

void ParticleRenderer::StepBillboards(BillboardArray& billboards, float time, float friction) {
	BillboardArray::iterator x = billboards.begin();
	while (x != billboards.end()) {
		if (friction > 0) {
			x->velocity_ = Math::Lerp(x->velocity_, x->target_velocity_, friction*time);
			x->angular_velocity_ = Math::Lerp(x->angular_velocity_, x->angular_velocity_ * 0.7f, friction*time);
		} else {
			x->velocity_ *= 1 + friction*time;
			x->velocity_ += x->target_velocity_ * time;
		}
		x->position_ += x->velocity_ * time;
		x->angle_ += x->angular_velocity_ * time;

		x->opacity_time_ += time * x->time_factor_;
		x->opacity_ = (::sin(x->opacity_time_) + 0.7f) * x->opacity_factor_;
		if (x->opacity_ <= 0.0f || x->opacity_time_ > PARTICLE_TIME) {
			//deb_assert(x->opacity_time_ > PIF);	// Verify that the particle was visible at all, or the algo's wrong.
			x = billboards.erase(x);
		} else {
			++x;
		}
	}
}



loginstance(kUiGfx3D, ParticleRenderer);



}
