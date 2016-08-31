
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/unordered.h"
#include "tbc.h"
#include "physicsmanager.h"
#include "../../lepra/include/fastallocator.h"
#include "../../lepra/include/log.h"
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#include <ode/ode.h>
#pragma warning(pop)
#include <vector>


namespace tbc {



class PhysicsManagerODE: public PhysicsManager {
public:
	PhysicsManagerODE(float radius, int levels, float sensitivity);
	virtual ~PhysicsManagerODE();

	virtual void SetSimulationParameters(float softness, float rubberbanding, float accuracy);
	virtual bool InitCurrentThread();

	virtual int QueryRayCollisionAgainst(const vec3& ray_position, const vec3& ray_direction, float length, BodyID body, vec3* collision_points, int max_collision_count);
	virtual int QueryRayPick(const vec3& ray_position, const vec3& ray_direction, float length, int* force_feedback_ids, vec3* positions, int max_bodies);

	virtual BodyID CreateSphere(bool is_root, const xform& transform, float32 mass, float32 radius, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false);
	virtual BodyID CreateCylinder(bool is_root, const xform& transform, float32 mass, float32 radius, float32 length, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false);
	virtual BodyID CreateCapsule(bool is_root, const xform& transform, float32 mass, float32 radius, float32 length, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false);
	virtual BodyID CreateBox(bool is_root, const xform& transform, float32 mass, const Vector3D<float32>& size, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false);
	virtual BodyID CreateTriMesh(bool is_root, unsigned vertex_count, const float* vertices, unsigned triangle_count, const lepra::uint32* indices,
		const xform& transform, float32 mass, BodyType type, float32 friction = 1, float32 bounce = 0, int force_listener_id = 0, bool is_trigger = false);
	virtual bool Attach(BodyID static_body, BodyID main_body);
	virtual bool DetachToDynamic(BodyID static_body, float32 mass);
	virtual bool MakeStatic(BodyID dynamic_body);
	virtual bool AddMass(BodyID static_body, BodyID main_body);
	virtual bool Scale(BodyID body, const vec3& scale);

	virtual bool IsStaticBody(BodyID body_id) const;

	virtual void DeleteBody(BodyID body_id);

	virtual vec3 GetBodyPosition(BodyID body_id) const;
	virtual void SetBodyPosition(BodyID body_id, const vec3& position) const;
	virtual quat GetBodyOrientation(BodyID body_id) const;
	virtual void SetBodyOrientation(BodyID body_id, const quat& orientation);
	virtual void GetBodyTransform(BodyID body_id, xform& transform) const;
	virtual void SetBodyTransform(BodyID body_id, const xform& transform);
	virtual void GetBodyVelocity(BodyID body_id, vec3& velocity) const;
	virtual void SetBodyVelocity(BodyID body_id, const vec3& velocity);
	virtual void GetBodyForce(BodyID body_id, vec3& acceleration) const;
	virtual void SetBodyForce(BodyID body_id, const vec3& acceleration);
	virtual void GetBodyAcceleration(BodyID body_id, float total_mass, vec3& acceleration) const;
	virtual void SetBodyAcceleration(BodyID body_id, float total_mass, const vec3& acceleration);
	virtual void GetBodyAngularVelocity(BodyID body_id, vec3& angular_velocity) const;
	virtual void SetBodyAngularVelocity(BodyID body_id, const vec3& angular_velocity);
	virtual void GetBodyTorque(BodyID body_id, vec3& angular_acceleration) const;
	virtual void SetBodyTorque(BodyID body_id, const vec3& angular_acceleration);
	virtual void GetBodyAngularAcceleration(BodyID body_id, float total_mass, vec3& angular_acceleration) const;
	virtual void SetBodyAngularAcceleration(BodyID body_id, float total_mass, const vec3& angular_acceleration);

	virtual float GetBodyMass(BodyID body_id);
	virtual void SetBodyMass(BodyID body_id, float mass);
	virtual void MassAdjustBody(BodyID body_id);

	// Set and get user data.
	virtual void SetBodyData(BodyID body_id, void* user_data);
	virtual void* GetBodyData(BodyID body_id);

	virtual int GetTriggerListenerId(BodyID trigger);
	virtual int GetForceFeedbackListenerId(BodyID body);
	virtual void SetForceFeedbackListener(BodyID body, int force_feedback_id);

	//
	// Create/delete joints.
	//
	virtual JointID CreateBallJoint(BodyID body1, BodyID body2, const Vector3D<float32>& anchor_pos);
	virtual JointID CreateHingeJoint(BodyID body1, BodyID body2, const Vector3D<float32>& anchor_pos, const Vector3D<float32>& axis);
	virtual JointID CreateHinge2Joint(BodyID body1, BodyID body2, const Vector3D<float32>& anchor_pos, const Vector3D<float32>& axis1, const Vector3D<float32>& axis2);
	virtual JointID CreateUniversalJoint(BodyID body1, BodyID body2, const Vector3D<float32>& anchor_pos, const Vector3D<float32>& axis1, const Vector3D<float32>& axis2);
	virtual JointID CreateSliderJoint(BodyID body1, BodyID body2, const Vector3D<float32>& axis);
	virtual JointID CreateFixedJoint(BodyID body1, BodyID body2);
	virtual JointID CreateAngularMotorJoint(BodyID body1, BodyID body2, const Vector3D<float32>& axis);

	virtual void DeleteJoint(JointID joint_id);

	virtual bool StabilizeJoint(JointID joint_id);
	virtual void SetIsGyroscope(BodyID body_id, bool is_gyro);

	bool GetJoint1Diff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const;
	bool SetJoint1Diff(BodyID body_id, JointID joint_id, const Joint1Diff& diff);
	bool GetJoint2Diff(BodyID body_id, JointID joint_id, Joint2Diff& diff) const;
	bool SetJoint2Diff(BodyID body_id, JointID joint_id, const Joint2Diff& diff);
	bool GetJoint3Diff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const;
	bool SetJoint3Diff(BodyID body_id, JointID joint_id, const Joint3Diff& diff);

	// Returns true on success, false if joint is of wrong type.
	virtual bool GetAnchorPos(JointID joint_id, Vector3D<float32>& anchor_pos) const;
	virtual bool GetAxis1(JointID joint_id, Vector3D<float32>& axis1) const;
	virtual bool GetAxis2(JointID joint_id, Vector3D<float32>& axis1) const;
	virtual bool GetAngle1(JointID joint_id, float32& angle) const;
	virtual bool GetAngle2(JointID joint_id, float32& angle) const;
	virtual bool GetAngleRate1(JointID joint_id, float32& angle_rate) const;
	virtual bool GetAngleRate2(JointID joint_id, float32& angle_rate) const;
	virtual bool SetAngle1(BodyID body_id, JointID joint_id, float32 angle);

	virtual bool SetAngularMotorAngle(JointID joint_id, float32 angle);
	virtual bool SetAngularMotorSpeed(JointID joint_id, float32 speed);
	virtual bool SetMotorMaxForce(JointID joint_id, float32 max_force);
	virtual bool SetAngularMotorRoll(JointID joint_id, float32 max_force, float32 target_velocity);
	virtual bool GetAngularMotorRoll(JointID joint_id, float32& max_force, float32& target_velocity);
	virtual bool SetAngularMotorTurn(JointID joint_id, float32 max_force, float32 target_velocity);
	virtual bool GetAngularMotorAngle(JointID joint_id, float32& angle) const;
	virtual bool GetAngularMotorSpeed(JointID joint_id, float32& speed) const;
	virtual bool GetAngularMotorMaxForce(JointID joint_id, float32& max_force) const;
	virtual bool SetMotorTarget(JointID joint_id, float32 max_force, float32 target_velocity);

	virtual bool SetJointParams(JointID joint_id, float32 low_stop, float32 high_stop, float32 bounce, int extra_index);
	virtual bool GetJointParams(JointID joint_id, float32& low_stop, float32& high_stop, float32& bounce) const;
	virtual bool SetSuspension(JointID joint_id, float32 frame_time, float32 spring_constant, float32 damping_constant);
	virtual bool GetSuspension(JointID joint_id, float32& erp, float32& cfm) const;

	virtual bool GetSliderPos(JointID joint_id, float32& pos) const;
	virtual bool GetSliderSpeed(JointID joint_id, float32& speed) const;

	// Adding a force to a joint is only possible on slider joints.
	virtual bool AddJointForce(JointID joint_id, float32 force);
	virtual bool AddJointTorque(JointID joint_id, float32 torque);
	virtual bool AddJointTorque(JointID joint_id, float32 torque1, float32 torque2);

	virtual void AddForce            (BodyID body_id, const Vector3D<float32>& force);
	virtual void AddTorque           (BodyID body_id, const Vector3D<float32>& torque);
	virtual void AddRelForce         (BodyID body_id, const Vector3D<float32>& force);
	virtual void AddRelTorque        (BodyID body_id, const Vector3D<float32>& torque);
	virtual void AddForceAtPos       (BodyID body_id, const Vector3D<float32>& force, const Vector3D<float32>& pos);
	virtual void AddForceAtRelPos    (BodyID body_id, const Vector3D<float32>& force, const Vector3D<float32>& pos);
	virtual void AddRelForceAtPos    (BodyID body_id, const Vector3D<float32>& force, const Vector3D<float32>& pos);
	virtual void AddRelForceAtRelPos (BodyID body_id, const Vector3D<float32>& force, const Vector3D<float32>& pos);

	virtual void RestrictBody(BodyID body_id, float32 max_speed, float32 max_angular_speed);

	virtual void EnableGravity(BodyID body_id, bool enable);
	virtual void SetGravity(const Vector3D<float32>& gravity);
	virtual vec3 GetGravity() const;

	virtual void EnableCollideWithSelf(BodyID body_id, bool enable);

	virtual void PreSteps();
	virtual void StepAccurate(float32 step_size, bool collide);
	virtual void StepFast(float32 step_size, bool collide);
	virtual bool IsColliding(int force_feedback_id);
	virtual void PostSteps();

	virtual const BodySet& GetIdledBodies();

private:
	void FlagMovingObjects();
	void HandleMovableObjects();

	void NormalizeRotation(BodyID object);

	static void RayPickCallback(void* data_ptr, dGeomID o1, dGeomID o2);

	enum JointType {
		kJointInvalid = 0,
		kJointBall,
		kJointHinge,
		kJointHinge2,
		kJointUniversal,
		kJointSlider,
		kJointFixed,
		kJointAngularmotor,
		kJointContact,
	};

	struct Object {
		Object(dWorldID world_id, bool is_root) :
			world_id_(world_id),
			is_root_(is_root),
			collide_with_self_(false),
			body_id_(0),
			geom_id_(0),
			tri_mesh_id_(0),
			mass_(0),
			friction_(0),
			bounce_(0),
			user_data_(0),
			trigger_listener_id_(0),
			force_feedback_id_(0),
			has_mass_children_(false),
			is_rotational_(false),
			did_stop_(false) {
		}

		dWorldID world_id_;
		bool is_root_;
		bool collide_with_self_;
		dBodyID body_id_;
		dGeomID geom_id_;
		float geometry_data_[3];
		dTriMeshDataID tri_mesh_id_;
		float mass_;
		float32 friction_;
		float32 bounce_;
		void* user_data_;

		// The only thing that differs between standard bodies and triggers
		// is the value of this member. If this is 0, this is a regular
		// body, a trigger otherwise.
		int trigger_listener_id_;

		int force_feedback_id_;

		bool has_mass_children_;
		bool is_rotational_;
		bool did_stop_;
	};

	class TriggerInfo;
	class JointInfo;
	typedef std::unordered_set<Object*, LEPRA_VOIDP_HASHER> ObjectTable;
	typedef std::unordered_set<JointInfo*, LEPRA_VOIDP_HASHER> JointTable;
	typedef std::vector<TriggerInfo> TriggerInfoList;
	typedef std::vector<JointInfo*> JointList;

	class TriggerInfo {
	public:
		inline TriggerInfo(BodyID trigger_id, int trigger_listener_id, int body_listener_id, BodyID body_id, const vec3& position, const vec3& normal):
			trigger_id_(trigger_id),
			trigger_listener_id_(trigger_listener_id),
			body_listener_id_(body_listener_id),
			body_id_(body_id),
			position_(position),
			normal_(normal) {
		}

		BodyID trigger_id_;
		int trigger_listener_id_;
		int body_listener_id_;
		BodyID body_id_;
		vec3 position_;
		vec3 normal_;
	};

	class JointInfo {
	public:
		inline JointInfo() :
			body1_id_(0),
			body2_id_(0),
			listener_id1_(0),
			listener_id2_(0) {
		}

		inline bool IsBody1Static(PhysicsManagerODE* manager) const {
			return !body1_id_ || manager->IsStaticBody(body1_id_);
		}
		inline bool IsBody2Static(PhysicsManagerODE* manager) const {
			return !body2_id_ || manager->IsStaticBody(body2_id_);
		}

		dJointID joint_id_;
		JointType type_;
		BodyID body1_id_;
		BodyID body2_id_;
		dJointFeedback feedback_;
		int listener_id1_;
		int listener_id2_;
		vec3 position_;
		vec3 relative_velocity_;
	};

	void RemoveJoint(JointInfo* joint_info);

	bool GetHingeDiff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const;
	bool SetHingeDiff(BodyID body_id, JointID joint_id, const Joint1Diff& diff);
	bool GetSliderDiff(BodyID body_id, JointID joint_id, Joint1Diff& diff) const;
	bool SetSliderDiff(BodyID body_id, JointID joint_id, const Joint1Diff& diff);
	bool GetUniversalDiff(BodyID body_id, JointID joint_id, Joint2Diff& diff) const;
	bool SetUniversalDiff(BodyID body_id, JointID joint_id, const Joint2Diff& diff);
	bool GetHinge2Diff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const;
	bool SetHinge2Diff(BodyID body_id, JointID joint_id, const Joint3Diff& diff);
	bool GetBallDiff(BodyID body_id, JointID joint_id, Joint3Diff& diff) const;
	bool SetBallDiff(BodyID body_id, JointID joint_id, const Joint3Diff& diff);

	bool CheckBodies(BodyID& body1, BodyID& body2, Object*& object1, Object*& object2, const char* function);
	bool CheckBodies2(BodyID& body1, BodyID& body2, Object*& object1, Object*& object2, const char* function);

	void DoForceFeedback();

	static void CollisionCallback(void* data, dGeomID object1, dGeomID object2);
	static void CollisionNoteCallback(void* data, dGeomID object1, dGeomID object2);

	dWorldID world_id_;
	static float world_erp_;
	static float world_cfm_;
	dSpaceID space_id_;
	dJointGroupID contact_joint_group_id_;
	int note_force_feedback_id_;
	bool note_is_collided_;

	ObjectTable object_table_;
	BodySet auto_disabled_object_set_;
	JointTable joint_table_;

	TriggerInfoList trigger_info_list_;
	JointList feedback_joint_list_;

	FastAllocator<JointInfo> joint_info_allocator_;

	logclass();
};



}
