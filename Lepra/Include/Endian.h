
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "LepraTypes.h"



namespace Lepra
{



class Endian
{
public:
	enum EndianType
	{
		TYPE_BIG_ENDIAN = 2,
		TYPE_LITTLE_ENDIAN
	};

	// Convertion between big/little endian and host (system) endian.
	inline static int16   BigToHost(int16 pValue);
	inline static int64   BigToHost(int64 pValue);
	inline static int32   BigToHost(int32 pValue);
	inline static uint16  BigToHost(uint16 pValue);
	inline static uint64  BigToHost(uint64 pValue);
	inline static uint32  BigToHost(uint32 pValue);
	inline static float32 BigToHost(float32 pValue);
	inline static float64 BigToHost(float64 pValue);
	inline static float32 BigToHostF(uint32 pValue);
	inline static float64 BigToHostF(uint64 pValue);

	inline static int16   LittleToHost(int16 pValue);
	inline static int64   LittleToHost(int64 pValue);
	inline static int32   LittleToHost(int32 pValue);
	inline static uint16  LittleToHost(uint16 pValue);
	inline static uint64  LittleToHost(uint64 pValue);
	inline static uint32  LittleToHost(uint32 pValue);
	inline static float32 LittleToHost(float32 pValue);
	inline static float64 LittleToHost(float64 pValue);
	inline static float32 LittleToHostF(uint32 pValue);
	inline static float64 LittleToHostF(uint64 pValue);

	inline static int16   HostToBig(int16 pValue);
	inline static int64   HostToBig(int64 pValue);
	inline static int32   HostToBig(int32 pValue);
	inline static uint16  HostToBig(uint16 pValue);
	inline static uint64  HostToBig(uint64 pValue);
	inline static uint32  HostToBig(uint32 pValue);
	inline static float32 HostToBig(float32 pValue);
	inline static float64 HostToBig(float64 pValue);
	inline static uint32  HostToBigF(float32 pValue);
	inline static uint64  HostToBigF(float64 pValue);

	inline static int16   HostToLittle(int16 pValue);
	inline static int64   HostToLittle(int64 pValue);
	inline static int32   HostToLittle(int32 pValue);
	inline static uint16  HostToLittle(uint16 pValue);
	inline static uint64  HostToLittle(uint64 pValue);
	inline static uint32  HostToLittle(uint32 pValue);
	inline static float32 HostToLittle(float32 pValue);
	inline static float64 HostToLittle(float64 pValue);
	inline static uint32  HostToLittleF(float32 pValue);
	inline static uint64  HostToLittleF(float64 pValue);

	inline static int16   HostTo(EndianType pTargetEndian, int16 pValue);
	inline static int64   HostTo(EndianType pTargetEndian, int64 pValue);
	inline static int32   HostTo(EndianType pTargetEndian, int32 pValue);
	inline static uint16  HostTo(EndianType pTargetEndian, uint16 pValue);
	inline static uint64  HostTo(EndianType pTargetEndian, uint64 pValue);
	inline static uint32  HostTo(EndianType pTargetEndian, uint32 pValue);
	inline static float32 HostTo(EndianType pTargetEndian, float32 pValue);
	inline static float64 HostTo(EndianType pTargetEndian, float64 pValue);

	static EndianType GetSystemEndian();

protected:
private:
	static EndianType mSystemEndian;

	static int16  SwapBytes(int16 pValue);
	static uint16 SwapBytes(uint16 pValue);
	static int32  SwapBytes(int32 pValue);
	static uint32 SwapBytes(uint32 pValue);
	static int64  SwapBytes(int64 pValue);
	static uint64 SwapBytes(uint64 pValue);
};

int16 Endian::HostTo(EndianType pTargetEndian, int16 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

int64 Endian::HostTo(EndianType pTargetEndian, int64 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

int32 Endian::HostTo(EndianType pTargetEndian, int32 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

uint16 Endian::HostTo(EndianType pTargetEndian, uint16 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

uint64 Endian::HostTo(EndianType pTargetEndian, uint64 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

uint32 Endian::HostTo(EndianType pTargetEndian, uint32 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

float32 Endian::HostTo(EndianType pTargetEndian, float32 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

float64 Endian::HostTo(EndianType pTargetEndian, float64 pValue)
{
	if (pTargetEndian == TYPE_BIG_ENDIAN)
		return HostToBig(pValue);
	else
		return HostToLittle(pValue);
}

int16 Endian::BigToHost(int16 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int64 Endian::BigToHost(int64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int32 Endian::BigToHost(int32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint16 Endian::BigToHost(uint16 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint64 Endian::BigToHost(uint64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint32 Endian::BigToHost(uint32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

float32 Endian::BigToHost(float32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	uint32 lValue = SwapBytes(*(uint32*)&pValue);
	return (*(float32*)&lValue);
}

float64 Endian::BigToHost(float64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	uint64 lValue = SwapBytes(*(uint64*)&pValue);
	return (*(float64*)&lValue);
}

float32 Endian::BigToHostF(uint32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return *(float32*)&pValue;
	uint32 lValue = SwapBytes(pValue);
	return (*(float32*)&lValue);
}

float64 Endian::BigToHostF(uint64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return *(float64*)&pValue;
	uint64 lValue = SwapBytes(pValue);
	return (*(float64*)&lValue);
}

int16 Endian::LittleToHost(int16 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int64 Endian::LittleToHost(int64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int32 Endian::LittleToHost(int32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint16 Endian::LittleToHost(uint16 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint64 Endian::LittleToHost(uint64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint32 Endian::LittleToHost(uint32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

float32 Endian::LittleToHost(float32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	uint32 lValue = SwapBytes(*(uint32*)&pValue);
	return (*(float32*)&lValue);
}

float64 Endian::LittleToHost(float64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	uint64 lValue = SwapBytes(*(uint64*)&pValue);
	return (*(float64*)&lValue);
}

float32 Endian::LittleToHostF(uint32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return *(float32*)&pValue;
	uint32 lValue = SwapBytes(pValue);
	return (*(float32*)&lValue);
}

float64 Endian::LittleToHostF(uint64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return *(float64*)&pValue;
	uint64 lValue = SwapBytes(pValue);
	return (*(float64*)&lValue);
}

int16 Endian::HostToBig(int16 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int64 Endian::HostToBig(int64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int32 Endian::HostToBig(int32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint16 Endian::HostToBig(uint16 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint64 Endian::HostToBig(uint64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint32 Endian::HostToBig(uint32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

float32 Endian::HostToBig(float32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	uint32 lValue = SwapBytes(*(uint32*)&pValue);
	return (*(float32*)&lValue);
}

float64 Endian::HostToBig(float64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return pValue;
	uint64 lValue = SwapBytes(*(uint64*)&pValue);
	return (*(float64*)&lValue);
}

uint32 Endian::HostToBigF(float32 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return *(uint32*)&pValue;
	return (SwapBytes(*(uint32*)&pValue));
}

uint64 Endian::HostToBigF(float64 pValue)
{
	if (mSystemEndian == TYPE_BIG_ENDIAN)
		return *(uint64*)&pValue;
	return (SwapBytes(*(uint64*)&pValue));
}

int16 Endian::HostToLittle(int16 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int64 Endian::HostToLittle(int64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

int32 Endian::HostToLittle(int32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint16 Endian::HostToLittle(uint16 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint64 Endian::HostToLittle(uint64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

uint32 Endian::HostToLittle(uint32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	else
		return SwapBytes(pValue);
}

float32 Endian::HostToLittle(float32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	uint32 lValue = SwapBytes(*(uint32*)&pValue);
	return (*(float32*)&lValue);
}

float64 Endian::HostToLittle(float64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return pValue;
	uint64 lValue = SwapBytes(*(uint64*)&pValue);
	return (*(float64*)&lValue);
}

uint32 Endian::HostToLittleF(float32 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return *(uint32*)&pValue;
	else
		return SwapBytes(*(uint32*)&pValue);
}

uint64 Endian::HostToLittleF(float64 pValue)
{
	if (mSystemEndian == TYPE_LITTLE_ENDIAN)
		return *(uint64*)&pValue;
	else
		return SwapBytes(*(uint64*)&pValue);
}



}
