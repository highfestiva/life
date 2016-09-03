
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uimachine.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/health.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/hashutil.h"
#include "../../lepra/include/random.h"
#include "../../tbc/include/chunkybonegeometry.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../tbc/include/physicsengine.h"
#include "../include/uiburnemitter.h"
#include "../include/uiexhaustemitter.h"
#include "../include/uigameuimanager.h"
#include "../include/uijetengineemitter.h"
#include "../include/uiprops.h"
#include "../include/uiruntimevariablename.h"
#include "../include/uisoundreleaser.h"



namespace UiCure {



Machine::Machine(cure::ResourceManager* resource_manager, const str& class_id, GameUiManager* ui_manager):
	Parent(resource_manager, class_id, ui_manager),
	jet_engine_emitter_(0),
	exhaust_emitter_(0),
	burn_emitter_(0),
	blink_time_(0),
	mesh_offset_time_(0),
	mesh_rotate_time_(0),
	prevent_engine_sounds_(false) {
	EnableMeshSlide(true);
}

Machine::~Machine() {
	DeleteEngineSounds();
	delete jet_engine_emitter_;
	jet_engine_emitter_ = 0;
	delete exhaust_emitter_;
	exhaust_emitter_ = 0;
	delete burn_emitter_;
	burn_emitter_ = 0;
}

void Machine::SetJetEngineEmitter(JetEngineEmitter* emitter) {
	jet_engine_emitter_ = emitter;
}

void Machine::SetExhaustEmitter(ExhaustEmitter* emitter) {
	exhaust_emitter_ = emitter;
}

void Machine::SetBurnEmitter(BurnEmitter* emitter) {
	burn_emitter_ = emitter;
}

BurnEmitter* Machine::GetBurnEmitter() const {
	return burn_emitter_;
}

void Machine::DeleteEngineSounds() {
	prevent_engine_sounds_ = true;
	TagSoundTable::iterator x = engine_sound_table_.begin();
	for (; x != engine_sound_table_.end(); ++x) {
		delete (x->second);
	}
	engine_sound_table_.clear();
}



void Machine::OnTick() {
	Parent::OnTick();

	const tbc::ChunkyPhysics* physics = GetPhysics();
	const tbc::ChunkyClass* clazz = GetClass();
	if (!physics || !clazz) {
		return;
	}
	if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId())) {
		return;
	}

	static const str float_childishness("float_childishness");
	static const str ambient_sounds("ambient_sounds");
	static const str eye("eye");
	static const str brake_light("brake_light");
	static const str engine_light("engine_light");
	static const str blink_light("blink_light");
	static const str jet_engine_emitter("jet_engine_emitter");
	static const str engine_sound("engine_sound");
	static const str engine_mesh_offset("engine_mesh_offset");
	static const str mesh_offset("mesh_offset");
	static const str mesh_rotate("mesh_rotate");
	static const str exhaust("exhaust");
	static const str burn("burn");

	const cure::TimeManager* time_manager = GetManager()->GetGameManager()->GetTimeManager();
	float _real_time_ratio = time_manager->GetRealTimeRatio();
	const float _frame_time = time_manager->GetNormalGameFrameTime();
	const bool _is_child = IsAttributeTrue(float_childishness);
	const tbc::PhysicsManager* _physics_manager = manager_->GetGameManager()->GetPhysicsManager();
	vec3 _velocity = GetVelocity();
	size_t _engine_sound_index = 0;
	for (size_t x = 0; x < clazz->GetTagCount(); ++x) {
		const uitbc::ChunkyClass::Tag& _tag = clazz->GetTag(x);
		if (_tag.tag_name_ == ambient_sounds) {
			HandleTagAmbientSounds(_tag, _real_time_ratio);
		} else if (_tag.tag_name_ == eye) {
			HandleTagEye(_tag, _physics_manager, _is_child);
		} else if (_tag.tag_name_ == brake_light) {
			HandleTagBrakeLight(_tag);
		} else if (_tag.tag_name_ == engine_light) {
			HandleTagEngineLight(_tag, _frame_time*_real_time_ratio);
		} else if (_tag.tag_name_ == blink_light) {
			HandleTagBlinkLight(_tag, _frame_time*_real_time_ratio);
		} else if (_tag.tag_name_ == jet_engine_emitter) {
			// Faijah!
			if (jet_engine_emitter_) {
				jet_engine_emitter_->EmitFromTag(this, _tag, _frame_time*_real_time_ratio);
			}
		} else if (_tag.tag_name_ == engine_sound) {
			HandleTagEngineSound(_tag, _physics_manager, _velocity, _frame_time*_real_time_ratio, _real_time_ratio, _engine_sound_index);
		} else if (_tag.tag_name_ == engine_mesh_offset) {
			HandleTagEngineMeshOffset(_tag, _frame_time);
		} else if (_tag.tag_name_ == mesh_offset) {
			HandleTagMeshOffset(_tag, _frame_time*_real_time_ratio);
		} else if (_tag.tag_name_ == mesh_rotate) {
			HandleTagMeshRotate(_tag, _frame_time*_real_time_ratio);
		} else if (_tag.tag_name_ == exhaust) {
			// Particles coming out of exhaust.
			if (exhaust_emitter_) {
				exhaust_emitter_->EmitFromTag(this, _tag, _frame_time);
			}
		} else if (_tag.tag_name_ == burn) {
			// Particles caused by burning.
			if (burn_emitter_) {
				const float health = Math::Clamp(cure::Health::Get(this), 0.0f, 1.0f);
				burn_emitter_->EmitFromTag(this, _tag, _frame_time, (-health + 0.7f)/0.7f);
			}
		}
	}

	if (_is_child || physics->GetGuideMode() == tbc::ChunkyPhysics::kGuideAlways) {
		StabilizeTick();
	}
}



void Machine::HandleTagAmbientSounds(const uitbc::ChunkyClass::Tag& tag, float real_time_ratio) {
	if (tag.float_value_list_.size() != 6 ||
		tag.string_value_list_.size() < 1 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 0 ||
		tag.mesh_index_list_.size() != 0) {
		log_.Errorf("The ambient_sounds tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	enum {
		kMinDelay,	MAX_DELAY,
		kMinPitch,	MAX_PITCH,
		kMinVolume,	MAX_VOLUME,
	};
	if (!ambient_sound_timer_.IsStarted()) {
		ambient_sound_timer_.Start(-tag.float_value_list_[kMinDelay]);
	} else if (ambient_sound_timer_.QueryTimeDiff() >= 0) {
		ambient_sound_timer_.Start(-Random::Uniform(tag.float_value_list_[kMinDelay], tag.float_value_list_[MAX_DELAY]));
		const size_t random_sound_index = Random::GetRandomNumber() % tag.string_value_list_.size();
		const str sound_name = tag.string_value_list_[random_sound_index];
		UiCure::UserSound2dResource* sound = new UiCure::UserSound2dResource(GetUiManager(), uilepra::SoundManager::kLoopNone);
		const float pitch = Random::Uniform(tag.float_value_list_[kMinPitch], tag.float_value_list_[MAX_PITCH]);
		const float volume = Random::Uniform(tag.float_value_list_[kMinVolume], tag.float_value_list_[MAX_VOLUME]);
		new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetManager(), sound_name, sound, volume, pitch*real_time_ratio);
	}
}

void Machine::HandleTagEye(const uitbc::ChunkyClass::Tag& tag, const tbc::PhysicsManager* physics_manager, bool is_child) {
	// Eyes follow steered wheels. Get wheel corresponding to eye and
	// move eye accordingly á là Lightning McQueen.

	if (tag.float_value_list_.size() != 1 ||
		tag.string_value_list_.size() != 0 ||
		tag.body_index_list_.size()+tag.engine_index_list_.size() != 1 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The eye tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}

	float joint_value = 0;
	if (!tag.body_index_list_.empty()) {
		const int body_index = tag.body_index_list_[0];
		tbc::ChunkyBoneGeometry* bone = GetPhysics()->GetBoneGeometry(body_index);
		tbc::PhysicsManager::JointID joint = bone->GetJointId();
		switch (bone->GetJointType()) {
			case tbc::ChunkyBoneGeometry::kJointHinge2: {
				tbc::PhysicsManager::Joint3Diff diff;
				physics_manager->GetJoint3Diff(bone->GetBodyId(), joint, diff);
				float low_stop = 0;
				float high_stop = 0;
				float bounce = 0;
				physics_manager->GetJointParams(joint, low_stop, high_stop, bounce);
				joint_value = diff.angle1_ * 2 / (high_stop-low_stop);
			} break;
			case tbc::ChunkyBoneGeometry::kJointHinge: {
				tbc::PhysicsManager::Joint1Diff diff;
				physics_manager->GetJoint1Diff(bone->GetBodyId(), joint, diff);
				float low_stop = 0;
				float high_stop = 0;
				float bounce = 0;
				physics_manager->GetJointParams(joint, low_stop, high_stop, bounce);
				joint_value = diff.value_ * 2 / (high_stop-low_stop);
			} break;
			case tbc::ChunkyBoneGeometry::kJointExclude: {
				// Simple, dead eyes.
				joint_value = 0;
			} break;
			default: {
				log_.Errorf("Joint type %i not implemented for tag type %s.", bone->GetJointType(), tag.tag_name_.c_str());
				deb_assert(false);
			} break;
		}
	} else {
		const int engine_index = tag.engine_index_list_[0];
		if (engine_index >= physics_->GetEngineCount()) {
			return;
		}
		tbc::PhysicsEngine* engine = GetPhysics()->GetEngine(engine_index);
		joint_value = engine->GetLerpThrottle(0.1f, 0.1f, false);
	}
	const float scale = tag.float_value_list_[0];
	const float joint_right_value = joint_value * scale;
	const float joint_down_value = (::cos(joint_value)-1) * scale * 0.5f;
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (mesh) {
			xform transform = mesh->GetBaseTransformation();
			transform.MoveRight(joint_right_value);
			transform.MoveBackward(joint_down_value);
			mesh->SetTransformation(transform);
			mesh->SetTransformationChanged(true);
			mesh->SetAlwaysVisible(is_child);
		}
	}
}

void Machine::HandleTagBrakeLight(const uitbc::ChunkyClass::Tag& tag) {
	if (tag.float_value_list_.size() != 3 ||
		tag.string_value_list_.size() != 0 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 1 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The brake_light tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	const int engine_index = tag.engine_index_list_[0];
	if (engine_index >= physics_->GetEngineCount()) {
		return;
	}
	const tbc::PhysicsEngine* engine = physics_->GetEngine(engine_index);
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (mesh) {
			vec3& ambient = mesh->GetBasicMaterialSettings().ambient_;
			if (::fabs(engine->GetValue()) > 0.001f) {
				ambient.Set(tag.float_value_list_[0], tag.float_value_list_[1], tag.float_value_list_[2]);
			} else {
				ambient.Set(0, 0, 0);
			}
		}
	}
}

void Machine::HandleTagEngineLight(const uitbc::ChunkyClass::Tag& tag, float frame_time) {
	if (tag.float_value_list_.size() != 2 ||
		tag.string_value_list_.size() != 0 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 1 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The engine_light tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	const int engine_index = tag.engine_index_list_[0];
	if (engine_index >= physics_->GetEngineCount()) {
		return;
	}
	const float glow_factor = tag.float_value_list_[0];
	const float throttle_up_speed = Math::GetIterateLerpTime(tag.float_value_list_[1]*0.5f, frame_time);
	const float throttle_down_speed = Math::GetIterateLerpTime(tag.float_value_list_[1], frame_time);
	const tbc::PhysicsEngine* engine = physics_->GetEngine(engine_index);
	const float engine_throttle = engine->GetLerpThrottle(throttle_up_speed, throttle_down_speed, true);
	const float ambient_channel = Math::Lerp(glow_factor, 1.0f, engine_throttle);
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (mesh) {
			mesh->GetBasicMaterialSettings().ambient_.Set(ambient_channel, ambient_channel, ambient_channel);
		}
	}
}

void Machine::HandleTagBlinkLight(const uitbc::ChunkyClass::Tag& tag, float frame_time) {
	if (tag.float_value_list_.size() != 9 ||
		tag.string_value_list_.size() != 0 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 0 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The blink_light tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	enum {
		kROff = 0, R_AMP, R_SPEED,
		kGOff,     G_AMP, G_SPEED,
		kBOff,     B_AMP, B_SPEED,
	};
#define V(i)	tag.float_value_list_[i]
	const float r = V(kROff) + V(R_AMP) * sin(blink_time_*2*PIF*V(R_SPEED));
	const float g = V(kGOff) + V(G_AMP) * sin(blink_time_*2*PIF*V(G_SPEED));
	const float b = V(kBOff) + V(B_AMP) * sin(blink_time_*2*PIF*V(B_SPEED));
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (mesh) {
			mesh->GetBasicMaterialSettings().ambient_.Set(r, g, b);
		}
	}
	blink_time_ += frame_time;
}

void Machine::HandleTagEngineSound(const uitbc::ChunkyClass::Tag& tag, const tbc::PhysicsManager* physics_manager, const vec3& velocity,
	float frame_time, float real_time_ratio, size_t& engine_sound_index) {
	// Sound controlled by engine.
	if (prevent_engine_sounds_) {
		return;
	}
	if (tag.float_value_list_.size() != 1+9+tag.engine_index_list_.size() ||
		tag.string_value_list_.size() != 1 ||
		tag.body_index_list_.size() != 1 ||
		tag.engine_index_list_.size() < 1 ||
		tag.mesh_index_list_.size() != 0) {
		log_.Errorf("The engine_sound tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}
	UserSound3dResource* engine_sound = HashUtil::FindMapObject(engine_sound_table_, &tag);
	if (engine_sound == 0) {
		const str sound_name = tag.string_value_list_[0];
		engine_sound = new UserSound3dResource(GetUiManager(), uilepra::SoundManager::kLoopForward);
		engine_sound_table_.insert(TagSoundTable::value_type(&tag, engine_sound));
		engine_sound->Load(GetResourceManager(), sound_name,
			UserSound3dResource::TypeLoadCallback(this, &Machine::LoadPlaySound3d));
	}
	if (engine_sound->GetLoadState() != cure::kResourceLoadComplete) {
		return;
	}

	int body_index = tag.body_index_list_[0];
	tbc::ChunkyBoneGeometry* bone = GetPhysics()->GetBoneGeometry(body_index);
	const vec3 position = physics_manager->GetBodyPosition(bone->GetBodyId());

	enum FloatValue {
		kFvThrottleFactor = 0,
		kFvPitchLow,
		kFvPitchHigh,
		kFvPitchExponent,
		kFvVolumeLow,
		kFvVolumeHigh,
		kFvVolumeExponent,
		kFvIntensityLow,
		kFvIntensityHigh,
		kFvIntensityExponent,
		kFvEngineFactorBase,
	};
	const float throttle_up_speed = Math::GetIterateLerpTime(0.05f, frame_time);
	const float throttle_down_speed = Math::GetIterateLerpTime(0.02f, frame_time);
	float intensity = 0;
	float low_volume = tag.float_value_list_[kFvVolumeLow];
	for (size_t y = 0; y < tag.engine_index_list_.size(); ++y) {
		const int engine_index = tag.engine_index_list_[y];
		if (engine_index >= physics_->GetEngineCount()) {
			continue;
		}
		const tbc::PhysicsEngine* engine = physics_->GetEngine(engine_index);
		float engine_intensity = Math::Clamp(engine->GetIntensity(), 0.0f, 1.0f);
		if (tag.float_value_list_[kFvThrottleFactor] > 0) {
			const float throttle = engine->GetLerpThrottle(throttle_up_speed, throttle_down_speed, true);
			engine_intensity *= Math::Lerp(1.0f, throttle, tag.float_value_list_[kFvThrottleFactor]);
			const float carburetor_sound = std::min(std::fabs(engine->GetValue()) * 0.7f, 1.0f);
			low_volume = Math::Lerp(low_volume, tag.float_value_list_[kFvVolumeHigh], carburetor_sound);
		}
		engine_intensity *= tag.float_value_list_[kFvEngineFactorBase+y];
		intensity += engine_intensity;
	}
	if (tag.float_value_list_[kFvThrottleFactor] <= 0) {
		// If motor is on/off type (electric for instance), we smooth out the
		// intensity, or it will become very jerky as wheels wobble along.
		if (engine_sound_intensity_.size() <= engine_sound_index) {
			engine_sound_intensity_.resize(engine_sound_index+1);
		}
		const float smooth = std::min(frame_time*8.0f, 0.5f);
		intensity = engine_sound_intensity_[engine_sound_index] = Math::Lerp(engine_sound_intensity_[engine_sound_index], intensity, smooth);
		++engine_sound_index;
	}
	//intensity = Math::Clamp(intensity, 0, 1);
	const float volume_lerp = ::pow(intensity, tag.float_value_list_[kFvVolumeExponent]);
	const float volume = Math::Lerp(low_volume, tag.float_value_list_[kFvVolumeHigh], volume_lerp);
	const float pitch_exp = tag.float_value_list_[kFvPitchExponent];
	const float pitch_lerp = ::pow(intensity, pitch_exp);
	const float pitch = Math::Lerp(tag.float_value_list_[kFvPitchLow], tag.float_value_list_[kFvPitchHigh], pitch_lerp);
	const float rtr_pitch = (real_time_ratio > 1)? ::pow(real_time_ratio, pitch_exp * pitch_exp) : real_time_ratio;
	const uilepra::SoundManager::SoundInstanceID sound_id = engine_sound->GetData();
	ui_manager_->GetSoundManager()->SetSoundPosition(sound_id, position, velocity);
	ui_manager_->GetSoundManager()->SetVolume(sound_id, volume);
	ui_manager_->GetSoundManager()->SetPitch(sound_id, pitch * rtr_pitch);
}

void Machine::HandleTagEngineMeshOffset(const uitbc::ChunkyClass::Tag& tag, float frame_time) {
	// Mesh offset controlled by engine.

	if (tag.float_value_list_.size() != 10 ||
		tag.string_value_list_.size() != 0 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 1 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The engine_mesh_offset tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}

	int engine_index = tag.engine_index_list_[0];
	if (engine_index >= GetPhysics()->GetEngineCount()) {
		return;
	}

	enum FloatValue {
		kFvX = 0,
		kFvY,
		kFvZ,
		kFvRotationAxisX,
		kFvRotationAxisY,
		kFvRotationAxisZ,
		kFvRotationAngle,
		kFvInertia,
		kFvPrimaryFactor,
		kFvSecondaryFactor,
	};

	tbc::PhysicsEngine* engine = GetPhysics()->GetEngine(engine_index);
	const float engine_factor = engine->GetValues()[tbc::PhysicsEngine::kAspectPrimary] * tag.float_value_list_[kFvPrimaryFactor] +
			engine->GetValues()[tbc::PhysicsEngine::kAspectSecondary] * tag.float_value_list_[kFvSecondaryFactor];
	const float engine_abs_factor = std::abs(engine_factor);

	const vec3 offset_position(vec3(tag.float_value_list_[kFvX], tag.float_value_list_[kFvY], tag.float_value_list_[kFvZ]) * engine_abs_factor);
	const float a = tag.float_value_list_[kFvRotationAngle] * engine_factor;
	quat offset_orientation(a, vec3(tag.float_value_list_[kFvRotationAxisX], tag.float_value_list_[kFvRotationAxisY], tag.float_value_list_[kFvRotationAxisZ]));
	const xform offset(offset_orientation, offset_position);
	const float t = Math::GetIterateLerpTime(1/tag.float_value_list_[kFvInertia], frame_time);

	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (!mesh) {
			continue;
		}
		tbc::GeometryReference* mesh_ref = (tbc::GeometryReference*)mesh;
		xform current_offset;
		current_offset.Interpolate(mesh_ref->GetExtraOffsetTransformation(), offset, t);
		mesh_ref->SetExtraOffsetTransformation(current_offset);
	}
}

void Machine::HandleTagMeshOffset(const uitbc::ChunkyClass::Tag& tag, float frame_time) {
	// Mesh offset controlled by engine.

	if (tag.float_value_list_.size() != 6 ||
		tag.string_value_list_.size() != 1 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 0 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The mesh_offset tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}

	const str _function = tag.string_value_list_[0];
	enum FloatValue {
		kFvInitialDelay,
		kFvDuration,
		kFvFrequency,
		kFvX,
		kFvY,
		kFvZ,
	};
#define V(i)	tag.float_value_list_[i]

	const float start_time = V(kFvInitialDelay);
	float amplitude = CalculateMeshOffset(_function, start_time, V(kFvDuration), V(kFvFrequency), frame_time, mesh_offset_time_);

	const vec3 offset_position(GetOrientation().GetInverseRotatedVector(vec3(V(kFvX), V(kFvY), V(kFvZ)) * amplitude));
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (!mesh) {
			continue;
		}
		tbc::GeometryReference* mesh_ref = (tbc::GeometryReference*)mesh;
		xform extra_offset = mesh_ref->GetExtraOffsetTransformation();
		extra_offset.position_ = offset_position;
		mesh_ref->SetExtraOffsetTransformation(extra_offset);
	}
}

void Machine::HandleTagMeshRotate(const uitbc::ChunkyClass::Tag& tag, float frame_time) {
	// Mesh offset controlled by engine.

	if (tag.float_value_list_.size() != 6 ||
		tag.string_value_list_.size() != 1 ||
		tag.body_index_list_.size() != 0 ||
		tag.engine_index_list_.size() != 0 ||
		tag.mesh_index_list_.size() < 1) {
		log_.Errorf("The mesh_rotate tag '%s' has the wrong # of parameters.", tag.tag_name_.c_str());
		deb_assert(false);
		return;
	}

	const str _function = tag.string_value_list_[0];
	enum FloatValue {
		kFvInitialDelay,
		kFvDuration,
		kFvFrequency,
		kFvX,
		kFvY,
		kFvZ,
	};
#define V(i)	tag.float_value_list_[i]

	const float start_time = V(kFvInitialDelay);
	float amplitude = CalculateMeshOffset(_function, start_time, V(kFvDuration), V(kFvFrequency), frame_time, mesh_rotate_time_);

	quat offset_orientation;
	offset_orientation.SetEulerAngles(V(kFvZ)*amplitude, V(kFvX)*amplitude, V(kFvY)*amplitude);
	for (size_t y = 0; y < tag.mesh_index_list_.size(); ++y) {
		tbc::GeometryBase* mesh = GetMesh(tag.mesh_index_list_[y]);
		if (!mesh) {
			continue;
		}
		tbc::GeometryReference* mesh_ref = (tbc::GeometryReference*)mesh;
		xform extra_offset = mesh_ref->GetExtraOffsetTransformation();
		extra_offset.orientation_ = offset_orientation;
		mesh_ref->SetExtraOffsetTransformation(extra_offset);
	}
}



float Machine::CalculateMeshOffset(const str& function, float start_time, float duration, float frequency, float frame_time, float& mesh_time) const {
	if (mesh_time < start_time || mesh_time > start_time+duration) {
		return 0;
	}
	float amplitude = 0;
	float x = mesh_time-start_time;
	if (function == "|sin|") {
		amplitude = std::abs(::sin(x*2*PIF*frequency));
	} else if (function == "linear") {
		amplitude = x;
	} else {
		log_.Errorf("Uknown mesh_xxx function '%s'.", function.c_str());
		deb_assert(false);
	}
	mesh_time += frame_time;
	return amplitude;
}



void Machine::LoadPlaySound3d(UserSound3dResource* sound_resource) {
	//deb_assert(sound_resource->GetLoadState() == cure::kResourceLoadComplete);
	if (sound_resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetSoundManager()->Play(sound_resource->GetData(), 0, 1.0);
	}
}



loginstance(kGameContextCpp, Machine);



}
