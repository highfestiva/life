
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "UiCure.h"
#include "UiResourceManager.h"



// TODO: remove!
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

	CppContextObject(const Lepra::String& pClassId, GameUiManager* pUiManager);
	virtual ~CppContextObject();

	virtual void StartLoading();

	void OnPhysicsTick();

	void DebugDrawPrimitive(DebugPrimitive pPrimitive);

private:
	void __GetFuckedUpMeshesRemoveMe(UiTbc::ChunkyClass* pClass) const;

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);
	void TryAddTexture();
	virtual bool TryComplete();
	void OnLoadSound3d(UserSound3dResource* pSoundResource);

	GameUiManager* mUiManager;
	UserClassResource* mUiClassResource;
	typedef std::vector<UserGeometryReferenceResource*> MeshArray;
	MeshArray mMeshResourceArray;
	size_t mMeshLoadCount;
	UserRendererImageResource mTextureResource;
	UserSound3dResource* mSoundResource;

	LOG_CLASS_DECLARE();
};



}
