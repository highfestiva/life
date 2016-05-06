
// Author: Jonas Byström
// Copyright (c) PixelDoctrin

#pragma once
#include "string.h"

namespace lepra {

class File;

class FileOpener {
public:
	virtual ~FileOpener() {};
	virtual File* Open(const str& filename) = 0;
};

}
