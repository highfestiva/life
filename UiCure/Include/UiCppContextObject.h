
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include "../../Cure/Include/CppContextObject.h"
#include "UiCure.h"
#include "UiResourceManager.h"



namespace UiCure
{



class CppContextObject: public Cure::CppContextObject
{
public:
	typedef Cure::CppContextObject Parent;

	CppContextObject(const Lepra::String& pClassId, GameUiManager* pUiManager);
	virtual ~CppContextObject();

	void StartLoading();

	void OnPhysicsTick();

	bool StartLoadGraphics(Cure::UserResource* pParentResource);

	void DebugDrawAxes();

private:
	void OnLoadClass(UserClassResource* pClassResource);
	void OnLoadMesh(UserGeometryReferenceResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);
	void TryAddTexture();
	virtual void TryComplete();

	GameUiManager* mUiManager;
	UserClassResource* mUiClassResource;
	typedef std::vector<UserGeometryReferenceResource*> MeshArray;
	MeshArray mMeshResourceArray;
	UserRendererImageResource mTextureResource;

	LOG_CLASS_DECLARE();
};



}
