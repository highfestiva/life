
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



ChunkyBoneCapsule::ChunkyBoneCapsule(const BodyData& pBodyData, Lepra::float32 pMass, Lepra::float32 pRadius,
	Lepra::float32 pLength, Lepra::float32 pFriction, Lepra::float32 pBounce):
	ChunkyBoneGeometry(pBodyData),
	mMass(pMass),
	mRadius(pRadius),
	mLength(pLength),
	mFriction(pFriction),
	mBounce(pBounce)
{
}

bool ChunkyBoneCapsule::CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateCapsule(pIsRoot, pTransform, mMass, mRadius, mLength, pType, mFriction, mBounce,
		mBodyData.mTriggerListener, mBodyData.mForceFeedbackListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneCapsule::CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateCapsuleTrigger(pTransform, mRadius, mLength, mBodyData.mTriggerListener);
	return (mTriggerId != INVALID_TRIGGER);
}

ChunkyType ChunkyBoneCapsule::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_CAPSULE);
}

unsigned ChunkyBoneCapsule::GetChunkySize() const
{
	return (sizeof(Lepra::float32)*5);
}

void ChunkyBoneCapsule::SaveChunkyData(void* pData) const
{
	float* lData = (float*)pData;
	lData[0] = mMass;
	lData[1] = mRadius;
	lData[2] = mLength;
	lData[3] = mFriction;
	lData[4] = mBounce;
}

void ChunkyBoneCapsule::LoadChunkyData(const void* pData)
{
	const float* lData = (const float*)pData;
	mMass = lData[0];
	mRadius = lData[1];
	mLength = lData[2];
	mFriction = lData[3];
	mBounce = lData[4];
}



ChunkyBoneSphere::ChunkyBoneSphere(const BodyData& pBodyData, Lepra::float32 pMass, Lepra::float32 pRadius,
	Lepra::float32 pFriction, Lepra::float32 pBounce):
	ChunkyBoneGeometry(pBodyData),
	mMass(pMass),
	mRadius(pRadius),
	mFriction(pFriction),
	mBounce(pBounce)
{
}

bool ChunkyBoneSphere::CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateSphere(pIsRoot, pTransform, mMass, mRadius, pType, mFriction, mBounce,
		mBodyData.mTriggerListener, mBodyData.mForceFeedbackListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneSphere::CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateSphereTrigger(pTransform, mRadius, mBodyData.mTriggerListener);
	return (mTriggerId != INVALID_TRIGGER);
}

ChunkyType ChunkyBoneSphere::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_SPHERE);
}

unsigned ChunkyBoneSphere::GetChunkySize() const
{
	return (sizeof(Lepra::float32)*4);
}

void ChunkyBoneSphere::SaveChunkyData(void* pData) const
{
	float* lData = (float*)pData;
	lData[0] = mMass;
	lData[1] = mRadius;
	lData[2] = mFriction;
	lData[3] = mBounce;
}

void ChunkyBoneSphere::LoadChunkyData(const void* pData)
{
	const float* lData = (const float*)pData;
	mMass = lData[0];
	mRadius = lData[1];
	mFriction = lData[2];
	mBounce = lData[3];
}



ChunkyBoneBox::ChunkyBoneBox(const BodyData& pBodyData, Lepra::float32 pMass, const Lepra::Vector3DF& pSize,
	Lepra::float32 pFriction, Lepra::float32 pBounce):
	ChunkyBoneGeometry(pBodyData),
	mMass(pMass),
	mSize(pSize),
	mFriction(pFriction),
	mBounce(pBounce)
{
}

bool ChunkyBoneBox::CreateBody(PhysicsEngine* pPhysics, bool pIsRoot, PhysicsEngine::BodyType pType, const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mBodyId = pPhysics->CreateBox(pIsRoot, pTransform, mMass, mSize, pType, mFriction, mBounce,
		mBodyData.mTriggerListener, mBodyData.mForceFeedbackListener);
	return (mBodyId != INVALID_BODY);
}

bool ChunkyBoneBox::CreateTrigger(PhysicsEngine* pPhysics, const Lepra::TransformationF& pTransform)
{
	RemovePhysics(pPhysics);
	mTriggerId = pPhysics->CreateBoxTrigger(pTransform, mSize, mBodyData.mTriggerListener);
	return (mTriggerId != INVALID_TRIGGER);
}

ChunkyType ChunkyBoneBox::GetChunkyType() const
{
	return (CHUNK_STRUCTURE_BONE_SHAPE_BOX);
}

unsigned ChunkyBoneBox::GetChunkySize() const
{
	return (sizeof(Lepra::float32)*6);
}

void ChunkyBoneBox::SaveChunkyData(void* pData) const
{
	float* lData = (float*)pData;
	lData[0] = mMass;
	lData[1] = mSize.x;
	lData[2] = mSize.y;
	lData[3] = mSize.z;
	lData[4] = mFriction;
	lData[5] = mBounce;
}

void ChunkyBoneBox::LoadChunkyData(const void* pData)
{
	const float* lData = (const float*)pData;
	mMass = lData[0];
	mSize.x = lData[1];
	mSize.x = lData[2];
	mSize.x = lData[3];
	mFriction = lData[4];
	mBounce = lData[5];
}



}
