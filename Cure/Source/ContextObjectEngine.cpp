
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ContextObjectEngine.h"
#include <assert.h>
#include "../../Lepra/Include/Math.h"
#include "../Include/ContextManager.h"
#include "../Include/ContextObject.h"
#include "../Include/GameManager.h"
#include "../Include/TimeManager.h"



namespace Cure
{



ContextObjectEngine::ContextObjectEngine(ContextObject* pContextObject, EngineType pEngineType,
	float pStrength, float pMaxSpeed, float pMaxSpeed2, unsigned pControllerIndex):
	ContextObjectAttribute(pContextObject),
	mEngineType(pEngineType),
	mStrength(pStrength),
	mMaxSpeed(pMaxSpeed),
	mMaxSpeed2(pMaxSpeed2),
	mControllerIndex(pControllerIndex)
{
	::memset(mValue, 0, sizeof(mValue));
}

ContextObjectEngine::~ContextObjectEngine()
{
}



ContextObjectEngine::Type ContextObjectEngine::GetType() const
{
	return (TYPE_ENGINE);
}

ContextObjectEngine::EngineType ContextObjectEngine::GetEngineType() const
{
	return (mEngineType);
}



void ContextObjectEngine::AddControlledNode(unsigned pPhysicsNodeId, float pScale, EngineMode pMode)
{
	mEngineNodeArray.push_back(EngineNode(pPhysicsNodeId, pScale, pMode));
}

bool ContextObjectEngine::SetValue(unsigned pAspect, float pValue, float pZAngle)
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



void ContextObjectEngine::OnTick(float pFrameTime)
{
	TBC::PhysicsEngine* lPhysicsManager = GetContextObject()->GetManager()->GetGameManager()->GetPhysicsManager();
	EngineNodeArray::const_iterator i = mEngineNodeArray.begin();
	for (; i != mEngineNodeArray.end(); ++i)
	{
		const EngineNode& lEngineNode = *i;
		PhysicsNode* lNode = GetContextObject()->GetPhysicsNode(lEngineNode.mId);
		const float lScale = lEngineNode.mScale;
		if (lNode)
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
					lPhysicsManager->GetBodyVelocity(lNode->GetBodyId(), lVelocityVector);
					lVelocityVector = lRotation*lVelocityVector;
					float lVelocity[3] = { lVelocityVector.y, lVelocityVector.x, lVelocityVector.z };
					bool lIsSpeeding = (lVelocityVector.GetLength() >= mMaxSpeed);
					for (int i = 0; i < 3; ++i)
					{
						if (!lIsSpeeding || (lVelocity[i]>0) != (mValue[i]>0))
						{
							lPhysicsManager->AddForce(lNode->GetBodyId(), mValue[i]*lAxis[i]*mStrength*lScale);
						}
					}
				}
				break;
				case ENGINE_HINGE2_ROLL:
				{
					assert(lNode->GetJointId() != TBC::INVALID_JOINT);
					if (lNode->GetJointId() != TBC::INVALID_JOINT)
					{
						// TODO: implement torque "curve" instead of constant angular force.
						const float lUsedStrength = mStrength*(::fabs(mValue[0])+0.01f);
						const float lDirectionalMaxSpeed = (mValue[0] >= 0)? mMaxSpeed : mMaxSpeed2;
						float lCurrentUsedStrength = 0;
						float lCurrentTargetSpeed = 0;
						lPhysicsManager->GetAngularMotorRoll(lNode->GetJointId(), lCurrentUsedStrength, lCurrentTargetSpeed);
						const float lTargetSpeed = Lepra::Math::Lerp(lCurrentTargetSpeed, lDirectionalMaxSpeed*mValue[0]*lScale, 0.5f);
						const float lTargetStrength = Lepra::Math::Lerp(lCurrentUsedStrength, lUsedStrength, 0.5f);
						lPhysicsManager->SetAngularMotorRoll(lNode->GetJointId(), lTargetStrength, lTargetSpeed);
					}
					else
					{
						mLog.AError("Missing roll joint!");
					}
				}
				break;
				case ENGINE_ROLL_STRAIGHT:
				{
					assert(lNode->GetJointId() != TBC::INVALID_JOINT);
					if (lNode->GetJointId() != TBC::INVALID_JOINT)
					{
						lPhysicsManager->SetAngle1(lNode->GetBodyId(), lNode->GetJointId(), 0);
					}
				}
				break;
				case ENGINE_HINGE2_TURN:
				case ENGINE_HINGE:
				{
					ApplyTorque(pFrameTime, lPhysicsManager, *lNode, lEngineNode);
				}
				break;
				case ENGINE_HINGE2_BREAK:
				{
					assert(lNode->GetJointId() != TBC::INVALID_JOINT);
					if (lNode->GetJointId() != TBC::INVALID_JOINT)
					{
						// "Max speed" used as a type of "break threashold", so that a joystick or similar
						// won't start breaking on the tiniest movement. "Scaling" here determines part of
						// functionality (such as only affecting some wheels), may be positive or negative.
						const float lAbsValue = ::fabs(mValue[0]);
						if (lAbsValue > mMaxSpeed && mValue[0] < lScale)
						{
							const float lBreakForceUsed = mStrength*lAbsValue;
							lNode->SetExtraData(1);
							lPhysicsManager->SetAngularMotorRoll(lNode->GetJointId(), lBreakForceUsed, 0);
						}
						else if (lNode->GetExtraData())
						{
							lNode->SetExtraData(0);
							lPhysicsManager->SetAngularMotorRoll(lNode->GetJointId(), 0, 0);
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
					assert(lNode->GetJointId() != TBC::INVALID_JOINT);
					if (lNode->GetJointId() != TBC::INVALID_JOINT)
					{
						lPhysicsManager->StabilizeJoint(lNode->GetJointId());
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



unsigned ContextObjectEngine::GetControllerIndex() const
{
	return (mControllerIndex);
}

float ContextObjectEngine::GetValue() const
{
	return (mValue[0]);
}

const float* ContextObjectEngine::GetValues() const
{
	return (mValue);
}



void ContextObjectEngine::ApplyTorque(float pFrameTime, TBC::PhysicsEngine* pPhysicsManager, const PhysicsNode& pNode, const EngineNode& pEngineNode)
{
	assert(pNode.GetJointId() != TBC::INVALID_JOINT);
	if (pNode.GetJointId() != TBC::INVALID_JOINT)
	{
		float lForce = mValue[0];

		const float lScale = pEngineNode.mScale;
		float lLoStop;
		float lHiStop;
		float lBounce;
		pPhysicsManager->GetJointParams(pNode.GetJointId(), lLoStop, lHiStop, lBounce);
		const float lMiddle = (lLoStop+lHiStop)*0.5f;
		if (lLoStop < -1000 || lHiStop > 1000)
		{
			// Open interval -> relative torque.
			pPhysicsManager->SetAngularMotorTurn(pNode.GetJointId(), mStrength, lForce*lScale*mMaxSpeed);
			return;
		}

		float lIrlAngle;
		if (!pPhysicsManager->GetAngle1(pNode.GetJointId(), lIrlAngle))
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
				pPhysicsManager->GetAngleRate2(pNode.GetJointId(), lAngleRate);
				lAngleSpeed *= 1+::fabs(lAngleRate)/30;
			}
			else
			{
				float lAngleRate = 0;
				pPhysicsManager->GetAngleRate1(pNode.GetJointId(), lAngleRate);
				lAngleSpeed += (lAngleSpeed-lAngleRate)*pFrameTime*mMaxSpeed2*50.0f*lScale*lScale/mMaxSpeed;

			}
			pPhysicsManager->SetAngularMotorTurn(pNode.GetJointId(), mStrength, lAngleSpeed);
		}
	}
	else
	{
		mLog.AError("Missing turn joint!");
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ContextObjectEngine);



}
