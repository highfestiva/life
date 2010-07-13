
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../Life.h"



namespace UiTbc
{
class GeometryBatch;
}



namespace Life
{



class MassObject: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	MassObject(Cure::ResourceManager* pResourceManager, const str& pClassResourceName,
		UiCure::GameUiManager* pUiManager, size_t pCopyCount);
	virtual ~MassObject();

	virtual void OnLoaded();
	virtual void UiMove();

private:
	typedef std::pair<UiTbc::GeometryBatch*, UiTbc::Renderer::GeometryID> MassMeshPair;
	typedef std::vector<MassMeshPair> MassMeshArray;

	size_t mCopyCount;
	MassMeshArray mMassMeshArray;

	LOG_CLASS_DECLARE();
};



}
