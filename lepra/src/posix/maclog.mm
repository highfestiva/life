
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../include/posix/maclog.h"



namespace lepra {



void MacLog::Write(const str& text) {
	NSLog(@"%@", MacLog::Encode(text));
}

NSString* MacLog::Encode(const str& text) {
	NSString* _text = [NSString stringWithCString:text.c_str() encoding:NSUTF8StringEncoding];
	return _text;
}

str MacLog::Decode(NSString* text) {
	if (text == nil) {
		return str();
	}
	const char* cString = [text UTF8String];
	lepra::str utf8_string(cString);
	return lepra::strutil::Encode(utf8_string);
}



}
