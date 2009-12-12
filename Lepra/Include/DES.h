/*
	Class:  DES
	Author: Jonas Bystr�
	Copyright (c) 2002-2009, Righteous Games

	The DES class has methods for encrypting and
	decrypting data using the single DES algorithm.
*/



#ifndef LEPRA_DES_H
#define LEPRA_DES_H



#include "LepraTypes.h"



namespace Lepra
{



class DES
{
public:
	void SetKey(uint64 pKey);
	void Encrypt(uint8* pData, unsigned pLength) const;
	void Decrypt(uint8* pData, unsigned pLength) const;
	uint64 Crypt(uint64 pData, bool pForward) const;

protected:
	uint64 mKey[16];
	static const uint8 mSBox[8][64];
	static const uint8 mMask[16];
	static const uint8 mShift[16];
	static const uint8 mKP1[56];
	static const uint8 mKP2[48];
	static const uint8 mIP1[64];
	static const uint8 mIP2[64];
	static const uint8 mExpansion[48];
	static const uint8 mPBox[32];
};



}



#endif // !LEPRA_DES_H
