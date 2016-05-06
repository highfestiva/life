
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/endian.h"



namespace lepra {



Endian::EndianType Endian::system_endian_ = Endian::GetSystemEndian();

Endian::EndianType Endian::GetSystemEndian() {
	uint32 test_value = 1;
	if (((uint8*)&test_value)[0] == 1)
		return (kTypeLittleEndian);
	else
		return (kTypeBigEndian);
}



int16 Endian::SwapBytes(int16 value) {
	return ((int16)SwapBytes((uint16)value));
}

uint16 Endian::SwapBytes(uint16 value) {
	uint8* old_data = (uint8*)&value;
	uint8 new_data[2];
	new_data[0] = old_data[1];
	new_data[1] = old_data[0];
	return *((uint16*)new_data);
}



int32 Endian::SwapBytes(int32 value) {
	return ((int32)SwapBytes((uint32)value));
}

uint32 Endian::SwapBytes(uint32 value) {
#ifdef LEPRA_MSVC_X86_32
// Nifty X86 optimization.
	__asm {
		mov	eax,[value]
		bswap	eax
		mov	[value],eax
	}
	return (value);
#else // <Generic target>
	return ((value>>24) + ((value>>8)&0xFF00) + ((value<<8)&0xFF0000) + (value<<24));
#endif // LEPRA_MSVC_X86_32/<Generic target>
}



int64 Endian::SwapBytes(int64 value) {
	return ((int64)SwapBytes((uint64)value));
}

uint64 Endian::SwapBytes(uint64 value) {
	uint8* old_data = (uint8*)&value;
	uint8 data[sizeof(uint64)];

	for (int64 i = 0; i < (int64)sizeof(int64); i++) {
		data[i] = old_data[sizeof(uint64) - (i + 1)];
	}
	return *(uint64*)data;
}



}
