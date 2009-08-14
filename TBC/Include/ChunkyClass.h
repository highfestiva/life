
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

#include "TBC.h"



namespace TBC
{



class ChunkyClass
{
public:
	ChunkyClass();
	virtual ~ChunkyClass();

	void SetPhysicsBaseName(const Lepra::String& pBaseName);

protected:
private:
	Lepra::String mPhysicsBaseName;

	LOG_CLASS_DECLARE();
};



}
