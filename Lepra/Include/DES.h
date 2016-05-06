
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"



namespace lepra {



class DES {
public:
	void SetKey(uint64 key);
	void Encrypt(uint8* data, unsigned length) const;
	void Decrypt(uint8* data, unsigned length) const;
	uint64 Crypt(uint64 data, bool forward) const;

protected:
	uint64 key_[16];
	static const uint8 s_box_[8][64];
	static const uint8 mask_[16];
	static const uint8 shift_[16];
	static const uint8 k_p1_[56];
	static const uint8 k_p2_[48];
	static const uint8 i_p1_[64];
	static const uint8 i_p2_[64];
	static const uint8 expansion_[48];
	static const uint8 p_box_[32];
};



}
