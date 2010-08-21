
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <hash_set>
#include "TBC.h"
#include "PhysicsManager.h"
#include "../../Lepra/Include/FastAllocator.h"
#include "../../Lepra/Include/Log.h"
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#include <ode/ode.h>
#pragma warning(pop)
#include <list>


namespace TBC
{



class PhysicsManagerODE: public PhysicsManager
{
public:
	PhysicsManagerODE();
	virtual ~PhysicsManagerODE();

	virtual bool InitCurrentThread();

	virtual int QueryRayCollisionAgainst(const Vector3DF& pRayPosition, const Vector3DF& pRayDirection,
		float pLength, BodyID pBody, Vector3DF* pCollisionPoints, int pMaxCollisionCount);

	virtual BodyID CreateSphere(bool pIsRoot, const TransformationF& pTransform, float32 pMass,
		float32 pRadius, BodyType pType, float32 pFriction = 1, float32 pBounce = 0,
		ForceFeedbackListener* pListener = 0);
	virtual BodyID CreateCylinder(bool pIsRoot, const TransformationF& pTransform, float32 pMass,
		float32 pRadius, float32 pLength, BodyType pType, float32 pFriction = 1,
		float32 pBounce = 0, ForceFeedbackListener* pListener = 0);
	virtual BodyID CreateCapsule(bool pIsRoot, const TransformationF& pTransform, float32 pMass,
		float32 pRadius, float32 pLength, BodyType pType, float32 pFriction = 1,
		float32 pBounce = 0, ForceFeedbackListener* pListener = 0);
	virtual BodyID CreateBox(bool pIsRoot, const TransformationF& pTransform, float32 pMass,
		const Vector3D<float32>& pSize, BodyType pType, float32 pFriction = 1,
		float32 pBounce = 0, ForceFeedbackListener* pListener = 0);
	virtual bool Attach(BodyID pStaticBody, BodyID pMainBody);

	// Tri meshes are always static.
	virtual BodyID CreateTriMesh(bool pIsRoot, unsigned pVertexCount, const float* pVertices,
		unsigned pTriangleCount, const Lepra::uint32* pIndices,
		const TransformationF& pTransform, float32 pFriction = 1,
		float32 pBounce = 0, ForceFeedbackListener* pListener = 0);

	virtual bool IsStaticBody(BodyID pBodyId) const;

	virtual void DeleteBody(BodyID pBodyId);

	Vector3DF GetBodyPosition(BodyID pBodyId) const;
	QuaternionF GetBodyOrientation(BodyID pBodyId) const;
	void GetBodyTransform(BodyID pBodyId, TransformationF& pTransform) const;
	void SetBodyTransform(BodyID pBodyId, const TransformationF& pTransform);
	void GetBodyVelocity(BodyID pBodyId, Vector3DF& pVelocity) const;
	void SetBodyVelocity(BodyID pBodyId, const Vector3DF& pVelocity);
	void GetBodyAcceleration(BodyID pBodyId, Vector3DF& pAcceleration) const;
	void SetBodyAcceleration(BodyID pBodyId, const Vector3DF& pAcceleration);
	void GetBodyAngularVelocity(BodyID pBodyId, Vector3DF& pAngularVelocity) const;
	void SetBodyAngularVelocity(BodyID pBodyId, const Vector3DF& pAngularVelocity);
	void GetBodyAngularAcceleration(BodyID pBodyId, Vector3DF& pAngularAcceleration) const;
	void SetBodyAngularAcceleration(BodyID pBodyId, const Vector3DF& pAngularAcceleration);

	virtual float GetBodyMass(BodyID pBodyId);
	virtual void MassAdjustBody(BodyID pBodyId);

	// Set and get user data.
	virtual void SetBodyData(BodyID pBodyId, void* pUserData);
	virtual void* GetBodyData(BodyID pBodyId);

	//
	// Create/delete triggers. A trigger is collision geometry which doesn't
	// affect the simulation. It's only purpose is to tell the listener
	// when an object intersects the trigger volume.
	//
	virtual TriggerID CreateSphereTrigger(const TransformationF& pTransform, float32 pRadius, TriggerListener* pListener);
	virtual TriggerID CreateCylinderTrigger(const TransformationF& pTransform, float32 pRadius,
		float32 pLength, TriggerListener* pListener);
	virtual TriggerID CreateCapsuleTrigger(const TransformationF& pTransform, float32 pRadius,
		float32 pLength, TriggerListener* pListener);
	virtual TriggerID CreateBoxTrigger(const TransformationF& pTransform,
		const Vector3D<float32>& pSize, TriggerListener* pListener);
	virtual TriggerID CreateRayTrigger(const TransformationF& pTransform, const Vector3D<float32>& pFromPos,
		const Vector3D<float32>& pToPos, TriggerListener* pListener);

	virtual void DeleteTrigger(TriggerID pTriggerID);

	virtual TriggerListener* GetTriggerListener(TriggerID pTrigger);
	virtual ForceFeedbackListener* GetForceFeedbackListener(BodyID pBody);

	virtual void GetTriggerTransform(TriggerID pTriggerID, TransformationF& pTransform);
	virtual void SetTriggerTransform(TriggerID pTriggerID, const TransformationF& pTransform);

	//
	// Create/delete joints.
	//
	virtual JointID CreateBallJoint(BodyID pBody1, 
									BodyID pBody2, 
									const Vector3D<float32>& pAnchorPos);
	virtual JointID CreateHingeJoint(BodyID pBody1, 
									 BodyID pBody2, 
									 const Vector3D<float32>& pAnchorPos, 
									 const Vector3D<float32>& pAxis);
	virtual JointID CreateHinge2Joint(BodyID pBody1, 
									  BodyID pBody2, 
									  const Vector3D<float32>& pAnchorPos, 
									  const Vector3D<float32>& pAxis1, 
									  const Vector3D<float32>& pAxis2);
	virtual JointID CreateUniversalJoint(BodyID pBody1, 
										 BodyID pBody2, 
										 const Vector3D<float32>& pAnchorPos, 
										 const Vector3D<float32>& pAxis1, 
										 const Vector3D<float32>& pAxis2);
	virtual JointID CreateSliderJoint(BodyID pBody1, 
									  BodyID pBody2, 
									  const Vector3D<float32>& pAxis);
	virtual JointID CreateFixedJoint(BodyID pBody1, 
									 BodyID pBody2);
	virtual JointID CreateAngularMotorJoint(BodyID pBody1, 
											BodyID pBody2, 
											const Vector3D<float32>& pAxis);

	virtual void DeleteJoint(JointID pJointId);

	virtual bool StabilizeJoint(JointID pJointId);
	virtual void SetIsGyroscope(BodyID pBodyId, bool pIsGyro);

	bool GetJoint1Diff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const;
	bool SetJoint1Diff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff);
	bool GetJoint2Diff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const;
	bool SetJoint2Diff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff);
	bool GetJoint3Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const;
	bool SetJoint3Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff);

	// Returns true on success, false if joint is of wrong type.
	virtual bool GetAnchorPos(JointID pJointId, Vector3D<float32>& pAnchorPos) const;
	virtual bool GetAxis1(JointID pJointId, Vector3D<float32>& pAxis1) const;
	virtual bool GetAxis2(JointID pJointId, Vector3D<float32>& pAxis1) const;
	virtual bool GetAngle1(JointID pJointId, float32& pAngle) const;
	virtual bool GetAngle2(JointID pJointId, float32& pAngle) const;
	virtual bool GetAngleRate1(JointID pJointId, float32& pAngleRate) const;
	virtual bool GetAngleRate2(JointID pJointId, float32& pAngleRate) const;
	virtual bool SetAngle1(BodyID pBodyId, JointID pJointId, float32 pAngle);

	virtual bool SetAngularMotorAngle(JointID pJointId, float32 pAngle);
	virtual bool SetAngularMotorSpeed(JointID pJointId, float32 pSpeed);
	virtual bool SetMotorMaxForce(JointID pJointId, float32 pMaxForce);
	virtual bool SetAngularMotorRoll(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity);
	virtual bool GetAngularMotorRoll(JointID pJointId, float32& pMaxForce, float32& pTargetVelocity);
	virtual bool SetAngularMotorTurn(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity);
	virtual bool GetAngularMotorAngle(JointID pJointId, float32& pAngle) const;
	virtual bool GetAngularMotorSpeed(JointID pJointId, float32& pSpeed) const;
	virtual bool GetAngularMotorMaxForce(JointID pJointId, float32& pMaxForce) const;
	virtual bool SetMotorTarget(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity);

	virtual bool SetJointParams(JointID pJointId, float32 pLowStop, float32 pHighStop, float32 pBounce);
	virtual bool GetJointParams(JointID pJointId, float32& pLowStop, float32& pHighStop, float32& pBounce) const;
	virtual bool SetSuspension(JointID pJointId, float32 pFrameTime, float32 pSpringConstant, float32 pDampingConstant);
	virtual bool GetSuspension(JointID pJointId, float32& pErp, float32& pCfm) const;

	virtual bool GetSliderPos(JointID pJointId, float32& pPos) const;
	virtual bool GetSliderSpeed(JointID pJointId, float32& pSpeed) const;

	// Adding a force to a joint is only possible on slider joints.
	virtual bool AddJointForce(JointID pJointId, float32 pForce);
	virtual bool AddJointTorque(JointID pJointId, float32 pTorque);
	virtual bool AddJointTorque(JointID pJointId, float32 pTorque1, float32 pTorque2);

	virtual void AddForce            (BodyID pBodyId, const Vector3D<float32>& pForce);
	virtual void AddTorque           (BodyID pBodyId, const Vector3D<float32>& pTorque);
	virtual void AddRelForce         (BodyID pBodyId, const Vector3D<float32>& pForce);
	virtual void AddRelTorque        (BodyID pBodyId, const Vector3D<float32>& pTorque);
	virtual void AddForceAtPos       (BodyID pBodyId, const Vector3D<float32>& pForce,
													   const Vector3D<float32>& pPos);
	virtual void AddForceAtRelPos    (BodyID pBodyId, const Vector3D<float32>& pForce,
													   const Vector3D<float32>& pPos);
	virtual void AddRelForceAtPos    (BodyID pBodyId, const Vector3D<float32>& pForce,
													   const Vector3D<float32>& pPos);
	virtual void AddRelForceAtRelPos (BodyID pBodyId, const Vector3D<float32>& pForce,
													   const Vector3D<float32>& pPos);

	virtual void RestrictBody(BodyID pBodyId, float32 pMaxSpeed, float32 pMaxAngularSpeed);

	virtual void ActivateGravity(BodyID pBodyId);
	virtual void DeactivateGravity(BodyID pBodyId);
	virtual void SetGravity(const Vector3D<float32>& pGravity);
	virtual Vector3DF GetGravity() const;

	virtual void PreSteps();
	virtual void StepAccurate(float32 pStepSize);
	virtual void StepFast(float32 pStepSize);
	virtual void PostSteps();

	virtual const BodySet& GetIdledBodies() const;

private:
	void FlagMovingObjects();
	void HandleMovableObjects();

	void NormalizeRotation(BodyID pObject);

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

	struct Object
	{
		Object(dWorldID pWorldID, bool pIsRoot) :
			mWorldID(pWorldID),
			mIsRoot(pIsRoot),
			mBodyID(0),
			mGeomID(0),
			mTriMeshID(0),
			mMass(0),
			mFriction(0),
			mBounce(0),
			mUserData(0),
			mTriggerListener(0),
			mForceFeedbackListener(0),
			mHasMassChildren(false)
		{
		}

		dWorldID mWorldID;
		bool mIsRoot;
		dBodyID mBodyID;
		dGeomID mGeomID;
		float mGeometryData[3];
		dTriMeshDataID mTriMeshID;
		float mMass;
		float32 mFriction;
		float32 mBounce;
		void* mUserData;

		// The only thing that differs between standard bodies and triggers
		// is the value of this pointer. If this is null, this is a regular
		// body, a trigger otherwise.
		TriggerListener* mTriggerListener;

		ForceFeedbackListener* mForceFeedbackListener;

		bool mHasMassChildren;
	};

	class JointInfo;
	typedef std::hash_set<Object*, LEPRA_VOIDP_HASHER> ObjectTable;
	typedef std::hash_set<JointInfo*, LEPRA_VOIDP_HASHER> JointTable;
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
		bool mBody1Static;
		bool mBody2Static;
		dJointFeedback mFeedback;
		JointList::iterator mListIter;
		ForceFeedbackListener* mListener1;
		ForceFeedbackListener* mListener2;
	};

	bool GetHingeDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const;
	bool SetHingeDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff);
	bool GetSliderDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const;
	bool SetSliderDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff);
	bool GetUniversalDiff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const;
	bool SetUniversalDiff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff);
	bool GetHinge2Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const;
	bool SetHinge2Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff);
	bool GetBallDiff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const;
	bool SetBallDiff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff);

	bool CheckBodies(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const tchar* pFunction);
	bool CheckBodies2(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const tchar* pFunction);
	
	void DoForceFeedback();

	static void CollisionCallback(void* pData, dGeomID pObject1, dGeomID pObject2);

	dWorldID mWorldID;
	dSpaceID mSpaceID;
	dJointGroupID mContactJointGroupID;

	ObjectTable mObjectTable;
	BodySet mAutoDisabledObjectSet;
	JointTable mJointTable;
	JointList mFeedbackJointList;

	FastAllocator<JointInfo> mJointInfoAllocator;

	LOG_CLASS_DECLARE();
};



}
