
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/physicsengine.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/endian.h"
#include "../../lepra/include/math.h"
#include "../../lepra/include/vector2d.h"
#include "../include/chunkybonegeometry.h"
#include "../include/chunkyphysics.h"



namespace tbc {



#define kMaxAspectIndex	400



PhysicsEngine::PhysicsEngine(EngineType engine_type, float strength, float max_speed,
	float max_speed2, float friction, unsigned controller_index):
	engine_type_(engine_type),
	strength_(strength),
	max_speed_(max_speed),
	max_speed2_(max_speed2),
	friction_(friction),
	controller_index_(controller_index),
	intensity_(0) {
	::memset(value_, 0, sizeof(value_));
	::memset(smooth_value_, 0, sizeof(smooth_value_));
}

PhysicsEngine::~PhysicsEngine() {
}

void PhysicsEngine::RelocatePointers(const ChunkyPhysics* target, const ChunkyPhysics* source, const PhysicsEngine& original) {
	const size_t cnt = engine_node_array_.size();
	for (size_t x = 0; x < cnt; ++x) {
		const int bone_index = source->GetIndex(original.engine_node_array_[x].geometry_);
		deb_assert(bone_index >= 0);
		engine_node_array_[x].geometry_ = target->GetBoneGeometry(bone_index);
	}
}



PhysicsEngine* PhysicsEngine::Load(ChunkyPhysics* structure, const void* data, unsigned byte_count) {
	const uint32* _data = (const uint32*)data;
	if (byte_count != sizeof(uint32)*7 + Endian::BigToHost(_data[6])*sizeof(uint32)*3) {
		log_.Error("Could not load; wrong data size.");
		deb_assert(false);
		return (0);
	}

	PhysicsEngine* engine = new PhysicsEngine(kEngineWalk, 0, 0, 0, 0, 0);
	engine->LoadChunkyData(structure, data);
	if (engine->GetChunkySize() != byte_count) {
		deb_assert(false);
		log_.Error("Corrupt data or error in loading algo.");
		delete (engine);
		engine = 0;
	}
	return (engine);
}



PhysicsEngine::EngineType PhysicsEngine::GetEngineType() const {
	return (engine_type_);
}



void PhysicsEngine::AddControlledGeometry(ChunkyBoneGeometry* geometry, float scale, EngineMode mode) {
	engine_node_array_.push_back(EngineNode(geometry, scale, mode));
}

void PhysicsEngine::RemoveControlledGeometry(ChunkyBoneGeometry* geometry) {
	EngineNodeArray::iterator i = engine_node_array_.begin();
	for (; i != engine_node_array_.end(); ++i) {
		if (i->geometry_ == geometry) {
			engine_node_array_.erase(i);
			return;
		}
	}
}

PhysicsEngine::GeometryList PhysicsEngine::GetControlledGeometryList() const {
	GeometryList list;
	EngineNodeArray::const_iterator i = engine_node_array_.begin();
	for (; i != engine_node_array_.end(); ++i) {
		list.push_back(i->geometry_);
	}
	return list;
}

void PhysicsEngine::SetStrength(float strength) {
	strength_ = strength;
}

bool PhysicsEngine::SetValue(unsigned aspect, float value) {
	deb_assert(controller_index_ >= 0 && controller_index_ < kMaxAspectIndex);
	deb_assert(value >= -10000);
	deb_assert(value <= +10000);

	switch (engine_type_) {
		case kEngineWalk:
		case kEnginePushRelative:
		case kEnginePushAbsolute:
		case kEnginePushTurnRelative:
		case kEnginePushTurnAbsolute: {
			const unsigned controlled_aspects = 3;
			if (aspect >= controller_index_+0 && aspect <= controller_index_+controlled_aspects) {
				switch (aspect - controller_index_) {
					case 0:	value_[kAspectPrimary]    = value;	break;
					case 2:	value_[kAspectPrimary]   += value;	break;	// Handbrake.
					case 1:	value_[kAspectSecondary]  = value;	break;
					case 3:	value_[kAspectTertiary]	  = value;	break;
				}
				return (true);
			}
		} break;
		case kEngineHover:
		case kEngineHingeRoll:
		case kEngineHingeGyro:
		case kEngineHingeBrake:
		case kEngineHingeTorque:
		case kEngineHinge2Turn:
		case kEngineRotor:
		case kEngineRotorTilt:
		case kEngineJet:
		case kEngineSliderForce:
		case kEngineYawBrake:
		case kEngineAirBrake: {
			if (aspect == controller_index_) {
				value_[kAspectPrimary] = value;
				return (true);
			}
		} break;
		case kEngineGlue:
		case kEngineBallBrake: {
			// Fixed mode "engine".
		} break;
		default: {
			deb_assert(false);
		} break;
	}
	return (false);
}

void PhysicsEngine::ForceSetValue(unsigned aspect, float value) {
	deb_assert(value >= -1 && value <= +1);
	value_[aspect] = value;
}



void PhysicsEngine::OnMicroTick(PhysicsManager* physics_manager, const ChunkyPhysics* structure, float frame_time) const {
	const float limited_frame_time = std::min(frame_time, 0.1f);
	const float normalized_frame_time = limited_frame_time * 90;
	const float primary_force = (value_[kAspectLocalPrimary] > std::abs(value_[kAspectPrimary]))? value_[kAspectLocalPrimary] : value_[kAspectPrimary];
	intensity_ = 0;
	EngineNodeArray::const_iterator i = engine_node_array_.begin();
	for (; i != engine_node_array_.end(); ++i) {
		const EngineNode& _engine_node = *i;
		ChunkyBoneGeometry* _geometry = _engine_node.geometry_;
		const float _scale = _engine_node.scale_;
		if (!_geometry) {
			log_.Error("Missing node!");
			continue;
		}
		switch (engine_type_) {
			case kEngineWalk:
			case kEnginePushRelative:
			case kEnginePushAbsolute: {
				vec3 axis[3] = {vec3(0, 1, 0),
					vec3(1, 0, 0), vec3(0, 0, 1)};
				if (engine_type_ == kEnginePushRelative) {
					const ChunkyBoneGeometry* root_geometry = structure->GetBoneGeometry(0);
					const quat orientation =
						physics_manager->GetBodyOrientation(root_geometry->GetBodyId()) *
						structure->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
					axis[0] = orientation*axis[0];
					axis[1] = orientation*axis[1];
					axis[2] = orientation*axis[2];
				}
				vec3 offset;
				while (_geometry->GetJointType() == ChunkyBoneGeometry::kJointExclude) {
					ChunkyBoneGeometry* parent = _geometry->GetParent();
					if (!parent) {
						break;
					}
					vec3 maya_offset = _geometry->GetOriginalOffset();
					std::swap(maya_offset.y, maya_offset.z);
					maya_offset.y = -maya_offset.y;
					offset += maya_offset;
					_geometry = parent;
				}
				vec3 push_vector;
				for (int i = kAspectPrimary; i <= kAspectTertiary; ++i) {
					push_vector += value_[i] * axis[i];
				}
				const float push_force = push_vector.GetLength();
				if (push_force > 0.1f || friction_) {
					if (friction_) {
						vec3 velocity_vector;
						physics_manager->GetBodyVelocity(_geometry->GetBodyId(), velocity_vector);
						velocity_vector /= max_speed_;
						if (engine_type_ == kEngineWalk) {
							velocity_vector.z = 0;	// When walking we won't apply brakes in Z.
						}
						vec3 f = (velocity_vector-push_vector) * (0.5f*friction_);
						push_vector -= f;
					}
					physics_manager->AddForceAtRelPos(_geometry->GetBodyId(), push_vector*strength_*_scale, offset);
				}
				intensity_ += Math::Lerp(Math::Clamp(friction_,0.1f,0.5f), 1.0f, push_force);
			} break;
			case kEnginePushTurnRelative:
			case kEnginePushTurnAbsolute: {
				vec3 axis[3] = {vec3(0, 0, 1), vec3(1, 0, 0), vec3(0, 1, 0)};
				if (engine_type_ == kEnginePushTurnRelative) {
					const ChunkyBoneGeometry* root_geometry = structure->GetBoneGeometry(0);
					const quat orientation =
						physics_manager->GetBodyOrientation(root_geometry->GetBodyId()) *
						structure->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
					axis[0] = orientation*axis[0];
					axis[1] = orientation*axis[1];
					axis[2] = orientation*axis[2];
				}
				vec3 push_vector;
				for (int i = kAspectPrimary; i <= kAspectTertiary; ++i) {
					push_vector += value_[i] * axis[i];
				}
				const float push_force = push_vector.GetLength();
				if (push_force > 0.1f || friction_ != 0) {
					if (friction_) {
						vec3 angular_velocity_vector;
						physics_manager->GetBodyAngularVelocity(_geometry->GetBodyId(), angular_velocity_vector);
						angular_velocity_vector /= max_speed_;
						vec3 f = (angular_velocity_vector-push_vector) * (0.5f*friction_);
						push_vector -= f;
					}
					physics_manager->AddTorque(_geometry->GetBodyId(), push_vector*strength_*_scale);
				}
				intensity_ += Math::Lerp(Math::Clamp(friction_,0.1f,0.5f), 1.0f, push_force);
			} break;
			case kEngineHover: {
				if (primary_force != 0 || value_[kAspectSecondary] != 0) {
					// Arcade stabilization for lifter (typically hovercraft, elevator or similar vehicle).
					vec3 lift_pivot = physics_manager->GetBodyPosition(_geometry->GetBodyId()) + vec3(0,0,1)*friction_*_scale;

					const vec3 lift_force = vec3(0,0,1)*strength_*_scale;
					physics_manager->AddForceAtPos(_geometry->GetBodyId(), lift_force, lift_pivot);
				}
			} break;
			case kEngineHingeGyro: {
				// Apply a fake gyro torque to parent in order to emulate a heavier gyro than
				// it actually is. The gyro must be light weight, or physics simulation will be
				// unstable when rolling bodies around any other axis than the hinge one.
				deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT && friction_ >= 0) {
					vec3 axis;
					physics_manager->GetAxis1(_geometry->GetJointId(), axis);
					vec3 __y;
					vec3 __z;
					axis.GetNormalized().GetOrthogonals(__y, __z);
					const float _strength = 3 * primary_force * strength_;
					__z *= _strength;
					vec3 pos;
					physics_manager->GetAnchorPos(_geometry->GetJointId(), pos);
					physics_manager->AddForceAtPos(_geometry->GetParent()->GetBodyId(), __z, pos+__y);
					physics_manager->AddForceAtPos(_geometry->GetParent()->GetBodyId(), -__z, pos-__y);
				}
			}
			// TRICKY: fall through.
			case kEngineHingeRoll: {
				//deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT) {
					float _value = primary_force;
					float directional_max_speed = ((_value >= 0)? max_speed_ : -max_speed2_) * _value;
					float rotation_speed;
					physics_manager->GetAngleRate2(_geometry->GetJointId(), rotation_speed);
					const float intensity = rotation_speed / max_speed_;
					intensity_ += std::abs(intensity);
					if (engine_type_ == kEngineHingeGyro) {
						_value = (_value+1)*0.5f;
						directional_max_speed = _value * (max_speed_ - max_speed2_) + max_speed2_;
					} else if (engine_type_ == kEngineHingeRoll) {
						//if (_value > 0)
						{
							// Torque curve approximation, (tested it out, looks ok to me):
							//   -8*(x-0.65)^2*(x-0.02) + 1
							//
							// Starts at about 100 % strength at 0 RPM, local strength minimum of approximately 75 %
							// at about 25 % RPM, maximum (in range) of 100 % strength at 65 % RPM, and drops to close
							// to 0 % strength at 100 % RPM.
							const float square = intensity - 0.65f;
							_value *= -8 * square * square * (intensity-0.02f) + 1;
						}
					}
					const float used_strength = strength_*(std::abs(_value) + std::abs(friction_));
					float previous_strength = 0;
					float previous_target_speed = 0;
					physics_manager->GetAngularMotorRoll(_geometry->GetJointId(), previous_strength, previous_target_speed);
					const float target_speed = Math::Lerp(previous_target_speed, directional_max_speed*_scale, 0.5f);
					const float target_strength = Math::Lerp(previous_strength, used_strength, 0.5f);
					physics_manager->SetAngularMotorRoll(_geometry->GetJointId(), target_strength, target_speed);
				} else {
					log_.Error("Missing roll joint!");
				}
			} break;
			case kEngineHingeBrake: {
				//deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT) {
					// "Max speed" used as a type of "break threashold", so that a joystick or similar
					// won't start breaking on the tiniest movement. "Scaling" here determines part of
					// functionality (such as only affecting some wheels), may be positive or negative.
					const float abs_value = std::abs(primary_force);
					if (abs_value > max_speed_ && primary_force < _scale) {
						const float break_force_used = strength_*abs_value;
						_geometry->SetExtraData(1);
						physics_manager->SetAngularMotorRoll(_geometry->GetJointId(), break_force_used, 0);
					} else if (_geometry->GetExtraData()) {
						_geometry->SetExtraData(0);
						physics_manager->SetAngularMotorRoll(_geometry->GetJointId(), 0, 0);
					}
				} else {
					log_.Error("Missing break joint!");
				}
			} break;
			case kEngineHingeTorque:
			case kEngineHinge2Turn: {
				ApplyTorque(physics_manager, limited_frame_time, _geometry, _engine_node);
			} break;
			case kEngineRotor: {
				deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT) {
					const vec3 rotor_force = GetRotorLiftForce(physics_manager, _geometry, _engine_node);
					vec3 lift_force = rotor_force * primary_force;
					const int parent_bone = structure->GetIndex(_geometry->GetParent());
					const quat orientation =
						physics_manager->GetBodyOrientation(_geometry->GetParent()->GetBodyId()) *
						structure->GetOriginalBoneTransformation(parent_bone).GetOrientation().GetInverse();

					vec3 rotor_pivot;
					physics_manager->GetAnchorPos(_geometry->GetJointId(), rotor_pivot);
					const vec3 offset = orientation * vec3(0, 0, max_speed_*_scale);
					rotor_pivot += offset;

					if (max_speed2_) {
						// Arcade stabilization for VTOL rotor.
						vec3 parent_angular_velocity;
						physics_manager->GetBodyAngularVelocity(_geometry->GetParent()->GetBodyId(), parent_angular_velocity);
						parent_angular_velocity = orientation.GetInverse() * parent_angular_velocity;
						const vec3 parent_angle = orientation.GetInverse() * vec3(0, 0, 1);	// TRICKY: assumes original joint direction is towards heaven.
						const float stability_x = -parent_angle.x * 0.5f + parent_angular_velocity.y * max_speed2_;
						const float stability_y = -parent_angle.y * 0.5f - parent_angular_velocity.x * max_speed2_;
						rotor_pivot += orientation * vec3(stability_x, stability_y, 0);
					}

					// Smooth rotor force - for digital controls and to make acceleration seem more realistic.
					const float smooth = normalized_frame_time * 0.05f * _engine_node.scale_;
					lift_force.x = smooth_value_[kAspectPrimary] = Math::Lerp(smooth_value_[kAspectPrimary], lift_force.x, smooth);
					lift_force.y = smooth_value_[kAspectSecondary] = Math::Lerp(smooth_value_[kAspectSecondary], lift_force.y, smooth);
					lift_force.z = smooth_value_[kAspectTertiary] = Math::Lerp(smooth_value_[kAspectTertiary], lift_force.z, smooth);

					// Counteract rotor's movement through perpendicular air.
					vec3 drag_force;
					physics_manager->GetBodyVelocity(_geometry->GetBodyId(), drag_force);
					drag_force = ((-drag_force*rotor_force.GetNormalized()) * friction_ * normalized_frame_time) * rotor_force;

					physics_manager->AddForceAtPos(_geometry->GetParent()->GetBodyId(), lift_force + drag_force, rotor_pivot);
				} else {
					log_.Error("Missing rotor joint!");
				}
			} break;
			case kEngineRotorTilt: {
				deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT) {
					const vec3 lift_force = GetRotorLiftForce(physics_manager, _geometry, _engine_node) * std::abs(primary_force);
					const int parent_bone = structure->GetIndex(_geometry->GetParent());
					const float placement = (primary_force >= 0)? 1.0f : -1.0f;
					const vec3 offset =
						physics_manager->GetBodyOrientation(_geometry->GetParent()->GetBodyId()) *
						structure->GetOriginalBoneTransformation(parent_bone).GetOrientation().GetInverse() *
						vec3(placement*max_speed_, -placement*max_speed2_, 0);
					const vec3 world_pos = offset + physics_manager->GetBodyPosition(_geometry->GetBodyId());
					physics_manager->AddForceAtPos(_geometry->GetParent()->GetBodyId(), lift_force, world_pos);
					//{
					//	static int cnt = 0;
					//	if ((++cnt)%300 == 0)
					//	{
					//		//vec3 r = physics_manager->GetBodyOrientation(_geometry->GetBodyId()).GetInverse() * rel_pos;
					//		//vec3 r = rel_pos;
					//		vec3 r = offset;
					//		vec3 w = physics_manager->GetBodyPosition(_geometry->GetBodyId());
					//		mLog.Infof("Got pos (%f, %f, %f - world pos is (%f, %f, %f)."), r.x, r.y, r.z, w.x, w.y, w.z);
					//	}
					//}
				} else {
					log_.Error("Missing rotor joint!");
				}
			} break;
			case kEngineJet: {
				deb_assert(false);	// TODO: use relative push instead!
				ChunkyBoneGeometry* root_geometry = structure->GetBoneGeometry(0);
				vec3 velocity;
				physics_manager->GetBodyVelocity(root_geometry->GetBodyId(), velocity);
				if (primary_force != 0 && velocity.GetLengthSquared() < max_speed_*max_speed_) {
					const quat orientation =
						physics_manager->GetBodyOrientation(root_geometry->GetBodyId()) *
						structure->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
					const vec3 push_force = orientation * vec3(0, primary_force*strength_, 0);
					physics_manager->AddForce(_geometry->GetBodyId(), push_force);
				}
				intensity_ += primary_force;
			} break;
			case kEngineSliderForce: {
				deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT) {
					if (!primary_force && _engine_node.mode_ == kModeNormal) {	// Normal slider behavior is to pull back to origin while half-lock keep last motor target.
						float position;
						physics_manager->GetSliderPos(_geometry->GetJointId(), position);
						if (!Math::IsEpsEqual(position, 0.0f, 0.1f)) {
							float _value = -position*std::abs(_scale);
							_value = (_value > 0)? _value*max_speed_ : _value*max_speed2_;
							physics_manager->SetMotorTarget(_geometry->GetJointId(), strength_, _value);
						}
					} else if (!primary_force && _engine_node.mode_ == kModeRelease) {	// Release slider behavior just lets go.
						physics_manager->SetMotorTarget(_geometry->GetJointId(), 0, 0);
					} else {
						const float _value = (primary_force > 0)? primary_force*max_speed_ : primary_force*max_speed2_;
						physics_manager->SetMotorTarget(_geometry->GetJointId(), strength_, _value*_scale);
					}
					float speed = 0;
					physics_manager->GetSliderSpeed(_geometry->GetJointId(), speed);
					intensity_ += std::abs(speed) / max_speed_;
				}
			} break;
			case kEngineGlue:
			case kEngineBallBrake: {
				deb_assert(_geometry->GetJointId() != INVALID_JOINT);
				if (_geometry->GetJointId() != INVALID_JOINT) {
					physics_manager->StabilizeJoint(_geometry->GetJointId());
				}
			} break;
			case kEngineYawBrake: {
				const tbc::PhysicsManager::BodyID body_id = _geometry->GetBodyId();
				vec3 angular_velocity;
				physics_manager->GetBodyAngularVelocity(body_id, angular_velocity);
				// Reduce rotation of craft.
				angular_velocity.z *= friction_;
				const float low_angular_velocity = max_speed_;
				if (Math::IsEpsEqual(primary_force, 0.0f) && std::abs(angular_velocity.z) < low_angular_velocity) {
					// Seriously kill speed depending on strength.
					angular_velocity.z *= 1/strength_;
				}
				physics_manager->SetBodyAngularVelocity(body_id, angular_velocity);
			} break;
			case kEngineAirBrake: {
				//      1
				// F =  - pv^2 C  A
				//  D   2       d
				const float cd_a = 0.5f * 1.225f * friction_;	// Density of air multiplied with friction coefficient and area (the two latter combined in friction_).
				const tbc::PhysicsManager::BodyID body_id = _geometry->GetBodyId();
				vec3 velocity;
				physics_manager->GetBodyVelocity(body_id, velocity);
				const float speed = velocity.GetLength();
				const vec3 drag = velocity * -speed * cd_a;
				physics_manager->AddForce(body_id, drag);
			} break;
			default: {
				deb_assert(false);
			} break;
		}
	}
	if (intensity_) {
		intensity_ /= engine_node_array_.size();
	}
}

vec3 PhysicsEngine::GetCurrentMaxSpeed(const PhysicsManager* physics_manager) const {
	vec3 max_velocity;
	float _max_speed = 0;
	EngineNodeArray::const_iterator i = engine_node_array_.begin();
	for (; i != engine_node_array_.end(); ++i) {
		const EngineNode& _engine_node = *i;
		ChunkyBoneGeometry* _geometry = _engine_node.geometry_;
		vec3 velocity;
		physics_manager->GetBodyVelocity(_geometry->GetBodyId(), velocity);
		const float speed = velocity.GetLengthSquared();
		if (speed > _max_speed) {
			_max_speed = speed;
			max_velocity = velocity;
		}
	}
	return max_velocity;
}



void PhysicsEngine::UprightStabilize(PhysicsManager* physics_manager, const ChunkyPhysics* structure,
	const ChunkyBoneGeometry* geometry, float strength, float friction) {
	const int root_bone = 0;	// Use root bone for fetching original transform, or "up" will be off.
	const quat orientation =
		physics_manager->GetBodyOrientation(geometry->GetBodyId()) *
		structure->GetOriginalBoneTransformation(root_bone).GetOrientation().GetInverse();
	// 1st: angular velocity damping (skipping z).
	vec3 angular;
	physics_manager->GetBodyAngularVelocity(geometry->GetBodyId(), angular);
	angular = orientation.GetInverse() * angular;
	angular.z = 0;
	angular *= -friction;
	vec3 torque = orientation * angular;
	// 2nd: strive towards straight.
	angular = orientation * vec3(0, 0, 1);
	torque += vec3(+angular.y * friction*5, -angular.x * friction*5, 0);
	physics_manager->AddTorque(geometry->GetBodyId(), torque*strength);
}

void PhysicsEngine::ForwardStabilize(PhysicsManager* physics_manager, const ChunkyPhysics* structure,
	const ChunkyBoneGeometry* geometry, float strength, float friction) {
	const int bone = structure->GetIndex(geometry);
	const quat orientation =
		physics_manager->GetBodyOrientation(geometry->GetBodyId()) *
		structure->GetOriginalBoneTransformation(bone).GetOrientation().GetInverse();
	// 1st: angular velocity damping in Z-axis.
	vec3 velocity3d;
	physics_manager->GetBodyAngularVelocity(geometry->GetBodyId(), velocity3d);
	const float angular_velocity = velocity3d.z;
	// 2nd: strive towards straight towards where we're heading.
	physics_manager->GetBodyVelocity(geometry->GetBodyId(), velocity3d);
	vec2 velocity2d(velocity3d.x, velocity3d.y);
	const vec3 forward3d = orientation * vec3(0, 1, 0);
	vec2 forward2d(forward3d.x, forward3d.y);
	if (forward2d.GetLengthSquared() > 0.1f && velocity2d.GetLengthSquared() > 0.3f) {
		float angle = forward2d.GetAngle(velocity2d);
		if (angle > PIF) {
			angle -= 2*PIF;
		} else if (angle < -PIF) {
			angle += 2*PIF;
		}
		velocity3d.Set(0, 0, (6*angle - angular_velocity*friction*1.5f) * strength);
		physics_manager->AddTorque(geometry->GetBodyId(), velocity3d);
	}
}



unsigned PhysicsEngine::GetControllerIndex() const {
	return (controller_index_);
}

float PhysicsEngine::GetValue() const {
	deb_assert(controller_index_ >= 0 && controller_index_ < kMaxAspectIndex);
	if (engine_type_ == kEngineWalk || engine_type_ == kEnginePushRelative || engine_type_ == kEnginePushAbsolute) {
		const float a = std::abs(value_[kAspectPrimary]);
		const float b = std::abs(value_[kAspectSecondary]);
		const float c = std::abs(value_[kAspectTertiary]);
		if (a > b && a > c) {
			return value_[kAspectPrimary];
		}
		if (b > c) {
			return value_[kAspectSecondary];
		}
		return value_[kAspectTertiary];
	}
	return value_[kAspectPrimary];
}

const float* PhysicsEngine::GetValues() const {
	return value_;
}

float PhysicsEngine::GetIntensity() const {
	return (intensity_);
}

float PhysicsEngine::GetMaxSpeed() const {
	return (max_speed_);
}

float PhysicsEngine::GetLerpThrottle(float up, float down, bool _abs) const {
	float& lerp_shadow = _abs? smooth_value_[kAspectLocalShadow] : smooth_value_[kAspectLocalShadowAbs];
	float _value = GetPrimaryValue();
	_value = _abs? std::abs(_value) : _value;
	lerp_shadow = Math::Lerp(lerp_shadow, _value, (_value > lerp_shadow)? up : down);
	return lerp_shadow;
}

bool PhysicsEngine::HasEngineMode(EngineMode mode) const {
	EngineNodeArray::const_iterator i = engine_node_array_.begin();
	for (; i != engine_node_array_.end(); ++i) {
		if (i->mode_ == mode) {
			return true;
		}
	}
	return false;
}



unsigned PhysicsEngine::GetChunkySize() const {
	return ((unsigned)(sizeof(uint32)*6 +
		sizeof(uint32) + sizeof(uint32)*3*engine_node_array_.size()));
}

void PhysicsEngine::SaveChunkyData(const ChunkyPhysics* structure, void* data) const {
	uint32* _data = (uint32*)data;
	_data[0] = Endian::HostToBig(GetEngineType());
	_data[1] = Endian::HostToBigF(strength_);
	_data[2] = Endian::HostToBigF(max_speed_);
	_data[3] = Endian::HostToBigF(max_speed2_);
	_data[4] = Endian::HostToBigF(friction_);
	_data[5] = Endian::HostToBig(controller_index_);
	_data[6] = Endian::HostToBig((uint32)engine_node_array_.size());
	int x;
	for (x = 0; x < (int)engine_node_array_.size(); ++x) {
		const EngineNode& controlled_node = engine_node_array_[x];
		_data[7+x*3] = Endian::HostToBig(structure->GetIndex(controlled_node.geometry_));
		_data[8+x*3] = Endian::HostToBigF(controlled_node.scale_);
		_data[9+x*3] = Endian::HostToBig(controlled_node.mode_);
	}
}



float PhysicsEngine::GetPrimaryValue() const {
	const float force = GetValue();
	const float primary_force = (value_[kAspectLocalPrimary] > std::abs(force))? value_[kAspectLocalPrimary] : force;
	return primary_force;
}



void PhysicsEngine::LoadChunkyData(ChunkyPhysics* structure, const void* data) {
	const uint32* _data = (const uint32*)data;

	engine_type_ = (EngineType)Endian::BigToHost(_data[0]);
	strength_ = Endian::BigToHostF(_data[1]);
	max_speed_ = Endian::BigToHostF(_data[2]);
	max_speed2_ = Endian::BigToHostF(_data[3]);
	friction_ = Endian::BigToHostF(_data[4]);
	controller_index_ = Endian::BigToHost(_data[5]);
	deb_assert(controller_index_ >= 0 && controller_index_ < kMaxAspectIndex);
	const int controlled_node_count = Endian::BigToHost(_data[6]);
	int x;
	for (x = 0; x < controlled_node_count; ++x) {
		ChunkyBoneGeometry* _geometry = structure->GetBoneGeometry(Endian::BigToHost(_data[7+x*3]));
		deb_assert(_geometry);
		float _scale = Endian::BigToHostF(_data[8+x*3]);
		EngineMode _mode = (EngineMode)Endian::BigToHost(_data[9+x*3]);
		AddControlledGeometry(_geometry, _scale, _mode);
	}
}



vec3 PhysicsEngine::GetRotorLiftForce(PhysicsManager* physics_manager, ChunkyBoneGeometry* geometry, const EngineNode& engine_node) const {
	vec3 axis;
	physics_manager->GetAxis1(geometry->GetJointId(), axis);
	float angular_rotor_speed = 0;
	physics_manager->GetAngleRate1(geometry->GetJointId(), angular_rotor_speed);
	const float lift_force = angular_rotor_speed*angular_rotor_speed*strength_*engine_node.scale_;
	return (axis*lift_force);
}

void PhysicsEngine::ApplyTorque(PhysicsManager* physics_manager, float frame_time, ChunkyBoneGeometry* geometry, const EngineNode& engine_node) const {
	frame_time;

	//deb_assert(geometry->GetJointId() != INVALID_JOINT);
	if (geometry->GetJointId() == INVALID_JOINT) {
		log_.Error("Missing torque joint!");
		return;
	}

	float force = GetPrimaryValue();

	const float _scale = engine_node.scale_;
	//const float lReverseScale = (_scale + 1) * 0.5f;	// Move towards linear scaling.
	float lo_stop;
	float hi_stop;
	float bounce;
	physics_manager->GetJointParams(geometry->GetJointId(), lo_stop, hi_stop, bounce);
	//const float middle = (lo_stop+hi_stop)*0.5f;
	const float _target = 0;
	if (lo_stop < -1000 || hi_stop > 1000) {
		// Open interval -> relative torque.
		const float target_speed = force*_scale*max_speed_;
		physics_manager->SetAngularMotorTurn(geometry->GetJointId(), strength_, target_speed);
		float actual_speed = 0;
		physics_manager->GetAngleRate2(geometry->GetJointId(), actual_speed);
		intensity_ += std::abs(target_speed - actual_speed) / max_speed_;
		return;
	}

	float irl_angle;
	if (!physics_manager->GetAngle1(geometry->GetJointId(), irl_angle)) {
		log_.Error("Bad joint angle!");
		return;
	}
	// Flip angle if parent is "world".
	if (physics_manager->IsStaticBody(geometry->GetParent()->GetBodyId())) {
		irl_angle = -irl_angle;
	}
	const float irl_angle_direction = (hi_stop < lo_stop)? -irl_angle : irl_angle;

	if (engine_node.mode_ == kModeHalfLock) {
		if ((force < 0.02f && irl_angle_direction < _target) ||
			(force > -0.02f && irl_angle_direction > _target)) {
			if (std::abs(force) > 0.02) {
				engine_node.lock_ = force;
			} else {
				force = engine_node.lock_;
			}
		} else {
			engine_node.lock_ = 0;
		}
	}
	if (friction_) {
		// Wants us to scale (down) rotation angle depending on vehicle speed. Otherwise most vehicles
		// quickly flips, not yeilding very fun gameplay. Plus, it's more like real racing cars! :)
		vec3 parent_velocity;
		physics_manager->GetBodyVelocity(geometry->GetParent()->GetBodyId(), parent_velocity);
		const float range_factor = ::pow(friction_, parent_velocity.GetLength());
		hi_stop *= range_factor;
		lo_stop *= range_factor;
	}
	const float angle_span = (hi_stop-lo_stop)*0.9f;
	const float target_angle = Math::Lerp(lo_stop, hi_stop, _scale*force*0.5f+0.5f);
	const float diff = (target_angle-irl_angle);
	const float abs_diff = std::abs(diff);
	float target_speed;
	const float abs_big_diff = std::abs(angle_span/7);
	const bool close_to_goal = (abs_diff < abs_big_diff);
	if (!close_to_goal) {
		target_speed = (diff > 0)? max_speed_ : -max_speed_;
	} else {
		target_speed = max_speed_*diff/abs_big_diff;
	}
	//target_speed *= (force > 0)? _scale : lReverseScale;
	// If we're far from the desired target speed, we speed up.
	float current_speed = 0;
	if (engine_type_ == kEngineHinge2Turn) {
		physics_manager->GetAngleRate2(geometry->GetJointId(), current_speed);
		if (!close_to_goal) {
			target_speed *= 1+std::abs(current_speed)/30;
		}
	} else {
		physics_manager->GetAngleRate1(geometry->GetJointId(), current_speed);
		if (!close_to_goal) {
			target_speed += (target_speed-current_speed) * std::abs(_scale);
		}
	}
	/*if (Math::IsEpsEqual(target_speed, 0.0f, 0.01f)) {	// Stop when almost already at a halt.
		target_speed = 0;
	}*/
	physics_manager->SetAngularMotorTurn(geometry->GetJointId(), strength_, target_speed);
	intensity_ += std::abs(target_speed / (max_speed_ * _scale));
}



loginstance(kPhysics, PhysicsEngine);



}
