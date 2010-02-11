
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#include <../ode/src/collision_kernel.h>
#include <../ode/src/joints/hinge.h>
#include <../ode/src/joints/universal.h>
#pragma warning(pop)
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../Include/PhysicsManagerODE.h"



namespace TBC
{



PhysicsManagerODE::PhysicsManagerODE()
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

PhysicsManagerODE::~PhysicsManagerODE()
{
	while (!mObjectTable.empty())
	{
		Object* lObject = *mObjectTable.begin();
		DeleteBody((BodyID)lObject);
	}

	::dWorldDestroy(mWorldID);
	::dSpaceDestroy(mSpaceID);
	::dJointGroupDestroy(mContactJointGroupID);
}

bool PhysicsManagerODE::InitCurrentThread()
{
	return ::dAllocateODEDataForThread((unsigned)dAllocateMaskAll) != 0;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateSphere(bool pIsRoot, const TransformationF& pTransform,
	float32 pMass, float32 pRadius, BodyType pType, float32 pFriction, float32 pBounce,
	TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, pIsRoot);
	lObject->mGeomID = dCreateSphere(mSpaceID, (dReal)pRadius);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;
	//assert(pType == STATIC || lObject->mForceFeedbackListener);

	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetSphereTotal(&lMass, (dReal)pMass, (dReal)pRadius);
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	::dGeomSetData(lObject->mGeomID, lObject);

	lObject->mGeometryData[0] = pRadius;
	lObject->mMass = pMass;
	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return ((BodyID)lObject);
}

PhysicsManager::BodyID PhysicsManagerODE::CreateCylinder(bool pIsRoot, const TransformationF& pTransform,
	float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction,
	float32 pBounce, TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	// TODO: Create a real cylinder when ODE supports it.
	lObject->mGeomID = ::dCreateCylinder(mSpaceID, (dReal)pRadius, (dReal)pLength);
	mLog.AWarning("Warning! Cylinders are not accurately supported by ODE!");

	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;
	//assert(pType == STATIC || lObject->mForceFeedbackListener);

	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetCylinderTotal(&lMass, (dReal)pMass, 3, (dReal)pRadius, (dReal)pLength);
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	::dGeomSetData(lObject->mGeomID, lObject);

	lObject->mGeometryData[0] = pRadius;
	lObject->mGeometryData[1] = pLength;
	lObject->mMass = pMass;
	lObject->mFriction = pFriction;
	lObject->mBounce   = pBounce;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (BodyID)lObject;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateCapsule(bool pIsRoot, const TransformationF& pTransform,
	float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction,
	float32 pBounce, TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	lObject->mGeomID = ::dCreateCapsule(mSpaceID, (dReal)pRadius, (dReal)pLength);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;
	//assert(pType == STATIC || lObject->mForceFeedbackListener);

	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetCylinderTotal(&lMass, (dReal)pMass, 3, (dReal)pRadius, (dReal)pLength);
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	lObject->mGeometryData[0] = pRadius;
	lObject->mGeometryData[1] = pLength;
	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mMass = pMass;
	lObject->mFriction = pFriction;
	lObject->mBounce = pBounce;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (BodyID)lObject;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateBox(bool pIsRoot, const TransformationF& pTransform,
	float32 pMass, const Vector3D<float32>& pSize, BodyType pType, float32 pFriction,
	float32 pBounce, TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	lObject->mGeomID = ::dCreateBox(mSpaceID, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;
	//assert(pType == STATIC || lObject->mForceFeedbackListener);

	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetBoxTotal(&lMass, (dReal)pMass, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);
		lObject->mBodyID = ::dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}

	::dGeomSetData(lObject->mGeomID, lObject);

	lObject->mGeometryData[0] = pSize.x;
	lObject->mGeometryData[1] = pSize.y;
	lObject->mGeometryData[2] = pSize.z;
	lObject->mMass = pMass;
	lObject->mFriction = -pFriction;
	lObject->mBounce = pBounce;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (BodyID)lObject;
}

bool PhysicsManagerODE::Attach(BodyID pStaticBody, BodyID pMainBody)
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
	if (lStaticObject->mBodyID)
	{
		mLog.AError("Attach() with non-static.");
		assert(false);
		return (false);
	}
	dVector3 lPos;
	::dGeomCopyPosition(lStaticObject->mGeomID, lPos);
	dQuaternion o;
	::dGeomGetQuaternion(lStaticObject->mGeomID, o);

	dBodyID lBodyId = lMainObject->mBodyID;
	if (lBodyId)
	{
		::dGeomSetBody(lStaticObject->mGeomID, lBodyId);
		::dGeomSetOffsetWorldPosition(lStaticObject->mGeomID, lPos[0], lPos[1], lPos[2]);
		::dGeomSetOffsetWorldQuaternion(lStaticObject->mGeomID, o);

		dMass lMass;
		const dReal lMassScalar = (dReal)lStaticObject->mMass;
		assert(lMassScalar > 0);
		float* lSize = lStaticObject->mGeometryData;
		// Adding mass to the dynamic object.
		switch (lStaticObject->mGeomID->type)
		{
			case dTriMeshClass:	// TRICKY: fall through (act as sphere).
			case dSphereClass:	::dMassSetSphereTotal(&lMass, lMassScalar, (dReal)lSize[0]);					break;
			case dBoxClass:		::dMassSetBoxTotal(&lMass, lMassScalar, (dReal)lSize[0], (dReal)lSize[1], (dReal)lSize[2]);	break;
			case dCapsuleClass:	::dMassSetCylinderTotal(&lMass, lMassScalar, 3, (dReal)lSize[0], (dReal)lSize[1]);		break;
			case dCylinderClass:	::dMassSetCylinderTotal(&lMass, lMassScalar, 3, (dReal)lSize[0], (dReal)lSize[1]);	break;
			default:
			{
				mLog.AError("Trying to attach object of unknown type!");
				assert(false);
				return (false);
			}
		}
		const dReal* lRelRot = ::dGeomGetOffsetRotation(lStaticObject->mGeomID);
		const dReal* lRelPos = ::dGeomGetOffsetPosition(lStaticObject->mGeomID);
		::dMassTranslate(&lMass, lRelPos[0], lRelPos[1], lRelPos[2]);
		::dMassRotate(&lMass, lRelRot);

		dMass lDynamicMass;
		::dBodyGetMass(lBodyId, &lDynamicMass);
		::dMassAdd(&lMass, &lDynamicMass);
		::dBodySetMass(lBodyId, &lMass);
	}

	return (true);
}

PhysicsManager::BodyID PhysicsManagerODE::CreateTriMesh(bool pIsRoot, unsigned pVertexCount,
	const float* pVertices, unsigned pTriangleCount, const Lepra::uint32* pIndices,
	const TransformationF& pTransform, float32 pFriction, float32 pBounce,
	TriggerListener* pTriggerListener, ForceFeedbackListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	lObject->mTriMeshID = ::dGeomTriMeshDataCreate();
	::dGeomTriMeshDataBuildSingle(lObject->mTriMeshID,
				    pVertices,
				    sizeof(pVertices[0]) * 3,
				    pVertexCount,
				    pIndices,
				    pTriangleCount * 3,
				    sizeof(pIndices[0]) * 3);

	lObject->mGeomID = ::dCreateTriMesh(mSpaceID, lObject->mTriMeshID, 0, 0, 0);
	//::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
	::dGeomSetData(lObject->mGeomID, lObject);
	lObject->mTriggerListener = pTriggerListener;
	lObject->mForceFeedbackListener = pForceListener;
	assert(lObject->mForceFeedbackListener);

//	dGeomTriMeshEnableTC(lObject->mGeomID, dBoxClass, 1);

	// TODO: add body approximation (sphere).

	lObject->mGeometryData[0] = 1.0f;	// TODO: approximate sphere radius by calculating average vertex distance.
	lObject->mMass = 1.0f;	// TODO: add mass to interface when/if suitable.
	lObject->mFriction = -pFriction;
	lObject->mBounce = pBounce;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (BodyID)lObject;
}

bool PhysicsManagerODE::IsStaticBody(BodyID pBodyId) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("IsStaticBody() - Body %i is not part of this world!"), pBodyId);
		return (true);
	}
	return (lObject->mBodyID == 0);
}

void PhysicsManagerODE::DeleteBody(BodyID pBodyId)
{
	ObjectTable::iterator x = mObjectTable.find((Object*)pBodyId);
	if (x != mObjectTable.end())
	{
		Object* lObject = *x;
		if (lObject->mBodyID != 0)
		{
			::dBodyDestroy(lObject->mBodyID);
		}
		if (lObject->mTriMeshID != 0)
		{
			::dGeomTriMeshDataDestroy(lObject->mTriMeshID);
		}
		::dGeomDestroy(lObject->mGeomID);
		delete (lObject);
		mObjectTable.erase(x);
		mAutoDisabledObjectSet.erase(lObject);
	}
	else
	{
		mLog.AError("DeleteBody() - Can't find body to delete!");
		assert(false);
	}
}

Vector3DF PhysicsManagerODE::GetBodyPosition(BodyID pBodyId) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyPosition() - Body %i is not part of this world!"), pBodyId);
		return (Vector3DF());
	}

	const dReal* lPosition = dGeomGetPosition(lObject->mGeomID);
	return (Vector3DF(lPosition[0], lPosition[1], lPosition[2]));
}

QuaternionF PhysicsManagerODE::GetBodyOrientation(BodyID pBodyId) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyOrientation() - Body %i is not part of this world!"), pBodyId);
		return (QuaternionF());
	}

	dQuaternion q;
	::dGeomGetQuaternion(lObject->mGeomID, q);
	QuaternionF lQuat(q[0], q[1], q[2], q[3]);
	AdjustOrientation(lObject->mGeomID, lQuat, false);

	return (lQuat);
}

void PhysicsManagerODE::GetBodyTransform(BodyID pBodyId, TransformationF& pTransform) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyTransform() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	const dReal* p = dGeomGetPosition(lObject->mGeomID);
	dQuaternion q;
	dGeomGetQuaternion(lObject->mGeomID, q);

	const Vector3DF lPos(p[0], p[1], p[2]);
	pTransform.SetPosition(lPos);
	QuaternionF lQuat(q[0], q[1], q[2], q[3]);
	AdjustOrientation(lObject->mGeomID, lQuat, false);
	pTransform.SetOrientation(lQuat);
}

void PhysicsManagerODE::SetBodyTransform(BodyID pBodyId, const TransformationF& pTransform)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyTransform() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	TransformationF lTransform(pTransform);
	const Vector3DF lPos(pTransform.GetPosition());
	QuaternionF lQuat = lTransform.GetOrientation();
	AdjustOrientation(lObject->mGeomID, lQuat, true);
	dReal lQ[4];
	lQ[0] = lQuat.GetA();
	lQ[1] = lQuat.GetB();
	lQ[2] = lQuat.GetC();
	lQ[3] = lQuat.GetD();
	if(lObject->mBodyID)
	{
		::dBodySetPosition(lObject->mBodyID, lPos.x, lPos.y, lPos.z);
		::dBodySetQuaternion(lObject->mBodyID, lQ);
		::dBodyEnable(lObject->mBodyID);
	}
	else
	{
		::dGeomSetPosition(lObject->mGeomID, lPos.x, lPos.y, lPos.z);
		::dGeomSetQuaternion(lObject->mGeomID, lQ);
	}
}

void PhysicsManagerODE::GetBodyVelocity(BodyID pBodyId, Vector3DF& pVelocity) const
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

void PhysicsManagerODE::SetBodyVelocity(BodyID pBodyId, const Vector3DF& pVelocity)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetLinearVel(lObject->mBodyID, pVelocity.x, pVelocity.y, pVelocity.z);
	}
}

void PhysicsManagerODE::GetBodyAcceleration(BodyID pBodyId, Vector3DF& pAcceleration) const
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

void PhysicsManagerODE::SetBodyAcceleration(BodyID pBodyId, const Vector3DF& pAcceleration)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
		dBodySetForce(lObject->mBodyID, pAcceleration.x, pAcceleration.y, pAcceleration.z);
}

void PhysicsManagerODE::GetBodyAngularVelocity(BodyID pBodyId, Vector3DF& pAngularVelocity) const
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		const dReal* lAngularVelocity = ::dBodyGetAngularVel(lObject->mBodyID);
		pAngularVelocity.x = lAngularVelocity[0];
		pAngularVelocity.y = lAngularVelocity[1];
		pAngularVelocity.z = lAngularVelocity[2];
	}
	else
	{
		pAngularVelocity.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyAngularVelocity(BodyID pBodyId, const Vector3DF& pAngularVelocity)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetAngularVel(lObject->mBodyID, pAngularVelocity.x, pAngularVelocity.y, pAngularVelocity.z);
	}
}

void PhysicsManagerODE::GetBodyAngularAcceleration(BodyID pBodyId, Vector3DF& pAngularAcceleration) const
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

void PhysicsManagerODE::SetBodyAngularAcceleration(BodyID pBodyId, const Vector3DF& pAngularAcceleration)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetTorque(lObject->mBodyID, pAngularAcceleration.x, pAngularAcceleration.y, pAngularAcceleration.z);
	}
}

float PhysicsManagerODE::GetBodyMass(BodyID pBodyId)
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyMass() - Body %i is not part of this world!"), pBodyId);
		return (0);
	}
	return (lObject->mMass);
}

void PhysicsManagerODE::MassAdjustBody(BodyID pBodyId)
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("MassAdjustBody() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyID lBody = lObject->mBodyID;
	if (!lBody)
	{
		//mLog.Warningf(_T("MassAdjustBody() - static geometry %i does not have a body!"), pBodyId);
		return;
	}
	// Move geometries and body to center of mass.
	dMass m;
	::dBodyGetMass(lBody, &m);
	if (m.c[0] != 0 || m.c[1] != 0 || m.c[2] != 0)
	{
		for (dGeomID lGeometry = lBody->geom; lGeometry; lGeometry = lGeometry->body_next)
		{
			const dReal* g = ::dGeomGetOffsetPosition(lGeometry);
			::dGeomSetOffsetPosition(lGeometry, g[0]-m.c[0], g[1]-m.c[1], g[2]-m.c[2]);
		}
		const dReal* p = ::dBodyGetPosition(lBody);
		::dBodySetPosition(lBody, p[0]-m.c[0], p[1]-m.c[1], p[2]-m.c[2]);
		::dMassTranslate(&m, -m.c[0], -m.c[1], -m.c[2]);
		::dBodySetMass(lBody, &m);
	}
}

void PhysicsManagerODE::SetBodyData(BodyID pBodyId, void* pUserData)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyData() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	lObject->mUserData = pUserData;
}

void* PhysicsManagerODE::GetBodyData(BodyID pBodyId)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyData() - Body %i is not part of this world!"), pBodyId);
		return 0;
	}

	return lObject->mUserData;
}

PhysicsManager::TriggerID PhysicsManagerODE::CreateSphereTrigger(const TransformationF& pTransform,
	float32 pRadius, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, false);
	lObject->mGeomID = dCreateSphere(mSpaceID, (dReal)pRadius);

	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (TriggerID)lObject;
}

PhysicsManager::TriggerID PhysicsManagerODE::CreateCylinderTrigger(const TransformationF& pTransform,
	float32 pRadius, float32 pLength, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, false);

	// TODO: Create a real cylinder when ODE supports it.
	lObject->mGeomID = ::dCreateCylinder(mSpaceID, (dReal)pRadius, (dReal)pLength);
	mLog.AWarning("Warning! Cylinders are not accurately supported by ODE!");

	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (TriggerID)lObject;
}

PhysicsManager::TriggerID PhysicsManagerODE::CreateCapsuleTrigger(const TransformationF& pTransform,
	float32 pRadius, float32 pLength, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, false);

	lObject->mGeomID = ::dCreateCapsule(mSpaceID, (dReal)pRadius, (dReal)pLength);

	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (TriggerID)lObject;
}

PhysicsManager::TriggerID PhysicsManagerODE::CreateBoxTrigger(const TransformationF& pTransform,
	const Vector3D<float32>& pSize, TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, false);

	lObject->mGeomID = dCreateBox(mSpaceID, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);

	dGeomSetData(lObject->mGeomID, lObject);

	lObject->mTriggerListener = pForceListener;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (TriggerID)lObject;
}

PhysicsManager::TriggerID PhysicsManagerODE::CreateRayTrigger(const TransformationF& pTransform,
	const Vector3D<float32>& pFromPos, const Vector3D<float32>& pToPos,
	TriggerListener* pForceListener)
{
	Object* lObject = new Object(mWorldID, false);

	// Calculate the direction vector.
	Vector3D<float32> lDir(pToPos - pFromPos);
	float32 lLength = lDir.GetLength();
	lDir.Div(lLength);

	lObject->mGeomID = dCreateRay(mSpaceID, lLength);

	dGeomSetData(lObject->mGeomID, lObject);

	dGeomRaySet(lObject->mGeomID, pFromPos.x, pFromPos.y, pFromPos.z, lDir.x, lDir.y, lDir.z);

	lObject->mTriggerListener = pForceListener;

	mObjectTable.insert(lObject);

	SetBodyTransform((BodyID)lObject, pTransform);

	return (TriggerID)lObject;
}

void PhysicsManagerODE::DeleteTrigger(TriggerID pTriggerID)
{
	ObjectTable::iterator x = mObjectTable.find((Object*)pTriggerID);

	if (x != mObjectTable.end())
	{
		dGeomDestroy((*x)->mGeomID);
		delete *x;
		mObjectTable.erase(x);
	}
	else
	{
		mLog.AError("DeleteBody() - Can't find body to delete!");
	}
}

PhysicsManager::ForceFeedbackListener* PhysicsManagerODE::GetForceFeedbackListener(BodyID pBody)
{
	Object* lObject = (Object*)pBody;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetForceFeedbackListener() - Body %i is not part of this world!"), pBody);
		return (0);
	}

	return (lObject->mForceFeedbackListener);
}

void PhysicsManagerODE::GetTriggerTransform(TriggerID pTriggerID, TransformationF& pTransform)
{
	GetBodyTransform((BodyID)pTriggerID, pTransform);
}

void PhysicsManagerODE::SetTriggerTransform(TriggerID pTriggerID, const TransformationF& pTransform)
{
	SetBodyTransform((BodyID)pTriggerID, pTransform);
}

PhysicsManager::JointID PhysicsManagerODE::CreateBallJoint(BodyID pBody1, BodyID pBody2, const Vector3D<float32>& pAnchorPos)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateBallJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateBall(mWorldID, 0);
	lJointInfo->mType = JOINT_BALL;
	lJointInfo->mListIter = mFeedbackJointList.end();
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

PhysicsManager::JointID PhysicsManagerODE::CreateHingeJoint(BodyID pBody1, BodyID pBody2, 
	const Vector3D<float32>& pAnchorPos, const Vector3D<float32>& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateHingeJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateHinge(mWorldID, 0);
	lJointInfo->mType = JOINT_HINGE;
	lJointInfo->mListIter = mFeedbackJointList.end();
	lJointInfo->mListener1 = lObject1->mForceFeedbackListener;
	lJointInfo->mListener2 = lObject2->mForceFeedbackListener;

	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mGeomID->body, lObject2->mGeomID->body);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mGeomID->body, 0);
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

PhysicsManager::JointID PhysicsManagerODE::CreateHinge2Joint(BodyID pBody1, BodyID pBody2,
	const Vector3D<float32>& pAnchorPos, const Vector3D<float32>& pAxis1,
	const Vector3D<float32>& pAxis2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies2(pBody1, pBody2, lObject1, lObject2, _T("CreateHinge2Joint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateHinge2(mWorldID, 0);
	lJointInfo->mType = JOINT_HINGE2;
	lJointInfo->mListIter = mFeedbackJointList.end();
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

PhysicsManager::JointID PhysicsManagerODE::CreateUniversalJoint(BodyID pBody1, BodyID pBody2,
	const Vector3D<float32>& pAnchorPos, const Vector3D<float32>& pAxis1,
	const Vector3D<float32>& pAxis2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateUniversalJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateUniversal(mWorldID, 0);
	lJointInfo->mType = JOINT_UNIVERSAL;
	lJointInfo->mListIter = mFeedbackJointList.end();
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

PhysicsManager::JointID PhysicsManagerODE::CreateSliderJoint(BodyID pBody1, BodyID pBody2,
	const Vector3D<float32>& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateSliderJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateSlider(mWorldID, 0);
	lJointInfo->mType = JOINT_SLIDER;
	lJointInfo->mListIter = mFeedbackJointList.end();
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

PhysicsManager::JointID PhysicsManagerODE::CreateFixedJoint(BodyID pBody1, BodyID pBody2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateFixedJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateFixed(mWorldID, 0);
	lJointInfo->mType = JOINT_FIXED;
	lJointInfo->mListIter = mFeedbackJointList.end();
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

PhysicsManager::JointID PhysicsManagerODE::CreateAngularMotorJoint(BodyID pBody1, BodyID pBody2,
	const Vector3D<float32>& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateAngularMotorJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateAMotor(mWorldID, 0);
	lJointInfo->mType = JOINT_ANGULARMOTOR;
	lJointInfo->mListIter = mFeedbackJointList.end();
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

void PhysicsManagerODE::DeleteJoint(JointID pJointId)
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

bool PhysicsManagerODE::StabilizeJoint(JointID pJointId)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = *x;
	switch (lJointInfo->mType)
	{
		case JOINT_BALL:
		{
			dVector3 lAnchor;
			::dJointGetBallAnchor(lJointInfo->mJointID, lAnchor);
			dVector3 lAnchor2;
			::dJointGetBallAnchor2(lJointInfo->mJointID, lAnchor2);
			dxBody* lChildBody = lJointInfo->mJointID->node[1].body;
			const dReal* lPos = ::dBodyGetPosition(lChildBody);
			lAnchor[0] = lPos[0] + lAnchor[0] - lAnchor2[0];
			lAnchor[1] = lPos[1] + lAnchor[1] - lAnchor2[1];
			lAnchor[2] = lPos[2] + lAnchor[2] - lAnchor2[2];
			::dBodySetPosition(lChildBody, lAnchor[0], lAnchor[1], lAnchor[2]);
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

void PhysicsManagerODE::SetIsGyroscope(BodyID pBodyId, bool pIsGyro)
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mBodyID)
	{
		::dBodySetGyroscopicMode(lObject->mBodyID, pIsGyro);
	}
}

bool PhysicsManagerODE::GetJoint1Diff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	const JointInfo* lJointInfo = *x;
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

bool PhysicsManagerODE::SetJoint1Diff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff)
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*x;
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

bool PhysicsManagerODE::GetJoint2Diff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*x;
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

bool PhysicsManagerODE::SetJoint2Diff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff)
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*x;
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

bool PhysicsManagerODE::GetJoint3Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*x;
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

bool PhysicsManagerODE::SetJoint3Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("Couldn't find joint %i!"), pJointId);
		return (false);
	}
	bool lOk = false;
	JointInfo* lJointInfo = (JointInfo*)*x;
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

bool PhysicsManagerODE::GetHingeDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_HINGE);
	if (lJointInfo->mType != JOINT_HINGE)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	Vector3DF lAxis;
	if (!GetAxis1(pJointId, lAxis) || !GetAngle1(pJointId, pDiff.mAngle))
	{
		return (false);
	}

	{
		Vector3DF lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mAngleVelocity = lAxis * lVelocity;
	}

	{
		Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mAngleAcceleration = lAxis * lAcceleration;
	}

	return (true);
}

bool PhysicsManagerODE::SetHingeDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_HINGE);
	if (lJointInfo->mType != JOINT_HINGE)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	Vector3DF lAnchor;
	Vector3DF lAxis;
	if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis))
	{
		return (false);
	}
	assert(lAxis.GetLengthSquared() > 0.99f && lAxis.GetLengthSquared() < 1.01f);

	{
		// Fetch parent orientation.
		assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		const QuaternionF lParentQ(lPQ[0], lPQ[1], lPQ[2], lPQ[3]);
		// Rotate to original child (us) orientation.
		dxJointHinge* lHinge = (dxJointHinge*)lJointInfo->mJointID;
		QuaternionF lQ(lHinge->qrel[0], lHinge->qrel[1], lHinge->qrel[2], lHinge->qrel[3]);
		lQ = lParentQ * lQ;
		// Rotate to input angle.
		lQ = QuaternionF(-pDiff.mAngle, lAxis) * lQ;
		// Set orientation.
		TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		lTransform.SetOrientation(lQ);
		// Align anchors (happen after rotation) and store.
		Vector3DF lAnchor2(lHinge->anchor2[0], lHinge->anchor2[1], lHinge->anchor2[2]);
		lAnchor2 = lQ*lAnchor2 + lTransform.GetPosition();
		lTransform.GetPosition() += lAnchor-lAnchor2;
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		Vector3DF lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (pDiff.mAngleVelocity < PIF*1000)
		{
			Vector3DF lAxisVelocity = lAxis*(lAxis*lVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis * pDiff.mAngleVelocity;
			SetBodyAngularVelocity(pBodyId, lVelocity);
		}
	}

	{
		Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		// Drop angular acceleration along axis, then add the specified amount.
		if (pDiff.mAngleAcceleration < PIF*1000)
		{
			Vector3DF lAxisAcceleration = lAxis*(lAxis*lAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis * pDiff.mAngleAcceleration;
			SetBodyAngularAcceleration(pBodyId, lAcceleration);
		}
	}

	return (true);
}

bool PhysicsManagerODE::GetUniversalDiff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const
{
	LEPRA_DEBUG_CODE(JointInfo* lJointInfo = (JointInfo*)pJointId;)
	assert(lJointInfo->mType == JOINT_UNIVERSAL);

	Vector3DF lAxis1;
	Vector3DF lAxis2;
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
		Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = -(lAxis2 * lAcceleration);
		pDiff.mAngleAcceleration = -(lAxis1 * lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::SetUniversalDiff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_UNIVERSAL);

	Vector3DF lAxis1;
	Vector3DF lAxis2;
	Vector3DF lAnchor;
	if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1))
	{
		return (false);
	}
	assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);

	{
		// Fetch parent orientation.
		assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		const QuaternionF lParentQ(lPQ[0], lPQ[1], lPQ[2], lPQ[3]);
		// Rotate to cross piece orientation.
		dxJointUniversal* lUniversal = (dxJointUniversal*)lJointInfo->mJointID;
		QuaternionF lRelativeQ(lUniversal->qrel1[0], lUniversal->qrel1[1], lUniversal->qrel1[2], lUniversal->qrel1[3]);
		QuaternionF lQ = lParentQ * lRelativeQ;
		// Rotate to body 1's input angle.
		lQ = QuaternionF(-pDiff.mValue, lAxis1) * lQ;
		// Apply rotation from cross piece to original child (us).
		lRelativeQ.Set(lUniversal->qrel2[0], lUniversal->qrel2[1], lUniversal->qrel2[2], lUniversal->qrel2[3]);
		//lQ = lQ * lRelativeQ;
		// Rotating around body 1's axis changes body 2's axis. Fetch and act on it AFTER rotation 'round axis1.
		lAxis2 = lQ * Vector3DF(lUniversal->axis2[0], lUniversal->axis2[1], lUniversal->axis2[2]);
		lQ = QuaternionF(-pDiff.mAngle, lAxis2) * lQ;
		// Set orientation.
		TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		lTransform.SetOrientation(lQ);
		// Align anchors (happen after rotation) and store.
		Vector3DF lAnchor2(lUniversal->anchor2[0], lUniversal->anchor2[1], lUniversal->anchor2[2]);
		lAnchor2 = lQ*lAnchor2 + lTransform.GetPosition();
		lTransform.GetPosition() += lAnchor-lAnchor2;
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		Vector3DF lAxisVelocity;
		Vector3DF lOriginalVelocity;
		GetBodyAngularVelocity(pBodyId, lOriginalVelocity);
		Vector3DF lVelocity = lOriginalVelocity;
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (pDiff.mValueVelocity < PIF*1000)
		{
			lAxisVelocity = lAxis1*(lAxis1*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis1 * -pDiff.mValueVelocity;
		}
		if (pDiff.mAngleVelocity < PIF*1000)
		{
			lAxisVelocity = lAxis2*(lAxis2*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis2 * -pDiff.mAngleVelocity;
		}
		//SetBodyVelocity(pBodyId, Vector3DF());
		SetBodyAngularVelocity(pBodyId, lVelocity);
	}

	{
		Vector3DF lAxisAcceleration;
		Vector3DF lOriginalAcceleration;
		GetBodyAngularAcceleration(pBodyId, lOriginalAcceleration);
		Vector3DF lAcceleration = lOriginalAcceleration;
		// Drop angular acceleration along axis 1 & 2, then add the specified amount.
		if (pDiff.mValueAcceleration < PIF*1000)
		{
			lAxisAcceleration = lAxis1*(lAxis1*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis1 * -pDiff.mValueAcceleration;
		}
		if (pDiff.mAngleAcceleration < PIF*1000)
		{
			lAxisAcceleration = lAxis2*(lAxis2*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis2 * -pDiff.mAngleAcceleration;
		}
		//SetBodyAcceleration(pBodyId, Vector3DF());
		SetBodyAngularAcceleration(pBodyId, lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetHinge2Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	LEPRA_DEBUG_CODE(JointInfo* lJointInfo = (JointInfo*)pJointId;)
	assert(lJointInfo->mType == JOINT_HINGE2);

	Vector3DF lAxis1;
	Vector3DF lAxis2;
	{
		TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		Vector3DF lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2))
		{
			return (false);
		}
		assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		const Vector3DF lDiff(lTransform.GetPosition()-lAnchor);
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
		Vector3DF lVelocity;
		GetBodyVelocity(pBodyId, lVelocity);
		pDiff.mValueVelocity = -(lAxis1 * lVelocity);
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mAngle1Velocity = -(lAxis2 * lVelocity);
		pDiff.mAngle2Velocity = -(lAxis1 * lVelocity);
	}

	{
		Vector3DF lAcceleration;
		GetBodyAcceleration(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = -(lAxis1 * lAcceleration);
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mAngle1Acceleration = -(lAxis2 * lAcceleration);
		pDiff.mAngle2Acceleration = -(lAxis1 * lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::SetHinge2Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	LEPRA_DEBUG_CODE(JointInfo* lJointInfo = (JointInfo*)pJointId;)
	assert(lJointInfo->mType == JOINT_HINGE2);

	Vector3DF lAxis1;
	Vector3DF lAxis2;
	{
		Vector3DF lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2))
		{
			return (false);
		}
		assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		const Vector3DF lDiff = lAxis1 * -pDiff.mValue;
		TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		lTransform.SetPosition(lAnchor+lDiff);
		float lCurrentAngle;
		if (pDiff.mAngle1 < PIF*10)
		{
			if (!GetAngle1(pJointId, lCurrentAngle))	// JB: not available in ODE: || !GetAngle2(pJointId, lCurrentAngle2));
			{
				return (false);
			}
			lTransform.GetOrientation().RotateAroundVector(lAxis1, lCurrentAngle-pDiff.mAngle1);
		}
		if (pDiff.mAngle2 < PIF*10)
		{
			//pDiff.mAngle2;	// JB-TODO: use this angle as well.
		}
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		Vector3DF lVelocity;
		GetBodyVelocity(pBodyId, lVelocity);
		// Drop suspension velocity along axis1.
		Vector3DF lAxisVelocity(lAxis1*(lAxis1*lVelocity));
		lVelocity -= lAxisVelocity;
		// Add suspension velocity.
		lVelocity += lAxis1 * -pDiff.mValueVelocity;
		SetBodyVelocity(pBodyId, lVelocity);

		Vector3DF lOriginalVelocity;
		GetBodyAngularVelocity(pBodyId, lOriginalVelocity);
		lVelocity = lOriginalVelocity;
		// Drop angular velocity along axis 1 & 2, then add the specified amount.
		if (pDiff.mAngle1Velocity < PIF*1000)
		{
			lAxisVelocity = lAxis1*(lAxis1*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis2 * -pDiff.mAngle1Velocity;
		}
		if (pDiff.mAngle2Velocity < PIF*1000)
		{
			lAxisVelocity = lAxis2*(lAxis2*lOriginalVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis1 * -pDiff.mAngle2Velocity;
		}
		SetBodyAngularVelocity(pBodyId, lVelocity);
	}

	{
		Vector3DF lAcceleration;
		GetBodyAcceleration(pBodyId, lAcceleration);
		// Drop suspension acceleration along axis1.
		Vector3DF lAxisAcceleration(lAxis1*(lAxis1*lAcceleration));
		lAcceleration -= lAxisAcceleration;
		// Add suspension acceleration.
		lAcceleration += lAxis1 * -pDiff.mValueAcceleration;
		SetBodyAcceleration(pBodyId, lAcceleration);

		Vector3DF lOriginalAcceleration;
		GetBodyAngularAcceleration(pBodyId, lOriginalAcceleration);
		lAcceleration = lOriginalAcceleration;
		// Drop angular acceleration along axis 1 & 2, then add the specified amount.
		if (pDiff.mAngle1Acceleration < PIF*1000)
		{
			lAxisAcceleration = lAxis1*(lAxis1*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis2 * -pDiff.mAngle1Acceleration;
		}
		if (pDiff.mAngle2Acceleration < PIF*1000)
		{
			lAxisAcceleration = lAxis2*(lAxis2*lOriginalAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis1 * -pDiff.mAngle2Acceleration;
		}
		SetBodyAngularAcceleration(pBodyId, lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetBallDiff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_BALL);

	{
		assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		QuaternionF lParentQ(lPQ[0], lPQ[1], lPQ[2], lPQ[3]);

		TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		const QuaternionF lQ = lTransform.GetOrientation()/lParentQ;
		lQ.GetEulerAngles(pDiff.mValue, pDiff.mAngle1, pDiff.mAngle2);
	}

	{
		Vector3DF lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mValueVelocity = lVelocity.x;
		pDiff.mAngle1Velocity = lVelocity.y;
		pDiff.mAngle2Velocity = lVelocity.z;
	}

	{
		Vector3DF lAcceleration;
		GetBodyAngularAcceleration(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = lAcceleration.x;
		pDiff.mAngle1Acceleration = lAcceleration.y;
		pDiff.mAngle2Acceleration = lAcceleration.z;
	}

	return (true);
}

bool PhysicsManagerODE::SetBallDiff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	assert(lJointInfo->mType == JOINT_BALL);

	{
		assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		QuaternionF lParentQ(lPQ[0], lPQ[1], lPQ[2], lPQ[3]);

		TransformationF lTransform;
		GetBodyTransform(pBodyId, lTransform);
		QuaternionF lRelativeToParentQ(lParentQ/lTransform.GetOrientation());
		dVector3 lRawAnchor;
		::dJointGetBallAnchor2(lJointInfo->mJointID, lRawAnchor);
		Vector3DF lAnchor2(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
		Vector3DF lPosition = lTransform.GetPosition()-lAnchor2;
		lPosition = lRelativeToParentQ*lPosition;	// Go to parent space.
		QuaternionF lRelativeFromParentQ;
		lRelativeFromParentQ.SetEulerAngles(pDiff.mValue, pDiff.mAngle1, pDiff.mAngle2);
		lPosition = lRelativeFromParentQ*lPosition;	// Go from parent to given child space.
		lTransform.SetOrientation(lRelativeFromParentQ*lParentQ);	// Set complete orientation.
		Vector3DF lAnchor;
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
		//SetBodyVelocity(pBodyId, Vector3DF(0, 0, 0));
		//SetBodyVelocity(pBodyId, Vector3DF(pDiff.mValueVelocity, pDiff.mAngle1Velocity, pDiff.mAngle2Velocity));
		SetBodyAngularVelocity(pBodyId, Vector3DF(pDiff.mValueVelocity, pDiff.mAngle1Velocity, pDiff.mAngle2Velocity));
		//SetBodyAngularVelocity(pBodyId, Vector3DF(0, 0, 0));
	}

	{
		// TODO: adjust linear acceleration.
		//SetBodyAcceleration(pBodyId, Vector3DF(0, 0, 0));
		//SetBodyAcceleration(pBodyId, Vector3DF(pDiff.mValueAcceleration, pDiff.mAngle1Acceleration, pDiff.mAngle2Acceleration));
		SetBodyAngularAcceleration(pBodyId, Vector3DF(pDiff.mValueAcceleration, pDiff.mAngle1Acceleration, pDiff.mAngle2Acceleration));
		//SetBodyAngularAcceleration(pBodyId, Vector3DF(0, 0, 0));
	}

	return (true);
}

bool PhysicsManagerODE::CheckBodies(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const tchar* pFunction)
{
	if (pBody1 == 0 && pBody2 == 0)
	{
		str lMsg(pFunction);
		lMsg += _T(" - body1 = body2 = NULL!");
		mLog.Error(lMsg);
		return (false);
	}

	if (pBody1 == pBody2)
	{
		str lMsg(pFunction);
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
			str lMsg(pFunction);
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
			str lMsg(pFunction);
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
			str lMsg(pFunction);
			lMsg += _T(" - Body2 is not part of this world!");
			mLog.Error(lMsg);
			return (false);
		}

		return (true);
	}
}

bool PhysicsManagerODE::CheckBodies2(BodyID& pBody1, BodyID& pBody2, Object*& pObject1, Object*& pObject2, const tchar* pFunction)
{
	if (pBody1 == 0)
	{
		str lMsg(pFunction);
		lMsg += _T(" - body1 = NULL!");
		mLog.Error(lMsg);
		return (false);
	}

	if (pBody2 == 0)
	{
		str lMsg(pFunction);
		lMsg += _T(" - body2 = NULL!");
		mLog.Error(lMsg);
		return (false);
	}

	if (pBody1 == pBody2)
	{
		str lMsg(pFunction);
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
		str lMsg(pFunction);
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
		str lMsg(pFunction);
		lMsg += _T(" - Body2 is not part of this world!");
		mLog.Error(lMsg);
		return (false);
	}

	return (true);
}

bool PhysicsManagerODE::GetAnchorPos(JointID pJointId, Vector3D<float32>& pAnchorPos) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAnchorPos() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	dVector3 lPos;

	switch ((*x)->mType)
	{
	case JOINT_BALL:
		dJointGetBallAnchor((*x)->mJointID, lPos);
		break;
	case JOINT_HINGE:
		dJointGetHingeAnchor((*x)->mJointID, lPos);
		break;
	case JOINT_HINGE2:
		dJointGetHinge2Anchor((*x)->mJointID, lPos);
		break;
	case JOINT_UNIVERSAL:
		dJointGetUniversalAnchor((*x)->mJointID, lPos);
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

bool PhysicsManagerODE::GetAxis1(JointID pJointId, Vector3D<float32>& pAxis1) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAxis1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	dVector3 lAxis;

	switch ((*x)->mType)
	{
	case JOINT_HINGE:
		dJointGetHingeAxis((*x)->mJointID, lAxis);
		break;
	case JOINT_HINGE2:
		dJointGetHinge2Axis1((*x)->mJointID, lAxis);
		break;
	case JOINT_UNIVERSAL:
		dJointGetUniversalAxis1((*x)->mJointID, lAxis);
		break;
	case JOINT_ANGULARMOTOR:
		dJointGetAMotorAxis((*x)->mJointID, 0, lAxis);
		break;
	case JOINT_SLIDER:
		dJointGetSliderAxis((*x)->mJointID, lAxis);
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

bool PhysicsManagerODE::GetAxis2(JointID pJointId, Vector3D<float32>& pAxis2) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAxis2() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	dVector3 lAxis;

	switch ((*x)->mType)
	{
	case JOINT_HINGE2:
		dJointGetHinge2Axis2((*x)->mJointID, lAxis);
		break;
	case JOINT_UNIVERSAL:
		dJointGetUniversalAxis2((*x)->mJointID, lAxis);
		break;
	case JOINT_BALL:
	case JOINT_HINGE:
	case JOINT_SLIDER:
	case JOINT_FIXED:
	case JOINT_ANGULARMOTOR:
		//mLog.AError("GetAxis2() - Joint doesn't have two axes!");
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

bool PhysicsManagerODE::GetAngle1(JointID pJointId, float32& pAngle) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngle1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_HINGE2:
		pAngle = dJointGetHinge2Angle1((*x)->mJointID);
		break;
	case JOINT_UNIVERSAL:
		pAngle = dJointGetUniversalAngle1((*x)->mJointID);
		break;
	case JOINT_HINGE:
		pAngle = dJointGetHingeAngle((*x)->mJointID);
		break;
	case JOINT_ANGULARMOTOR:
		pAngle = dJointGetAMotorAngle((*x)->mJointID, 0);
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

bool PhysicsManagerODE::GetAngle2(JointID pJointId, float32& pAngle) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngle2() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_UNIVERSAL:
		pAngle = dJointGetUniversalAngle2((*x)->mJointID);
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

bool PhysicsManagerODE::GetAngleRate1(JointID pJointId, float32& pAngleRate) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngleRate1() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_HINGE2:
		pAngleRate = dJointGetHinge2Angle1Rate((*x)->mJointID);
		break;
	case JOINT_UNIVERSAL:
		pAngleRate = dJointGetUniversalAngle1Rate((*x)->mJointID);
		break;
	case JOINT_HINGE:
		pAngleRate = dJointGetHingeAngleRate((*x)->mJointID);
		break;
	case JOINT_ANGULARMOTOR:
		pAngleRate = dJointGetAMotorAngleRate((*x)->mJointID, 0);
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

bool PhysicsManagerODE::GetAngleRate2(JointID pJointId, float32& pAngleRate) const
{	
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngleRate2() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_HINGE2:
		pAngleRate = dJointGetHinge2Angle2Rate((*x)->mJointID);
		break;
	case JOINT_UNIVERSAL:
		pAngleRate = dJointGetUniversalAngle2Rate((*x)->mJointID);
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

bool PhysicsManagerODE::SetAngle1(BodyID pBodyId, JointID pJointId, float32 pAngle)
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
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
			TransformationF lTransform;
			GetBodyTransform(pBodyId, lTransform);
			Vector3DF lAxis;
			if (GetAxis1(pJointId, lAxis))
			{
				dVector3 lRawAnchor;
				::dJointGetHingeAnchor((*x)->mJointID, lRawAnchor);
				Vector3DF lAnchor(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
				::dJointGetHingeAnchor2((*x)->mJointID, lRawAnchor);
				Vector3DF lAnchor2(lRawAnchor[0], lRawAnchor[1], lRawAnchor[2]);
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
			TransformationF lTransform;
			GetBodyTransform(pBodyId, lTransform);
			Vector3DF lAxis1;
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

bool PhysicsManagerODE::SetAngularMotorAngle(JointID pJointId, float32 pAngle)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorAngle() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("SetAngularMotorAngle() - Joint is not an angular motor!");
		return (false);
	}

	dJointSetAMotorAngle((*x)->mJointID, 0, pAngle);
	return (true);
}

bool PhysicsManagerODE::SetAngularMotorSpeed(JointID pJointId, float32 pSpeed)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorSpeed() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("SetAngularMotorSpeed() - Joint is not an angular motor!");
		return (false);
	}

	dJointSetAMotorParam((*x)->mJointID, dParamVel, pSpeed);
	return (true);
}

bool PhysicsManagerODE::SetAngularMotorMaxForce(JointID pJointId, float32 pMaxForce)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorMaxForce() - Couldn't find joint %i!"), pJointId);
		return (false);
	}


	JointInfo* lJoint = *x;
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
	else if (lJoint->mType == JOINT_HINGE)
	{
		dJointSetHingeParam(lJoint->mJointID, dParamFMax, pMaxForce);
		dJointSetHingeParam(lJoint->mJointID, dParamFMax2, pMaxForce);
		return (true);
	}
	mLog.AError("SetAngularMotorMaxForce() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::SetAngularMotorRoll(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorRoll() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *x;
	if (lJoint->mType == JOINT_HINGE2)
	{
		::dJointSetHinge2Param(lJoint->mJointID, dParamFMax2, pMaxForce);
		::dJointSetHinge2Param(lJoint->mJointID, dParamVel2, pTargetVelocity);
		::dBodyEnable(::dJointGetBody(lJoint->mJointID, 1));
		return (true);
	}
	else if (lJoint->mType == JOINT_HINGE)
	{
		::dJointSetHingeParam(lJoint->mJointID, dParamFMax, pMaxForce);
		::dJointSetHingeParam(lJoint->mJointID, dParamVel, pTargetVelocity);
		::dBodyEnable(::dJointGetBody(lJoint->mJointID, 1));
		return (true);
	}
	else if (lJoint->mType == JOINT_UNIVERSAL)
	{
		::dJointSetUniversalParam(lJoint->mJointID, dParamFMax, pMaxForce);
		::dJointSetUniversalParam(lJoint->mJointID, dParamVel, pTargetVelocity);
		::dJointSetUniversalParam(lJoint->mJointID, dParamFMax2, pMaxForce);
		::dJointSetUniversalParam(lJoint->mJointID, dParamVel2, pTargetVelocity);
		::dBodyEnable(::dJointGetBody(lJoint->mJointID, 1));
		return (true);
	}
	mLog.AError("SetAngularMotorRoll() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::GetAngularMotorRoll(JointID pJointId, float32& pMaxForce, float32& pTargetVelocity)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorRoll() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *x;
	if (lJoint->mType == JOINT_HINGE2)
	{
		pMaxForce = ::dJointGetHinge2Param(lJoint->mJointID, dParamFMax2);
		pTargetVelocity = ::dJointGetHinge2Param(lJoint->mJointID, dParamVel2);
		return (true);
	}
	else if (lJoint->mType == JOINT_HINGE)
	{
		pMaxForce = ::dJointGetHingeParam(lJoint->mJointID, dParamFMax);
		pTargetVelocity = ::dJointGetHingeParam(lJoint->mJointID, dParamVel);
		return (true);
	}
	mLog.AError("GetAngularMotorRoll() - Joint is not an angular motor!");
	return (false);
}

bool PhysicsManagerODE::SetAngularMotorTurn(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetAngularMotorTurn() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *x;
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

bool PhysicsManagerODE::GetAngularMotorAngle(JointID pJointId, float32& pAngle) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorAngle() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("GetAngularMotorAngle() - Joint is not an angular motor!");
		return (false);
	}

	pAngle = dJointGetAMotorAngle((*x)->mJointID, 0);
	return (true);
}

bool PhysicsManagerODE::GetAngularMotorSpeed(JointID pJointId, float32& pSpeed) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorSpeed() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("GetAngularMotorSpeed() - Joint is not an angular motor!");
		return (false);
	}

	pSpeed = dJointGetAMotorParam((*x)->mJointID, dParamVel);
	return (true);
}

bool PhysicsManagerODE::GetAngularMotorMaxForce(JointID pJointId, float32& pMaxForce) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetAngularMotorMaxForce() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_ANGULARMOTOR)
	{
		mLog.AError("GetAngularMotorMaxForce() - Joint is not an angular motor!");
		return (false);
	}

	pMaxForce = dJointGetAMotorParam((*x)->mJointID, dParamFMax);
	return (true);
}

bool PhysicsManagerODE::SetJointParams(JointID pJointId, float32 pLowStop, float32 pHighStop, float32 pBounce)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetJointParams() - Couldn't find joint!"), pJointId);
		return (false);
	}

	dJointID lJointId = (*x)->mJointID;
	switch ((*x)->mType)
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
			dJointSetUniversalParam(lJointId, dParamFudgeFactor, pBounce);
			dJointSetUniversalParam(lJointId, dParamCFM, 0);
			dJointSetUniversalParam(lJointId, dParamLoStop2, pLowStop);
			dJointSetUniversalParam(lJointId, dParamHiStop2, pHighStop);
			dJointSetUniversalParam(lJointId, dParamBounce2, pBounce);
			dJointSetUniversalParam(lJointId, dParamFudgeFactor2, pBounce);
			dJointSetUniversalParam(lJointId, dParamCFM2, 0);
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

bool PhysicsManagerODE::GetJointParams(JointID pJointId, float32& pLowStop, float32& pHighStop, float32& pBounce) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetJointParams() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_SLIDER:
		pLowStop  = dJointGetSliderParam((*x)->mJointID, dParamLoStop);
		pHighStop = dJointGetSliderParam((*x)->mJointID, dParamHiStop);
		pBounce   = dJointGetSliderParam((*x)->mJointID, dParamBounce);
		break;
	case JOINT_HINGE:
		pLowStop  = dJointGetHingeParam((*x)->mJointID, dParamLoStop);
		pHighStop = dJointGetHingeParam((*x)->mJointID, dParamHiStop);
		pBounce   = dJointGetHingeParam((*x)->mJointID, dParamBounce);
		break;
	case JOINT_HINGE2:
		pLowStop  = dJointGetHinge2Param((*x)->mJointID, dParamLoStop);
		pHighStop = dJointGetHinge2Param((*x)->mJointID, dParamHiStop);
		pBounce   = dJointGetHinge2Param((*x)->mJointID, dParamBounce);
		break;
	case JOINT_UNIVERSAL:
		pLowStop  = dJointGetUniversalParam((*x)->mJointID, dParamLoStop);
		pHighStop = dJointGetUniversalParam((*x)->mJointID, dParamHiStop);
		pBounce   = dJointGetUniversalParam((*x)->mJointID, dParamBounce);
		break;
	case JOINT_ANGULARMOTOR:
		pLowStop  = dJointGetAMotorParam((*x)->mJointID, dParamLoStop);
		pHighStop = dJointGetAMotorParam((*x)->mJointID, dParamHiStop);
		pBounce   = dJointGetAMotorParam((*x)->mJointID, dParamBounce);
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

bool PhysicsManagerODE::SetSuspension(JointID pJointId, float32 pFrameTime, float32 pSpringConstant, float32 pDampingConstant)
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetJointParams() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *x;
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

bool PhysicsManagerODE::GetSuspension(JointID pJointId, float32& pErp, float32& pCfm) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetJointParams() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *x;
	if (lJoint->mType == JOINT_HINGE2)
	{
		pErp = ::dJointGetHinge2Param(lJoint->mJointID, dParamSuspensionERP);
		pCfm = ::dJointGetHinge2Param(lJoint->mJointID, dParamSuspensionCFM);
		return (true);
	}
	mLog.AError("SetSuspension() - Joint is not a hinge-2!");
	return (false);
}

bool PhysicsManagerODE::GetSliderPos(JointID pJointId, float32& pPos) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetSliderPos() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_SLIDER)
	{
		mLog.AError("GetSliderPos() - Joint is not a slider!");
		return (false);
	}

	pPos = dJointGetSliderPosition((*x)->mJointID);
	return (true);
}

bool PhysicsManagerODE::GetSliderSpeed(JointID pJointId, float32& pSpeed) const
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("GetSliderSleep() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_SLIDER)
	{
		mLog.AError("GetSliderSleep() - Joint is not a slider!");
		return (false);
	}

	pSpeed = dJointGetSliderPositionRate((*x)->mJointID);

	return (true);
}

bool PhysicsManagerODE::AddJointForce(JointID pJointId, float32 pForce)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("AddJointForce() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_SLIDER)
	{
		mLog.AError("AddJointForce() - Joint is not a slider!");
		return (false);
	}

	dJointAddSliderForce((*x)->mJointID, pForce);

	return (true);
}

bool PhysicsManagerODE::AddJointTorque(JointID pJointId, float32 pTorque)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("AddJointTorque() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_HINGE:
		dJointAddHingeTorque((*x)->mJointID, pTorque);
		break;
	case JOINT_HINGE2:
		dJointAddHinge2Torques((*x)->mJointID, pTorque, 0);
		break;
	case JOINT_UNIVERSAL:
		dJointAddUniversalTorques((*x)->mJointID, pTorque, 0);
		break;
	case JOINT_ANGULARMOTOR:
		dJointAddAMotorTorques((*x)->mJointID, pTorque, 0, 0);
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

bool PhysicsManagerODE::AddJointTorque(JointID pJointId, float32 pTorque1, float32 pTorque2)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("AddJointTorque() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	switch ((*x)->mType)
	{
	case JOINT_HINGE2:
		dJointAddHinge2Torques((*x)->mJointID, pTorque1, pTorque2);
		break;
	case JOINT_UNIVERSAL:
		dJointAddUniversalTorques((*x)->mJointID, pTorque1, pTorque2);
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

void PhysicsManagerODE::AddForce(BodyID pBodyId, const Vector3D<float32>& pForce)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForce() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForce(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z);
}

void PhysicsManagerODE::AddTorque(BodyID pBodyId, const Vector3D<float32>& pTorque)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddTorque() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddTorque(((Object*)pBodyId)->mBodyID, pTorque.x, pTorque.y, pTorque.z);
}

void PhysicsManagerODE::AddRelForce(BodyID pBodyId, const Vector3D<float32>& pForce)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForce() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForce(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z);
}

void PhysicsManagerODE::AddRelTorque(BodyID pBodyId, const Vector3D<float32>& pTorque)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelTorque() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelTorque(((Object*)pBodyId)->mBodyID, pTorque.x, pTorque.y, pTorque.z);
}

void PhysicsManagerODE::AddForceAtPos(BodyID pBodyId, const Vector3D<float32>& pForce,
									 const Vector3D<float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForceAtPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForceAtPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::AddForceAtRelPos(BodyID pBodyId, const Vector3D<float32>& pForce,
										const Vector3D<float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForceAtRelPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForceAtRelPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::AddRelForceAtPos(BodyID pBodyId, const Vector3D<float32>& pForce,
										const Vector3D<float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForceAtPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForceAtPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::AddRelForceAtRelPos(BodyID pBodyId, const Vector3D<float32>& pForce,
										   const Vector3D<float32>& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForceAtRelPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForceAtRelPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::RestrictBody(BodyID pBodyId, float32 pMaxSpeed, float32 pMaxAngularSpeed)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("RestrictBody() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	const dReal* lAVel = dBodyGetAngularVel(((Object*)pBodyId)->mBodyID);
	const dReal* lLVel = dBodyGetLinearVel(((Object*)pBodyId)->mBodyID);

	float32 lASpeed = lAVel[0] * lAVel[0] + lAVel[1] * lAVel[1] + lAVel[2] * lAVel[2];
	float32 lLSpeed = lLVel[0] * lLVel[0] + lLVel[1] * lLVel[1] + lLVel[2] * lLVel[2];

	if (lASpeed > pMaxAngularSpeed * pMaxAngularSpeed)
	{
		float32 k = pMaxAngularSpeed / sqrtf(lASpeed);
		dBodySetAngularVel(((Object*)pBodyId)->mBodyID, lAVel[0] * k, lAVel[1] * k, lAVel[2] * k);
	}

	if (lLSpeed > pMaxSpeed * pMaxSpeed)
	{
		float32 k = pMaxSpeed / sqrtf(lLSpeed);
		dBodySetLinearVel(((Object*)pBodyId)->mBodyID, lLVel[0] * k, lLVel[1] * k, lLVel[2] * k);
	}
}

void PhysicsManagerODE::ActivateGravity(BodyID pBodyId)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("ActivateGravity() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodySetGravityMode(((Object*)pBodyId)->mBodyID, 1);
}

void PhysicsManagerODE::DeactivateGravity(BodyID pBodyId)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("DeactivateGravity() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodySetGravityMode(((Object*)pBodyId)->mBodyID, 0);
}

void PhysicsManagerODE::SetGravity(const Vector3D<float32>& pGravity)
{
	dWorldSetGravity(mWorldID, pGravity.x, pGravity.y, pGravity.z);
}

Vector3DF PhysicsManagerODE::GetGravity() const
{
	dVector3 lGravity;
	::dWorldGetGravity(mWorldID, lGravity);
	return (Vector3DF(lGravity[0], lGravity[1], lGravity[2]));
}

void PhysicsManagerODE::StepAccurate(float32 pStepSize)
{
	if (pStepSize > 0)
	{
		FlagMovingObjects();

		dSpaceCollide(mSpaceID, this, CollisionCallback);
		dWorldStep(mWorldID, pStepSize);

		HandleMovableObjects();
		DoForceFeedback();
		dJointGroupEmpty(mContactJointGroupID);
	}
}

void PhysicsManagerODE::StepFast(float32 pStepSize)
{
	if (pStepSize > 0)
	{
		FlagMovingObjects();

		dSpaceCollide(mSpaceID, this, CollisionCallback);
		dWorldQuickStep(mWorldID, pStepSize);
		
		HandleMovableObjects();
		DoForceFeedback();
		dJointGroupEmpty(mContactJointGroupID);
	}
}

void PhysicsManagerODE::DoForceFeedback()
{
	JointList::iterator x;
	for (x = mFeedbackJointList.begin(); x != mFeedbackJointList.end();)
	{
		JointInfo* lJointInfo = *x;

		if (lJointInfo->mListener1 != lJointInfo->mListener2)
		{
			if (lJointInfo->mListener1 != 0)
			{
				dJointFeedback* lFeedback = &lJointInfo->mFeedback;
				lJointInfo->mListener1->OnForceApplied(
					lJointInfo->mListener2,
					Vector3D<float32>(lFeedback->f1[0], lFeedback->f1[1], lFeedback->f1[2]),
					Vector3D<float32>(lFeedback->t1[0], lFeedback->t1[1], lFeedback->t1[2]));
			}
			if (lJointInfo->mListener2 != 0)
			{
				dJointFeedback* lFeedback = &lJointInfo->mFeedback;
				lJointInfo->mListener2->OnForceApplied(
					lJointInfo->mListener1,
					Vector3D<float32>(-lFeedback->f1[0], -lFeedback->f1[1], -lFeedback->f1[2]),
					// TODO: check out how the torque should be. The force2 is always -force1
					// (actually f2[] just contains crap), but where does the torque2 go, if not in torque1?
					Vector3D<float32>(-lFeedback->t1[0], -lFeedback->t1[1], -lFeedback->t1[2]));
			}
		}

		if (lJointInfo->mType == JOINT_CONTACT)
		{
			JointList::iterator y = x;
			++x;
			mFeedbackJointList.erase(y);
			mJointInfoAllocator.Free(lJointInfo);
		}
		else
		{
			++x;
		}
	}
}

void PhysicsManagerODE::CollisionCallback(void* pData, dGeomID pGeom1, dGeomID pGeom2)
{
	Object* lObject1 = (Object*)dGeomGetData(pGeom1);
	Object* lObject2 = (Object*)dGeomGetData(pGeom2);

	if (lObject1->mForceFeedbackListener != 0 && lObject1->mForceFeedbackListener == lObject2->mForceFeedbackListener)
	{
		// The same force feedback recipient means same object. We don't collide here.
		return;
	}

	dBodyID lBody1 = ::dGeomGetBody(pGeom1);
	dBodyID lBody2 = ::dGeomGetBody(pGeom2);

	// Check if all bodies are static or disabled.
	if ((!lBody1 || !::dBodyIsEnabled(lBody1)) && (!lBody2 || !::dBodyIsEnabled(lBody2)))
	{
		// We don't want to act on static and disabled bodies.
		return;
	}
	if (lObject1->mTriggerListener == 0 && lObject2->mTriggerListener == 0)
	{
		// Exit without doing anything if the two bodies are connected by a joint.
		if (lBody1 && lBody2 && ::dAreConnectedExcluding(lBody1, lBody2, dJointTypeContact) != 0)
		{
			return;
		}
	}

	PhysicsManagerODE* lThis = (PhysicsManagerODE*)pData;
	dContact lContact[8];
	int lContactPointCount = -1;

	if (lObject1->mTriggerListener != 0)
	{
		lContactPointCount = (lContactPointCount < 0)? ::dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(dContact)) : lContactPointCount;
		if (lContactPointCount > 0)
		{
			lObject1->mTriggerListener->OnTrigger((BodyID)(size_t)lObject1, (BodyID)(size_t)lObject2);
		}
	}
	if(lObject2->mTriggerListener != 0)
	{
		lContactPointCount = (lContactPointCount < 0)? ::dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(dContact)) : lContactPointCount;
		if (lContactPointCount > 0)
		{
			lObject2->mTriggerListener->OnTrigger((BodyID)(size_t)lObject2, (BodyID)(size_t)lObject1);
		}
	}

	// Bounce/slide (if we haven't tried colliding OR we ARE colliding) AND NOT BOTH objects are triggers.
	if ((lContactPointCount < 0 || lContactPointCount > 0) &&
		(lObject1->mTriggerListener == 0 || lObject2->mTriggerListener == 0))
	{
		lContactPointCount = (lContactPointCount < 0)? ::dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(dContact)) : lContactPointCount;

		// Fetch force, will be used to scale friction (projected against surface normal).
		Vector3DF lPosition1 = lThis->GetBodyPosition((BodyID)lObject1);
		Vector3DF lPosition2 = lThis->GetBodyPosition((BodyID)lObject2);
		Vector3DF lLinearVelocity1;
		lThis->GetBodyVelocity((BodyID)lObject1, lLinearVelocity1);
		Vector3DF lLinearVelocity2;
		lThis->GetBodyVelocity((BodyID)lObject2, lLinearVelocity2);
		Vector3DF lAngularVelocity1;
		lThis->GetBodyAngularVelocity((BodyID)lObject1, lAngularVelocity1);
		Vector3DF lAngularVelocity2;
		lThis->GetBodyAngularVelocity((BodyID)lObject2, lAngularVelocity2);

		dMass lMass;
		float lMass1 = 1;
		if (lBody1)
		{
			::dBodyGetMass(lBody1, &lMass);
			lMass1 = lMass.mass;
		}
		float lMass2 = 1;
		if (lBody2)
		{
			::dBodyGetMass(lBody2, &lMass);
			lMass2 = lMass.mass;
		}

		// Perform normal collision detection.
		for (int i = 0; i < lContactPointCount; i++)
		{
			dContact& lC = lContact[i];

			const float lTotalFriction = ::fabs(lObject1->mFriction*lObject2->mFriction)+0.01f;
			// Negative friction factor means simple friction model.
			if (lObject1->mFriction > 0 || lObject2->mFriction > 0)
			{
				lC.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1 | dContactFDir1 | dContactBounce;

				const Vector3DF lNormal(lC.geom.normal[0], lC.geom.normal[1], lC.geom.normal[2]);
				const Vector3DF lCollisionPoint(lC.geom.pos[0], lC.geom.pos[1], lC.geom.pos[2]);
				const Vector3DF lDistance1(lCollisionPoint-lPosition1);
				const Vector3DF lDistance2(lCollisionPoint-lPosition2);
				const Vector3DF lAngularSurfaceVelocity1 = lAngularVelocity1.Cross(lDistance1);
				const Vector3DF lAngularSurfaceVelocity2 = lAngularVelocity2.Cross(lDistance2);
				const Vector3DF lSurfaceVelocity1 = lLinearVelocity1.ProjectOntoPlane(lNormal) + lAngularSurfaceVelocity1;
				const Vector3DF lSurfaceVelocity2 = lLinearVelocity2.ProjectOntoPlane(lNormal) + lAngularSurfaceVelocity1;
				const float lRelativeVelocity = lSurfaceVelocity1.GetDistance(lSurfaceVelocity2);
				Vector3DF lSpinDirection = (lAngularSurfaceVelocity1-lAngularSurfaceVelocity2);
				if (lSpinDirection.GetLengthSquared() <= 1e-4)
				{
					Vector3DF lDummy;
					lNormal.GetOrthogonals(lSpinDirection, lDummy);
				}
				else
				{
					lSpinDirection.Normalize();
				}
				lC.fdir1[0] = lSpinDirection.x;
				lC.fdir1[1] = lSpinDirection.y;
				lC.fdir1[2] = lSpinDirection.z;

				lC.surface.mu = dInfinity;
				const float lSlip = (1e-4f * lRelativeVelocity + 1e-6f) / lTotalFriction;
				lC.surface.slip1 = lSlip;
				lC.surface.slip2 = lSlip;
			}
			else
			{
				lC.surface.mode = dContactBounce;
				lC.surface.mu = lTotalFriction * 3 * lMass1 * lMass2;
			}
			lC.surface.bounce = (dReal)(lObject1->mBounce * lObject2->mBounce);
			lC.surface.bounce_vel = (dReal)0.000001;

			if (lObject1->mForceFeedbackListener != 0 ||
			   lObject2->mForceFeedbackListener != 0)
			{
				// Create a joint whith feedback info.
				JointInfo* lJointInfo = lThis->mJointInfoAllocator.Alloc();
				lJointInfo->mJointID = dJointCreateContact(lThis->mWorldID, lThis->mContactJointGroupID, &lC);
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
				dJointID lJointID = dJointCreateContact(lThis->mWorldID, lThis->mContactJointGroupID, &lC);
				dJointAttach(lJointID, lBody1, lBody2);
			}
		}
	}
}



const PhysicsManager::BodySet& PhysicsManagerODE::GetIdledBodies() const
{
	return (mAutoDisabledObjectSet);
}



void PhysicsManagerODE::FlagMovingObjects()
{
	mAutoDisabledObjectSet.clear();
	ObjectTable::iterator x = mObjectTable.begin();
	for (; x != mObjectTable.end(); ++x)
	{
		Object* lObject = *x;
		if (lObject->mBodyID && lObject->mIsRoot && ::dBodyIsEnabled(lObject->mBodyID))
		{
			mAutoDisabledObjectSet.insert((BodyID)lObject);
		}
	}
}

void PhysicsManagerODE::HandleMovableObjects()
{
	BodySet::iterator x = mAutoDisabledObjectSet.begin();
	while(x != mAutoDisabledObjectSet.end())
	{
		Object* lObject = (Object*)(*x);
		if (lObject->mBodyID && ::dBodyIsEnabled(lObject->mBodyID))
		{
			BodySet::iterator y = x;
			++x;
			mAutoDisabledObjectSet.erase(y);

			NormalizeRotation(lObject);
		}
		else
		{
			++x;
		}
	}
}

void PhysicsManagerODE::NormalizeRotation(BodyID pObject)
{
	Object* lObject = (Object*)pObject;
	if (lObject->mBodyID->geom->type == dBoxClass)
	{
		Vector3DF lVelocity;
		GetBodyAngularVelocity(lObject, lVelocity);
		const float lMaxAngularVelocity = 12.0f;
		if (lVelocity.GetLength() > lMaxAngularVelocity)
		{
			lVelocity.Normalize(lMaxAngularVelocity);
			SetBodyAngularVelocity(lObject, lVelocity);
			SetBodyAngularAcceleration(lObject, Vector3DF());
		}
	}
}



void PhysicsManagerODE::AdjustOrientation(dGeomID pGeom, QuaternionF& pQ, bool pSetter) const
{
	if (pGeom->type == dCapsuleClass)
	{
		// Capsules have different orientations in editor (along Y-axis) and ODE (along Z).
		QuaternionF lQ;
		if (pSetter)
		{
			//lQ.RotateAroundWorldX(PIF/2);
		}
		else
		{
			//lQ.RotateAroundWorldZ(PIF/2);
		}
		pQ = pQ * lQ;
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsManagerODE);



}
