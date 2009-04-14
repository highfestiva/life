
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



#include <assert.h>
#include "../../Thirdparty/ode-060223/ode/src/joint.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../Include/PhysicsEngineODE.h"
#include "../Include/GeometryBase.h"



namespace TBC
{



PhysicsEngineODE::PhysicsEngineODE()
{
	mWorldID = dWorldCreate();
	
	// Play with these to make the simulation behave better.
	::dWorldSetCFM(mWorldID, 0);	// World softness and numerical stability.
	//::dWorldSetERP(mWorldID, 0.2f);

	::dWorldSetAutoDisableFlag(mWorldID, 1);
	
	::dWorldSetAutoDisableLinearThreshold(mWorldID, 0.02f);
	::dWorldSetAutoDisableAngularThreshold(mWorldID, 0.02f);
	//::dWorldSetAutoDisableSteps(mWorldID, 10);
	//::dWorldSetAutoDisableTime(mWorldID, 0);

	// Collision space center and extents (10 km).
	dReal lCenter[4] = {0, 0, 0, 0};
	dReal lExtents[4] = {10000, 10000, 10000, 0};
	mSpaceID = ::dQuadTreeSpaceCreate(0, lCenter, lExtents, 10);

	::dWorldSetGravity(mWorldID, 0, 0, -9.82f);

	// Create joint group for contact joints used in collision detection.
	mContactJointGroupID = ::dJointGroupCreate(0);
}

PhysicsEngineODE::~PhysicsEngineODE()
{
	ObjectTable::iterator x = mObjectTable.begin();
	for (; x != mObjectTable.end(); ++x)
	{
		Object* lObject = *x;
		if (lObject->mTriMeshID != 0)
		{
			::dGeomTriMeshDataDestroy(lObject->mTriMeshID);
		}
		if (lObject->mBodyID != 0)
		{
			::dBodyDestroy(lObject->mBodyID);
		}
		::dGeomDestroy(lObject->mGeomID);
		delete lObject;
	}

	JointTable::iterator lJIter;

	::dWorldDestroy(mWorldID);
	::dSpaceDestroy(mSpaceID);
	::dJointGroupDestroy(mContactJointGroupID);
	//::dCloseODE();
}

PhysicsEngine::BodyID PhysicsEngineODE::CreateSphere(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pMass, Lepra::float32 pRadius, BodyType pType, Lepra::float32 pFriction, Lepra::float32 pBounce,
	TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);
	lObject->mGeomID = dCreateSphere(mSpaceID, (dReal)pRadius);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;

	if (pType == PhysicsEngine::DYNAMIC)
	{
		dMass lMass;
		::dMassSetZero(&lMass);
		::dMassSetSphereTotal(&lMass, (dReal)pMass, (dReal)pRadius);
	
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);

		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
	::dGeomSetData(lObject->mGeomID, lObject);

	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (BodyID)(Lepra::uint64)lObject;
}

PhysicsEngine::BodyID PhysicsEngineODE::CreateCylinder(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pMass, Lepra::float32 pRadius, Lepra::float32 pLength, BodyType pType, Lepra::float32 pFriction,
	Lepra::float32 pBounce, TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	// TODO: Create a real cylinder when ODE supports it.
	lObject->mGeomID = ::dCreateCCylinder(mSpaceID, (dReal)pRadius, (dReal)pLength);
	mLog.AWarning("Warning! Cylinders are not accurately supported by ODE!");

	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;

	if (pType == PhysicsEngine::DYNAMIC)
	{
		dMass lMass;
		::dMassSetZero(&lMass);
		::dMassSetCylinderTotal(&lMass, (dReal)pMass, 3, (dReal)pRadius, (dReal)pLength);
	
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);

		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
	::dGeomSetData(lObject->mGeomID, lObject);

	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (BodyID)(Lepra::uint64)lObject;
}

PhysicsEngine::BodyID PhysicsEngineODE::CreateCapsule(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pMass, Lepra::float32 pRadius, Lepra::float32 pLength, BodyType pType, Lepra::float32 pFriction,
	Lepra::float32 pBounce, TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	lObject->mGeomID = dCreateCCylinder(mSpaceID, (dReal)pRadius, (dReal)pLength);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;

	if (pType == PhysicsEngine::DYNAMIC)
	{
		dMass lMass;
		dMassSetZero(&lMass);
		dMassSetCappedCylinderTotal(&lMass, (dReal)pMass, 3, (dReal)pRadius, (dReal)pLength);
	
		lObject->mBodyID = dBodyCreate(mWorldID);
		dBodySetMass(lObject->mBodyID, &lMass);

		dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (BodyID)(Lepra::uint64)lObject;
}

PhysicsEngine::BodyID PhysicsEngineODE::CreateBox(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pMass, const Lepra::Vector3D<Lepra::float32>& pSize, BodyType pType, Lepra::float32 pFriction,
	Lepra::float32 pBounce, TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	lObject->mGeomID = ::dCreateBox(mSpaceID, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;

	if (pType == PhysicsEngine::DYNAMIC)
	{
		dMass lMass;
		::dMassSetZero(&lMass);
		::dMassSetBoxTotal(&lMass, (dReal)pMass, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);
		lObject->mBodyID = ::dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
	}

	::dGeomSetData(lObject->mGeomID, lObject);

	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (BodyID)(Lepra::uint64)lObject;
}

bool PhysicsEngineODE::Attach(BodyID pStaticBody, BodyID pMainBody)
{
	ObjectTable::iterator x = mObjectTable.find((Object*)pStaticBody);
	if (x == mObjectTable.end())
	{
		assert(false);
		return (false);
	}
	ObjectTable::iterator y = mObjectTable.find((Object*)pMainBody);
	if (y == mObjectTable.end() || x == y)
	{
		assert(false);
		return (false);
	}
	Object* lStaticObject = *x;
	Object* lMainObject = *y;
	if (lStaticObject->mBodyID || !lMainObject->mBodyID)
	{
		mLog.AError("Attach() with non-static/static.");
		assert(false);
		return (false);
	}
	const dReal* lPos = dGeomGetPosition(lStaticObject->mGeomID);
	dQuaternion o;
	::dGeomGetQuaternion(lStaticObject->mGeomID, o);
	::dGeomSetBody(lStaticObject->mGeomID, lMainObject->mBodyID);
	::dGeomSetOffsetWorldPosition(lStaticObject->mGeomID, lPos[0], lPos[1], lPos[2]);
	::dGeomSetOffsetWorldQuaternion(lStaticObject->mGeomID, o);
	return (true);
}

PhysicsEngine::BodyID PhysicsEngineODE::CreateTriMesh(const GeometryBase* pMesh,
	const Lepra::Transformation<Lepra::float32>& pTransform, Lepra::float32 pFriction, Lepra::float32 pBounce,
	TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);
	lObject->mTriMeshID = dGeomTriMeshDataCreate();

	dGeomTriMeshDataBuildSingle(lObject->mTriMeshID,
				    (const dReal*)pMesh->GetVertexData(),
				    sizeof(Lepra::float32) * 3,
				    (int)pMesh->GetVertexCount(),
				    (const int*)pMesh->GetIndexData(),
				    (int)pMesh->GetTriangleCount() * 3,
				    sizeof(int) * 3);

	lObject->mGeomID = dCreateTriMesh(mSpaceID, lObject->mTriMeshID, 0, 0, 0);
	dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
	dGeomSetData(lObject->mGeomID, lObject);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;

//	dGeomTriMeshEnableTC(lObject->mGeomID, dBoxClass, 1);

	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (BodyID)(Lepra::uint64)lObject;
}

void PhysicsEngineODE::DeleteBody(BodyID pBodyId)
{
	ObjectTable::iterator lIter = mObjectTable.find((Object*)pBodyId);
	if (lIter != mObjectTable.end())
	{
		Object* lObject = *lIter;
		if (lObject->mBodyID != 0)
		{
			dBodyDestroy(lObject->mBodyID);
		}
		dGeomDestroy(lObject->mGeomID);
		delete lObject;
		mObjectTable.erase(lIter);
		mAutoDisabledObjectSet.erase(lObject);
	}
	else
	{
		mLog.AError("DeleteBody() - Can't find body to delete!");
		assert(false);
	}
}

void PhysicsEngineODE::GetBodyTransform(BodyID pBodyId, Lepra::Transformation<Lepra::float32>& pTransform) const
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyTransform() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	const dReal* lPos = dGeomGetPosition(lObject->mGeomID);
	dQuaternion lQ;
	dGeomGetQuaternion(lObject->mGeomID, lQ);

	pTransform.SetPosition(Lepra::Vector3D<Lepra::float32>(lPos[0], lPos[1], lPos[2]));
	pTransform.SetOrientation(Lepra::Quaternion<Lepra::float32>(lQ[0], lQ[1], lQ[2], lQ[3]));
}

void PhysicsEngineODE::SetBodyTransform(BodyID pBodyId, const Lepra::Transformation<Lepra::float32>& pTransform)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyTransform() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	if(lObject->mBodyID)
	{
		const Lepra::Vector3D<Lepra::float32>& lPos = pTransform.GetPosition();
		::dBodySetPosition(lObject->mBodyID, lPos.x, lPos.y, lPos.z);

		const Lepra::Quaternion<Lepra::float32>& lQuat = pTransform.GetOrientation();
		dReal lQ[4];
		lQ[0] = lQuat.GetA();
		lQ[1] = lQuat.GetB();
		lQ[2] = lQuat.GetC();
		lQ[3] = lQuat.GetD();
		::dBodySetQuaternion(lObject->mBodyID, lQ);
		::dBodyEnable(lObject->mBodyID);
	}
	else
	{
		SetGeomTransform(lObject->mGeomID, pTransform);
	}
}

void PhysicsEngineODE::GetBodyVelocity(BodyID pBodyId, Lepra::Vector3DF& pVelocity) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mBodyID)
	{
		const dReal* lVelocity = dBodyGetLinearVel(lObject->mBodyID);
		pVelocity.x = lVelocity[0];
		pVelocity.y = lVelocity[1];
		pVelocity.z = lVelocity[2];
	}
	else
	{
		pVelocity.Set(0, 0, 0);
	}
}

void PhysicsEngineODE::SetBodyVelocity(BodyID pBodyId, const Lepra::Vector3DF& pVelocity)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetLinearVel(lObject->mBodyID, pVelocity.x, pVelocity.y, pVelocity.z);
	}
}

void PhysicsEngineODE::GetBodyAcceleration(BodyID pBodyId, Lepra::Vector3DF& pAcceleration) const
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		const dReal* lAcceleration = dBodyGetForce(lObject->mBodyID);
		pAcceleration.x = lAcceleration[0];
		pAcceleration.y = lAcceleration[1];
		pAcceleration.z = lAcceleration[2];
	}
	else
	{
		pAcceleration.Set(0, 0, 0);
	}
}

void PhysicsEngineODE::SetBodyAcceleration(BodyID pBodyId, const Lepra::Vector3DF& pAcceleration)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
		dBodySetForce(lObject->mBodyID, pAcceleration.x, pAcceleration.y, pAcceleration.z);
}

void PhysicsEngineODE::GetBodyAngularVelocity(BodyID pBodyId, Lepra::Vector3DF& pAngularVelocity) const
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		const dReal* lAngularVelocity = dBodyGetAngularVel(lObject->mBodyID);
		pAngularVelocity.x = lAngularVelocity[0];
		pAngularVelocity.y = lAngularVelocity[1];
		pAngularVelocity.z = lAngularVelocity[2];
	}
	else
	{
		pAngularVelocity.Set(0, 0, 0);
	}
}

void PhysicsEngineODE::SetBodyAngularVelocity(BodyID pBodyId, const Lepra::Vector3DF& pAngularVelocity)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetAngularVel(lObject->mBodyID, pAngularVelocity.x, pAngularVelocity.y, pAngularVelocity.z);
	}
}

void PhysicsEngineODE::GetBodyAngularAcceleration(BodyID pBodyId, Lepra::Vector3DF& pAngularAcceleration) const
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		const dReal* lAngularAcceleration = dBodyGetTorque(lObject->mBodyID);
		pAngularAcceleration.x = lAngularAcceleration[0];
		pAngularAcceleration.y = lAngularAcceleration[1];
		pAngularAcceleration.z = lAngularAcceleration[2];
	}
	else
	{
		pAngularAcceleration.Set(0, 0, 0);
	}
}

void PhysicsEngineODE::SetBodyAngularAcceleration(BodyID pBodyId, const Lepra::Vector3DF& pAngularAcceleration)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetTorque(lObject->mBodyID, pAngularAcceleration.x, pAngularAcceleration.y, pAngularAcceleration.z);
	}
}

void PhysicsEngineODE::SetBodyData(BodyID pBodyId, void* pUserData)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyData() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	lObject->mUserData = pUserData;
}

void* PhysicsEngineODE::GetBodyData(BodyID pBodyId)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyData() - Body %i is not part of this world!"), pBodyId);
		return 0;
	}

	return lObject->mUserData;
}

PhysicsEngine::TriggerID PhysicsEngineODE::CreateSphereTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pRadius, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);
	lObject->mGeomID = dCreateSphere(mSpaceID, (dReal)pRadius);

	dGeomSetBody(lObject->mGeomID, 0);
	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (TriggerID)lObject->mGeomID;
}

PhysicsEngine::TriggerID PhysicsEngineODE::CreateCylinderTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pRadius, Lepra::float32 pLength, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	// TODO: Create a real cylinder when ODE supports it.
	lObject->mGeomID = dCreateCCylinder(mSpaceID, (dReal)pRadius, (dReal)(pLength - pRadius * 2));
	mLog.AWarning("Warning! Cylinders are not accurately supported by ODE!");

	dGeomSetBody(lObject->mGeomID, 0);
	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (TriggerID)lObject->mGeomID;
}

PhysicsEngine::TriggerID PhysicsEngineODE::CreateCapsuleTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
	Lepra::float32 pRadius, Lepra::float32 pLength, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	lObject->mGeomID = dCreateCCylinder(mSpaceID, (dReal)pRadius, (dReal)pLength);

	dGeomSetBody(lObject->mGeomID, 0);
	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (TriggerID)lObject->mGeomID;
}

PhysicsEngine::TriggerID PhysicsEngineODE::CreateBoxTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
	const Lepra::Vector3D<Lepra::float32>& pSize, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	lObject->mGeomID = dCreateBox(mSpaceID, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);

	dGeomSetBody(lObject->mGeomID, 0);
	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (TriggerID)lObject->mGeomID;
}

PhysicsEngine::TriggerID PhysicsEngineODE::CreateRayTrigger(const Lepra::Transformation<Lepra::float32>& pTransform,
	const Lepra::Vector3D<Lepra::float32>& pFromPos, const Lepra::Vector3D<Lepra::float32>& pToPos,
	TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID);

	// Calculate the direction vector.
	Lepra::Vector3D<Lepra::float32> lDir(pToPos - pFromPos);
	Lepra::float32 lLength = lDir.GetLength();
	lDir.Div(lLength);

	lObject->mGeomID = dCreateRay(mSpaceID, lLength);

	dGeomSetBody(lObject->mGeomID, 0);
	dGeomSetData(lObject->mGeomID, lObject);

	dGeomRaySet(lObject->mGeomID, pFromPos.x, pFromPos.y, pFromPos.z, lDir.x, lDir.y, lDir.z);

	lObject->mTriggerListener = pForceListener;

	SetGeomTransform(lObject->mGeomID, pTransform);

	mObjectTable.insert(lObject);
	return (TriggerID)lObject->mGeomID;
}

void PhysicsEngineODE::DeleteTrigger(TriggerID pTriggerID)
{
	ObjectTable::iterator lIter = mObjectTable.find((Object*)pTriggerID);

	if (lIter != mObjectTable.end())
	{
		dGeomDestroy((*lIter)->mGeomID);
		delete *lIter;
		mObjectTable.erase(lIter);
	}
	else
	{
		mLog.AError("DeleteBody() - Can't find body to delete!");
	}
}

void PhysicsEngineODE::GetTriggerTransform(TriggerID pTriggerID, Lepra::Transformation<Lepra::float32>& pTransform)
{
	Object* lObject = (Object*)pTriggerID;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetTriggerTransform() - Body %i is not part of this world!"), pTriggerID);
		return;
	}

	const dReal* lPos = dGeomGetPosition(lObject->mGeomID);
	dQuaternion lQ;
	dGeomGetQuaternion(lObject->mGeomID, lQ);

	pTransform.SetPosition(Lepra::Vector3D<Lepra::float32>(lPos[0], lPos[1], lPos[2]));
	pTransform.SetOrientation(Lepra::Quaternion<Lepra::float32>(lQ[0], lQ[1], lQ[2], lQ[3]));
}

void PhysicsEngineODE::SetTriggerTransform(TriggerID pTriggerID, const Lepra::Transformation<Lepra::float32>& pTransform)
{
	Object* lObject = (Object*)pTriggerID;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyTransform() - Body %i is not part of this world!"), pTriggerID);
		return;
	}

	SetGeomTransform(lObject->mGeomID, pTransform);
}

PhysicsEngine::JointID PhysicsEngineODE::CreateBallJoint(BodyID pBody1, BodyID pBody2, const Lepra::Vector3D<Lepra::float32>& pAnchorPos)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateBallJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateBall(mWorldID, 0);
	lJointInfo->mType = JOINT_BALL;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	if ((lObject1 != 0 && lObject1->mForceFeedbackListener != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackListener != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	dJointSetBallAnchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsEngine::JointID PhysicsEngineODE::CreateHingeJoint(BodyID pBody1, BodyID pBody2, 
	const Lepra::Vector3D<Lepra::float32>& pAnchorPos, const Lepra::Vector3D<Lepra::float32>& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateHingeJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateHinge(mWorldID, 0);
	lJointInfo->mType = JOINT_HINGE;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	if ((lObject1 != 0 && lObject1->mForceFeedbackListener != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackListener != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	dJointSetHingeAnchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);
	dJointSetHingeAxis(lJointInfo->mJointID, pAxis.x, pAxis.y, pAxis.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsEngine::JointID PhysicsEngineODE::CreateHinge2Joint(BodyID pBody1, BodyID pBody2,
	const Lepra::Vector3D<Lepra::float32>& pAnchorPos, const Lepra::Vector3D<Lepra::float32>& pAxis1,
	const Lepra::Vector3D<Lepra::float32>& pAxis2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies2(pBody1, pBody2, lObject1, lObject2, _T("CreateHinge2Joint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateHinge2(mWorldID, 0);
	lJointInfo->mType = JOINT_HINGE2;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);

	if (lObject1->mForceFeedbackListener != 0 || 
	   lObject2->mForceFeedbackListener != 0)
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	dJointSetHinge2Anchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);
	dJointSetHinge2Axis1(lJointInfo->mJointID, pAxis1.x, pAxis1.y, pAxis1.z);
	dJointSetHinge2Axis2(lJointInfo->mJointID, pAxis2.x, pAxis2.y, pAxis2.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsEngine::JointID PhysicsEngineODE::CreateUniversalJoint(BodyID pBody1, BodyID pBody2,
	const Lepra::Vector3D<Lepra::float32>& pAnchorPos, const Lepra::Vector3D<Lepra::float32>& pAxis1,
	const Lepra::Vector3D<Lepra::float32>& pAxis2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateUniversalJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateUniversal(mWorldID, 0);
	lJointInfo->mType = JOINT_UNIVERSAL;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	if ((lObject1 != 0 && lObject1->mForceFeedbackListener != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackListener != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	dJointSetUniversalAnchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);
	dJointSetUniversalAxis1(lJointInfo->mJointID, pAxis1.x, pAxis1.y, pAxis1.z);
	dJointSetUniversalAxis2(lJointInfo->mJointID, pAxis2.x, pAxis2.y, pAxis2.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsEngine::JointID PhysicsEngineODE::CreateSliderJoint(BodyID pBody1, BodyID pBody2,
	const Lepra::Vector3D<Lepra::float32>& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateSliderJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateSlider(mWorldID, 0);
	lJointInfo->mType = JOINT_SLIDER;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	if ((lObject1 != 0 && lObject1->mForceFeedbackListener != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackListener != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	dJointSetSliderAxis(lJointInfo->mJointID, pAxis.x, pAxis.y, pAxis.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsEngine::JointID PhysicsEngineODE::CreateFixedJoint(BodyID pBody1, BodyID pBody2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateFixedJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateFixed(mWorldID, 0);
	lJointInfo->mType = JOINT_FIXED;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	if ((lObject1 != 0 && lObject1->mForceFeedbackListener != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackListener != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsEngine::JointID PhysicsEngineODE::CreateAngularMotorJoint(BodyID pBody1, BodyID pBody2,
	const Lepra::Vector3D<Lepra::float32>& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateAngularMotorJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateAMotor(mWorldID, 0);
	lJointInfo->mType = JOINT_ANGULARMOTOR;
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	if ((lObject1 != 0 && lObject1->mForceFeedbackListener != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackListener != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
		mFeedbackJointList.push_back(lJointInfo);
		lJointInfo->mListIter = --mFeedbackJointList.end();
	}

	dJointSetAMotorMode(lJointInfo->mJointID, dAMotorUser);
	dJointSetAMotorNumAxes(lJointInfo->mJointID, 1);

	// Set axis 0, relative to body 1.
	dJointSetAMotorAxis(lJointInfo->mJointID, 0, 1, pAxis.x, pAxis.y, pAxis.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

void PhysicsEngineODE::DeleteJoint(JointID pJointId)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	dJointDestroy(lJointInfo->mJointID);
	if (lJointInfo->mListIter != mFeedbackJointList.end())
	{
		mFeedbackJointList.erase(lJointInfo->mListIter);
	}
	lJointInfo->mListener1 = 0;
	lJointInfo->mListener2 = 0;
	mJointInfoAllocator.Free(lJointInfo);
}

bool PhysicsEngineODE::GetJoint1Diff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*lIter;
	switch (lJointInfo->mType)
	{
		case JOINT_HINGE:
		{
			lOk = GetHingeDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-1-type!"), lJointInfo->mType);
			assert(false);
		}
		break;
	}
	return (lOk);
}

bool PhysicsEngineODE::SetJoint1Diff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff)
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*lIter;
	switch (lJointInfo->mType)
	{
		case JOINT_HINGE:
		{
			lOk = SetHingeDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-1-type!"), lJointInfo->mType);
			assert(false);
		}
		break;
	}
	return (lOk);
}

bool PhysicsEngineODE::GetJoint2Diff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*lIter;
	switch (lJointInfo->mType)
	{
		case JOINT_UNIVERSAL:
		{
			lOk = GetUniversalDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-2-type!"), lJointInfo->mType);
			assert(false);
		}
		break;
	}
	return (lOk);
}

bool PhysicsEngineODE::SetJoint2Diff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff)
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*lIter;
	switch (lJointInfo->mType)
	{
		case JOINT_UNIVERSAL:
		{
			lOk = SetUniversalDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-2-type!"), lJointInfo->mType);
			assert(false);
		}
		break;
	}
	return (lOk);
}

bool PhysicsEngineODE::GetJoint3Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*lIter;
	switch (lJointInfo->mType)
	{
		case JOINT_HINGE2:
		{
			lOk = GetHinge2Diff(pBodyId, pJointId, pDiff);
		}
		break;
		case JOINT_BALL:
		{
			lOk = GetBallDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-3-type!"), lJointInfo->mType);
			assert(false);
		}
		break;
	}
	return (lOk);
}

bool PhysicsEngineODE::SetJoint3Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*lIter;
	switch (lJointInfo->mType)
	{
		case JOINT_HINGE2:
		{
			lOk = SetHinge2Diff(pBodyId, pJointId, pDiff);
		}
		break;
		case JOINT_BALL:
		{
			lOk = SetBallDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-3-type!"), lJointInfo->mType);
			assert(false);
		}
		break;
	}
	return (lOk);
}

bool PhysicsEngineODE::GetHingeDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_HINGE);
	if (lJointInfo->mType != JOINT_HINGE)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	Lepra::Vector3DF lAxis;
	if (!GetAxis1(pJointId, lAxis) || !GetAngle1(pJointId, pDiff.mAngle))
	{
		return (false);
	}

	{
		Lepra::Vector3DF lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mAngleVelocity = lAxis * lVelocity;
	}

	{
		Lepra::Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mAngleAcceleration = lAxis * lAcceleration;
	}

	return (true);
}

bool PhysicsEngineODE::SetHingeDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_HINGE);
	if (lJointInfo->mType != JOINT_HINGE)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	Lepra::Vector3DF lAnchor;
	Lepra::Vector3DF lAxis;
	float lAngle1;
	if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis) || !GetAngle1(pJointId, lAngle1))
	{
		return (false);
	}
	assert(lAxis.GetLengthSquared() > 0.99f && lAxis.GetLengthSquared() < 1.01f);

	{
		// Align anchors before rotating.
		Lepra::TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		dVector3 lRawAnchor;
		::dJointGetHingeAnchor2(lJointInfo->mJointID, lRawAnchor);
		Lepra::Vector3DF lAnchor2(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
		lTransform.GetPosition() += lAnchor-lAnchor2;
		lTransform.RotateAroundAnchor(lAnchor, lAxis, -pDiff.mAngle+lAngle1);
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		Lepra::Vector3DF lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (pDiff.mAngleVelocity < Lepra::PIF*1000)
		{
			Lepra::Vector3DF lAxisVelocity = lAxis*(lAxis*lVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis * pDiff.mAngleVelocity;
			SetBodyAngularVelocity(pBodyId, lVelocity);
		}
	}

	{
		Lepra::Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		// Drop angular acceleration along axis, then add the specified amount.
		if (pDiff.mAngleAcceleration < Lepra::PIF*1000)
		{
			Lepra::Vector3DF lAxisAcceleration = lAxis*(lAxis*lAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis * pDiff.mAngleAcceleration;
			SetBodyAngularAcceleration(pBodyId, lAcceleration);
		}
	}

	return (true);
}

bool PhysicsEngineODE::GetUniversalDiff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_UNIVERSAL);

	Lepra::Vector3DF lAxis1;
	Lepra::Vector3DF lAxis2;
	{
		if (!GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2) ||
			!GetAngle1(pJointId, pDiff.mValue) || !GetAngle2(pJointId, pDiff.mAngle))
		{
			return (false);
		}
		assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
	}

	{
		if (!GetAngleRate1(pJointId, pDiff.mValueVelocity) || !GetAngleRate2(pJointId, pDiff.mAngleVelocity))
		{
			return (false);
		}
	}

	{
		Lepra::Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = -(lAxis2 * lAcceleration);
		pDiff.mAngleAcceleration = -(lAxis1 * lAcceleration);
	}

	return (true);
}

bool PhysicsEngineODE::SetUniversalDiff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_UNIVERSAL);

	Lepra::Vector3DF lAxis1;
	Lepra::Vector3DF lAxis2;
	{
		Lepra::Vector3DF lAnchor;
		float lAngle1;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAngle1(pJointId, lAngle1))
		{
			return (false);
		}
		assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);

		// Align anchors before rotation.
		Lepra::TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		dVector3 lRawAnchor;
		::dJointGetUniversalAnchor2(lJointInfo->mJointID, lRawAnchor);
		Lepra::Vector3DF lAnchor2(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
		lTransform.GetPosition() += lAnchor-lAnchor2;
		// Rotate around first body's axis.
		lTransform.RotateAroundAnchor(lAnchor, lAxis1, -pDiff.mValue+lAngle1);
		// Rotating around body 1's axis changes body 2's axis. Fetch and act on it after rotation 'round axis1.
		float lAngle2;
		if (!GetAxis2(pJointId, lAxis2) || !GetAngle2(pJointId, lAngle2))
		{
			return (false);
		}
		assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		lTransform.RotateAroundAnchor(lAnchor, lAxis2, -pDiff.mAngle+lAngle2);
		SetBodyTransform(pBodyId, lTransform);

		/*// TODO: remove test code.
		GetAngle1(pJointId, lAngle1);
		GetAngle2(pJointId, lAngle2);
		assert(Lepra::Math::IsEpsEqual(lAngle1, pDiff.mValue, 0.01f));
		assert(Lepra::Math::IsEpsEqual(lAngle2, pDiff.mAngle, 0.01f));*/
	}

	{
		Lepra::Vector3DF lAxisVelocity;
		Lepra::Vector3DF lOriginalVelocity;
		GetBodyAngularVelocity(pBodyId, lOriginalVelocity);
		Lepra::Vector3DF lVelocity = lOriginalVelocity;
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (pDiff.mValueVelocity < Lepra::PIF*1000)
		{
			lAxisVelocity = lAxis1*(lAxis1*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis1 * -pDiff.mValueVelocity;
		}
		if (pDiff.mAngleVelocity < Lepra::PIF*1000)
		{
			lAxisVelocity = lAxis2*(lAxis2*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis2 * -pDiff.mAngleVelocity;
		}
		//SetBodyVelocity(pBodyId, Lepra::Vector3DF());
		SetBodyAngularVelocity(pBodyId, lVelocity);
	}

	{
		Lepra::Vector3DF lAxisAcceleration;
		Lepra::Vector3DF lOriginalAcceleration;
		GetBodyAngularAcceleration(pBodyId, lOriginalAcceleration);
		Lepra::Vector3DF lAcceleration = lOriginalAcceleration;
		// Drop angular acceleration along axis 1 & 2, then add the specified amount.
		if (pDiff.mValueAcceleration < Lepra::PIF*1000)
		{
			lAxisAcceleration = lAxis1*(lAxis1*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis1 * -pDiff.mValueAcceleration;
		}
		if (pDiff.mAngleAcceleration < Lepra::PIF*1000)
		{
			lAxisAcceleration = lAxis2*(lAxis2*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis2 * -pDiff.mAngleAcceleration;
		}
		//SetBodyAcceleration(pBodyId, Lepra::Vector3DF());
		SetBodyAngularAcceleration(pBodyId, lAcceleration);
	}

	return (true);
}

bool PhysicsEngineODE::GetHinge2Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_HINGE2);

	Lepra::Vector3DF lAxis1;
	Lepra::Vector3DF lAxis2;
	{
		Lepra::TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		Lepra::Vector3DF lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2))
		{
			return (false);
		}
		assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		const Lepra::Vector3DF lDiff(lTransform.GetPosition()-lAnchor);
		float lPosition;
		lPosition = -(lAxis1 * lDiff);
		pDiff.mValue = lPosition;
		pDiff.mAngle2 = 0;	// JB-TODO: use this angle as well (go through body rather than ODE).
		if (!GetAngle1(pJointId, lPosition))	// JB: not available through ODE: || !GetAngle2(pJointId, pAngle2));
		{
			return (false);
		}
		pDiff.mAngle1 = lPosition;
	}

	{
		Lepra::Vector3DF lVelocity;
		GetBodyVelocity(pBodyId, lVelocity);
		pDiff.mValueVelocity = -(lAxis1 * lVelocity);
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mAngle1Velocity = -(lAxis2 * lVelocity);
		pDiff.mAngle2Velocity = -(lAxis1 * lVelocity);
	}

	{
		Lepra::Vector3DF lAcceleration;
		GetBodyAcceleration(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = -(lAxis1 * lAcceleration);
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mAngle1Acceleration = -(lAxis2 * lAcceleration);
		pDiff.mAngle2Acceleration = -(lAxis1 * lAcceleration);
	}

	return (true);
}

bool PhysicsEngineODE::SetHinge2Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_HINGE2);

	Lepra::Vector3DF lAxis1;
	Lepra::Vector3DF lAxis2;
	{
		Lepra::Vector3DF lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2))
		{
			return (false);
		}
		assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		const Lepra::Vector3DF lDiff = lAxis1 * -pDiff.mValue;
		Lepra::TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		lTransform.SetPosition(lAnchor+lDiff);
		float lCurrentAngle;
		if (pDiff.mAngle1 < Lepra::PIF*10)
		{
			if (!GetAngle1(pJointId, lCurrentAngle))	// JB: not available in ODE: || !GetAngle2(pJointId, lCurrentAngle2));
			{
				return (false);
			}
			lTransform.GetOrientation().RotateAroundVector(lAxis1, lCurrentAngle-pDiff.mAngle1);
		}
		if (pDiff.mAngle2 < Lepra::PIF*10)
		{
			pDiff.mAngle2;	// JB-TODO: use this angle as well.
		}
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		Lepra::Vector3DF lVelocity;
		GetBodyVelocity(pBodyId, lVelocity);
		// Drop suspension velocity along axis1.
		Lepra::Vector3DF lAxisVelocity(lAxis1*(lAxis1*lVelocity));
		lVelocity -= lAxisVelocity;
		// Add suspension velocity.
		lVelocity += lAxis1 * -pDiff.mValueVelocity;
		SetBodyVelocity(pBodyId, lVelocity);

		Lepra::Vector3DF lOriginalVelocity;
		GetBodyAngularVelocity(pBodyId, lOriginalVelocity);
		lVelocity = lOriginalVelocity;
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (pDiff.mAngle1Velocity < Lepra::PIF*1000)
		{
			lAxisVelocity = lAxis1*(lAxis1*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis2 * -pDiff.mAngle1Velocity;
		}
		if (pDiff.mAngle2Velocity < Lepra::PIF*1000)
		{
			lAxisVelocity = lAxis2*(lAxis2*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis1 * -pDiff.mAngle2Velocity;
		}
		SetBodyAngularVelocity(pBodyId, lVelocity);
	}

	{
		Lepra::Vector3DF lAcceleration;
		GetBodyAcceleration(pBodyId, lAcceleration);
		// Drop suspension acceleration along axis1.
		Lepra::Vector3DF lAxisAcceleration(lAxis1*(lAxis1*lAcceleration));
		lAcceleration -= lAxisAcceleration;
		// Add suspension acceleration.
		lAcceleration += lAxis1 * -pDiff.mValueAcceleration;
		SetBodyAcceleration(pBodyId, lAcceleration);

		Lepra::Vector3DF lOriginalAcceleration;
		GetBodyAngularAcceleration(pBodyId, lOriginalAcceleration);
		lAcceleration = lOriginalAcceleration;
		// Drop angular acceleration along axis 1 & 2, then add the specified amount.
		if (pDiff.mAngle1Acceleration < Lepra::PIF*1000)
		{
			lAxisAcceleration = lAxis1*(lAxis1*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis2 * -pDiff.mAngle1Acceleration;
		}
		if (pDiff.mAngle2Acceleration < Lepra::PIF*1000)
		{
			lAxisAcceleration = lAxis2*(lAxis2*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis1 * -pDiff.mAngle2Acceleration;
		}
		SetBodyAngularAcceleration(pBodyId, lAcceleration);
	}

	return (true);
}

bool PhysicsEngineODE::GetBallDiff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_BALL);

	{
		assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		Lepra::QuaternionF lParentQ(lPQ[0], lPQ[1], lPQ[2], lPQ[3]);

		Lepra::TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		const Lepra::QuaternionF lQ = lTransform.GetOrientation()/lParentQ;
		lQ.GetEulerAngles(pDiff.mValue, pDiff.mAngle1, pDiff.mAngle2);
	}

	{
		Lepra::Vector3DF lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mValueVelocity = lVelocity.x;
		pDiff.mAngle1Velocity = lVelocity.y;
		pDiff.mAngle2Velocity = lVelocity.z;
	}

	{
		Lepra::Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = lAcceleration.x;
		pDiff.mAngle1Acceleration = lAcceleration.y;
		pDiff.mAngle2Acceleration = lAcceleration.z;
	}

	return (true);
}

bool PhysicsEngineODE::SetBallDiff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_BALL);

	{
		assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		Lepra::QuaternionF lParentQ(lPQ[0], lPQ[1], lPQ[2], lPQ[3]);

		Lepra::TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		Lepra::QuaternionF lRelativeToParentQ(lParentQ/lTransform.GetOrientation());
		dVector3 lRawAnchor;
		::dJointGetBallAnchor2(lJointInfo->mJointID, lRawAnchor);
		Lepra::Vector3DF lAnchor2(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
		Lepra::Vector3DF lPosition = lTransform.GetPosition()-lAnchor2;
		lPosition = lRelativeToParentQ*lPosition;	// Go to parent space.
		Lepra::QuaternionF lRelativeFromParentQ;
		lRelativeFromParentQ.SetEulerAngles(pDiff.mValue, pDiff.mAngle1, pDiff.mAngle2);
		lPosition = lRelativeFromParentQ*lPosition;	// Go from parent to given child space.
		lTransform.SetOrientation(lRelativeFromParentQ*lParentQ);	// Set complete orientation.
		Lepra::Vector3DF lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor))
		{
			return (false);
		}
		lPosition += lAnchor;
		lTransform.SetPosition(lPosition);
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		// TODO: adjust linear velocity.
		//SetBodyVelocity(pBodyId, Lepra::Vector3DF(0, 0, 0));
		//SetBodyVelocity(pBodyId, Lepra::Vector3DF(pDiff.mValueVelocity, pDiff.mAngle1Velocity, pDiff.mAngle2Velocity));
		SetBodyAngularVelocity(pBodyId, Lepra::Vector3DF(pDiff.mValueVelocity, pDiff.mAngle1Velocity, pDiff.mAngle2Velocity));
		//SetBodyAngularVelocity(pBodyId, Lepra::Vector3DF(0, 0, 0));
	}

	{
		// TODO: adjust linear acceleration.
		//SetBodyAcceleration(pBodyId, Lepra::Vector3DF(0, 0, 0));
		//SetBodyAcceleration(pBodyId, Lepra::Vector3DF(pDiff.mValueAcceleration, pDiff.mAngle1Acceleration, pDiff.mAngle2Acceleration));
		SetBodyAngularAcceleration(pBodyId, Lepra::Vector3DF(pDiff.mValueAcceleration, pDiff.mAngle1Acceleration, pDiff.mAngle2Acceleration));
		//SetBodyAngularAcceleration(pBodyId, Lepra::Vector3DF(0, 0, 0));
	}

	return (true);
}

void PhysicsEngineODE::SetGeomTransform(dGeomID pGeomID, const Lepra::Transformation<Lepra::float32>& pTransform)
{
	const Lepra::Vector3D<Lepra::float32>& lPos = pTransform.GetPosition();
	dGeomSetPosition(pGeomID, lPos.x, lPos.y, lPos.z);

	const Lepra::Quaternion<Lepra::float32>& lQuat = pTransform.GetOrientation();
	dReal lQ[4];

	lQ[0] = lQuat.GetA();
	lQ[1] = lQuat.GetB();
	lQ[2] = lQuat.GetC();
	lQ[3] = lQuat.GetD();

	dGeomSetQuaternion(pGeomID, lQ);
}

bool PhysicsEngineODE::CheckBodies(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const Lepra::tchar* pFunction)
{
	if (pBody1 == 0 && pBody2 == 0)
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - body1 = body2 = NULL!");
		mLog.Error(lMsg);
		return (false);
	}

	if (pBody1 == pBody2)
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - body1 = body2!");
		mLog.Error(lMsg);
		return (false);
	}

	pObject1 = 0;
	pObject2 = 0;

	if (pBody1 == 0)
	{
		pBody1 = pBody2;
		pBody2 = INVALID_BODY;

		if (((Object*)pBody1)->mWorldID == mWorldID)
		{
			pObject1 = (Object*)pBody1;
			return (true);
		}
		else
		{
			Lepra::String lMsg(pFunction);
			lMsg += _T(" - Body is not part of this world!");
			mLog.Error(lMsg);
			return (false);
		}
	}
	else
	{
		if (((Object*)pBody1)->mWorldID == mWorldID)
		{
			pObject1 = (Object*)pBody1;
		}
		else
		{
			Lepra::String lMsg(pFunction);
			lMsg += _T(" - Body1 is not part of this world!");
			mLog.Error(lMsg);
			return (false);
		}

		if (((Object*)pBody2)->mWorldID == mWorldID)
		{
			pObject2 = (Object*)pBody2;
		}
		else
		{
			Lepra::String lMsg(pFunction);
			lMsg += _T(" - Body2 is not part of this world!");
			mLog.Error(lMsg);
			return (false);
		}

		return (true);
	}
}

bool PhysicsEngineODE::CheckBodies2(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const Lepra::tchar* pFunction)
{
	if (pBody1 == 0)
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - body1 = NULL!");
		mLog.Error(lMsg);
		return (false);
	}

	if (pBody2 == 0)
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - body2 = NULL!");
		mLog.Error(lMsg);
		return (false);
	}

	if (pBody1 == pBody2)
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - body1 = body2!");
		mLog.Error(lMsg);
		return (false);
	}

	pObject1 = 0;
	pObject2 = 0;

	if (((Object*)pBody1)->mWorldID == mWorldID)
	{
		pObject1 = (Object*)pBody1;
	}
	else
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - Body1 is not part of this world!");
		mLog.Error(lMsg);
		return (false);
	}

	if (((Object*)pBody2)->mWorldID == mWorldID)
	{
		pObject2 = (Object*)pBody2;
	}
	else
	{
		Lepra::String lMsg(pFunction);
		lMsg += _T(" - Body2 is not part of this world!");
		mLog.Error(lMsg);
		return (false);
	}

	return (true);
}

bool PhysicsEngineODE::GetAnchorPos(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAnchorPos) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAnchorPos() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	dVector3 lPos;

	switch ((*lIter)->mType)
	{
	case JOINT_BALL:
		dJointGetBallAnchor((*lIter)->mJointID, lPos);
		break;
	case JOINT_HINGE:
		dJointGetHingeAnchor((*lIter)->mJointID, lPos);
		break;
	case JOINT_HINGE2:
		dJointGetHinge2Anchor((*lIter)->mJointID, lPos);
		break;
	case JOINT_UNIVERSAL:
		dJointGetUniversalAnchor((*lIter)->mJointID, lPos);
		break;
	case JOINT_FIXED:
	case JOINT_ANGULARMOTOR:
	case JOINT_SLIDER:
		mLog.AError("GetAnchorPos() - Joint doesn't have an anchor!");
		return (false);
	default:
		mLog.AError("GetAnchorPos() - Unknown joint type!");
		return (false);
	};

	pAnchorPos.x = lPos[0];
	pAnchorPos.y = lPos[1];
	pAnchorPos.z = lPos[2];

	return (true);
}

bool PhysicsEngineODE::GetAxis1(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAxis1) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAxis1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	dVector3 lAxis;

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE:
		dJointGetHingeAxis((*lIter)->mJointID, lAxis);
		break;
	case JOINT_HINGE2:
		dJointGetHinge2Axis1((*lIter)->mJointID, lAxis);
		break;
	case JOINT_UNIVERSAL:
		dJointGetUniversalAxis1((*lIter)->mJointID, lAxis);
		break;
	case JOINT_ANGULARMOTOR:
		dJointGetAMotorAxis((*lIter)->mJointID, 0, lAxis);
		break;
	case JOINT_SLIDER:
		dJointGetSliderAxis((*lIter)->mJointID, lAxis);
		break;
	case JOINT_BALL:
	case JOINT_FIXED:
		mLog.AError("GetAxis1() - Joint doesn't have an axis!");
		return (false);
	default:
		mLog.AError("GetAxis1() - Unknown joint type!");
		return (false);
	};

	pAxis1.x = lAxis[0];
	pAxis1.y = lAxis[1];
	pAxis1.z = lAxis[2];

	return (true);
}

bool PhysicsEngineODE::GetAxis2(JointID pJointId, Lepra::Vector3D<Lepra::float32>& pAxis2) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAxis2() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	dVector3 lAxis;

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE2:
		dJointGetHinge2Axis2((*lIter)->mJointID, lAxis);
		break;
	case JOINT_UNIVERSAL:
		dJointGetUniversalAxis2((*lIter)->mJointID, lAxis);
		break;
	case JOINT_BALL:
	case JOINT_HINGE:
	case JOINT_SLIDER:
	case JOINT_FIXED:
	case JOINT_ANGULARMOTOR:
		mLog.AError("GetAxis2() - Joint doesn't have two axes!");
		return (false);
	default:
		mLog.AError("GetAxis2() - Unknown joint type!");
		return (false);
	};

	pAxis2.x = lAxis[0];
	pAxis2.y = lAxis[1];
	pAxis2.z = lAxis[2];

	return (true);
}

bool PhysicsEngineODE::GetAngle1(JointID pJointId, Lepra::float32& pAngle) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngle1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE2:
		pAngle = dJointGetHinge2Angle1((*lIter)->mJointID);
		break;
	case JOINT_UNIVERSAL:
		pAngle = dJointGetUniversalAngle1((*lIter)->mJointID);
		break;
	case JOINT_HINGE:
		pAngle = dJointGetHingeAngle((*lIter)->mJointID);
		break;
	case JOINT_ANGULARMOTOR:
		pAngle = dJointGetAMotorAngle((*lIter)->mJointID, 0);
		break;
	case JOINT_BALL:
	case JOINT_SLIDER:
	case JOINT_FIXED:
		mLog.AError("GetAngle1() - Joint doesn't have an angle!");
		return (false);
	default:
		mLog.AError("GetAngle1() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsEngineODE::GetAngle2(JointID pJointId, Lepra::float32& pAngle) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngle2() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_UNIVERSAL:
		pAngle = dJointGetUniversalAngle2((*lIter)->mJointID);
		break;
	case JOINT_HINGE2:
		// TODO: implement this!
		mLog.AError("GetAngle2() - Missing hinge2 implementation in ODE!");
		return (false);
	case JOINT_HINGE:
	case JOINT_ANGULARMOTOR:
	case JOINT_BALL:
	case JOINT_SLIDER:
	case JOINT_FIXED:
		mLog.AError("GetAngle2() - Joint doesn't have two angles!");
		return (false);
	default:
		mLog.AError("GetAngle2() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsEngineODE::GetAngleRate1(JointID pJointId, Lepra::float32& pAngleRate) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngleRate1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE2:
		pAngleRate = dJointGetHinge2Angle1Rate((*lIter)->mJointID);
		break;
	case JOINT_UNIVERSAL:
		pAngleRate = dJointGetUniversalAngle1Rate((*lIter)->mJointID);
		break;
	case JOINT_HINGE:
		pAngleRate = dJointGetHingeAngleRate((*lIter)->mJointID);
		break;
	case JOINT_ANGULARMOTOR:
		pAngleRate = dJointGetAMotorAngleRate((*lIter)->mJointID, 0);
		break;
	case JOINT_BALL:
	case JOINT_SLIDER:
	case JOINT_FIXED:
		mLog.AError("GetAngleRate1() - Joint doesn't have an angle!");
		return (false);
	default:
		mLog.AError("GetAngleRate1() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsEngineODE::GetAngleRate2(JointID pJointId, Lepra::float32& pAngleRate) const
{	
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngleRate2() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE2:
		pAngleRate = dJointGetHinge2Angle2Rate((*lIter)->mJointID);
		break;
	case JOINT_UNIVERSAL:
		pAngleRate = dJointGetUniversalAngle2Rate((*lIter)->mJointID);
		break;
	case JOINT_BALL:
	case JOINT_HINGE:
	case JOINT_SLIDER:
	case JOINT_FIXED:
	case JOINT_ANGULARMOTOR:
		mLog.AError("GetAngleRate2() - Joint doesn't have two angles!");
		return (false);
	default:
		mLog.AError("GetAngleRate2() - Unknown joint type!");
		return (false);
	};

	return (true);
}

bool PhysicsEngineODE::SetAngle1(BodyID pBodyId, JointID pJointId, Lepra::float32 pAngle)
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngle1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyData() - Body %i is not part of this world!"), pBodyId);
		return (false);
	}

	JointInfo* lJoint = (JointInfo*)pJointId;
	switch (lJoint->mType)
	{
		case JOINT_HINGE:
		{
			assert(::dJointGetBody(lJoint->mJointID, 1) == lObject->mBodyID);
			const float lCurrentAngle = ::dJointGetHingeAngle(lJoint->mJointID);
			if (lCurrentAngle == pAngle)
			{
				return (true);
			}
			Lepra::TransformationF lTransform;
			GetBodyTransform(pBodyId, lTransform);
			Lepra::Vector3DF lAxis;
			if (GetAxis1(pJointId, lAxis))
			{
				dVector3 lRawAnchor;
				::dJointGetHingeAnchor((*lIter)->mJointID, lRawAnchor);
				Lepra::Vector3DF lAnchor(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
				::dJointGetHingeAnchor2((*lIter)->mJointID, lRawAnchor);
				Lepra::Vector3DF lAnchor2(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
				lTransform.GetPosition() += lAnchor-lAnchor2;
				lTransform.RotateAroundAnchor(lAnchor, lAxis, -pAngle+lCurrentAngle);
				SetBodyTransform(pBodyId, lTransform);
				return (true);
			}
			else
			{
				mLog.AError("SetAngle1() - hinge-2 joint error!");
			}
		}
		break;
		case JOINT_HINGE2:
		{
			assert(::dJointGetBody(lJoint->mJointID, 1) == lObject->mBodyID);
			const float lCurrentAngle = ::dJointGetHinge2Angle1(lJoint->mJointID);
			if (lCurrentAngle == pAngle)
			{
				return (true);
			}
			Lepra::TransformationF lTransform;
			GetBodyTransform(pBodyId, lTransform);
			Lepra::Vector3DF lAxis1;
			if (GetAxis1(pJointId, lAxis1))
			{
				lTransform.GetOrientation().RotateAroundVector(lAxis1, -pAngle+lCurrentAngle);
				SetBodyTransform(pBodyId, lTransform);
				return (true);
			}
			else
			{
				mLog.AError("SetAngle1() - hinge-2 joint error!");
			}
		}
		break;
		default:
		{
			mLog.AError("SetAngle1() - Unknown joint type!");
		}
	};

	return (false);
}

bool PhysicsEngineODE::SetAngularMotorAngle(JointID pJointId, Lepra::float32 pAngle)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorAngle() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("SetAngularMotorAngle() - Joint is not an angular motor!");
		return (false);
	}

	dJointSetAMotorAngle((*lIter)->mJointID, 0, pAngle);
	return (true);
}

bool PhysicsEngineODE::SetAngularMotorSpeed(JointID pJointId, Lepra::float32 pSpeed)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorSpeed() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("SetAngularMotorSpeed() - Joint is not an angular motor!");
		return (false);
	}

	dJointSetAMotorParam((*lIter)->mJointID, dParamVel, pSpeed);
	return (true);
}

bool PhysicsEngineODE::SetAngularMotorMaxForce(JointID pJointId, Lepra::float32 pMaxForce)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorMaxForce() - Couldn't find joint %i!"), pJointId);
		return (false);
	}


	JointInfo* lJoint = *lIter;
	if (lJoint->mType == JOINT_ANGULARMOTOR)
	{
		dJointSetAMotorParam(lJoint->mJointID, dParamFMax, pMaxForce);
		return (true);
	}
	else if (lJoint->mType == JOINT_HINGE2)
	{
		dJointSetHinge2Param(lJoint->mJointID, dParamFMax, pMaxForce);
		dJointSetHinge2Param(lJoint->mJointID, dParamFMax2, pMaxForce);
		return (true);
	}
	mLog.AError("SetAngularMotorMaxForce() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsEngineODE::SetAngularMotorRoll(JointID pJointId, Lepra::float32 pMaxForce, Lepra::float32 pTargetVelocity)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorRoll() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *lIter;
	if (lJoint->mType == JOINT_HINGE2)
	{
		::dJointSetHinge2Param(lJoint->mJointID, dParamFMax2, pMaxForce);
		::dJointSetHinge2Param(lJoint->mJointID, dParamVel2, pTargetVelocity);
		::dBodyEnable(::dJointGetBody(lJoint->mJointID, 1));
		return (true);
	}
	mLog.AError("SetAngularMotorRoll() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsEngineODE::GetAngularMotorRoll(JointID pJointId, Lepra::float32& pMaxForce, Lepra::float32& pTargetVelocity)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorRoll() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *lIter;
	if (lJoint->mType == JOINT_HINGE2)
	{
		pMaxForce = ::dJointGetHinge2Param(lJoint->mJointID, dParamFMax2);
		pTargetVelocity = ::dJointGetHinge2Param(lJoint->mJointID, dParamVel2);
		return (true);
	}
	mLog.AError("GetAngularMotorRoll() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsEngineODE::SetAngularMotorTurn(JointID pJointId, Lepra::float32 pMaxForce, Lepra::float32 pTargetVelocity)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorTurn() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *lIter;
	if (lJoint->mType == JOINT_HINGE2)
	{
		dJointSetHinge2Param(lJoint->mJointID, dParamFMax, pMaxForce);
		dJointSetHinge2Param(lJoint->mJointID, dParamVel, pTargetVelocity);
		::dBodyEnable(::dJointGetBody(lJoint->mJointID, 1));
		return (true);
	}
	else if (lJoint->mType == JOINT_HINGE)
	{
		dJointSetHingeParam(lJoint->mJointID, dParamFMax, pMaxForce);
		dJointSetHingeParam(lJoint->mJointID, dParamVel, pTargetVelocity);
		::dBodyEnable(::dJointGetBody(lJoint->mJointID, 1));
		return (true);
	}
	mLog.AError("SetAngularMotorTurn() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsEngineODE::GetAngularMotorAngle(JointID pJointId, Lepra::float32& pAngle) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorAngle() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("GetAngularMotorAngle() - Joint is not an angular motor!");
		return (false);
	}

	pAngle = dJointGetAMotorAngle((*lIter)->mJointID, 0);
	return (true);
}

bool PhysicsEngineODE::GetAngularMotorSpeed(JointID pJointId, Lepra::float32& pSpeed) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorSpeed() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("GetAngularMotorSpeed() - Joint is not an angular motor!");
		return (false);
	}

	pSpeed = dJointGetAMotorParam((*lIter)->mJointID, dParamVel);
	return (true);
}

bool PhysicsEngineODE::GetAngularMotorMaxForce(JointID pJointId, Lepra::float32& pMaxForce) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorMaxForce() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("GetAngularMotorMaxForce() - Joint is not an angular motor!");
		return (false);
	}

	pMaxForce = dJointGetAMotorParam((*lIter)->mJointID, dParamFMax);
	return (true);
}

bool PhysicsEngineODE::SetJointParams(JointID pJointId, Lepra::float32 pLowStop, Lepra::float32 pHighStop, Lepra::float32 pBounce)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("SetJointParams() - Couldn't find joint!"), pJointId);
		return (false);
	}

	dJointID lJointId = (*lIter)->mJointID;
	switch ((*lIter)->mType)
	{
		case JOINT_SLIDER:
		{
			dJointSetSliderParam(lJointId, dParamLoStop, pLowStop);
			dJointSetSliderParam(lJointId, dParamHiStop, pHighStop);
			dJointSetSliderParam(lJointId, dParamBounce, pBounce);
		}
		break;
		case JOINT_HINGE:
		{
			dJointSetHingeParam(lJointId, dParamLoStop, pLowStop);
			dJointSetHingeParam(lJointId, dParamHiStop, pHighStop);
			dJointSetHingeParam(lJointId, dParamBounce, pBounce);
			dJointSetHingeParam(lJointId, dParamCFM, 1e-5f);
		}
		break;
		case JOINT_HINGE2:
		{
			dJointSetHinge2Param(lJointId, dParamLoStop, pLowStop);
			dJointSetHinge2Param(lJointId, dParamHiStop, pHighStop);
			dJointSetHinge2Param(lJointId, dParamBounce, pBounce);
		}
		break;
		case JOINT_UNIVERSAL:
		{
			dJointSetUniversalParam(lJointId, dParamLoStop, pLowStop);
			dJointSetUniversalParam(lJointId, dParamHiStop, pHighStop);
			dJointSetUniversalParam(lJointId, dParamBounce, pBounce);
		}
		break;
		case JOINT_ANGULARMOTOR:
		{
			dJointSetAMotorParam(lJointId, dParamLoStop, pLowStop);
			dJointSetAMotorParam(lJointId, dParamHiStop, pHighStop);
			dJointSetAMotorParam(lJointId, dParamBounce, pBounce);
		}
		break;
		case JOINT_BALL:
		case JOINT_FIXED:
		{
			mLog.AError("SetJointParams() - joint doesn't have params!");
		}
		break;
		default:
		{
			mLog.AError("SetJointParams() - Unknown joint type!");
		}
		return (false);
	}

	return (true);
}

bool PhysicsEngineODE::GetJointParams(JointID pJointId, Lepra::float32& pLowStop, Lepra::float32& pHighStop, Lepra::float32& pBounce) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetJointParams() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_SLIDER:
		pLowStop  = dJointGetSliderParam((*lIter)->mJointID, dParamLoStop);
		pHighStop = dJointGetSliderParam((*lIter)->mJointID, dParamHiStop);
		pBounce   = dJointGetSliderParam((*lIter)->mJointID, dParamBounce);
		break;
	case JOINT_HINGE:
		pLowStop  = dJointGetHingeParam((*lIter)->mJointID, dParamLoStop);
		pHighStop = dJointGetHingeParam((*lIter)->mJointID, dParamHiStop);
		pBounce   = dJointGetHingeParam((*lIter)->mJointID, dParamBounce);
		break;
	case JOINT_HINGE2:
		pLowStop  = dJointGetHinge2Param((*lIter)->mJointID, dParamLoStop);
		pHighStop = dJointGetHinge2Param((*lIter)->mJointID, dParamHiStop);
		pBounce   = dJointGetHinge2Param((*lIter)->mJointID, dParamBounce);
		break;
	case JOINT_UNIVERSAL:
		pLowStop  = dJointGetUniversalParam((*lIter)->mJointID, dParamLoStop);
		pHighStop = dJointGetUniversalParam((*lIter)->mJointID, dParamHiStop);
		pBounce   = dJointGetUniversalParam((*lIter)->mJointID, dParamBounce);
		break;
	case JOINT_ANGULARMOTOR:
		pLowStop  = dJointGetAMotorParam((*lIter)->mJointID, dParamLoStop);
		pHighStop = dJointGetAMotorParam((*lIter)->mJointID, dParamHiStop);
		pBounce   = dJointGetAMotorParam((*lIter)->mJointID, dParamBounce);
		break;
	case JOINT_BALL:
	case JOINT_FIXED:
		mLog.AError("GetJointParams() - joint doesn't have params!");
	default:
		mLog.AError("GetJointParams() - Unknown joint type!");
		return (false);
	}

	return (true);
}

bool PhysicsEngineODE::SetSuspension(JointID pJointId, Lepra::float32 pFrameTime, Lepra::float32 pSpringConstant, Lepra::float32 pDampingConstant)
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetJointParams() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *lIter;
	if (lJoint->mType == JOINT_HINGE2)
	{
		::dJointSetHinge2Param(lJoint->mJointID, dParamSuspensionERP,
			pFrameTime * pSpringConstant / (pFrameTime * pSpringConstant + pDampingConstant));
		::dJointSetHinge2Param(lJoint->mJointID, dParamSuspensionCFM,
			1 / (pFrameTime * pSpringConstant + pDampingConstant));
		return (true);
	}
	mLog.AError("SetSuspension() - Joint is not a hinge-2!");
	return (false);
}

bool PhysicsEngineODE::GetSliderPos(JointID pJointId, Lepra::float32& pPos) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetSliderPos() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_SLIDER)
	{
		mLog.AError("GetSliderPos() - Joint is not a slider!");
		return (false);
	}

	pPos = dJointGetSliderPosition((*lIter)->mJointID);
	return (true);
}

bool PhysicsEngineODE::GetSliderSpeed(JointID pJointId, Lepra::float32& pSpeed) const
{
	JointTable::const_iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("GetSliderSleep() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_SLIDER)
	{
		mLog.AError("GetSliderSleep() - Joint is not a slider!");
		return (false);
	}

	pSpeed = dJointGetSliderPositionRate((*lIter)->mJointID);

	return (true);
}

bool PhysicsEngineODE::AddJointForce(JointID pJointId, Lepra::float32 pForce)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("AddJointForce() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*lIter)->mType != JOINT_SLIDER)
	{
		mLog.AError("AddJointForce() - Joint is not a slider!");
		return (false);
	}

	dJointAddSliderForce((*lIter)->mJointID, pForce);

	return (true);
}

bool PhysicsEngineODE::AddJointTorque(JointID pJointId, Lepra::float32 pTorque)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("AddJointTorque() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE:
		dJointAddHingeTorque((*lIter)->mJointID, pTorque);
		break;
	case JOINT_HINGE2:
		dJointAddHinge2Torques((*lIter)->mJointID, pTorque, 0);
		break;
	case JOINT_UNIVERSAL:
		dJointAddUniversalTorques((*lIter)->mJointID, pTorque, 0);
		break;
	case JOINT_ANGULARMOTOR:
		dJointAddAMotorTorques((*lIter)->mJointID, pTorque, 0, 0);
		break;
	case JOINT_BALL:
	case JOINT_FIXED:
	case JOINT_SLIDER:
		mLog.AError("AddJointTorque() - joint is of wrong type!");
	default:
		mLog.AError("AddJointTorque() - Unknown joint type!");
		return (false);
	}

	return (true);
}

bool PhysicsEngineODE::AddJointTorque(JointID pJointId, Lepra::float32 pTorque1, Lepra::float32 pTorque2)
{
	JointTable::iterator lIter = mJointTable.find((JointInfo*)pJointId);
	if (lIter == mJointTable.end())
	{
		mLog.Errorf(_T("AddJointTorque() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*lIter)->mType)
	{
	case JOINT_HINGE2:
		dJointAddHinge2Torques((*lIter)->mJointID, pTorque1, pTorque2);
		break;
	case JOINT_UNIVERSAL:
		dJointAddUniversalTorques((*lIter)->mJointID, pTorque1, pTorque2);
		break;
	case JOINT_BALL:
	case JOINT_HINGE:
	case JOINT_FIXED:
	case JOINT_SLIDER:
	case JOINT_ANGULARMOTOR:
		mLog.AError("AddJointTorque() - joint is of wrong type!");
	default:
		mLog.AError("AddJointTorque() - Unknown joint type!");
		return (false);
	}

	return (true);
}

void PhysicsEngineODE::AddForce(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForce() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForce(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z);
}

void PhysicsEngineODE::AddTorque(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pTorque)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddTorque() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddTorque(((Object*)pBodyId)->mBodyID, pTorque.x, pTorque.y, pTorque.z);
}

void PhysicsEngineODE::AddRelForce(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForce() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForce(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z);
}

void PhysicsEngineODE::AddRelTorque(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pTorque)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelTorque() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelTorque(((Object*)pBodyId)->mBodyID, pTorque.x, pTorque.y, pTorque.z);
}

void PhysicsEngineODE::AddForceAtPos(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
									 const Lepra::Vector3D<Lepra::float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForceAtPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForceAtPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsEngineODE::AddForceAtRelPos(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
										const Lepra::Vector3D<Lepra::float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForceAtRelPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForceAtRelPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsEngineODE::AddRelForceAtPos(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
										const Lepra::Vector3D<Lepra::float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForceAtPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForceAtPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsEngineODE::AddRelForceAtRelPos(BodyID pBodyId, const Lepra::Vector3D<Lepra::float32>& pForce,
										   const Lepra::Vector3D<Lepra::float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForceAtRelPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForceAtRelPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsEngineODE::RestrictBody(BodyID pBodyId, Lepra::float32 pMaxSpeed, Lepra::float32 pMaxAngularSpeed)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("RestrictBody() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	const dReal* lAVel = dBodyGetAngularVel(((Object*)pBodyId)->mBodyID);
	const dReal* lLVel = dBodyGetLinearVel(((Object*)pBodyId)->mBodyID);

	Lepra::float32 lASpeed = lAVel[0] * lAVel[0] + lAVel[1] * lAVel[1] + lAVel[2] * lAVel[2];
	Lepra::float32 lLSpeed = lLVel[0] * lLVel[0] + lLVel[1] * lLVel[1] + lLVel[2] * lLVel[2];

	if (lASpeed > pMaxAngularSpeed * pMaxAngularSpeed)
	{
		Lepra::float32 k = pMaxAngularSpeed / sqrtf(lASpeed);
		dBodySetAngularVel(((Object*)pBodyId)->mBodyID, lAVel[0] * k, lAVel[1] * k, lAVel[2] * k);
	}

	if (lLSpeed > pMaxSpeed * pMaxSpeed)
	{
		Lepra::float32 k = pMaxSpeed / sqrtf(lLSpeed);
		dBodySetLinearVel(((Object*)pBodyId)->mBodyID, lLVel[0] * k, lLVel[1] * k, lLVel[2] * k);
	}
}

void PhysicsEngineODE::ActivateGravity(BodyID pBodyId)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("ActivateGravity() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodySetGravityMode(((Object*)pBodyId)->mBodyID, 1);
}

void PhysicsEngineODE::DeactivateGravity(BodyID pBodyId)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("DeactivateGravity() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodySetGravityMode(((Object*)pBodyId)->mBodyID, 0);
}

void PhysicsEngineODE::SetGravity(const Lepra::Vector3D<Lepra::float32>& pGravity)
{
	dWorldSetGravity(mWorldID, pGravity.x, pGravity.y, pGravity.z);
}

void PhysicsEngineODE::StepAccurate(Lepra::float32 pStepSize)
{
	if (pStepSize > 0)
	{
		ListEnabledObjects();

		dSpaceCollide(mSpaceID, this, CollisionCallback);
		dWorldStep(mWorldID, pStepSize);

		HandleAutoDisabledObjects();
		DoForceFeedback();
		dJointGroupEmpty(mContactJointGroupID);
	}
}

void PhysicsEngineODE::StepFast(Lepra::float32 pStepSize)
{
	if (pStepSize > 0)
	{
		ListEnabledObjects();

		dSpaceCollide(mSpaceID, this, CollisionCallback);
		dWorldQuickStep(mWorldID, pStepSize);
		
		HandleAutoDisabledObjects();
		DoForceFeedback();
		dJointGroupEmpty(mContactJointGroupID);
	}
}

void PhysicsEngineODE::DoForceFeedback()
{
	JointList::iterator lIter;
	for (lIter = mFeedbackJointList.begin(); lIter != mFeedbackJointList.end();)
	{
		JointInfo* lJointInfo = *lIter;

		if (lJointInfo->mListener1 != lJointInfo->mListener2)
		{
			if (lJointInfo->mListener1 != 0)
			{
				dJointFeedback* lFeedback = &lJointInfo->mFeedback;
				lJointInfo->mListener1->OnForceApplied(
					lJointInfo->mListener2,
					Lepra::Vector3D<Lepra::float32>(lFeedback->f1[0], lFeedback->f1[1], lFeedback->f1[2]),
					Lepra::Vector3D<Lepra::float32>(lFeedback->t1[0], lFeedback->t1[1], lFeedback->t1[2]));
			}
			if (lJointInfo->mListener2 != 0)
			{
				dJointFeedback* lFeedback = &lJointInfo->mFeedback;
				lJointInfo->mListener2->OnForceApplied(
					lJointInfo->mListener1,
					Lepra::Vector3D<Lepra::float32>(-lFeedback->f1[0], -lFeedback->f1[1], -lFeedback->f1[2]),
					// TODO: check out how the torque should be. The force2 is always -force1
					// (actually f2[] just contains crap), but where does the torque2 go, if not in torque1?
					Lepra::Vector3D<Lepra::float32>(-lFeedback->t1[0], -lFeedback->t1[1], -lFeedback->t1[2]));
			}
		}

		if (lJointInfo->mType == JOINT_CONTACT)
		{
			mFeedbackJointList.erase(lIter++);
			mJointInfoAllocator.Free(lJointInfo);
		}
		else
		{
			++lIter;
		}
	}
}

void PhysicsEngineODE::CollisionCallback(void* pData, dGeomID pGeom1, dGeomID pGeom2)
{
	Object* lObject1 = (Object*)dGeomGetData(pGeom1);
	Object* lObject2 = (Object*)dGeomGetData(pGeom2);

	if (lObject1->mForceFeedbackListener != 0 && lObject1->mForceFeedbackListener == lObject2->mForceFeedbackListener)
	{
		// The same force feedback recipient means same object. We don't collide here.
		return;
	}

	dBodyID lBody1 = dGeomGetBody(pGeom1);
	dBodyID lBody2 = dGeomGetBody(pGeom2);

	// Check if all bodies are static or disabled.
	if ((!lBody1 || !dBodyIsEnabled(lBody1)) && (!lBody2 || !dBodyIsEnabled(lBody2)))
	{
		// We don't want to act on static and disabled bodies.
		return;
	}
	if (lObject1->mTriggerListener == 0 && lObject2->mTriggerListener == 0)
	{
		// Exit without doing anything if the two bodies are connected by a joint.
		if (lBody1 && lBody2 && dAreConnectedExcluding(lBody1, lBody2, dJointTypeContact) != 0)
		{
			return;
		}
	}

	PhysicsEngineODE* lThis = (PhysicsEngineODE*)pData;
	dContact lContact[8];

	if (lObject1->mTriggerListener != 0 && lBody2 != 0)
	{
		if (dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(dContact)) > 0)
		{
			lObject1->mTriggerListener->OnBodyInside((BodyID)(Lepra::uint64)lObject2);
		}
	}
	if(lObject2->mTriggerListener != 0 && lBody1 != 0)
	{
		if (dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(dContact)) > 0)
		{
			lObject2->mTriggerListener->OnBodyInside((BodyID)(Lepra::uint64)lObject1);
		}
	}

	// Bounce if NOT BOTH objects are triggers.
	if (lObject1->mTriggerListener == 0 || lObject2->mTriggerListener == 0)
	{
		int lNumContactPoints = dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(dContact));
		dMass lMass1;
		if (lBody1)
		{
			::dBodyGetMass(lBody1, &lMass1);
		}
		else
		{
			lMass1.mass = 1.0;
		}
		dMass lMass2;
		if (lBody2)
		{
			::dBodyGetMass(lBody2, &lMass2);
		}
		else
		{
			lMass2.mass = 1.0;
		}

		// Perform normal collision detection.
		for (int i = 0; i < lNumContactPoints; i++)
		{
			lContact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactBounce | dContactApprox1;
			lContact[i].surface.mu = dInfinity;
			const float lSlip = 0.1f / (lObject1->mFriction * lObject2->mFriction * lMass1.mass * lMass2.mass);
			lContact[i].surface.slip1 = lSlip;
			lContact[i].surface.slip2 = lSlip;
			lContact[i].surface.bounce = (dReal)(lObject1->mBounce * lObject2->mBounce);
			lContact[i].surface.bounce_vel = (dReal)0.000001;

			if (lObject1->mForceFeedbackListener != 0 ||
			   lObject2->mForceFeedbackListener != 0)
			{
				// Create a joint whith feedback info.
				JointInfo* lJointInfo = lThis->mJointInfoAllocator.Alloc();
				lJointInfo->mJointID = dJointCreateContact(lThis->mWorldID, lThis->mContactJointGroupID, &lContact[i]);
				lJointInfo->mType = JOINT_CONTACT;
				lThis->mFeedbackJointList.push_back(lJointInfo);
				lJointInfo->mListIter = --lThis->mFeedbackJointList.end();
				lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
				lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

				dJointAttach(lJointInfo->mJointID, lBody1, lBody2);
				dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
			}
			else
			{
				// Create a temporary joint without feedback info.
				dJointID lJointID = dJointCreateContact(lThis->mWorldID, lThis->mContactJointGroupID, &lContact[i]);
				dJointAttach(lJointID, lBody1, lBody2);
			}
		}
	}
}



PhysicsEngine::BodySet PhysicsEngineODE::GetIdledBodies() const
{
	return (mAutoDisabledObjectSet);
}



void PhysicsEngineODE::ListEnabledObjects()
{
	mAutoDisabledObjectSet.clear();
	ObjectTable::iterator x = mObjectTable.begin();
	for (; x != mObjectTable.end(); ++x)
	{
		Object* lObject = *x;
		if (lObject->mBodyID && dBodyIsEnabled(lObject->mBodyID))
		{
			mAutoDisabledObjectSet.insert((BodyID)lObject);
		}
	}
}

void PhysicsEngineODE::HandleAutoDisabledObjects()
{
	BodySet::iterator x = mAutoDisabledObjectSet.begin();
	while(x != mAutoDisabledObjectSet.end())
	{
		Object* lObject = (Object*)(*x);
		if (lObject->mBodyID && dBodyIsEnabled(lObject->mBodyID))
		{
			mAutoDisabledObjectSet.erase(x++);
		}
		else
		{
			++x;
		}
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsEngineODE);



}
