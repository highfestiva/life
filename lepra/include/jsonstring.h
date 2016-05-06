
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "string.h"



namespace lepra {



class JsonString {
public:
	static str ToJson(const str& s);
	static str FromJson(const str& json_string);

	static str UrlEncode(const str& url);
};



}
