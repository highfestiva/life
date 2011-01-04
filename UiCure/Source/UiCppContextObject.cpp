
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include <math.h>
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../Include/UiCppContextObject.h"
#include "../Include/UiCure.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



CppContextObject::CppContextObject(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager):
	Cure::CppContextObject(pResourceManager, pClassId),
	mUiManager(pUiManager),
	mUiClassResource(0),
	mEnableUi(true),
	mAllowRootShadow(true),
	mMeshLoadCount(0),
	mTextureLoadCount(0),
	mMeshSlideMode(MESH_SLIDE_STOP)
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

	for (TextureArray::iterator x = mTextureResourceArray.begin(); x != mTextureResourceArray.end(); ++x)
	{
		delete (*x);
	}
	mTextureResourceArray.clear();

	delete (mUiClassResource);
	mUiClassResource = 0;

	mUiManager = 0;
}



void CppContextObject::EnableUi(bool pEnable)
{
	mEnableUi = pEnable;
}

void CppContextObject::DisableRootShadow()
{
	mAllowRootShadow = false;
}



void CppContextObject::StartLoading()
{
	assert(mUiClassResource == 0);
	mUiClassResource = new UserClassResource(mUiManager);
	const str lClassName = _T("Data/")+GetClassId()+_T(".class");	// TODO: move to central source file.
	mUiClassResource->Load(GetResourceManager(), lClassName,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}



void CppContextObject::OnTick()
{
	if (!mEnableUi)
	{
		return;
	}
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
}

void CppContextObject::UiMove()
{
	const float lFrameTime = GetManager()->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime();
	const float lLerpFactor = Math::GetIterateLerpTime(0.2f, lFrameTime);
	const Vector3DF lOrigo;
	TransformationF lPhysicsTransform;
	if (mMeshSlideMode == MESH_SLIDE_START)
	{
		mMeshOffset.Set(0, 0, 0);
	}
	//QuaternionF lGfxPhysMeshAngularOffset;
	int lGfxPhysMeshOffsetCount = 0;
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		TBC::GeometryBase* lGfxGeometry = lResource->GetRamData();

		if (mPhysicsOverride != PHYSICS_OVERRIDE_BONES)
		{
			TBC::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(lResource->GetOffset().mGeometryIndex);
			if (lGeometry == 0 || lGeometry->GetBodyId() == TBC::INVALID_BODY)
			{
				mLog.Warningf(_T("Physical body for %s not loaded!"), lResource->GetName().c_str());
				continue;
			}
			mManager->GetGameManager()->GetPhysicsManager()->GetBodyTransform(lGeometry->GetBodyId(), lPhysicsTransform);

			if (mMeshSlideMode == MESH_SLIDE_START)
			{
				// Start out by fetching offset.
				Vector3DF lBasePositionOffset = lGfxGeometry->GetBaseTransformation().GetPosition();
				lBasePositionOffset.Sub(lPhysicsTransform.GetPosition());
				Math::Lerp(lBasePositionOffset, lOrigo, lLerpFactor);
				if (lBasePositionOffset.GetLengthSquared() >= 0.5f*0.5f)	// Motion becomes jerky if we allow filtering of small movements.
				{
					mMeshOffset.Add(lBasePositionOffset);
					/*if (x == 0)
					{
						lGfxPhysMeshAngularOffset = lPhysicsTransform.GetOrientation().GetInverse() * lTransform.GetOrientation();
					}*/
					lPhysicsTransform.GetPosition() += lBasePositionOffset;
				}
				++lGfxPhysMeshOffsetCount;
			}
			else if (mMeshSlideMode == MESH_SLIDE_RUN)
			{
				// Smooth (sliding average) to the physics position if we're close enough. Otherwise warp.
				lPhysicsTransform.GetPosition() += mMeshOffset;
				//lPhysicsTransform.GetOrientation() *= mMeshAngularOffset;
			}
		}
		else
		{
			lPhysicsTransform = mPhysics->GetBoneTransformation(lResource->GetOffset().mGeometryIndex);
		}

		lGfxGeometry->SetTransformation(lPhysicsTransform);
	}
	if (lGfxPhysMeshOffsetCount)
	{
		mMeshSlideMode = MESH_SLIDE_RUN;
		//mMeshOffset = lGfxPhysMeshOffset;
		mMeshOffset /= (float)lGfxPhysMeshOffsetCount;
		if (mMeshOffset.GetLengthSquared() >= 20*20)
		{
			mMeshOffset.Set(0, 0, 0);
			//mMeshAngularOffset.SetIdentity();
		}
	}
	else if (mMeshSlideMode == MESH_SLIDE_RUN && mMeshOffset.GetLengthSquared() < 0.1f)
	{
		/*QuaternionF lDiff;
		lDiff.Sub(mMeshAngularOffset);
		if (lDiff.GetNorm() < 0.01f)*/
		{
			mMeshSlideMode = MESH_SLIDE_STOP;
		}
	}
	mMeshOffset = Math::Lerp(mMeshOffset, lOrigo, lLerpFactor);
	//mMeshAngularOffset.Normalize();
	//mMeshAngularOffset.Slerp(mMeshAngularOffset, QuaternionF(), Math::GetIterateLerpTime(0.12f, lFrameTime));
}

void CppContextObject::ActivateLerp()
{
	mMeshSlideMode = MESH_SLIDE_START;
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
						if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_SLIDER)
						{
							// Ignore, no anchor to be had.
						}
						else if (mManager->GetGameManager()->GetPhysicsManager()->GetAnchorPos(lJoint, lAnchor))
						{
							const float lLength = 1;
							Vector3DF lAxis;
							if (lGeometry->GetJointType() == TBC::ChunkyBoneGeometry::JOINT_BALL)
							{
								// Ball joints don't have axes.
								mUiManager->GetRenderer()->DrawLine(lAnchor, Vector3DF(0,0,3), BLACK);
								break;
							}
							else if (mManager->GetGameManager()->GetPhysicsManager()->GetAxis1(lJoint, lAxis))
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

const TBC::ChunkyClass* CppContextObject::GetClass() const
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



void CppContextObject::OnLoadClass(UserClassResource* pClassResource)
{
	UiTbc::ChunkyClass* lClass = pClassResource->GetData();
	if (pClassResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		mLog.Errorf(_T("Could not load class '%s'."), pClassResource->GetName().c_str());
		assert(false);
		return;
	}
	else
	{
		StartLoadingPhysics(lClass->GetPhysicsBaseName());
	}

	if (!mEnableUi)
	{
		return;
	}

	assert(mMeshLoadCount == 0);
	//assert(mTextureLoadCount == 0);
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
			lMeshInstance = strutil::Format(_T("%s"), GetMeshInstanceId().c_str());
		}
		else
		{
			lMeshInstance = strutil::Format(_T("%s_%s"), lMeshNameList[1].c_str(), GetMeshInstanceId().c_str());
		}
		UserGeometryReferenceResource* lMesh = new UserGeometryReferenceResource(
			mUiManager, GeometryOffset(lPhysIndex, lTransform));
		mMeshResourceArray.push_back(lMesh);
		lMesh->Load(GetResourceManager(),
			strutil::Format(_T("%s.mesh;%s"), lMeshName.c_str(), lMeshInstance.c_str()),
			UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh));
	}
	LoadTextures();
}

void CppContextObject::LoadTextures()
{
	const UiTbc::ChunkyClass* lClass = ((UiTbc::ChunkyClass*)mUiClassResource->GetRamData());
	assert(lClass);
	for (size_t x = 0; x < lClass->GetMeshCount(); ++x)
	{
		const std::vector<str>& lTextureList = lClass->GetMaterial(x).mTextureList;
		for (std::vector<str>::const_iterator y = lTextureList.begin(); y != lTextureList.end(); ++y)
		{
			UserRendererImageResource* lTexture = new UserRendererImageResource(mUiManager);
			mTextureResourceArray.push_back(lTexture);
			lTexture->Load(GetResourceManager(), *y,
				UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
		}
	}
}

void CppContextObject::OnLoadMesh(UserGeometryReferenceResource* pMeshResource)
{
	DispatchOnLoadMesh(pMeshResource);
}

void CppContextObject::DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource)
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
		if (lMeshIndex == 0 && !mAllowRootShadow)
		{
			mUiManager->GetRenderer()->SetShadows(pMeshResource->GetData(), UiTbc::Renderer::FORCE_NO_SHADOWS);
		}
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
	++mTextureLoadCount;
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
	if (!mUiClassResource)
	{
		return;
	}
	for (size_t x = 0; x < mTextureResourceArray.size(); ++x)
	{
		UserRendererImageResource* lTexture = mTextureResourceArray[x];
		if (lTexture->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			return;
		}
	}
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
		if (lMesh->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			return;
		}
	}
	const UiTbc::ChunkyClass* lClass = ((UiTbc::ChunkyClass*)mUiClassResource->GetRamData());
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
		if (mUiManager->GetRenderer()->GetMaterialType(lMesh->GetData()) == UiTbc::Renderer::MAT_NULL)
		{
			if (lMesh->GetRamData()->GetUVData(0) && mTextureResourceArray.size() > 0)
			{
				UserRendererImageResource* lTexture = 0;
				TextureArray::iterator tx = mTextureResourceArray.begin();
				for (; !lTexture && tx != mTextureResourceArray.end(); ++tx)
				{
					const std::vector<str>& lTextureList = lClass->GetMaterial(x).mTextureList;
					std::vector<str>::const_iterator y = lTextureList.begin();
					for (; !lTexture && y != lTextureList.end(); ++y)
					{
						if ((*tx)->GetName() == *y)
						{
							lTexture = *tx;
						}
					}
				}
				if (!lTexture)
				{
					lTexture = mTextureResourceArray[0];
				}
				const bool lIsBlended = (lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha < 1 ||
					((UiTbc::ChunkyClass*)mUiClassResource->GetRamData())->GetMaterial(x).mShaderName == _T("blend"));
				UiTbc::Renderer::MaterialType lMaterialType = lIsBlended?
					UiTbc::Renderer::MAT_SINGLE_TEXTURE_BLENDED : UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID_PXS;
				mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
				mUiManager->GetRenderer()->TryAddGeometryTexture(lMesh->GetData(), lTexture->GetData());
			}
			else
			{
				UiTbc::Renderer::MaterialType lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS;
				if (lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha != 1)
				{
					lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED;
				}
				mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
			}
		}
	}
	TryComplete();
}

bool CppContextObject::TryComplete()
{
	if (!mUiClassResource)
	{
		return (false);
	}
	// Meshes/textures must be both 1) all attempted, and 2) all loaded OK. Plus, server (or otherwise
	// headless) should totally ignore this.
	if (mEnableUi &&
		(mTextureLoadCount != mTextureResourceArray.size() || mMeshLoadCount != mMeshResourceArray.size()))
	{
		return (false);
	}
	for (size_t x = 0;  x < mTextureResourceArray.size(); ++x)
	{
		UserRendererImageResource* lTexture = mTextureResourceArray[x];
		if (lTexture->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			return (false);
		}
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

	OnLoaded();
	return (true);
}

str CppContextObject::GetMeshInstanceId() const
{
	return (strutil::IntToString(GetInstanceId(), 10));
}



LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, CppContextObject);



}
