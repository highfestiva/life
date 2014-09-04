
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/PhysicsEngine.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Endian.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyPhysics.h"



namespace Tbc
{



#define MAX_ASPECT_INDEX	40



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

void PhysicsEngine::RelocatePointers(const ChunkyPhysics* pTarget, const ChunkyPhysics* pSource, const PhysicsEngine& pOriginal)
{
	const size_t cnt = mEngineNodeArray.size();
	for (size_t x = 0; x < cnt; ++x)
	{
		const int lBoneIndex = pSource->GetIndex(pOriginal.mEngineNodeArray[x].mGeometry);
		deb_assert(lBoneIndex >= 0);
		mEngineNodeArray[x].mGeometry = pTarget->GetBoneGeometry(lBoneIndex);
	}
}



PhysicsEngine* PhysicsEngine::Load(ChunkyPhysics* pStructure, const void* pData, unsigned pByteCount)
{
	const uint32* lData = (const uint32*)pData;
	if (pByteCount != sizeof(uint32)*7 + Endian::BigToHost(lData[6])*sizeof(uint32)*3)
	{
		mLog.AError("Could not load; wrong data size.");
		deb_assert(false);
		return (0);
	}

	PhysicsEngine* lEngine = new PhysicsEngine(ENGINE_WALK, 0, 0, 0, 0, 0);
	lEngine->LoadChunkyData(pStructure, pData);
	if (lEngine->GetChunkySize() != pByteCount)
	{
		deb_assert(false);
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

PhysicsEngine::GeometryList PhysicsEngine::GetControlledGeometryList() const
{
	GeometryList lList;
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		lList.push_back(i->mGeometry);
	}
	return lList;
}

void PhysicsEngine::SetStrength(float pStrength)
{
	mStrength = pStrength;
}

bool PhysicsEngine::SetValue(unsigned pAspect, float pValue)
{
	deb_assert(mControllerIndex >= 0 && mControllerIndex < MAX_ASPECT_INDEX);
	deb_assert(pValue >= -10000);
	deb_assert(pValue <= +10000);

	switch (mEngineType)
	{
		case ENGINE_WALK:
		{
			deb_assert(false);
			mLog.AError("Walk not implemented!");
		}
		break;
		case ENGINE_PUSH_RELATIVE:
		case ENGINE_PUSH_ABSOLUTE:
		{
			const unsigned lControlledAspects = (mEngineType == ENGINE_PUSH_RELATIVE)? 2 : 3;
			if (pAspect >= mControllerIndex+0 && pAspect <= mControllerIndex+lControlledAspects)
			{
				switch (pAspect - mControllerIndex)
				{
					case 0:	mValue[ASPECT_PRIMARY]    = pValue;	break;
					case 2:	mValue[ASPECT_PRIMARY]   += pValue;	break;	// Handbrake.
					case 1:	mValue[ASPECT_SECONDARY]  = pValue;	break;
					case 3:	mValue[ASPECT_TERTIARY]	  = pValue;	break;
				}
				return (true);
			}
		}
		break;
		case ENGINE_HOVER:
		case ENGINE_HINGE_ROLL:
		case ENGINE_HINGE_GYRO:
		case ENGINE_HINGE_BRAKE:
		case ENGINE_HINGE_TORQUE:
		case ENGINE_HINGE2_TURN:
		case ENGINE_ROTOR:
		case ENGINE_TILTER:
		case ENGINE_JET:
		case ENGINE_SLIDER_FORCE:
		case ENGINE_YAW_BRAKE:
		case ENGINE_AIR_BRAKE:
		{
			if (pAspect == mControllerIndex)
			{
				mValue[ASPECT_PRIMARY] = pValue;
				return (true);
			}
		}
		break;
		case ENGINE_GLUE:
		case ENGINE_BALL_BRAKE:
		{
			// Fixed mode "engine".
		}
		break;
		default:
		{
			deb_assert(false);
		}
		break;
	}
	return (false);
}

void PhysicsEngine::ForceSetValue(unsigned pAspect, float pValue)
{
	deb_assert(pValue >= -1 && pValue <= +1);
	mValue[pAspect] = pValue;
}



void PhysicsEngine::OnMicroTick(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure, float pFrameTime) const
{
	const float lLimitedFrameTime = std::min(pFrameTime, 0.1f);
	const float lNormalizedFrameTime = lLimitedFrameTime * 90;
	const float lPrimaryForce = (mValue[ASPECT_LOCAL_PRIMARY] > std::abs(mValue[ASPECT_PRIMARY]))? mValue[ASPECT_LOCAL_PRIMARY] : mValue[ASPECT_PRIMARY];
	mIntensity = 0;
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		const EngineNode& lEngineNode = *i;
		ChunkyBoneGeometry* lGeometry = lEngineNode.mGeometry;
		const float lScale = lEngineNode.mScale;
		if (!lGeometry)
		{
			mLog.AError("Missing node!");
			continue;
		}
		switch (mEngineType)
		{
			case ENGINE_WALK:
			{
				deb_assert(false);
				mLog.AError("Walk not implemented!");
			}
			break;
			case ENGINE_PUSH_RELATIVE:
			case ENGINE_PUSH_ABSOLUTE:
			{
				vec3 lAxis[3] = {vec3(0, 1, 0),
					vec3(1, 0, 0), vec3(0, 0, 1)};
				if (mEngineType == ENGINE_PUSH_RELATIVE)
				{
					const ChunkyBoneGeometry* lRootGeometry = pStructure->GetBoneGeometry(0);
					const quat lOrientation =
						pPhysicsManager->GetBodyOrientation(lRootGeometry->GetBodyId()) *
						pStructure->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
					/*float lYaw;
					float lPitch;
					float lRoll;
					lOrientation.GetEulerAngles(lYaw, lPitch, lRoll);
					quat lRotation;
					lRotation.RotateAroundWorldZ(lYaw);*/
					lAxis[0] = lOrientation*lAxis[0];
					lAxis[1] = lOrientation*lAxis[1];
					lAxis[2] = lOrientation*lAxis[2];
				}
				vec3 lOffset;
				while (lGeometry->GetJointType() == ChunkyBoneGeometry::JOINT_EXCLUDE)
				{
					ChunkyBoneGeometry* lParent = lGeometry->GetParent();
					if (!lParent)
					{
						break;
					}
					vec3 lMayaOffset = lGeometry->GetOriginalOffset();
					std::swap(lMayaOffset.y, lMayaOffset.z);
					lMayaOffset.y = -lMayaOffset.y;
					lOffset += lMayaOffset;
					lGeometry = lParent;
				}
				vec3 lVelocityVector;
				pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lVelocityVector);
				//lVelocityVector = lRotation*lVelocityVector;
				vec3 lPushVector;
				for (int i = ASPECT_PRIMARY; i <= ASPECT_TERTIARY; ++i)
				{
					lPushVector += mValue[i] * lAxis[i];
				}
				const float lPushForce = lPushVector.GetLength();
				if (lPushForce > 0.1f || mFriction != 0)
				{
					if (lPushForce > 0.1f)
					{
						lVelocityVector = lVelocityVector.ProjectOntoPlane(lPushVector);
					}
					if (lPushVector.Dot(lVelocityVector) >= 0)
					{
						//mLog.Infof(_T("Reducing push vector (%f; %f; %f) with velocity (%f; %f; %f) and friction."),
						//	lPushVector.x, lPushVector.y, lPushVector.z,
						//	lVelocityVector.x, lVelocityVector.y, lVelocityVector.z);						lVelocityVector /= mMaxSpeed;
						lVelocityVector *= mFriction*0.5f / mMaxSpeed;
						lPushVector -= lVelocityVector;
					}
					pPhysicsManager->AddForceAtRelPos(lGeometry->GetBodyId(), lPushVector*mStrength*lScale, lOffset);
				}
				mIntensity += lPushForce;
			}
			break;
			case ENGINE_HOVER:
			{
				if (lPrimaryForce != 0 || mValue[ASPECT_SECONDARY] != 0)
				{
					// Arcade stabilization for lifter (typically hovercraft, elevator or similar vehicle).
					vec3 lLiftPivot = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId()) + vec3(0,0,1)*mFriction*lScale;

					const vec3 lLiftForce = vec3(0,0,1)*mStrength*lScale;
					pPhysicsManager->AddForceAtPos(lGeometry->GetBodyId(), lLiftForce, lLiftPivot);
				}
			}
			break;
			case ENGINE_HINGE_GYRO:
			{
				// Apply a fake gyro torque to parent in order to emulate a heavier gyro than
				// it actually is. The gyro must be light weight, or physics simulation will be
				// unstable when rolling bodies around any other axis than the hinge one.
				deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT && mFriction >= 0)
				{
					vec3 lAxis;
					pPhysicsManager->GetAxis1(lGeometry->GetJointId(), lAxis);
					vec3 lY;
					vec3 lZ;
					lAxis.GetNormalized().GetOrthogonals(lY, lZ);
					const float lStrength = 3 * lPrimaryForce * mStrength;
					lZ *= lStrength;
					vec3 lPos;
					pPhysicsManager->GetAnchorPos(lGeometry->GetJointId(), lPos);
					pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lZ, lPos+lY);
					pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), -lZ, lPos-lY);
				}
			}
			// TRICKY: fall through.
			case ENGINE_HINGE_ROLL:
			{
				//deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT)
				{
					float lValue = lPrimaryForce;
					float lDirectionalMaxSpeed = ((lValue >= 0)? mMaxSpeed : -mMaxSpeed2) * lValue;
					float lRotationSpeed;
					pPhysicsManager->GetAngleRate2(lGeometry->GetJointId(), lRotationSpeed);
					const float lIntensity = lRotationSpeed / mMaxSpeed;
					mIntensity += std::abs(lIntensity);
					if (mEngineType == ENGINE_HINGE_GYRO)
					{
						lValue = (lValue+1)*0.5f;
						lDirectionalMaxSpeed = lValue * (mMaxSpeed - mMaxSpeed2) + mMaxSpeed2;
					}
					else if (mEngineType == ENGINE_HINGE_ROLL)
					{
						//if (lValue > 0)
						{
							// Torque curve approximation, (tested it out, looks ok to me):
							//   -8*(x-0.65)^2*(x-0.02) + 1
							//
							// Starts at about 100 % strength at 0 RPM, local strength minimum of approximately 75 %
							// at about 25 % RPM, maximum (in range) of 100 % strength at 65 % RPM, and drops to close
							// to 0 % strength at 100 % RPM.
							const float lSquare = lIntensity - 0.65f;
							lValue *= -8 * lSquare * lSquare * (lIntensity-0.02f) + 1;
						}
					}
					const float lUsedStrength = mStrength*(std::abs(lValue) + std::abs(mFriction));
					float lPreviousStrength = 0;
					float lPreviousTargetSpeed = 0;
					pPhysicsManager->GetAngularMotorRoll(lGeometry->GetJointId(), lPreviousStrength, lPreviousTargetSpeed);
					const float lTargetSpeed = Math::Lerp(lPreviousTargetSpeed, lDirectionalMaxSpeed*lScale, 0.5f);
					const float lTargetStrength = Math::Lerp(lPreviousStrength, lUsedStrength, 0.5f);
					pPhysicsManager->SetAngularMotorRoll(lGeometry->GetJointId(), lTargetStrength, lTargetSpeed);
				}
				else
				{
					mLog.AError("Missing roll joint!");
				}
			}
			break;
			case ENGINE_HINGE_BRAKE:
			{
				//deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT)
				{
					// "Max speed" used as a type of "break threashold", so that a joystick or similar
					// won't start breaking on the tiniest movement. "Scaling" here determines part of
					// functionality (such as only affecting some wheels), may be positive or negative.
					const float lAbsValue = std::abs(lPrimaryForce);
					if (lAbsValue > mMaxSpeed && lPrimaryForce < lScale)
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
				deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT)
				{
					const vec3 lRotorForce = GetRotorLiftForce(pPhysicsManager, lGeometry, lEngineNode);
					vec3 lLiftForce = lRotorForce * lPrimaryForce;
					const int lParentBone = pStructure->GetIndex(lGeometry->GetParent());
					const quat lOrientation =
						pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
						pStructure->GetOriginalBoneTransformation(lParentBone).GetOrientation().GetInverse();

					vec3 lRotorPivot;
					pPhysicsManager->GetAnchorPos(lGeometry->GetJointId(), lRotorPivot);
					const vec3 lOffset =
						pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
						vec3(0, 0, mMaxSpeed*lScale);
					lRotorPivot += lOffset;

					const float lAbsFriction = std::abs(mFriction);
					if (mFriction < 0)
					{
						// Arcade stabilization for VTOL rotor.
						vec3 lParentAngularVelocity;
						pPhysicsManager->GetBodyAngularVelocity(lGeometry->GetParent()->GetBodyId(), lParentAngularVelocity);
						lParentAngularVelocity = lOrientation.GetInverse() * lParentAngularVelocity;
						const vec3 lParentAngle = lOrientation.GetInverse() * vec3(0, 0, 1);	// TRICKY: assumes original joint direction is towards heaven.
						const float lStabilityX = -lParentAngle.x * 0.5f + lParentAngularVelocity.y * lAbsFriction;
						const float lStabilityY = -lParentAngle.y * 0.5f - lParentAngularVelocity.x * lAbsFriction;
						lRotorPivot += lOrientation * vec3(lStabilityX, lStabilityY, 0);
					}

					// Smooth rotor force - for digital controls and to make acceleration seem more realistic.
					const float lSmooth = lNormalizedFrameTime * 0.05f * lEngineNode.mScale;
					lLiftForce.x = mSmoothValue[ASPECT_PRIMARY] = Math::Lerp(mSmoothValue[ASPECT_PRIMARY], lLiftForce.x, lSmooth);
					lLiftForce.y = mSmoothValue[ASPECT_SECONDARY] = Math::Lerp(mSmoothValue[ASPECT_SECONDARY], lLiftForce.y, lSmooth);
					lLiftForce.z = mSmoothValue[ASPECT_TERTIARY] = Math::Lerp(mSmoothValue[ASPECT_TERTIARY], lLiftForce.z, lSmooth);

					// Counteract rotor's movement through perpendicular air.
					vec3 lDragForce;
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
				deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT)
				{
					const vec3 lLiftForce = GetRotorLiftForce(pPhysicsManager, lGeometry, lEngineNode) * std::abs(lPrimaryForce);
					const int lParentBone = pStructure->GetIndex(lGeometry->GetParent());
					const float lPlacement = (lPrimaryForce >= 0)? 1.0f : -1.0f;
					const vec3 lOffset =
						pPhysicsManager->GetBodyOrientation(lGeometry->GetParent()->GetBodyId()) *
						pStructure->GetOriginalBoneTransformation(lParentBone).GetOrientation().GetInverse() *
						vec3(lPlacement*mMaxSpeed, -lPlacement*mMaxSpeed2, 0);
					const vec3 lWorldPos = lOffset + pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
					pPhysicsManager->AddForceAtPos(lGeometry->GetParent()->GetBodyId(), lLiftForce, lWorldPos);
					//{
					//	static int cnt = 0;
					//	if ((++cnt)%300 == 0)
					//	{
					//		//vec3 r = pPhysicsManager->GetBodyOrientation(lGeometry->GetBodyId()).GetInverse() * lRelPos;
					//		//vec3 r = lRelPos;
					//		vec3 r = lOffset;
					//		vec3 w = pPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
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
			case ENGINE_JET:
			{
				ChunkyBoneGeometry* lRootGeometry = pStructure->GetBoneGeometry(0);
				vec3 lVelocity;
				pPhysicsManager->GetBodyVelocity(lRootGeometry->GetBodyId(), lVelocity);
				if (lPrimaryForce != 0 && lVelocity.GetLengthSquared() < mMaxSpeed*mMaxSpeed)
				{
					const quat lOrientation =
						pPhysicsManager->GetBodyOrientation(lRootGeometry->GetBodyId()) *
						pStructure->GetOriginalBoneTransformation(0).GetOrientation().GetInverse();
					const vec3 lPushForce = lOrientation * vec3(0, lPrimaryForce*mStrength, 0);
					pPhysicsManager->AddForce(lGeometry->GetBodyId(), lPushForce);
				}
				mIntensity += lPrimaryForce;
			}
			break;
			case ENGINE_SLIDER_FORCE:
			{
				deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT)
				{
					if (!lPrimaryForce && lEngineNode.mMode == MODE_NORMAL)	// Normal slider behavior is to pull back to origin while half-lock keep last motor target.
					{
						float lPosition;
						pPhysicsManager->GetSliderPos(lGeometry->GetJointId(), lPosition);
						if (!Math::IsEpsEqual(lPosition, 0.0f, 0.1f))
						{
							float lValue = -lPosition*std::abs(lScale);
							lValue = (lValue > 0)? lValue*mMaxSpeed : lValue*mMaxSpeed2;
							pPhysicsManager->SetMotorTarget(lGeometry->GetJointId(), mStrength, lValue);
						}
					}
					else if (!lPrimaryForce && lEngineNode.mMode == MODE_RELEASE)	// Release slider behavior just lets go.
					{
						pPhysicsManager->SetMotorTarget(lGeometry->GetJointId(), 0, 0);
					}
					else
					{
						const float lValue = (lPrimaryForce > 0)? lPrimaryForce*mMaxSpeed : lPrimaryForce*mMaxSpeed2;
						pPhysicsManager->SetMotorTarget(lGeometry->GetJointId(), mStrength, lValue*lScale);
					}
					float lSpeed = 0;
					pPhysicsManager->GetSliderSpeed(lGeometry->GetJointId(), lSpeed);
					mIntensity += std::abs(lSpeed) / mMaxSpeed;
				}
			}
			break;
			case ENGINE_GLUE:
			case ENGINE_BALL_BRAKE:
			{
				deb_assert(lGeometry->GetJointId() != INVALID_JOINT);
				if (lGeometry->GetJointId() != INVALID_JOINT)
				{
					pPhysicsManager->StabilizeJoint(lGeometry->GetJointId());
				}
			}
			break;
			case ENGINE_YAW_BRAKE:
			{
				const Tbc::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId();
				vec3 lAngularVelocity;
				pPhysicsManager->GetBodyAngularVelocity(lBodyId, lAngularVelocity);
				// Reduce rotation of craft.
				lAngularVelocity.z *= mFriction;
				const float lLowAngularVelocity = mMaxSpeed;
				if (Math::IsEpsEqual(lPrimaryForce, 0.0f) && std::abs(lAngularVelocity.z) < lLowAngularVelocity)
				{
					// Seriously kill speed depending on strength.
					lAngularVelocity.z *= 1/mStrength;
				}
				pPhysicsManager->SetBodyAngularVelocity(lBodyId, lAngularVelocity);
			}
			break;
			case ENGINE_AIR_BRAKE:
			{
				//      1
				// F =  - pv^2 C  A
				//  D   2       d
				const float pCdA = 0.5f * 1.225f * mFriction;	// Density of air multiplied with friction coefficient and area (the two latter combined in mFriction).
				const Tbc::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId();
				vec3 lVelocity;
				pPhysicsManager->GetBodyVelocity(lBodyId, lVelocity);
				const float lSpeed = lVelocity.GetLength();
				const vec3 lDrag = lVelocity * -lSpeed * pCdA;
				pPhysicsManager->AddForce(lBodyId, lDrag);
			}
			break;
			default:
			{
				deb_assert(false);
			}
			break;
		}
	}
	mIntensity /= mEngineNodeArray.size();
}

vec3 PhysicsEngine::GetCurrentMaxSpeed(const PhysicsManager* pPhysicsManager) const
{
	vec3 lMaxVelocity;
	float lMaxSpeed = 0;
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		const EngineNode& lEngineNode = *i;
		ChunkyBoneGeometry* lGeometry = lEngineNode.mGeometry;
		vec3 lVelocity;
		pPhysicsManager->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
		const float lSpeed = lVelocity.GetLengthSquared();
		if (lSpeed > lMaxSpeed)
		{
			lMaxSpeed = lSpeed;
			lMaxVelocity = lVelocity;
		}
	}
	return lMaxVelocity;
}



void PhysicsEngine::UprightStabilize(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure,
	const ChunkyBoneGeometry* pGeometry, float pStrength, float pFriction)
{
	const int lRootBone = 0;	// Use root bone for fetching original transform, or "up" will be off.
	const quat lOrientation =
		pPhysicsManager->GetBodyOrientation(pGeometry->GetBodyId()) *
		pStructure->GetOriginalBoneTransformation(lRootBone).GetOrientation().GetInverse();
	// 1st: angular velocity damping (skipping z).
	vec3 lAngular;
	pPhysicsManager->GetBodyAngularVelocity(pGeometry->GetBodyId(), lAngular);
	lAngular = lOrientation.GetInverse() * lAngular;
	lAngular.z = 0;
	lAngular *= -pFriction;
	vec3 lTorque = lOrientation * lAngular;
	// 2nd: strive towards straight.
	lAngular = lOrientation * vec3(0, 0, 1);
	lTorque += vec3(+lAngular.y * pFriction*5, -lAngular.x * pFriction*5, 0);
	pPhysicsManager->AddTorque(pGeometry->GetBodyId(), lTorque*pStrength);
}

void PhysicsEngine::ForwardStabilize(PhysicsManager* pPhysicsManager, const ChunkyPhysics* pStructure,
	const ChunkyBoneGeometry* pGeometry, float pStrength, float pFriction)
{
	const int lBone = pStructure->GetIndex(pGeometry);
	const quat lOrientation =
		pPhysicsManager->GetBodyOrientation(pGeometry->GetBodyId()) *
		pStructure->GetOriginalBoneTransformation(lBone).GetOrientation().GetInverse();
	// 1st: angular velocity damping in Z-axis.
	vec3 lVelocity3d;
	pPhysicsManager->GetBodyAngularVelocity(pGeometry->GetBodyId(), lVelocity3d);
	const float lAngularVelocity = lVelocity3d.z;
	// 2nd: strive towards straight towards where we're heading.
	pPhysicsManager->GetBodyVelocity(pGeometry->GetBodyId(), lVelocity3d);
	vec2 lVelocity2d(lVelocity3d.x, lVelocity3d.y);
	const vec3 lForward3d = lOrientation * vec3(0, 1, 0);
	vec2 lForward2d(lForward3d.x, lForward3d.y);
	if (lForward2d.GetLengthSquared() > 0.1f && lVelocity2d.GetLengthSquared() > 0.3f)
	{
		float lAngle = lForward2d.GetAngle(lVelocity2d);
		if (lAngle > PIF)
		{
			lAngle -= 2*PIF;
		}
		else if (lAngle < -PIF)
		{
			lAngle += 2*PIF;
		}
		lVelocity3d.Set(0, 0, (6*lAngle - lAngularVelocity*pFriction*1.5f) * pStrength);
		pPhysicsManager->AddTorque(pGeometry->GetBodyId(), lVelocity3d);
	}
}



unsigned PhysicsEngine::GetControllerIndex() const
{
	return (mControllerIndex);
}

float PhysicsEngine::GetValue() const
{
	deb_assert(mControllerIndex >= 0 && mControllerIndex < MAX_ASPECT_INDEX);
	if (mEngineType == ENGINE_PUSH_RELATIVE || mEngineType == ENGINE_PUSH_ABSOLUTE)
	{
		const float a = std::abs(mValue[ASPECT_PRIMARY]);
		const float b = std::abs(mValue[ASPECT_SECONDARY]);
		const float c = std::abs(mValue[ASPECT_TERTIARY]);
		if (a > b && a > c)
		{
			return mValue[ASPECT_PRIMARY];
		}
		if (b > c)
		{
			return mValue[ASPECT_SECONDARY];
		}
		return mValue[ASPECT_TERTIARY];
	}
	return mValue[ASPECT_PRIMARY];
}

const float* PhysicsEngine::GetValues() const
{
	return mValue;
}

float PhysicsEngine::GetIntensity() const
{
	return (mIntensity);
}

float PhysicsEngine::GetMaxSpeed() const
{
	return (mMaxSpeed);
}

float PhysicsEngine::GetLerpThrottle(float pUp, float pDown, bool pAbs) const
{
	float& lLerpShadow = pAbs? mSmoothValue[ASPECT_LOCAL_SHADOW] : mSmoothValue[ASPECT_LOCAL_SHADOW_ABS];
	float lValue = GetPrimaryValue();
	lValue = pAbs? std::abs(lValue) : lValue;
	lLerpShadow = Math::Lerp(lLerpShadow, lValue, (lValue > lLerpShadow)? pUp : pDown);
	return lLerpShadow;
}

bool PhysicsEngine::HasEngineMode(EngineMode pMode) const
{
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		if (i->mMode == pMode)
		{
			return true;
		}
	}
	return false;
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



float PhysicsEngine::GetPrimaryValue() const
{
	const float lForce = GetValue();
	const float lPrimaryForce = (mValue[ASPECT_LOCAL_PRIMARY] > std::abs(lForce))? mValue[ASPECT_LOCAL_PRIMARY] : lForce;
	return lPrimaryForce;
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
	deb_assert(mControllerIndex >= 0 && mControllerIndex < MAX_ASPECT_INDEX);
	const int lControlledNodeCount = Endian::BigToHost(lData[6]);
	int x;
	for (x = 0; x < lControlledNodeCount; ++x)
	{
		ChunkyBoneGeometry* lGeometry = pStructure->GetBoneGeometry(Endian::BigToHost(lData[7+x*3]));
		deb_assert(lGeometry);
		float lScale = Endian::BigToHostF(lData[8+x*3]);
		EngineMode lMode = (EngineMode)Endian::BigToHost(lData[9+x*3]);
		AddControlledGeometry(lGeometry, lScale, lMode);
	}
}



vec3 PhysicsEngine::GetRotorLiftForce(PhysicsManager* pPhysicsManager, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const
{
	vec3 lAxis;
	pPhysicsManager->GetAxis1(pGeometry->GetJointId(), lAxis);
	float lAngularRotorSpeed = 0;
	pPhysicsManager->GetAngleRate1(pGeometry->GetJointId(), lAngularRotorSpeed);
	const float lLiftForce = lAngularRotorSpeed*lAngularRotorSpeed*mStrength*pEngineNode.mScale;
	return (lAxis*lLiftForce);
}

void PhysicsEngine::ApplyTorque(PhysicsManager* pPhysicsManager, float pFrameTime, ChunkyBoneGeometry* pGeometry, const EngineNode& pEngineNode) const
{
	pFrameTime;

	//deb_assert(pGeometry->GetJointId() != INVALID_JOINT);
	if (pGeometry->GetJointId() == INVALID_JOINT)
	{
		mLog.AError("Missing torque joint!");
		return;
	}

	float lForce = GetPrimaryValue();

	const float lScale = pEngineNode.mScale;
	//const float lReverseScale = (lScale + 1) * 0.5f;	// Move towards linear scaling.
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
		mIntensity += std::abs(lTargetSpeed - lActualSpeed) / mMaxSpeed;
		return;
	}

	float lIrlAngle;
	if (!pPhysicsManager->GetAngle1(pGeometry->GetJointId(), lIrlAngle))
	{
		mLog.AError("Bad joint angle!");
		return;
	}
	// Flip angle if parent is "world".
	if (pPhysicsManager->IsStaticBody(pGeometry->GetParent()->GetBodyId()))
	{
		lIrlAngle = -lIrlAngle;
	}
	const float lIrlAngleDirection = (lHiStop < lLoStop)? -lIrlAngle : lIrlAngle;

	if (pEngineNode.mMode == MODE_HALF_LOCK)
	{
		if ((lForce < 0.02f && lIrlAngleDirection < lTarget) ||
			(lForce > -0.02f && lIrlAngleDirection > lTarget))
		{
			if (std::abs(lForce) > 0.02)
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
		vec3 lParentVelocity;
		pPhysicsManager->GetBodyVelocity(pGeometry->GetParent()->GetBodyId(), lParentVelocity);
		const float lRangeFactor = ::pow(mFriction, lParentVelocity.GetLength());
		lHiStop *= lRangeFactor;
		lLoStop *= lRangeFactor;
	}
	const float lAngleSpan = (lHiStop-lLoStop)*0.9f;
	const float lTargetAngle = Math::Lerp(lLoStop, lHiStop, lScale*lForce*0.5f+0.5f);
	const float lDiff = (lTargetAngle-lIrlAngle);
	const float lAbsDiff = std::abs(lDiff);
	float lTargetSpeed;
	const float lAbsBigDiff = std::abs(lAngleSpan/7);
	const bool lCloseToGoal = (lAbsDiff < lAbsBigDiff);
	if (!lCloseToGoal)
	{
		lTargetSpeed = (lDiff > 0)? mMaxSpeed : -mMaxSpeed;
	}
	else
	{
		lTargetSpeed = mMaxSpeed*lDiff/lAbsBigDiff;
	}
	//lTargetSpeed *= (lForce > 0)? lScale : lReverseScale;
	// If we're far from the desired target speed, we speed up.
	float lCurrentSpeed = 0;
	if (mEngineType == ENGINE_HINGE2_TURN)
	{
		pPhysicsManager->GetAngleRate2(pGeometry->GetJointId(), lCurrentSpeed);
		if (!lCloseToGoal)
		{
			lTargetSpeed *= 1+std::abs(lCurrentSpeed)/30;
		}
	}
	else
	{
		pPhysicsManager->GetAngleRate1(pGeometry->GetJointId(), lCurrentSpeed);
		if (!lCloseToGoal)
		{
			lTargetSpeed += (lTargetSpeed-lCurrentSpeed) * std::abs(lScale);
		}
	}
	/*if (Math::IsEpsEqual(lTargetSpeed, 0.0f, 0.01f))	// Stop when almost already at a halt.
	{
		lTargetSpeed = 0;
	}*/
	pPhysicsManager->SetAngularMotorTurn(pGeometry->GetJointId(), mStrength, lTargetSpeed);
	mIntensity += std::abs(lTargetSpeed / (mMaxSpeed * lScale));
}



loginstance(PHYSICS, PhysicsEngine);



}
