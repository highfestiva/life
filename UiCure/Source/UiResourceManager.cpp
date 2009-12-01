
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include <assert.h>
#include "../../Cure/Include/TerrainFunctionManager.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../../UiTBC/Include/UiBasicMeshCreator.h"
#include "../../UiTBC/Include/UiChunkyLoader.h"
#include "../../UiTBC/Include/UiTEXLoader.h"
#include "../../UiTBC/Include/UiTriangleBasedGeometry.h"
#include "../../UiTBC/Include/UiUvMapper.h"
#include "../Include/UiCure.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiResourceManager.h"
#include "../Include/UiSoundManager.h"



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



PainterImageResource::PainterImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	OptimizedResource<Lepra::Canvas*, UiTbc::Painter::ImageID>(pManager, pName),
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

const Lepra::String PainterImageResource::GetType() const
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
	SetRamData(new Lepra::Canvas());
	Lepra::ImageLoader lLoader;
	return (lLoader.Load(GetName(), *GetRamData()));
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



RendererImageBaseResource::RendererImageBaseResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	OptimizedResource<UiTbc::Texture*, UiTbc::Renderer::TextureID>(pManager, pName),
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

RendererImageResource::RendererImageResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	RendererImageBaseResource(pUiManager, pManager, pName)
{
}

RendererImageResource::~RendererImageResource()
{
	// Handled by parent.
}

const Lepra::String RendererImageResource::GetType() const
{
	return (_T("RenderImg"));
}

bool RendererImageResource::Load()
{
	assert(!IsUnique());
	assert(GetRamData() == 0);
	Lepra::Canvas lImage;
	Lepra::ImageLoader lLoader;
	bool lOk = lLoader.Load(GetName(), lImage);
	if (lOk)
	{
		SetRamData(new UiTbc::Texture(lImage));
	}
	return (lOk);
}



// ----------------------------------------------------------------------------



TextureResource::TextureResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	RendererImageBaseResource(pUiManager, pManager, pName)
{
}

TextureResource::~TextureResource()
{
	// Handled by parent.
}

const Lepra::String TextureResource::GetType() const
{
	return (_T("Texture"));
}

bool TextureResource::Load()
{
	assert(!IsUnique());
	assert(GetRamData() == 0);
	SetRamData(new UiTbc::Texture());
	UiTbc::TEXLoader lLoader;
	return (lLoader.Load(GetName(), *GetRamData(), true) == UiTbc::TEXLoader::STATUS_SUCCESS);
}



// ----------------------------------------------------------------------------



GeometryResource::GeometryResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	OptimizedResource(pManager, pName),
	UiResource(pUiManager)
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

const Lepra::String GeometryResource::GetType() const
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

	float lCubeMappingScale = -1;
	TBC::GeometryBase::BasicMaterialSettings lMaterial;
	lMaterial.SetColor(0.5f, 0.5f, 0.5f);

	UiTbc::TriangleBasedGeometry* lGeometry = 0;

	//Lepra::StringUtility::StringVector lParts = Lepra::StringUtility::Split(GetName(), _T(";"));
	//const Lepra::String& lFilename = lParts[0];
	const Lepra::String& lFilename = GetName();

	Lepra::DiskFile lFile;
	if (lFile.Open(lFilename, Lepra::DiskFile::MODE_READ))
	{
		UiTbc::ChunkyMeshLoader lLoader(&lFile, false);
		lGeometry = new UiTbc::TriangleBasedGeometry();
		bool lOk = lLoader.Load(lGeometry);
		if (!lOk)
		{
			assert(false);
			delete (lGeometry);
			lGeometry = 0;
		}
	}
	else if (lFilename.find(_T("box_002_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lDimensions(2.0f, 1.0f, 3.5f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lDimensions.x, lDimensions.y, lDimensions.z);
		lMaterial.SetColor(0, 1, 0);
		lCubeMappingScale = lGeometry->GetBoundingRadius();
	}
	else if (lFilename.find(_T("sphere_002_mesh")) != Lepra::String::npos)
	{
		float lRadius = 1.0f;
		lGeometry = UiTbc::BasicMeshCreator::CreateEllipsoid(lRadius, lRadius, lRadius, 10, 10);
		lMaterial.SetColor(1, 0, 1);
	}
	else if (lFilename.find(_T("car_001_body_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(1.9f, 4.9f, 0.6f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.9f, 0.8f, 0.3f);
		lCubeMappingScale = lGeometry->GetBoundingRadius();
	}
	else if (lFilename.find(_T("car_001_top_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(1.6f, 2.9f, 0.6f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.8f, 0.6f, 0.2f);
	}
	else if (lFilename.find(_T("monster_001_body_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(3.0f, 5.0f, 0.7f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.0f, 0.0f, 0.5f);
	}
	else if (lFilename.find(_T("monster_001_top_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(3.0f, 3.0f, 0.7f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(1, 1, 1);
		lCubeMappingScale = lGeometry->GetBoundingRadius();
	}
	else if (lFilename.find(_T("excavator_703_body_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(2.9f, 3.0f, 1.5f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.7f, 0.2f, 0.2f);
	}
	else if (lFilename.find(_T("excavator_703_top_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(1.0f, 1.5f, 0.6f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.2f, 0.8f, 0.9f);
	}
	else if (lFilename.find(_T("car_antenna_mesh")) != Lepra::String::npos)
	{
		lGeometry = UiTbc::BasicMeshCreator::CreateCylinder(0.1f, 0.05f, 2, 8);
	}
	else if (lFilename.find(_T("car_001_wheel_mesh")) != Lepra::String::npos)
	{
		const float lWheelRadius = 0.3f;
		const float lWheelWidth = lWheelRadius*1;
		lGeometry = UiTbc::BasicMeshCreator::CreateCylinder(lWheelRadius, lWheelRadius, lWheelWidth, 10);
		lMaterial.SetColor(0.1f, 0.1f, 0.1f);
	}
	else if (lFilename.find(_T("monster_001_wheel_mesh")) != Lepra::String::npos)
	{
		const float lWheelRadius = 1.0f;
		const float lWheelWidth = lWheelRadius*0.8f;
		lGeometry = UiTbc::BasicMeshCreator::CreateCylinder(lWheelRadius, lWheelRadius, lWheelWidth, 10);
		lMaterial.SetColor(0.1f, 0.1f, 0.1f);
	}
	else if (lFilename.find(_T("excavator_703_boom1_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(0.6f, 0.6f, 2.5f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.7f, 0.2f, 0.2f);
	}
	else if (lFilename.find(_T("excavator_703_boom2_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(0.6f, 0.6f, 3.2f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.7f, 0.2f, 0.2f);
	}
	else if (lFilename.find(_T("excavator_703_arm_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(0.4f, 3.0f, 0.4f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.7f, 0.2f, 0.2f);
	}
	else if (lFilename.find(_T("excavator_703_bucket_back_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(1.5f, 0.8f, 0.1f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.3f, 0.2f, 0.1f);
	}
	else if (lFilename.find(_T("excavator_703_bucket_floor_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(1.5f, 1.0f, 0.1f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
		lMaterial.SetColor(0.3f, 0.2f, 0.1f);
	}
	else if (lFilename.find(_T("crane_whatever_tower_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(2.5f, 2.5f, 25.0f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
	}
	else if (lFilename.find(_T("crane_whatever_jib_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(2.5f, 20.0f, 2.5f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
	}
	else if (lFilename.find(_T("crane_whatever_wire_mesh")) != Lepra::String::npos)
	{
		lGeometry = UiTbc::BasicMeshCreator::CreateCylinder(0.05f, 0.05f, 4.75f, 10);
	}
	else if (lFilename.find(_T("crane_whatever_hook_mesh")) != Lepra::String::npos)
	{
		Lepra::Vector3DF lBodyDimensions(0.25f, 0.25f, 0.5f);
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lBodyDimensions.x, lBodyDimensions.y, lBodyDimensions.z);
	}
	else if (lFilename.find(_T("ground_002_")) != Lepra::String::npos)
	{
		static const float lRoadWidth = 5;
		static const float lRoadHeight = 7;
		static const float lUphillLength = 70;
		static const float lUphillOrthogonalLength = lUphillLength*::sin(Lepra::PIF/4);
		static const float lPlateauLength = 15;
		static const float lPlateauLengthCompensation = lRoadHeight/1.5f;
		static const float lFloorSize = 500;
		Lepra::Vector3DF lDimensions;
		if (lFilename.find(_T("ground_002_0_mesh")) != Lepra::String::npos)
		{
			lDimensions.Set(lFloorSize, lFloorSize, lFloorSize);
		}
		else if (lFilename.find(_T("ground_002_1_mesh")) != Lepra::String::npos)
		{
			lDimensions.Set(lRoadWidth, lUphillLength, lRoadHeight);
		}
		else if (lFilename.find(_T("ground_002_2_mesh")) != Lepra::String::npos)
		{
			lDimensions.Set(lUphillLength, lRoadWidth, lRoadHeight);
		}
		else if (lFilename.find(_T("ground_002_3_mesh")) != Lepra::String::npos)
		{
			lDimensions.Set(lRoadWidth, lPlateauLength+lPlateauLengthCompensation, lRoadHeight);
		}
		else if (lFilename.find(_T("ground_002_4_mesh")) != Lepra::String::npos)
		{
			lDimensions.Set(lPlateauLength, lRoadWidth, lRoadHeight);
		}
		else if (lFilename.find(_T("ground_002_5_mesh")) != Lepra::String::npos)
		{
			lDimensions.Set(lPlateauLength+lPlateauLengthCompensation, lRoadWidth, lRoadHeight);
		}
		else
		{
			assert(false);
		}
		lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lDimensions.x, lDimensions.y, lDimensions.z);
		lMaterial.SetColor(0.9f, 0.9f, 0.9f);
		lCubeMappingScale = lGeometry->GetBoundingRadius()/20;
	}
	else
	{
		mLog.Errorf(_T("Could not load mesh with name '%s'."), lFilename.c_str());
		assert(false);
	}
	if (lGeometry)
	{
		lGeometry->SetBasicMaterialSettings(lMaterial);
		if (lCubeMappingScale > 0)
		{
			lGeometry->AddEmptyUVSet();
			Lepra::Vector2DD lUVOffset(0.5, 0.5);
			UiTbc::UVMapper::ApplyCubeMapping(lGeometry, 0, lCubeMappingScale, lUVOffset);
			//lGeometry->DupUVSet(0);
		}

		SetRamData(lGeometry);
	}
	return (lGeometry != 0);
}

Cure::ResourceLoadState GeometryResource::PostProcess()
{
	assert(mOptimizedData == UiTbc::Renderer::INVALID_GEOMETRY);
	mOptimizedData = GetUiManager()->GetRenderer()->AddGeometry(
		GetRamData(), UiTbc::Renderer::MAT_NULL, UiTbc::Renderer::NO_SHADOWS);
	assert(mOptimizedData != UiTbc::Renderer::INVALID_GEOMETRY);
	Cure::ResourceLoadState lLoadState;
	if (mOptimizedData == UiTbc::Renderer::INVALID_GEOMETRY)
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

LOG_CLASS_DEFINE(UI_GFX_3D, GeometryResource);



// ----------------------------------------------------------------------------



GeometryReferenceResource::GeometryReferenceResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
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

const Lepra::String GeometryReferenceResource::GetType() const
{
	return (_T("GeometryRef"));
}

bool GeometryReferenceResource::Load()
{
	//assert(IsUnique());
	bool lOk = (mClassResource != 0);
	assert(lOk);
	if (lOk)
	{
		const Lepra::String lFilename = Lepra::StringUtility::Split(GetName(), _T(";"), 1)[0];
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

void GeometryReferenceResource::OnLoadClass(ClassResource*)
{
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

Cure::Resource* UserGeometryReferenceResource::CreateResource(Cure::ResourceManager* pManager, const Lepra::String& pName) const
{
	return (new GeometryReferenceResource(GetUiManager(), pManager, pName));
}

LOG_CLASS_DEFINE(UI_GFX_3D, UserGeometryReferenceResource);



// ----------------------------------------------------------------------------



SoundResource::SoundResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName, SoundDimension pDimension):
	DiversifiedResource<UiLepra::SoundManager::SoundID, UiLepra::SoundManager::SoundInstanceID>(pManager, pName),
	UiResource(pUiManager),
	mDimension(pDimension)
{
	SetRamDataType(UiLepra::SoundManager::INVALID_SOUNDID);
}

SoundResource::~SoundResource()
{
	UserDataTable::Iterator x = mUserDiversifiedTable.First();
	for (; x != mUserDiversifiedTable.End(); ++x)
	{
		UserData lInstanceId = *x;
		GetUiManager()->GetSoundManager()->GetSoundManager()->DeleteSoundInstance(lInstanceId);
	}
	mUserDiversifiedTable.RemoveAll();
	if (GetRamData() != UiLepra::SoundManager::INVALID_SOUNDID)
	{
		GetUiManager()->GetSoundManager()->GetSoundManager()->Release(GetRamData());
		SetRamDataType(UiLepra::SoundManager::INVALID_SOUNDID);
	}
}

bool SoundResource::Load()
{
	assert(!IsUnique());
	assert(GetRamData() == UiLepra::SoundManager::INVALID_SOUNDID);
	if (mDimension == DIMENSION_2D)
	{
		SetRamDataType(GetUiManager()->GetSoundManager()->GetSoundManager()->LoadSound2D(GetName(), UiLepra::SoundManager::LOOP_NONE, 0));
	}
	else
	{
		SetRamDataType(GetUiManager()->GetSoundManager()->GetSoundManager()->LoadSound3D(GetName(), UiLepra::SoundManager::LOOP_NONE, 0));
	}
	return (GetRamData() != UiLepra::SoundManager::INVALID_SOUNDID);
}

SoundResource::UserData SoundResource::CreateDiversifiedData() const
{
	UserData lInstanceId = GetUiManager()->GetSoundManager()->GetSoundManager()->CreateSoundInstance(GetRamData());
	return (lInstanceId);
}

void SoundResource::ReleaseDiversifiedData(UserData pData) const
{
	GetUiManager()->GetSoundManager()->GetSoundManager()->DeleteSoundInstance(pData);
}

SoundResource2d::SoundResource2d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	SoundResource(pUiManager, pManager, pName, DIMENSION_2D)
{
}

const Lepra::String SoundResource2d::GetType() const
{
	return (_T("Sound2D"));
}

SoundResource3d::SoundResource3d(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	SoundResource(pUiManager, pManager, pName, DIMENSION_3D)
{
}

const Lepra::String SoundResource3d::GetType() const
{
	return (_T("Sound3D"));
}



// ----------------------------------------------------------------------------



ClassResource::ClassResource(GameUiManager* pUiManager, Cure::ResourceManager* pManager, const Lepra::String& pName):
	Parent(pManager, pName),
	UiResource(pUiManager)
{
}

ClassResource::~ClassResource()
{
}



/*AnimationResource::AnimationResource(const Lepra::String& pName):
	OptimizedResource(pName)
{
}

bool AnimationResource::Load()
{
}*/



}
