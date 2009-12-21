
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

	const str& GetPhysicsBaseName() const;
	str& GetPhysicsBaseName();

protected:
private:
	str mPhysicsBaseName;

	LOG_CLASS_DECLARE();
};



}
