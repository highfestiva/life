
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include <assert.h>
#include <math.h>
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../Include/UiCppContextObject.h"
#include "../Include/UiCure.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



CppContextObject::CppContextObject(const Lepra::String& pClassId, GameUiManager* pUiManager):
	Cure::CppContextObject(pClassId),
	mUiManager(pUiManager),
	mTextureResource(pUiManager)
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
	mUiManager = 0;
}



void CppContextObject::OnPhysicsTick()
{
	if (!mStructure)
	{
		mLog.Warningf(_T("Physical body for %s not loaded!"), GetClassId().c_str());
		return;
	}

	Lepra::TransformationF lPhysicsTransform;
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(lResource->GetOffset().mGeometryIndex);
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
			Lepra::Vector3DF lPhysicsVelocity;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyVelocity(lNode->GetBodyId(), lPhysicsVelocity);
			const Lepra::Vector3DF& lPosition = lGfxGeometry->GetBaseTransformation().GetPosition();
			const float lCloseTime = 0.5f;
			const float lCloseStandStillDistance = 1.5f;
			float lClose = lPhysicsVelocity.GetLength()*lCloseTime;
			lClose = std::max(lCloseStandStillDistance, lClose);
			if (lPosition.GetDistanceSquared(lPhysicsTransform.GetPosition()) < lClose*lClose)
			{
				lPhysicsTransform.SetPosition(Lepra::Math::Lerp(lPosition, lPhysicsTransform.GetPosition(), 0.1f));
			}
		}*/
		lGfxGeometry->SetTransformation(lPhysicsTransform);
	}
}



bool CppContextObject::StartLoadGraphics(Cure::UserResource* pParentResource)
{
	Lepra::StringUtility::StringVector lUniqueClassArray = Lepra::StringUtility::Split(GetClassId(), _T("/"), 1);
	assert(lUniqueClassArray.size() == 2);
	Lepra::String& lIdClass = lUniqueClassArray[1];
	Cure::ResourceManager* lResourceManager = pParentResource->GetConstResource()->GetManager();

	if (GetClassId().find(_T("car_001")) != Lepra::String::npos)
	{
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0, Lepra::Vector3DF(0, 1, 0.6f))));
		const Lepra::QuaternionF lOrientation(Lepra::PIF/2, Lepra::Vector3DF(1, 0, 0));
		const Lepra::TransformationF lTransform(lOrientation, Lepra::Vector3DF(0.9f, 0.9f, 1.2f));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0, lTransform)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(2)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(3)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(4)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(5)));
		mMeshResourceArray[0]->SetParentResource(pParentResource);
		mMeshResourceArray[0]->LoadUnique(lResourceManager, lIdClass+_T("_body_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[1]->SetParentResource(pParentResource);
		mMeshResourceArray[1]->LoadUnique(lResourceManager, lIdClass+_T("_top_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[2]->SetParentResource(pParentResource);
		mMeshResourceArray[2]->LoadUnique(lResourceManager, lIdClass+_T("car_antenna_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));

		Lepra::StringUtility::StringVector lIdClassArray = Lepra::StringUtility::Split(lIdClass, _T(":"), 1);
		assert(lIdClassArray.size() == 2);
		const int lWheelBase = 3;
		for (int x = 0; x < 4; ++x)
		{
			assert((size_t)x < mMeshResourceArray.size());
			Lepra::String lWheelClass = lIdClassArray[0]+_T("_")+
				Lepra::StringUtility::IntToString(x, 10)+_T(":")+lIdClassArray[1]+_T("_wheel_mesh");
			mMeshResourceArray[x+lWheelBase]->SetParentResource(pParentResource);
			mMeshResourceArray[x+lWheelBase]->LoadUnique(lResourceManager, lWheelClass,
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		}
	}
	else if (GetClassId().find(_T("monster_001")) != Lepra::String::npos)
	{
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0, Lepra::Vector3DF(0, 0, 0.7f))));
		const Lepra::QuaternionF lOrientation(Lepra::PIF/2, Lepra::Vector3DF(1, 0, 0));
		const Lepra::TransformationF lTransform(lOrientation, Lepra::Vector3DF(1.4f, 1, 2.4f));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0, lTransform)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(2)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(3)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(4)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(5)));
		mMeshResourceArray[0]->SetParentResource(pParentResource);
		mMeshResourceArray[0]->LoadUnique(lResourceManager, lIdClass+_T("_body_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[1]->SetParentResource(pParentResource);
		mMeshResourceArray[1]->LoadUnique(lResourceManager, lIdClass+_T("_top_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[2]->SetParentResource(pParentResource);
		mMeshResourceArray[2]->LoadUnique(lResourceManager, lIdClass+_T("car_antenna_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));

		Lepra::StringUtility::StringVector lIdClassArray = Lepra::StringUtility::Split(lIdClass, _T(":"), 1);
		assert(lIdClassArray.size() == 2);
		const int lWheelBase = 3;
		for (int x = 0; x < 4; ++x)
		{
			assert((size_t)x < mMeshResourceArray.size());
			Lepra::String lWheelClass = lIdClassArray[0]+_T("_")+
				Lepra::StringUtility::IntToString(x, 10)+_T(":")+lIdClassArray[1]+_T("_wheel_mesh");
			mMeshResourceArray[x+lWheelBase]->SetParentResource(pParentResource);
			mMeshResourceArray[x+lWheelBase]->LoadUnique(lResourceManager, lWheelClass,
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		}
	}
	else if (GetClassId().find(_T("excavator_703")) != Lepra::String::npos)
	{
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0, Lepra::Vector3DF(1.0f, -0.75f, 1.00f))));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(2)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(3)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(4)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(5)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(6)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(7)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(8)));
		const float lBoom2Angle = Lepra::PIF/4;
		Lepra::TransformationF lBoom2Offset;
		lBoom2Offset.RotatePitch(lBoom2Angle);
		lBoom2Offset.GetPosition().Set(0, -3.2f/2*::sin(lBoom2Angle), (2.5f+3.2f*::cos(lBoom2Angle))/2);
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(8, lBoom2Offset)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(10)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(11)));
		const float lBucketBackAngle = Lepra::PIF/4;
		const float lBucketFloorAngle = Lepra::PIF/4;
		Lepra::TransformationF lBucketFloorOffset;
		lBucketFloorOffset.RotatePitch(lBucketBackAngle+lBucketFloorAngle);
		lBucketFloorOffset.GetPosition().Set(0, -0.4f, -0.5f);
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(11, lBucketFloorOffset)));

		mMeshResourceArray[0]->SetParentResource(pParentResource);
		mMeshResourceArray[0]->LoadUnique(lResourceManager, lIdClass+_T("_body_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[1]->SetParentResource(pParentResource);
		mMeshResourceArray[1]->LoadUnique(lResourceManager, lIdClass+_T("_top_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));

		Lepra::StringUtility::StringVector lIdClassArray = Lepra::StringUtility::Split(lIdClass, _T(":"), 1);
		assert(lIdClassArray.size() == 2);
		const int lWheelBase = 2;
		int x;
		for (x = lWheelBase; x < 6+lWheelBase; ++x)
		{
			assert((size_t)x < mMeshResourceArray.size());
			Lepra::String lWheelClass = lIdClassArray[0]+_T("_")+
				Lepra::StringUtility::IntToString(x-lWheelBase, 10)+_T(":")+_T("monster_001_wheel_mesh");
			mMeshResourceArray[x]->SetParentResource(pParentResource);
			mMeshResourceArray[x]->LoadUnique(lResourceManager, lWheelClass,
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		}

		mMeshResourceArray[x]->SetParentResource(pParentResource);
		mMeshResourceArray[x]->LoadUnique(lResourceManager, lIdClass+_T("_boom1_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		++x;
		mMeshResourceArray[x]->SetParentResource(pParentResource);
		mMeshResourceArray[x]->LoadUnique(lResourceManager, lIdClass+_T("_boom2_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		++x;
		mMeshResourceArray[x]->SetParentResource(pParentResource);
		mMeshResourceArray[x]->LoadUnique(lResourceManager, lIdClass+_T("_arm_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		++x;
		mMeshResourceArray[x]->SetParentResource(pParentResource);
		mMeshResourceArray[x]->LoadUnique(lResourceManager, lIdClass+_T("_bucket_back_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		++x;
		mMeshResourceArray[x]->SetParentResource(pParentResource);
		mMeshResourceArray[x]->LoadUnique(lResourceManager, lIdClass+_T("_bucket_floor_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		++x;
	}
	else if (GetClassId().find(_T("crane_whatever")) != Lepra::String::npos)
	{
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(1)));
		Lepra::TransformationF lWireTransform(Lepra::QuaternionF(Lepra::PIF/2,  Lepra::Vector3DF(1, 0, 0)), Lepra::Vector3DF());
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(2, lWireTransform)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(3, lWireTransform)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(4, lWireTransform)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(5, lWireTransform)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(6, lWireTransform)));
		mMeshResourceArray[0]->SetParentResource(pParentResource);
		mMeshResourceArray[0]->LoadUnique(lResourceManager, lIdClass+_T("_tower_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[1]->SetParentResource(pParentResource);
		mMeshResourceArray[1]->LoadUnique(lResourceManager, lIdClass+_T("_jib_mesh0"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[2]->SetParentResource(pParentResource);
		mMeshResourceArray[2]->LoadUnique(lResourceManager, lIdClass+_T("_wire_mesh1"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[3]->SetParentResource(pParentResource);
		mMeshResourceArray[3]->LoadUnique(lResourceManager, lIdClass+_T("_wire_mesh2"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[4]->SetParentResource(pParentResource);
		mMeshResourceArray[4]->LoadUnique(lResourceManager, lIdClass+_T("_wire_mesh3"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[5]->SetParentResource(pParentResource);
		mMeshResourceArray[5]->LoadUnique(lResourceManager, lIdClass+_T("_wire_mesh4"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[6]->SetParentResource(pParentResource);
		mMeshResourceArray[6]->LoadUnique(lResourceManager, lIdClass+_T("_hook_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
	}
	else if (GetClassId().find(_T("ground_002")) != Lepra::String::npos)
	{
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(1)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(2)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(3)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(4)));
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(5)));
		mMeshResourceArray[0]->SetParentResource(pParentResource);
		mMeshResourceArray[0]->LoadUnique(lResourceManager, lIdClass+_T("_0_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[1]->SetParentResource(pParentResource);
		mMeshResourceArray[1]->LoadUnique(lResourceManager, lIdClass+_T("_1_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[2]->SetParentResource(pParentResource);
		mMeshResourceArray[2]->LoadUnique(lResourceManager, lIdClass+_T("_2_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[3]->SetParentResource(pParentResource);
		mMeshResourceArray[3]->LoadUnique(lResourceManager, lIdClass+_T("_3_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[4]->SetParentResource(pParentResource);
		mMeshResourceArray[4]->LoadUnique(lResourceManager, lIdClass+_T("_4_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
		mMeshResourceArray[5]->SetParentResource(pParentResource);
		mMeshResourceArray[5]->LoadUnique(lResourceManager, lIdClass+_T("_5_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
	}
	else
	{
		mMeshResourceArray.push_back(new UserGeometryReferenceResource(mUiManager, GeometryOffset(0)));
		mMeshResourceArray.back()->SetParentResource(pParentResource);
		mMeshResourceArray.back()->LoadUnique(lResourceManager, lIdClass+_T("_mesh"),
			UiCure::UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
	}
	// TODO: not everybody should load the texture, not everybody should load *A* texture. Load from group file definition.
	mTextureResource.SetParentResource(pParentResource);
	mTextureResource.Load(lResourceManager, _T("Checker.tga"),
		UiCure::UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
	return (true);
}



void CppContextObject::DebugDrawAxes()
{
	const int lBoneCount = mStructure->GetBoneCount();
	for (int x = 0; x < lBoneCount; ++x)
	{
		const TBC::ChunkyBoneGeometry* lGeometry = mStructure->GetBoneGeometry(x);
		if (lGeometry->GetBodyId() != TBC::INVALID_BODY)
		{
			Lepra::TransformationF lPhysicsTransform;
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(
				lGeometry->GetBodyId(), lPhysicsTransform);
			Lepra::Vector3DF lPos = lPhysicsTransform.GetPosition();
			const float lLength = 2;
			Lepra::Vector3DF lAxis = lPhysicsTransform.GetOrientation().GetAxisX();
			mUiManager->GetRenderer()->DrawLine(lPos, lAxis*lLength, Lepra::RED);
			lAxis = lPhysicsTransform.GetOrientation().GetAxisY();
			mUiManager->GetRenderer()->DrawLine(lPos, lAxis*lLength, Lepra::GREEN);
			lAxis = lPhysicsTransform.GetOrientation().GetAxisZ();
			mUiManager->GetRenderer()->DrawLine(lPos, lAxis*lLength, Lepra::BLUE);
		}
	}
}



void CppContextObject::OnLoadMesh(UserGeometryReferenceResource* pMeshResource)
{
	if (pMeshResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
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
				mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
				//UiTbc::Renderer::Shadows lShadows = UiTbc::Renderer::CAST_SHADOWS;
				//lMesh->SetShadows(lShadows);
			}
		}
	}
	if (lLoadCount == mMeshResourceArray.size())
	{
		OnPhysicsTick();
	}
}



CppContextObjectFactory::CppContextObjectFactory(GameUiManager* pUiManager):
	mUiManager(pUiManager)
{
}

CppContextObjectFactory::~CppContextObjectFactory()
{
	mUiManager = 0;
}

CppContextObject* CppContextObjectFactory::Create(const Lepra::String& pClassId) const
{
	return (new CppContextObject(pClassId, mUiManager));
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
