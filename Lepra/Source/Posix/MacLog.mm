
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../Include/Posix/MacLog.h"



namespace Lepra
{



void MacLog::Write(const str& pText)
{
	NSLog(@"%@", MacLog::Encode(pText));
}

NSString* MacLog::Encode(const str& pText)
{
	NSString* lText = [NSString stringWithCString:pText.c_str() encoding:NSUTF8StringEncoding];
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
