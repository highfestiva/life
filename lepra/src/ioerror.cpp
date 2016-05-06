/*
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/ioerror.h"

namespace lepra {

const char* kIOErrorString[] =
{
	"OK",
	"File is already open.",
	"File not found.",
	"Couldn't create file.",
	"Stream not open.",
	"Failed writing to stream.",
	"Failed reading from stream.",
	"Invalid format.",
	"Invalid filename.",
	"Invalid mode.",
	"Permission denied.",
	"Invalid operation.",
	"Cancelled.",
	"Package not open.",
	"Buffer overflow.",
	"Buffer underflow.",
	"No data available.",
	"Unknown IOError."
};

const char* GetIOErrorString(IOError err) {
	const char* s = kIOErrorString[(int)kNumIoErrors-1];

	int _err = (int)err;
	if (_err >= 0 && _err < kNumIoErrors) {
		s = kIOErrorString[_err];
	}

	return s;
}

}
