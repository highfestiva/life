
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "vehicleai.h"
#include "../cure/include/contextmanager.h"
#include "../cure/include/contextpath.h"
#include "../cure/include/elevator.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/random.h"
#include "goal.h"
#include "vehicle.h"
#include "game.h"
#include "level.h"



#define	NORMAL_AIM_AHEAD		5.0f	// How far ahead to try and intersect the path.
#define ON_GOAL_DISTANCE		3.2f	// When at goal.
#define ELEVATOR_WAIT_DISTANCE		8.8f	// When close to elevator.
#define ELEVATOR_TOO_CLOSE_DISTANCE	8.2f	// When too close to elevator.
#define ELEVATOR_GOT_OFF_EXTRA_DISTANCE	1.7f	// Distance to get off elevator.
#define ON_ELEVATOR_DISTANCE		2.3f	// When on elevator.
#define ELEVATOR_FAR_DISTANCE		15.0f	// When far to elevator.
#define SLOW_DOWN_DISTANCE		6.0f	// When to slow down before stopping at goal.
#define OFF_COURSE_DISTANCE		4.5f	// When to start heading back.
#define TOTALLY_OFF_COURSE_DISTANCE	16.0f	// When fallen off some cliff or similar.
#define END_PATH_TIME			0.9999f	// Time is considered at end of path.
#define	OFF_END_PATH_TIME		0.9995f	// Close to end, but not quite.
#define DOUBLE_OFF_END_PATH_TIME	0.9990f	// Close to end, but not quite. Double that.
#define SMOOTH_BRAKING_FACTOR		0.7f	// Factor to multiply with to ensure monster truck does not "rotate" too much.
#define AIM_DISTANCE			(1.2f * SCALE_FACTOR * NORMAL_AIM_AHEAD)


namespace tirefire {



typedef cure::ContextPath::SplinePath Spline;

struct PathIndexLikeliness {
	int path_index_;
	float likeliness_;
	float distance_;
};



VehicleAi::VehicleAi(Game* game):
	Parent(game->GetResourceManager(), "VehicleAi"),
	game_(game) {
}

VehicleAi::~VehicleAi() {
}

void VehicleAi::Init() {
	stopped_frame_ = -1;
	active_path_ = -1;
	mode_ = kModeAtGoal;	// Will end up in previous mode.
	SetMode(kModeFindBestPath);
	GetManager()->EnableTickCallback(this);
}



void VehicleAi::OnTick() {
	if (!game_->GetVehicle() || !game_->GetVehicle()->IsLoaded() ||
		!game_->GetLevel() || !game_->GetLevel()->IsLoaded() ||
		game_->GetFlybyMode() != Game::kFlybyInactive) {
		return;
	}

	const cure::TimeManager* _time = GetManager()->GetGameManager()->GetTimeManager();
	const int mode_run_delta_frame_count = _time->GetCurrentPhysicsFrameDelta(mode_start_frame_);
	const float mode_run_time = _time->ConvertPhysicsFramesToSeconds(mode_run_delta_frame_count);
	const float aim_distance = AIM_DISTANCE;

	float strength = 1.0f;
	const vec3 _position = game_->GetVehicle()->GetPosition();
	const vec3 _velocity = game_->GetVehicle()->GetVelocity();
	switch (mode_) {
		case kModeFindBestPath:
		case kModeFindPathOffElevator: {
			float start_time = 0.5f;
			if (active_path_ != -1) {
				// Synchronize all paths.
				Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
				start_time = _path->GetCurrentInterpolationTime();
				active_path_ = -1;
			}
			log_.Headlinef("Trying to find new path... starting iterating from  %.2f.", start_time);
			vec3 elevator_direction;
			if (mode_ == kModeFindPathOffElevator) {
				game_->GetVehicle()->SetEnginePower(0, 0);
				game_->GetVehicle()->SetEnginePower(2, -strength);	// Negative = use full brakes, not only hand brake.
				const cure::Elevator* _nearest_elevator;
				const vec3 elevator_position = GetClosestElevatorPosition(_position, _nearest_elevator);
				if (elevator_position.GetDistanceSquared(_position) > ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE) {
					log_.AHeadline("Fell off elevator while looking for get-off route. Looking for somewhere else to go.");
					SetMode(kModeFindBestPath);
					return;
				}
				elevator_direction = _nearest_elevator->GetVelocity().GetNormalized(0.5f);
			}
			float best_path_distance = 1000000;
			std::vector<PathIndexLikeliness> relevant_paths;
			bool lifting_towards_goal = false;
			const int path_count = game_->GetLevel()->QueryPath()->GetPathCount();
			for (int x = 0; x < path_count; ++x) {
				bool current_lifting_towards_goal = false;
				Spline* _path = game_->GetLevel()->QueryPath()->GetPath(x);
				_path->GotoAbsoluteTime(start_time);
				float _likeliness = 1;
				const float nearest_distance = GetClosestPathDistance(_position, x, &_likeliness)/SCALE_FACTOR/2;
				log_.Infof(" - Path %2i is %2.2f units away.", x, nearest_distance);
				if (mode_ == kModeFindPathOffElevator) {
					if (_path->GetCurrentInterpolationTime() > 0.7f) {
						// This path is probably the one I used to get ON the elevator (or one
						// just like it from another direction), we're not using that!
						log_.AInfo("   (Not relevant, too close to path end.)");
						continue;
					} else {
						const float towards_distance = GetClosestPathDistance(_position+elevator_direction, x)/SCALE_FACTOR/2;
						if (towards_distance < nearest_distance) {
							current_lifting_towards_goal = true;
							if (!lifting_towards_goal) {
								lifting_towards_goal = true;
								best_path_distance = 1000000;
							}
						}
					}
				}
				if (!current_lifting_towards_goal && lifting_towards_goal) {
					// This elevator isn't heading in the right direction, but at least one other is.
					continue;
				}
				PathIndexLikeliness pl;
				pl.path_index_ = x;
				pl.likeliness_ = _likeliness;
				pl.distance_ = nearest_distance;
				relevant_paths.push_back(pl);
				if (nearest_distance < best_path_distance) {
					best_path_distance = nearest_distance;
				}
			}
			// Sort out those that are too far away.
			float total_likeliness = 0;
			std::vector<PathIndexLikeliness>::iterator x;
			for (x = relevant_paths.begin(); x != relevant_paths.end();) {
				if (x->distance_ < best_path_distance+2.0f) {
					total_likeliness += x->likeliness_;
					++x;
				} else {
					x = relevant_paths.erase(x);
				}
			}
			if (mode_ == kModeFindPathOffElevator) {
				if (best_path_distance > 5 || relevant_paths.size() != 1) {
					if (relevant_paths.size() == 1) {
						// Point wheels in the right direction for us to get off safely.
						Spline* _path = game_->GetLevel()->QueryPath()->GetPath(relevant_paths[0].path_index_);
						const vec3 _direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
						const vec3 wanted_direction = _path->GetValue() - _position;
						const float angle = LEPRA_XY_ANGLE(wanted_direction, _direction);
						game_->GetVehicle()->SetEnginePower(1, angle*0.5f);
					}
					log_.Headlinef("On elevator: too long distance to path %.1f, or too many paths %u.", best_path_distance, relevant_paths.size());
					if (best_path_distance > 15) {
						const cure::Elevator* _nearest_elevator;
						const vec3 nearest_lift_position = GetClosestElevatorPosition(_position, _nearest_elevator);
						if (nearest_lift_position.GetDistanceSquared(_position) > ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE) {
							// DUCK!!! We fell off!
							log_.AHeadline("Was on elevator: I'm far from the elevator, so must've fallen off!");
							SetMode(kModeFindBestPath);
						}
					}
					if (mode_run_time >= 20) {
						log_.AHeadline("On elevator: been here too long, getting off!");
						SetMode(kModeFindBestPath);
					}
					return;
				}
				log_.Headlinef("Getting off elevator: distance to path %.1f.", best_path_distance);
			}
			deb_assert(!relevant_paths.empty());
			if (relevant_paths.empty()) {
				return;
			}
			const float picked_likeliness = Random::Uniform(0.0f, total_likeliness);
			total_likeliness = 0;
			for (x = relevant_paths.begin(); x != relevant_paths.end(); ++x) {
				const float next_likeliness = total_likeliness + x->likeliness_;
				if (picked_likeliness >= total_likeliness && picked_likeliness <= next_likeliness) {
					active_path_ = x->path_index_;
					break;
				}
				total_likeliness = next_likeliness;
			}
			if (active_path_ < 0) {
				active_path_ = relevant_paths[Random::GetRandomNumber() % relevant_paths.size()].path_index_;
			}
			Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
			const float wanted_distance = aim_distance;
			float step = wanted_distance * _path->GetDistanceNormal();
			if (step + _path->GetCurrentInterpolationTime() > 1) {
				step = 1 - _path->GetCurrentInterpolationTime();
			}
			_path->StepInterpolation(step);
			// Fetch ending position.
			const float t = _path->GetCurrentInterpolationTime();
			_path->GotoAbsoluteTime(END_PATH_TIME);
			elevator_get_on_position_ = _path->GetValue();
			_path->GotoAbsoluteTime(t);

			log_.Headlinef("Picked path %i (%i pickable."), active_path_, relevant_paths.size());
			if (mode_ == kModeFindPathOffElevator) {
				SetMode(kModeGetOffElevator);
			} else {
				SetMode(kModeHeadingBackOnTrack);
			}
		} break;
		case kModeHeadingBackOnTrack: {
			if (mode_run_delta_frame_count%5 == 2) {
				const float velocity_scale_factor = std::min(1.0f, _velocity.GetLength() / 2.5f);
				Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
				const float current_time = _path->GetCurrentInterpolationTime();
				const float nearest_path_distance = GetClosestPathDistance(_position, active_path_, 0, 1);
				if (nearest_path_distance > 3.0f) {
					// First verify that we haven't ended up under path somehow. We do that by checking
					// steepness, since pure Z-distance may be big when going over ditches.
					const vec3 path_position = _path->GetValue();
					const float steepness = (path_position.z - _position.z) / nearest_path_distance;
					//log_.Infof("Checking steepness, nearest path distance is %.3f, steepness is %.3f.", nearest_path_distance, steepness);
					if (steepness > 0.6f) {
						log_.Infof("Searching for new, better path, we seem to have ended up under the path. Beneath a bridge perhaps? Nearest path is %.2f, steepness is %.2f.", nearest_path_distance, steepness);
						SetMode(kModeFindBestPath);
						return;
					}
				}
				_path->GotoAbsoluteTime(current_time);
				if (nearest_path_distance < SCALE_FACTOR * OFF_COURSE_DISTANCE * velocity_scale_factor) {
					// We were able to return to normal, keep on running.
					SetMode(kModeNormal);
					return;
				}
				/*else if (nearest_path_distance > SCALE_FACTOR * OFF_COURSE_DISTANCE * velocity_scale_factor * 5) {
					// We're far off, perhaps we fell down from a plateu.
					active_path_ = -1;
					SetMode(kModeFindBestPath);
					return;
				}*/
				else if (mode_run_time > 7.0f) {
					SetMode(kModeFindBestPath);
					return;
				}
			}
		}
		// TRICKY: fall through.
		case kModeNormal:
		case kModeGetOnElevator:
		case kModeGetOffElevator: {
			if (mode_ == kModeGetOnElevator) {
				if (mode_run_time > 4.5) {
					log_.Headlinef("Something presumably hinders me getting on the elevator, back square one. (mode run time=%f"), mode_run_time);
					SetMode(kModeFindBestPath);
					return;
				}
				const cure::Elevator* _nearest_elevator;
				const vec3 nearest_lift_position = GetClosestElevatorPosition(elevator_get_on_position_, _nearest_elevator);
				if (nearest_lift_position.z > _position.z+0.5f) {
					log_.AHeadline("Couldn't get on in time, going back to waiting.");
					SetMode(kModeWaitingForElevator);
					return;
				}
			}

			if (mode_ != kModeHeadingBackOnTrack && mode_ != kModeGetOnElevator && mode_run_delta_frame_count%20 == 19) {
				const float _distance = GetClosestPathDistance(_position);
				if (_distance > SCALE_FACTOR * TOTALLY_OFF_COURSE_DISTANCE) {
					log_.AHeadline("Fell off something. Trying some new path.");
					SetMode(kModeFindBestPath);
					return;
				}
				const float velocity_scale_factor = ((mode_ == kModeNormal)? 1.0f : 3.0f) * Math::Clamp(_velocity.GetLength() / 2.5f, 0.3f, 1.0f);
				if (_distance > SCALE_FACTOR * OFF_COURSE_DISTANCE * velocity_scale_factor) {
					log_.AHeadline("Going about my way, but got offside somehow. Heading back.");
					SetMode(kModeHeadingBackOnTrack);
					return;
				}
			}

			Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
			vec3 target = _path->GetValue();

			// Check if vehicle stopped. That would mean either crashed against something or too steep hill.
			if (mode_run_delta_frame_count%7 == 4 && game_->GetVehicle()->GetHealth() > 0) {
				if (QueryVehicleHindered(_time, _velocity)) {
					const vec3 _direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
					const vec3 wanted_direction = target-_position;
					const float forward_angle = LEPRA_XY_ANGLE(wanted_direction, _direction);
					// Amplify angle to be either full left or full right.
					const float angle = (forward_angle < 0)? -1.0f : 1.0f;
					game_->GetVehicle()->SetEnginePower(1, -angle);
					SetMode(kModeBackingUp);
					return;
				}
			}

			// Are we heading towards an elevator?
			if (mode_ != kModeGetOnElevator && mode_ != kModeGetOffElevator && _path->GetType() == "to_elevator") {
				if (_path->GetDistanceLeft() <= ELEVATOR_WAIT_DISTANCE) {
					if (elevator_get_on_position_.GetDistanceSquared(_position) <= ELEVATOR_WAIT_DISTANCE*ELEVATOR_WAIT_DISTANCE) {
						log_.AHeadline("Normal mode close to end of path to elevator, changing mode.");
						SetMode(kModeWaitingForElevator);
						return;
					}
				}
			}

			// Did we just pass (fly by?) the goal?
			if (mode_run_delta_frame_count%3 == 0) {
				const vec3 goal_direction = game_->GetGoal()->GetPosition() - _position;
				if (::fabs(goal_direction.z) < 2 &&
					goal_direction.GetLengthSquared() < ELEVATOR_FAR_DISTANCE*ELEVATOR_FAR_DISTANCE &&
					_velocity.GetLengthSquared() < 6*6) {
					const vec3 vehicle_direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
					const float delta_angle = ::fabs(LEPRA_XY_ANGLE(goal_direction, vehicle_direction));
					if (delta_angle >= PIF-PIF/4 && delta_angle <= PIF+PIF/4) {
						log_.AHeadline("Passed goal, it's right behind me!");
						SetMode(kModeBackingUpToGoal);
						return;
					}
				}
			}

			// Step target (aim) ahead.
			{
				const float actual_distance2 = target.GetDistanceSquared(_position);
				const float max_aim_factor = (mode_ == kModeGetOffElevator)? 1.0f : 1.5f;
				const float wanted_distance = aim_distance * Math::Clamp(_velocity.GetLength() / 2.5f, 0.5f, max_aim_factor);
				if (actual_distance2 < wanted_distance*wanted_distance) {
					const float move_ahead = wanted_distance*1.1f - ::sqrt(actual_distance2);
					_path->StepInterpolation(move_ahead * _path->GetDistanceNormal());
					log_volatile(log_.Debugf("Stepping %f (=%f m from %f."), move_ahead*_path->GetDistanceNormal(), move_ahead, _path->GetCurrentInterpolationTime()));
				}

				// Check if we're there yet.
				const float t = _path->GetCurrentInterpolationTime();
				_path->GotoAbsoluteTime(1.0f);
				const float target_distance = (mode_ == kModeGetOnElevator)? ON_ELEVATOR_DISTANCE : ON_GOAL_DISTANCE + game_->GetVehicle()->GetForwardSpeed()/4;
				if (IsCloseToTarget(_position, target_distance)) {
					const bool towards_elevator = (_path->GetType() == "to_elevator");
					if (towards_elevator) {
						if (mode_ == kModeGetOnElevator) {
							SetMode(kModeOnElevator);
							return;
						} else if (mode_ != kModeGetOffElevator) {
							// We got off track somewhere, try to shape up!
							log_.AHeadline("Normal mode target wrapped on our way to an elevator, changing mode.");
							SetMode(kModeWaitingForElevator);
							return;
						}
					} else {
						SetMode(kModeStoppingAtGoal);
						return;
					}
				}
				_path->GotoAbsoluteTime(t);

				target = _path->GetValue();
			}

			const float get_off_delay_time = 0.4f;
			if (!(mode_ == kModeGetOffElevator && mode_run_time < get_off_delay_time)) {
				// Move forward.
				game_->GetVehicle()->SetEnginePower(0, +strength);
				game_->GetVehicle()->SetEnginePower(2, 0);
			}

			// Steer.
			const vec3 _direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
			const vec3 wanted_direction = target-_position;
			float angle = LEPRA_XY_ANGLE(wanted_direction, _direction);
			if (mode_ == kModeGetOffElevator) {
				// Aborting too early might cause us to stop, waiting for the next ride in mid-air.
				const float get_off_distance = GetClosestElevatorRadius() + ELEVATOR_GOT_OFF_EXTRA_DISTANCE;
				vec2 elevator_get_off2d(elevator_get_off_position_.x, elevator_get_off_position_.y);
				vec2 position2d(_position.x, _position.y);
				log_.Infof("ElevatorGetOff (%f;%f, pos (%f;%f)"), elevator_get_off2d.x, elevator_get_off2d.y, position2d.x, position2d.y);
				if (elevator_get_off2d.GetDistanceSquared(position2d) > get_off_distance*get_off_distance) {
					SetMode(kModeNormal);
				}
				angle *= 2;	// Make steering more powerful while getting off.
			}
			game_->GetVehicle()->SetEnginePower(1, +angle);
			last_average_angle_ = Math::Lerp(last_average_angle_, angle, 0.5f);

			// Check if we need to slow down.
			const float high_speed = SCALE_FACTOR * 2.7f;
			const float abs_angle = ::fabs(angle);
			if (_velocity.GetLengthSquared() > high_speed*high_speed) {
				if (abs_angle > 0.2f) {
					float factor = 0.10f;
					game_->GetVehicle()->SetEnginePower(2, abs_angle*factor + _velocity.GetLength()*factor*0.1f);
				} else if (_path->GetCurrentInterpolationTime() >= DOUBLE_OFF_END_PATH_TIME &&
					IsCloseToTarget(_position, SLOW_DOWN_DISTANCE)) {
					game_->GetVehicle()->SetEnginePower(2, 0.2f);
				}
			}
		} break;
		case kModeBackingUp: {
			// Brake or move backward.
			const bool is_moving_forward = (game_->GetVehicle()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
			game_->GetVehicle()->SetEnginePower(0, is_moving_forward? 0.0f : -strength);
			game_->GetVehicle()->SetEnginePower(2, is_moving_forward? strength :  0.0f);

			const float back_time = 1.7f;
			if (!is_moving_forward && mode_run_time > back_time) {
				SetMode(kModeHeadingBackOnTrack);
				return;
			}
		} break;
		case kModeBackingUpToGoal: {
			vec3 wanted_direction = game_->GetGoal()->GetPosition() - _position;
			const float distance2 = wanted_direction.GetLengthSquared();
			if (distance2 <= ON_GOAL_DISTANCE*ON_GOAL_DISTANCE) {
				Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
				_path->GotoAbsoluteTime(END_PATH_TIME);
				SetMode(kModeStoppingAtGoal);
				return;
			} else if (distance2 >= TOTALLY_OFF_COURSE_DISTANCE*TOTALLY_OFF_COURSE_DISTANCE) {
				SetMode(kModeFindBestPath);
				return;
			}

			// Brake or move backward.
			const bool is_moving_forward = (game_->GetVehicle()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
			game_->GetVehicle()->SetEnginePower(0, is_moving_forward? 0.0f : -strength);
			game_->GetVehicle()->SetEnginePower(2, is_moving_forward? strength :  0.0f);

			// Turn steering wheel.
			const vec3 _direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
			float angle = LEPRA_XY_ANGLE(wanted_direction, _direction);
			angle += (angle < 0)? +PIF : -PIF;
			angle *= 3;
			game_->GetVehicle()->SetEnginePower(1, -angle);

			if (mode_run_time > 15) {
				log_.AHeadline("Not getting back to goal. Fuck it.");
				SetMode(kModeRotateOnTheSpot);
				return;
			}
		} break;
		case kModeFlee: {
			// Pedal to the metal.
			game_->GetVehicle()->SetEnginePower(0, +strength);
			game_->GetVehicle()->SetEnginePower(1, 0);
			game_->GetVehicle()->SetEnginePower(2, 0);
			if (mode_run_time > 3.0f) {
				SetMode(kModeFindBestPath);
				return;
			}
		} break;
		case kModeStoppingAtGoal:
		case kModeAtGoal: {
			Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
			if (!IsCloseToTarget(_position, ON_GOAL_DISTANCE)) {
				// If either already stopped at goal, OR stopped but at the wrong spot.
				if (mode_ != kModeStoppingAtGoal || game_->GetVehicle()->GetForwardSpeed() < 0.5f*SCALE_FACTOR) {
					_path->GotoAbsoluteTime(DOUBLE_OFF_END_PATH_TIME);	// Close to end, but not at end.
					SetMode(kModeHeadingBackOnTrack);
					return;
				}
			}
			if (mode_ != kModeAtGoal) {
				SetMode(kModeAtGoal);
			}
			// Brake!
			game_->GetVehicle()->SetEnginePower(0, 0);
			game_->GetVehicle()->SetEnginePower(2, -strength);	// Negative = use full brakes, not only hand brake.
		} break;
		case kModeWaitingForElevator: {
			if (mode_run_time > 25.0f) {
				log_.AHeadline("Movin' on, I've waited for the elevator too long.");
				SetMode(kModeFlee);
				return;
			}
			if (::fabs(last_average_angle_) > 0.1f) {
				strength *= SMOOTH_BRAKING_FACTOR;	// Smooth braking when turning, we can always back up if necessary.
			}
			const float elevator_distance2 = elevator_get_on_position_.GetDistanceSquared(_position);
			if (elevator_distance2 < ELEVATOR_TOO_CLOSE_DISTANCE*ELEVATOR_TOO_CLOSE_DISTANCE) {
				log_.AHeadline("Got too close to the elevator stop position, backing up.");
				// Back up parallel to the spline direction.
				const vec3 _direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
				Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
				const vec3 wanted_direction = _path->GetSlope();
				const float angle = LEPRA_XY_ANGLE(wanted_direction, _direction);
				game_->GetVehicle()->SetEnginePower(1, +angle);
				const bool is_moving_forward = (game_->GetVehicle()->GetForwardSpeed() > 0.1f*SCALE_FACTOR);
				game_->GetVehicle()->SetEnginePower(0, is_moving_forward? 0.0f : -strength);
				game_->GetVehicle()->SetEnginePower(2, is_moving_forward? strength :  0.0f);

				const cure::Elevator* _nearest_elevator;
				vec3 _nearest_lift_position2d;
				float _elevator_xy_distance2_to_elevator_stop;
				const bool is_elevator_here = HasElevatorArrived(_nearest_elevator, _position.z, _nearest_lift_position2d, _elevator_xy_distance2_to_elevator_stop);
				if (is_elevator_here) {
					SetMode(kModeGetOnElevator);
				} else if (QueryVehicleHindered(_time, _velocity)) {
					last_average_angle_ = (Random::Uniform(0.0f, 1.0f) > 0.5f)? +2.0f : -2.0f;
					SetMode(kModeRotateOnTheSpot);
				}
				return;
			}
			if (::fabs(elevator_get_on_position_.z-_position.z) >= 3 ||
				elevator_distance2 > ELEVATOR_FAR_DISTANCE*ELEVATOR_FAR_DISTANCE) {
				log_.AHeadline("Somehow got away from the elevator wait position, doing something else.");
				SetMode(kModeFindBestPath);
				return;
			}

			// Check that we're headed towards the elevator center.
			if (_velocity.GetLengthSquared() < 0.5f) {
				vec3 up(0, 0, 1);
				up = game_->GetVehicle()->GetOrientation() * up;
				if (up.z > 0.7f) {
					const vec3 _direction = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
					const vec3 wanted_direction = elevator_get_on_position_ - _position;
					const float angle = LEPRA_XY_ANGLE(wanted_direction, _direction);
					if (::fabs(angle) > PIF/12) {
						rotate_angle_ = -angle;
						SetMode(kModeRotateOnTheSpotWaiting);
						return;
					}
				}
			}

			const cure::Elevator* _nearest_elevator;
			vec3 _nearest_lift_position2d;
			float _elevator_xy_distance2_to_elevator_stop;
			if (HasElevatorArrived(_nearest_elevator, _position.z, _nearest_lift_position2d, _elevator_xy_distance2_to_elevator_stop)) {
				vec3 velocity_xy = _nearest_elevator->GetVelocity();
				bool try_get_on = false;
				// Check if elevator is on it's way out.
				if (IsVertical(velocity_xy)) {
					try_get_on = true;
				} else {
					velocity_xy.x *= 0.1f;
					velocity_xy.y *= 0.1f;
					velocity_xy.z  = 0;
					if (_elevator_xy_distance2_to_elevator_stop+0.1f >= elevator_get_on_position_.GetDistanceSquared(_nearest_lift_position2d+velocity_xy)) {
						try_get_on = true;
					}
				}
				if (try_get_on) {
					log_.AInfo("Elevator here - getting on!");
					SetMode(kModeGetOnElevator);
					return;
				} else {
					log_.AInfo("Waiting for elevator: not getting on, since elevator is departing!");
				}
			}

			game_->GetVehicle()->SetEnginePower(1, 0);
			// Brake!
			game_->GetVehicle()->SetEnginePower(0, 0);
			game_->GetVehicle()->SetEnginePower(2, -strength);	// Negative = use full brakes, not only hand brake.
		} break;
		case kModeOnElevator: {
			strength *= SMOOTH_BRAKING_FACTOR;	// Smooth braking, we can always back up if necessary.

			// Brake!
			game_->GetVehicle()->SetEnginePower(0, 0);
			game_->GetVehicle()->SetEnginePower(1, 0);
			game_->GetVehicle()->SetEnginePower(2, -strength);	// Negative = use full brakes, not only hand brake.

			// Check if elevator departed.
			const float minimum_velocity2 = 0.5f*0.5f;
			if (mode_run_time > 0.7f && _velocity.GetLengthSquared() > minimum_velocity2) {
				const cure::Elevator* _nearest_elevator;
				const vec3 nearest_lift_position = GetClosestElevatorPosition(elevator_get_on_position_, _nearest_elevator);
				if (nearest_lift_position.z > _position.z+0.2f) {
					// Crap, we missed it!
					log_.AHeadline("Must have missed the elevator (it's not close!), waiting for it again!");
					SetMode(kModeWaitingForElevator);
					return;
				}
				// Vehicle speed check not enouch (bouncy wheels), so check elevator speed too.
				vec3 elevator_velocity = _nearest_elevator->GetVelocity();
				if (elevator_velocity.GetLengthSquared() > minimum_velocity2) {
					const bool is_horizontal = !IsVertical(elevator_velocity);
					const vec3 _direction = is_horizontal? elevator_velocity : game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
					rotate_angle_ = -GetRelativeDriveOnAngle(_direction);
					if (::fabs(rotate_angle_) > PIF/6 || is_horizontal) {
						if (is_horizontal) {
							rotate_angle_ = (rotate_angle_ < 0)? -1.3f : +1.3f;
						}
						SetMode(kModeRotateOnTheSpotDuring);
						return;
					}
					SetMode(kModeFindPathOffElevator);
					return;
				}
			} else if (mode_run_time > 4.5f) {
				// Crap, we missed it!
				log_.AHeadline("Must have missed the elevator (I'm still here!), waiting for it again!");
				SetMode(kModeWaitingForElevator);
				return;
			}

			if (mode_run_time > 0.8f) {
				// Check if we should adjust pos.
				const vec3 forward = game_->GetVehicle()->GetOrientation() * vec3(0,1,0);
				const float dist = elevator_get_on_position_.GetDistanceSquared(_position);
				if (dist > elevator_get_on_position_.GetDistanceSquared(_position+forward)) {
					game_->GetVehicle()->SetEnginePower(0, +strength);
					game_->GetVehicle()->SetEnginePower(2, 0);
				} else if (dist > elevator_get_on_position_.GetDistanceSquared(_position-forward)) {
					game_->GetVehicle()->SetEnginePower(0, -strength);
					game_->GetVehicle()->SetEnginePower(2, 0);
				}
			}
		} break;
		case kModeRotateOnTheSpot:
		case kModeRotateOnTheSpotDuring:
		case kModeRotateOnTheSpotWaiting: {
			float angle = rotate_angle_;
			const float min_angle = 0.3f;
			if (::fabs(angle) < min_angle) {
				angle = (angle < 0)? -min_angle : +min_angle;
			}
			float steer_end_time = 0.4f;
			float forward_end_time = steer_end_time + 0.9f;
			float other_steer_end_time = forward_end_time + steer_end_time;
			float period = other_steer_end_time + 0.8f;

			// A monster truck's steering impared.
			angle *= 2;
			steer_end_time = 0.7f;
			forward_end_time = steer_end_time + 0.7f;
			other_steer_end_time = forward_end_time + steer_end_time;
			period = other_steer_end_time + 1.0f;
			strength *= SMOOTH_BRAKING_FACTOR;
			if (mode_ == kModeRotateOnTheSpotWaiting) {
				const cure::Elevator* _nearest_elevator;
				vec3 _nearest_lift_position2d;
				float _elevator_xy_distance2_to_elevator_stop;
				if (HasElevatorArrived(_nearest_elevator, _position.z, _nearest_lift_position2d, _elevator_xy_distance2_to_elevator_stop)) {
					log_.AHeadline("Elevator arrived while rotating on the spot, getting on instead!");
					SetMode(kModeGetOnElevator);
					return;
				}
			}
			// Finish this rotation show if we're getting there.
			const int iterations = (mode_ == kModeRotateOnTheSpotWaiting)? 1 : 2;
			if (mode_run_time > iterations*period+steer_end_time) {
				game_->GetVehicle()->SetEnginePower(0, 0);
				game_->GetVehicle()->SetEnginePower(1, -angle);
				game_->GetVehicle()->SetEnginePower(2, -1);
				if (mode_ == kModeRotateOnTheSpot) {
					SetMode(kModeHeadingBackOnTrack);
				} else if (mode_ == kModeRotateOnTheSpotDuring) {
					SetMode(kModeFindPathOffElevator);
				} else {
					SetMode(kModeWaitingForElevator);
				}
				return;
			}
			for (int x = 0; x < iterations+1; ++x) {
				const float base = x*period;
				if (mode_run_time >= base && mode_run_time < base+steer_end_time) {
					// Brake and turn in "forward direction".
					game_->GetVehicle()->SetEnginePower(0, 0);
					game_->GetVehicle()->SetEnginePower(1, -angle);
					game_->GetVehicle()->SetEnginePower(2, -strength);
					break;
				} else if (mode_run_time >= base+steer_end_time && mode_run_time < base+forward_end_time) {
					// Drive forward.
					game_->GetVehicle()->SetEnginePower(0, +strength);
					game_->GetVehicle()->SetEnginePower(2, 0);
					break;
				} else if (mode_run_time >= base+forward_end_time && mode_run_time < base+other_steer_end_time) {
					// Brake and turn in "backward direction".
					game_->GetVehicle()->SetEnginePower(0, 0);
					game_->GetVehicle()->SetEnginePower(1, +angle);
					game_->GetVehicle()->SetEnginePower(2, -strength);
					break;
				} else if (mode_run_time >= base+other_steer_end_time && mode_run_time < base+period) {
					// Drive backward.
					game_->GetVehicle()->SetEnginePower(0, -0.7f*strength);
					game_->GetVehicle()->SetEnginePower(2, 0);
					break;
				}
			}
		} break;
	}
}

void VehicleAi::SetMode(Mode mode) {
	if (mode == previous_mode_ && mode != kModeNormal) {
		++stuck_count_;
	} else {
		stuck_count_ = 0;
	}
	if (stuck_count_ >= 2) {
		vec3 _velocity = game_->GetVehicle()->GetVelocity();
		_velocity.z = 0;
		if (_velocity.GetLengthSquared() < 2*2) {
			log_.AHeadline("Stuck in a vehicle AI loop, trying to break out!");
			if (mode != kModeFindBestPath && mode_ != kModeFindBestPath) {
				mode = kModeFindBestPath;
			} else {
				rotate_angle_ = (Random::Uniform(0.0f, 1.0f) > 0.5f)? +2.0f : -2.0f;
				mode = kModeRotateOnTheSpot;
			}
		}
		stuck_count_ = 0;
	} else if (mode == kModeFindPathOffElevator) {
		active_path_ = -1;	// Treat all paths as equals.
	} else if (mode == kModeHeadingBackOnTrack || mode == kModeNormal) {
		last_average_angle_ = 0;
	} else if (mode == kModeGetOffElevator) {
		elevator_get_off_position_ = game_->GetVehicle()->GetPosition();
	}
	stopped_frame_ = -1;
	previous_mode_ = mode_;
	mode_ = mode;
	mode_start_frame_ = GetManager()->GetGameManager()->GetTimeManager()->GetCurrentPhysicsFrame();
	const tchar* mode_name = "???";
	switch (mode_) {
		case kModeFindBestPath:		mode_name = "FIND BEST PATH";		break;
		case kModeFindPathOffElevator:	mode_name = "FIND PATH OFF ELEVATOR";	break;
		case kModeNormal:			mode_name = "NORMAL";			break;
		case kModeHeadingBackOnTrack:	mode_name = "HEADING BACK ON TRACK";	break;
		case kModeBackingUp:			mode_name = "BACKING UP";			break;
		case kModeBackingUpToGoal:		mode_name = "BACKING UP TO GOAL";		break;
		case kModeFlee:				mode_name = "FLEE";				break;
		case kModeStoppingAtGoal:		mode_name = "STOPPING AT GOAL";		break;
		case kModeAtGoal:			mode_name = "AT GOAL";			break;
		case kModeWaitingForElevator:		mode_name = "WAITING FOR ELEVATOR";		break;
		case kModeGetOnElevator:		mode_name = "GET ON ELEVATOR";		break;
		case kModeGetOffElevator:		mode_name = "GET OFF ELEVATOR";		break;
		case kModeOnElevator:			mode_name = "ON ELEVATOR";			break;
		case kModeRotateOnTheSpot:		mode_name = "ROTATE ON THE SPOT";		break;
		case kModeRotateOnTheSpotDuring:	mode_name = "ROTATE ON THE SPOT DURING";	break;
		case kModeRotateOnTheSpotWaiting:	mode_name = "ROTATE ON THE SPOT WAITING";	break;
	}
	log_.Headlinef("Switching mode to %s.", mode_name);
}

bool VehicleAi::IsCloseToTarget(const vec3& position, float distance) const {
	Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
	const vec3 target = _path->GetValue();
	const float target_distance2 = target.GetDistanceSquared(position);
	//log_.Headlinef("IsCloseToTarget ^2: %f.", target_distance2);
	const float goal_distance = distance*SCALE_FACTOR;
	return (target_distance2 <= goal_distance*goal_distance);
}

float VehicleAi::GetClosestPathDistance(const vec3& position, const int path, float* likeliness, float steep_factor) const {
	Spline* _path = game_->GetLevel()->QueryPath()->GetPath((path >= 0)? path : active_path_);
	if (likeliness) {
		*likeliness = _path->GetLikeliness();
	}
	const float current_time = _path->GetCurrentInterpolationTime();

	if (path < 0) {
		// We can assume the path "current" pointer is a bit ahead, so step back some to get a closer
		// approximation of where to start looking for our closest point on the spline.
		const float wanted_distance = AIM_DISTANCE;
		float delta_time = -wanted_distance * _path->GetDistanceNormal();
		if (current_time+delta_time < 0) {
			delta_time = -current_time;
		}
		_path->StepInterpolation(delta_time);
	} else {
		// Coarse check first to find a reasonable sample in the whole path.
		int best_sample = -1;
		float best_distance2 = 1e8f;
		const int steps = 5;
		for (int x = 0; x < 5; ++x) {
			const float sample_time = 1.0f/(steps+1) * (x+1);
			_path->GotoAbsoluteTime(sample_time);
			const float distance2 = _path->GetValue().GetDistanceSquared(position);
			if (distance2 < best_distance2) {
				best_sample = x;
				best_distance2 = distance2;
			}
		}
		const float best_time = 1.0f/(steps+1) * (best_sample+1);
		_path->GotoAbsoluteTime(best_time);
	}

	float nearest_distance;
	vec3 closest_point;
	const float search_step_length = (path >= 0)? -0.1f : 0.0125f;
	const int search_steps = (path >= 0)? 10 : 3;
	_path->FindNearestTime(search_step_length, position, nearest_distance, closest_point, search_steps);
	// Steep check.
	if (nearest_distance < ::fabs(position.z - closest_point.z)*3) {
		nearest_distance *= steep_factor;
	}

	if (path < 0) {
		// Step back to target point.
		_path->GotoAbsoluteTime(current_time);
	}

	return nearest_distance;
}

vec3 VehicleAi::GetClosestElevatorPosition(const vec3& position, const cure::Elevator*& nearest_elevator) const {
	nearest_elevator = 0;
	typedef cure::ContextManager::ContextObjectTable ContextTable;
	const ContextTable& object_table = GetManager()->GetObjectTable();
	ContextTable::const_iterator x = object_table.begin();
	const str elevator_class_id = "Elevator";
	float distance2 = -1;
	vec3 nearest_position;
	for (; x != object_table.end(); ++x) {
		const cure::ContextObject* object = x->second;
		if (object->GetClassId() != elevator_class_id) {
			continue;
		}
		const cure::Elevator* elevator = (const cure::Elevator*)object;
		const vec3 elevator_position = elevator->GetPosition();
		const float this_distance2 = elevator_position.GetDistanceSquared(position);
		if (distance2 < 0) {
			nearest_elevator = elevator;
			distance2 = this_distance2;
			nearest_position = elevator_position;
		} else if (this_distance2 < distance2) {
			nearest_elevator = elevator;
			distance2 = this_distance2;
			nearest_position = elevator_position;
		}
	}
	return nearest_position;
}

bool VehicleAi::HasElevatorArrived(const cure::Elevator*& nearest_elevator, const float position_z, vec3& nearest_lift_position2d, float& elevator_xy_distance2_to_elevator_stop) {
	const vec3 nearest_lift_position = GetClosestElevatorPosition(elevator_get_on_position_, nearest_elevator);
	nearest_lift_position2d = nearest_lift_position;
	nearest_lift_position2d.z = elevator_get_on_position_.z;
	elevator_xy_distance2_to_elevator_stop = elevator_get_on_position_.GetDistanceSquared(nearest_lift_position2d);
	return (nearest_lift_position.z < position_z+0.5f && elevator_xy_distance2_to_elevator_stop < 2*2);
}

float VehicleAi::GetClosestElevatorRadius() const {
	const cure::Elevator* _nearest_elevator;
	GetClosestElevatorPosition(elevator_get_on_position_, _nearest_elevator);
	return _nearest_elevator->GetRadius();
}

bool VehicleAi::IsVertical(const vec3& _vector) {
	return (::fabs(_vector.z) > 2 * (::fabs(_vector.x) + ::fabs(_vector.y)));
}

float VehicleAi::GetRelativeDriveOnAngle(const vec3& direction) const {
	deb_assert(active_path_ >= 0);
	if (active_path_ < 0) {
		return 0;
	}
	Spline* _path = game_->GetLevel()->QueryPath()->GetPath(active_path_);
	_path->GotoAbsoluteTime(0.95f);
	const vec3 p1 = _path->GetValue();
	_path->StepInterpolation(0.04f);
	const vec3 p2 = _path->GetValue();
	const vec3 wanted_direction = p2-p1;
	const float angle = LEPRA_XY_ANGLE(wanted_direction, direction);
	return angle;
}

bool VehicleAi::QueryVehicleHindered(const cure::TimeManager* time, const vec3& velocity) {
	const float slow_speed = 0.35f * SCALE_FACTOR;
	if (velocity.GetLengthSquared() < slow_speed*slow_speed) {
		if (stopped_frame_ == -1) {
			stopped_frame_ = time->GetCurrentPhysicsFrame();
		}
		const int stopped_delta_frame_count = time->GetCurrentPhysicsFrameDelta(stopped_frame_);
		const float stopped_time = time->ConvertPhysicsFramesToSeconds(stopped_delta_frame_count);
		if (stopped_time >= 1.0f) {
			return true;
		}
	} else {
		stopped_frame_ = -1;
	}
	return false;
}




loginstance(kGameContextCpp, VehicleAi);



}
