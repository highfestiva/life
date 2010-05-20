
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

	CppContextObject(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~CppContextObject();

	void EnableUi(bool pEnable);

	virtual void StartLoading();

	void OnPhysicsTick();
	void UiMove();
	void ActivateLerp();
	void OnSoundMoved(const Vector3DF& pPosition, const Vector3DF& pVelocity, float pVolume, float pPitch);

	void DebugDrawPrimitive(DebugPrimitive pPrimitive);

protected:
	GameUiManager* GetUiManager() const;
	const UiTbc::ChunkyClass* GetClass() const;
	TBC::GeometryBase* GetMesh(int pIndex) const;

	void __GetFuckedUpMeshesRemoveMe(UiTbc::ChunkyClass* pClass) const;

	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	virtual void DispatchOnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);
	void TryAddTexture();
	virtual bool TryComplete();
	void OnLoadSound3d(UserSound3dResource* pSoundResource);
	virtual str GetMeshInstanceId() const;

	typedef std::vector<UserGeometryReferenceResource*> MeshArray;

	GameUiManager* mUiManager;
	UserClassResource* mUiClassResource;
	bool mEnableUi;
	MeshArray mMeshResourceArray;
	size_t mMeshLoadCount;
	Vector3DF mSoundPosition;
	Vector3DF mSoundVelocity;
	float mSoundVolume;
	float mSoundPitch;
	float mMeshLerp;

	UserRendererImageResource mTextureResource;
	UserSound3dResource mEngineSoundResource;

	LOG_CLASS_DECLARE();
};



}
