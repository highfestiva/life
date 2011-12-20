
// Author: Jonas Byström
// Copyright (c) 2002-2011, PixelDoctrin

#pragma once
#include "String.h"

namespace Lepra
{

class File;

class FileOpener
{
public:
	virtual File* Open(const str& pFilename) = 0;
};

}
