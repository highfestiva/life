
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyStructure.h"



namespace TBC
{



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

bool ChunkyBoneGeometry::CreateJoint(ChunkyStructure* pStructure, PhysicsEngine* pPhysics)
{
	bool lOk = false;
	if (mBodyData.mParent)
	{
		if (mBodyData.mJointType == TYPE_EXCLUDE)
		{
			lOk = pPhysics->Attach(GetBodyId(), mBodyData.mParent->GetBodyId());
		}
		else if (mBodyData.mJointType == TYPE_SUSPEND_HINGE || mBodyData.mJointType == TYPE_HINGE2)
		{
			// Calculate axis from given euler angles.
			Lepra::Vector3DF lSuspensionAxis(-1, 0, 0);
			Lepra::Vector3DF lHingeAxis(0, 0, 1);
			Lepra::QuaternionF lRotator;
			lRotator.SetEulerAngles(mBodyData.mParameter[3], 0, mBodyData.mParameter[4]);
			lSuspensionAxis = lRotator*lSuspensionAxis;
			lHingeAxis = lRotator*lHingeAxis;

			mJointId = pPhysics->CreateHinge2Joint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), pStructure->GetTransformation(this).GetPosition(),
				lSuspensionAxis, lHingeAxis);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[5], mBodyData.mParameter[6], 0);
			pPhysics->SetSuspension(mJointId, mBodyData.mParameter[0], mBodyData.mParameter[1],
				mBodyData.mParameter[2]);
			pPhysics->SetAngularMotorRoll(mJointId, 0, 0);
			pPhysics->SetAngularMotorTurn(mJointId, 0, 0);
			lOk = true;
		}
		else if (mBodyData.mJointType == TYPE_HINGE)
		{
			// Calculate axis from given euler angles.
			Lepra::Vector3DF lHingeAxis(0, 0, 1);
			Lepra::QuaternionF lHingeRotator;
			lHingeRotator.SetEulerAngles(mBodyData.mParameter[3], 0, mBodyData.mParameter[4]);
			lHingeAxis = lHingeRotator*lHingeAxis;

			Lepra::Vector3DF lAnchor;
			lAnchor.x = mBodyData.mParameter[7];
			lAnchor.y = mBodyData.mParameter[8];
			lAnchor.z = mBodyData.mParameter[9];
			mJointId = pPhysics->CreateHingeJoint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), lAnchor, lHingeAxis);
			pPhysics->SetJointParams(mJointId, mBodyData.mParameter[5], mBodyData.mParameter[6], 0);
			pPhysics->SetAngularMotorTurn(mJointId, 0, 0);
			lOk = true;
		}
		else if (mBodyData.mJointType == TYPE_UNIVERSAL)
		{
			// Calculate axis from given euler angles.
			Lepra::Vector3DF lAxis1(0, 0, 1);
			Lepra::Vector3DF lAxis2(0, 1, 0);
			Lepra::QuaternionF lRotator;
			lRotator.SetEulerAngles(mBodyData.mParameter[3], 0, mBodyData.mParameter[4]);
			lAxis1 = lRotator*lAxis1;
			lAxis2 = lRotator*lAxis2;

			Lepra::Vector3DF lAnchor;
			lAnchor.x = mBodyData.mParameter[7];
			lAnchor.y = mBodyData.mParameter[8];
			lAnchor.z = mBodyData.mParameter[9];
			mJointId = pPhysics->CreateUniversalJoint(mBodyData.mParent->GetBodyId(),
				GetBodyId(), lAnchor, lAxis1, lAxis2);
			/*pPhysics->SetJointParams(mJointId, mBodyData.mParameter[5], mBodyData.mParameter[6], 0);
			pPhysics->SetSuspension(mJointId, mBodyData.mParameter[0], mBodyData.mParameter[1],
				mBodyData.mParameter[2]);
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
		assert(mBodyData.mJointType == TYPE_EXCLUDE);
		lOk = true;
	}
	assert(lOk);
	return (lOk);
}

void ChunkyBoneGeometry::RemovePhysics(PhysicsEngine* pPhysics)
{
	if (mJointId != INVALID_JOINT)
	{
		pPhysics->DeleteJoint(mJointId);
		mJointId = INVALID_JOINT;
	}
	if (mBodyId != INVALID_BODY)
	{
		pPhysics->DeleteBody(mBodyId);
		mBodyId = INVALID_BODY;
	}
	if (mTriggerId != INVALID_TRIGGER)
	{
		pPhysics->DeleteTrigger(mTriggerId);
		mTriggerId = INVALID_TRIGGER;
	}
}



ChunkyBoneGeometry* ChunkyBoneGeometry::GetParent() const
{
	return (mBodyData.mParent);
}

ChunkyBoneGeometry::JointType ChunkyBoneGeometry::GetJointType() const
{
	return (mBodyData.mJointType);
}

PhysicsEngine::JointID ChunkyBoneGeometry::GetJointId() const
{
	return (mJointId);
}

PhysicsEngine::BodyID ChunkyBoneGeometry::GetBodyId() const
{
	return (mBodyId);
}

PhysicsEngine::TriggerID ChunkyBoneGeometry::GetTriggerId() const
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



float ChunkyBoneGeometry::GetExtraData() const
{
	return (mExtraData);
}

void ChunkyBoneGeometry::SetExtraData(float pExtraData)
{
	mExtraData = pExtraData;
}



unsigned ChunkyBoneGeometry::GetChunkySize() const
{
	return ((unsigned)(sizeof(Lepra::int32)*6 + sizeof(mBodyData.mParameter) +
		sizeof(Lepra::int32) + sizeof(Lepra::int32)*mConnectorArray.size()));
}

void ChunkyBoneGeometry::SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const
{
	Lepra::uint32* lData = (Lepra::uint32*)pData;
	lData[0] = Lepra::Endian::HostToBigF(mBodyData.mMass);
	lData[1] = Lepra::Endian::HostToBigF(mBodyData.mFriction);
	lData[2] = Lepra::Endian::HostToBigF(mBodyData.mBounce);
	lData[3] = Lepra::Endian::HostToBig(pStructure->GetIndex(mBodyData.mParent));
	lData[4] = Lepra::Endian::HostToBig(mBodyData.mJointType);
	lData[5] = Lepra::Endian::HostToBig(mBodyData.mIsAffectedByGravity? 1 : 0);
	int x;
	for (x = 0; x < sizeof(mBodyData.mParameter)/sizeof(mBodyData.mParameter[0]); ++x)
	{
		lData[6+x] = Lepra::Endian::HostToBigF(mBodyData.mParameter[x]);
	}
	const int lConnectorTypes = (int)mConnectorArray.size();
	lData[6+x] = Lepra::Endian::HostToBig(lConnectorTypes);
	for (int y = 0; y < lConnectorTypes; ++y)
	{
		lData[7+x+y] = Lepra::Endian::HostToBig(mConnectorArray[y]);
	}
}

void ChunkyBoneGeometry::LoadChunkyData(ChunkyStructure* pStructure, const void* pData)
{
	const Lepra::uint32* lData = (const Lepra::uint32*)pData;

	mBodyData.mMass = Lepra::Endian::BigToHostF(lData[0]);
	mBodyData.mFriction = Lepra::Endian::BigToHostF(lData[1]);
	mBodyData.mBounce = Lepra::Endian::BigToHostF(lData[2]);
	int lParentIndex = Lepra::Endian::BigToHost(lData[3]);
	mBodyData.mParent = pStructure->GetBoneGeometry(lParentIndex);
	mBodyData.mJointType = (JointType)Lepra::Endian::BigToHost(lData[4]);
	mBodyData.mIsAffectedByGravity = Lepra::Endian::BigToHost(lData[5])? true : false;
	int x;
	for (x = 0; x < sizeof(mBodyData.mParameter)/sizeof(mBodyData.mParameter[0]); ++x)
	{
		mBodyData.mParameter[x] = Lepra::Endian::BigToHostF(lData[6+x]);
	}
	const int lConnectorTypes = Lepra::Endian::BigToHost(lData[6+x]);
	for (int y = 0; y < lConnectorTypes; ++y)
	{
		 mConnectorArray.push_back((ConnectorType)Lepra::Endian::BigToHost(lData[7+x+y]));
	}
}



ChunkyBoneCapsule::ChunkyBoneCapsule(const BodyData& pBodyData, Lepra::float32 pRadius, Lepra::float32 pLength):
	Parent(pBodyData),
	mRadius(pRadius),
	mLength(pLength)
{
}

bool ChunkyBoneCapsule::CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
	PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
	const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateCapsule(pIsRoot, pTransform, mBodyData.mMass, mRadius, mLength, pType,
		mBodyData.mFriction, mBodyData.mBounce, pTrigListener, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneCapsule::CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
	const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateCapsuleTrigger(pTransform, mRadius, mLength, pTrigListener);
	return (mTriggerId != INVALID_TRIGGER);
}

ChunkyType ChunkyBoneCapsule::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_CAPSULE);
}

unsigned ChunkyBoneCapsule::GetChunkySize() const
{
	return (Parent::GetChunkySize() + sizeof(Lepra::float32)*2);
}

void ChunkyBoneCapsule::SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	Lepra::uint32* lData = (Lepra::uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	lData[0] = Lepra::Endian::HostToBigF(mRadius);
	lData[1] = Lepra::Endian::HostToBigF(mLength);
}

void ChunkyBoneCapsule::LoadChunkyData(ChunkyStructure* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const Lepra::uint32* lData = (const Lepra::uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mRadius = Lepra::Endian::BigToHostF(lData[0]);
	mLength = Lepra::Endian::BigToHostF(lData[1]);
}



ChunkyBoneSphere::ChunkyBoneSphere(const BodyData& pBodyData, Lepra::float32 pRadius):
	Parent(pBodyData),
	mRadius(pRadius)
{
}

bool ChunkyBoneSphere::CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
	PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
	const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateSphere(pIsRoot, pTransform, mBodyData.mMass, mRadius, pType, mBodyData.mFriction,
		mBodyData.mBounce, pTrigListener, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneSphere::CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
	const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateSphereTrigger(pTransform, mRadius, pTrigListener);
	return (mTriggerId != INVALID_TRIGGER);
}

ChunkyType ChunkyBoneSphere::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_SPHERE);
}

unsigned ChunkyBoneSphere::GetChunkySize() const
{
	return (Parent::GetChunkySize() + sizeof(Lepra::float32));
}

void ChunkyBoneSphere::SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	Lepra::uint32* lData = (Lepra::uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	lData[0] = Lepra::Endian::HostToBigF(mRadius);
}

void ChunkyBoneSphere::LoadChunkyData(ChunkyStructure* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const Lepra::uint32* lData = (const Lepra::uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mRadius = Lepra::Endian::BigToHostF(lData[0]);
}



ChunkyBoneBox::ChunkyBoneBox(const BodyData& pBodyData, const Lepra::Vector3DF& pSize):
	Parent(pBodyData),
	mSize(pSize)
{
}

bool ChunkyBoneBox::CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::TriggerListener* pTrigListener,
	PhysicsEngine::ForceFeedbackListener* pForceListener, PhysicsEngine::BodyType pType,
	const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateBox(pIsRoot, pTransform, mBodyData.mMass, mSize, pType, mBodyData.mFriction,
		mBodyData.mBounce, pTrigListener, pForceListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneBox::CreateTrigger(PhysicsEngine* pPhysics, PhysicsEngine::TriggerListener* pTrigListener,
	const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateBoxTrigger(pTransform, mSize, pTrigListener);
	return (mTriggerId != INVALID_TRIGGER);
}

ChunkyType ChunkyBoneBox::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_BOX);
}

unsigned ChunkyBoneBox::GetChunkySize() const
{
	return (Parent::GetChunkySize() + sizeof(Lepra::float32)*3);
}

void ChunkyBoneBox::SaveChunkyData(const ChunkyStructure* pStructure, void* pData) const
{
	Parent::SaveChunkyData(pStructure, pData);

	Lepra::uint32* lData = (Lepra::uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	lData[0] = Lepra::Endian::HostToBigF(mSize.x);
	lData[1] = Lepra::Endian::HostToBigF(mSize.y);
	lData[2] = Lepra::Endian::HostToBigF(mSize.z);
}

void ChunkyBoneBox::LoadChunkyData(ChunkyStructure* pStructure, const void* pData)
{
	Parent::LoadChunkyData(pStructure, pData);

	const Lepra::uint32* lData = (const Lepra::uint32*)&((const char*)pData)[Parent::GetChunkySize()];
	mSize.x = Lepra::Endian::BigToHostF(lData[0]);
	mSize.x = Lepra::Endian::BigToHostF(lData[1]);
	mSize.x = Lepra::Endian::BigToHostF(lData[2]);
}



}
