
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



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
	ChunkyPhysics* mPhysics;

	Lepra::String mPhysicsBaseName;

	LOG_CLASS_DECLARE();
};



}
