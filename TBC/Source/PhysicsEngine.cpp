
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsEngine.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"



namespace TBC
{



PhysicsEngine::PhysicsEngine(EngineType pEngineType, float pStrength, float pMaxSpeed,
	float pMaxSpeed2, unsigned pControllerIndex):
	mEngineType(pEngineType),
	mStrength(pStrength),
	mMaxSpeed(pMaxSpeed),
	mMaxSpeed2(pMaxSpeed2),
	mControllerIndex(pControllerIndex)
{
	::memset(mValue, 0, sizeof(mValue));
}

PhysicsEngine::~PhysicsEngine()
{
}



PhysicsEngine* PhysicsEngine::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	const Lepra::uint32* lData = (const Lepra::uint32*)pData;
	if (pByteCount != sizeof(Lepra::uint32)*6 + Lepra::Endian::BigToHost(lData[5])*sizeof(Lepra::uint32)*3)
	{
		mLog.AError("Could not load; wrong data size.");
		assert(false);
		return (0);
	}

	PhysicsEngine* lEngine = new PhysicsEngine(ENGINE_WALK, 0, 0, 0, 0);
	lEngine->LoadChunkyData(pStructure, pData);
	if (lEngine->GetChunkySize() != pByteCount)
	{
		assert(false);
		mLog.AError("Corrupt data or error in loading algo.");
		delete (lEngine);
		lEngine = 0;
	}
	return (lEngine);
}



PhysicsEngine::EngineType PhysicsEngine::GetEngineType() const
{
	return (mEngineType);
}



void PhysicsEngine::AddControlledGeometry(ChunkyBoneGeometry* pGeometry, float pScale, EngineMode pMode)
{
	mEngineNodeArray.push_back(EngineNode(pGeometry, pScale, pMode));
}

bool PhysicsEngine::SetValue(unsigned pAspect, float pValue, float pZAngle)
{
	pValue = (pValue > 1)? 1 : pValue;
	pValue = (pValue < -1)? -1 : pValue;

	switch (mEngineType)
	{
		case ENGINE_WALK:
		{
			assert(false);
			mLog.AError("Walk not implemented!");
		}
		break;
		case ENGINE_CAMERA_FLAT_PUSH:
		{
			if (pAspect >= 0 && pAspect <= 4)
			{
				switch (pAspect)
				{
					case 0:		mValue[0] = pValue;		break;
					case 4:		mValue[0] += pValue;		break;
					case 1:		mValue[1] = pValue;		break;
					case 2:		mValue[0] -= ::fabs(pValue);	break;	// Breaking and handbreaking always reverse.
					case 3:		mValue[2] = pValue;		break;
				}
				mValue[3] = pZAngle;
				return (true);
			}
		}
		break;
		case ENGINE_HINGE2_ROLL:
		{
			if (pAspect == mControllerIndex)
			{
				mValue[0] = pValue;
				return (true);
			}
		}
		break;
		case ENGINE_HINGE2_TURN:
		{
			if (pAspect == mControllerIndex)
			{
				mValue[0] = pValue;
				return (true);
			}
		}
		break;
		case ENGINE_HINGE2_BREAK:
		{
			if (pAspect == mControllerIndex)
			{
				mValue[0] = pValue;
				return (true);
			}
		}
		break;
		case ENGINE_HINGE:
		{
			if (pAspect == mControllerIndex)
			{
				mValue[0] = pValue;
				return (true);
			}
		}
		break;
		case ENGINE_ROLL_STRAIGHT:
		case ENGINE_GLUE:
		{
			// Fixed mode "engine".
		}
		break;
		default:
		{
			assert(false);
		}
		break;
	}
	return (false);
}



void PhysicsEngine::OnTick(PhysicsManager* pPhysicsManager, float pFrameTime)
{
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		const EngineNode& lEngineNode = *i;
		ChunkyBoneGeometry* lGeometry = lEngineNode.mGeometry;
		const float lScale = lEngineNode.mScale;
		if (lGeometry)
		{
			switch (mEngineType)
			{
				case ENGINE_WALK:
				{
					assert(false);
					mLog.AError("Walk not implemented!");
				}
				break;
				case ENGINE_CAMERA_FLAT_PUSH:
				{
					Lepra::Vector3DF lAxis[3] = {Lepra::Vector3DF(0, 1, 0),
						Lepra::Vector3DF(1, 0, 0), Lepra::Vector3DF(0, 0, 1)};
					Lepra::QuaternionF lRotation;
					lRotation.RotateAroundWorldZ(mValue[3] - Lepra::MathTraits<float>::Pi() / 2);
					lAxis[0] = lRotation*lAxis[0];
					lAxis[1] = lRotation*lAxis[1];
					Lepra::Vector3DF lVelocityVector;
					pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lVelocityVector);
					lVelocityVector = lRotation*lVelocityVector;
					float lVelocity[3] = { lVelocityVector.y, lVelocityVector.x, lVelocityVector.z };
					bool lIsSpeeding = (lVelocityVector.GetLength() >= mMaxSpeed);
					for (int i = 0; i < 3; ++i)
					{
						if (!lIsSpeeding || (lVelocity[i]>0) != (mValue[i]>0))
						{
							pPhysicsManager->AddForce(lGeometry->GetBodyId(), mValue[i]*lAxis[i]*mStrength*lScale);
						}
					}
				}
				break;
				case ENGINE_HINGE2_ROLL:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						// TODO: implement torque "curve" instead of constant angular force.
						const float lUsedStrength = mStrength*(::fabs(mValue[0])+0.01f);
						const float lDirectionalMaxSpeed = (mValue[0] >= 0)? mMaxSpeed : mMaxSpeed2;
						float lCurrentUsedStrength = 0;
						float lCurrentTargetSpeed = 0;
						pPhysicsManager->GetAngularMotorRoll(lGeometry->GetJointId(), lCurrentUsedStrength, lCurrentTargetSpeed);
						const float lTargetSpeed = Lepra::Math::Lerp(lCurrentTargetSpeed, lDirectionalMaxSpeed*mValue[0]*lScale, 0.5f);
						const float lTargetStrength = Lepra::Math::Lerp(lCurrentUsedStrength, lUsedStrength, 0.5f);
						pPhysicsManager->SetAngularMotorRoll(lGeometry->GetJointId(), lTargetStrength, lTargetSpeed);
					}
					else
					{
						mLog.AError("Missing roll joint!");
					}
				}
				break;
				case ENGINE_ROLL_STRAIGHT:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						pPhysicsManager->SetAngle1(lGeometry->GetBodyId(), lGeometry->GetJointId(), 0);
					}
				}
				break;
				case ENGINE_HINGE2_TURN:
				case ENGINE_HINGE:
				{
					ApplyTorque(pPhysicsManager, pFrameTime, lGeometry, lEngineNode);
				}
				break;
				case ENGINE_HINGE2_BREAK:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						// "Max speed" used as a type of "break threashold", so that a joystick or similar
						// won't start breaking on the tiniest movement. "Scaling" here determines part of
						// functionality (such as only affecting some wheels), may be positive or negative.
						const float lAbsValue = ::fabs(mValue[0]);
						if (lAbsValue > mMaxSpeed && mValue[0] < lScale)
						{
							const float lBreakForceUsed = mStrength*lAbsValue;
							lGeometry->SetExtraData(1);
							pPhysicsManager->SetAngularMotorRoll(lGeometry->GetJointId(), lBreakForceUsed, 0);
						}
						else if (lGeometry->GetExtraData())
						{
							lGeometry->SetExtraData(0);
							pPhysicsManager->SetAngularMotorRoll(lGeometry->GetJointId(), 0, 0);
						}
					}
					else
					{
						mLog.AError("Missing break joint!");
					}
				}
				break;
				case ENGINE_GLUE:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						pPhysicsManager->StabilizeJoint(lGeometry->GetJointId());
					}
				}
				break;
				default:
				{
					assert(false);
				}
				break;
			}
		}
		else
		{
			mLog.AError("Missing node!");
		}
	}
}



unsigned PhysicsEngine::GetControllerIndex() const
{
	return (mControllerIndex);
}

float PhysicsEngine::GetValue() const
{
	return (mValue[0]);
}

const float* PhysicsEngine::GetValues() const
{
	return (mValue);
}



unsigned PhysicsEngine::GetChunkySize() const
{
	return ((unsigned)(sizeof(Lepra::uint32)*5 +
		sizeof(Lepra::uint32) + sizeof(Lepra::uint32)*3*mEngineNodeArray.size()));
}

void PhysicsEngine::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	Lepra::uint32* lData = (Lepra::uint32*)pData;
	lData[0] = Lepra::Endian::HostToBig(GetEngineType());
	lData[1] = Lepra::Endian::HostToBigF(mStrength);
	lData[2] = Lepra::Endian::HostToBigF(mMaxSpeed);
	lData[3] = Lepra::Endian::HostToBigF(mMaxSpeed2);
	lData[4] = Lepra::Endian::HostToBig(mControllerIndex);
	lData[5] = Lepra::Endian::HostToBig((Lepra::uint32)mEngineNodeArray.size());
	int x;
	for (x = 0; x < (int)mEngineNodeArray.size(); ++x)
	{
		const EngineNode& lControlledNode = mEngineNodeArray[x];
		lData[6+x*3] = Lepra::Endian::HostToBig(pStructure->GetIndex(lControlledNode.mGeometry));
		lData[7+x*3] = Lepra::Endian::HostToBigF(lControlledNode.mScale);
		lData[8+x*3] = Lepra::Endian::HostToBig(lControlledNode.mMode);
	}
}

void PhysicsEngine::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const Lepra::uint32* lData = (const Lepra::uint32*)pData;

	mEngineType = (EngineType)Lepra::Endian::BigToHost(lData[0]);
	mStrength = Lepra::Endian::BigToHostF(lData[1]);
	mMaxSpeed = Lepra::Endian::BigToHostF(lData[2]);
	mMaxSpeed2 = Lepra::Endian::BigToHostF(lData[3]);
	mControllerIndex = Lepra::Endian::BigToHost(lData[4]);
	const int lControlledNodeCount = Lepra::Endian::BigToHost(lData[5]);
	int x;
	for (x = 0; x < lControlledNodeCount; ++x)
	{
		ChunkyBoneGeometry* lGeometry = pStructure->GetBoneGeometry(Lepra::Endian::BigToHost(lData[6+x*3]));
		assert(lGeometry);
		float lScale = Lepra::Endian::BigToHostF(lData[7+x*3]);
		EngineMode lMode = (EngineMode)Lepra::Endian::BigToHost(lData[8+x*3]);
		AddControlledGeometry(lGeometry, lScale, lMode);
	}
}



void PhysicsEngine::ApplyTorque(PhysicsManager* pPhysicsManager, float pFrameTime, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode)
{
	assert(pGeometry->GetJointId() != INVALID_JOINT);
	if (pGeometry->GetJointId() != INVALID_JOINT)
	{
		float lForce = mValue[0];

		const float lScale = pEngineNode.mScale;
		float lLoStop;
		float lHiStop;
		float lBounce;
		pPhysicsManager->GetJointParams(pGeometry->GetJointId(), lLoStop, lHiStop, lBounce);
		const float lMiddle = (lLoStop+lHiStop)*0.5f;
		if (lLoStop < -1000 || lHiStop > 1000)
		{
			// Open interval -> relative torque.
			pPhysicsManager->SetAngularMotorTurn(pGeometry->GetJointId(), mStrength, lForce*lScale*mMaxSpeed);
			return;
		}

		float lIrlAngle;
		if (!pPhysicsManager->GetAngle1(pGeometry->GetJointId(), lIrlAngle))
		{
			mLog.AError("Bad joint angle!");
			return;
		}

		if (pEngineNode.mMode == MODE_HALF_LOCK)
		{
			if ((lForce < 0.02f && lIrlAngle < lMiddle) ||
				(lForce > -0.02f && lIrlAngle > lMiddle))
			{
				if (::fabs(mValue[1]) < ::fabs(lForce))
				{
					mValue[1] = lForce;
				}
				else
				{
					lForce = mValue[1];
				}
			}
			else
			{
				mValue[1] = 0;
			}
		}
		const float lAngleSpan = (lHiStop-lLoStop)*0.9f;
		const float lTargetAngle = lForce*lAngleSpan*0.5f + lMiddle;
		const float lDiff = (lTargetAngle-lIrlAngle);
		const float lAbsDiff = ::fabs(lDiff);
		if (lAbsDiff > 0.0001f)
		{
			float lAngleSpeed;
			const float lBigDiff = lAngleSpan/20;
			if (lAbsDiff > lBigDiff)
			{
				lAngleSpeed = (lDiff > 0)? mMaxSpeed : -mMaxSpeed;
			}
			else
			{
				lAngleSpeed = mMaxSpeed*lDiff/lBigDiff;
			}
			lAngleSpeed /= lScale;
			if (mEngineType == ENGINE_HINGE2_TURN)
			{
				float lAngleRate = 0;
				pPhysicsManager->GetAngleRate2(pGeometry->GetJointId(), lAngleRate);
				lAngleSpeed *= 1+::fabs(lAngleRate)/30;
			}
			else
			{
				float lAngleRate = 0;
				pPhysicsManager->GetAngleRate1(pGeometry->GetJointId(), lAngleRate);
				lAngleSpeed += (lAngleSpeed-lAngleRate)*pFrameTime*mMaxSpeed2*50.0f*lScale*lScale/mMaxSpeed;

			}
			pPhysicsManager->SetAngularMotorTurn(pGeometry->GetJointId(), mStrength, lAngleSpeed);
		}
	}
	else
	{
		mLog.AError("Missing turn joint!");
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsEngine);



}
