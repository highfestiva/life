
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "TBC.h"



namespace TBC
{



class ChunkyPhysics;



class ChunkyClass
{
public:
	ChunkyClass();
	virtual ~ChunkyClass();

	const Lepra::String& GetPhysicsBaseName() const;
	Lepra::String& GetPhysicsBaseName();

protected:
private:
	Lepra::String mPhysicsBaseName;

	LOG_CLASS_DECLARE();
};



}
