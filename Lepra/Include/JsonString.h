
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
