
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ChunkyBoneGeometry.h"
#include <assert.h>
#include "../../Lepra/Include/CyclicArray.h"
#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/ChunkyPhysics.h"



namespace TBC
{



const int PARAM_SPRING_CONSTANT	= 0;
const int PARAM_SPRING_DAMPING	= 1;
const int PARAM_EULER_THETA	= 2;
const int PARAM_EULER_PHI	= 3;
const int PARAM_LOW_STOP	= 4;
const int PARAM_HIGH_STOP	= 5;
const int PARAM_OFFSET_X	= 6;
const int PARAM_OFFSET_Y	= 7;
const int PARAM_OFFSET_Z	= 8;
const int PARAM_IMPACT_FACTOR	= 9;



ChunkyBoneGeometry::ChunkyBoneGeometry(const BodyData& pBodyData):
	mBodyData(pBodyData),
	mJointId(INVALID_JOINT),
	mBodyId(INVALID_BODY),
	mTriggerId(INVALID_TRIGGER),
	mExtraData(0)
{
	AddConnectorType(pBodyData.mConnectorType);
}

ChunkyBoneGeometry::~ChunkyBoneGeometry()
{
	// Ensure all resources has been released prior to delete.
	assert(mJointId == INVALID_JOINT && mBodyId == INVALID_BODY && mTriggerId == INVALID_TRIGGER);
}

ChunkyBoneGeometry* ChunkyBoneGeometry::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	if (pByteCount < sizeof(uint32))
	{
		mLog.AError("Could not load; very small data size.");
		assert(false);
		return (0);
	}

	ChunkyBoneGeometry* lGeometry = 0;
	const uint32* lData = (const uint32*)pData;
	BodyData lBodyData(0, 0, 0);
	switch (Endian::BigToHost(lData[0]))
	{
		case GEOMETRY_CAPSULE:	lGeometry = new ChunkyBoneCapsule(lBodyData);	break;
		case GEOMETRY_SPHERE:	lGeometry = new ChunkyBoneSphere(lBodyData);	break;
		case GEOMETRY_BOX:	lGeometry = new ChunkyBoneBox(lBodyData);	break;
		case GEOMETRY_MESH:	lGeometry = new ChunkyBoneMesh(lBodyData);	break;
	}
	if (lGeometry)
	{
		if (pByteCount == lGeometry->GetChunkySize(lData))
		{
			lGeometry->LoadChunkyData(pStructure, lData);
			assert(lGeometry->GetChunkySize() == pByteCount);
		}
		else
		{
			mLog.AError("Could not load; wrong data size.");
			assert(false);
			delete (lGeometry);
			lGeometry = 0;
		}
	}
	return (lGeometry);
}

bool ChunkyBoneGeometry::CreateJoint(ChunkyPhysics* pStructure, PhysicsManager* pPhysics, unsigned pPhysicsFps)
{
	bool lOk = false;
	if (mBodyData.mParent)
	{
		if (GetBoneType() == BONE_POSITION)
		{
			// Need not do jack. It's not a physical object.
			lOk = true;
		}
		else if (GetTriggerId())
		{
			lOk = pPhysics->Attach(GetTriggerId(), mBodyData.mParent->GetBodyId());
		}
		else if (mBodyData.mJointType == JOINT_EXCLUDE)
		{
			lOk = pPhysics->Attach(GetBodyId(), mBodyData.mParent->GetBodyId());
		}
		else if (mBodyData.mJointType == JOINT_SUSPEND_HINGE || mBodyData.mJointType == JOINT_HINGE2)
		{
			// Calculate axis from given euler angles.
			Vector3DF lSuspensionAxis(-1, 0, 0);
			Vector3DF lHingeAxis(0, 0, 1);
			QuaternionF lRotator;
			lRotator.SetEulerAngles(mBodyData.mParameter[PARAM_EULER_THETA], 0, mBodyData.mParameter[PARAM_EULER_PHI]);
			lSuspensionAxis = lRotator*lSuspensionAxis;
			lHingeAxis = lRotator*lHingeAxis;

			mJointId = pPhysics->CreateHinge2Joint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), pStructure->GetTransformation(this).GetPosition(),
				lSuspensionAxis, lHingeAxis);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], 0);
			pPhysics->SetSuspension(mJointId, 1/(float)pPhysicsFps, mBodyData.mParameter[PARAM_SPRING_CONSTANT],
				mBodyData.mParameter[PARAM_SPRING_DAMPING]);
			pPhysics->SetAngularMotorRoll(mJointId, 0, 0);
			pPhysics->SetAngularMotorTurn(mJointId, 0, 0);
			lOk = true;
		}
		else if (mBodyData.mJointType == JOINT_HINGE)
		{
			// Calculate axis from given euler angles.
			Vector3DF lHingeAxis(0, 0, 1);
			QuaternionF lHingeRotator;
			lHingeRotator.SetEulerAngles(mBodyData.mParameter[PARAM_EULER_THETA], 0, mBodyData.mParameter[PARAM_EULER_PHI]);
			lHingeAxis = lHingeRotator*lHingeAxis;

			const TransformationF& lBodyTransform = pStructure->GetTransformation(this);
			const Vector3DF lAnchor = lBodyTransform.GetPosition() +
				Vector3DF(mBodyData.mParameter[PARAM_OFFSET_X], mBodyData.mParameter[PARAM_OFFSET_Y], mBodyData.mParameter[PARAM_OFFSET_Z]);
			mJointId = pPhysics->CreateHingeJoint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), lAnchor, lHingeAxis);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], mBodyData.mBounce);
			pPhysics->SetAngularMotorTurn(mJointId, 0, 0);
			//pPhysics->GetAxis1(mJointId, lHingeAxis);
			lOk = true;
		}
		else if (mBodyData.mJointType == JOINT_SLIDER)
		{
			// Calculate axis from given euler angles.
			Vector3DF lAxis(0, 0, 1);
			QuaternionF lRotator;
			lRotator.SetEulerAngles(mBodyData.mParameter[PARAM_EULER_THETA], 0, mBodyData.mParameter[PARAM_EULER_PHI]);
			lAxis = lRotator*lAxis;

			mJointId = pPhysics->CreateSliderJoint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), lAxis);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], 0);
			pPhysics->SetMotorTarget(mJointId, 0, 0);
			lOk = true;
		}
		else if (mBodyData.mJointType == JOINT_UNIVERSAL)
		{
			// Calculate axis from given euler angles.
			Vector3DF lAxis1(0, 0, 1);
			Vector3DF lAxis2(0, 1, 0);
			QuaternionF lRotator;
			lRotator.SetEulerAngles(mBodyData.mParameter[PARAM_EULER_THETA], 0, mBodyData.mParameter[PARAM_EULER_PHI]);
			lAxis1 = lRotator*lAxis1;
			lAxis2 = lRotator*lAxis2;

			const TransformationF& lBodyTransform = pStructure->GetTransformation(this);
			const Vector3DF lAnchor = lBodyTransform.GetPosition() +
				Vector3DF(mBodyData.mParameter[PARAM_OFFSET_X], mBodyData.mParameter[PARAM_OFFSET_Y], mBodyData.mParameter[PARAM_OFFSET_Z]);
			mJointId = pPhysics->CreateUniversalJoint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), lAnchor, lAxis1, lAxis2);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], 0);
			/*pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], 0);
			pPhysics->SetSuspension(mJointId, 1/(float)pPhysicsFps, mBodyData.mParameter[0],
				mBodyData.mParameter[1]);
			pPhysics->SetAngularMotorRoll(mJointId, 0, 0);
			pPhysics->SetAngularMotorTurn(mJointId, 0, 0);*/
			lOk = true;
		}
		else if (mBodyData.mJointType == JOINT_BALL)
		{
			const TransformationF& lBodyTransform = pStructure->GetTransformation(this);
			const Vector3DF lAnchor = lBodyTransform.GetPosition() +
				Vector3DF(mBodyData.mParameter[PARAM_OFFSET_X], mBodyData.mParameter[PARAM_OFFSET_Y], mBodyData.mParameter[PARAM_OFFSET_Z]);
			mJointId = pPhysics->CreateBallJoint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), lAnchor);
			/*pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], 0);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[PARAM_LOW_STOP], mBodyData.mParameter[PARAM_HIGH_STOP], 0);
			pPhysics->SetSuspension(mJointId, 1/(float)pPhysicsFps, mBodyData.mParameter[0],
				mBodyData.mParameter[1]);
			pPhysics->SetAngularMotorRoll(mJointId, 0, 0);
			pPhysics->SetAngularMotorTurn(mJointId, 0, 0);*/
			lOk = true;
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		assert(mBodyData.mJointType == JOINT_EXCLUDE);
		lOk = true;
	}
	assert(lOk);
	return (lOk);
}

void ChunkyBoneGeometry::RemovePhysics(PhysicsManager* pPhysics)
{
	if (mJointId != INVALID_JOINT)
	{
		if (pPhysics)
		{
			pPhysics->DeleteJoint(mJointId);
		}
		mJointId = INVALID_JOINT;
	}
	if (mBodyId != INVALID_BODY)
	{
		if (pPhysics)
		{
			pPhysics->DeleteBody(mBodyId);
		}
		mBodyId = INVALID_BODY;
	}
	if (mTriggerId != INVALID_TRIGGER)
	{
		if (pPhysics)
		{
			pPhysics->DeleteTrigger(mTriggerId);
		}
		mTriggerId = INVALID_TRIGGER;
	}
}



float ChunkyBoneGeometry::GetMass() const
{
	return mBodyData.mMass;
}

ChunkyBoneGeometry* ChunkyBoneGeometry::GetParent() const
{
	return (mBodyData.mParent);
}

ChunkyBoneGeometry::JointType ChunkyBoneGeometry::GetJointType() const
{
	return (mBodyData.mJointType);
}

ChunkyBoneGeometry::BoneType ChunkyBoneGeometry::GetBoneType() const
{
	assert(mBodyData.mBoneType >= BONE_BODY && mBodyData.mBoneType <= BONE_POSITION);
	return (mBodyData.mBoneType);
}

PhysicsManager::JointID ChunkyBoneGeometry::GetJointId() const
{
	return (mJointId);
}

void ChunkyBoneGeometry::ResetJointId()
{
	mJointId = INVALID_JOINT;
}

PhysicsManager::BodyID ChunkyBoneGeometry::GetBodyId() const
{
	return (mBodyId);
}

void ChunkyBoneGeometry::ResetBodyId()
{
	mBodyId = INVALID_BODY;
}

PhysicsManager::TriggerID ChunkyBoneGeometry::GetTriggerId() const
{
	return (mTriggerId);
}

bool ChunkyBoneGeometry::IsConnectorType(ConnectorType pType) const
{
	return (std::find(mConnectorArray.begin(), mConnectorArray.end(), pType) != mConnectorArray.end());
}

void ChunkyBoneGeometry::AddConnectorType(ConnectorType pType)
{
	if (pType != CONNECT_NONE)
	{
		mConnectorArray.push_back(pType);
	}
}

float ChunkyBoneGeometry::GetImpactFactor() const
{
	return mBodyData.mParameter[PARAM_IMPACT_FACTOR];
}

const str& ChunkyBoneGeometry::GetMaterial() const
{
	return mMaterial;
}



float ChunkyBoneGeometry::GetExtraData() const
{
	return (mExtraData);
}

void ChunkyBoneGeometry::SetExtraData(float pExtraData)
{
	mExtraData = pExtraData;
}



unsigned ChunkyBoneGeometry::GetChunkySize(const void* pData) const
{
	unsigned lSize = (unsigned)(sizeof(int32)*8 + sizeof(mBodyData.mParameter) +
		sizeof(int32) + sizeof(int32)*mConnectorArray.size());

	if (pData && mConnectorArray.empty() && mMaterial.empty())	// Shouldn't go here if we have something in RAM already.
	{
		const uint32* lData = (const uint32*)pData;
		unsigned x = 8 + LEPRA_ARRAY_COUNT(mBodyData.mParameter);
		const unsigned lConnectorSize = Endian::BigToHost(lData[x]) * sizeof(ConnectorType);
		lSize += lConnectorSize;
		x += 1 + lConnectorSize/sizeof(int32);
		lSize += PackerUnicodeString::UnpackRaw(0, (const uint8*)&lData[x], 100);
	}
	else
	{
		lSize += PackerUnicodeString::Pack(0, wstrutil::Encode(mMaterial));
	}
	return (lSize);
}

void ChunkyBoneGeometry::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	uint32* lData = (uint32*)pData;
	lData[0] = Endian::HostToBig(GetGeometryType());
	lData[1] = Endian::HostToBigF(mBodyData.mMass);
	lData[2] = Endian::HostToBigF(mBodyData.mFriction);
	lData[3] = Endian::HostToBigF(mBodyData.mBounce);
	lData[4] = mBodyData.mParent? Endian::HostToBig(pStructure->GetIndex(mBodyData.mParent)) : (unsigned)-1;
	lData[5] = Endian::HostToBig(mBodyData.mJointType);
	lData[6] = Endian::HostToBig(mBodyData.mIsAffectedByGravity? 1 : 0);
	lData[7] = Endian::HostToBig(mBodyData.mBoneType);
	int y = 8;
	for (int x = 0; (size_t)x < LEPRA_ARRAY_COUNT(mBodyData.mParameter); ++x)
	{
		lData[y++] = Endian::HostToBigF(mBodyData.mParameter[x]);
	}
	const int lConnectorTypes = (int)mConnectorArray.size();
	lData[y++] = Endian::HostToBig(lConnectorTypes);
	for (int x = 0; x < lConnectorTypes; ++x)
	{
		lData[y++] = Endian::HostToBig(mConnectorArray[x]);
	}
	PackerUnicodeString::Pack((uint8*)&lData[y], wstrutil::Encode(mMaterial));
}

void ChunkyBoneGeometry::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const uint32* lData = (const uint32*)pData;

	assert((GeometryType)Endian::BigToHost(lData[0]) == GetGeometryType());
	mBodyData.mMass = Endian::BigToHostF(lData[1]);
	mBodyData.mFriction = Endian::BigToHostF(lData[2]);
	mBodyData.mBounce = Endian::BigToHostF(lData[3]);
	int lParentIndex = Endian::BigToHost(lData[4]);
	mBodyData.mParent = (lParentIndex < 0)? 0 : pStructure->GetBoneGeometry(lParentIndex);
	mBodyData.mJointType = (JointType)Endian::BigToHost(lData[5]);
	mBodyData.mIsAffectedByGravity = Endian::BigToHost(lData[6])? true : false;
	mBodyData.mBoneType = (BoneType)Endian::BigToHost(lData[7]);
	int y = 8;
	for (int x = 0; (size_t)x < LEPRA_ARRAY_COUNT(mBodyData.mParameter); ++x)
	{
		mBodyData.mParameter[x] = Endian::BigToHostF(lData[y++]);
	}
	const int lConnectorTypes = Endian::BigToHost(lData[y++]);
	for (int x = 0; x < lConnectorTypes; ++x)
	{
		mConnectorArray.push_back((ConnectorType)Endian::BigToHost(lData[y++]));
	}
	PackerUnicodeString::Unpack(mMaterial, (uint8*)&lData[y], 100);
}



ChunkyBoneCapsule::ChunkyBoneCapsule(const BodyData& pBodyData):
	Parent(pBodyData),
	mRadius(0),
	mLength(0)
{
}

bool ChunkyBoneCapsule::CreateBody(PhysicsManager* pPhysics, bool pIsRoot,
	PhysicsManager::ForceFeedbackListener* pForceListener, PhysicsManager::BodyType pType,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateCapsule(pIsRoot, pTransform, mBodyData.mMass, mRadius, mLength, pType,
		mBodyData.mFriction, mBodyData.mBounce, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneCapsule::CreateTrigger(PhysicsManager* pPhysics, PhysicsManager::TriggerListener* pTrigListener,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateCapsuleTrigger(pTransform, mRadius, mLength, pTrigListener);
	return (mTriggerId != INVALID_TRIGGER);
}

unsigned ChunkyBoneCapsule::GetChunkySize(const void* pData) const
{
	return (Parent::GetChunkySize(pData) + sizeof(float32)*2);
}

void ChunkyBoneCapsule::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	uint32* lData = (uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	lData[0] = Endian::HostToBigF(mRadius);
	lData[1] = Endian::HostToBigF(mLength);
}

Vector3DF ChunkyBoneCapsule::GetShapeSize() const
{
	return (Vector3DF(mRadius*2, mRadius*2, mLength+mRadius*2));
}

void ChunkyBoneCapsule::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const uint32* lData = (const uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mRadius = Endian::BigToHostF(lData[0]);
	mLength = Endian::BigToHostF(lData[1]);
}

ChunkyBoneGeometry::GeometryType ChunkyBoneCapsule::GetGeometryType() const
{
	return (GEOMETRY_CAPSULE);
}



ChunkyBoneSphere::ChunkyBoneSphere(const BodyData& pBodyData):
	Parent(pBodyData),
	mRadius(0)
{
}

bool ChunkyBoneSphere::CreateBody(PhysicsManager* pPhysics, bool pIsRoot,
	PhysicsManager::ForceFeedbackListener* pForceListener, PhysicsManager::BodyType pType,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateSphere(pIsRoot, pTransform, mBodyData.mMass, mRadius, pType, mBodyData.mFriction,
		mBodyData.mBounce, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneSphere::CreateTrigger(PhysicsManager* pPhysics, PhysicsManager::TriggerListener* pTrigListener,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateSphereTrigger(pTransform, mRadius, pTrigListener);
	return (mTriggerId != INVALID_TRIGGER);
}

unsigned ChunkyBoneSphere::GetChunkySize(const void* pData) const
{
	return (Parent::GetChunkySize(pData) + sizeof(float32));
}

void ChunkyBoneSphere::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	uint32* lData = (uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	lData[0] = Endian::HostToBigF(mRadius);
}

Vector3DF ChunkyBoneSphere::GetShapeSize() const
{
	return (Vector3DF(mRadius*2, mRadius*2, mRadius*2));
}

void ChunkyBoneSphere::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const uint32* lData = (const uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mRadius = Endian::BigToHostF(lData[0]);
}

ChunkyBoneGeometry::GeometryType ChunkyBoneSphere::GetGeometryType() const
{
	return (GEOMETRY_SPHERE);
}



ChunkyBoneBox::ChunkyBoneBox(const BodyData& pBodyData):
	Parent(pBodyData)
{
}

bool ChunkyBoneBox::CreateBody(PhysicsManager* pPhysics, bool pIsRoot,
	PhysicsManager::ForceFeedbackListener* pForceListener, PhysicsManager::BodyType pType,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateBox(pIsRoot, pTransform, mBodyData.mMass, mSize, pType, mBodyData.mFriction,
		mBodyData.mBounce, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneBox::CreateTrigger(PhysicsManager* pPhysics, PhysicsManager::TriggerListener* pTrigListener,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateBoxTrigger(pTransform, mSize, pTrigListener);
	return (mTriggerId != INVALID_TRIGGER);
}

unsigned ChunkyBoneBox::GetChunkySize(const void* pData) const
{
	return (Parent::GetChunkySize(pData) + sizeof(float32)*3);
}

void ChunkyBoneBox::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	uint32* lData = (uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	lData[0] = Endian::HostToBigF(mSize.x);
	lData[1] = Endian::HostToBigF(mSize.y);
	lData[2] = Endian::HostToBigF(mSize.z);
}

Vector3DF ChunkyBoneBox::GetShapeSize() const
{
	return (mSize);
}

void ChunkyBoneBox::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const uint32* lData = (const uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mSize.x = Endian::BigToHostF(lData[0]);
	mSize.y = Endian::BigToHostF(lData[1]);
	mSize.z = Endian::BigToHostF(lData[2]);
}

ChunkyBoneGeometry::GeometryType ChunkyBoneBox::GetGeometryType() const
{
	return (GEOMETRY_BOX);
}



ChunkyBoneMesh::ChunkyBoneMesh(const BodyData& pBodyData):
	Parent(pBodyData),
	mVertexCount(0),
	mVertices(0),
	mTriangleCount(0),
	mIndices(0)
{
}

ChunkyBoneMesh::~ChunkyBoneMesh()
{
	Clear();
}

bool ChunkyBoneMesh::CreateBody(PhysicsManager* pPhysics, bool pIsRoot,
	PhysicsManager::ForceFeedbackListener* pForceListener, PhysicsManager::BodyType,
	const TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateTriMesh(pIsRoot, mVertexCount, mVertices, mTriangleCount, mIndices,
		pTransform, mBodyData.mFriction, mBodyData.mBounce, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneMesh::CreateTrigger(PhysicsManager*, PhysicsManager::TriggerListener*, const TransformationF&)
{
	assert(false);
	return (false);
}

unsigned ChunkyBoneMesh::GetChunkySize(const void* pData) const
{
	unsigned lSize = Parent::GetChunkySize(pData);
	uint32 lVertexCount;
	uint32 lTriangleCount;
	if (mVertexCount)	// Checking size when already loaded?
	{
		lVertexCount = mVertexCount;
		lTriangleCount = mTriangleCount;
	}
	else
	{
		const uint32* lData = (const uint32*)&((const uint8*)pData)[lSize];
		lVertexCount = Endian::BigToHost(lData[0]);
		lTriangleCount = Endian::BigToHost(lData[1]);
	}
	lSize += sizeof(mVertexCount)*2 + lVertexCount*sizeof(mVertices[0])*3 +
		lTriangleCount*sizeof(mIndices[0])*3;
	return (lSize);
}

void ChunkyBoneMesh::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	uint32* lData = (uint32*)&((char*)pData)[Parent::GetChunkySize()];
	lData[0] = Endian::HostToBig(mVertexCount);
	lData[1] = Endian::HostToBig(mTriangleCount);
	uint32 lBase = 2;
	uint32 x;
	for (x = 0; x < mVertexCount*3; ++x)
	{
		lData[lBase+x] = Endian::HostToBigF(mVertices[x]);
	}
	lBase += x;
	for (x = 0; x < mTriangleCount*3; ++x)
	{
		lData[lBase+x] = Endian::HostToBig(mIndices[x]);
	}
}

Vector3DF ChunkyBoneMesh::GetShapeSize() const
{
	return (Vector3DF(10,10,10));	// Implement if you want to be able to debug mesh EXTENTS. Doesn't seem very interesting...
}

void ChunkyBoneMesh::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const uint32* lData = (const uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mVertexCount = Endian::BigToHost(lData[0]);
	mTriangleCount = Endian::BigToHost(lData[1]);
	assert(!mVertices && !mIndices);
	mVertices = new float[mVertexCount*3];
	mIndices = new uint32[mTriangleCount*3];
	uint32 lBase = 2;
	uint32 x;
	for (x = 0; x < mVertexCount*3; ++x)
	{
		mVertices[x] = Endian::BigToHostF(lData[lBase+x]);
	}
	lBase += x;
	for (x = 0; x < mTriangleCount*3; ++x)
	{
		mIndices[x] = Endian::BigToHost(lData[lBase+x]);
	}
}

void ChunkyBoneMesh::Clear()
{
	mVertexCount = 0;
	delete (mVertices);
	mVertices = 0;
	mTriangleCount = 0;
	delete (mIndices);
	mIndices = 0;
}

ChunkyBoneGeometry::GeometryType ChunkyBoneMesh::GetGeometryType() const
{
	return (GEOMETRY_MESH);
}



LOG_CLASS_DEFINE(PHYSICS, ChunkyBoneGeometry);



}
