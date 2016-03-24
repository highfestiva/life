
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Lepra/Include/LepraAssert.h"
#pragma warning(push)
#pragma warning(disable: 4100)	// Warning: unreferenced formal parameter (in ODE).
#pragma warning(disable: 4127)	// Warning: conditional expression is constant (in ODE).
#include <ode/odemath.h>
#include <../ode/src/collision_kernel.h>
#include <../ode/src/collision_std.h>
#include <../ode/src/joints/ball.h>
#include <../ode/src/joints/hinge.h>
#include <../ode/src/joints/slider.h>
#include <../ode/src/joints/universal.h>
#pragma warning(pop)
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../Include/PhysicsManagerODE.h"



namespace Tbc
{



bool AreBodiesConnectedExcluding(dBodyID b1, dBodyID b2, int joint_type)
{
	deb_assert(b1 || b2);
	dBodyID b = b1? b1 : b2;
	dBodyID s = b1? b2 : b1;
	for (dxJointNode* n=b->firstjoint; n; n=n->next)
	{
		if (dJointGetType(n->joint) != joint_type && n->body == s)
			return true;
	}
	return false;
}



PhysicsManagerODE::PhysicsManagerODE(float pRadius, int pLevels, float pSensitivity)
{
	mWorldID = dWorldCreate();
	
	// Play with these to make the simulation behave better.
	SetSimulationParameters(0, 0.1f, 1);

	if (pSensitivity)
	{
		::dWorldSetAutoDisableFlag(mWorldID, 1);
		::dWorldSetAutoDisableLinearThreshold(mWorldID, 0.02f/pSensitivity);
		::dWorldSetAutoDisableAngularThreshold(mWorldID, 0.02f/pSensitivity);
		::dWorldSetAutoDisableSteps(mWorldID, (int)(2*pSensitivity));
		//::dWorldSetAutoDisableTime(mWorldID, 0);
	}
	else
	{
		::dWorldSetAutoDisableFlag(mWorldID, 0);
	}

	//::dWorldSetLinearDampingThreshold(mWorldID, 100.0f);
	//::dWorldSetLinearDamping(mWorldID, 0.9f);
	::dWorldSetAngularDampingThreshold(mWorldID, 15.0f);
	::dWorldSetAngularDamping(mWorldID, 0.5f);
	::dWorldSetMaxAngularSpeed(mWorldID, 200.0f);

	// Collision space center and extents.
	dVector3 lCenter = {0, 0, 0};
	dVector3 lExtents = {pRadius, pRadius, pRadius};
	mSpaceID = ::dQuadTreeSpaceCreate(0, lCenter, lExtents, pLevels);

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

void PhysicsManagerODE::SetSimulationParameters(float pSoftness, float pRubberbanding, float pAccuracy)
{
	mWorldCfm = Math::Lerp(1e-9f, 1e-2f, pSoftness);	// World softness and numerical stability, i.e peneraation.
	::dWorldSetCFM(mWorldID, mWorldCfm);
	mWorldErp = Math::Lerp(1.0f, 0.2f, pRubberbanding);
	::dWorldSetERP(mWorldID, mWorldErp);	// Error reduction.
	::dWorldSetQuickStepNumIterations(mWorldID, (int)Math::Lerp(1, 20, pAccuracy));
}

bool PhysicsManagerODE::InitCurrentThread()
{
	return ::dAllocateODEDataForThread((unsigned)dAllocateMaskAll) != 0;
}

int PhysicsManagerODE::QueryRayCollisionAgainst(const vec3& pRayPosition, const vec3& pRayDirection,
	float pLength, BodyID pBody, vec3* pCollisionPoints, int pMaxCollisionCount)
{
	if (pMaxCollisionCount <= 0)
	{
		deb_assert(false);
		return 0;
	}

	ObjectTable::iterator x = mObjectTable.find((Object*)pBody);
	if (x == mObjectTable.end())
	{
		deb_assert(false);
		return 0;
	}
	const Object* lObject = *x;

	dGeomID lRayGeometryId = ::dCreateRay(0, pLength);
	::dGeomRaySet(lRayGeometryId, pRayPosition.x, pRayPosition.y, pRayPosition.z,
		pRayDirection.x, pRayDirection.y, pRayDirection.z);

	dContactGeom lContact[8];
	const int lMaxCount = std::min((int)LEPRA_ARRAY_COUNT(lContact), pMaxCollisionCount*2);
	const int lCollisionCount = ::dCollide(lRayGeometryId, lObject->mGeomID, lMaxCount, &lContact[0], sizeof(lContact[0]));

	::dGeomDestroy(lRayGeometryId);

	int lFoundCollisionPoints = 0;
	for (int x = 0; x < lCollisionCount; ++x)
	{
		// Check that we've found a surface turned towards the given direction.
		const vec3 lNormal(lContact[x].normal[0], lContact[x].normal[1], lContact[x].normal[2]);
		if (lNormal*pRayDirection > 0)
		{
			pCollisionPoints[lFoundCollisionPoints++].Set(lContact[x].pos[0], lContact[x].pos[1], lContact[x].pos[2]);
		}
	}

	return lFoundCollisionPoints;
}

int PhysicsManagerODE::QueryRayPick(const vec3& pRayPosition, const vec3& pRayDirection, float pLength, int* pForceFeedbackIds, vec3* pPositions, int pMaxBodies)
{
	dGeomID lRayGeometryId = ::dCreateRay(0, pLength);
	::dGeomRaySet(lRayGeometryId, pRayPosition.x, pRayPosition.y, pRayPosition.z,
		pRayDirection.x, pRayDirection.y, pRayDirection.z);

	int lHits = 0;
	void* lData[4] = {pForceFeedbackIds, pPositions, (void*)&lHits, (void*)&pMaxBodies};
	mSpaceID->collide2(&lData, lRayGeometryId, &PhysicsManagerODE::RayPickCallback);

	::dGeomDestroy(lRayGeometryId);

	return lHits;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateSphere(bool pIsRoot, const xform& pTransform,
	float32 pMass, float32 pRadius, BodyType pType, float32 pFriction, float32 pBounce,
	int pForceListenerId, bool pIsTrigger)
{
	Object* lObject = new Object(mWorldID, pIsRoot);
	lObject->mGeomID = dCreateSphere(mSpaceID, (dReal)pRadius);
	if (pIsTrigger)
	{
		lObject->mTriggerListenerId = pForceListenerId;
	}
	else
	{
		lObject->mForceFeedbackId = pForceListenerId;
	}

	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetSphereTotal(&lMass, (dReal)pMass, (dReal)pRadius);
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
		::dBodySetAngularDampingThreshold(lObject->mBodyID, 200.0f);
		::dBodySetAngularDamping(lObject->mBodyID, 0.2f);
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

PhysicsManager::BodyID PhysicsManagerODE::CreateCylinder(bool pIsRoot, const xform& pTransform,
	float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction,
	float32 pBounce, int pForceListenerId, bool pIsTrigger)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	// TODO: Create a real cylinder when ODE supports it.
	lObject->mGeomID = ::dCreateCylinder(mSpaceID, (dReal)pRadius, (dReal)pLength);

	if (pIsTrigger)
	{
		lObject->mTriggerListenerId = pForceListenerId;
	}
	else
	{
		lObject->mForceFeedbackId = pForceListenerId;
	}

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

PhysicsManager::BodyID PhysicsManagerODE::CreateCapsule(bool pIsRoot, const xform& pTransform,
	float32 pMass, float32 pRadius, float32 pLength, BodyType pType, float32 pFriction,
	float32 pBounce, int pForceListenerId, bool pIsTrigger)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	lObject->mGeomID = ::dCreateCapsule(mSpaceID, (dReal)pRadius, (dReal)pLength);
	if (pIsTrigger)
	{
		lObject->mTriggerListenerId = pForceListenerId;
	}
	else
	{
		lObject->mForceFeedbackId = pForceListenerId;
	}

	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetCapsuleTotal(&lMass, (dReal)pMass, 3, (dReal)pRadius, (dReal)pLength);
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

PhysicsManager::BodyID PhysicsManagerODE::CreateBox(bool pIsRoot, const xform& pTransform,
	float32 pMass, const vec3& pSize, BodyType pType, float32 pFriction,
	float32 pBounce, int pForceListenerId, bool pIsTrigger)
{
	Object* lObject = new Object(mWorldID, pIsRoot);

	lObject->mGeomID = ::dCreateBox(mSpaceID, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);
	if (pIsTrigger)
	{
		lObject->mTriggerListenerId = pForceListenerId;
	}
	else
	{
		lObject->mForceFeedbackId = pForceListenerId;
	}

	if (pType == PhysicsManager::DYNAMIC)
	{
		lObject->mBodyID = ::dBodyCreate(mWorldID);
		dMass lMass;
		::dMassSetBoxTotal(&lMass, (dReal)pMass, (dReal)pSize.x, (dReal)pSize.y, (dReal)pSize.z);
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
		deb_assert(false);
		return (false);
	}
	ObjectTable::iterator y = mObjectTable.find((Object*)pMainBody);
	if (y == mObjectTable.end() || x == y)
	{
		deb_assert(false);
		return (false);
	}
	Object* lStaticObject = *x;
	dVector3 lPos;
	::dGeomCopyPosition(lStaticObject->mGeomID, lPos);
	dQuaternion o;
	::dGeomGetQuaternion(lStaticObject->mGeomID, o);
	Object* lMainObject = *y;
	dBodyID lBodyId = lMainObject->mBodyID;
	if (!lBodyId)
	{
		return true;
	}
	if (lStaticObject->mBodyID)
	{
		JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
		lJointInfo->mJointID = dJointCreateFixed(mWorldID, 0);
		lJointInfo->mType = JOINT_FIXED;
		lJointInfo->mBody1Id = pStaticBody;
		lJointInfo->mBody2Id = pMainBody;
		lJointInfo->mListenerId1 = lStaticObject->mForceFeedbackId;
		lJointInfo->mListenerId2 = lMainObject->mForceFeedbackId;
		lMainObject->mHasMassChildren = true;
		dJointAttach(lJointInfo->mJointID, lStaticObject->mBodyID, lMainObject->mBodyID);
		dJointSetFixed(lJointInfo->mJointID);
	}
	else
	{
		::dGeomSetBody(lStaticObject->mGeomID, lBodyId);
		::dGeomSetOffsetWorldPosition(lStaticObject->mGeomID, lPos[0], lPos[1], lPos[2]);
		::dGeomSetOffsetWorldQuaternion(lStaticObject->mGeomID, o);
		AddMass(pStaticBody, pMainBody);
	}
	return (true);
}

bool PhysicsManagerODE::DetachToDynamic(BodyID pStaticBody, float32 pMass)
{
	Object* lObject = (Object*)pStaticBody;

	dGeomID g = lObject->mGeomID;
	dMass lMass;
	switch (g->type)
	{
		case dTriMeshClass:	// TRICKY: fall through (act as sphere).
		case dSphereClass:	::dMassSetSphereTotal(&lMass, (dReal)pMass, ((dxSphere*)g)->radius);						break;
		case dBoxClass:		::dMassSetBoxTotal(&lMass, (dReal)pMass, ((dxBox*)g)->side[0], ((dxBox*)g)->side[1], ((dxBox*)g)->side[2]);	break;
		case dCapsuleClass:	::dMassSetCapsuleTotal(&lMass, (dReal)pMass, 3, ((dxCapsule*)g)->radius, ((dxCapsule*)g)->lz);			break;
		case dCylinderClass:	::dMassSetCylinderTotal(&lMass, (dReal)pMass, 3, ((dxCylinder*)g)->radius, ((dxCylinder*)g)->lz);		break;
		default:
		{
			mLog.AError("Trying to detach object of unknown type!");
			deb_assert(false);
		}
		return (false);
	}

	if (lObject->mBodyID)
	{
		// Already dynamic, just update mass.
	}
	else
	{
		// Create dynamic body for it.
		lObject->mBodyID = ::dBodyCreate(mWorldID);
		const dReal* lPos = ::dGeomGetPosition(lObject->mGeomID);
		dQuaternion lQuat;
		::dGeomGetQuaternion(lObject->mGeomID, lQuat);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dGeomSetPosition(lObject->mGeomID, lPos[0], lPos[1], lPos[2]);
		::dGeomSetQuaternion(lObject->mGeomID, lQuat);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
	}
	::dBodySetMass(lObject->mBodyID, &lMass);
	return true;
}

bool PhysicsManagerODE::MakeStatic(BodyID pDynamicBody)
{
	ObjectTable::iterator x = mObjectTable.find((Object*)pDynamicBody);
	if (x == mObjectTable.end())
	{
		deb_assert(false);
		return (false);
	}
	Object* lDynamicObject = *x;
	if (lDynamicObject->mBodyID)
	{
		::dGeomSetBody(lDynamicObject->mGeomID, 0);
		::dBodyDestroy(lDynamicObject->mBodyID);
		lDynamicObject->mBodyID = 0;
	}
	return true;
}

bool PhysicsManagerODE::AddMass(BodyID pStaticBody, BodyID pMainBody)
{
	ObjectTable::iterator x = mObjectTable.find((Object*)pStaticBody);
	if (x == mObjectTable.end())
	{
		deb_assert(false);
		return (false);
	}
	ObjectTable::iterator y = mObjectTable.find((Object*)pMainBody);
	if (y == mObjectTable.end() || x == y)
	{
		deb_assert(false);
		return (false);
	}
	Object* lStaticObject = *x;
	Object* lMainObject = *y;
	if (lStaticObject->mBodyID)
	{
		mLog.AError("Attach() with non-static.");
		deb_assert(false);
		return (false);
	}
	if (!lStaticObject->mMass)	// No point in trying to add flyweight trigger.
	{
		return false;
	}
	dBodyID lBodyId = lMainObject->mBodyID;
	if (!lBodyId)
	{
		deb_assert(false);
		return (false);
	}

	lMainObject->mHasMassChildren = true;

	dMass lMass;
	const dReal lMassScalar = (dReal)lStaticObject->mMass;
	deb_assert(lMassScalar > 0);
	float* lSize = lStaticObject->mGeometryData;
	// Adding mass to the dynamic object.
	switch (lStaticObject->mGeomID->type)
	{
		case dTriMeshClass:	// TRICKY: fall through (act as sphere).
		case dSphereClass:	::dMassSetSphereTotal(&lMass, lMassScalar, (dReal)lSize[0]);					break;
		case dBoxClass:		::dMassSetBoxTotal(&lMass, lMassScalar, (dReal)lSize[0], (dReal)lSize[1], (dReal)lSize[2]);	break;
		case dCapsuleClass:	::dMassSetCapsuleTotal(&lMass, lMassScalar, 3, (dReal)lSize[0], (dReal)lSize[1]);		break;
		case dCylinderClass:	::dMassSetCylinderTotal(&lMass, lMassScalar, 3, (dReal)lSize[0], (dReal)lSize[1]);		break;
		default:
		{
			mLog.AError("Trying to attach object of unknown type!");
			deb_assert(false);
			return (false);
		}
	}
	const dReal* lRelRot = ::dGeomGetOffsetRotation(lStaticObject->mGeomID);
	::dMassRotate(&lMass, lRelRot);
	const dReal* lRelPos = ::dGeomGetOffsetPosition(lStaticObject->mGeomID);
	::dMassTranslate(&lMass, lRelPos[0], lRelPos[1], lRelPos[2]);

	dMass lDynamicMass;
	::dBodyGetMass(lBodyId, &lDynamicMass);
	::dMassAdd(&lDynamicMass, &lMass);
	::dBodySetMass(lBodyId, &lDynamicMass);

	return true;
}

PhysicsManager::BodyID PhysicsManagerODE::CreateTriMesh(bool pIsRoot, unsigned pVertexCount, const float* pVertices, unsigned pTriangleCount, const Lepra::uint32* pIndices,
	const xform& pTransform, float32 pMass, BodyType pType, float32 pFriction, float32 pBounce, int pForceListenerId, bool pIsTrigger)
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
	if (pIsTrigger)
	{
		lObject->mTriggerListenerId = pForceListenerId;
	}
	else
	{
		lObject->mForceFeedbackId = pForceListenerId;
	}

//	dGeomTriMeshEnableTC(lObject->mGeomID, dBoxClass, 1);

	float lAverageRadius = 0;
	for (unsigned x = 0; x < pVertexCount; ++x)
	{
		lAverageRadius += vec3(pVertices[x*3+0], pVertices[x*3+0], pVertices[x*3+0]).GetLength();
	}
	lAverageRadius /= pVertexCount;
	if (pType == PhysicsManager::DYNAMIC)
	{
		dMass lMass;
		::dMassSetSphereTotal(&lMass, (dReal)pMass, (dReal)lAverageRadius);
		lObject->mBodyID = dBodyCreate(mWorldID);
		::dBodySetMass(lObject->mBodyID, &lMass);
		::dGeomSetBody(lObject->mGeomID, lObject->mBodyID);
		::dBodySetAutoDisableDefaults(lObject->mBodyID);
		::dBodySetAngularDampingThreshold(lObject->mBodyID, 200.0f);
		::dBodySetAngularDamping(lObject->mBodyID, 0.2f);
	}

	lObject->mGeometryData[0] = lAverageRadius;
	lObject->mMass = pMass;
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
	}
	else
	{
		mLog.AError("DeleteBody() - Can't find body to delete!");
		deb_assert(false);
	}
}

vec3 PhysicsManagerODE::GetBodyPosition(BodyID pBodyId) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyPosition() - Body %i is not part of this world!"), pBodyId);
		return (vec3());
	}

	const dReal* lPosition = dGeomGetPosition(lObject->mGeomID);
	return (vec3(lPosition[0], lPosition[1], lPosition[2]));
}

void PhysicsManagerODE::SetBodyPosition(BodyID pBodyId, const vec3& pPosition) const
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyPosition() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	if(lObject->mBodyID)
	{
		::dBodySetPosition(lObject->mBodyID, pPosition.x, pPosition.y, pPosition.z);
		::dBodyEnable(lObject->mBodyID);
	}
	else
	{
		::dGeomSetPosition(lObject->mGeomID, pPosition.x, pPosition.y, pPosition.z);
	}
}

quat PhysicsManagerODE::GetBodyOrientation(BodyID pBodyId) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyOrientation() - Body %i is not part of this world!"), pBodyId);
		return gIdentityQuaternionF;
	}

	quat lQuat;
	::dGeomGetQuaternion(lObject->mGeomID, lQuat.mData);
	return lQuat;
}

void PhysicsManagerODE::SetBodyOrientation(BodyID pBodyId, const quat& pOrientation)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyOrientation() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	if(lObject->mBodyID)
	{
		::dBodySetQuaternion(lObject->mBodyID, pOrientation.mData);
		::dBodyEnable(lObject->mBodyID);
	}
	else
	{
		::dGeomSetQuaternion(lObject->mGeomID, pOrientation.mData);
	}
}

void PhysicsManagerODE::GetBodyTransform(BodyID pBodyId, xform& pTransform) const
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetBodyTransform() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	::dGeomCopyPosition(lObject->mGeomID, pTransform.mPosition.mData);
	::dGeomGetQuaternion(lObject->mGeomID, pTransform.mOrientation.mData);
}

void PhysicsManagerODE::SetBodyTransform(BodyID pBodyId, const xform& pTransform)
{
	Object* lObject = (Object*)pBodyId;

	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyTransform() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	const vec3 lPos(pTransform.GetPosition());
	quat lQuat = pTransform.GetOrientation();
	dReal lQ[4];
	lQ[0] = lQuat.a;
	lQ[1] = lQuat.b;
	lQ[2] = lQuat.c;
	lQ[3] = lQuat.d;
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

void PhysicsManagerODE::GetBodyVelocity(BodyID pBodyId, vec3& pVelocity) const
{
	Object* lObject = (Object*)pBodyId;
	dBodyID lBodyId = lObject->mBodyID;
	if (!lBodyId)
	{
		lBodyId = lObject->mGeomID->body;
	}
	if (lBodyId)
	{
		const dReal* lVelocity = dBodyGetLinearVel(lBodyId);
		pVelocity.x = lVelocity[0];
		pVelocity.y = lVelocity[1];
		pVelocity.z = lVelocity[2];
	}
	else
	{
		pVelocity.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyVelocity(BodyID pBodyId, const vec3& pVelocity)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetLinearVel(lObject->mBodyID, pVelocity.x, pVelocity.y, pVelocity.z);
		::dBodyEnable(lObject->mBodyID);
	}
}

void PhysicsManagerODE::GetBodyForce(BodyID pBodyId, vec3& pAcceleration) const
{
	Object* lObject = (Object*)pBodyId;
	dBodyID lBodyId = lObject->mBodyID;
	if (!lBodyId)
	{
		lBodyId = lObject->mGeomID->body;
	}
	if(lBodyId)
	{
		const dReal* lAcceleration = dBodyGetForce(lBodyId);
		pAcceleration.x = lAcceleration[0];
		pAcceleration.y = lAcceleration[1];
		pAcceleration.z = lAcceleration[2];
	}
	else
	{
		pAcceleration.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyForce(BodyID pBodyId, const vec3& pAcceleration)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
		dBodySetForce(lObject->mBodyID, pAcceleration.x, pAcceleration.y, pAcceleration.z);
}

void PhysicsManagerODE::GetBodyAcceleration(BodyID pBodyId, float pTotalMass, vec3& pAcceleration) const
{
	deb_assert(pTotalMass > 0);
	GetBodyForce(pBodyId, pAcceleration);
	pAcceleration /= pTotalMass;
}

void PhysicsManagerODE::SetBodyAcceleration(BodyID pBodyId, float pTotalMass, const vec3& pAcceleration)
{
	deb_assert(pTotalMass > 0);
	SetBodyForce(pBodyId, pAcceleration * pTotalMass);
}

void PhysicsManagerODE::GetBodyAngularVelocity(BodyID pBodyId, vec3& pAngularVelocity) const
{
	Object* lObject = (Object*)pBodyId;
	dBodyID lBodyId = lObject->mBodyID;
	if (!lBodyId)
	{
		lBodyId = lObject->mGeomID->body;
	}
	if(lBodyId)
	{
		const dReal* lAngularVelocity = ::dBodyGetAngularVel(lBodyId);
		pAngularVelocity.x = lAngularVelocity[0];
		pAngularVelocity.y = lAngularVelocity[1];
		pAngularVelocity.z = lAngularVelocity[2];
	}
	else
	{
		pAngularVelocity.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyAngularVelocity(BodyID pBodyId, const vec3& pAngularVelocity)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetAngularVel(lObject->mBodyID, pAngularVelocity.x, pAngularVelocity.y, pAngularVelocity.z);
	}
}

void PhysicsManagerODE::GetBodyTorque(BodyID pBodyId, vec3& pAngularAcceleration) const
{
	Object* lObject = (Object*)pBodyId;
	dBodyID lBodyId = lObject->mBodyID;
	if (!lBodyId)
	{
		lBodyId = lObject->mGeomID->body;
	}
	if(lBodyId)
	{
		const dReal* lAngularAcceleration = dBodyGetTorque(lBodyId);
		pAngularAcceleration.x = lAngularAcceleration[0];
		pAngularAcceleration.y = lAngularAcceleration[1];
		pAngularAcceleration.z = lAngularAcceleration[2];
	}
	else
	{
		pAngularAcceleration.Set(0, 0, 0);
	}
}

void PhysicsManagerODE::SetBodyTorque(BodyID pBodyId, const vec3& pAngularAcceleration)
{
	Object* lObject = (Object*)pBodyId;
	if(lObject->mBodyID)
	{
		::dBodySetTorque(lObject->mBodyID, pAngularAcceleration.x, pAngularAcceleration.y, pAngularAcceleration.z);
	}
}

void PhysicsManagerODE::GetBodyAngularAcceleration(BodyID pBodyId, float pTotalMass, vec3& pAngularAcceleration) const
{
	GetBodyTorque(pBodyId, pAngularAcceleration);
	// TODO: handle moment of inertia?
	pAngularAcceleration /= pTotalMass;
}

void PhysicsManagerODE::SetBodyAngularAcceleration(BodyID pBodyId, float pTotalMass, const vec3& pAngularAcceleration)
{
	// TODO: handle moment of inertia?
	SetBodyTorque(pBodyId, pAngularAcceleration * pTotalMass);
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

void PhysicsManagerODE::SetBodyMass(BodyID pBodyId, float pMass)
{
	Object* lObject = (Object*)pBodyId;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetBodyMass() - Body %i is not part of this world!"), pBodyId);
		return;
	}
	if (!lObject->mBodyID || pMass <= 0)
	{
		mLog.Errorf(_T("SetBodyMass() - body %i is static or mass %f is not greater than zero!"), pBodyId, pMass);
		return;
	}
	lObject->mMass = pMass;
	dMass lMass;
	lMass.setSphereTotal(lObject->mMass, lObject->mGeometryData[0]);
	::dBodySetMass(lObject->mBodyID, &lMass);
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


int PhysicsManagerODE::GetTriggerListenerId(BodyID pTrigger)
{
	Object* lObject = (Object*)pTrigger;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetForceFeedbackListenerId() - trigger %i is not part of this world!"), pTrigger);
		return (0);
	}
	return (lObject->mTriggerListenerId);
}

int PhysicsManagerODE::GetForceFeedbackListenerId(BodyID pBody)
{
	Object* lObject = (Object*)pBody;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("GetForceFeedbackListenerId() - Body %i is not part of this world!"), pBody);
		return (0);
	}
	return (lObject->mForceFeedbackId);
}

void PhysicsManagerODE::SetForceFeedbackListener(BodyID pBody, int pForceFeedbackId)
{
	Object* lObject = (Object*)pBody;
	if (lObject->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("SetForceFeedbackListener() - Body %i is not part of this world!"), pBody);
		return;
	}
	lObject->mForceFeedbackId = pForceFeedbackId;
}

PhysicsManager::JointID PhysicsManagerODE::CreateBallJoint(BodyID pBody1, BodyID pBody2, const vec3& pAnchorPos)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateBallJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateBall(mWorldID, 0);
	lJointInfo->mType = JOINT_BALL;
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	lObject1->mHasMassChildren = true;
	if (lObject2 != 0)
	{
		lObject2->mHasMassChildren = true;
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
		//::dBodySetLinearDampingThreshold(lObject2->mBodyID, 30.0f);
		//::dBodySetLinearDamping(lObject2->mBodyID, 0.95f);
		::dBodySetAngularDampingThreshold(lObject2->mBodyID, 10.0f);
		::dBodySetAngularDamping(lObject2->mBodyID, 0.97f);
		::dBodySetMaxAngularSpeed(lObject2->mBodyID, 20.0f);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
		::dBodySetAngularDampingThreshold(lObject2->mBodyID, 15.0f);
		::dBodySetAngularDamping(lObject2->mBodyID, 0.97f);
		::dBodySetMaxAngularSpeed(lObject2->mBodyID, 30.0f);
	}

	/*if ((lObject1 != 0 && lObject1->mForceFeedbackId != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackId != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

	dJointSetBallAnchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsManager::JointID PhysicsManagerODE::CreateHingeJoint(BodyID pBody1, BodyID pBody2, 
	const vec3& pAnchorPos, const vec3& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateHingeJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateHinge(mWorldID, 0);
	lJointInfo->mType = JOINT_HINGE;
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	lObject1->mHasMassChildren = true;
	if (lObject2 != 0)
	{
		lObject2->mHasMassChildren = true;
		dJointAttach(lJointInfo->mJointID, lObject1->mGeomID->body, lObject2->mGeomID->body);

		if (lObject2->mGeomID->type == dBoxClass)
		{
			// Someone is attaching a revolving box to another dynamic object. That means that we
			// potentially could crash into something hard, causing overflow in the physics stepper.
			// We eliminate by making the 2nd body use a pretty limited maximum angular speed. However,
			// we raise the angular damping threshold to avoid limiting engine power for these high-
			// rotation/low torque engines (usually rotor or similar).
			::dBodySetMaxAngularSpeed(lObject2->mBodyID, 50.0f);
			::dBodySetAngularDampingThreshold(lObject2->mBodyID, 55.0f);
			lObject2->mIsRotational = true;
		}
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mGeomID->body, 0);
		lObject1->mIsRotational = true;
	}

	/*if ((lObject1 != 0 && lObject1->mForceFeedbackId != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackId != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

	dJointSetHingeAnchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);
	dJointSetHingeAxis(lJointInfo->mJointID, pAxis.x, pAxis.y, pAxis.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsManager::JointID PhysicsManagerODE::CreateHinge2Joint(BodyID pBody1, BodyID pBody2,
	const vec3& pAnchorPos, const vec3& pAxis1,
	const vec3& pAxis2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies2(pBody1, pBody2, lObject1, lObject2, _T("CreateHinge2Joint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateHinge2(mWorldID, 0);
	lJointInfo->mType = JOINT_HINGE2;
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);

	lObject1->mHasMassChildren = true;
	lObject2->mHasMassChildren = true;
	lObject2->mIsRotational = true;
	/*if (lObject1->mForceFeedbackId != 0 || 
	   lObject2->mForceFeedbackId != 0)
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

	dJointSetHinge2Anchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);
	dJointSetHinge2Axis1(lJointInfo->mJointID, pAxis1.x, pAxis1.y, pAxis1.z);
	dJointSetHinge2Axis2(lJointInfo->mJointID, pAxis2.x, pAxis2.y, pAxis2.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsManager::JointID PhysicsManagerODE::CreateUniversalJoint(BodyID pBody1, BodyID pBody2,
	const vec3& pAnchorPos, const vec3& pAxis1,
	const vec3& pAxis2)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateUniversalJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = ::dJointCreateUniversal(mWorldID, 0);
	lJointInfo->mType = JOINT_UNIVERSAL;
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	lObject1->mHasMassChildren = true;
	if (lObject2 != 0)
	{
		lObject2->mHasMassChildren = true;
		::dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		::dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	/*if ((lObject1 != 0 && lObject1->mForceFeedbackId != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackId != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

	::dJointSetUniversalAnchor(lJointInfo->mJointID, pAnchorPos.x, pAnchorPos.y, pAnchorPos.z);
	::dJointSetUniversalAxis1(lJointInfo->mJointID, pAxis1.x, pAxis1.y, pAxis1.z);
	::dJointSetUniversalAxis2(lJointInfo->mJointID, pAxis2.x, pAxis2.y, pAxis2.z);

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsManager::JointID PhysicsManagerODE::CreateSliderJoint(BodyID pBody1, BodyID pBody2,
	const vec3& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateSliderJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateSlider(mWorldID, 0);
	lJointInfo->mType = JOINT_SLIDER;
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	lObject1->mHasMassChildren = true;
	if (lObject2 != 0)
	{
		lObject2->mHasMassChildren = true;
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	/*if ((lObject1 != 0 && lObject1->mForceFeedbackId != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackId != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

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
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	lObject1->mHasMassChildren = true;
	if (lObject2 != 0)
	{
		lObject2->mHasMassChildren = true;
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
	}

	/*if ((lObject1 != 0 && lObject1->mForceFeedbackId != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackId != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

	mJointTable.insert(lJointInfo);
	return (JointID)lJointInfo;
}

PhysicsManager::JointID PhysicsManagerODE::CreateAngularMotorJoint(BodyID pBody1, BodyID pBody2,
	const vec3& pAxis)
{
	Object* lObject1;
	Object* lObject2;
	if (CheckBodies(pBody1, pBody2, lObject1, lObject2, _T("CreateAngularMotorJoint()")) == false)
		return INVALID_JOINT;

	JointInfo* lJointInfo = mJointInfoAllocator.Alloc();
	lJointInfo->mJointID = dJointCreateAMotor(mWorldID, 0);
	lJointInfo->mType = JOINT_ANGULARMOTOR;
	lJointInfo->mBody1Id = pBody1;
	lJointInfo->mBody2Id = pBody2;
	lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
	lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;

	lObject1->mHasMassChildren = true;
	if (lObject2 != 0)
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, lObject2->mBodyID);
		lObject2->mHasMassChildren = true;
		lObject2->mIsRotational = true;
	}
	else
	{
		dJointAttach(lJointInfo->mJointID, lObject1->mBodyID, 0);
		lObject1->mIsRotational = true;
	}

	/*if ((lObject1 != 0 && lObject1->mForceFeedbackId != 0) || 
	   (lObject2 != 0 && lObject2->mForceFeedbackId != 0))
	{
		dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
	}*/

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
	RemoveJoint(lJointInfo);

	Object* lObject1;
	Object* lObject2;
	if (lJointInfo->mBody1Id && CheckBodies(lJointInfo->mBody1Id, lJointInfo->mBody2Id, lObject1, lObject2, _T("DeleteJoint()")))
	{
		if (lObject2)
		{
			lObject2->mIsRotational = false;
		}
		else
		{
			lObject1->mIsRotational = false;
		}
	}
}

bool PhysicsManagerODE::StabilizeJoint(JointID pJointId)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
			if (!lChildBody)
			{
				lChildBody = lJointInfo->mJointID->node[0].body;
			}
			const dReal* lPos = ::dBodyGetPosition(lChildBody);
			lAnchor[0] = lPos[0] + lAnchor[0] - lAnchor2[0];
			lAnchor[1] = lPos[1] + lAnchor[1] - lAnchor2[1];
			lAnchor[2] = lPos[2] + lAnchor[2] - lAnchor2[2];
			::dBodySetPosition(lChildBody, lAnchor[0], lAnchor[1], lAnchor[2]);
			const dReal* lVel = ::dBodyGetLinearVel(lChildBody);
			if (::fabs(lVel[0]) > 50 || ::fabs(lVel[1]) > 50 || ::fabs(lVel[2]) > 50)
			{
				::dBodySetLinearVel(lChildBody, 0, 0, 0);
				::dBodySetAngularVel(lChildBody, 0, 0, 0);
			}
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-1-type!"), lJointInfo->mType);
			deb_assert(false);
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
		//mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
		case JOINT_SLIDER:
		{
			lOk = GetSliderDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-1-type!"), lJointInfo->mType);
			deb_assert(false);
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
		mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
		case JOINT_SLIDER:
		{
			lOk = SetSliderDiff(pBodyId, pJointId, pDiff);
		}
		break;
		default:
		{
			mLog.Errorf(_T("Joint type %i of non-1-type!"), lJointInfo->mType);
			deb_assert(false);
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
		//mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
			deb_assert(false);
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
		mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
			deb_assert(false);
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
		//mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
			deb_assert(false);
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
		mLog.Warningf(_T("Couldn't find joint %i!"), pJointId);
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
			deb_assert(false);
		}
		break;
	}
	return (lOk);
}

void PhysicsManagerODE::RemoveJoint(JointInfo* pJointInfo)
{
	pJointInfo->mListenerId1 = 0;
	pJointInfo->mListenerId2 = 0;
	pJointInfo->mBody1Id = 0;
	pJointInfo->mBody2Id = 0;
	mJointInfoAllocator.Free(pJointInfo);
}

bool PhysicsManagerODE::GetHingeDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_HINGE);
	if (lJointInfo->mType != JOINT_HINGE)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	vec3 lAxis;
	if (!GetAxis1(pJointId, lAxis) || !GetAngle1(pJointId, pDiff.mValue))
	{
		return (false);
	}

	{
		vec3 lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mVelocity = lAxis * lVelocity;
	}

	{
		vec3 lAcceleration;
		GetBodyTorque(pBodyId, lAcceleration);
		pDiff.mAcceleration = lAxis * lAcceleration;
	}

	return (true);
}

bool PhysicsManagerODE::SetHingeDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_HINGE);
	if (lJointInfo->mType != JOINT_HINGE)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	vec3 lAnchor;
	vec3 lAxis;
	if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis))
	{
		return (false);
	}
	deb_assert(lAxis.GetLengthSquared() > 0.99f && lAxis.GetLengthSquared() < 1.01f);

	dxBody* lParentBody = lJointInfo->mJointID->node[0].body;

	// Fetch parent data (or identity if parent is World).
	const dReal* lPos = ::dBodyGetPosition(lParentBody);
	const vec3 lParentPosition(lPos);
	quat lParentQ(-1, 0, 0, 0);
	vec3 lParentVelocity;
	vec3 lParentAcceleration;
	if (!lJointInfo->mJointID->node[1].body)
	{
	}
	else
	{
		deb_assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		lParentQ.Set(lPQ);
	}

	{
		// Rotate to original child (us) orientation.
		dxJointHinge* lHinge = (dxJointHinge*)lJointInfo->mJointID;
		quat lQ(lHinge->qrel);
		// Set orientation.
		xform lTransform;
		GetBodyTransform(pBodyId, lTransform);
		if (lJointInfo->mJointID->node[1].body)
		{
			lQ = lParentQ * lQ;
		}
		else
		{
			lQ.MakeInverse();
		}
		// Rotate to input angle.
		lQ = quat(-pDiff.mValue, lAxis) * lQ;
		lTransform.SetOrientation(lQ);
		if (lJointInfo->mJointID->node[1].body)
		{
			// Align anchors (happen after rotation) and store.
			vec3 lAnchor2(lHinge->anchor2);
			lAnchor2 = lQ*lAnchor2 + lTransform.GetPosition();
			lTransform.GetPosition() += lAnchor-lAnchor2;
		}
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		if (pDiff.mVelocity < PIF*1000)
		{
			vec3 lVelocity;
			GetBodyAngularVelocity(pBodyId, lVelocity);
			// Drop angular velocity along axis 1 & 2, then add the specified amount.
			vec3 lAxisVelocity = lAxis*(lAxis*lVelocity);
			lVelocity -= lAxisVelocity;
			lVelocity += lAxis * pDiff.mVelocity;
			SetBodyAngularVelocity(pBodyId, lVelocity);
		}
	}

	{
		if (pDiff.mAcceleration < PIF*1000)
		{
			vec3 lAcceleration;
			GetBodyTorque(pBodyId, lAcceleration);
			// Drop angular acceleration along axis, then add the specified amount.
			vec3 lAxisAcceleration = lAxis*(lAxis*lAcceleration);
			lAcceleration -= lAxisAcceleration;
			lAcceleration += lAxis * pDiff.mAcceleration;
			SetBodyTorque(pBodyId, lAcceleration);
		}
	}

	return (true);
}

bool PhysicsManagerODE::GetSliderDiff(BodyID pBodyId, JointID pJointId, Joint1Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_SLIDER);
	if (lJointInfo->mType != JOINT_SLIDER)
	{
		mLog.Errorf(_T("Joint type %i of non-slider-type!"), lJointInfo->mType);
		return (false);
	}

	vec3 lAxis;
	if (!GetAxis1(pJointId, lAxis))
	{
		return (false);
	}
	deb_assert(lAxis.GetLengthSquared() > 0.99f && lAxis.GetLengthSquared() < 1.01f);

	GetSliderPos(pJointId, pDiff.mValue);
	GetSliderSpeed(pJointId, pDiff.mVelocity);

	{
		vec3 lAcceleration;
		GetBodyForce(pBodyId, lAcceleration);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lParentForce = ::dBodyGetForce(lParentBody);
		lAcceleration -= vec3(lParentForce);
		pDiff.mAcceleration = lAxis * lAcceleration;
	}

	return (true);
}

bool PhysicsManagerODE::SetSliderDiff(BodyID pBodyId, JointID pJointId, const Joint1Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_SLIDER);
	if (lJointInfo->mType != JOINT_SLIDER)
	{
		mLog.Errorf(_T("Joint type %i of non-hinge-type!"), lJointInfo->mType);
		return (false);
	}

	vec3 lAxis;
	if (!GetAxis1(pJointId, lAxis))
	{
		return (false);
	}
	deb_assert(lAxis.GetLengthSquared() > 0.99f && lAxis.GetLengthSquared() < 1.01f);

	dxBody* lParentBody = lJointInfo->mJointID->node[0].body;

	// Fetch parent data (or identity if parent is World).
	const dReal* lPos = ::dBodyGetPosition(lParentBody);
	const vec3 lParentPosition(lPos);
	quat lParentQ(-1, 0, 0, 0);
	vec3 lParentVelocity;
	vec3 lParentAcceleration;
	if (!lJointInfo->mJointID->node[1].body)
	{
	}
	else
	{
		deb_assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		lParentQ.Set(lPQ);
		const dReal* lParentV = ::dBodyGetLinearVel(lParentBody);
		lParentVelocity.Set(lParentV);
		const dReal* lParentForce = ::dBodyGetForce(lParentBody);
		lParentAcceleration.Set(lParentForce);
		// Downscale acceleration with mass.
		lParentAcceleration *= lJointInfo->mJointID->node[1].body->mass.mass / lParentBody->mass.mass;
	}

	{
		// Rotate to original child (us) orientation.
		dxJointSlider* lSlider = (dxJointSlider*)lJointInfo->mJointID;
		quat lQ(lSlider->qrel);
		// Relative translation.
		vec3 lOffset(lSlider->offset);
		if (lJointInfo->mJointID->node[1].body)
		{
			lQ = lParentQ * lQ;
			lOffset = lQ * lOffset;
		}
		else
		{
			lQ.MakeInverse();
			lOffset = lParentPosition - lOffset;
		}
		lOffset += lAxis*pDiff.mValue;
		// Small translational diff, no orientational diff (world parent is stiff) means we get a
		// better experiance by not forcing this tiny jerk into the game. Instead let the local
		// (most probably client) physics engine be chief.
		if (lJointInfo->mJointID->node[1].body || lOffset.GetLengthSquared() > 0.5f)
		{
			// Set orientation.
			xform lTransform(lQ,
				lParentPosition - lOffset);
			SetBodyTransform(pBodyId, lTransform);
		}
	}

	{
		lParentVelocity -= lAxis*pDiff.mVelocity;
		SetBodyVelocity(pBodyId, lParentVelocity);
	}

	{
		lParentAcceleration -= lAxis*pDiff.mAcceleration;
		SetBodyForce(pBodyId, lParentAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetUniversalDiff(BodyID pBodyId, JointID pJointId, Joint2Diff& pDiff) const
{
	LEPRA_DEBUG_CODE(JointInfo* lJointInfo = (JointInfo*)pJointId;)
	LEPRA_DEBUG_CODE(deb_assert(lJointInfo->mType == JOINT_UNIVERSAL));

	vec3 lAxis1;
	vec3 lAxis2;
	{
		if (!GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2) ||
			!GetAngle1(pJointId, pDiff.mValue) || !GetAngle2(pJointId, pDiff.mAngle))
		{
			return (false);
		}
		deb_assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		deb_assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
	}

	{
		if (!GetAngleRate1(pJointId, pDiff.mValueVelocity) || !GetAngleRate2(pJointId, pDiff.mAngleVelocity))
		{
			return (false);
		}
	}

	{
		vec3 lAcceleration;
		GetBodyTorque(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = -(lAxis2 * lAcceleration);
		pDiff.mAngleAcceleration = -(lAxis1 * lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::SetUniversalDiff(BodyID pBodyId, JointID pJointId, const Joint2Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_UNIVERSAL);

	vec3 lAxis1;
	vec3 lAxis2;
	vec3 lAnchor;
	if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1))
	{
		return (false);
	}
	deb_assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);

	{
		// Fetch parent orientation.
		deb_assert(!lJointInfo->mJointID->node[1].body || lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		const quat lParentQ(lPQ);
		const dReal* lPP = ::dBodyGetPosition(lParentBody);
		// Rotate to cross piece orientation.
		dxJointUniversal* lUniversal = (dxJointUniversal*)lJointInfo->mJointID;
		//quat lRelativeQ(lUniversal->qrel1);
		quat lQ = lParentQ;
		//quat lQ = lParentQ.GetInverse() * lRelativeQ;
		// Rotate to body 1's input angle.
		lQ = quat(-pDiff.mValue, lAxis1) * lQ;
		// Apply rotation from cross piece to original child (us).
		//lRelativeQ.Set(lUniversal->qrel2);
		//lQ = lRelativeQ.GetInverse() * lQ;
		// Rotating around body 1's axis changes body 2's axis. Fetch and act on it AFTER rotation 'round axis1.
		lAxis2 = lQ * vec3(lUniversal->axis2);
		lQ = quat(-pDiff.mAngle, lAxis2) * lQ;
		// Set orientation.
		vec3 lParentP(lPP);
		xform lTransform;
		GetBodyTransform(pBodyId, lTransform);
		xform lParentTransformation(lParentQ, lParentP);
		lTransform.mPosition = lParentQ*(lTransform.mPosition-lParentP) + lParentP;
		lTransform.mOrientation = lParentQ;
		// TODO: fix your linear algebra shit first!
		//lTransform.SetOrientation(lQ);
		// Align anchors (happen after rotation) and store.
		vec3 lAnchor2(lUniversal->anchor2);
		lAnchor2 = lQ*lAnchor2 + lTransform.GetPosition();
		//dVector3 lRealAnchor;
		//::dJointGetUniversalAnchor2(lUniversal, lRealAnchor);
		// TODO: fix your linear algebra shit first!
		lTransform.mPosition += lAnchor-lAnchor2;
		SetBodyTransform(pBodyId, lTransform);
	}

	{
		vec3 lAxisVelocity;
		vec3 lOriginalVelocity;
		GetBodyAngularVelocity(pBodyId, lOriginalVelocity);
		vec3 lVelocity = lOriginalVelocity;
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
		//SetBodyVelocity(pBodyId, vec3());
		SetBodyAngularVelocity(pBodyId, lVelocity);
	}

	{
		vec3 lAxisAcceleration;
		vec3 lOriginalAcceleration;
		GetBodyTorque(pBodyId, lOriginalAcceleration);
		vec3 lAcceleration = lOriginalAcceleration;
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
		//SetBodyForce(pBodyId, vec3());
		SetBodyTorque(pBodyId, lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetHinge2Diff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	LEPRA_DEBUG_CODE(JointInfo* lJointInfo = (JointInfo*)pJointId;)
	LEPRA_DEBUG_CODE(deb_assert(lJointInfo->mType == JOINT_HINGE2));

	vec3 lAxis1;
	vec3 lAxis2;
	{
		xform lTransform;
		GetBodyTransform(pBodyId, lTransform);
		vec3 lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2))
		{
			return (false);
		}
		deb_assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		deb_assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		const vec3 lDiff(lTransform.GetPosition()-lAnchor);
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
		vec3 lVelocity;
		GetBodyVelocity(pBodyId, lVelocity);
		pDiff.mValueVelocity = -(lAxis1 * lVelocity);
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mAngle1Velocity = -(lAxis2 * lVelocity);
		pDiff.mAngle2Velocity = -(lAxis1 * lVelocity);
	}

	{
		vec3 lAcceleration;
		GetBodyForce(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = -(lAxis1 * lAcceleration);
		GetBodyTorque(pBodyId, lAcceleration);
		pDiff.mAngle1Acceleration = -(lAxis2 * lAcceleration);
		pDiff.mAngle2Acceleration = -(lAxis1 * lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::SetHinge2Diff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	LEPRA_DEBUG_CODE(JointInfo* lJointInfo = (JointInfo*)pJointId;)
	LEPRA_DEBUG_CODE(deb_assert(lJointInfo->mType == JOINT_HINGE2));

	vec3 lAxis1;
	vec3 lAxis2;
	{
		vec3 lAnchor;
		if (!GetAnchorPos(pJointId, lAnchor) || !GetAxis1(pJointId, lAxis1) || !GetAxis2(pJointId, lAxis2))
		{
			return (false);
		}
		deb_assert(lAxis1.GetLengthSquared() > 0.99f && lAxis1.GetLengthSquared() < 1.01f);
		deb_assert(lAxis2.GetLengthSquared() > 0.99f && lAxis2.GetLengthSquared() < 1.01f);
		const vec3 lDiff = lAxis1 * -pDiff.mValue;
		xform lTransform;
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
		vec3 lVelocity;
		GetBodyVelocity(pBodyId, lVelocity);
		// Drop suspension velocity along axis1.
		vec3 lAxisVelocity(lAxis1*(lAxis1*lVelocity));
		lVelocity -= lAxisVelocity;
		// Add suspension velocity.
		lVelocity += lAxis1 * -pDiff.mValueVelocity;
		SetBodyVelocity(pBodyId, lVelocity);

		vec3 lOriginalVelocity;
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
		vec3 lAcceleration;
		GetBodyForce(pBodyId, lAcceleration);
		// Drop suspension acceleration along axis1.
		vec3 lAxisAcceleration(lAxis1*(lAxis1*lAcceleration));
		lAcceleration -= lAxisAcceleration;
		// Add suspension acceleration.
		lAcceleration += lAxis1 * -pDiff.mValueAcceleration;
		SetBodyForce(pBodyId, lAcceleration);

		vec3 lOriginalAcceleration;
		GetBodyTorque(pBodyId, lOriginalAcceleration);
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
		SetBodyTorque(pBodyId, lAcceleration);
	}

	return (true);
}

bool PhysicsManagerODE::GetBallDiff(BodyID pBodyId, JointID pJointId, Joint3Diff& pDiff) const
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_BALL);

	{
		deb_assert(lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		quat lParentQ(lPQ);

		xform lTransform;
		GetBodyTransform(pBodyId, lTransform);
		const quat lQ = lTransform.GetOrientation()/lParentQ;
		lQ.GetEulerAngles(pDiff.mValue, pDiff.mAngle1, pDiff.mAngle2);
	}

	{
		vec3 lVelocity;
		GetBodyAngularVelocity(pBodyId, lVelocity);
		pDiff.mValueVelocity = lVelocity.x;
		pDiff.mAngle1Velocity = lVelocity.y;
		pDiff.mAngle2Velocity = lVelocity.z;
	}

	{
		vec3 lAcceleration;
		GetBodyTorque(pBodyId, lAcceleration);
		pDiff.mValueAcceleration = lAcceleration.x;
		pDiff.mAngle1Acceleration = lAcceleration.y;
		pDiff.mAngle2Acceleration = lAcceleration.z;
	}

	return (true);
}

bool PhysicsManagerODE::SetBallDiff(BodyID pBodyId, JointID pJointId, const Joint3Diff& pDiff)
{
	JointInfo* lJointInfo = (JointInfo*)pJointId;
	deb_assert(lJointInfo->mType == JOINT_BALL);

	{
		deb_assert(!lJointInfo->mJointID->node[1].body || lJointInfo->mJointID->node[1].body == ((Object*)pBodyId)->mBodyID);
		dxBody* lParentBody = lJointInfo->mJointID->node[0].body;
		const dReal* lPQ = ::dBodyGetQuaternion(lParentBody);
		quat lParentQ(lPQ);

		xform lTransform;
		GetBodyTransform(pBodyId, lTransform);
		quat lRelativeToParentQ(lParentQ/lTransform.GetOrientation());
		dVector3 lRawAnchor;
		::dJointGetBallAnchor2(lJointInfo->mJointID, lRawAnchor);
		vec3 lAnchor2(lRawAnchor);
		vec3 lPosition = lTransform.GetPosition()-lAnchor2;
		lPosition = lRelativeToParentQ*lPosition;	// Go to parent space.
		quat lRelativeFromParentQ;
		lRelativeFromParentQ.SetEulerAngles(pDiff.mValue, pDiff.mAngle1, pDiff.mAngle2);
		lPosition = lRelativeFromParentQ*lPosition;	// Go from parent to given child space.
		lTransform.SetOrientation(lRelativeFromParentQ*lParentQ);	// Set complete orientation.
		vec3 lAnchor;
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
		//SetBodyVelocity(pBodyId, vec3(0, 0, 0));
		//SetBodyVelocity(pBodyId, vec3(pDiff.mValueVelocity, pDiff.mAngle1Velocity, pDiff.mAngle2Velocity));
		SetBodyAngularVelocity(pBodyId, vec3(pDiff.mValueVelocity, pDiff.mAngle1Velocity, pDiff.mAngle2Velocity));
		//SetBodyAngularVelocity(pBodyId, vec3(0, 0, 0));
	}

	{
		// TODO: adjust linear acceleration.
		//SetBodyForce(pBodyId, vec3(0, 0, 0));
		//SetBodyForce(pBodyId, vec3(pDiff.mValueAcceleration, pDiff.mAngle1Acceleration, pDiff.mAngle2Acceleration));
		SetBodyTorque(pBodyId, vec3(pDiff.mValueAcceleration, pDiff.mAngle1Acceleration, pDiff.mAngle2Acceleration));
		//SetBodyTorque(pBodyId, vec3(0, 0, 0));
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

bool PhysicsManagerODE::GetAnchorPos(JointID pJointId, vec3& pAnchorPos) const
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

bool PhysicsManagerODE::GetAxis1(JointID pJointId, vec3& pAxis1) const
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

bool PhysicsManagerODE::GetAxis2(JointID pJointId, vec3& pAxis2) const
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
	case JOINT_HINGE:
		pAngleRate = dJointGetHingeAngleRate((*x)->mJointID);
		break;
	case JOINT_BALL:
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
			deb_assert(::dJointGetBody(lJoint->mJointID, 1) == lObject->mBodyID);
			const float lCurrentAngle = ::dJointGetHingeAngle(lJoint->mJointID);
			if (lCurrentAngle == pAngle)
			{
				return (true);
			}
			xform lTransform;
			GetBodyTransform(pBodyId, lTransform);
			vec3 lAxis;
			if (GetAxis1(pJointId, lAxis))
			{
				dVector3 lRawAnchor;
				::dJointGetHingeAnchor((*x)->mJointID, lRawAnchor);
				vec3 lAnchor(lRawAnchor);
				::dJointGetHingeAnchor2((*x)->mJointID, lRawAnchor);
				vec3 lAnchor2(lRawAnchor);
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
			deb_assert(::dJointGetBody(lJoint->mJointID, 1) == lObject->mBodyID);
			const float lCurrentAngle = ::dJointGetHinge2Angle1(lJoint->mJointID);
			if (lCurrentAngle == pAngle)
			{
				return (true);
			}
			xform lTransform;
			GetBodyTransform(pBodyId, lTransform);
			vec3 lAxis1;
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

bool PhysicsManagerODE::SetMotorMaxForce(JointID pJointId, float32 pMaxForce)
{
	JointTable::iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetMotorMaxForce() - Couldn't find joint %i!"), pJointId);
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
	else if (lJoint->mType == JOINT_SLIDER)
	{
		dJointSetSliderParam(lJoint->mJointID, dParamFMax, pMaxForce);
		dJointSetSliderParam(lJoint->mJointID, dParamFMax2, pMaxForce);
		return (true);
	}
	mLog.AError("SetMotorMaxForce() - Joint is not an angular motor!");
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

bool PhysicsManagerODE::SetMotorTarget(JointID pJointId, float32 pMaxForce, float32 pTargetVelocity)
{
	JointTable::const_iterator x = mJointTable.find((JointInfo*)pJointId);
	if (x == mJointTable.end())
	{
		mLog.Errorf(_T("SetMotorTarget() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_SLIDER)
	{
		mLog.AError("SetMotorTarget() - Joint is not an angular motor!");
		return (false);
	}

	::dJointSetSliderParam((*x)->mJointID, dParamFMax, pMaxForce);
	::dJointSetSliderParam((*x)->mJointID, dParamVel, pTargetVelocity);
	::dBodyEnable(::dJointGetBody((*x)->mJointID, 1));
	return (true);
}

bool PhysicsManagerODE::SetJointParams(JointID pJointId, float32 pLowStop, float32 pHighStop, float32 pBounce, int pExtraIndex)
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
			dJointSetSliderParam(lJointId, dParamBounce2, pBounce);
		}
		break;
		case JOINT_HINGE:
		{
			dJointSetHingeParam(lJointId, dParamLoStop, pLowStop);
			dJointSetHingeParam(lJointId, dParamHiStop, pHighStop);
			dJointSetHingeParam(lJointId, dParamBounce, pBounce);
			dJointSetHingeParam(lJointId, dParamStopERP, pBounce);
			dJointSetHingeParam(lJointId, dParamCFM, 1e-11f);
			dJointSetHingeParam(lJointId, dParamFudgeFactor, 0.01f);
		}
		break;
		case JOINT_HINGE2:
		{
			if (pExtraIndex == 0)
			{
				dJointSetHinge2Param(lJointId, dParamLoStop, pLowStop);
				dJointSetHinge2Param(lJointId, dParamHiStop, pHighStop);
			}
			else
			{
				dJointSetHinge2Param(lJointId, dParamLoStop2, pLowStop);
				dJointSetHinge2Param(lJointId, dParamHiStop2, pHighStop);
			}
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
		//mLog.Warningf(_T("GetJointParams() - Couldn't find joint %i!"), pJointId);
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
		mLog.Errorf(_T("SetSyspension() - Couldn't find joint %i!"), pJointId);
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
		mLog.Errorf(_T("GetSuspension() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	JointInfo* lJoint = *x;
	if (lJoint->mType == JOINT_HINGE2)
	{
		pErp = ::dJointGetHinge2Param(lJoint->mJointID, dParamSuspensionERP);
		pCfm = ::dJointGetHinge2Param(lJoint->mJointID, dParamSuspensionCFM);
		return (true);
	}
	mLog.AError("GetSuspension() - Joint is not a hinge-2!");
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
		mLog.Errorf(_T("GetSliderSpeed() - Couldn't find joint %i!"), pJointId);
		return (false);
	}

	if ((*x)->mType != JOINT_SLIDER)
	{
		mLog.AError("GetSliderSpeed() - Joint is not a slider!");
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

	::dJointAddSliderForce((*x)->mJointID, pForce);
	::dBodyEnable(::dJointGetBody((*x)->mJointID, 1));

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
	::dBodyEnable(::dJointGetBody((*x)->mJointID, 1));

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
	::dBodyEnable(::dJointGetBody((*x)->mJointID, 1));

	return (true);
}

void PhysicsManagerODE::AddForce(BodyID pBodyId, const vec3& pForce)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForce() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	if (((Object*)pBodyId)->mBodyID)
	{
		dBodyEnable(((Object*)pBodyId)->mBodyID);
		dBodyAddForce(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z);
	}
	else
	{
		mLog.Errorf(_T("AddForce() - Body %i is only geometry, not body!"), pBodyId);
	}
}

void PhysicsManagerODE::AddTorque(BodyID pBodyId, const vec3& pTorque)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddTorque() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddTorque(((Object*)pBodyId)->mBodyID, pTorque.x, pTorque.y, pTorque.z);
}

void PhysicsManagerODE::AddRelForce(BodyID pBodyId, const vec3& pForce)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForce() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForce(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z);
}

void PhysicsManagerODE::AddRelTorque(BodyID pBodyId, const vec3& pTorque)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelTorque() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelTorque(((Object*)pBodyId)->mBodyID, pTorque.x, pTorque.y, pTorque.z);
}

void PhysicsManagerODE::AddForceAtPos(BodyID pBodyId, const vec3& pForce,
									 const vec3& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForceAtPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForceAtPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::AddForceAtRelPos(BodyID pBodyId, const vec3& pForce,
										const vec3& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddForceAtRelPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddForceAtRelPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::AddRelForceAtPos(BodyID pBodyId, const vec3& pForce,
										const vec3& pPos)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("AddRelForceAtPos() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	dBodyEnable(((Object*)pBodyId)->mBodyID);
	dBodyAddRelForceAtPos(((Object*)pBodyId)->mBodyID, pForce.x, pForce.y, pForce.z, pPos.x, pPos.y, pPos.z);
}

void PhysicsManagerODE::AddRelForceAtRelPos(BodyID pBodyId, const vec3& pForce,
										   const vec3& pPos)
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

	::dBodySetMaxAngularSpeed(((Object*)pBodyId)->mBodyID, pMaxAngularSpeed);

	const dReal* lLVel = dBodyGetLinearVel(((Object*)pBodyId)->mBodyID);
	float32 lLSpeed = lLVel[0] * lLVel[0] + lLVel[1] * lLVel[1] + lLVel[2] * lLVel[2];
	if (lLSpeed > pMaxSpeed * pMaxSpeed)
	{
		float32 k = pMaxSpeed / sqrtf(lLSpeed);
		dBodySetLinearVel(((Object*)pBodyId)->mBodyID, lLVel[0] * k, lLVel[1] * k, lLVel[2] * k);
	}
}

void PhysicsManagerODE::EnableGravity(BodyID pBodyId, bool pEnable)
{
	if (((Object*)pBodyId)->mWorldID != mWorldID)
	{
		mLog.Errorf(_T("EnableGravity() - Body %i is not part of this world!"), pBodyId);
		return;
	}

	if (((Object*)pBodyId)->mBodyID)
	{
		::dBodySetGravityMode(((Object*)pBodyId)->mBodyID, pEnable? 1 : 0);
	}
}

void PhysicsManagerODE::SetGravity(const vec3& pGravity)
{
	dWorldSetGravity(mWorldID, pGravity.x, pGravity.y, pGravity.z);
}

vec3 PhysicsManagerODE::GetGravity() const
{
	dVector3 lGravity;
	::dWorldGetGravity(mWorldID, lGravity);
	return vec3(lGravity);
}

void PhysicsManagerODE::EnableCollideWithSelf(BodyID pBodyId, bool pEnable)
{
	ObjectTable::iterator x = mObjectTable.find((Object*)pBodyId);
	if (x != mObjectTable.end())
	{
		(*x)->mCollideWithSelf = pEnable;
	}
	else
	{
		mLog.Errorf(_T("EnableCollideWithSelf() - body %p is not part of this world!"), pBodyId);
		deb_assert(false);
		return;
	}
}

void PhysicsManagerODE::PreSteps()
{
	FlagMovingObjects();
}

void PhysicsManagerODE::StepAccurate(float32 pStepSize, bool pCollide)
{
	if (pStepSize > 0)
	{
		if (pCollide) dSpaceCollide(mSpaceID, this, CollisionCallback);
		dWorldStep(mWorldID, pStepSize);

		if (pCollide)
		{
			DoForceFeedback();
			dJointGroupEmpty(mContactJointGroupID);
		}
	}
}

void PhysicsManagerODE::StepFast(float32 pStepSize, bool pCollide)
{
	if (pStepSize > 0)
	{
		if (pCollide) dSpaceCollide(mSpaceID, this, CollisionCallback);
		dWorldQuickStep(mWorldID, pStepSize);
		
		if (pCollide)
		{
			DoForceFeedback();
			dJointGroupEmpty(mContactJointGroupID);
		}
	}
}

bool PhysicsManagerODE::IsColliding(int pForceFeedbackId)
{
	mNoteForceFeedbackId = pForceFeedbackId;
	mNoteIsCollided = false;
	dSpaceCollide(mSpaceID, this, CollisionNoteCallback);
	return mNoteIsCollided;
}

void PhysicsManagerODE::PostSteps()
{
	HandleMovableObjects();
}

void PhysicsManagerODE::DoForceFeedback()
{
	TriggerInfoList::iterator y;
	for (y = mTriggerInfoList.begin(); y != mTriggerInfoList.end(); ++y)
	{
		const TriggerInfo& lTriggerInfo = *y;
		mTriggerCallback->OnTrigger(lTriggerInfo.mTriggerId, lTriggerInfo.mTriggerListenerId, lTriggerInfo.mBodyListenerId, lTriggerInfo.mBodyId, lTriggerInfo.mPosition, lTriggerInfo.mNormal);
	}
	mTriggerInfoList.clear();

	JointList::iterator x;
	for (x = mFeedbackJointList.begin(); x != mFeedbackJointList.end(); ++x)
	{
		JointInfo* lJointInfo = *x;

		const bool lIsBody1Static = lJointInfo->IsBody1Static(this);
		const bool lIsBody2Static = lJointInfo->IsBody2Static(this);
		//const bool lOneIsDynamic = (!lIsBody1Static || !lIsBody2Static);
		if (lJointInfo->mListenerId1 != lJointInfo->mListenerId2 /*&& lOneIsDynamic*/)
		{
			const dJointFeedback* lFeedback = &lJointInfo->mFeedback;
			if (lJointInfo->mListenerId1 != 0)
			{
				mForceFeedbackCallback->OnForceApplied(
					lJointInfo->mListenerId1,
					lJointInfo->mListenerId2,
					lJointInfo->mBody1Id,
					lJointInfo->mBody2Id,
					vec3(lFeedback->f1),
					vec3(lFeedback->t1),
					lJointInfo->mPosition,
					lJointInfo->mRelativeVelocity);
			}
			if (lJointInfo->mListenerId2 != 0)
			{
				if (lIsBody1Static || lIsBody2Static)	// Only a single force/torque pair set?
				{
					mForceFeedbackCallback->OnForceApplied(
						lJointInfo->mListenerId2,
						lJointInfo->mListenerId1,
						lJointInfo->mBody2Id,
						lJointInfo->mBody1Id,
						vec3(lFeedback->f1),
						vec3(lFeedback->t1),
						lJointInfo->mPosition,
						lJointInfo->mRelativeVelocity);
				}
				else
				{
					mForceFeedbackCallback->OnForceApplied(
						lJointInfo->mListenerId2,
						lJointInfo->mListenerId1,
						lJointInfo->mBody2Id,
						lJointInfo->mBody1Id,
						vec3(lFeedback->f2),
						vec3(lFeedback->t2),
						lJointInfo->mPosition,
						lJointInfo->mRelativeVelocity);
				}
			}
		}
		RemoveJoint(lJointInfo);
	}
	mFeedbackJointList.clear();
}

void PhysicsManagerODE::CollisionCallback(void* pData, dGeomID pGeom1, dGeomID pGeom2)
{
	Object* lObject1 = (Object*)dGeomGetData(pGeom1);
	Object* lObject2 = (Object*)dGeomGetData(pGeom2);

	if ((lObject1->mForceFeedbackId && lObject1->mForceFeedbackId == lObject2->mForceFeedbackId) ||	// Same body.
		(lObject1->mTriggerListenerId && lObject2->mTriggerListenerId) ||	// Elevator platform trigger moves into down trigger.
		(lObject1->mForceFeedbackId && lObject1->mForceFeedbackId == lObject2->mTriggerListenerId) ||	// Trigger on self.
		(lObject2->mForceFeedbackId && lObject2->mForceFeedbackId == lObject1->mTriggerListenerId))	// Trigger on self.
	{
		if (!lObject1->mCollideWithSelf || !lObject2->mCollideWithSelf)
		{
			return;
		}
	}

	dBodyID lBody1 = ::dGeomGetBody(pGeom1);
	dBodyID lBody2 = ::dGeomGetBody(pGeom2);

	// Check if both bodies are static or disabled.
	if ((!lBody1 || !::dBodyIsEnabled(lBody1)) && (!lBody2 || !::dBodyIsEnabled(lBody2)))
	{
		if (!lObject1->mTriggerListenerId && !lObject2->mTriggerListenerId)
		{
			return;
		}
	}
	// Exit without doing anything if the two bodies are connected by a joint.
	if (AreBodiesConnectedExcluding(lBody1, lBody2, dJointTypeContact) != 0)
	{
		if (!lObject1->mCollideWithSelf || !lObject2->mCollideWithSelf)
		{
			return;
		}
	}

	dContact lContact[8];
	const int lTriggerContactPointCount = ::dCollide(pGeom1, pGeom2, 8, &lContact[0].geom, sizeof(lContact[0]));
	if (lTriggerContactPointCount <= 0)
	{
		// In AABB range (since call came here), but no real contact.
		return;
	}

	PhysicsManagerODE* lThis = (PhysicsManagerODE*)pData;
	if (lObject1->mTriggerListenerId != 0)	// Only trig, no force application.
	{
		vec3 lPosition(lContact[0].geom.pos);
		vec3 lNormal(lContact[0].geom.normal);
		lThis->mTriggerInfoList.push_back(TriggerInfo((BodyID)lObject1, lObject1->mTriggerListenerId, lObject2->mForceFeedbackId, (BodyID)lObject2, lPosition, lNormal));
		return;
	}
	if(lObject2->mTriggerListenerId != 0)	// Only trig, no force application.
	{
		vec3 lPosition(lContact[0].geom.pos);
		vec3 lNormal(lContact[0].geom.normal);
		lThis->mTriggerInfoList.push_back(TriggerInfo((BodyID)lObject2, lObject2->mTriggerListenerId, lObject1->mForceFeedbackId, (BodyID)lObject1, lPosition, lNormal));
		return;
	}

	// Bounce/slide when both objects are dynamic, non-trigger objects.
	{
		// Fetch force, will be used to scale friction (projected against surface normal).
		vec3 lPosition1 = lThis->GetBodyPosition((BodyID)lObject1);
		vec3 lPosition2 = lThis->GetBodyPosition((BodyID)lObject2);
		vec3 lLinearVelocity1;
		lThis->GetBodyVelocity((BodyID)lObject1, lLinearVelocity1);
		vec3 lLinearVelocity2;
		lThis->GetBodyVelocity((BodyID)lObject2, lLinearVelocity2);
		vec3 lAngularVelocity1;
		lThis->GetBodyAngularVelocity((BodyID)lObject1, lAngularVelocity1);
		vec3 lAngularVelocity2;
		lThis->GetBodyAngularVelocity((BodyID)lObject2, lAngularVelocity2);

		dMass lMass;
		float lMass1 = 1;
		if (lBody1)
		{
			::dBodyGetMass(lBody1, &lMass);
			lMass1 = lMass.mass;
		}
		float lMass2Propotions = 1;
		if (lBody2)
		{
			::dBodyGetMass(lBody2, &lMass);
			lMass2Propotions = lMass.mass / lMass1;
		}

		// Perform normal collision detection.
		for (int i = 0; i < lTriggerContactPointCount; i++)
		{
			dContact& lC = lContact[i];
			vec3 lPosition(lC.geom.pos);
			vec3 lSpin;
			const float lTotalFriction = ::fabs(lObject1->mFriction*lObject2->mFriction)+0.0001f;
			// Negative friction factor means simple friction model.
			if (lObject1->mFriction > 0 || lObject2->mFriction > 0)
			{
				lC.surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1 | dContactFDir1 | dContactBounce;

				const vec3 lNormal(lC.geom.normal);
				const vec3 lCollisionPoint(lC.geom.pos);
				const vec3 lDistance1(lCollisionPoint-lPosition1);
				const vec3 lDistance2(lCollisionPoint-lPosition2);
				const vec3 lAngularSurfaceVelocity1 = lAngularVelocity1.Cross(lDistance1);
				const vec3 lAngularSurfaceVelocity2 = lAngularVelocity2.Cross(lDistance2);
				const vec3 lSurfaceVelocity1 = -lLinearVelocity1.ProjectOntoPlane(lNormal) + lAngularSurfaceVelocity1;
				const vec3 lSurfaceVelocity2 = -lLinearVelocity2.ProjectOntoPlane(lNormal) + lAngularSurfaceVelocity1;
				lSpin = lSurfaceVelocity1-lSurfaceVelocity2;
				const float lRelativeVelocity = lSpin.GetLength();
				vec3 lSpinDirection(lSpin);
				if (lSpinDirection.GetLengthSquared() <= 1e-4)
				{
					vec3 lDummy;
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
				lC.surface.mu = lTotalFriction * 3 * lMass1 * lMass2Propotions;
			}
			lC.surface.bounce = (dReal)(lObject1->mBounce * lObject2->mBounce);
			lC.surface.bounce_vel = (dReal)0.000001;
			if (lC.surface.bounce < 1e-1f)
			{
				lC.surface.mode |= dContactSoftERP;
				lC.surface.soft_erp = lC.surface.bounce * 1e1f * mWorldErp;
			}
			if (lC.surface.bounce < 1e-7f)
			{
				lC.surface.mode |= dContactSoftCFM;
				lC.surface.soft_cfm = Math::Lerp(1e8f, 1.0f, lC.surface.bounce * 1e7f) * mWorldCfm;
			}

			if (lObject1->mForceFeedbackId != 0 ||
			   lObject2->mForceFeedbackId != 0)
			{
				// Create a joint whith feedback info.
				JointInfo* lJointInfo = lThis->mJointInfoAllocator.Alloc();
				lJointInfo->mJointID = dJointCreateContact(lThis->mWorldID, lThis->mContactJointGroupID, &lC);
				lJointInfo->mType = JOINT_CONTACT;
				lThis->mFeedbackJointList.push_back(lJointInfo);
				lJointInfo->mListenerId1 = lObject1->mForceFeedbackId;
				lJointInfo->mListenerId2 = lObject2->mForceFeedbackId;
				lJointInfo->mPosition = lPosition;
				lJointInfo->mRelativeVelocity = lSpin;

				dJointAttach(lJointInfo->mJointID, lBody1, lBody2);
				dJointSetFeedback(lJointInfo->mJointID, &lJointInfo->mFeedback);
				lJointInfo->mBody1Id = lObject1;
				lJointInfo->mBody2Id = lObject2;
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

void PhysicsManagerODE::CollisionNoteCallback(void* pData, dGeomID pGeom1, dGeomID pGeom2)
{
	Object* lObject1 = (Object*)dGeomGetData(pGeom1);
	Object* lObject2 = (Object*)dGeomGetData(pGeom2);

	if (!lObject1->mForceFeedbackId ||
		!lObject2->mForceFeedbackId ||
		lObject1->mForceFeedbackId == lObject2->mForceFeedbackId)	// One is trigger, or same body.
	{
		return;
	}

	PhysicsManagerODE* lThis = (PhysicsManagerODE*)pData;
	if (lObject1->mForceFeedbackId != lThis->mNoteForceFeedbackId &&
		lObject2->mForceFeedbackId != lThis->mNoteForceFeedbackId)	// Not observed body.
	{
		return;
	}

	dContact lContact[1];
	const int lTriggerContactPointCount = ::dCollide(pGeom1, pGeom2, 1, &lContact[0].geom, sizeof(lContact[0]));
	lThis->mNoteIsCollided |= (lTriggerContactPointCount > 0);	// False means in AABB range (since call came here), but no real contact.
}



const PhysicsManager::BodySet& PhysicsManagerODE::GetIdledBodies()
{
	mAutoDisabledObjectSet.clear();
	ObjectTable::iterator x = mObjectTable.begin();
	for (; x != mObjectTable.end(); ++x)
	{
		Object* lObject = *x;
		if (lObject->mDidStop)
		{
			mAutoDisabledObjectSet.insert((BodyID)lObject);
		}
	}
	return (mAutoDisabledObjectSet);
}



void PhysicsManagerODE::FlagMovingObjects()
{
	ObjectTable::iterator x = mObjectTable.begin();
	for (; x != mObjectTable.end(); ++x)
	{
		Object* lObject = *x;
		if (lObject->mBodyID && lObject->mIsRoot && ::dBodyIsEnabled(lObject->mBodyID))
		{
			lObject->mDidStop = true;
		}
	}
}

void PhysicsManagerODE::HandleMovableObjects()
{
	ObjectTable::iterator x = mObjectTable.begin();
	for (; x != mObjectTable.end(); ++x)
	{
		Object* lObject = (Object*)(*x);
		if (lObject->mBodyID && ::dBodyIsEnabled(lObject->mBodyID))
		{
			lObject->mDidStop = false;
			NormalizeRotation(lObject);
		}
	}
}

void PhysicsManagerODE::NormalizeRotation(BodyID pObject)
{
	Object* lObject = (Object*)pObject;
	if (!lObject->mIsRotational && (lObject->mHasMassChildren || lObject->mBodyID->geom->type == dBoxClass))
	{
		vec3 lVelocity;
		GetBodyAngularVelocity(lObject, lVelocity);
		const float lMaxAngularVelocity = 12.0f;
		if (lVelocity.GetLength() > lMaxAngularVelocity)
		{
			lVelocity.Normalize(lMaxAngularVelocity);
			SetBodyAngularVelocity(lObject, lVelocity);
			SetBodyTorque(lObject, vec3());
		}
	}
}



void PhysicsManagerODE::RayPickCallback(void* pDataPtr, dGeomID o1, dGeomID o2)
{
	void** lData = (void**)pDataPtr;
	int* lForceFeedbackIds = (int*)lData[0];
	vec3* lPositions = (vec3*)lData[1];
	int* lHits = (int*)lData[2];
	const int lMaxBodies = *(int*)lData[3];
	if (*lHits >= lMaxBodies)
	{
		return;
	}
	dContactGeom lContact[4];
	const int lCollisions = ::dCollide(o1, o2, 1, &lContact[0], sizeof(lContact[0]));
	if (lCollisions)
	{
		Object* lObject = (Object*)::dGeomGetData(o1);
		lForceFeedbackIds[*lHits] = lObject->mForceFeedbackId;
		lPositions[*lHits] = vec3(lContact[0].pos);
		(*lHits)++;
	}
}



float PhysicsManagerODE::mWorldErp;
float PhysicsManagerODE::mWorldCfm;
loginstance(PHYSICS, PhysicsManagerODE);



}
