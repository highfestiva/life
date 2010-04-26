
// Author: Jonas Byström
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

	CppContextObject(const str& pClassId, GameUiManager* pUiManager);
	virtual ~CppContextObject();

	virtual void StartLoading();

	void OnPhysicsTick();

	void DebugDrawPrimitive(DebugPrimitive pPrimitive);

protected:
	GameUiManager* GetUiManager() const;
	const UiTbc::ChunkyClass* GetClass() const;
	TBC::GeometryBase* GetMesh(int pIndex) const;

private:
	void __GetFuckedUpMeshesRemoveMe(UiTbc::ChunkyClass* pClass) const;

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);
	void TryAddTexture();
	virtual bool TryComplete();
	void OnLoadSound3d(UserSound3dResource* pSoundResource);
	UserSound3dResource* CreateSharedSound(const str& pId);
	void DeleteSharedSound(const str& pId);

	typedef std::vector<UserGeometryReferenceResource*> MeshArray;
	typedef std::hash_map<str, UserSound3dResource*> SoundTable;

	GameUiManager* mUiManager;
	UserClassResource* mUiClassResource;
	MeshArray mMeshResourceArray;
	size_t mMeshLoadCount;
	UserRendererImageResource mTextureResource;
	UserSound3dResource* mSoundResource;

	static SoundTable mSoundTable;	// Sound resources are shared between split-screen players.

	LOG_CLASS_DECLARE();
};



}
