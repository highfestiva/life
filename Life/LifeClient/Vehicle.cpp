
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Vehicle.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../TBC/Include/ChunkyBoneGeometry.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../RtVar.h"



namespace Life
{



Vehicle::Vehicle(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager)
{
}

Vehicle::~Vehicle()
{
}



void Vehicle::OnPhysicsTick()
{
	Parent::OnPhysicsTick();

	if (GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
	{
		return;
	}

	const TBC::ChunkyPhysics* lPhysics = GetPhysics();
	const UiTbc::ChunkyClass* lClass = GetClass();
	if (!lPhysics || !lClass)
	{
		return;
	}
	bool lIsChild = CURE_RTVAR_GETSET(mManager->GetGameManager()->GetVariableScope(), RTVAR_GAME_ISCHILD, true);
	const TBC::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	for (size_t x = 0; x < lClass->GetTagCount(); ++x)
	{
		const UiTbc::ChunkyClass::Tag& lTag = lClass->GetTag(x);
		if (lTag.mTagName == _T("eye") &&
			lTag.mFloatValueList.size() == 1 &&
			lTag.mBodyIndexList.size() == 1 &&
			lTag.mMeshIndexList.size() >= 1)
		{
			// Eyes follow steered wheels. Get wheel corresponding to eye and
			// move eye accordingly á là Lightning McQueen.
			float lJointValue = 0;
			int lBodyIndex = lTag.mBodyIndexList[0];
			TBC::ChunkyBoneGeometry* lBone = lPhysics->GetBoneGeometry(lBodyIndex);
			TBC::PhysicsManager::JointID lJoint = lBone->GetJointId();
			switch (lBone->GetJointType())
			{
				case TBC::ChunkyBoneGeometry::JOINT_HINGE2:
				{
					TBC::PhysicsManager::Joint3Diff lDiff;
					lPhysicsManager->GetJoint3Diff(lBone->GetBodyId(), lJoint, lDiff);
					float lLowStop = 0;
					float lHighStop = 0;
					float lBounce = 0;
					lPhysicsManager->GetJointParams(lJoint, lLowStop, lHighStop, lBounce);
					lJointValue = lDiff.mAngle1 * 2 / (lHighStop-lLowStop);
				}
				break;
				default:
				{
					assert(false);
					mLog.Errorf(_T("Joint type %i not implemented for tag type %s."), lBone->GetJointType(), lTag.mTagName.c_str());
				}
				break;
			}
			const float lScale = lTag.mFloatValueList[0];
			const float lJointRightValue = lJointValue * lScale;
			const float lJointDownValue = (::cos(lJointValue)-1) * lScale * 0.5f;
			for (size_t y = 0; y < lTag.mMeshIndexList.size(); ++y)
			{
				TBC::GeometryBase* lMesh = GetMesh(lTag.mMeshIndexList[y]);
				if (lMesh)
				{
					TransformationF lTransform = lMesh->GetBaseTransformation();
					lTransform.MoveRight(lJointRightValue);
					lTransform.MoveBackward(lJointDownValue);
					//lMesh->SetTransformation(lTransform);
					lMesh->SetAlwaysVisible(lIsChild);
				}
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Vehicle);



}
