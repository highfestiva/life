
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiResourceManager.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Cure/Include/TerrainFunctionManager.h"
#include "../../Lepra/Include/MemFile.h"
#include "../../UiTBC/Include/UiTriangleBasedGeometry.h"
#include "../Include/UiCure.h"
#include "../Include/UiGameUiManager.h"



namespace UiCure
{



UiResource::UiResource(GameUiManager* pUiManager):
	mUiManager(pUiManager)
{
}

UiResource::~UiResource()
{
}

GameUiManager* UiResource::GetUiManager() const
{
	return (mUiManager);
}



// ----------------------------------------------------------------------------



PainterImageResource::PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName),
	UiResource(pUiManager),
	mReleaseMode(RELEASE_DELETE)
{
}

PainterImageResource::PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName, ImageReleaseMode pReleaseMode):
	Parent(pManager, pName),
	UiResource(pUiManager),
	mReleaseMode(pReleaseMode)
{
}

PainterImageResource::~PainterImageResource()
{
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (UiLepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	if (mOptimizedData != UiTbc::Painter::INVALID_IMAGEID)
	{
		GetUiManager()->GetPainter()->RemoveImage(mOptimizedData);
		mOptimizedData = UiTbc::Painter::INVALID_IMAGEID;
	}
}

const str PainterImageResource::GetType() const
{
	return (_T("PaintImg"));
}

PainterImageResource::UserData PainterImageResource::GetUserData(const Cure::UserResource*) const
{
	return (mOptimizedData);
}

bool PainterImageResource::Load()
{
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	SetRamData(new Canvas());
	File* lFile = GetManager()->QueryFile(GetName());
	bool lOk = (lFile != 0);
	if (lOk)
	{
		ImageLoader lLoader;
		lOk = lLoader.Load(ImageLoader::GetFileTypeFromName(GetName()), *lFile, *GetRamData());
	}
	delete lFile;
	return lOk;
}

Cure::ResourceLoadState PainterImageResource::PostProcess()
{
	deb_assert(mOptimizedData == UiTbc::Painter::INVALID_IMAGEID);
	mOptimizedData = GetUiManager()->GetPainter()->AddImage(GetRamData(), 0);
	deb_assert(mOptimizedData != UiTbc::Painter::INVALID_IMAGEID);
	switch (mReleaseMode)
	{
		case RELEASE_DELETE:		SetRamData(0);			break;
		case RELEASE_FREE_BUFFER:	GetRamData()->SetBuffer(0);	break;
		case RELEASE_NONE:						break;
	}
	Cure::ResourceLoadState lLoadState;
	if (mOptimizedData == UiTbc::Painter::INVALID_IMAGEID)
	{
		lLoadState = Cure::RESOURCE_LOAD_ERROR;
	}
	else
	{
		lLoadState = Cure::RESOURCE_LOAD_COMPLETE;
	}

	Parent::PostProcess();

	return (lLoadState);
}



// ----------------------------------------------------------------------------



RendererImageBaseResource::RendererImageBaseResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName),
	UiResource(pUiManager)
{
}

RendererImageBaseResource::~RendererImageBaseResource()
{
	// TRICKY: delete of data must lie in this class, and not in parent class RamResource.
	// This is so since types (UiLepra::SoundManager::SoundID) cannot be deleted!
	SetRamData(0);

	if (mOptimizedData != UiTbc::Renderer::INVALID_TEXTURE)
	{
		GetUiManager()->GetRenderer()->RemoveTexture(mOptimizedData);
		mOptimizedData = UiTbc::Renderer::INVALID_TEXTURE;
	}
}

RendererImageBaseResource::UserData RendererImageBaseResource::GetUserData(const Cure::UserResource*) const
{
	return (mOptimizedData);
}

Cure::ResourceLoadState RendererImageBaseResource::PostProcess()
{
	deb_assert(mOptimizedData == UiTbc::Renderer::INVALID_TEXTURE);
	mOptimizedData = GetUiManager()->GetRenderer()->AddTexture(GetRamData());
	deb_assert(mOptimizedData != UiTbc::Renderer::INVALID_TEXTURE);
	SetRamData(0);
	Cure::ResourceLoadState lLoadState;
	if (mOptimizedData == UiTbc::Renderer::INVALID_TEXTURE)
	{
		lLoadState = Cure::RESOURCE_LOAD_ERROR;
	}
	else
	{
		lLoadState = Cure::RESOURCE_LOAD_COMPLETE;
	}

	Parent::PostProcess();

	return (lLoadState);
}

RendererImageResource::RendererImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName, bool pMipMap):
	RendererImageBaseResource(pUiManager, pManager, pName),
	mMipMap(pMipMap)
{
}

RendererImageResource::~RendererImageResource()
{
	// Handled by parent.
}

const str RendererImageResource::GetType() const
{
	return (_T("RenderImg"));
}

bool RendererImageResource::Load()
{
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	Canvas lImage;
	File* lFile = GetManager()->QueryFile(GetName());
	bool lOk = (lFile != 0);
	if (lOk)
	{
		ImageLoader lLoader;
		lOk = lLoader.Load(ImageLoader::GetFileTypeFromName(GetName()), *lFile, lImage);
	}
	delete lFile;
	if (lOk)
	{
		SetRamData(new UiTbc::Texture(lImage, Canvas::RESIZE_FAST, mMipMap? 0 : 1));
	}
	return (lOk);
}



// ----------------------------------------------------------------------------



/*TextureResource::TextureResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	RendererImageBaseResource(pUiManager, pManager, pName)
{
}

TextureResource::~TextureResource()
{
	// Handled by parent.
}

const str TextureResource::GetType() const
{
	return (_T("Texture"));
}

bool TextureResource::Load()
{
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == 0);
	SetRamData(new UiTbc::Texture());
	UiTbc::TEXLoader lLoader;
	bool lOk = false;
	for (int x = 0; x < 3 && !lOk; ++x)	// Retry file loading, file might be held by anti-virus/Windoze/similar shit.
	{
		lOk = (lLoader.Load(GetName(), *GetRamData(), true) == UiTbc::TEXLoader::STATUS_SUCCESS);
	}
	return lOk;
}*/



// ----------------------------------------------------------------------------



GeometryResource::GeometryResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName),
	UiResource(pUiManager),
	mCastsShadows(0)
{
}

GeometryResource::~GeometryResource()
{
	ReleaseGeometry();
}

void GeometryResource::ReleaseGeometry()
{
	if (mOptimizedData)
	{
		GetUiManager()->GetRenderer()->RemoveGeometry(mOptimizedData);
		mOptimizedData = UiTbc::Renderer::INVALID_GEOMETRY;
	}
	SetRamData(0);
	SetLoadState(Cure::RESOURCE_UNLOADED);
}

const str GeometryResource::GetType() const
{
	return (_T("Geometry"));
}

GeometryResource::UserData GeometryResource::GetUserData(const Cure::UserResource*) const
{
	return (mOptimizedData);
}

bool GeometryResource::Load()
{
	deb_assert(!IsUnique());

	//float lCubeMappingScale = -1;
	TBC::GeometryBase::BasicMaterialSettings lMaterial;
	lMaterial.SetColor(0.5f, 0.5f, 0.5f);

	UiTbc::TriangleBasedGeometry* lGeometry = 0;

	//strutil::strvec lParts = strutil::Split(GetName(), _T(";"));
	//const str& lFilename = lParts[0];
	const str& lFilename = GetName();

	File* lFile = GetManager()->QueryFile(lFilename);
	bool lOk = (lFile != 0);
	if (lOk)
	{
		UiTbc::ChunkyMeshLoader lLoader(lFile, false);
		lGeometry = new UiTbc::TriangleBasedGeometry();
		LEPRA_DEBUG_CODE(lGeometry->mName = lFilename);
		lOk = lLoader.Load(lGeometry, mCastsShadows);
		if (!lOk)
		{
			deb_assert(false);
			delete (lGeometry);
			lGeometry = 0;
		}
	}
	delete lFile;
	lFile = 0;
	deb_assert(lGeometry);

	if (lGeometry)
	{
		lGeometry->SetBasicMaterialSettings(lMaterial);
		/*if (lCubeMappingScale > 0)
		{
			lGeometry->AddEmptyUVSet();
			Vector2DD lUVOffset(0.5, 0.5);
			UiTbc::UVMapper::ApplyCubeMapping(lGeometry, 0, lCubeMappingScale, lUVOffset);
			//lGeometry->DupUVSet(0);
		}*/

		SetRamData(lGeometry);
	}
	return (lGeometry != 0);
}

Cure::ResourceLoadState GeometryResource::PostProcess()
{
	typedef UiTbc::Renderer R;
	deb_assert(mOptimizedData == R::INVALID_GEOMETRY);
	if (!GetUiManager()->CanRender())
	{
		return Cure::RESOURCE_LOAD_IN_PROGRESS;
	}
	R::Shadows lShadows = R::NO_SHADOWS;
	switch (mCastsShadows)
	{
		case 1:		lShadows = R::CAST_SHADOWS;	break;
		case -1:	lShadows = R::FORCE_NO_SHADOWS;	break;
	}
	log_volatile(mLog.Debugf(_T("%s has casts shadows = %i."), GetName().c_str(), mCastsShadows));
	mOptimizedData = GetUiManager()->GetRenderer()->AddGeometry(GetRamData(), R::MAT_NULL, lShadows);
	deb_assert(mOptimizedData != R::INVALID_GEOMETRY);
	Cure::ResourceLoadState lLoadState;
	if (mOptimizedData == R::INVALID_GEOMETRY)
	{
		lLoadState = Cure::RESOURCE_LOAD_ERROR;
	}
	else
	{
		lLoadState = Cure::RESOURCE_LOAD_COMPLETE;
	}

	Parent::PostProcess();

	return (lLoadState);
}

int GeometryResource::GetCastsShadows() const
{
	return (mCastsShadows);
}

LOG_CLASS_DEFINE(UI_GFX_3D, GeometryResource);



// ----------------------------------------------------------------------------



GeometryReferenceResource::GeometryReferenceResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	GeometryResource(pUiManager, pManager, pName),
	mClassResource(new ClassResource(pUiManager))
{
}

GeometryReferenceResource::~GeometryReferenceResource()
{
	ReleaseGeometry();
}

void GeometryReferenceResource::ReleaseGeometry()
{
	delete (mClassResource);
	mClassResource = 0;
	Parent::ReleaseGeometry();
}

const str GeometryReferenceResource::GetType() const
{
	return (_T("GeometryRef"));
}

bool GeometryReferenceResource::IsReferenceType() const
{
	return true;
}

bool GeometryReferenceResource::Load()
{
	//deb_assert(IsUnique());
	bool lOk = (mClassResource != 0);
	deb_assert(lOk);
	if (lOk)
	{
		const str lFilename = strutil::Split(GetName(), _T(";"), 1)[0];
		deb_assert(lFilename != GetName());
		mClassResource->Load(GetManager(), lFilename, ClassResource::TypeLoadCallback(this,
			&GeometryReferenceResource::OnLoadClass));
	}
	return (lOk);
}

Cure::ResourceLoadState GeometryReferenceResource::PostProcess()
{
	deb_assert(mClassResource);
	return (CreateInstance());
}

Cure::ResourceLoadState GeometryReferenceResource::CreateInstance()
{
	deb_assert(mClassResource);
	Cure::ResourceLoadState lLoadState = mClassResource->GetLoadState();
	if (lLoadState == Cure::RESOURCE_LOAD_COMPLETE)
	{
		SetRamData(new TBC::GeometryReference(mClassResource->GetRamData()));
		GetRamData()->SetAlwaysVisible(true);
		//GetRamData()->SetBasicMaterialSettings(mClassResource->GetRamData()->GetBasicMaterialSettings());
		lLoadState = Parent::PostProcess();
	}
	return (lLoadState);
}

void GeometryReferenceResource::Resume()
{
	deb_assert(!mClassResource);
	mClassResource = new ClassResource(GetUiManager());
}

void GeometryReferenceResource::Suspend()
{
	ReleaseGeometry();
}

void GeometryReferenceResource::OnLoadClass(ClassResource* pResource)
{
	mCastsShadows = ((const GeometryResource*)pResource->GetConstResource())->GetCastsShadows();
}

LOG_CLASS_DEFINE(UI_GFX_3D, GeometryReferenceResource);



// ----------------------------------------------------------------------------



UserGeometryReferenceResource::UserGeometryReferenceResource(GameUiManager* pUiManager, const GeometryOffset& pOffset):
	UiResource(pUiManager),
	mOffset(pOffset)
	//mGeometryReference(0),
	//mGeometryReferenceId(UiTbc::Renderer::INVALID_GEOMETRY)
{
}

UserGeometryReferenceResource::~UserGeometryReferenceResource()
{
	//if (mGeometryReferenceId)
	//{
	//	GetUiManager()->GetRenderer()->RemoveGeometry(mGeometryReferenceId);
	//	mGeometryReferenceId = UiTbc::Renderer::INVALID_GEOMETRY;
	//}
	//delete (mGeometryReference);
	//mGeometryReference = 0;
}

//void UserGeometryReferenceResource::PostProcess()
//{
//	deb_assert(mGeometryReferenceId == UiTbc::Renderer::INVALID_GEOMETRY);
//	deb_assert(mGeometryReference == 0);
//
//	TBC::GeometryBase* lOriginal = Parent::GetRamData();
//	mGeometryReference = new TBC::GeometryReference(lOriginal);
//	mGeometryReference->SetAlwaysVisible(true);
//	mGeometryReference->SetBasicMaterialSettings(lOriginal->GetBasicMaterialSettings());
//	mGeometryReferenceId = GetUiManager()->GetRenderer()->AddGeometry(
//		mGeometryReference, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::NO_SHADOWS);
//	deb_assert(mGeometryReferenceId != UiTbc::Renderer::INVALID_GEOMETRY);
//}

const GeometryOffset& UserGeometryReferenceResource::GetOffset() const
{
	return (mOffset);
}

//TBC::GeometryBase* UserGeometryReferenceResource::GetRamData() const
//{
//	return (mGeometryReference);
//}
//
//UiTbc::Renderer::GeometryID UserGeometryReferenceResource::GetData() const
//{
//	return (mGeometryReferenceId);
//}

Cure::Resource* UserGeometryReferenceResource::CreateResource(Cure::ResourceManager* pManager, const str& pName) const
{
	return (new GeometryReferenceResource(GetUiManager(), pManager, pName));
}

LOG_CLASS_DEFINE(UI_GFX_3D, UserGeometryReferenceResource);



// ----------------------------------------------------------------------------



SoundResource::SoundResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
	SoundDimension pDimension, LoopMode pLoopMode):
	Parent(pManager, pName),
	UiResource(pUiManager),
	mDimension(pDimension),
	mLoopMode(pLoopMode)
{
	SetRamDataType(UiLepra::INVALID_SOUNDID);
}

SoundResource::~SoundResource()
{
	UserDataTable::iterator x = mUserDiversifiedTable.begin();
	for (; x != mUserDiversifiedTable.end(); ++x)
	{
		UserData lInstanceId = x->second;
		GetUiManager()->GetSoundManager()->DeleteSoundInstance(lInstanceId);
	}
	mUserDiversifiedTable.clear();
	if (GetRamData() != UiLepra::INVALID_SOUNDID)
	{
		GetUiManager()->GetSoundManager()->Release(GetRamData());
		SetRamDataType(UiLepra::INVALID_SOUNDID);
	}
}

bool SoundResource::Load()
{
	deb_assert(!IsUnique());
	deb_assert(GetRamData() == UiLepra::INVALID_SOUNDID);
	File* lFile = GetManager()->QueryFile(GetName());
	bool lOk = (lFile != 0);
	bool lOwnData = false;
	void* lData = 0;
	size_t lDataSize = 0;
	if (lOk)
	{
		lDataSize = (size_t)lFile->GetSize();
		MemFile* lMemFile = dynamic_cast<MemFile*>(lFile);
		if (lMemFile)
		{
			lData = lMemFile->GetBuffer();
		}
		else
		{
			lOk = (lFile->AllocReadData(&lData, lDataSize) == IO_OK);
			lOwnData = true;
		}
	}
	if (lOk)
	{
		UiLepra::SoundManager::SoundID lSoundId;
		if (mDimension == DIMENSION_2D)
		{
			lSoundId = GetUiManager()->GetSoundManager()->LoadSound2D(GetName(), lData, lDataSize, mLoopMode, 0);
		}
		else
		{
			lSoundId = GetUiManager()->GetSoundManager()->LoadSound3D(GetName(), lData, lDataSize, mLoopMode, 0);
		}
		lOk = (lSoundId != UiLepra::INVALID_SOUNDID);
		deb_assert(lOk);
		if (lOk)
		{
			SetRamDataType(lSoundId);
		}
	}
	if (lOwnData)
	{
		delete (char*)lData;
	}
	delete lFile;
	return lOk;
}

SoundResource::UserData SoundResource::CreateDiversifiedData() const
{
	UserData lInstanceId = GetUiManager()->GetSoundManager()->CreateSoundInstance(GetRamData());
	return (lInstanceId);
}

void SoundResource::ReleaseDiversifiedData(UserData pData) const
{
	GetUiManager()->GetSoundManager()->DeleteSoundInstance(pData);
}

SoundResource2d::SoundResource2d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
	LoopMode pLoopMode):
	SoundResource(pUiManager, pManager, pName, DIMENSION_2D, pLoopMode)
{
}

const str SoundResource2d::GetType() const
{
	return (_T("Sound2D"));
}

SoundResource3d::SoundResource3d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName,
	LoopMode pLoopMode):
	SoundResource(pUiManager, pManager, pName, DIMENSION_3D, pLoopMode)
{
}

const str SoundResource3d::GetType() const
{
	return (_T("Sound3D"));
}



// ----------------------------------------------------------------------------



ClassResource::ClassResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	Parent(pManager, pName),
	UiResource(pUiManager)
{
}

ClassResource::~ClassResource()
{
}

bool ClassResource::Load()
{
	return LoadWithName(Resource::GetName().substr(3));	// TRICKY: cut out "UI:" prefix (to distinguish from those used by non-UiCure classes).
}



/*AnimationResource::AnimationResource(const str& pName):
	OptimizedResource(pName)
{
}

bool AnimationResource::Load()
{
}*/



}
