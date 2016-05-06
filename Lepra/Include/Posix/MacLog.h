
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../lepra.h"
#ifdef __OBJC__
#include <Foundation/Foundation.h>
#endif // Objective-C



namespace lepra {



class MacLog {
public:
	static void Write(const str& text);

#ifdef __OBJC__
	static NSString* Encode(const str& text);
	static str Decode(NSString* text);
#endif // Objective-C
};



}
