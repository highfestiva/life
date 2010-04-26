
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include <math.h>
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../Include/UiCppContextObject.h"
#include "../Include/UiCure.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



CppContextObject::CppContextObject(const str& pClassId, GameUiManager* pUiManager):
	Cure::CppContextObject(pClassId),
	mUiManager(pUiManager),
	mUiClassResource(0),
	mMeshLoadCount(0),
	mSoundVolume(0),
	mSoundPitch(0),
	mTextureResource(pUiManager),
	mEngineSoundResource(mUiManager, UiLepra::SoundManager::LOOP_FORWARD)
{
	log_volatile(mLog.Tracef(_T("Construct CppCO %s."), pClassId.c_str()));
}

CppContextObject::~CppContextObject()
{
	log_volatile(mLog.Tracef(_T("Delete CppCO %X:%s."), GetInstanceId(), GetClassId().c_str()));

	for (MeshArray::iterator x = mMeshResourceArray.begin(); x != mMeshResourceArray.end(); ++x)
	{
		delete (*x);
	}
	mMeshResourceArray.clear();

	delete (mUiClassResource);
	mUiClassResource = 0;

	mUiManager = 0;
}



void CppContextObject::StartLoading()
{
	assert(mUiClassResource == 0);
	mUiClassResource = new UserClassResource(mUiManager);
	const str lClassName = _T("Data/")+GetClassId()+_T(".class");	// TODO: move to central source file.
	mUiClassResource->Load(GetManager()->GetGameManager()->GetResourceManager(), lClassName,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));

	//if (strutil::StartsWith(GetClassId(), _T("helicopter")))
	{
		const str lSoundName = _T("Data/Bark.wav");
		mEngineSoundResource.Load(GetManager()->GetGameManager()->GetResourceManager(), lSoundName,
			UserSound3dResource::TypeLoadCallback(this, &CppContextObject::OnLoadSound3d));
	}
}



void CppContextObject::OnPhysicsTick()
{
	if (!mPhysics)
	{
		mLog.Warningf(_T("Physical body for %s not loaded!"), GetClassId().c_str());
		return;
	}

	if (!GetManager()->GetGameManager()->IsUiMoveForbidden(GetInstanceId()))
	{
		// Only move UI representation of objects that are not owned by other split screen players.
		UiMove();
	}
	if (mEngineSoundResource.GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->SetSoundPosition(mEngineSoundResource.GetData(), mSoundPosition, mSoundVelocity);
		mUiManager->GetSoundManager()->SetVolume(mEngineSoundResource.GetData(), mSoundVolume);
		mUiManager->GetSoundManager()->SetPitch(mEngineSoundResource.GetData(), mSoundPitch);
	}
}

void CppContextObject::UiMove()
{
	TransformationF lPhysicsTransform;
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(lResource->GetOffset().mGeometryIndex);
		if (lGeometry == 0 || lGeometry->GetBodyId() == TBC::INVALID_BODY)
		{
			mLog.Warningf(_T("Physical body for %s not loaded!"), lResource->GetName().c_str());
			continue;
		}

		mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lGeometry->GetBodyId(), lPhysicsTransform);

		TBC::GeometryBase* lGfxGeometry = lResource->GetRamData();
		//if (GetNetworkObjectType() == Cure::NETWORK_OBJECT_REMOTE_CONTROLLED)
		/*{
			// Smooth (sliding average) to the physics position if we're close enough. Otherwise warp.
			Vector3DF lPhysicsVelocity;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lNode->GetBodyId(), lPhysicsVelocity);
			const Vector3DF& lPosition = lGfxGeometry->GetBaseTransformation().GetPosition();
			const float lCloseTime = 0.5f;
			const float lCloseStandStillDistance = 1.5f;
			float lClose = lPhysicsVelocity.GetLength()*lCloseTime;
			lClose = std::max(lCloseStandStillDistance, lClose);
			if (lPosition.GetDistanceSquared(lPhysicsTransform.GetPosition()) < lClose*lClose)
			{
				lPhysicsTransform.SetPosition(Math::Lerp(lPosition, lPhysicsTransform.GetPosition(), 0.1f));
			}
		}*/
		lGfxGeometry->SetTransformation(lPhysicsTransform);
	}

	if (mEngineSoundResource.GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(mPhysics->GetRootBone());	// TODO: add sound->engine index in class info!
		if (lGeometry == 0 || lGeometry->GetBodyId() == TBC::INVALID_BODY)
		{
			mLog.Warningf(_T("Physical body for %s not loaded!"), mEngineSoundResource.GetName().c_str());
		}
		else
		{
			Vector3DF lPosition = mManager->GetGameManager()->GetPhysicsManager()->GetBodyPosition(lGeometry->GetBodyId());
			Vector3DF lVelocity;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lGeometry->GetBodyId(), lVelocity);
			float lVolume = 1;
			float lPitch = 1;
			if (mPhysics->GetEngineCount() > 0)
			{
				const TBC::PhysicsEngine* lEngine = mPhysics->GetEngine(0);	// TODO: add sound->engine index in class info!
				const float lIntensity = ::fabs(lEngine->GetIntensity());
				const float lLowPitch = 3;
				const float lHighPitch = 8;
				lPitch = Math::Lerp(lLowPitch, lHighPitch, lIntensity);
				lVolume = (lIntensity < 1/5.0f)? lIntensity*5 : 1;
				lVolume = Math::Lerp(0.3f, 1.0f, lVolume);
			}
			Cure::ContextObject::Array lSiblings;
			mManager->GetGameManager()->GetSiblings(GetInstanceId(), lSiblings);
			Cure::ContextObject::Array::iterator x;
			for (x = lSiblings.begin(); x != lSiblings.end(); ++x)
			{
				((CppContextObject*)(*x))->OnSoundMoved(lPosition, lVelocity, lVolume, lPitch);
			}
		}
	}
}

void CppContextObject::OnSoundMoved(const Vector3DF& pPosition, const Vector3DF& pVelocity, float pVolume, float pPitch)
{
	mSoundPosition = pPosition;
	mSoundVelocity = pVelocity;
	mSoundVolume = pVolume;
	mSoundPitch = pPitch;
}



void CppContextObject::DebugDrawPrimitive(DebugPrimitive pPrimitive)
{
	if (!mPhysics)
	{
		return;
	}

	const int lBoneCount = mPhysics->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
		TBC::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId()? lGeometry->GetBodyId() : lGeometry->GetTriggerId();
		if (lBodyId != TBC::INVALID_BODY)
		{
			TransformationF lPhysicsTransform;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(
				lBodyId, lPhysicsTransform);
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
						if (mManager->GetGameManager()->GetPhysicsManager()->GetAnchorPos(lJoint, lAnchor))
						{
							const float lLength = 1;
							Vector3DF lAxis;
							if (mManager->GetGameManager()->GetPhysicsManager()->GetAxis1(lJoint, lAxis))
							{
								mUiManager->GetRenderer()->DrawLine(lAnchor, lAxis*lLength, DARK_GRAY);
							}
							else
							{
								assert(false);
							}
							if (mManager->GetGameManager()->GetPhysicsManager()->GetAxis2(lJoint, lAxis))
							{
								mUiManager->GetRenderer()->DrawLine(lAnchor, lAxis*lLength, BLACK);
							}
						}
						else
						{
							assert(false);
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
					assert(false);
				}
				break;
			}
		}
	}
}



GameUiManager* CppContextObject::GetUiManager() const
{
	return (mUiManager);
}

const UiTbc::ChunkyClass* CppContextObject::GetClass() const
{
	if (mUiClassResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mUiClassResource->GetRamData());
	}
	return (0);
}

TBC::GeometryBase* CppContextObject::GetMesh(int pIndex) const
{
	if (pIndex < (int)mMeshResourceArray.size())
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[pIndex];
		if (lResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			return (lResource->GetRamData());
		}
	}
	else
	{
		assert(false);
	}
	return (0);
}


void CppContextObject::__GetFuckedUpMeshesRemoveMe(UiTbc::ChunkyClass* pClass) const
{
	if (GetClassId().find(_T("box_002")) != str::npos)
	{
		pClass->AddMesh(0, _T("box_002_mesh"), TransformationF());
	}
	else if (GetClassId().find(_T("car_001")) != str::npos)
	{
		pClass->AddMesh(0, _T("car_001_body_mesh"), TransformationF());
		pClass->AddMesh(0, _T("car_001_top_mesh"), TransformationF(QuaternionF(), Vector3DF(0, 1, 0.6f)));
		const QuaternionF lOrientation(PIF/2, Vector3DF(1, 0, 0));
		const TransformationF lTransform(lOrientation, Vector3DF(0.9f, 0.9f, 1.2f));
		pClass->AddMesh(0, _T("car_001_car_antenna_mesh"), lTransform);
		pClass->AddMesh(2, _T("car_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(3, _T("car_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(4, _T("car_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(5, _T("car_001_wheel_mesh"), TransformationF());
	}
	else if (GetClassId().find(_T("monster_001")) != str::npos)
	{
		pClass->AddMesh(0, _T("monster_001_body_mesh"), TransformationF());
		pClass->AddMesh(0, _T("monster_001_top_mesh"), TransformationF(QuaternionF(), Vector3DF(0, 0, 0.7f)));
		const QuaternionF lOrientation(PIF/2, Vector3DF(1, 0, 0));
		const TransformationF lTransform(lOrientation, Vector3DF(1.4f, 1, 2.4f));
		pClass->AddMesh(0, _T("monster_001_car_antenna_mesh"), lTransform);
		pClass->AddMesh(2, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(3, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(4, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(5, _T("monster_001_wheel_mesh"), TransformationF());
	}
	else if (GetClassId().find(_T("excavator_703")) != str::npos)
	{
		pClass->AddMesh(0, _T("excavator_703_body_mesh"), TransformationF());
		pClass->AddMesh(0, _T("excavator_703_top_mesh"), TransformationF(QuaternionF(), Vector3DF(1.0f, -0.75f, 1.00f)));
		pClass->AddMesh(2, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(3, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(4, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(5, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(6, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(7, _T("monster_001_wheel_mesh"), TransformationF());
		pClass->AddMesh(8, _T("excavator_703_boom1_mesh"), TransformationF());
		const float lBoom2Angle = PIF/4;
		TransformationF lBoom2Offset;
		lBoom2Offset.RotatePitch(lBoom2Angle);
		lBoom2Offset.GetPosition().Set(0, -3.2f/2*::sin(lBoom2Angle), (2.5f+3.2f*::cos(lBoom2Angle))/2);
		pClass->AddMesh(8, _T("excavator_703_boom2_mesh"), lBoom2Offset);
		pClass->AddMesh(10, _T("excavator_703_arm_mesh"), TransformationF());
		pClass->AddMesh(11, _T("excavator_703_bucket_back_mesh"), TransformationF());
		const float lBucketBackAngle = PIF/4;
		const float lBucketFloorAngle = PIF/4;
		TransformationF lBucketFloorOffset;
		lBucketFloorOffset.RotatePitch(lBucketBackAngle+lBucketFloorAngle);
		lBucketFloorOffset.GetPosition().Set(0, -0.4f, -0.5f);
		pClass->AddMesh(11, _T("excavator_703_bucket_floor_mesh"), lBucketFloorOffset);
	}
	else if (GetClassId().find(_T("crane_whatever")) != str::npos)
	{
		pClass->AddMesh(0, _T("crane_whatever_tower_mesh"), TransformationF());
		pClass->AddMesh(1, _T("crane_whatever_jib_mesh0"), TransformationF());
		TransformationF lWireTransform(QuaternionF(PIF/2,  Vector3DF(1, 0, 0)), Vector3DF());
		pClass->AddMesh(2, _T("crane_whatever_wire_mesh1"), lWireTransform);
		pClass->AddMesh(3, _T("crane_whatever_wire_mesh2"), lWireTransform);
		pClass->AddMesh(4, _T("crane_whatever_wire_mesh3"), lWireTransform);
		pClass->AddMesh(5, _T("crane_whatever_wire_mesh4"), lWireTransform);
		pClass->AddMesh(6, _T("crane_whatever_hook_mesh"), lWireTransform);
	}
	else if (GetClassId().find(_T("ground_002")) != str::npos)
	{
		pClass->AddMesh(0, _T("ground_002_0_mesh"), TransformationF());
		pClass->AddMesh(1, _T("ground_002_1_mesh"), TransformationF());
		pClass->AddMesh(2, _T("ground_002_2_mesh"), TransformationF());
		pClass->AddMesh(3, _T("ground_002_3_mesh"), TransformationF());
		pClass->AddMesh(4, _T("ground_002_4_mesh"), TransformationF());
		pClass->AddMesh(5, _T("ground_002_5_mesh"), TransformationF());
	}
	else
	{
		assert(false);
	}
}



void CppContextObject::OnLoadClass(UserClassResource* pClassResource)
{
	UiTbc::ChunkyClass* lClass = pClassResource->GetData();

	if (pClassResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		Parent::__StartLoadingFuckedUpPhysicsRemoveMe(pClassResource, lClass);
		__GetFuckedUpMeshesRemoveMe((UiTbc::ChunkyClass*)lClass);
		/* TODO: put me back!
		mLog.Errorf(_T("Could not load class '%s'."), pClassResource->GetName().c_str());
		assert(false);
		return;*/
	}
	else
	{
		StartLoadingPhysics(lClass->GetPhysicsBaseName());
	}

	assert(mMeshLoadCount == 0);
	Cure::ResourceManager* lResourceManager = GetManager()->GetGameManager()->GetResourceManager();
	const size_t lMeshCount = lClass->GetMeshCount();
	assert(lMeshCount > 0);
	for (size_t x = 0; x < lMeshCount; ++x)
	{
		int lPhysIndex = -1;
		str lMeshName;
		TransformationF lTransform;
		lClass->GetMesh(x, lPhysIndex, lMeshName, lTransform);
		str lMeshInstance;
		strutil::strvec lMeshNameList = strutil::Split(lMeshName, _T(";"), 1);
		lMeshName = lMeshNameList[0];
		if (lMeshNameList.size() == 1)
		{
			lMeshInstance = strutil::Format(_T("%u"), GetInstanceId());
		}
		else
		{
			lMeshInstance = strutil::Format(_T("%s_%u"), lMeshNameList[1].c_str(), GetInstanceId());
		}
		UiCure::UserGeometryReferenceResource* lMesh = new UiCure::UserGeometryReferenceResource(
			mUiManager, UiCure::GeometryOffset(lPhysIndex, lTransform));
		mMeshResourceArray.push_back(lMesh);
		lMesh->Load(lResourceManager,
			strutil::Format(_T("%s.mesh;%s"), lMeshName.c_str(), lMeshInstance.c_str()),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,&CppContextObject::OnLoadMesh));
	}
	// TODO: not everybody should load the texture, not everybody should load *A* texture. Load from group file definition.
	mTextureResource.Load(lResourceManager, _T("Data/Checker.tga"),
		UiCure::UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
}

void CppContextObject::OnLoadMesh(UserGeometryReferenceResource* pMeshResource)
{
	++mMeshLoadCount;
	if (pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		size_t lMeshIndex = (size_t)-1;
		for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
		{
			if (mMeshResourceArray[x] == pMeshResource)
			{
				lMeshIndex = x;
				break;
			}
		}
		assert((int)lMeshIndex >= 0);
		const UiTbc::ChunkyClass::Material& lLoadedMaterial =
			((UiTbc::ChunkyClass*)mUiClassResource->GetRamData())->GetMaterial(lMeshIndex);
		TBC::GeometryBase::BasicMaterialSettings lMaterial(lLoadedMaterial.mAmbient,
			lLoadedMaterial.mDiffuse, lLoadedMaterial.mSpecular,
			lLoadedMaterial.mShininess, lLoadedMaterial.mAlpha, true);
		pMeshResource->GetRamData()->SetBasicMaterialSettings(lMaterial);

		((TBC::GeometryReference*)pMeshResource->GetRamData())->SetOffsetTransformation(pMeshResource->GetOffset().mOffset);
		TryAddTexture();
	}
	else
	{
		mLog.AError("Could not load mesh! Shit.");
		assert(false);
	}
}

void CppContextObject::OnLoadTexture(UserRendererImageResource* pTextureResource)
{
	if (pTextureResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		TryAddTexture();
	}
	else
	{
		mLog.AError("Could not load texture. Gah!");
		assert(false);
	}
}

void CppContextObject::TryAddTexture()
{
	size_t lLoadCount = 0;
	for (size_t x = 0;  x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
		if (lMesh->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		++lLoadCount;
		if (mUiManager->GetRenderer()->GetMaterialType(lMesh->GetData()) == UiTbc::Renderer::MAT_NULL)
		{
			if (lMesh->GetRamData()->GetUVData(0))
			{
				if (mTextureResource.GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
				{
					UiTbc::Renderer::MaterialType lMaterialType = UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID;
					mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
					mUiManager->GetRenderer()->TryAddGeometryTexture(lMesh->GetData(), mTextureResource.GetData());
				}
			}
			else
			{
				UiTbc::Renderer::MaterialType lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID;
				if (lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha != 1)
				{
					lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED;
				}
				mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
				//UiTbc::Renderer::Shadows lShadows = UiTbc::Renderer::CAST_SHADOWS;
				//lMesh->SetShadows(lShadows);
			}
		}
	}
	if (lLoadCount == mMeshResourceArray.size())
	{
		TryComplete();
	}
}

bool CppContextObject::TryComplete()
{
	if (!mUiClassResource)
	{
		return (false);
	}
	if (mTextureResource.GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (false);
	}
	// Meshes must be both 1) all attempted, and 2) all loaded OK.
	if (mMeshLoadCount != mMeshResourceArray.size())
	{
		return (false);
	}
	for (size_t x = 0;  x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
		if (lMesh->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			return (false);
		}
	}

	if (!Parent::TryComplete())
	{
		return (false);
	}

	OnPhysicsTick();
	if (GetPhysics())
	{
		GetManager()->EnablePhysicsUpdateCallback(this);	// TODO: clear out this mess. How to use these two callback types?
	}
	return (true);
}

void CppContextObject::OnLoadSound3d(UserSound3dResource* pSoundResource)
{
	assert(pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pSoundResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		mUiManager->GetSoundManager()->Play(pSoundResource->GetData(), 0, 1.0);
	}
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
