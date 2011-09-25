
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/Posix/MacLog.h"



namespace Lepra
{



void MacLog::Write(const str& pText)
{
	NSLog(@"%@", MacLog::Encode(pText));
}

NSString* MacLog::Encode(const str& pText)
{
#if defined(LEPRA_UNICODE)
	NSString* lText = [NSString stringWithCString:pText.c_str() encoding:NSUTF32StringEncoding];
#else
	NSString* lText = [NSString stringWithCString:pText.c_str() encoding:NSUTF8StringEncoding];
#endif
	return lText;
}



}
