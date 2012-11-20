
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "UiCppContextObject.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../UiTBC/Include/UiChunkyClass.h"



namespace UiCure
{



class Machine: public CppContextObject
{
public:
	typedef CppContextObject Parent;

	Machine(Cure::ResourceManager* pResourceManager, const str& pClassId, GameUiManager* pUiManager);
	virtual ~Machine();
	void DeleteEngineSounds();

protected:
	void OnTick();

private:
	void LoadPlaySound3d(UserSound3dResource* pSoundResource);

	typedef std::hash_map<const UiTbc::ChunkyClass::Tag*, UserSound3dResource*, LEPRA_VOIDP_HASHER> TagSoundTable;
	typedef std::vector<float> TagSoundIntensityArray;

	TagSoundTable mEngineSoundTable;
	TagSoundIntensityArray mEngineSoundIntensity;
	float mExhaustTimeout;

	LOG_CLASS_DECLARE();
};



}
