/*
	Class:  SHA1
	Author: Jonas Byström
	Copyright (c) 2002-2009, Righteous Games

	SHA stands for Secure Hash Algoritm. SHA-1
	delivers a 20 byte digest from arbitrary input
	data.
*/



#ifndef LEPRA_SHA1_H
#define LEPRA_SHA1_H



#include "LepraTypes.h"



namespace Lepra
{



class SHA1
{
public:
	SHA1();
	void Update(const uint8* pData, size_t pDataLength);
	void Finish(uint8* pHash);
	static void Hash(const uint8* pData, size_t pDataLength, uint8* pHash);

protected:
	void Process(const uint8 pData[64]);

	uint64 mTotal;
	uint32 mState[5];
	uint8 mBuffer[64];
};



}



#endif // !LEPRA_SHA1_H
