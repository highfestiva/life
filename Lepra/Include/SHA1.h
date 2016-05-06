/*
	Class:  SHA1
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	SHA stands for Secure Hash Algoritm. SHA-1
	delivers a 20 byte digest from arbitrary input
	data.
*/



#pragma once

#include "lepratypes.h"



namespace lepra {



class SHA1 {
public:
	SHA1();
	void Update(const uint8* data, size_t data_length);
	void Finish(uint8* hash);
	static void Hash(const uint8* data, size_t data_length, uint8* hash);

protected:
	void Process(const uint8 data[64]);

	uint64 total_;
	uint32 state_[5];
	uint8 buffer_[64];
};



}
