
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include <hash_set>
#include "TBC.h"
#include "PhysicsEngine.h"
#include "../../Lepra/Include/FastAllocator.h"
#include "../../Lepra/Include/Log.h"
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#include <ode/ode.h>
#pragma warning(pop)
#include <list>


namespace TBC
{



class PhysicsEngineODE: public PhysicsEngine
{
public:
	PhysicsEngineODE();
	virtual ~PhysicsEngineODE();

	virtual BodyID CreateSphere(const Lepra::TransformationF& pTransform, Lepra::float32 pMass,
		Lepra::float32 pRadius, BodyType pType, Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0,
		TriggerListener* pTriggerListener = 0, ForceFeedbackListener* pListener = 0);
	virtual BodyID CreateCylinder(const Lepra::TransformationF& pTransform, Lepra::float32 pMass,
		Lepra::float32 pRadius, Lepra::float32 pLength, BodyType pType, Lepra::float32 pFriction = 1,
		Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0, ForceFeedbackListener* pListener = 0);
	virtual BodyID CreateCapsule(const Lepra::TransformationF& pTransform, Lepra::float32 pMass,
		Lepra::float32 pRadius, Lepra::float32 pLength, BodyType pType, Lepra::float32 pFriction = 1,
		Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0, ForceFeedbackListener* pListener = 0);
	virtual BodyID CreateBox(const Lepra::TransformationF& pTransform, Lepra::float32 pMass,
		const Lepra::Vector3D<Lepra::float32>& pSize, BodyType pType, Lepra::float32 pFriction = 1,
		Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0, ForceFeedbackListener* pListener = 0);
	virtual bool Attach(BodyID pStaticBody, BodyID pMainBody);

	// Tri meshes are always static.
	virtual BodyID CreateTriMesh(const GeometryBase* pMesh, const Lepra::TransformationF& pTransform,
		Lepra::float32 pFriction = 1, Lepra::float32 pBounce = 0, TriggerListener* pTriggerListener = 0,
		ForceFeedbackListener* pListener = 0);

	virtual bool IsStaticBody(BodyID pBodyId) const;

	virtual void DeleteBody(BodyID pBodyId);

	Lepra::Vector3DF GetBodyPosition(BodyID pBodyId) const;
	void GetBodyTransform(BodyID pBodyId, Lepra::TransformationF& pTransform) const;
	void SetBodyTransform(BodyID pBodyId, const Lepra::TransformationF& pTransform);
	void GetBodyVelocity(BodyID pBodyId, Lepra::Vector3DF& pVelocity) const;
	void SetBodyVelocity(BodyID pBodyId, const Lepra::Vector3DF& pVelocity);
	void GetBodyAcceleration(BodyID pBodyId, Lepra::Vector3DF& pAcceleration) const;
	void SetBodyAcceleration(BodyID pBodyId, const Lepra::Vector3DF& pAcceleration);
	void GetBodyAngularVelocity(BodyID pBodyId, Lepra::Vector3DF& pAngularVelocity) const;
	void SetBodyAngularVelocity(BodyID pBodyId, const Lepra::Vector3DF& pAngularVelocity);
	void GetBodyAngularAcceleration(BodyID pBodyId, Lepra::Vector3DF& pAngularAcceleration) const;
	void SetBodyAngularAcceleration(BodyID pBodyId, const Lepra::Vector3DF& pAngularAcceleration);

	// Set and get user data.
	virtual void SetBodyData(BodyID pBodyId, void* pUserData);
	virtual void* GetBodyData(BodyID pBodyId);

	//
	// Create/delete triggers. A trigger is collision geometry which doesn't
	// affect the simulation. It's only purpose is to tell the listener
	// when an object intersects the trigger volume.
	//
	virtual TriggerID CreateSphereTrigger(const Lepra::TransformationF& pTransform,
										  Lepra::float32 pRadius,
										  TriggerListener* pListener);
	virtual TriggerID CreateCylinderTrigger(const Lepra::TransformationF& pTransform,
											Lepra::float32 pRadius,
											Lepra::float32 pLength,
											TriggerListener* pListener);
	virtual TriggerID CreateCapsuleTrigger(const Lepra::TransformationF& pTransform,
											Lepra::float32 pRadius,
											Lepra::float32 pLength,
											TriggerListener* pListener);
	virtual TriggerID CreateBoxTrigger(const Lepra::TransformationF& pTransform,
										const Lepra::Vector3D<Lepra::float32>& pSize,
										TriggerListener* pListener);
	virtual TriggerID CreateRayTrigger(const Lepra::TransformationF& pTransform,
										const Lepra::Vector3D<Lepra::float32>& pFromPos,
										const Lepra::Vector3D<Lepra::float32>& pToPos,
										TriggerListener* pListener);

	virtual void DeleteTrigger(TriggerID pTriggerID);

	virtual ForceFeedbackListener* GetForceFeedbackListener(BodyID pBody);

	virtual void GetTriggerTransform(TriggerID pTriggerID, Lepra::TransformationF& pTransform);
	virtual void SetTriggerTransform(TriggerID pTriggerID, const Lepra::TransformationF& pTransform);

	//
	// Create/delete joints.
	//
	virtual JointID CreateBallJoint(BodyID pBody1, 
									BodyID pBody2, 
									const Lepra::Vector3D<Lepra::float32>& pAnchorPos);
	virtual JointID CreateHingeJoint(BodyID pBody1, 
									 BodyID pBody2, 
									 const Lepra::Vector3D<Lepra::float32>& pAnchorPos, 
									 const Lepra::Vector3D<Lepra::float32>& pAxis);
	virtual JointID CreateHinge2Joint(BodyID pBody1, 
									  BodyID pBody2, 
									  const Lepra::Vector3D<Lepra::float32>& pAnchorPos, 
									  const Lepra::Vector3D<Lepra::float32>& pAxis1, 
									  const Lepra::Vector3D<Lepra::float32>& pAxis2);
	virtual JointID CreateUniversalJoint(BodyID pBody1, 
										 BodyID pBody2, 
										 const Lepra::Vector3D<Lepra::float32>& pAnchorPos, 
										 const Lepra::Vector3D<Lepra::float32>& pAxis1, 
										 const Lepra::Vector3D<Lepra::float32>& pAxis2);
	virtual JointID CreateSliderJoint(BodyID pBody1, 
									  BodyID pBody2, 
									  const Lepra::Vector3D<Lepra::float32>& pAxis);
	virtual JointID CreateFixedJoint(BodyID pBody1, 
									 BodyID pBody2);
	virtual JointID CreateAngularMotorJoint(BodyID pBody1, 
											BodyID pBody2, 
											const Lepra::Vector3D<Lepra::float32>& pAxis);

	virtual void DeleteJoint(JointID pJointId);

	virtual bool StabilizeJoint(JointID pJointId);

	bool GetJoint1Diff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const;
	bool SetJoint1Diff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff);
	bool GetJoint2Diff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const;
	bool SetJoint2Diff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff);
	bool GetJoint3Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const;
	bool SetJoint3Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff);

	// Returns true on success, false if joint is of wrong type.
	virtual bool GetAnchorPos(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAnchorPos) const;
	virtual bool GetAxis1(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAxis1) const;
	virtual bool GetAxis2(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAxis1) const;
	virtual bool GetAngle1(JointID pJointId, Lepra::float32& pAngle) const;
	virtual bool GetAngle2(JointID pJointId, Lepra::float32& pAngle) const;
	virtual bool GetAngleRate1(JointID pJointId, Lepra::float32& pAngleRate) const;
	virtual bool GetAngleRate2(JointID pJointId, Lepra::float32& pAngleRate) const;
	virtual bool SetAngle1(BodyID pBodyId, JointID pJointId, Lepra::float32 pAngle);

	virtual bool SetAngularMotorAngle(JointID pJointId, Lepra::float32 pAngle);
	virtual bool SetAngularMotorSpeed(JointID pJointId, Lepra::float32 pSpeed);
	virtual bool SetAngularMotorMaxForce(JointID pJointId, Lepra::float32 pMaxForce);
	virtual bool SetAngularMotorRoll(JointID pJointId, Lepra::float32 pMaxForce, Lepra::float32 pTargetVelocity);
	virtual bool GetAngularMotorRoll(JointID pJointId, Lepra::float32& pMaxForce, Lepra::float32& pTargetVelocity);
	virtual bool SetAngularMotorTurn(JointID pJointId, Lepra::float32 pMaxForce, Lepra::float32 pTargetVelocity);
	virtual bool GetAngularMotorAngle(JointID pJointId, Lepra::float32& pAngle) const;
	virtual bool GetAngularMotorSpeed(JointID pJointId, Lepra::float32& pSpeed) const;
	virtual bool GetAngularMotorMaxForce(JointID pJointId, Lepra::float32& pMaxForce) const;

	virtual bool SetJointParams(JointID pJointId, Lepra::float32 pLowStop, Lepra::float32 pHighStop, Lepra::float32 pBounce);
	virtual bool GetJointParams(JointID pJointId, Lepra::float32& pLowStop, Lepra::float32& pHighStop, Lepra::float32& pBounce) const;
	virtual bool SetSuspension(JointID pJointId, Lepra::float32 pFrameTime, Lepra::float32 pSpringConstant, Lepra::float32 pDampingConstant);

	virtual bool GetSliderPos(JointID pJointId, Lepra::float32& pPos) const;
	virtual bool GetSliderSpeed(JointID pJointId, Lepra::float32& pSpeed) const;

	// Adding a force to a joint is only possible on slider joints.
	virtual bool AddJointForce(JointID pJointId, Lepra::float32 pForce);
	virtual bool AddJointTorque(JointID pJointId, Lepra::float32 pTorque);
	virtual bool AddJointTorque(JointID pJointId, Lepra::float32 pTorque1, Lepra::float32 pTorque2);

	virtual void AddForce            (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce);
	virtual void AddTorque           (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pTorque);
	virtual void AddRelForce         (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce);
	virtual void AddRelTorque        (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pTorque);
	virtual void AddForceAtPos       (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos);
	virtual void AddForceAtRelPos    (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos);
	virtual void AddRelForceAtPos    (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos);
	virtual void AddRelForceAtRelPos (BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
													   const Lepra::Vector3D<Lepra::float32>& pPos);

	virtual void RestrictBody(BodyID pBodyId, Lepra::float32 pMaxSpeed, Lepra::float32 pMaxAngularSpeed);

	virtual void ActivateGravity(BodyID pBodyId);
	virtual void DeactivateGravity(BodyID pBodyId);
	virtual void SetGravity(const Lepra::Vector3D<Lepra::float32>& pGravity);

	virtual void StepAccurate(Lepra::float32 pStepSize);
	virtual void StepFast(Lepra::float32 pStepSize);

	virtual BodySet GetIdledBodies() const;

private:
	void ListEnabledObjects();
	void HandleAutoDisabledObjects();

	enum JointType
	{
		JOINT_INVALID = 0,
		JOINT_BALL,
		JOINT_HINGE,
		JOINT_HINGE2,
		JOINT_UNIVERSAL,
		JOINT_SLIDER,
		JOINT_FIXED,
		JOINT_ANGULARMOTOR,
		JOINT_CONTACT,
	};

	class Object
	{
	public:
		Object(dWorldID pWorldID) :
			mWorldID(pWorldID),
			mBodyID(0),
			mGeomID(0),
			mTriMeshID(0),
			mFriction(0),
			mBounce(0),
			mUserData(0),
			mTriggerListener(0),
			mForceFeedbackListener(0)
		{
		}

		dWorldID mWorldID;
		dBodyID mBodyID;
		dGeomID mGeomID;
		dTriMeshDataID mTriMeshID;
		Lepra::float32 mFriction;
		Lepra::float32 mBounce;
		void* mUserData;

		// The only thing that differs between standard bodies and triggers
		// is the value of this pointer. If this is null, this is a regular
		// body, a trigger otherwise.
		TriggerListener* mTriggerListener;

		ForceFeedbackListener* mForceFeedbackListener;
	};

	class JointInfo;
	typedef std::hash_set<Object*, std::hash<void*> > ObjectTable;
	typedef std::hash_set<JointInfo*, std::hash<void*> > JointTable;
	typedef std::list<JointInfo*> JointList;

	class JointInfo
	{
	public:
		inline JointInfo() :
			mListener1(0),
			mListener2(0)
		{
		}

		dJointID mJointID;
		JointType mType;
		dJointFeedback mFeedback;
		JointList::iterator mListIter;
		ForceFeedbackListener* mListener1;
		ForceFeedbackListener* mListener2;
	};

	bool GetHingeDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const;
	bool SetHingeDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff);
	bool GetUniversalDiff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const;
	bool SetUniversalDiff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff);
	bool GetHinge2Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const;
	bool SetHinge2Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff);
	bool GetBallDiff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const;
	bool SetBallDiff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff);

	void SetGeomTransform(dGeomID pGeomID, const Lepra::TransformationF& pTransform);
	bool CheckBodies(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const Lepra::tchar* pFunction);
	bool CheckBodies2(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const Lepra::tchar* pFunction);
	
	void DoForceFeedback();

	static void CollisionCallback(void* pData, dGeomID pObject1, dGeomID pObject2);

	dWorldID mWorldID;
	dSpaceID mSpaceID;
	dJointGroupID mContactJointGroupID;

	ObjectTable mObjectTable;
	BodySet mAutoDisabledObjectSet;
	JointTable mJointTable;
	JointList mFeedbackJointList;

	Lepra::FastAllocator<JointInfo> mJointInfoAllocator;

	LOG_CLASS_DECLARE();
};



}
