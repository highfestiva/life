
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "String.h"



namespace Lepra
{



class JsonString
{
public:
	static str ToJson(const str& pString);
	static str FromJson(const str& pJsonString);

	static str UrlEncode(const str& pUrl);
};



}
