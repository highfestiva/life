
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiTBC/Include/UiChunkyClass.h"
#include "../Life.h"



namespace Life
{



class Vehicle: public UiCure::CppContextObject
{
public:
	typedef UiCure::CppContextObject Parent;

	Vehicle(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager);
	virtual ~Vehicle();

protected:
	void OnPhysicsTick();

private:
	void LoadPlaySound3d(UiCure::UserSound3dResource* pSoundResource);

	typedef std::hash_map<const UiTbc::ChunkyClass::Tag*, UiCure::UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;

	TagSoundTable mEngineSoundTable;

	LOG_CLASS_DECLARE();
};



}
