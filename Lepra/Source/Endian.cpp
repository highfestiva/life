/*
	Class:  Endian
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/Endian.h"

namespace Lepra
{

Endian::EndianType Endian::mSystemEndian = Endian::GetSystemEndian();

Endian::EndianType Endian::GetSystemEndian()
{
	uint16 lTestValue = 1;
	if (((uint8*)&lTestValue)[0] == 1)
		return TYPE_LITTLE_ENDIAN;
	else
		return TYPE_BIG_ENDIAN;
}

int16 Endian::SwapBytes(int16 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[2];

	lNewData[0] = lOldData[1];
	lNewData[1] = lOldData[0];

	return *((int16*)lNewData);
}

int64 Endian::SwapBytes(int64 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lData[sizeof(int64)];

	for (int64 i = 0; i < (int64)sizeof(int64); i++)
	{
		lData[i] = lOldData[sizeof(int64) - (i + 1)];
	}

	return *(int64*)lData;
}

#ifdef LEPRA_MSVC_X86
// Nifty X86 optimization.
int32 Endian::SwapBytes(int32 pValue)
{
	__asm
	{
		mov	eax,[pValue]
		bswap	eax
		mov	[pValue],eax
	}
	return (pValue);
}
#else // <Generic target>
int32 Endian::SwapBytes(int32 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[4];

	lNewData[0] = lOldData[3];
	lNewData[1] = lOldData[2];
	lNewData[2] = lOldData[1];
	lNewData[3] = lOldData[0];

	return *((int32*)lNewData);
}
#endif // LEPRA_MSVC_X86/<Generic target>

uint16 Endian::SwapBytes(uint16 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[2];

	lNewData[0] = lOldData[1];
	lNewData[1] = lOldData[0];

	return *((uint16*)lNewData);
}

uint64 Endian::SwapBytes(uint64 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lData[sizeof(uint64)];

	for (int64 i = 0; i < (int64)sizeof(int64); i++)
	{
		lData[i] = lOldData[sizeof(uint64) - (i + 1)];
	}
	return *(uint64*)lData;
}

uint32 Endian::SwapBytes(uint32 pValue)
{
	return ((uint32)SwapBytes((int32)pValue));
}

float32 Endian::SwapBytes(float32 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[4];

	lNewData[0] = lOldData[3];
	lNewData[1] = lOldData[2];
	lNewData[2] = lOldData[1];
	lNewData[3] = lOldData[0];

	return *((float32*)lNewData);
}

float64 Endian::SwapBytes(float64 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[8];

	lNewData[0] = lOldData[7];
	lNewData[1] = lOldData[6];
	lNewData[2] = lOldData[5];
	lNewData[3] = lOldData[4];
	lNewData[4] = lOldData[3];
	lNewData[5] = lOldData[2];
	lNewData[6] = lOldData[1];
	lNewData[7] = lOldData[0];

	return *((float64*)lNewData);
}

} // End namespace.
