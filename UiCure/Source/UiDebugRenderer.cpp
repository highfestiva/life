
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiDebugRenderer.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/ContextObject.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Tbc/Include/ChunkyBoneGeometry.h"
#include "../../Tbc/Include/ChunkyPhysics.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiRuntimeVariableName.h"



namespace UiCure
{



DebugRenderer::DebugRenderer(const Cure::RuntimeVariableScope* pVariableScope, GameUiManager* pUiManager, const Cure::ContextManager* pContext, const Cure::ContextManager* pRemoteContext, Lock* pTickLock):
	mVariableScope(pVariableScope),
	mUiManager(pUiManager),
	mContext(pContext),
	mRemoteContext(pRemoteContext),
	mTickLock(pTickLock)
{
}

DebugRenderer::~DebugRenderer()
{
}



void DebugRenderer::Render(const GameUiManager* pUiManager, const PixelRect& pRenderArea)
{
	bool lDebugAxes;
	bool lDebugJoints;
	bool lDebugShapes;
	CURE_RTVAR_GET(lDebugAxes, =, GetSettings(), RTVAR_DEBUG_3D_ENABLEAXES, false);
	CURE_RTVAR_GET(lDebugJoints, =, GetSettings(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	CURE_RTVAR_GET(lDebugShapes, =, GetSettings(), RTVAR_DEBUG_3D_ENABLESHAPES, false);
	if (lDebugAxes || lDebugJoints || lDebugShapes)
	{
		ScopeLock lLock(mTickLock);
		pUiManager->GetRenderer()->ResetClippingRect();
		pUiManager->GetRenderer()->SetClippingRect(pRenderArea);
		pUiManager->GetRenderer()->SetViewport(pRenderArea);

		const Cure::ContextManager::ContextObjectTable& lObjectTable = mContext->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			Cure::ContextObject* lObject;
			if (mRemoteContext)
			{
				if (mContext->IsLocalGameObjectId(x->first))
				{
					continue;
				}
				lObject = mRemoteContext->GetObject(x->first);
				if (!lObject)
				{
					lObject = x->second;
				}
			}
			else
			{
				lObject = x->second;
			}
			if (lDebugAxes)
			{
				DebugDrawPrimitive(lObject, DEBUG_AXES);
			}
			if (lDebugJoints)
			{
				DebugDrawPrimitive(lObject, DEBUG_JOINTS);
			}
			if (lDebugShapes)
			{
				DebugDrawPrimitive(lObject, DEBUG_SHAPES);
			}
		}
	}
}

void DebugRenderer::DebugDrawPrimitive(Cure::ContextObject* pObject, DebugPrimitive pPrimitive)
{
	if (!pObject->GetPhysics())
	{
		return;
	}

	Cure::ContextManager* lContextManager = pObject->GetManager();
	TransformationF lPhysicsTransform;
	const int lBoneCount = pObject->GetPhysics()->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = pObject->GetPhysics()->GetBoneGeometry(x);
		TBC::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId()? lGeometry->GetBodyId() : lGeometry->GetTriggerId();
		if (lBodyId != TBC::INVALID_BODY)
		{
			lContextManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lBodyId, lPhysicsTransform);
		}
		else if (lGeometry->GetBoneType() == TBC::ChunkyBoneGeometry::BONE_POSITION)
		{
			lBodyId = pObject->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
			lContextManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lBodyId, lPhysicsTransform);
			lPhysicsTransform.GetPosition() += lPhysicsTransform.GetOrientation() * pObject->GetPhysics()->GetOriginalBoneTransformation(x).GetPosition();
		}
		else
		{
			continue;
		}
		Vector3DF lPos = lPhysicsTransform.GetPosition();
		switch (pPrimitive)
		{
			case DEBUG_AXES:
			{
				const float lLength = 2;
				const Vector3DF& lAxisX = lPhysicsTransform.GetOrientation().GetAxisX();
				mUiManager->GetRenderer()->DrawLine(lPos, lAxisX*lLength, RED);
				const Vector3DF& lAxisY = lPhysicsTransform.GetOrientation().GetAxisY();
				mUiManager->GetRenderer()->DrawLine(lPos, lAxisY*lLength, GREEN);
				const Vector3DF& lAxisZ = lPhysicsTransform.GetOrientation().GetAxisZ();
				mUiManager->GetRenderer()->DrawLine(lPos, lAxisZ*lLength, BLUE);
			}
			break;
			case DEBUG_JOINTS:
			{
				const TBC::PhysicsManager::JointID lJoint = lGeometry->GetJointId();
				if (lJoint != TBC::INVALID_JOINT)
				{
					Vector3DF lAnchor;
					if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_SLIDER)
					{
						// Ignore, no anchor to be had.
					}
					else if (lContextManager->GetGameManager()->GetPhysicsManager()->GetAnchorPos(lJoint, lAnchor))
					{
						const float lLength = 1;
						Vector3DF lAxis;
						if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_BALL)
						{
							// Ball joints don't have axes.
							mUiManager->GetRenderer()->DrawLine(lAnchor, Vector3DF(0,0,3), BLACK);
							break;
						}
						else if (lContextManager->GetGameManager()->GetPhysicsManager()->GetAxis1(lJoint, lAxis))
						{
							mUiManager->GetRenderer()->DrawLine(lAnchor, lAxis*lLength, DARK_GRAY);
						}
						else
						{
							deb_assert(false);
						}
						if (lContextManager->GetGameManager()->GetPhysicsManager()->GetAxis2(lJoint, lAxis))
						{
							mUiManager->GetRenderer()->DrawLine(lAnchor, lAxis*lLength, BLACK);
						}
					}
					else
					{
						deb_assert(false);
					}
				}
			}
			break;
			case DEBUG_SHAPES:
			{
				const Vector3DF lSize = lGeometry->GetShapeSize() / 2;
				const QuaternionF& lRot = lPhysicsTransform.GetOrientation();
				Vector3DF lVertex[8];
				for (int x = 0; x < 8; ++x)
				{
					lVertex[x] = lPos - lRot *
						Vector3DF(lSize.x*((x&4)? 1 : -1),
							lSize.y*((x&1)? 1 : -1),
							lSize.z*((x&2)? 1 : -1));
				}
				mUiManager->GetRenderer()->DrawLine(lVertex[0], lVertex[1]-lVertex[0], YELLOW);
				mUiManager->GetRenderer()->DrawLine(lVertex[1], lVertex[3]-lVertex[1], YELLOW);
				mUiManager->GetRenderer()->DrawLine(lVertex[3], lVertex[2]-lVertex[3], YELLOW);
				mUiManager->GetRenderer()->DrawLine(lVertex[2], lVertex[0]-lVertex[2], YELLOW);
				mUiManager->GetRenderer()->DrawLine(lVertex[4], lVertex[5]-lVertex[4], MAGENTA);
				mUiManager->GetRenderer()->DrawLine(lVertex[5], lVertex[7]-lVertex[5], MAGENTA);
				mUiManager->GetRenderer()->DrawLine(lVertex[7], lVertex[6]-lVertex[7], MAGENTA);
				mUiManager->GetRenderer()->DrawLine(lVertex[6], lVertex[4]-lVertex[6], MAGENTA);
				mUiManager->GetRenderer()->DrawLine(lVertex[0], lVertex[4]-lVertex[0], CYAN);
				mUiManager->GetRenderer()->DrawLine(lVertex[1], lVertex[5]-lVertex[1], CYAN);
				mUiManager->GetRenderer()->DrawLine(lVertex[2], lVertex[6]-lVertex[2], ORANGE);
				mUiManager->GetRenderer()->DrawLine(lVertex[3], lVertex[7]-lVertex[3], ORANGE);
			}
			break;
			default:
			{
				deb_assert(false);
			}
			break;
		}
	}
}

void DebugRenderer::RenderSpline(const GameUiManager* pUiManager, Spline* pSpline)
{
	bool lDebugJoints;
	CURE_RTVAR_GET(lDebugJoints, =, GetSettings(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	if (!lDebugJoints)
	{
		return;
	}
	const float t = pSpline->GetCurrentInterpolationTime();
	for (float x = 0; x < 1; x += 0.01f)
	{
		pSpline->GotoAbsoluteTime(x);
		pUiManager->GetRenderer()->DrawLine(pSpline->GetValue(), pSpline->GetSlope() * 2, WHITE);
	}
	pSpline->GotoAbsoluteTime(t);
}


}
