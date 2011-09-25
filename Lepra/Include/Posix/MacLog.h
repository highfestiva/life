
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Lepra.h"
#ifdef __OBJC__
#include <Foundation/Foundation.h>
#endif // Objective-C



namespace Lepra
{



class MacLog
{
public:
	static void Write(const str& pText);

#ifdef __OBJC__
	static NSString* Encode(const str& pText);
#endif // Objective-C
};



}
