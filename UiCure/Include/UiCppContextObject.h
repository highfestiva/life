
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "UiCure.h"
#include "UiResourceManager.h"



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
	enum DebugPrimitive
	{
		DEBUG_AXES = 1,
		DEBUG_JOINTS,
		DEBUG_SHAPES,
	};

	CppContextObject(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~CppContextObject();

	void EnableUi(bool pEnable);
	void DisableRootShadow();
	void SetUseDefaultTexture(bool pUseDefaultTexture);
	void EnablePixelShader(bool pEnable);
	void EnableMeshMove(bool pEnable);
	void EnableMeshSlide(bool pEnable);

	virtual void StartLoading();

	void OnTick();
	virtual void UiMove();
	void ActivateLerp();

	TBC::GeometryBase* GetMesh(int pIndex) const;
	UserGeometryReferenceResource* GetMeshResource(int pIndex) const;
	void CenterMeshes();
	virtual void UpdateMaterial(int pMeshIndex);

	virtual void ReplaceTexture(int pTextureIndex, const str& pNewTextureName);

	void DebugDrawPrimitive(DebugPrimitive pPrimitive);

	GameUiManager* GetUiManager() const;
	virtual const TBC::ChunkyClass* GetClass() const;

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
	MeshArray mMeshResourceArray;
	size_t mMeshLoadCount;
	TextureArray mTextureResourceArray;
	size_t mTextureLoadCount;
	TransformationF mLerpOffset;
	MeshSlideMode mLerpMode;

	LOG_CLASS_DECLARE();
};



}
