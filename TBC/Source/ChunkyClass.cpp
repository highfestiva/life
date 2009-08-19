
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ChunkyClass.h"



namespace TBC
{



ChunkyClass::ChunkyClass()
{
}

ChunkyClass::~ChunkyClass()
{
}



/*bool ChunkyClass::CreateInstance(UserInstance*)
{
	Lepra::DiskFile lFile;
	bool lOk = lFile.Open(GetPhysicsBaseName()+_T(".phys"), Lepra::DiskFile::MODE_READ);
	if (lOk)
	{
		mPhysics = new TBC::ChunkyPhysics();
		TBC::ChunkyPhysicsLoader lLoader(&lFile, false);
		lOk = lLoader.Load(mPhysics);
	}
	return (lOk);
}*/

const Lepra::String& ChunkyClass::GetPhysicsBaseName() const
{
	return (mPhysicsBaseName);
}

Lepra::String& ChunkyClass::GetPhysicsBaseName()
{
	return (mPhysicsBaseName);
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
