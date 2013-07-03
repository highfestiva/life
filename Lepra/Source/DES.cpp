/*
	Class:  Checksum
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/



#include "../Include/LepraAssert.h"
#include <memory.h>
#include "../Include/DES.h"



namespace Lepra
{



#define GETBIT(a, array)		((uint8)((a>>array)&1))
#define SETBIT(a, array)		(a |= ((uint64)1<<array))
#define CLRBIT(a, array)		(a &= (~((uint64)1<<array)))
#define PUTBIT(a, array, c)		(c? SETBIT(a, array) : CLRBIT(a, array))
#define STIRBITS(a, array, c, d)	{ d = 0; for (int ii = 0; ii < array; ii++) PUTBIT(d, ii, GETBIT(a, c[ii])); }



void DES::SetKey(uint64 pKey)
{
	uint64 c = 0;
	uint64 d = 0;
	for (int k = 0; k < 28; k++)
	{
		PUTBIT(c, k, GETBIT(pKey, mKP1[k]));
		PUTBIT(d, k, GETBIT(pKey, mKP1[k+28]));
	}
	for (int q = 0; q < 16; q++)
	{
		c = (c>>mShift[q]) | ((c&mMask[q]) << (28-mShift[q]));
		d = (d>>mShift[q]) | ((d&mMask[q]) << (28-mShift[q]));
		uint64 t = ((uint64)d << 28) | c;
		STIRBITS(t, 48, mKP2, mKey[q]);
		/*{
			mKey[q] = 0;
			for (int ii = 0; ii < 48; ii++)
			{
				uint8 a = GETBIT(t, mKP2[ii]);
				PUTBIT(mKey[q], ii, a);
			}
		}*/
	}
	c = d = pKey = 0;	// Swipe stack.
}

void DES::Encrypt(uint8* pData, unsigned pLength) const
{
	deb_assert(pLength%8 == 0);
	pLength >>= 3;
	uint64* lBlock = (uint64*)pData;
	unsigned u;
	for (u = 0; u < pLength; ++u)
	{
		lBlock[u] = Crypt(lBlock[u], true);
	}
}

void DES::Decrypt(uint8* pData, unsigned pLength) const
{
	deb_assert(pLength%8 == 0);
	pLength >>= 3;
	uint64* lBlock = (uint64*)pData;
	for (unsigned u = 0; u < pLength; ++u)
	{
		lBlock[u] = Crypt(lBlock[u], false);
	}
}

uint64 DES::Crypt(uint64 pData, bool pForward) const
{
	uint64 a;
	uint64 array;
	STIRBITS(pData, 64, mIP1, a);
	uint32 r = (uint32)(a>>32);
	uint32 l = (uint32)a;
	uint32 lTemp;
	for (int idx = 0; idx <= 15; ++idx)
	{
		STIRBITS(r, 48, mExpansion, a);
		if (pForward)
		{
			array = a ^ mKey[idx];
		}
		else
		{
			array = a ^ mKey[15 - idx];
		}
		a = 0;
		for (int j = 0; j <= 7; ++j)
		{
			a <<= 4;
			a |= mSBox[j][(array>>((7-j)*6)) & 0x3f];
		}
		STIRBITS(a, 32, mPBox, array);
		lTemp = r;
		r = l^(uint32)array;
		l = lTemp;
	}
	a = ((uint64)l<<32)|r;
	STIRBITS(a, 64, mIP2, pData);
	/*{
		pData = 0;
		for (int ii = 0; ii < 64; ii++)
			PUTBIT(pData, ii, GETBIT(a, mIP2[ii]));
	}*/
	a = array = r = l = lTemp = 0;	// Swipe stack.
	return (pData);
}



// S-box permutation. Even small modifications to the S-box could significantly weaken DES.
const uint8 DES::mSBox[8][64] =
{
	{
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},
	{
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},
	{
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},
	{
		 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},
	{
		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},
	{
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	},
	{
		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	},
	{
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	},
};

// Bit-mask used in key-making.
const uint8 DES::mMask[16] =
{
	1, 1, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 1
};

// Bit-shifts used in key-making.
const uint8 DES::mShift[16] =
{
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

// Key permutation.
const uint8 DES::mKP1[56] =
{
	56, 48, 40, 32, 24, 16,  8,
	 0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26,
	18, 10,  2, 59, 51, 43, 35,
	62, 54, 46, 38, 30, 22, 14,
	 6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28,
	20, 12,  4, 27, 19, 11,  3
};

// Key compression permutation.
const uint8 DES::mKP2[48] =
{
	13, 16, 10, 23,  0,  4,
	 2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7,
	15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54,
	29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52,
	45, 41, 49, 35, 28, 31
};

// Initial permutation IP.
const uint8 DES::mIP1[64] =
{
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7,
	56, 48, 40, 32, 24, 16,  8,  0,
	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6
};

// Final permutation IP.
const uint8 DES::mIP2[64] =
{
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25,
	32,  0, 40,  8, 48, 16, 56, 24
};

// Expansion permutation.
const uint8 DES::mExpansion[48] =
{
	31,  0,  1,  2,  3,  4,
	 3,  4,  5,  6,  7,  8,
	 7,  8,  9, 10, 11, 12,
	11, 12, 13, 14, 15, 16,
	15, 16, 17, 18, 19, 20,
	19, 20, 21, 22, 23, 24,
	23, 24, 25, 26, 27, 28,
	27, 28, 29, 30, 31,  0
};

// P-box permutation.
const uint8 DES::mPBox[32] =
{
	15,  6, 19, 20, 28, 11, 27, 16,
	 0, 14, 22, 25,  4, 17, 30,  9,
	 1,  7, 23, 13, 31, 26,  2,  8,
	18, 12, 29,  5, 21, 10,  3, 24
};



}
