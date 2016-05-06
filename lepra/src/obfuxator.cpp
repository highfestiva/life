
// Author: Jonas Byström
// Copyright (c) 2002-2009, Pixel Doctrine



#include "pch.h"
#include "../include/obfuxator.h"



namespace lepra {



str Obfuxator::de(str s) {
	str deobfuscated;
	size_t obfuscated_length = s.length();
	for (size_t i = 0; i < obfuscated_length; ++i) {
		const char c = s[i];
		deobfuscated += (char)(126-((int)c-32));
		if ((i&11) == 0) {
			s = s.substr(0, i) + s.substr(i+1);
			--obfuscated_length;
		}
	}
	return deobfuscated;
}



}
