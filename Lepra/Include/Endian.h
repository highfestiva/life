
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"



namespace lepra {



class Endian {
public:
	enum EndianType {
		kTypeBigEndian = 2,
		kTypeLittleEndian
	};

	// Convertion between big/little endian and host (system) endian.
	inline static int16   BigToHost(int16 value);
	inline static int64   BigToHost(int64 value);
	inline static int32   BigToHost(int32 value);
	inline static uint16  BigToHost(uint16 value);
	inline static uint64  BigToHost(uint64 value);
	inline static uint32  BigToHost(uint32 value);
	inline static float32 BigToHost(float32 value);
	inline static float64 BigToHost(float64 value);
	inline static float32 BigToHostF(uint32 value);
	inline static float64 BigToHostF(uint64 value);

	inline static int16   LittleToHost(int16 value);
	inline static int64   LittleToHost(int64 value);
	inline static int32   LittleToHost(int32 value);
	inline static uint16  LittleToHost(uint16 value);
	inline static uint64  LittleToHost(uint64 value);
	inline static uint32  LittleToHost(uint32 value);
	inline static float32 LittleToHost(float32 value);
	inline static float64 LittleToHost(float64 value);
	inline static float32 LittleToHostF(uint32 value);
	inline static float64 LittleToHostF(uint64 value);

	inline static int16   HostToBig(int16 value);
	inline static int64   HostToBig(int64 value);
	inline static int32   HostToBig(int32 value);
	inline static uint16  HostToBig(uint16 value);
	inline static uint64  HostToBig(uint64 value);
	inline static uint32  HostToBig(uint32 value);
	inline static float32 HostToBig(float32 value);
	inline static float64 HostToBig(float64 value);
	inline static uint32  HostToBigF(float32 value);
	inline static uint64  HostToBigF(float64 value);

	inline static int16   HostToLittle(int16 value);
	inline static int64   HostToLittle(int64 value);
	inline static int32   HostToLittle(int32 value);
	inline static uint16  HostToLittle(uint16 value);
	inline static uint64  HostToLittle(uint64 value);
	inline static uint32  HostToLittle(uint32 value);
	inline static float32 HostToLittle(float32 value);
	inline static float64 HostToLittle(float64 value);
	inline static uint32  HostToLittleF(float32 value);
	inline static uint64  HostToLittleF(float64 value);

	inline static int16   HostTo(EndianType target_endian, int16 value);
	inline static int64   HostTo(EndianType target_endian, int64 value);
	inline static int32   HostTo(EndianType target_endian, int32 value);
	inline static uint16  HostTo(EndianType target_endian, uint16 value);
	inline static uint64  HostTo(EndianType target_endian, uint64 value);
	inline static uint32  HostTo(EndianType target_endian, uint32 value);
	inline static float32 HostTo(EndianType target_endian, float32 value);
	inline static float64 HostTo(EndianType target_endian, float64 value);

	static EndianType GetSystemEndian();

protected:
private:
	static EndianType system_endian_;

	static int16  SwapBytes(int16 value);
	static uint16 SwapBytes(uint16 value);
	static int32  SwapBytes(int32 value);
	static uint32 SwapBytes(uint32 value);
	static int64  SwapBytes(int64 value);
	static uint64 SwapBytes(uint64 value);
};

int16 Endian::HostTo(EndianType target_endian, int16 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

int64 Endian::HostTo(EndianType target_endian, int64 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

int32 Endian::HostTo(EndianType target_endian, int32 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

uint16 Endian::HostTo(EndianType target_endian, uint16 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

uint64 Endian::HostTo(EndianType target_endian, uint64 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

uint32 Endian::HostTo(EndianType target_endian, uint32 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

float32 Endian::HostTo(EndianType target_endian, float32 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

float64 Endian::HostTo(EndianType target_endian, float64 value) {
	if (target_endian == kTypeBigEndian)
		return HostToBig(value);
	else
		return HostToLittle(value);
}

int16 Endian::BigToHost(int16 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

int64 Endian::BigToHost(int64 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

int32 Endian::BigToHost(int32 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

uint16 Endian::BigToHost(uint16 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

uint64 Endian::BigToHost(uint64 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

uint32 Endian::BigToHost(uint32 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

float32 Endian::BigToHost(float32 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	uint32 _value = SwapBytes(*(uint32*)&value);
	return (*(float32*)&_value);
}

float64 Endian::BigToHost(float64 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	uint64 _value = SwapBytes(*(uint64*)&value);
	return (*(float64*)&_value);
}

float32 Endian::BigToHostF(uint32 value) {
	if (system_endian_ == kTypeBigEndian)
		return *(float32*)&value;
	uint32 _value = SwapBytes(value);
	return (*(float32*)&_value);
}

float64 Endian::BigToHostF(uint64 value) {
	if (system_endian_ == kTypeBigEndian)
		return *(float64*)&value;
	uint64 _value = SwapBytes(value);
	return (*(float64*)&_value);
}

int16 Endian::LittleToHost(int16 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

int64 Endian::LittleToHost(int64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

int32 Endian::LittleToHost(int32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

uint16 Endian::LittleToHost(uint16 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

uint64 Endian::LittleToHost(uint64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

uint32 Endian::LittleToHost(uint32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

float32 Endian::LittleToHost(float32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	uint32 _value = SwapBytes(*(uint32*)&value);
	return (*(float32*)&_value);
}

float64 Endian::LittleToHost(float64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	uint64 _value = SwapBytes(*(uint64*)&value);
	return (*(float64*)&_value);
}

float32 Endian::LittleToHostF(uint32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return *(float32*)&value;
	uint32 _value = SwapBytes(value);
	return (*(float32*)&_value);
}

float64 Endian::LittleToHostF(uint64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return *(float64*)&value;
	uint64 _value = SwapBytes(value);
	return (*(float64*)&_value);
}

int16 Endian::HostToBig(int16 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

int64 Endian::HostToBig(int64 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

int32 Endian::HostToBig(int32 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

uint16 Endian::HostToBig(uint16 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

uint64 Endian::HostToBig(uint64 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

uint32 Endian::HostToBig(uint32 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	else
		return SwapBytes(value);
}

float32 Endian::HostToBig(float32 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	uint32 _value = SwapBytes(*(uint32*)&value);
	return (*(float32*)&_value);
}

float64 Endian::HostToBig(float64 value) {
	if (system_endian_ == kTypeBigEndian)
		return value;
	uint64 _value = SwapBytes(*(uint64*)&value);
	return (*(float64*)&_value);
}

uint32 Endian::HostToBigF(float32 value) {
	if (system_endian_ == kTypeBigEndian)
		return *(uint32*)&value;
	return (SwapBytes(*(uint32*)&value));
}

uint64 Endian::HostToBigF(float64 value) {
	if (system_endian_ == kTypeBigEndian)
		return *(uint64*)&value;
	return (SwapBytes(*(uint64*)&value));
}

int16 Endian::HostToLittle(int16 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

int64 Endian::HostToLittle(int64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

int32 Endian::HostToLittle(int32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

uint16 Endian::HostToLittle(uint16 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

uint64 Endian::HostToLittle(uint64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

uint32 Endian::HostToLittle(uint32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	else
		return SwapBytes(value);
}

float32 Endian::HostToLittle(float32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	uint32 _value = SwapBytes(*(uint32*)&value);
	return (*(float32*)&_value);
}

float64 Endian::HostToLittle(float64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return value;
	uint64 _value = SwapBytes(*(uint64*)&value);
	return (*(float64*)&_value);
}

uint32 Endian::HostToLittleF(float32 value) {
	if (system_endian_ == kTypeLittleEndian)
		return *(uint32*)&value;
	else
		return SwapBytes(*(uint32*)&value);
}

uint64 Endian::HostToLittleF(float64 value) {
	if (system_endian_ == kTypeLittleEndian)
		return *(uint64*)&value;
	else
		return SwapBytes(*(uint64*)&value);
}



}
