
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Endian.h"



namespace Lepra
{



Endian::EndianType Endian::mSystemEndian = Endian::GetSystemEndian();

Endian::EndianType Endian::GetSystemEndian()
{
	uint32 lTestValue = 1;
	if (((uint8*)&lTestValue)[0] == 1)
		return (TYPE_LITTLE_ENDIAN);
	else
		return (TYPE_BIG_ENDIAN);
}



int16 Endian::SwapBytes(int16 pValue)
{
	return ((int16)SwapBytes((uint16)pValue));
}

uint16 Endian::SwapBytes(uint16 pValue)
{
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[2];
	lNewData[0] = lOldData[1];
	lNewData[1] = lOldData[0];
	return *((uint16*)lNewData);
}



int32 Endian::SwapBytes(int32 pValue)
{
	return ((int32)SwapBytes((uint32)pValue));
}

uint32 Endian::SwapBytes(uint32 pValue)
{
#ifdef LEPRA_MSVC_X86
// Nifty X86 optimization.
	__asm
	{
		mov	eax,[pValue]
		bswap	eax
		mov	[pValue],eax
	}
	return (pValue);
#else // <Generic target>
	uint8* lOldData = (uint8*)&pValue;
	uint8 lNewData[4];
	lNewData[0] = lOldData[3];
	lNewData[1] = lOldData[2];
	lNewData[2] = lOldData[1];
	lNewData[3] = lOldData[0];
	return *((uint32*)lNewData);
#endif // LEPRA_MSVC_X86/<Generic target>
}



int64 Endian::SwapBytes(int64 pValue)
{
	return ((int64)SwapBytes((uint64)pValue));
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



}
