/*
	Class:  Checksum
	Author: Jonas Bystr�
	Copyright (c) Pixel Doctrine

	The Checksum class defines methods for calculating
	LRC and CRC-32.
*/



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class Checksum
{
public:
	static uint8 CalculateLrc(const uint8* pData, unsigned pLength);
	static uint32 CalculateCrc32(const uint8* pData, unsigned pLength);

protected:
	static const unsigned mCrc32Table[256];
};



}
