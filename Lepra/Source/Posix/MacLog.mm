
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
	NSString* lText = [[[NSString alloc] initWithBytes:(const char*)pText.c_str() length:pText.length()*sizeof(wchar_t) encoding:NSUTF32LittleEndianStringEncoding] autorelease];
#else
	NSString* lText = [NSString stringWithCString:pText.c_str() encoding:NSUTF8StringEncoding];
#endif
	return lText;
}

str MacLog::Decode(NSString* pText)
{
	if (pText == nil)
	{
		return str();
	}
	const char* cString = [pText UTF8String];
	Lepra::astr lUtf8String(cString);
	return Lepra::strutil::Encode(lUtf8String);
}



}
