
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

	void OnPhysicsTick();

	bool StartLoadGraphics(Cure::UserResource* pParentResource);

	void DebugDrawAxis();

private:
	void OnLoadMesh(UserGeometryReferenceOffsetResource* pMeshResource);
	void OnLoadTexture(UserRendererImageResource* pTextureResource);

	void TryAddTexture();

	GameUiManager* mUiManager;
	typedef std::vector<UserGeometryReferenceOffsetResource*> MeshArray;
	MeshArray mMeshResourceArray;
	UserRendererImageResource mTextureResource;

	LOG_CLASS_DECLARE();
};



class CppContextObjectFactory: public Cure::CppContextObjectFactory
{
public:
	CppContextObjectFactory(GameUiManager* pUiManager, unsigned pPhysicsFps);
	virtual ~CppContextObjectFactory();

	CppContextObject* Create(const Lepra::String& pClassId) const;

private:
	GameUiManager* mUiManager;
};



}
