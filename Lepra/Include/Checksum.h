/*
	Class:  Checksum
	Author: Jonas Bystr�
	Copyright (c) 2002-2009, Righteous Games

	The Checksum class defines methods for calculating
	LRC and CRC-32.
*/



#ifndef LEPRA_CHECKSUM_H
#define LEPRA_CHECKSUM_H



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



#endif // !LEPRA_CHECKSUM_H
