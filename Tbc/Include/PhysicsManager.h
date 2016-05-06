
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/unordered.h"
#include "../../lepra/include/transformation.h"
#include "../include/tbc.h"



namespace tbc {



class PhysicsManager {
public:
	typedef void* BodyID;
	typedef void* JointID;
	typedef void* GeomID;

	typedef std::unordered_set<BodyID> BodySet;

	class TriggerListener {
	public:
		virtual void OnTrigger(BodyID trigger, int trigger_listener_id, int other_object_id, BodyID body_id, const vec3& position, const vec3& normal) = 0;
	};

	class ForceFeedbackListener {
	public:
		virtual void OnForceApplied(int object_id, int other_object_id,
			BodyID body_id, BodyID other_body_id,
			const vec3& force, const vec3& torque,
			const vec3& position, const vec3& relative_velocity) = 0;
	};

	enum BodyType {
		kStatic = 1,	// Static world object.
		kDynamic,	// Dynamic object within the world.
	};

	struct Joint1Diff {
		Joint1Diff() {};
		Joint1Diff(float value, float velocity, float acceleration):
			value_(value),
			velocity_(velocity),
			acceleration_(acceleration) {
		}

		float value_;		// Could be angle or distance.
		float velocity_;	// Angular or linear.
		float acceleration_;	// Angular or linear.
	};

	struct Joint2Diff {
		Joint2Diff() {};
		Joint2Diff(float value, float angle,
			float value_velocity, float angle_velocity,
			float value_acceleration, float angle_acceleration):
			value_(value),
			angle_(angle),
			value_velocity_(value_velocity),
			angle_velocity_(angle_velocity),
			value_acceleration_(value_acceleration),
			angle_acceleration_(angle_acceleration) {
		}

		float value_;
		float angle_;
		float value_velocity_;
		float angle_velocity_;
		float value_acceleration_;
		float angle_acceleration_;
	};

	// Positive hinge-2 axis points from parent to child anchor.
	struct Joint3Diff {
		Joint3Diff() {};
		Joint3Diff(float value, float angle1, float angle2,
			float value_velocity, float angle1_velocity, float angle2_velocity,
			float value_acceleration, float angle1_acceleration, float angle2_acceleration):
			value_(value),
			angle1_(angle1),
			angle2_(angle2),
			value_velocity_(value_velocity),
			angle1_velocity_(angle1_velocity),
			angle2_velocity_(angle2_velocity),
			value_acceleration_(value_acceleration),
			angle1_acceleration_(angle1_acceleration),
			angle2_acceleration_(angle2_acceleration) {
		}

		float value_;
		float angle1_;
		float angle2_;
		float value_velocity_;
		float angle1_velocity_;
		float angle2_velocity_;
		float value_acceleration_;
		float angle1_acceleration_;
		float angle2_acceleration_;
	};

	PhysicsManager();
	virtual ~PhysicsManager();

	void SetTriggerListener(TriggerListener* trigger_callback);
	void SetForceFeedbackListener(ForceFeedbackListener* force_feedback_callback);

	// The parameters scale up, so normally a value in [0,1] should suffice.
	virtual void SetSimulationParameters(float softness, float rubberbanding, float accuracy) = 0;
	virtual bool InitCurrentThread() = 0;

	int QueryRayCollisionAgainst(const xform& ray_transform, float length, BodyID body, vec3* collision_points, int max_collision_count);
	virtual int QueryRayCollisionAgainst(const vec3& ray_position, const vec3& ray_direction, float length, BodyID body, vec3* collision_points, int max_collision_count) = 0;
	virtual int QueryRayPick(const vec3& ray_position, const vec3& ray_direction, float length, int* force_feedback_ids, vec3* positions, int max_bodies) = 0;

	// The "friction" parameter is a factor such that the friction coefficient
	// between two bodies is calculated as mu = body1.friction * body2.friction.
	// This is indeed a "hack", but it's there to make life easier writing games.
	//
	// The cylinder and capsule are both created along the local Z-axis.
	virtual BodyID CreateSphere(bool is_root, const xform& transform, float32 mass, float32 radius, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false) = 0;
	virtual BodyID CreateCylinder(bool is_root, const xform& transform, float32 mass, float32 radius, float32 length, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false) = 0;
	virtual BodyID CreateCapsule(bool is_root, const xform& transform, float32 mass, float32 radius, float32 length, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false) = 0;
	virtual BodyID CreateBox(bool is_root, const xform& transform, float32 mass, const vec3& size, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false) = 0;
	virtual bool Attach(BodyID static_body, BodyID main_body) = 0;
	virtual bool DetachToDynamic(BodyID static_body, float32 mass) = 0;
	virtual bool MakeStatic(BodyID dynamic_body) = 0;
	virtual bool AddMass(BodyID static_body, BodyID main_body) = 0;

	// Tri meshes are always static.
	virtual BodyID CreateTriMesh(bool is_root, unsigned vertex_count, const float* vertices,
		unsigned triangle_count, const lepra::uint32* indices,
		const xform& transform, float32 mass, BodyType type,
		float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false) = 0;

	virtual bool IsStaticBody(BodyID body_id) const = 0;

	virtual void DeleteBody(BodyID body_id) = 0;

	virtual vec3 GetBodyPosition(BodyID body_id) const = 0;
	virtual void SetBodyPosition(BodyID body_id, const vec3& position) const = 0;
	virtual quat GetBodyOrientation(BodyID body_id) const = 0;
	virtual void SetBodyOrientation(BodyID body_id, const quat& orientation) = 0;
	virtual void GetBodyTransform(BodyID body_id, xform& transform) const = 0;
	virtual void SetBodyTransform(BodyID body_id, const xform& transform) = 0;
	virtual void GetBodyVelocity(BodyID body_id, vec3& velocity) const = 0;
	virtual void SetBodyVelocity(BodyID body_id, const vec3& velocity) = 0;
	virtual void GetBodyForce(BodyID body_id, vec3& acceleration) const = 0;
	virtual void SetBodyForce(BodyID body_id, const vec3& acceleration) = 0;
	virtual void GetBodyAcceleration(BodyID body_id, float total_mass, vec3& acceleration) const = 0;
	virtual void SetBodyAcceleration(BodyID body_id, float total_mass, const vec3& acceleration) = 0;
	virtual void GetBodyAngularVelocity(BodyID body_id, vec3& angular_velocity) const = 0;
	virtual void SetBodyAngularVelocity(BodyID body_id, const vec3& angular_velocity) = 0;
	virtual void GetBodyTorque(BodyID body_id, vec3& angular_acceleration) const = 0;
	virtual void SetBodyTorque(BodyID body_id, const vec3& angular_acceleration) = 0;
	virtual void GetBodyAngularAcceleration(BodyID body_id, float total_mass, vec3& angular_acceleration) const = 0;	// TODO: don't assume spheric shapes!
	virtual void SetBodyAngularAcceleration(BodyID body_id, float total_mass, const vec3& angular_acceleration) = 0;	// TODO: don't assume spheric shapes!

	virtual float GetBodyMass(BodyID body_id) = 0;
	virtual void SetBodyMass(BodyID body_id, float mass) = 0;
	virtual void MassAdjustBody(BodyID body_id) = 0;

	// Set and get user data.
	virtual void SetBodyData(BodyID body_id, void* user_data) = 0;
	virtual void* GetBodyData(BodyID body_id) = 0;

	virtual int GetTriggerListenerId(BodyID body) = 0;
	virtual int GetForceFeedbackListenerId(BodyID body) = 0;
	virtual void SetForceFeedbackListener(BodyID body, int force_feedback_id) = 0;

	//
	// Create/delete joints.
	//

	virtual JointID CreateBallJoint(BodyID body1, BodyID body2, const vec3& anchor_pos) = 0;
	virtual JointID CreateHingeJoint(BodyID body1, BodyID body2, const vec3& anchor_pos, const vec3& axis) = 0;
	virtual JointID CreateHinge2Joint(BodyID body1, BodyID body2, const vec3& anchor_pos, const vec3& axis1, const vec3& axis2) = 0;
	virtual JointID CreateUniversalJoint(BodyID body1, BodyID body2, const vec3& anchor_pos, const vec3& axis1, const vec3& axis2) = 0;
	virtual JointID CreateSliderJoint(BodyID body1, BodyID body2, const vec3& axis) = 0;
	virtual JointID CreateFixedJoint(BodyID body1, BodyID body2) = 0;
	virtual JointID CreateAngularMotorJoint(BodyID body1, BodyID body2, const vec3& axis) = 0;

	virtual void DeleteJoint(JointID joint_id) = 0;

	virtual bool StabilizeJoint(JointID joint_id) = 0;
	virtual void SetIsGyroscope(BodyID body_id, bool is_gyro) = 0;

	virtual bool GetJoint1Diff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const = 0;
	virtual bool SetJoint1Diff(BodyID body_id, JointID joint_id, const Joint1Diff& diff) = 0;
	virtual bool GetJoint2Diff(BodyID body_id, JointID joint_id, Joint2Diff& diff) const = 0;
	virtual bool SetJoint2Diff(BodyID body_id, JointID joint_id, const Joint2Diff& diff) = 0;
	virtual bool GetJoint3Diff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const = 0;
	virtual bool SetJoint3Diff(BodyID body_id, JointID joint_id, const Joint3Diff& diff) = 0;

	// Returns true on success, false if joint is of wrong type.
	virtual bool GetAnchorPos(JointID joint_id, vec3& anchor_pos) const = 0;
	virtual bool GetAxis1(JointID joint_id, vec3& axis1) const = 0;
	virtual bool GetAxis2(JointID joint_id, vec3& axis1) const = 0;
	virtual bool GetAngle1(JointID joint_id, float32& angle) const = 0;
	virtual bool GetAngle2(JointID joint_id, float32& angle) const = 0;
	virtual bool GetAngleRate1(JointID joint_id, float32& angle_rate) const = 0;
	virtual bool GetAngleRate2(JointID joint_id, float32& angle_rate) const = 0;
	virtual bool SetAngle1(BodyID body_id, JointID joint_id, float32 angle) = 0;

	virtual bool SetAngularMotorAngle(JointID joint_id, float32 angle) = 0;
	virtual bool SetAngularMotorSpeed(JointID joint_id, float32 speed) = 0;
	virtual bool SetMotorMaxForce(JointID joint_id, float32 max_force) = 0;
	virtual bool SetAngularMotorRoll(JointID joint_id, float32 max_force, float32 target_velocity) = 0;
	virtual bool GetAngularMotorRoll(JointID joint_id, float32& max_force, float32& target_velocity) = 0;
	virtual bool SetAngularMotorTurn(JointID joint_id, float32 max_force, float32 target_velocity) = 0;
	virtual bool GetAngularMotorAngle(JointID joint_id, float32& angle) const = 0;
	virtual bool GetAngularMotorSpeed(JointID joint_id, float32& speed) const = 0;
	virtual bool GetAngularMotorMaxForce(JointID joint_id, float32& max_force) const = 0;
	virtual bool SetMotorTarget(JointID joint_id, float32 max_force, float32 target_velocity) = 0;

	virtual bool SetJointParams(JointID joint_id, float32 low_stop, float32 high_stop, float32 bounce, int extra_index = 0) = 0;
	virtual bool GetJointParams(JointID joint_id, float32& low_stop, float32& high_stop, float32& bounce) const = 0;
	virtual bool SetSuspension(JointID joint_id, float32 frame_time, float32 spring_constant, float32 damping_constant) = 0;
	virtual bool GetSuspension(JointID joint_id, float32& erp, float32& cfm) const = 0;

	virtual bool GetSliderPos(JointID joint_id, float32& pos) const = 0;
	virtual bool GetSliderSpeed(JointID joint_id, float32& speed) const = 0;

	// Adding a force to a joint is only possible on slider joints.
	virtual bool AddJointForce(JointID joint_id, float32 force) = 0;
	virtual bool AddJointTorque(JointID joint_id, float32 torque) = 0;
	virtual bool AddJointTorque(JointID joint_id, float32 torque1, float32 torque2) = 0;

	virtual void AddForce            (BodyID body_id, const vec3& force) = 0;
	virtual void AddTorque           (BodyID body_id, const vec3& torque) = 0;
	virtual void AddRelForce         (BodyID body_id, const vec3& force) = 0;
	virtual void AddRelTorque        (BodyID body_id, const vec3& torque) = 0;
	virtual void AddForceAtPos       (BodyID body_id, const vec3& force, const vec3& pos) = 0;
	virtual void AddForceAtRelPos    (BodyID body_id, const vec3& force, const vec3& pos) = 0;
	virtual void AddRelForceAtPos    (BodyID body_id, const vec3& force, const vec3& pos) = 0;
	virtual void AddRelForceAtRelPos (BodyID body_id, const vec3& force, const vec3& pos) = 0;

	virtual void RestrictBody(BodyID body_id, float32 max_speed, float32 max_angular_speed) = 0;

	virtual void EnableGravity(BodyID body_id, bool enable) = 0;
	virtual void SetGravity(const vec3& gravity) = 0;
	virtual vec3 GetGravity() const = 0;

	virtual void EnableCollideWithSelf(BodyID body_id, bool enable) = 0;

	virtual void PreSteps() = 0;
	virtual void StepAccurate(float32 step_size, bool collide) = 0;
	virtual void StepFast(float32 step_size, bool collide) = 0;
	virtual bool IsColliding(int force_feedback_id) = 0;
	virtual void PostSteps() = 0;

	// Returns the bodies that were "idled" last step.
	virtual const BodySet& GetIdledBodies() = 0;

protected:
	TriggerListener* trigger_callback_;
	ForceFeedbackListener* force_feedback_callback_;
};



const PhysicsManager::BodyID INVALID_BODY = 0;
const PhysicsManager::JointID INVALID_JOINT = 0;
const PhysicsManager::GeomID INVALID_GEOM = 0;



}
