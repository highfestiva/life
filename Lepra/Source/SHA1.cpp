/*
	Class:  SHA1
	Author: Jonas Byström
	Copyright (c) 2002-2009, Righteous Games
*/



#include <memory.h>
#include "../Include/SHA1.h"



namespace Lepra
{



#define GET_UINT32(u32, array, idx)	(u32) = ((uint32)(array)[(idx)]<<24) | ((uint32)(array)[(idx)+1]<<16) | ((uint32)(array)[(idx)+2]<<8) | ((uint32)(array)[(idx)+3]);
#define PUT_UINT32(u32, array, idx)	(array)[(idx)+0] = (uint8)((u32)>>24); (array)[(idx)+1] = (uint8)((u32)>>16); (array)[(idx)+2] = (uint8)((u32)>>8); (array)[(idx)+3] = (uint8)(u32)


	
SHA1::SHA1()
{
	mTotal = 0;

	mState[0] = 0x67452301;
	mState[1] = 0xEFCDAB89;
	mState[2] = 0x98BADCFE;
	mState[3] = 0x10325476;
	mState[4] = 0xC3D2E1F0;
}

void SHA1::Update(const uint8* pData, size_t pDataLength)
{
	size_t lLeft = (size_t)mTotal&0x3F;
	size_t lFill = 64-lLeft;
	mTotal += pDataLength;
	if (lLeft && pDataLength >= lFill)
	{
		::memcpy(mBuffer+lLeft, pData, lFill);
		Process(mBuffer);
		pDataLength -= lFill;
		pData += lFill;
		lLeft = 0;
	}
	while (pDataLength >= 64)
	{
		Process(pData);
		pDataLength -= 64;
		pData  += 64;
	}
	if (pDataLength)
	{
		memcpy(mBuffer+lLeft, pData, pDataLength);
	}
}

void SHA1::Finish(uint8* pHash)
{
	uint32 lHigh = (unsigned)(mTotal>>29);
	uint32 lLow  = (unsigned)mTotal<<3;
	uint8 lLengthHash[8];
	PUT_UINT32(lHigh, lLengthHash, 0);
	PUT_UINT32(lLow, lLengthHash, 4);

	size_t lLast = (size_t)mTotal&0x3F;
	size_t lPadLength = (lLast<56)? 56-lLast : 120-lLast;
	static uint8 lSha1Padding[64] =
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	Update(lSha1Padding, lPadLength);
	Update(lLengthHash, 8);

	for (size_t u = 0; u < 5; ++u)
	{
		PUT_UINT32(mState[u], pHash, u<<2);
	}
}

void SHA1::Hash(const uint8* pData, size_t pDataLength, uint8* pHash)
{
	SHA1 lSha1;
	lSha1.Update(pData, pDataLength);
	lSha1.Finish(pHash);
}


void SHA1::Process(const uint8 pData[64])
{
	uint32 W[16];

	GET_UINT32(W[0], pData, 0);
	GET_UINT32(W[1], pData, 4);
	GET_UINT32(W[2], pData, 8);
	GET_UINT32(W[3], pData, 12);
	GET_UINT32(W[4], pData, 16);
	GET_UINT32(W[5], pData, 20);
	GET_UINT32(W[6], pData, 24);
	GET_UINT32(W[7], pData, 28);
	GET_UINT32(W[8], pData, 32);
	GET_UINT32(W[9], pData, 36);
	GET_UINT32(W[10], pData, 40);
	GET_UINT32(W[11], pData, 44);
	GET_UINT32(W[12], pData, 48);
	GET_UINT32(W[13], pData, 52);
	GET_UINT32(W[14], pData, 56);
	GET_UINT32(W[15], pData, 60);

	#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

	uint32 lTemp;
	#define R(t)									\
	(										\
		lTemp = W[(t-3)&0x0F] ^ W[(t-8)&0x0F] ^ W[(t-14)&0x0F]^W[t&0x0F],	\
		(W[t&0x0F] = S(lTemp,1))							\
	)

	#define P(a,b,c,d,e,x)	e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);

	uint32 A = mState[0];
	uint32 B = mState[1];
	uint32 C = mState[2];
	uint32 D = mState[3];
	uint32 E = mState[4];

	#define F(x,y,z) (z ^ (x & (y ^ z)))
	#define K 0x5A827999

	P(A, B, C, D, E, W[0]);
	P(E, A, B, C, D, W[1]);
	P(D, E, A, B, C, W[2]);
	P(C, D, E, A, B, W[3]);
	P(B, C, D, E, A, W[4]);
	P(A, B, C, D, E, W[5]);
	P(E, A, B, C, D, W[6]);
	P(D, E, A, B, C, W[7]);
	P(C, D, E, A, B, W[8]);
	P(B, C, D, E, A, W[9]);
	P(A, B, C, D, E, W[10]);
	P(E, A, B, C, D, W[11]);
	P(D, E, A, B, C, W[12]);
	P(C, D, E, A, B, W[13]);
	P(B, C, D, E, A, W[14]);
	P(A, B, C, D, E, W[15]);
	P(E, A, B, C, D, R(16));
	P(D, E, A, B, C, R(17));
	P(C, D, E, A, B, R(18));
	P(B, C, D, E, A, R(19));

	#undef K
	#undef F

	#define F(x,y,z) (x ^ y ^ z)
	#define K 0x6ED9EBA1

	P(A, B, C, D, E, R(20));
	P(E, A, B, C, D, R(21));
	P(D, E, A, B, C, R(22));
	P(C, D, E, A, B, R(23));
	P(B, C, D, E, A, R(24));
	P(A, B, C, D, E, R(25));
	P(E, A, B, C, D, R(26));
	P(D, E, A, B, C, R(27));
	P(C, D, E, A, B, R(28));
	P(B, C, D, E, A, R(29));
	P(A, B, C, D, E, R(30));
	P(E, A, B, C, D, R(31));
	P(D, E, A, B, C, R(32));
	P(C, D, E, A, B, R(33));
	P(B, C, D, E, A, R(34));
	P(A, B, C, D, E, R(35));
	P(E, A, B, C, D, R(36));
	P(D, E, A, B, C, R(37));
	P(C, D, E, A, B, R(38));
	P(B, C, D, E, A, R(39));

	#undef K
	#undef F

	#define F(x,y,z) ((x&y) | (z&(x|y)))
	#define K 0x8F1BBCDC

	P(A, B, C, D, E, R(40));
	P(E, A, B, C, D, R(41));
	P(D, E, A, B, C, R(42));
	P(C, D, E, A, B, R(43));
	P(B, C, D, E, A, R(44));
	P(A, B, C, D, E, R(45));
	P(E, A, B, C, D, R(46));
	P(D, E, A, B, C, R(47));
	P(C, D, E, A, B, R(48));
	P(B, C, D, E, A, R(49));
	P(A, B, C, D, E, R(50));
	P(E, A, B, C, D, R(51));
	P(D, E, A, B, C, R(52));
	P(C, D, E, A, B, R(53));
	P(B, C, D, E, A, R(54));
	P(A, B, C, D, E, R(55));
	P(E, A, B, C, D, R(56));
	P(D, E, A, B, C, R(57));
	P(C, D, E, A, B, R(58));
	P(B, C, D, E, A, R(59));

	#undef K
	#undef F

	#define F(x,y,z) (x ^ y ^ z)
	#define K 0xCA62C1D6

	P(A, B, C, D, E, R(60));
	P(E, A, B, C, D, R(61));
	P(D, E, A, B, C, R(62));
	P(C, D, E, A, B, R(63));
	P(B, C, D, E, A, R(64));
	P(A, B, C, D, E, R(65));
	P(E, A, B, C, D, R(66));
	P(D, E, A, B, C, R(67));
	P(C, D, E, A, B, R(68));
	P(B, C, D, E, A, R(69));
	P(A, B, C, D, E, R(70));
	P(E, A, B, C, D, R(71));
	P(D, E, A, B, C, R(72));
	P(C, D, E, A, B, R(73));
	P(B, C, D, E, A, R(74));
	P(A, B, C, D, E, R(75));
	P(E, A, B, C, D, R(76));
	P(D, E, A, B, C, R(77));
	P(C, D, E, A, B, R(78));
	P(B, C, D, E, A, R(79));

	#undef K
	#undef F

	mState[0] += A;
	mState[1] += B;
	mState[2] += C;
	mState[3] += D;
	mState[4] += E;
}



}
