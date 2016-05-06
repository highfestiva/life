/*
	Class:  Checksum
	Author: Jonas Bystr�
	Copyright (c) Pixel Doctrine

	The Checksum class defines methods for calculating
	LRC and CRC-32.
*/



#pragma once

#include "lepratypes.h"



namespace lepra {



class Checksum {
public:
	static uint8 CalculateLrc(const uint8* data, unsigned length);
	static uint32 CalculateCrc32(const uint8* data, unsigned length);

protected:
	static const unsigned crc32_table_[256];
};



}
