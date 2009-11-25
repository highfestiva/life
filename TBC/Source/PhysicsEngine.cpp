
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/PhysicsEngine.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"



namespace TBC
{



PhysicsEngine::PhysicsEngine(EngineType pEngineType, float pStrength, float pMaxSpeed,
	float pMaxSpeed2, float pFriction, unsigned pControllerIndex):
	mEngineType(pEngineType),
	mStrength(pStrength),
	mMaxSpeed(pMaxSpeed),
	mMaxSpeed2(pMaxSpeed2),
	mFriction(pFriction),
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
	if (pByteCount != sizeof(Lepra::uint32)*7 + Lepra::Endian::BigToHost(lData[6])*sizeof(Lepra::uint32)*3)
	{
		mLog.AError("Could not load; wrong data size.");
		assert(false);
		return (0);
	}

	PhysicsEngine* lEngine = new PhysicsEngine(ENGINE_WALK, 0, 0, 0, 0, 0);
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
			if (pAspect >= mControllerIndex+0 && pAspect <= mControllerIndex+4)
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
		case ENGINE_HINGE_ROLL:
		case ENGINE_HINGE_BREAK:
		case ENGINE_HINGE_TORQUE:
		case ENGINE_HINGE2_TURN:
		case ENGINE_ROTOR:
		case ENGINE_TILTER:
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



void PhysicsEngine::OnTick(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure, float pFrameTime)
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
				case ENGINE_HINGE_ROLL:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						// TODO: implement torque "curve" instead of constant angular force.
						const float lUsedStrength = mStrength*(::fabs(mValue[0]) + mFriction);
						const float lDirectionalMaxSpeed = (mValue[0] >= 0)? mMaxSpeed : -mMaxSpeed2;
						float lPreviousStrength = 0;
						float lPreviousTargetSpeed = 0;
						pPhysicsManager->GetAngularMotorRoll(lGeometry->GetJointId(), lPreviousStrength, lPreviousTargetSpeed);
						const float lTargetSpeed = Lepra::Math::Lerp(lPreviousTargetSpeed, lDirectionalMaxSpeed*mValue[0], 0.5f);
						const float lTargetStrength = Lepra::Math::Lerp(lPreviousStrength, lUsedStrength*lScale, 0.5f);
						pPhysicsManager->SetAngularMotorRoll(lGeometry->GetJointId(), lTargetStrength, lTargetSpeed);
					}
					else
					{
						mLog.AError("Missing roll joint!");
					}
				}
				break;
				case ENGINE_HINGE_BREAK:
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
				case ENGINE_HINGE_TORQUE:
				case ENGINE_HINGE2_TURN:
				{
					ApplyTorque(pPhysicsManager, pFrameTime, lGeometry, lEngineNode);
				}
				break;
				case ENGINE_ROTOR:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						//pPhysicsManager->SetIsGyroscope(lGeometry->GetBodyId(), false);

						const int lForcesUsed = 4;
						const Lepra::Vector3DF lRotorForce = GetRotorLiftForce(pPhysicsManager, lGeometry, lEngineNode);
						const Lepra::Vector3DF lLiftForce = lRotorForce * (mValue[0]/lForcesUsed);
						const int lParentBone = pStructure->GetIndex(lGeometry->GetParent());
						const Lepra::QuaternionF lOrientation =
							pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
							pStructure->GetOriginalBoneTransformation(lParentBone).GetOrientation().GetInverse();

						// Stabilization.
						Lepra::Vector3DF lParentAngularVelocity;
						pPhysicsManager->GetBodyAngularVelocity(lGeometry->GetParent()->GetBodyId(), lParentAngularVelocity);
						lParentAngularVelocity = lOrientation.GetInverse() * lParentAngularVelocity;
						const Lepra::Vector3DF lParentAngle = lOrientation.GetInverse() * Lepra::Vector3DF(0, 0, 1);	// TRICKY: assumes original joint direction is towards heaven.
						//const float lStabilityX = -lParentAngle.x + 0.1f * (lParentAngularVelocity.y * mFriction);
						//const float lStabilityY = -lParentAngle.y - 0.1f * (lParentAngularVelocity.x * mFriction);
						//const float lStabilityX = -lParentAngle.x * mFriction;
						//const float lStabilityY = -lParentAngle.y * mFriction;
						//const float lLiftStrength = lLiftForce.GetLength();
						//const Lepra::Vector3DF lFrontForce = lOrientation * Lepra::Vector3DF(-lParentAngle.x*lLiftStrength*0.1f, 0, 0);
						//const float lStabilityX = lParentAngularVelocity.y * mFriction;
						//const float lStabilityY = lParentAngularVelocity.x * mFriction;

						// Counteract rotor's movement through perpendicular air.
						Lepra::Vector3DF lDragForce;
						pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lDragForce);
						lDragForce = (-lDragForce*lRotorForce.GetNormalized()) * mFriction * lRotorForce;

						// Apply rotor force.
						const Lepra::Vector3DF lWorldCenter = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
						//const float lPlacement[lForcesUsed][2] = { {1+lStabilityX,0}, {0,1+lStabilityY}, {-1+lStabilityX,0}, {0,-1+lStabilityY} };
						const float lPlacement[lForcesUsed][2] = { {1,0}, {0,1}, {-1,0}, {0,-1} };
						//const float lLiftForceStrength = lLiftForce.GetLength();
						for (int i = 0; i < lForcesUsed; ++i)
						{
							const Lepra::Vector3DF lOffset = lOrientation *
								Lepra::Vector3DF(lPlacement[i][0]*mMaxSpeed, lPlacement[i][1]*mMaxSpeed, 0);
							//const Lepra::Vector3DF lConeForce = lLiftForce -
							//	lLiftForceStrength*lOffset.GetNormalized(0.1f);
							pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lLiftForce + lDragForce, lWorldCenter+lOffset);
						}
					}
					else
					{
						mLog.AError("Missing rotor joint!");
					}
				}
				break;
				case ENGINE_TILTER:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						const Lepra::Vector3DF lLiftForce = GetRotorLiftForce(pPhysicsManager, lGeometry, lEngineNode) * ::fabs(mValue[0]);
						const int lParentBone = pStructure->GetIndex(lGeometry->GetParent());
						const float lPlacement = (mValue[0] >= 0)? 1.0f : -1.0f;
						const Lepra::Vector3DF lOffset =
							pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
							pStructure->GetOriginalBoneTransformation(lParentBone).GetOrientation().GetInverse() *
							Lepra::Vector3DF(lPlacement*mMaxSpeed, -lPlacement*mMaxSpeed2, 0);
						const Lepra::Vector3DF lWorldPos = lOffset + pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
						pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lLiftForce, lWorldPos);
						//{
						//	static int cnt = 0;
						//	if ((++cnt)%300 == 0)
						//	{
						//		//Lepra::Vector3DF r = pPhysicsManager->GetBodyOrientation(lGeometry->GetBodyId()).GetInverse() * lRelPos;
						//		//Lepra::Vector3DF r = lRelPos;
						//		Lepra::Vector3DF r = lOffset;
						//		Lepra::Vector3DF w = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
						//		mLog.Infof(_T("Got pos (%f, %f, %f) - world pos is (%f, %f, %f)."), r.x, r.y, r.z, w.x, w.y, w.z);
						//	}
						//}
					}
					else
					{
						mLog.AError("Missing rotor joint!");
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
	return ((unsigned)(sizeof(Lepra::uint32)*6 +
		sizeof(Lepra::uint32) + sizeof(Lepra::uint32)*3*mEngineNodeArray.size()));
}

void PhysicsEngine::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	Lepra::uint32* lData = (Lepra::uint32*)pData;
	lData[0] = Lepra::Endian::HostToBig(GetEngineType());
	lData[1] = Lepra::Endian::HostToBigF(mStrength);
	lData[2] = Lepra::Endian::HostToBigF(mMaxSpeed);
	lData[3] = Lepra::Endian::HostToBigF(mMaxSpeed2);
	lData[4] = Lepra::Endian::HostToBigF(mFriction);
	lData[5] = Lepra::Endian::HostToBig(mControllerIndex);
	lData[6] = Lepra::Endian::HostToBig((Lepra::uint32)mEngineNodeArray.size());
	int x;
	for (x = 0; x < (int)mEngineNodeArray.size(); ++x)
	{
		const EngineNode& lControlledNode = mEngineNodeArray[x];
		lData[7+x*3] = Lepra::Endian::HostToBig(pStructure->GetIndex(lControlledNode.mGeometry));
		lData[8+x*3] = Lepra::Endian::HostToBigF(lControlledNode.mScale);
		lData[9+x*3] = Lepra::Endian::HostToBig(lControlledNode.mMode);
	}
}

void PhysicsEngine::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const Lepra::uint32* lData = (const Lepra::uint32*)pData;

	mEngineType = (EngineType)Lepra::Endian::BigToHost(lData[0]);
	mStrength = Lepra::Endian::BigToHostF(lData[1]);
	mMaxSpeed = Lepra::Endian::BigToHostF(lData[2]);
	mMaxSpeed2 = Lepra::Endian::BigToHostF(lData[3]);
	mFriction = Lepra::Endian::BigToHostF(lData[4]);
	mControllerIndex = Lepra::Endian::BigToHost(lData[5]);
	const int lControlledNodeCount = Lepra::Endian::BigToHost(lData[6]);
	int x;
	for (x = 0; x < lControlledNodeCount; ++x)
	{
		ChunkyBoneGeometry* lGeometry = pStructure->GetBoneGeometry(Lepra::Endian::BigToHost(lData[7+x*3]));
		assert(lGeometry);
		float lScale = Lepra::Endian::BigToHostF(lData[8+x*3]);
		EngineMode lMode = (EngineMode)Lepra::Endian::BigToHost(lData[9+x*3]);
		AddControlledGeometry(lGeometry, lScale, lMode);
	}
}



Lepra::Vector3DF PhysicsEngine::GetRotorLiftForce(PhysicsManager* pPhysicsManager, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const
{
	Lepra::Vector3DF lAxis;
	pPhysicsManager->GetAxis1(pGeometry->GetJointId(), lAxis);
	float lAngularRotorSpeed = 0;
	pPhysicsManager->GetAngleRate1(pGeometry->GetJointId(), lAngularRotorSpeed);
	const float lLiftForce = lAngularRotorSpeed*mStrength*pEngineNode.mScale;
	return (lAxis*lLiftForce);
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
