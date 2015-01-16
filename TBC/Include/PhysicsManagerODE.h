
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Unordered.h"
#include "Tbc.h"
#include "PhysicsManager.h"
#include "../../Lepra/Include/FastAllocator.h"
#include "../../Lepra/Include/Log.h"
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#include <ode/ode.h>
#pragma warning(pop)
#include <vector>


namespace Tbc
{



class PhysicsManagerODE: public PhysicsManager
{
public:
	PhysicsManagerODE(float pRadius, int pLevels, float pSensitivity);
	virtual ~PhysicsManagerODE();

	virtual void SetSimulationParameters(float pSoftness, float pRubberbanding, float pAccuracy);
	virtual bool InitCurrentThread();

	virtual int QueryRayCollisionAgainst(const vec3& pRayPosition, const vec3& pRayDirection, float pLength, BodyID pBody, vec3* pCollisionPoints, int pMaxCollisionCount);

	virtual BodyID CreateSphere(bool pIsRoot, const xform& pTransform, float32 pMass, float32 pRadius, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0);
	virtual BodyID CreateCylinder(bool pIsRoot, const xform& pTransform, float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0);
	virtual BodyID CreateCapsule(bool pIsRoot, const xform& pTransform, float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0);
	virtual BodyID CreateBox(bool pIsRoot, const xform& pTransform, float32 pMass, const Vector3D<float32>& pSize, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0);
	virtual bool Attach(BodyID pStaticBody, BodyID pMainBody);
	virtual bool DetachToDynamic(BodyID pStaticBody, float32 pMass);
	virtual bool MakeStatic(BodyID pDynamicBody);
	virtual bool AddMass(BodyID pStaticBody, BodyID pMainBody);

	// Tri meshes are always static.
	virtual BodyID CreateTriMesh(bool pIsRoot, unsigned pVertexCount, const float* pVertices, unsigned pTriangleCount, const Lepra::uint32* pIndices,
		const xform& pTransform, float32 pMass, BodyType pType, float32 pFriction = 1, float32 pBounce = 0, int pForceListenerId = 0);

	virtual bool IsStaticBody(BodyID pBodyId) const;

	virtual void DeleteBody(BodyID pBodyId);

	virtual vec3 GetBodyPosition(BodyID pBodyId) const;
	virtual void SetBodyPosition(BodyID pBodyId, const vec3& pPosition) const;
	virtual quat GetBodyOrientation(BodyID pBodyId) const;
	virtual void GetBodyTransform(BodyID pBodyId, xform& pTransform) const;
	virtual void SetBodyTransform(BodyID pBodyId, const xform& pTransform);
	virtual void GetBodyVelocity(BodyID pBodyId, vec3& pVelocity) const;
	virtual void SetBodyVelocity(BodyID pBodyId, const vec3& pVelocity);
	virtual void GetBodyForce(BodyID pBodyId, vec3& pAcceleration) const;
	virtual void SetBodyForce(BodyID pBodyId, const vec3& pAcceleration);
	virtual void GetBodyAcceleration(BodyID pBodyId, float pTotalMass, vec3& pAcceleration) const;
	virtual void SetBodyAcceleration(BodyID pBodyId, float pTotalMass, const vec3& pAcceleration);
	virtual void GetBodyAngularVelocity(BodyID pBodyId, vec3& pAngularVelocity) const;
	virtual void SetBodyAngularVelocity(BodyID pBodyId, const vec3& pAngularVelocity);
	virtual void GetBodyTorque(BodyID pBodyId, vec3& pAngularAcceleration) const;
	virtual void SetBodyTorque(BodyID pBodyId, const vec3& pAngularAcceleration);
	virtual void GetBodyAngularAcceleration(BodyID pBodyId, float pTotalMass, vec3& pAngularAcceleration) const;
	virtual void SetBodyAngularAcceleration(BodyID pBodyId, float pTotalMass, const vec3& pAngularAcceleration);

	virtual float GetBodyMass(BodyID pBodyId);
	virtual void SetBodyMass(BodyID pBodyId, float pMass);
	virtual void MassAdjustBody(BodyID pBodyId);

	// Set and get user data.
	virtual void SetBodyData(BodyID pBodyId, void* pUserData);
	virtual void* GetBodyData(BodyID pBodyId);

	//
	// Create/delete triggers. A trigger is collision geometry which doesn't
	// affect the simulation. It's only purpose is to tell the listener
	// when an object intersects the trigger volume.
	//
	virtual TriggerID CreateSphereTrigger(const xform& pTransform, float32 pRadius, int pTriggerListenerId);
	virtual TriggerID CreateCylinderTrigger(const xform& pTransform, float32 pRadius, float32 pLength, int pTriggerListenerId);
	virtual TriggerID CreateCapsuleTrigger(const xform& pTransform, float32 pRadius, float32 pLength, int pTriggerListenerId);
	virtual TriggerID CreateBoxTrigger(const xform& pTransform, const Vector3D<float32>& pSize, int pTriggerListenerId);
	virtual TriggerID CreateRayTrigger(const xform& pTransform, const Vector3D<float32>& pFromPos, const Vector3D<float32>& pToPos, int pTriggerListenerId);

	virtual void DeleteTrigger(TriggerID pTriggerID);

	virtual int GetTriggerListenerId(TriggerID pTrigger);
	virtual int GetForceFeedbackListenerId(BodyID pBody);
	virtual void SetForceFeedbackListener(BodyID pBody, int pForceFeedbackId);

	virtual void GetTriggerTransform(TriggerID pTriggerID, xform& pTransform);
	virtual void SetTriggerTransform(TriggerID pTriggerID, const xform& pTransform);

	//
	// Create/delete joints.
	//
	virtual JointID CreateBallJoint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAnchorPos);
	virtual JointID CreateHingeJoint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAnchorPos, const Vector3D<float32>& pAxis);
	virtual JointID CreateHinge2Joint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAnchorPos, const Vector3D<float32>& pAxis1, const Vector3D<float32>& pAxis2);
	virtual JointID CreateUniversalJoint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAnchorPos, const Vector3D<float32>& pAxis1, const Vector3D<float32>& pAxis2);
	virtual JointID CreateSliderJoint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAxis);
	virtual JointID CreateFixedJoint(BodyID pBody1, BodyID pBody2);
	virtual JointID CreateAngularMotorJoint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAxis);

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

	virtual bool SetJointParams(JointID pJointId, float32 pLowStop, float32 pHighStop, float32 pBounce, int pExtraIndex);
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
	virtual void AddForceAtPos       (BodyID pBodyId, const Vector3D<float32>& pForce, const Vector3D<float32>& pPos);
	virtual void AddForceAtRelPos    (BodyID pBodyId, const Vector3D<float32>& pForce, const Vector3D<float32>& pPos);
	virtual void AddRelForceAtPos    (BodyID pBodyId, const Vector3D<float32>& pForce, const Vector3D<float32>& pPos);
	virtual void AddRelForceAtRelPos (BodyID pBodyId, const Vector3D<float32>& pForce, const Vector3D<float32>& pPos);

	virtual void RestrictBody(BodyID pBodyId, float32 pMaxSpeed, float32 pMaxAngularSpeed);

	virtual void EnableGravity(BodyID pBodyId, bool pEnable);
	virtual void SetGravity(const Vector3D<float32>& pGravity);
	virtual vec3 GetGravity() const;

	virtual void EnableTriggerBySelf(TriggerID pTriggerId, bool pEnable);
	virtual void EnableCollideWithSelf(BodyID pBodyId, bool pEnable);

	virtual void PreSteps();
	virtual void StepAccurate(float32 pStepSize, bool pCollide);
	virtual void StepFast(float32 pStepSize, bool pCollide);
	virtual bool IsColliding(int pForceFeedbackId);
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
			mCollideWithSelf(false),
			mBodyID(0),
			mGeomID(0),
			mTriMeshID(0),
			mMass(0),
			mFriction(0),
			mBounce(0),
			mUserData(0),
			mTriggerListenerId(0),
			mForceFeedbackId(0),
			mHasMassChildren(false),
			mIsRotational(false)
		{
		}

		dWorldID mWorldID;
		bool mIsRoot;
		bool mCollideWithSelf;
		dBodyID mBodyID;
		dGeomID mGeomID;
		float mGeometryData[3];
		dTriMeshDataID mTriMeshID;
		float mMass;
		float32 mFriction;
		float32 mBounce;
		void* mUserData;

		// The only thing that differs between standard bodies and triggers
		// is the value of this member. If this is 0, this is a regular
		// body, a trigger otherwise.
		int mTriggerListenerId;

		int mForceFeedbackId;

		bool mHasMassChildren;
		bool mIsRotational;
	};

	class TriggerInfo;
	class JointInfo;
	typedef std::unordered_set<Object*, LEPRA_VOIDP_HASHER> ObjectTable;
	typedef std::unordered_set<JointInfo*, LEPRA_VOIDP_HASHER> JointTable;
	typedef std::vector<TriggerInfo> TriggerInfoList;
	typedef std::vector<JointInfo*> JointList;

	class TriggerInfo
	{
	public:
		inline TriggerInfo(TriggerID pTriggerId, int pTriggerListenerId, int pBodyListenerId, BodyID pBodyId, const vec3& pNormal):
			mTriggerId(pTriggerId),
			mTriggerListenerId(pTriggerListenerId),
			mBodyListenerId(pBodyListenerId),
			mBodyId(pBodyId),
			mNormal(pNormal)
		{
		}

		TriggerID mTriggerId;
		int mTriggerListenerId;
		int mBodyListenerId;
		BodyID mBodyId;
		vec3 mNormal;
	};

	class JointInfo
	{
	public:
		inline JointInfo() :
			mListenerId1(0),
			mListenerId2(0),
			mBody1Id(0),
			mBody2Id(0)
		{
		}

		inline bool IsBody1Static(PhysicsManagerODE* pManager) const
		{
			return !mBody1Id || pManager->IsStaticBody(mBody1Id);
		}
		inline bool IsBody2Static(PhysicsManagerODE* pManager) const
		{
			return !mBody2Id || pManager->IsStaticBody(mBody2Id);
		}

		dJointID mJointID;
		JointType mType;
		BodyID mBody1Id;
		BodyID mBody2Id;
		dJointFeedback mFeedback;
		int mListenerId1;
		int mListenerId2;
		vec3 mPosition;
		vec3 mRelativeVelocity;
	};

	void RemoveJoint(JointInfo* pJointInfo);

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
	static void CollisionNoteCallback(void* pData, dGeomID pObject1, dGeomID pObject2);

	dWorldID mWorldID;
	static float mWorldErp;
	static float mWorldCfm;
	dSpaceID mSpaceID;
	dJointGroupID mContactJointGroupID;
	int mNoteForceFeedbackId;
	bool mNoteIsCollided;

	ObjectTable mObjectTable;
	BodySet mAutoDisabledObjectSet;
	JointTable mJointTable;

	TriggerInfoList mTriggerInfoList;
	JointList mFeedbackJointList;

	FastAllocator<JointInfo> mJointInfoAllocator;

	logclass();
};



}
