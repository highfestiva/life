
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include <unordered_set>
#include "../../Lepra/Include/Transformation.h"
#include "../Include/TBC.h"



namespace TBC
{



class PhysicsManager
{
public:
	typedef void* BodyID;
	typedef void* JointID;
	typedef void* GeomID;
	typedef void* TriggerID;

	typedef std::unordered_set<BodyID> BodySet;

	class TriggerListener
	{
	public:
		virtual void OnTrigger(TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal) = 0;
	};

	class ForceFeedbackListener
	{
	public:
		virtual void OnForceApplied(int pObjectId, int pOtherObjectId,
			BodyID pBodyId, BodyID pOtherBodyId,
			const Vector3DF& pForce, const Vector3DF& pTorque,
			const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity) = 0;
	};

	enum BodyType
	{
		STATIC = 1,	// Static world object.
		DYNAMIC,	// Dynamic object within the world.
	};

	struct Joint1Diff
	{
		Joint1Diff() {};
		Joint1Diff(float pValue, float pVelocity, float pAcceleration):
			mValue(pValue),
			mVelocity(pVelocity),
			mAcceleration(pAcceleration)
		{
		}

		float mValue;		// Could be angle or distance.
		float mVelocity;	// Angular or linear.
		float mAcceleration;	// Angular or linear.
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

	PhysicsManager();
	virtual ~PhysicsManager();

	void SetTriggerListener(TriggerListener* pTriggerCallback);
	void SetForceFeedbackListener(ForceFeedbackListener* pForceFeedbackCallback);

	// The parameters scale up, so normally a value in [0,1] should suffice.
	virtual void SetSimulationParameters(float pSoftness, float pRubberbanding, float pAccuracy) = 0;
	virtual bool InitCurrentThread() = 0;

	int QueryRayCollisionAgainst(const TransformationF& pRayTransform, float pLength, BodyID pBody, Vector3DF* pCollisionPoints, int pMaxCollisionCount);
	virtual int QueryRayCollisionAgainst(const Vector3DF& pRayPosition, const Vector3DF& pRayDirection, float pLength, BodyID pBody, Vector3DF* pCollisionPoints, int pMaxCollisionCount) = 0;

	// The "friction" parameter is a factor such that the friction coefficient
	// between two bodies is calculated as mu = body1.friction * body2.friction.
	// This is indeed a "hack", but it's there to make life easier writing games.
	//
	// The cylinder and capsule are both created along the local Z-axis.
	virtual BodyID CreateSphere(bool pIsRoot, const TransformationF& pTransform, float32 pMass, float32 pRadius, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0) = 0;
	virtual BodyID CreateCylinder(bool pIsRoot, const TransformationF& pTransform, float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0) = 0;
	virtual BodyID CreateCapsule(bool pIsRoot, const TransformationF& pTransform, float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0) = 0;
	virtual BodyID CreateBox(bool pIsRoot, const TransformationF& pTransform, float32 pMass, const Vector3DF& pSize, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0) = 0;
	virtual bool Attach(BodyID pStaticBody, BodyID pMainBody) = 0;
	virtual bool DetachToDynamic(BodyID pStaticBody, float32 pMass) = 0;

	// Tri meshes are always static.
	virtual BodyID CreateTriMesh(bool pIsRoot, unsigned pVertexCount, const float* pVertices,
		unsigned pTriangleCount, const Lepra::uint32* pIndices,
		const TransformationF& pTransform, float32 pFriction = 1,
		float32 pBounce = 0, int pForceListenerId = 0) = 0;

	virtual bool IsStaticBody(BodyID pBodyId) const = 0;

	virtual void DeleteBody(BodyID pBodyId) = 0;

	virtual Vector3DF GetBodyPosition(BodyID pBodyId) const = 0;
	virtual void SetBodyPosition(BodyID pBodyId, const Vector3DF& pPosition) const = 0;
	virtual QuaternionF GetBodyOrientation(BodyID pBodyId) const = 0;
	virtual void GetBodyTransform(BodyID pBodyId, TransformationF& pTransform) const = 0;
	virtual void SetBodyTransform(BodyID pBodyId, const TransformationF& pTransform) = 0;
	virtual void GetBodyVelocity(BodyID pBodyId, Vector3DF& pVelocity) const = 0;
	virtual void SetBodyVelocity(BodyID pBodyId, const Vector3DF& pVelocity) = 0;
	virtual void GetBodyForce(BodyID pBodyId, Vector3DF& pAcceleration) const = 0;
	virtual void SetBodyForce(BodyID pBodyId, const Vector3DF& pAcceleration) = 0;
	virtual void GetBodyAcceleration(BodyID pBodyId, float pTotalMass, Vector3DF& pAcceleration) const = 0;
	virtual void SetBodyAcceleration(BodyID pBodyId, float pTotalMass, const Vector3DF& pAcceleration) = 0;
	virtual void GetBodyAngularVelocity(BodyID pBodyId, Vector3DF& pAngularVelocity) const = 0;
	virtual void SetBodyAngularVelocity(BodyID pBodyId, const Vector3DF& pAngularVelocity) = 0;
	virtual void GetBodyTorque(BodyID pBodyId, Vector3DF& pAngularAcceleration) const = 0;
	virtual void SetBodyTorque(BodyID pBodyId, const Vector3DF& pAngularAcceleration) = 0;
	virtual void GetBodyAngularAcceleration(BodyID pBodyId, float pTotalMass, Vector3DF& pAngularAcceleration) const = 0;	// TODO: don't assume spheric shapes!
	virtual void SetBodyAngularAcceleration(BodyID pBodyId, float pTotalMass, const Vector3DF& pAngularAcceleration) = 0;	// TODO: don't assume spheric shapes!

	virtual float GetBodyMass(BodyID pBodyId) = 0;
	virtual void MassAdjustBody(BodyID pBodyId) = 0;

	// Set and get user data.
	virtual void SetBodyData(BodyID pBodyId, void* pUserData) = 0;
	virtual void* GetBodyData(BodyID pBodyId) = 0;

	//
	// Create/delete triggers. A trigger is collision geometry which doesn't
	// affect the simulation. It's only purpose is to tell the listener
	// when an object intersects the trigger volume.
	//
	virtual TriggerID CreateSphereTrigger(const TransformationF& pTransform, float32 pRadius, int pTriggerListenerId) = 0;
	virtual TriggerID CreateCylinderTrigger(const TransformationF& pTransform, float32 pRadius, float32 pLength, int pTriggerListenerId) = 0;
	virtual TriggerID CreateCapsuleTrigger(const TransformationF& pTransform, float32 pRadius, float32 pLength, int pTriggerListenerId) = 0;
	virtual TriggerID CreateBoxTrigger(const TransformationF& pTransform, const Vector3DF& pSize, int pTriggerListenerId) = 0;
	virtual TriggerID CreateRayTrigger(const TransformationF& pTransform, const Vector3DF& pFromPos, const Vector3DF& pToPos, int pTriggerListenerId) = 0;

	virtual void DeleteTrigger(TriggerID pTriggerID) = 0;

	virtual int GetTriggerListenerId(TriggerID pTrigger) = 0;
	virtual int GetForceFeedbackListenerId(BodyID pBody) = 0;
	virtual void SetForceFeedbackListener(BodyID pBody, int pForceFeedbackId) = 0;

	virtual void GetTriggerTransform(TriggerID pTriggerID, TransformationF& pTransform) = 0;
	virtual void SetTriggerTransform(TriggerID pTriggerID, const TransformationF& pTransform) = 0;

	//
	// Create/delete joints.
	//

	virtual JointID CreateBallJoint(BodyID pBody1, BodyID pBody2, const Vector3DF& pAnchorPos) = 0;
	virtual JointID CreateHingeJoint(BodyID pBody1, BodyID pBody2, const Vector3DF& pAnchorPos, const Vector3DF& pAxis) = 0;
	virtual JointID CreateHinge2Joint(BodyID pBody1, BodyID pBody2, const Vector3DF& pAnchorPos, const Vector3DF& pAxis1, const Vector3DF& pAxis2) = 0;
	virtual JointID CreateUniversalJoint(BodyID pBody1, BodyID pBody2, const Vector3DF& pAnchorPos, const Vector3DF& pAxis1, const Vector3DF& pAxis2) = 0;
	virtual JointID CreateSliderJoint(BodyID pBody1, BodyID pBody2, const Vector3DF& pAxis) = 0;
	virtual JointID CreateFixedJoint(BodyID pBody1, BodyID pBody2) = 0;
	virtual JointID CreateAngularMotorJoint(BodyID pBody1, BodyID pBody2, const Vector3DF& pAxis) = 0;

	virtual void DeleteJoint(JointID pJointId) = 0;

	virtual bool StabilizeJoint(JointID pJointId) = 0;
	virtual void SetIsGyroscope(BodyID pBodyId, bool pIsGyro) = 0;

	virtual bool GetJoint1Diff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const = 0;
	virtual bool SetJoint1Diff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff) = 0;
	virtual bool GetJoint2Diff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const = 0;
	virtual bool SetJoint2Diff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff) = 0;
	virtual bool GetJoint3Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const = 0;
	virtual bool SetJoint3Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff) = 0;

	// Returns true on success, false if joint is of wrong type.
	virtual bool GetAnchorPos(JointID pJointId, Vector3DF& pAnchorPos) const = 0;
	virtual bool GetAxis1(JointID pJointId, Vector3DF& pAxis1) const = 0;
	virtual bool GetAxis2(JointID pJointId, Vector3DF& pAxis1) const = 0;
	virtual bool GetAngle1(JointID pJointId, float32& pAngle) const = 0;
	virtual bool GetAngle2(JointID pJointId, float32& pAngle) const = 0;
	virtual bool GetAngleRate1(JointID pJointId, float32& pAngleRate) const = 0;
	virtual bool GetAngleRate2(JointID pJointId, float32& pAngleRate) const = 0;
	virtual bool SetAngle1(BodyID pBodyId, JointID pJointId, float32 pAngle) = 0;

	virtual bool SetAngularMotorAngle(JointID pJointId, float32 pAngle) = 0;
	virtual bool SetAngularMotorSpeed(JointID pJointId, float32 pSpeed) = 0;
	virtual bool SetMotorMaxForce(JointID pJointId, float32 pMaxForce) = 0;
	virtual bool SetAngularMotorRoll(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity) = 0;
	virtual bool GetAngularMotorRoll(JointID pJointId, float32& pMaxForce, float32& pTargetVelocity) = 0;
	virtual bool SetAngularMotorTurn(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity) = 0;
	virtual bool GetAngularMotorAngle(JointID pJointId, float32& pAngle) const = 0;
	virtual bool GetAngularMotorSpeed(JointID pJointId, float32& pSpeed) const = 0;
	virtual bool GetAngularMotorMaxForce(JointID pJointId, float32& pMaxForce) const = 0;
	virtual bool SetMotorTarget(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity) = 0;

	virtual bool SetJointParams(JointID pJointId, float32 pLowStop, float32 pHighStop, float32 pBounce, int pExtraIndex = 0) = 0;
	virtual bool GetJointParams(JointID pJointId, float32& pLowStop, float32& pHighStop, float32& pBounce) const = 0;
	virtual bool SetSuspension(JointID pJointId, float32 pFrameTime, float32 pSpringConstant, float32 pDampingConstant) = 0;
	virtual bool GetSuspension(JointID pJointId, float32& pErp, float32& pCfm) const = 0;

	virtual bool GetSliderPos(JointID pJointId, float32& pPos) const = 0;
	virtual bool GetSliderSpeed(JointID pJointId, float32& pSpeed) const = 0;

	// Adding a force to a joint is only possible on slider joints.
	virtual bool AddJointForce(JointID pJointId, float32 pForce) = 0;
	virtual bool AddJointTorque(JointID pJointId, float32 pTorque) = 0;
	virtual bool AddJointTorque(JointID pJointId, float32 pTorque1, float32 pTorque2) = 0;

	virtual void AddForce            (BodyID pBodyId, const Vector3DF& pForce) = 0;
	virtual void AddTorque           (BodyID pBodyId, const Vector3DF& pTorque) = 0;
	virtual void AddRelForce         (BodyID pBodyId, const Vector3DF& pForce) = 0;
	virtual void AddRelTorque        (BodyID pBodyId, const Vector3DF& pTorque) = 0;
	virtual void AddForceAtPos       (BodyID pBodyId, const Vector3DF& pForce, const Vector3DF& pPos) = 0;
	virtual void AddForceAtRelPos    (BodyID pBodyId, const Vector3DF& pForce, const Vector3DF& pPos) = 0;
	virtual void AddRelForceAtPos    (BodyID pBodyId, const Vector3DF& pForce, const Vector3DF& pPos) = 0;
	virtual void AddRelForceAtRelPos (BodyID pBodyId, const Vector3DF& pForce, const Vector3DF& pPos) = 0;

	virtual void RestrictBody(BodyID pBodyId, float32 pMaxSpeed, float32 pMaxAngularSpeed) = 0;

	virtual void EnableGravity(BodyID pBodyId, bool pEnable) = 0;
	virtual void SetGravity(const Vector3DF& pGravity) = 0;
	virtual Vector3DF GetGravity() const = 0;

	virtual void EnableTriggerBySelf(TriggerID pTriggerId, bool pEnable) = 0;
	virtual void EnableCollideWithSelf(BodyID pBodyId, bool pEnable) = 0;

	virtual void PreSteps() = 0;
	virtual void StepAccurate(float32 pStepSize, bool pCollide) = 0;
	virtual void StepFast(float32 pStepSize, bool pCollide) = 0;
	virtual bool IsColliding(int pForceFeedbackId) = 0;
	virtual void PostSteps() = 0;

	// Returns the bodies that were "idled" last step.
	virtual const BodySet& GetIdledBodies() const = 0;

protected:
	TriggerListener* mTriggerCallback;
	ForceFeedbackListener* mForceFeedbackCallback;
};



const PhysicsManager::BodyID INVALID_BODY = 0;
const PhysicsManager::JointID INVALID_JOINT = 0;
const PhysicsManager::GeomID INVALID_GEOM = 0;
const PhysicsManager::TriggerID INVALID_TRIGGER = 0;



}
