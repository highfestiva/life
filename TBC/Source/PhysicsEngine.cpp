
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
	float pMaxSpeed2, float pFriction, unsigned pControllerIndex):
	mEngineType(pEngineType),
	mStrength(pStrength),
	mMaxSpeed(pMaxSpeed),
	mMaxSpeed2(pMaxSpeed2),
	mFriction(pFriction),
	mControllerIndex(pControllerIndex),
	mIntensity(0)
{
	::memset(mValue, 0, sizeof(mValue));
	::memset(mSmoothValue, 0, sizeof(mSmoothValue));
}

PhysicsEngine::~PhysicsEngine()
{
}



PhysicsEngine* PhysicsEngine::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	const uint32* lData = (const uint32*)pData;
	if (pByteCount != sizeof(uint32)*7 + Endian::BigToHost(lData[6])*sizeof(uint32)*3)
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
	assert(mControllerIndex >= 0 && mControllerIndex < MAX_CONTROLLER_COUNT);

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
		case ENGINE_HOVER:
		case ENGINE_HINGE_ROLL:
		case ENGINE_HINGE_GYRO:
		case ENGINE_HINGE_BREAK:
		case ENGINE_HINGE_TORQUE:
		case ENGINE_HINGE2_TURN:
		case ENGINE_ROTOR:
		case ENGINE_TILTER:
		case ENGINE_SLIDER_FORCE:
		{
			if (pAspect == mControllerIndex)
			{
				mValue[0] = pValue;
				return (true);
			}
		}
		break;
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

void PhysicsEngine::ForceSetValue(unsigned pAspect, float pValue)
{
	mValue[pAspect] = pValue;
}



void PhysicsEngine::OnTick(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure, float pFrameTime) const
{
	const float lLimitedFrameTime = std::min(pFrameTime, 0.1f);
	const float lNormalizedFrameTime = lLimitedFrameTime * 90;
	mIntensity = 0;
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
					Vector3DF lAxis[3] = {Vector3DF(0, 1, 0),
						Vector3DF(1, 0, 0), Vector3DF(0, 0, 1)};
					QuaternionF lRotation;
					lRotation.RotateAroundWorldZ(mValue[3] - MathTraits<float>::Pi() / 2);
					lAxis[0] = lRotation*lAxis[0];
					lAxis[1] = lRotation*lAxis[1];
					Vector3DF lVelocityVector;
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
					mIntensity += lVelocityVector.GetLength() / mMaxSpeed;
				}
				break;
				case ENGINE_HOVER:
				{
					if (mValue[0] != 0 || mValue[1] != 0)
					{
						// Arcade stabilization for lifter (typically hovercraft, elevator or similar vehicle).
						Vector3DF lLiftPivot = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId()) + Vector3DF(0,0,1)*mFriction*lScale;

						const Vector3DF lLiftForce = Vector3DF(0,0,1)*mStrength*lScale;
						pPhysicsManager->AddForceAtPos(lGeometry->GetBodyId(), lLiftForce, lLiftPivot);
					}
				}
				break;
				case ENGINE_HINGE_GYRO:
				{
					// Apply a fake gyro torque to parent in order to emulate a heavier gyro than
					// it actually is. The gyro must be light weight, or physics simulation will be
					// unstable when rolling bodies around any other axis than the hinge one.
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT && mFriction >= 0)
					{
						Vector3DF lAxis;
						pPhysicsManager->GetAxis1(lGeometry->GetJointId(), lAxis);
						Vector3DF lY;
						Vector3DF lZ;
						lAxis.GetNormalized().GetOrthogonals(lY, lZ);
						const float lStrength = 3 * mValue[0] * mStrength;
						lZ *= lStrength;
						Vector3DF lPos;
						pPhysicsManager->GetAnchorPos(lGeometry->GetJointId(), lPos);
						pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lZ, lPos+lY);
						pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), -lZ, lPos-lY);
					}
				}
				// TRICKY: fall through.
				case ENGINE_HINGE_ROLL:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						// TODO: implement torque "curve" instead of constant angular force.
						float lValue = mValue[0];
						float lDirectionalMaxSpeed = ((lValue >= 0)? mMaxSpeed : -mMaxSpeed2) * lValue;
						if (mEngineType == ENGINE_HINGE_GYRO)
						{
							lValue = (lValue+1)*0.5f;
							lDirectionalMaxSpeed = lValue * (mMaxSpeed - mMaxSpeed2) + mMaxSpeed2;
						}
						const float lUsedStrength = mStrength*(::fabs(lValue) + ::fabs(mFriction));
						float lPreviousStrength = 0;
						float lPreviousTargetSpeed = 0;
						pPhysicsManager->GetAngularMotorRoll(lGeometry->GetJointId(), lPreviousStrength, lPreviousTargetSpeed);
						const float lTargetSpeed = Math::Lerp(lPreviousTargetSpeed, lDirectionalMaxSpeed*lScale, 0.5f);
						const float lTargetStrength = Math::Lerp(lPreviousStrength, lUsedStrength, 0.5f);
						pPhysicsManager->SetAngularMotorRoll(lGeometry->GetJointId(), lTargetStrength, lTargetSpeed);
						pPhysicsManager->GetAngleRate2(lGeometry->GetJointId(), lPreviousTargetSpeed);
						mIntensity += lPreviousTargetSpeed / mMaxSpeed;
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
					ApplyTorque(pPhysicsManager, lLimitedFrameTime, lGeometry, lEngineNode);
				}
				break;
				case ENGINE_ROTOR:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						const Vector3DF lRotorForce = GetRotorLiftForce(pPhysicsManager, lGeometry, lEngineNode);
						Vector3DF lLiftForce = lRotorForce * mValue[0];
						const int lParentBone = pStructure->GetIndex(lGeometry->GetParent());
						const QuaternionF lOrientation =
							pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
							pStructure->GetOriginalBoneTransformation(lParentBone).GetOrientation().GetInverse();

						Vector3DF lRotorPivot;
						pPhysicsManager->GetAnchorPos(lGeometry->GetJointId(), lRotorPivot);
						const Vector3DF lOffset =
							pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
							Vector3DF(0, 0, mMaxSpeed*lScale);
						lRotorPivot += lOffset;

						const float lAbsFriction = ::fabs(mFriction);
						if (mFriction < 0)
						{
							// Arcade stabilization for VTOL rotor.
							Vector3DF lParentAngularVelocity;
							pPhysicsManager->GetBodyAngularVelocity(lGeometry->GetParent()->GetBodyId(), lParentAngularVelocity);
							lParentAngularVelocity = lOrientation.GetInverse() * lParentAngularVelocity;
							const Vector3DF lParentAngle = lOrientation.GetInverse() * Vector3DF(0, 0, 1);	// TRICKY: assumes original joint direction is towards heaven.
							const float lStabilityX = -lParentAngle.x * 0.5f + lParentAngularVelocity.y * lAbsFriction;
							const float lStabilityY = -lParentAngle.y * 0.5f - lParentAngularVelocity.x * lAbsFriction;
							lRotorPivot += lOrientation * Vector3DF(lStabilityX, lStabilityY, 0);
						}

						// Smooth rotor force - for digital controls and to make acceleration seem more realistic.
						const float lSmooth = lNormalizedFrameTime * 0.05f * lEngineNode.mScale;
						lLiftForce.x = mSmoothValue[0] = Math::Lerp(mSmoothValue[0], lLiftForce.x, lSmooth);
						lLiftForce.y = mSmoothValue[1] = Math::Lerp(mSmoothValue[1], lLiftForce.y, lSmooth);
						lLiftForce.z = mSmoothValue[2] = Math::Lerp(mSmoothValue[2], lLiftForce.z, lSmooth);

						// Counteract rotor's movement through perpendicular air.
						Vector3DF lDragForce;
						pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lDragForce);
						lDragForce = ((-lDragForce*lRotorForce.GetNormalized()) * lAbsFriction * lNormalizedFrameTime) * lRotorForce;

						pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lLiftForce + lDragForce, lRotorPivot);
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
						const Vector3DF lLiftForce = GetRotorLiftForce(pPhysicsManager, lGeometry, lEngineNode) * ::fabs(mValue[0]);
						const int lParentBone = pStructure->GetIndex(lGeometry->GetParent());
						const float lPlacement = (mValue[0] >= 0)? 1.0f : -1.0f;
						const Vector3DF lOffset =
							pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
							pStructure->GetOriginalBoneTransformation(lParentBone).GetOrientation().GetInverse() *
							Vector3DF(lPlacement*mMaxSpeed, -lPlacement*mMaxSpeed2, 0);
						const Vector3DF lWorldPos = lOffset + pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
						pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lLiftForce, lWorldPos);
						//{
						//	static int cnt = 0;
						//	if ((++cnt)%300 == 0)
						//	{
						//		//Vector3DF r = pPhysicsManager->GetBodyOrientation(lGeometry->GetBodyId()).GetInverse() * lRelPos;
						//		//Vector3DF r = lRelPos;
						//		Vector3DF r = lOffset;
						//		Vector3DF w = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
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
				case ENGINE_SLIDER_FORCE:
				{
					assert(lGeometry->GetJointId() != INVALID_JOINT);
					if (lGeometry->GetJointId() != INVALID_JOINT)
					{
						if (!mValue[0] && lEngineNode.mMode != MODE_HALF_LOCK)	// Normal slider behavior is to pull back to origin while half-lock keep last motor target.
						{
							float lPosition;
							pPhysicsManager->GetSliderPos(lGeometry->GetJointId(), lPosition);
							if (!Math::IsEpsEqual(lPosition, 0.0f, 0.1f))
							{
								float lValue = -lPosition*::fabs(lScale);
								lValue = (lValue > 0)? lValue*mMaxSpeed : lValue*mMaxSpeed2;
								pPhysicsManager->SetMotorTarget(lGeometry->GetJointId(), mStrength, lValue);
							}
						}
						else
						{
							const float lValue = (mValue[0] > 0)? mValue[0]*mMaxSpeed : mValue[0]*mMaxSpeed2;
							pPhysicsManager->SetMotorTarget(lGeometry->GetJointId(), mStrength, lValue*lScale);
						}
						Vector3DF lVelocity;
						pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
						mIntensity += lVelocity.GetLength() / mMaxSpeed;
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
	mIntensity /= mEngineNodeArray.size();
}

float PhysicsEngine::GetCurrentMaxSpeedSquare(const PhysicsManager* pPhysicsManager) const
{
	float lMaxSpeed = 0;
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		const EngineNode& lEngineNode = *i;
		ChunkyBoneGeometry* lGeometry = lEngineNode.mGeometry;
		Vector3DF lVelocity;
		pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
		const float lSpeed = lVelocity.GetLengthSquared();
		lMaxSpeed = (lSpeed > lMaxSpeed)? lSpeed : lMaxSpeed;
	}
	return (lMaxSpeed);
}



unsigned PhysicsEngine::GetControllerIndex() const
{
	return (mControllerIndex);
}

float PhysicsEngine::GetValue() const
{
	assert(mControllerIndex >= 0 && mControllerIndex < MAX_CONTROLLER_COUNT);
	return (mValue[0]);
}

const float* PhysicsEngine::GetValues() const
{
	return (mValue);
}

float PhysicsEngine::GetIntensity() const
{
	return (mIntensity);
}

float PhysicsEngine::GetMaxSpeed() const
{
	return (mMaxSpeed);
}

float PhysicsEngine::GetLerpThrottle(float pUp, float pDown) const
{
	float& lLerpShadow = mSmoothValue[MAX_CONTROLLER_COUNT-1];
	lLerpShadow = Math::Lerp(lLerpShadow, GetValue(), (GetValue() > lLerpShadow)? pUp : pDown);
	return lLerpShadow;
}

unsigned PhysicsEngine::GetChunkySize() const
{
	return ((unsigned)(sizeof(uint32)*6 +
		sizeof(uint32) + sizeof(uint32)*3*mEngineNodeArray.size()));
}

void PhysicsEngine::SaveChunkyData(const ChunkyPhysics* pStructure, void* pData) const
{
	uint32* lData = (uint32*)pData;
	lData[0] = Endian::HostToBig(GetEngineType());
	lData[1] = Endian::HostToBigF(mStrength);
	lData[2] = Endian::HostToBigF(mMaxSpeed);
	lData[3] = Endian::HostToBigF(mMaxSpeed2);
	lData[4] = Endian::HostToBigF(mFriction);
	lData[5] = Endian::HostToBig(mControllerIndex);
	lData[6] = Endian::HostToBig((uint32)mEngineNodeArray.size());
	int x;
	for (x = 0; x < (int)mEngineNodeArray.size(); ++x)
	{
		const EngineNode& lControlledNode = mEngineNodeArray[x];
		lData[7+x*3] = Endian::HostToBig(pStructure->GetIndex(lControlledNode.mGeometry));
		lData[8+x*3] = Endian::HostToBigF(lControlledNode.mScale);
		lData[9+x*3] = Endian::HostToBig(lControlledNode.mMode);
	}
}

void PhysicsEngine::LoadChunkyData(ChunkyPhysics* pStructure, const void* pData)
{
	const uint32* lData = (const uint32*)pData;

	mEngineType = (EngineType)Endian::BigToHost(lData[0]);
	mStrength = Endian::BigToHostF(lData[1]);
	mMaxSpeed = Endian::BigToHostF(lData[2]);
	mMaxSpeed2 = Endian::BigToHostF(lData[3]);
	mFriction = Endian::BigToHostF(lData[4]);
	mControllerIndex = Endian::BigToHost(lData[5]);
	assert(mControllerIndex >= 0 && mControllerIndex < MAX_CONTROLLER_COUNT);
	const int lControlledNodeCount = Endian::BigToHost(lData[6]);
	int x;
	for (x = 0; x < lControlledNodeCount; ++x)
	{
		ChunkyBoneGeometry* lGeometry = pStructure->GetBoneGeometry(Endian::BigToHost(lData[7+x*3]));
		assert(lGeometry);
		float lScale = Endian::BigToHostF(lData[8+x*3]);
		EngineMode lMode = (EngineMode)Endian::BigToHost(lData[9+x*3]);
		AddControlledGeometry(lGeometry, lScale, lMode);
	}
}



Vector3DF PhysicsEngine::GetRotorLiftForce(PhysicsManager* pPhysicsManager, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const
{
	Vector3DF lAxis;
	pPhysicsManager->GetAxis1(pGeometry->GetJointId(), lAxis);
	float lAngularRotorSpeed = 0;
	pPhysicsManager->GetAngleRate1(pGeometry->GetJointId(), lAngularRotorSpeed);
	const float lLiftForce = lAngularRotorSpeed*mStrength*pEngineNode.mScale;
	return (lAxis*lLiftForce);
}

void PhysicsEngine::ApplyTorque(PhysicsManager* pPhysicsManager, float pFrameTime, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const
{
	pFrameTime;

	assert(pGeometry->GetJointId() != INVALID_JOINT);
	if (pGeometry->GetJointId() != INVALID_JOINT)
	{
		float lForce = mValue[0];

		const float lScale = pEngineNode.mScale;
		float lLoStop;
		float lHiStop;
		float lBounce;
		pPhysicsManager->GetJointParams(pGeometry->GetJointId(), lLoStop, lHiStop, lBounce);
		//const float lMiddle = (lLoStop+lHiStop)*0.5f;
		const float lTarget = 0;
		if (lLoStop < -1000 || lHiStop > 1000)
		{
			// Open interval -> relative torque.
			const float lTargetSpeed = lForce*lScale*mMaxSpeed;
			pPhysicsManager->SetAngularMotorTurn(pGeometry->GetJointId(), mStrength, lTargetSpeed);
			float lActualSpeed = 0;
			pPhysicsManager->GetAngleRate2(pGeometry->GetJointId(), lActualSpeed);
			mIntensity += (lTargetSpeed - lActualSpeed) / mMaxSpeed;
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
			if ((lForce < 0.02f && lIrlAngle < lTarget) ||
				(lForce > -0.02f && lIrlAngle > lTarget))
			{
				if (::fabs(pEngineNode.mLock) < ::fabs(lForce))
				{
					pEngineNode.mLock = lForce;
				}
				else
				{
					lForce = pEngineNode.mLock;
				}
			}
			else
			{
				pEngineNode.mLock = 0;
			}
		}
		if (mFriction)
		{
			// Wants us to scale (down) rotation angle depending on vehicle speed. Otherwise most vehicles
			// quickly flips, not yeilding very fun gameplay. Plus, it's more like real racing cars! :)
			Vector3DF lParentVelocity;
			pPhysicsManager->GetBodyVelocity(pGeometry->GetParent()->GetBodyId(), lParentVelocity);
			const float lRangeFactor = ::pow(mFriction, lParentVelocity.GetLength());
			lHiStop *= lRangeFactor;
			lLoStop *= lRangeFactor;
		}
		const float lAngleSpan = (lHiStop-lLoStop)*0.9f;
		const float lTargetAngle = (lForce < 0)? -lForce*lLoStop+lTarget : lForce*lHiStop+lTarget;
		const float lDiff = (lTargetAngle-lIrlAngle);
		const float lAbsDiff = ::fabs(lDiff);
		float lTargetSpeed;
		const float lBigDiff = lAngleSpan/7;
		if (lAbsDiff > lBigDiff)
		{
			lTargetSpeed = (lDiff > 0)? mMaxSpeed : -mMaxSpeed;
		}
		else
		{
			lTargetSpeed = mMaxSpeed*lDiff/lBigDiff;
		}
		lTargetSpeed /= lScale;
		float lCurrentSpeed = 0;
		if (mEngineType == ENGINE_HINGE2_TURN)
		{
			pPhysicsManager->GetAngleRate2(pGeometry->GetJointId(), lCurrentSpeed);
			lTargetSpeed *= 1+::fabs(lCurrentSpeed)/30;
		}
		else
		{
			pPhysicsManager->GetAngleRate1(pGeometry->GetJointId(), lCurrentSpeed);
			lTargetSpeed += (lTargetSpeed-lCurrentSpeed)*mMaxSpeed2*lScale*lScale/mMaxSpeed;
		}
		pPhysicsManager->SetAngularMotorTurn(pGeometry->GetJointId(), mStrength, lTargetSpeed);
		mIntensity += (lTargetSpeed - lCurrentSpeed) / mMaxSpeed;
	}
	else
	{
		mLog.AError("Missing turn joint!");
	}
}



LOG_CLASS_DEFINE(PHYSICS, PhysicsEngine);



}
