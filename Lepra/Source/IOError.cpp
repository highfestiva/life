/*
	Class:  -
	Author: Jonas Bystr�m
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/IOError.h"

namespace Lepra
{

const char* gIOErrorString[] = 
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

const char* GetIOErrorString(IOError pErr)
{
	const char* lString = gIOErrorString[(int)NUM_IO_ERRORS];

	int lErr = (int)pErr;
	if (lErr >= 0 && lErr < NUM_IO_ERRORS)
	{
		lString = gIOErrorString[lErr];
	}

	return lString;
}

} // End namespace.
