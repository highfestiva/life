
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "../../ThirdParty/FastDelegate/FastDelegate.h"
#include "UiResourceManager.h"
#include "UiCure.h"



namespace Tbc
{
class GeometryBase;
}
namespace Cure
{
class ResourceManager;
}
namespace UiTbc
{
class ChunkyClass;
}



namespace UiCure
{



class CppContextObject: public Cure::CppContextObject
{
	typedef Cure::CppContextObject Parent;
public:
	typedef fastdelegate::FastDelegate1<UserGeometryReferenceResource*, void> PostLoadMaterialDelegate;

	CppContextObject(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~CppContextObject();

	void EnableUi(bool pEnable);
	void EnableRootShadow(bool pEnable);
	void SetUseDefaultTexture(bool pUseDefaultTexture);
	void EnablePixelShader(bool pEnable);
	void EnableMeshMove(bool pEnable);
	void EnableMeshSlide(bool pEnable);
	void SetPositionFinalized();

	virtual void StartLoading();

	void OnTick();
	virtual void UiMove();
	void ActivateLerp();
	void SetSinking(float pSinkSpeed);
	void ShrinkMeshBigOrientationThreshold(float pThreshold);

	Tbc::GeometryBase* GetMesh(int pIndex) const;
	UserGeometryReferenceResource* GetMeshResource(int pIndex) const;
	void CenterMeshes();
	virtual void UpdateMaterial(int pMeshIndex);
	void SetPostLoadMaterialDelegate(const PostLoadMaterialDelegate& pDelegate);

	virtual void ReplaceTexture(int pTextureIndex, const str& pNewTextureName);

	GameUiManager* GetUiManager() const;
	virtual const Tbc::ChunkyClass* GetClass() const;

protected:
	enum MeshSlideMode
	{
		LERP_STOP,
		LERP_START,
		LERP_RUN,
	};

	void OnLoadClass(UserClassResource* pClassResource);
	virtual void LoadTextures();
	void OnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	virtual void DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);
	virtual void TryAddTexture();
	virtual bool TryComplete();
	virtual str GetMeshInstanceId() const;

	typedef std::vector<UserGeometryReferenceResource*> MeshArray;
	typedef std::vector<UserRendererImageResource*> TextureArray;

	GameUiManager* mUiManager;
	UserClassResource* mUiClassResource;
	bool mEnableUi;
	bool mAllowRootShadow;
	bool mUseDefaultTexture;
	bool mEnablePixelShader;
	bool mEnableMeshMove;
	bool mEnableMeshSlide;
	float mSinkSpeed;
	float mSinkOffset;
	MeshArray mMeshResourceArray;
	size_t mMeshLoadCount;
	TextureArray mTextureResourceArray;
	size_t mTextureLoadCount;
	xform mLerpOffset;
	MeshSlideMode mLerpMode;
	PostLoadMaterialDelegate mPostLoadMaterialDelegate;

	logclass();
};



}
