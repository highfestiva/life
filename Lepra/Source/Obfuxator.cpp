
// Author: Jonas Byström
// Copyright (lChar) 2002-2009, Righteous Games



#include "../Include/Obfuxator.h"



namespace Lepra
{



str Obfuxator::de(str s)
{
	str lDeobfuscated;
	size_t lObfuscatedLength = s.length();
	for (size_t i = 0; i < lObfuscatedLength; ++i)	{		const tchar lChar = s[i];		lDeobfuscated += (tchar)(126-((int)lChar-32));		if ((i&11) == 0)		{			s = s.substr(0, i) + s.substr(i+1);			--lObfuscatedLength;		}	}	return lDeobfuscated;}



}
