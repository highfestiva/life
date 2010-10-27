
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiResourceManager.h"
#include <assert.h>
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
	UiResource(pUiManager)
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
	assert(!IsUnique());
	assert(GetRamData() == 0);
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
	assert(mOptimizedData == UiTbc::Painter::INVALID_IMAGEID);
	mOptimizedData = GetUiManager()->GetPainter()->AddImage(GetRamData(), 0);
	assert(mOptimizedData != UiTbc::Painter::INVALID_IMAGEID);
	SetRamData(0);
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
	assert(mOptimizedData == UiTbc::Renderer::INVALID_TEXTURE);
	mOptimizedData = GetUiManager()->GetRenderer()->AddTexture(GetRamData());
	assert(mOptimizedData != UiTbc::Renderer::INVALID_TEXTURE);
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

RendererImageResource::RendererImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const str& pName):
	RendererImageBaseResource(pUiManager, pManager, pName)
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
	assert(!IsUnique());
	assert(GetRamData() == 0);
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
		SetRamData(new UiTbc::Texture(lImage));
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
	assert(!IsUnique());
	assert(GetRamData() == 0);
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
	assert(!IsUnique());

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
		lOk = lLoader.Load(lGeometry, mCastsShadows);
		if (!lOk)
		{
			assert(false);
			delete (lGeometry);
			lGeometry = 0;
		}
	}
	delete lFile;
	lFile = 0;
	assert(lGeometry);

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
	assert(mOptimizedData == R::INVALID_GEOMETRY);
	mOptimizedData = GetUiManager()->GetRenderer()->AddGeometry(GetRamData(), R::MAT_NULL,
		mCastsShadows? R::CAST_SHADOWS : R::NO_SHADOWS);
	assert(mOptimizedData != R::INVALID_GEOMETRY);
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

bool GeometryResource::GetCastsShadows() const
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
	return (true);
}

bool GeometryReferenceResource::Load()
{
	//assert(IsUnique());
	bool lOk = (mClassResource != 0);
	assert(lOk);
	if (lOk)
	{
		const str lFilename = strutil::Split(GetName(), _T(";"), 1)[0];
		assert(lFilename != GetName());
		mClassResource->Load(GetManager(), lFilename, ClassResource::TypeLoadCallback(this,
			&GeometryReferenceResource::OnLoadClass));
	}
	return (lOk);
}

Cure::ResourceLoadState GeometryReferenceResource::PostProcess()
{
	assert(mClassResource);
	return (CreateInstance());
}

Cure::ResourceLoadState GeometryReferenceResource::CreateInstance()
{
	assert(mClassResource);
	Cure::ResourceLoadState lLoadState = mClassResource->GetLoadState();
	if (lLoadState == Cure::RESOURCE_LOAD_COMPLETE)
	{
		SetRamData(new TBC::GeometryReference(mClassResource->GetRamData()));
		GetRamData()->SetAlwaysVisible(true);
		GetRamData()->SetBasicMaterialSettings(mClassResource->GetRamData()->GetBasicMaterialSettings());
		lLoadState = Parent::PostProcess();
	}
	return (lLoadState);
}

void GeometryReferenceResource::Resume()
{
	assert(!mClassResource);
	mClassResource = new ClassResource(GetUiManager());
}

void GeometryReferenceResource::Suspend()
{
	ReleaseGeometry();
}

void GeometryReferenceResource::OnLoadClass(ClassResource* pResource)
{
	mCastsShadows = ((GeometryResource*)pResource->GetConstResource())->GetCastsShadows();
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
//	assert(mGeometryReferenceId == UiTbc::Renderer::INVALID_GEOMETRY);
//	assert(mGeometryReference == 0);
//
//	TBC::GeometryBase* lOriginal = Parent::GetRamData();
//	mGeometryReference = new TBC::GeometryReference(lOriginal);
//	mGeometryReference->SetAlwaysVisible(true);
//	mGeometryReference->SetBasicMaterialSettings(lOriginal->GetBasicMaterialSettings());
//	mGeometryReferenceId = GetUiManager()->GetRenderer()->AddGeometry(
//		mGeometryReference, UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::NO_SHADOWS);
//	assert(mGeometryReferenceId != UiTbc::Renderer::INVALID_GEOMETRY);
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
	assert(!IsUnique());
	assert(GetRamData() == UiLepra::INVALID_SOUNDID);
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
		if (lOk)
		{
			SetRamDataType(lSoundId);
		}
	}
	if (lOwnData)
	{
		delete lData;
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



/*AnimationResource::AnimationResource(const str& pName):
	OptimizedResource(pName)
{
}

bool AnimationResource::Load()
{
}*/



}
