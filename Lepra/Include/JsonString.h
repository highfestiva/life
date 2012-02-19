
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "String.h"



namespace Lepra
{



class JsonString
{
public:
	static str toJson(const str& pString);
	static str fromJson(const str& pJsonString);
};



}
