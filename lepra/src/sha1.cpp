/*
	Class:  SHA1
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/



#include "pch.h"
#include <memory.h>
#include "../include/sha1.h"



namespace lepra {



#define GET_UINT32(u32, array, idx)	(u32) = ((uint32)(array)[(idx)]<<24) | ((uint32)(array)[(idx)+1]<<16) | ((uint32)(array)[(idx)+2]<<8) | ((uint32)(array)[(idx)+3]);
#define PUT_UINT32(u32, array, idx)	(array)[(idx)+0] = (uint8)((u32)>>24); (array)[(idx)+1] = (uint8)((u32)>>16); (array)[(idx)+2] = (uint8)((u32)>>8); (array)[(idx)+3] = (uint8)(u32)



SHA1::SHA1() {
	total_ = 0;

	state_[0] = 0x67452301;
	state_[1] = 0xEFCDAB89;
	state_[2] = 0x98BADCFE;
	state_[3] = 0x10325476;
	state_[4] = 0xC3D2E1F0;
}

void SHA1::Update(const uint8* data, size_t data_length) {
	size_t left = (size_t)total_&0x3F;
	size_t fill = 64-left;
	total_ += data_length;
	if (left && data_length >= fill) {
		::memcpy(buffer_+left, data, fill);
		Process(buffer_);
		data_length -= fill;
		data += fill;
		left = 0;
	}
	while (data_length >= 64) {
		Process(data);
		data_length -= 64;
		data  += 64;
	}
	if (data_length) {
		memcpy(buffer_+left, data, data_length);
	}
}

void SHA1::Finish(uint8* hash) {
	uint32 high = (unsigned)(total_>>29);
	uint32 low  = (unsigned)total_<<3;
	uint8 length_hash[8];
	PUT_UINT32(high, length_hash, 0);
	PUT_UINT32(low, length_hash, 4);

	size_t last = (size_t)total_&0x3F;
	size_t pad_length = (last<56)? 56-last : 120-last;
	static uint8 sha1_padding[64] =
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	Update(sha1_padding, pad_length);
	Update(length_hash, 8);

	for (size_t u = 0; u < 5; ++u) {
		PUT_UINT32(state_[u], hash, u<<2);
	}
}

void SHA1::Hash(const uint8* data, size_t data_length, uint8* hash) {
	SHA1 sha1;
	sha1.Update(data, data_length);
	sha1.Finish(hash);
}


void SHA1::Process(const uint8 data[64]) {
	uint32 W[16];

	GET_UINT32(W[0], data, 0);
	GET_UINT32(W[1], data, 4);
	GET_UINT32(W[2], data, 8);
	GET_UINT32(W[3], data, 12);
	GET_UINT32(W[4], data, 16);
	GET_UINT32(W[5], data, 20);
	GET_UINT32(W[6], data, 24);
	GET_UINT32(W[7], data, 28);
	GET_UINT32(W[8], data, 32);
	GET_UINT32(W[9], data, 36);
	GET_UINT32(W[10], data, 40);
	GET_UINT32(W[11], data, 44);
	GET_UINT32(W[12], data, 48);
	GET_UINT32(W[13], data, 52);
	GET_UINT32(W[14], data, 56);
	GET_UINT32(W[15], data, 60);

	#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

	uint32 temp;
	#define R(t)									\
	(										\
		temp = W[(t-3)&0x0F] ^ W[(t-8)&0x0F] ^ W[(t-14)&0x0F]^W[t&0x0F],	\
		(W[t&0x0F] = S(temp,1))							\
	)

	#define P(a,b,c,d,e,x)	e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);

	uint32 A = state_[0];
	uint32 B = state_[1];
	uint32 C = state_[2];
	uint32 D = state_[3];
	uint32 E = state_[4];

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

	state_[0] += A;
	state_[1] += B;
	state_[2] += C;
	state_[3] += D;
	state_[4] += E;
}



}
