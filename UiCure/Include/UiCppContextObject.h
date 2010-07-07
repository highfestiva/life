
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

	virtual void StartLoading();

	void OnPhysicsTick();
	void UiMove();
	void ActivateLerp();

	void DebugDrawPrimitive(DebugPrimitive pPrimitive);

protected:
	GameUiManager* GetUiManager() const;
	const UiTbc::ChunkyClass* GetClass() const;
	TBC::GeometryBase* GetMesh(int pIndex) const;

	void OnLoadClass(UserClassResource* pClassResource);
	virtual void LoadTextures();
	void OnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	virtual void DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);
	void TryAddTexture();
	virtual bool TryComplete();
	virtual str GetMeshInstanceId() const;

	typedef std::vector<UserGeometryReferenceResource*> MeshArray;
	typedef std::vector<UserRendererImageResource*> TextureArray;

	GameUiManager* mUiManager;
	UserClassResource* mUiClassResource;
	bool mEnableUi;
	MeshArray mMeshResourceArray;
	size_t mMeshLoadCount;
	TextureArray mTextureResourceArray;
	size_t mTextureLoadCount;
	TransformationF mMeshOffset;
	bool mStartMeshSlide;

	LOG_CLASS_DECLARE();
};



}
