
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

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
