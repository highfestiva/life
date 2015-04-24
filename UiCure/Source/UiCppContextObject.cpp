
// Author: Jonas Byström
// Copyright (c) 2002-, Pixel Doctrine



#include "pch.h"
#include "../../Lepra/Include/LepraAssert.h"
#include <math.h>
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Random.h"
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
	mUseDefaultTexture(false),
	mEnablePixelShader(true),
	mEnableMeshMove(true),
	mEnableMeshSlide(false),
	mMeshLoadCount(0),
	mTextureLoadCount(0),
	mLerpMode(LERP_STOP),
	mSinkSpeed(0),
	mSinkOffset(0)
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

void CppContextObject::EnableRootShadow(bool pEnable)
{
	mAllowRootShadow = pEnable;
}

void CppContextObject::SetUseDefaultTexture(bool pUseDefaultTexture)
{
	mUseDefaultTexture = pUseDefaultTexture;
}

void CppContextObject::EnablePixelShader(bool pEnable)
{
	mEnablePixelShader = pEnable;
}

void CppContextObject::EnableMeshMove(bool pEnable)
{
	mEnableMeshMove = pEnable;
}

void CppContextObject::EnableMeshSlide(bool pEnable)
{
	mEnableMeshSlide = pEnable;
}

void CppContextObject::SetPositionFinalized()
{
	Parent::SetPositionFinalized();
	mLerpMode = LERP_STOP;
}



void CppContextObject::StartLoading()
{
	deb_assert(mUiClassResource == 0);
	mUiClassResource = new UserClassResource(mUiManager);
	const str lClassName = _T("UI:")+GetClassId()+_T(".class");
	mUiClassResource->Load(GetResourceManager(), lClassName,
		UserClassResource::TypeLoadCallback(this, &CppContextObject::OnLoadClass));
}



void CppContextObject::OnTick()
{
	if (!mEnableUi || !mEnableMeshMove)
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
	Tbc::PhysicsManager* lPhysicsManager = mManager->GetGameManager()->GetPhysicsManager();
	const float lFrameTime = GetManager()->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime();
	const float lLerpFactor = Math::GetIterateLerpTime(0.2f, lFrameTime);
	vec3 lRootPosition;
	xform lPhysicsTransform;
	if (mSinkSpeed)
	{
		const int lGeometryCount = mPhysics->GetBoneCount();
		for (int x = 0; x < lGeometryCount; ++x)
		{
			const Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(x);
			if (lGeometry && lGeometry->GetBodyId())
			{
				vec3 lPosition = lPhysicsManager->GetBodyPosition(lGeometry->GetBodyId());
				lPosition.z -= mSinkSpeed * GetManager()->GetGameManager()->GetTimeManager()->GetNormalGameFrameTime();
				lPhysicsManager->SetBodyPosition(lGeometry->GetBodyId(), lPosition);
			}
		}
	}
	/*if (mLerpMode == LERP_START)
	{
		mLerpOffset.SetIdentity();
		//log_volatile(mLog.Debugf(_T("Starting slide of mesh on object %u/%s."), GetInstanceId(), GetClassId().c_str()));
	}*/
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		Tbc::GeometryBase* lGfxGeometry = lResource->GetRamData();

		if (mPhysicsOverride != Cure::PHYSICS_OVERRIDE_BONES)
		{
			Tbc::ChunkyBoneGeometry* lGeometry = mPhysics->GetBoneGeometry(lResource->GetOffset().mGeometryIndex);
			Tbc::PhysicsManager::BodyID lBodyId = lGeometry->GetBodyId();
			if (!lGeometry || lBodyId == Tbc::INVALID_BODY)
			{
				if (lGeometry)
				{
					lBodyId = lGeometry->GetTriggerId();
				}
				if (!lBodyId)
				{
					mLog.Warningf(_T("Physical body (index %u) for %s not loaded!"), lResource->GetOffset().mGeometryIndex, lResource->GetName().c_str());
					continue;
				}
			}
			lPhysicsManager->GetBodyTransform(lBodyId, lPhysicsTransform);
			/*if (mAutoDisableMeshMove && mPhysics->GetPhysicsType() == Tbc::ChunkyPhysics::STATIC && mPhysics->GetBodyType(lGeometry) == Tbc::PhysicsManager::STATIC)
			{
				mEnableMeshMove = false;
			}*/

			if (mLerpMode == LERP_START)
			{
				if (x == 0)
				{
					// Start out by fetching offset.
					mLerpOffset = lGfxGeometry->GetBaseTransformation();
					mLerpOffset.GetOrientation() *= lPhysicsTransform.GetOrientation().GetInverse();
					mLerpOffset.GetPosition() -= lPhysicsTransform.GetPosition();
					mLerpOffset.Interpolate(mLerpOffset, gIdentityTransformationF, lLerpFactor);
				}
			}
			if (mLerpMode == LERP_START || mLerpMode == LERP_RUN)
			{
				// Phys + offset = current lerped mesh position. Also account for this physical offset to object root.
				if (x == 0)
				{
					lRootPosition = lPhysicsTransform.GetPosition();
				}
				vec3 lMeshOffsetWithRootOrientation = lPhysicsTransform.GetPosition() - lRootPosition;
				lMeshOffsetWithRootOrientation = mLerpOffset.GetOrientation() * lMeshOffsetWithRootOrientation - lMeshOffsetWithRootOrientation;
				lPhysicsTransform.GetOrientation() = mLerpOffset.GetOrientation() * lPhysicsTransform.GetOrientation();
				lPhysicsTransform.GetPosition() += mLerpOffset.GetPosition() + lMeshOffsetWithRootOrientation;
			}
		}
		else
		{
			lPhysicsTransform = mPhysics->GetBoneTransformation(lResource->GetOffset().mGeometryIndex);
		}

		lGfxGeometry->SetTransformation(lPhysicsTransform);
	}
	if (mLerpMode == LERP_START)
	{
		mLerpMode = LERP_RUN;
		if (mLerpOffset.GetPosition().GetLengthSquared() >= 20*20)
		{
			mLerpOffset.SetIdentity();
		}
	}
	else if (mLerpMode == LERP_RUN)
	{
		if (mLerpOffset.GetPosition().GetLengthSquared() < 0.1f && mLerpOffset.GetOrientation().a > 0.999f)
		{
			mLerpMode = LERP_STOP;
			mLerpOffset.SetIdentity();
		}
		else
		{
			mLerpOffset.Interpolate(mLerpOffset, gIdentityTransformationF, lLerpFactor);
		}
	}
}

void CppContextObject::ActivateLerp()
{
	if (mEnableMeshSlide)
	{
		mLerpMode = LERP_START;
	}
}

void CppContextObject::SetSinking(float pSinkSpeed)
{
	mSinkSpeed = pSinkSpeed;
}

void CppContextObject::ShrinkMeshBigOrientationThreshold(float pThreshold)
{
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		Tbc::GeometryBase* lGeometry = lResource->GetRamData();
		if (lGeometry->GetBigOrientationThreshold() > pThreshold)
		{
			lGeometry->SetBigOrientationThreshold(pThreshold);
		}
	}
}



Tbc::GeometryBase* CppContextObject::GetMesh(int pIndex) const
{
	const UserGeometryReferenceResource* lResource = GetMeshResource(pIndex);
	if (lResource && lResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (lResource->GetRamData());
	}
	return (0);
}

UserGeometryReferenceResource* CppContextObject::GetMeshResource(int pIndex) const
{
	if (pIndex < (int)mMeshResourceArray.size())
	{
		return mMeshResourceArray[pIndex];
	}
	else
	{
		deb_assert(false);
	}
	return (0);
}

void CppContextObject::AddMeshResource(Tbc::GeometryBase* pMesh, int pCastsShadows)
{
	static int lMeshCounter = 0;
	str lMeshName = strutil::Format(_T("RawMesh%i"), lMeshCounter++);
	DoAddMeshResource(lMeshName, pMesh, pCastsShadows);
}

void CppContextObject::AddMeshResourceRef(const str& pMeshName, int pCastsShadows)
{
	DoAddMeshResource(pMeshName, 0, pCastsShadows);
}

void CppContextObject::CenterMeshes()
{
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lResource = mMeshResourceArray[x];
		if (lResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			continue;
		}
		Tbc::GeometryReference* lGfxGeometry = (Tbc::GeometryReference*)lResource->GetRamData();
		xform lOffset = lGfxGeometry->GetOffsetTransformation();
		lOffset.SetPosition(vec3(0, 0, 0));
		lGfxGeometry->SetOffsetTransformation(lOffset);
	}
}

void CppContextObject::LoadTexture(const str& pName, Canvas::ResizeHint pResizeHint)
{
	UserRendererImageResource* lTexture = new UserRendererImageResource(mUiManager, ImageProcessSettings(pResizeHint, mUiManager->GetRenderer()->GetMipMappingEnabled()));
	mTextureResourceArray.push_back(lTexture);
	lTexture->Load(GetResourceManager(), pName, UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
}

void CppContextObject::UpdateMaterial(int pMeshIndex)
{
	if (!mUiManager->CanRender())
	{
		return;
	}
	const UiTbc::ChunkyClass* lClass = (const UiTbc::ChunkyClass*)GetClass();
	if (!lClass || lClass->GetMeshCount() <= pMeshIndex)
	{
		UiTbc::Renderer::MaterialType lMaterialType = mEnablePixelShader? UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS : UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID;
		mUiManager->GetRenderer()->ChangeMaterial(mMeshResourceArray[pMeshIndex]->GetData(), lMaterialType);
		return;
	}
	UserGeometryReferenceResource* lMesh = mMeshResourceArray[pMeshIndex];
	const bool lTransparent = (lMesh->GetRamData()->GetBasicMaterialSettings().mAlpha < 1);
	UserRendererImageResource* lTexture = 0;
	if (lMesh->GetRamData()->GetUVData(0) && mTextureResourceArray.size() > 0)
	{
		TextureArray::iterator tx = mTextureResourceArray.begin();
		for (; !lTexture && tx != mTextureResourceArray.end(); ++tx)
		{
			const std::vector<str>& lTextureList = lClass->GetMaterial(pMeshIndex).mTextureList;
			std::vector<str>::const_iterator y = lTextureList.begin();
			for (; !lTexture && y != lTextureList.end(); ++y)
			{
				if ((*tx)->GetName() == *y)
				{
					lTexture = *tx;
				}
			}
		}
		if (!lTexture && mUseDefaultTexture)
		{
			lTexture = mTextureResourceArray[0];
		}
	}
	if (mPostLoadMaterialDelegate)
	{
		mPostLoadMaterialDelegate(lMesh);
	}
	if (lMesh->GetRamData()->GetUVData(0) && lTexture)
	{
		const str lShader = lClass->GetMaterial(pMeshIndex).mShaderName;
		const bool lIsBlended = (lTransparent || lShader == _T("blend"));
		const bool lIsHighlight = (lShader == _T("highlight"));
		const bool lIsEnv = (lShader == _T("env"));
		const bool lIsEnvBlend = ((lTransparent && lIsEnv) || lShader == _T("env_blend"));
		UiTbc::Renderer::MaterialType lMaterialType = mEnablePixelShader? UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID_PXS : UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID;
		if (lIsEnvBlend)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_TEXTURE_ENVMAP_BLENDED;
		}
		else if (lIsEnv)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_TEXTURE_ENVMAP_SOLID;
		}
		else if (lIsHighlight)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_TEXTURE_HIGHLIGHT;
		}
		else if (lIsBlended)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_TEXTURE_BLENDED;
		}
		mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
		mUiManager->GetRenderer()->TryAddGeometryTexture(lMesh->GetData(), lTexture->GetData());
	}
	else
	{
		UiTbc::Renderer::MaterialType lMaterialType = mEnablePixelShader? UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS : UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID;
		const str lShader = lClass->GetMaterial(pMeshIndex).mShaderName;
		const bool lIsBlended = (lTransparent || lShader == _T("blend"));
		const bool lIsEnv = (lShader == _T("env"));
		const bool lIsEnvBlend = ((lTransparent && lIsEnv) || lShader == _T("env_blend"));
		if (lIsEnvBlend)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_ENVMAP_BLENDED;
		}
		else if (lIsEnv)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_ENVMAP_SOLID;
		}
		else if (lIsBlended)
		{
			lMaterialType = UiTbc::Renderer::MAT_SINGLE_COLOR_BLENDED;
		}
		mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), lMaterialType);
	}
}

void CppContextObject::SetPostLoadMaterialDelegate(const PostLoadMaterialDelegate& pDelegate)
{
	mPostLoadMaterialDelegate = pDelegate;
}



void CppContextObject::ReplaceTexture(int pTextureIndex, const str& pNewTextureName)
{
	UserRendererImageResource* lTexture = mTextureResourceArray[pTextureIndex];
	if (lTexture->GetName() == pNewTextureName)
	{
		return;
	}
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
		if (lMesh->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			if (mUiManager->GetRenderer()->DisconnectGeometryTexture(lMesh->GetData(), lTexture->GetData()))
			{
				mUiManager->GetRenderer()->ChangeMaterial(lMesh->GetData(), UiTbc::Renderer::MAT_NULL);
			}
		}
	}
	mUseDefaultTexture = true;
	--mTextureLoadCount;
	UserRendererImageResource* lNewTexture = new UserRendererImageResource(mUiManager, ((const RendererImageResource*)lTexture->GetConstResource())->mSettings);
	mTextureResourceArray[pTextureIndex] = lNewTexture;
	lNewTexture->Load(GetResourceManager(), pNewTextureName,
		UserRendererImageResource::TypeLoadCallback(this, &CppContextObject::OnLoadTexture));
	delete lTexture;
}



GameUiManager* CppContextObject::GetUiManager() const
{
	return (mUiManager);
}

const Tbc::ChunkyClass* CppContextObject::GetClass() const
{
	if (mUiClassResource && mUiClassResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		return (mUiClassResource->GetRamData());
	}
	return (0);
}



void CppContextObject::DoAddMeshResource(const str& pMeshName, Tbc::GeometryBase* pMesh, int pCastsShadows)
{
	const str lMeshName = pMeshName + _T(".mesh");
	const str lMeshRefName = strutil::Format(_T("%s;%i"), lMeshName.c_str(), GetInstanceId());
	int lPhysIndex = 0;
	xform lTransform;
	float lScale = 1;
	UserGeometryReferenceResource* lGeometryRef = new UserGeometryReferenceResource(mUiManager, GeometryOffset(lPhysIndex, lTransform, lScale));
	mMeshResourceArray.push_back(lGeometryRef);
	if (pMesh)
	{
		GeometryReferenceResource* lGeometryRefResource = (GeometryReferenceResource*)lGeometryRef->CreateResource(GetResourceManager(), lMeshRefName);
		lGeometryRefResource->SetIsUnique(true);
		lGeometryRef->SetResource(lGeometryRefResource);
		Cure::UserResource::LoadCallback lCallbackCast;
		lCallbackCast.SetMemento(UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh).GetMemento());
		lGeometryRefResource->AddCaller(lGeometryRef, lCallbackCast);
		lGeometryRefResource->SetLoadState(Cure::RESOURCE_LOAD_IN_PROGRESS);	// We're waiting for the root resource to get loaded.
		lGeometryRefResource->SetCastsShadows(pCastsShadows);
		GeometryReferenceResource::ClassResource* lGeometry = lGeometryRefResource->GetParent();
		GeometryResource* lGeometryResource = (GeometryResource*)lGeometry->CreateResource(GetResourceManager(), lMeshName);
		lGeometryResource->SetIsUnique(true);
		lGeometry->SetResource(lGeometryResource);
		lGeometryResource->SetRamDataType(pMesh);
		lGeometryResource->SetLoadState(Cure::RESOURCE_LOAD_IN_PROGRESS);	// Handle pushing to gfx card in postprocessing by some other thread at a later stage.
		GetResourceManager()->AddLoaded(lGeometry);
		GetResourceManager()->AddLoaded(lGeometryRef);
	}
	else
	{
		lGeometryRef->Load(GetResourceManager(),
			lMeshRefName,
			UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh),
			false);
		((GeometryReferenceResource*)lGeometryRef->GetResource())->SetCastsShadows(pCastsShadows);
	}
}



void CppContextObject::OnLoadClass(UserClassResource* pClassResource)
{
	UiTbc::ChunkyClass* lClass = pClassResource->GetData();
	if (pClassResource->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
	{
		mLog.Errorf(_T("Could not load class '%s'."), pClassResource->GetName().c_str());
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}

	const size_t lMeshCount = lClass->GetMeshCount();
	if (mEnableUi)
	{
		deb_assert(lMeshCount > 0);
		for (size_t x = 0; x < lMeshCount; ++x)
		{
			int lPhysIndex = -1;
			str lMeshName;
			xform lTransform;
			float lScale;
			lClass->GetMesh(x, lPhysIndex, lMeshName, lTransform, lScale);
			UserGeometryReferenceResource* lMesh = new UserGeometryReferenceResource(
				mUiManager, GeometryOffset(lPhysIndex, lTransform, lScale));
			mMeshResourceArray.push_back(lMesh);
		}
	}

	StartLoadingPhysics(lClass->GetPhysicsBaseName());

	if (!mEnableUi)
	{
		return;
	}

	deb_assert(mMeshLoadCount == 0);
	//deb_assert(mTextureLoadCount == 0);
	deb_assert(lMeshCount == mMeshResourceArray.size());
	size_t x = 0;
	MeshArray::iterator y = mMeshResourceArray.begin();
	for (; y != mMeshResourceArray.end(); ++x, ++y)
	{
		int lPhysIndex = -1;
		str lMeshName;
		xform lTransform;
		float lScale;
		lClass->GetMesh(x, lPhysIndex, lMeshName, lTransform, lScale);
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
		// TRICKY: load non-unique, since this mesh reference is shared. However, we set it as
		// "don't keep", which makes sure it doesn't get cached. Example: client 0's car 1 mesh
		// is the same as client 1's car 1 mesh. But when car 1 dies, the mesh REFERENCE should
		// also die immediately. (The MESH, on the other hand, is a totally different topic.)
		(*y)->Load(GetResourceManager(),
			strutil::Format(_T("%s.mesh;%s"), lMeshName.c_str(), lMeshInstance.c_str()),
			UserGeometryReferenceResource::TypeLoadCallback(this, &CppContextObject::OnLoadMesh),
			false);
	}
	LoadTextures();
}

void CppContextObject::LoadTextures()
{
	const UiTbc::ChunkyClass* lClass = ((UiTbc::ChunkyClass*)mUiClassResource->GetRamData());
	deb_assert(lClass);
	for (size_t x = 0; x < lClass->GetMeshCount(); ++x)
	{
		const Canvas::ResizeHint lResizeHint = lClass->GetMaterial(x).mResizeHint;
		const std::vector<str>& lTextureList = lClass->GetMaterial(x).mTextureList;
		for (std::vector<str>::const_iterator y = lTextureList.begin(); y != lTextureList.end(); ++y)
		{
			LoadTexture(*y, lResizeHint);
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
		deb_assert((int)lMeshIndex >= 0);
		if (lMeshIndex == 0 && !mAllowRootShadow)
		{
			mUiManager->GetRenderer()->SetShadows(pMeshResource->GetData(), UiTbc::Renderer::FORCE_NO_SHADOWS);
		}
		if (mUiClassResource)
		{
			const UiTbc::ChunkyClass::Material& lLoadedMaterial =
				((UiTbc::ChunkyClass*)mUiClassResource->GetRamData())->GetMaterial(lMeshIndex);
			Tbc::GeometryBase::BasicMaterialSettings lMaterial(lLoadedMaterial.mAmbient,
				lLoadedMaterial.mDiffuse, lLoadedMaterial.mSpecular,
				lLoadedMaterial.mShininess, lLoadedMaterial.mAlpha, lLoadedMaterial.mSmooth);
			pMeshResource->GetRamData()->SetBasicMaterialSettings(lMaterial);
		}

		((Tbc::GeometryReference*)pMeshResource->GetRamData())->SetOffsetTransformation(pMeshResource->GetOffset().mOffset);
		pMeshResource->GetRamData()->SetScale(pMeshResource->GetOffset().mScale);
		TryAddTexture();
	}
	else
	{
		mLog.AError("Could not load mesh! Sheit.");
		deb_assert(false);
		GetManager()->PostKillObject(GetInstanceId());
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
		mLog.Errorf(_T("Could not load texture %s. Gah!"), pTextureResource->GetName().c_str());
		deb_assert(false);
	}
}

void CppContextObject::TryAddTexture()
{
	if (!mUiManager->CanRender())
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
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		UserGeometryReferenceResource* lMesh = mMeshResourceArray[x];
		if (mUiManager->GetRenderer()->GetMaterialType(lMesh->GetData()) == UiTbc::Renderer::MAT_NULL)
		{
			UpdateMaterial(x);
		}
	}
	TryComplete();
}

bool CppContextObject::TryComplete()
{
	// Meshes/textures must be both 1) all attempted, and 2) all loaded OK. Plus, server (or otherwise
	// headless) should totally ignore this.
	if (mEnableUi &&
		(mTextureLoadCount != mTextureResourceArray.size() || mMeshLoadCount != mMeshResourceArray.size()))
	{
		return (false);
	}
	for (size_t x = 0; x < mTextureResourceArray.size(); ++x)
	{
		UserRendererImageResource* lTexture = mTextureResourceArray[x];
		if (lTexture->GetLoadState() != Cure::RESOURCE_LOAD_COMPLETE)
		{
			return (false);
		}
	}
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
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

	return (true);
}

str CppContextObject::GetMeshInstanceId() const
{
	return (strutil::IntToString(GetInstanceId(), 10));
}



loginstance(GAME_CONTEXT_CPP, CppContextObject);



}
