
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include <hash_set>
#include "../../Lepra/Include/Transformation.h"



namespace TBC
{



class GeometryBase;



class PhysicsManager
{
public:
	typedef void* BodyID;
	typedef void* JointID;
	typedef void* GeomID;
	typedef void* TriggerID;

	typedef std::hash_set<BodyID> BodySet;

	enum BodyType
	{
		STATIC = 0,	// Static world object.
		DYNAMIC,	// Dynamic object within the world.
	};

	class TriggerListener
	{
	public:
		virtual void OnTrigger(BodyID pBody1, BodyID pBody2) = 0;
	};

	class ForceFeedbackListener
	{
	public:
		virtual void OnForceApplied(ForceFeedbackListener* pOtherObject,
			const Lepra::Vector3D<Lepra::float32>& pForce,
			const Lepra::Vector3D<Lepra::float32>& pTorque) = 0;
	};

	struct Joint1Diff
	{
		Joint1Diff() {};
		Joint1Diff(float pAngle, float pAngleVelocity, float pAngleAcceleration):
			mAngle(pAngle),
			mAngleVelocity(pAngleVelocity),
			mAngleAcceleration(pAngleAcceleration)
		{
		}

		float mAngle;
		float mAngleVelocity;
		float mAngleAcceleration;
	};

	struct Joint2Diff
	{
		Joint2Diff() {};
		Joint2Diff(float pValue, float pAngle,
			float pValueVelocity, float pAngleVelocity,
			float pValueAcceleration, float pAngleAcceleration):
			mValue(pValue),
			mAngle(pAngle),
			mValueVelocity(pValueVelocity),
			mAngleVelocity(pAngleVelocity),
			mValueAcceleration(pValueAcceleration),
			mAngleAcceleration(pAngleAcceleration)
		{
		}

		float mValue;
		float mAngle;
		float mValueVelocity;
		float mAngleVelocity;
		float mValueAcceleration;
		float mAngleAcceleration;
	};

	// Positive hinge-2 axis points from parent to child anchor.
	struct Joint3Diff
	{
		Joint3Diff() {};
		Joint3Diff(float pValue, float pAngle1, float pAngle2,
			float pValueVelocity, float pAngle1Velocity, float pAngle2Velocity,
			float pValueAcceleration, float pAngle1Acceleration, float pAngle2Acceleration):
			mValue(pValue),
			mAngle1(pAngle1),
			mAngle2(pAngle2),
			mValueVelocity(pValueVelocity),
			mAngle1Velocity(pAngle1Velocity),
			mAngle2Velocity(pAngle2Velocity),
			mValueAcceleration(pValueAcceleration),
			mAngle1Acceleration(pAngle1Acceleration),
			mAngle2Acceleration(pAngle2Acceleration)
		{
		}

		float mValue;
		float mAngle1;
		float mAngle2;
		float mValueVelocity;
		float mAngle1Velocity;
		float mAngle2Velocity;
		float mValueAcceleration;
		float mAngle1Acceleration;
		float mAngle2Acceleration;
	};

	virtual ~PhysicsManager(){};

	// The "friction" parameter is a factor such that the friction coefficient
	// between two bodies is calculated as mu = body1.friction * body2.friction.
	// This is indeed a "hack", but it's there to make life easier writing games.
	//
	// The cylinder and capsule are both created along the local Z-axis.
	virtual BodyID CreateSphere(bool pIsRoot, const Lepra::Transformation<Lepra::float32>& pTransform,
		Lepra::float32 pMass, Lepra::float32 pRadius, BodyType pType, Lepra::float32 pFriction = 1,
		Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0,
		ForceFeedbackListener* pForceListener = 0) = 0;
	virtual BodyID CreateCylinder(bool pIsRoot, const Lepra::Transformation<Lepra::float32>& pTransform,
		Lepra::float32 pMass, Lepra::float32 pRadius, Lepra::float32 pLength, BodyType pType,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0,
		ForceFeedbackListener* pForceListener = 0) = 0;
	virtual BodyID CreateCapsule(bool pIsRoot, const Lepra::Transformation<Lepra::float32>& pTransform,
		Lepra::float32 pMass, Lepra::float32 pRadius, Lepra::float32 pLength, BodyType pType,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0,
		ForceFeedbackListener* pForceListener = 0) = 0;
	virtual BodyID CreateBox(bool pIsRoot, const Lepra::Transformation<Lepra::float32>& pTransform,
		Lepra::float32 pMass, const Lepra::Vector3D<Lepra::float32>& pSize, BodyType pType,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0,
		ForceFeedbackListener* pForceListener = 0) = 0;
	virtual bool Attach(BodyID pStaticBody, BodyID pMainBody) = 0;

	// Tri meshes are always static.
	virtual BodyID CreateTriMesh(bool pIsRoot, const GeometryBase* pMesh,
		const Lepra::Transformation<Lepra::float32>& pTransform, Lepra::float32 pFriction = 1,
		Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0,
		ForceFeedbackListener* pForceListener = 0) = 0;

	virtual bool IsStaticBody(BodyID pBodyId) const = 0;

	virtual void DeleteBody(BodyID pBodyId) = 0;

	virtual Lepra::Vector3DF GetBodyPosition(BodyID pBodyId) const = 0;
	virtual void GetBodyTransform(BodyID pBodyId, Lepra::Transformation<Lepra::float32>& pTransform) const = 0;
	virtual void SetBodyTransform(BodyID pBodyId, const Lepra::Transformation<Lepra::float32>& pTransform) = 0;
	virtual void GetBodyVelocity(BodyID pBodyId, Lepra::Vector3DF& pVelocity) const = 0;
	virtual void SetBodyVelocity(BodyID pBodyId, const Lepra::Vector3DF& pVelocity) = 0;
	virtual void GetBodyAcceleration(BodyID pBodyId, Lepra::Vector3DF& pAcceleration) const = 0;
	virtual void SetBodyAcceleration(BodyID pBodyId, const Lepra::Vector3DF& pAcceleration) = 0;
	virtual void GetBodyAngularVelocity(BodyID pBodyId, Lepra::Vector3DF& pAngularVelocity) const = 0;
	virtual void SetBodyAngularVelocity(BodyID pBodyId, const Lepra::Vector3DF& pAngularVelocity) = 0;
	virtual void GetBodyAngularAcceleration(BodyID pBodyId, Lepra::Vector3DF& pAngularAcceleration) const = 0;
	virtual void SetBodyAngularAcceleration(BodyID pBodyId, const Lepra::Vector3DF& pAngularAcceleration) = 0;

	virtual float GetBodyMass(BodyID pBodyId) = 0;

	// Set and get user data.
	virtual void SetBodyData(BodyID pBodyId, void* pUserData) = 0;
	virtual void* GetBodyData(BodyID pBodyId) = 0;

	//
	// Create/delete triggers. A trigger is collision geometry which doesn't
	// affect the simulation. It's only purpose is to tell the listener
	// when an object intersects the trigger volume.
	//
	virtual TriggerID CreateSphereTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
						  Lepra::float32 pRadius,
						  TriggerListener* pListener) = 0;
	virtual TriggerID CreateCylinderTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
						Lepra::float32 pRadius,
						Lepra::float32 pLength,
						TriggerListener* pListener) = 0;
	virtual TriggerID CreateCapsuleTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
						Lepra::float32 pRadius,
						Lepra::float32 pLength,
						TriggerListener* pListener) = 0;
	virtual TriggerID CreateBoxTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
						const Lepra::Vector3D<Lepra::float32>& pSize,
						TriggerListener* pListener) = 0;
	virtual TriggerID CreateRayTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
						const Lepra::Vector3D<Lepra::float32>& pFromPos,
						const Lepra::Vector3D<Lepra::float32>& pToPos,
						TriggerListener* pListener) = 0;

	virtual void DeleteTrigger(TriggerID pTriggerID) = 0;

	virtual ForceFeedbackListener* GetForceFeedbackListener(BodyID pBody) = 0;

	virtual void GetTriggerTransform(TriggerID pTriggerID, Lepra::Transformation<Lepra::float32>& pTransform) = 0;
	virtual void SetTriggerTransform(TriggerID pTriggerID, const Lepra::Transformation<Lepra::float32>& pTransform) = 0;

	//
	// Create/delete joints.
	//

	virtual JointID CreateBallJoint(BodyID pBody1, 
					BodyID pBody2, 
					const Lepra::Vector3D<Lepra::float32>& pAnchorPos) = 0;
	virtual JointID CreateHingeJoint(BodyID pBody1, 
					 BodyID pBody2, 
					 const Lepra::Vector3D<Lepra::float32>& pAnchorPos, 
					 const Lepra::Vector3D<Lepra::float32>& pAxis) = 0;
	virtual JointID CreateHinge2Joint(BodyID pBody1, 
					  BodyID pBody2, 
					  const Lepra::Vector3D<Lepra::float32>& pAnchorPos, 
					  const Lepra::Vector3D<Lepra::float32>& pAxis1, 
					  const Lepra::Vector3D<Lepra::float32>& pAxis2) = 0;
	virtual JointID CreateUniversalJoint(BodyID pBody1, 
						 BodyID pBody2, 
						 const Lepra::Vector3D<Lepra::float32>& pAnchorPos, 
						 const Lepra::Vector3D<Lepra::float32>& pAxis1, 
						 const Lepra::Vector3D<Lepra::float32>& pAxis2) = 0;
	virtual JointID CreateSliderJoint(BodyID pBody1, 
					  BodyID pBody2, 
					  const Lepra::Vector3D<Lepra::float32>& pAxis) = 0;
	virtual JointID CreateFixedJoint(BodyID pBody1, 
					 BodyID pBody2) = 0;
	virtual JointID CreateAngularMotorJoint(BodyID pBody1, 
						BodyID pBody2, 
						const Lepra::Vector3D<Lepra::float32>& pAxis) = 0;

	virtual void DeleteJoint(JointID pJointId) = 0;

	virtual bool StabilizeJoint(JointID pJointId) = 0;

	virtual bool GetJoint1Diff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const = 0;
	virtual bool SetJoint1Diff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff) = 0;
	virtual bool GetJoint2Diff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const = 0;
	virtual bool SetJoint2Diff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff) = 0;
	virtual bool GetJoint3Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const = 0;
	virtual bool SetJoint3Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff) = 0;

	// Returns true on success, false if joint is of wrong type.
	virtual bool GetAnchorPos(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAnchorPos) const = 0;
	virtual bool GetAxis1(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAxis1) const = 0;
	virtual bool GetAxis2(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAxis1) const = 0;
	virtual bool GetAngle1(JointID pJointId, Lepra::float32& pAngle) const = 0;
	virtual bool GetAngle2(JointID pJointId, Lepra::float32& pAngle) const = 0;
	virtual bool GetAngleRate1(JointID pJointId, Lepra::float32& pAngleRate) const = 0;
	virtual bool GetAngleRate2(JointID pJointId, Lepra::float32& pAngleRate) const = 0;
	virtual bool SetAngle1(BodyID pBodyId, JointID pJointId, Lepra::float32 pAngle) = 0;

	virtual bool SetAngularMotorAngle(JointID pJointId, Lepra::float32 pAngle) = 0;
	virtual bool SetAngularMotorSpeed(JointID pJointId, Lepra::float32 pSpeed) = 0;
	virtual bool SetAngularMotorMaxForce(JointID pJointId, Lepra::float32 pMaxForce) = 0;
	virtual bool SetAngularMotorRoll(JointID pJointId, Lepra::float32 pMaxForce, Lepra::float32 pTargetVelocity) = 0;
	virtual bool GetAngularMotorRoll(JointID pJointId, Lepra::float32& pMaxForce, Lepra::float32& pTargetVelocity) = 0;
	virtual bool SetAngularMotorTurn(JointID pJointId, Lepra::float32 pMaxForce, Lepra::float32 pTargetVelocity) = 0;
	virtual bool GetAngularMotorAngle(JointID pJointId, Lepra::float32& pAngle) const = 0;
	virtual bool GetAngularMotorSpeed(JointID pJointId, Lepra::float32& pSpeed) const = 0;
	virtual bool GetAngularMotorMaxForce(JointID pJointId, Lepra::float32& pMaxForce) const = 0;

	virtual bool SetJointParams(JointID pJointId, Lepra::float32 pLowStop, Lepra::float32 pHighStop, Lepra::float32 pBounce) = 0;
	virtual bool GetJointParams(JointID pJointId, Lepra::float32& pLowStop, Lepra::float32& pHighStop, Lepra::float32& pBounce) const = 0;
	virtual bool SetSuspension(JointID pJointId, Lepra::float32 pFrameTime, Lepra::float32 pSpringConstant, Lepra::float32 pDampingConstant) = 0;
	virtual bool GetSuspension(JointID pJointId, Lepra::float32& pErp, Lepra::float32& pCfm) const = 0;

	virtual bool GetSliderPos(JointID pJointId, Lepra::float32& pPos) const = 0;
	virtual bool GetSliderSpeed(JointID pJointId, Lepra::float32& pSleep) const = 0;

	// Adding a force to a joint is only possible on slider joints.
	virtual bool AddJointForce(JointID pJointId, Lepra::float32 pForce) = 0;
	virtual bool AddJointTorque(JointID pJointId, Lepra::float32 pTorque) = 0;
	virtual bool AddJointTorque(JointID pJointId, Lepra::float32 pTorque1, Lepra::float32 pTorque2) = 0;

	virtual void AddForce            (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce) = 0;
	virtual void AddTorque           (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pTorque) = 0;
	virtual void AddRelForce         (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce) = 0;
	virtual void AddRelTorque        (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pTorque) = 0;
	virtual void AddForceAtPos       (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos) = 0;
	virtual void AddForceAtRelPos    (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos) = 0;
	virtual void AddRelForceAtPos    (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos) = 0;
	virtual void AddRelForceAtRelPos (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos) = 0;

	virtual void RestrictBody(BodyID pBodyId, Lepra::float32 pMaxSpeed, Lepra::float32 pMaxAngularSpeed) = 0;

	virtual void ActivateGravity(BodyID pBodyId) = 0;
	virtual void DeactivateGravity(BodyID pBodyId) = 0;
	virtual void SetGravity(const Lepra::Vector3D<Lepra::float32>& pGravity) = 0;
	virtual Lepra::Vector3DF GetGravity() const = 0;

	virtual void StepAccurate(Lepra::float32 pStepSize) = 0;
	virtual void StepFast(Lepra::float32 pStepSize)     = 0;

	// Returns the bodies that were "idled" last step.
	virtual const BodySet& GetIdledBodies() const = 0;

protected:
};



const PhysicsManager::BodyID INVALID_BODY = 0;
const PhysicsManager::JointID INVALID_JOINT = 0;
const PhysicsManager::GeomID INVALID_GEOM = 0;
const PhysicsManager::TriggerID INVALID_TRIGGER = 0;



}
