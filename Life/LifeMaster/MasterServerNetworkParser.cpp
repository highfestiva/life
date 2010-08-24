
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games
 


#include "MasterServerNetworkParser.h"
#include <assert.h>
#include "../../Lepra/Include/Checksum.h"
#include "../../Lepra/Include/DES.h"
#include "../../Lepra/Include/Packer.h"



namespace Life
{



#define MASTER_SERVER_CRC32_DES_KEY	0x8923479832746565
#define MASTER_SERVER_FOOTER_PADDING32	0x65132154



unsigned MasterServerNetworkParser::StrToRaw(uint8* pRawData, const wstr& pStr)
{
	const int lHeaderSize = 4;
	const int lFooterSize = 8;
	const int lDataSize = PackerUnicodeString::Pack(pRawData+lHeaderSize, pStr);
	PackerInt32::Pack(pRawData, lDataSize+lFooterSize);
	const uint32 lCrc32 = Checksum::CalculateCrc32(pRawData, lHeaderSize+lDataSize);
	PackerInt32::Pack(&pRawData[lHeaderSize+lDataSize], lCrc32);
	PackerInt32::Pack(&pRawData[lHeaderSize+lDataSize+4], MASTER_SERVER_FOOTER_PADDING32);
	DES lDes;
	lDes.SetKey(MASTER_SERVER_CRC32_DES_KEY);
	lDes.Encrypt(&pRawData[lHeaderSize+lDataSize], lFooterSize);
	const int lSize = lHeaderSize + lDataSize + lFooterSize;
	LEPRA_DEBUG_CODE(wstr lTextStr);
	assert(MasterServerNetworkParser::RawToStr(lTextStr, pRawData, lSize));
	assert(lTextStr == pStr);
	return lSize;
}

bool MasterServerNetworkParser::RawToStr(wstr& pStr, const uint8* pRawData, unsigned pRawLength)
{
	const int lHeaderSize = 4;
	const int lFooterSize = 8;
	const int lMinimumCommandSize = 5;
	if (pRawLength < lHeaderSize + lMinimumCommandSize + lFooterSize)
	{
		return false;
	}
	int32 lBodySize;
	if (PackerInt32::Unpack(lBodySize, pRawData, pRawLength) != 4)
	{
		return false;
	}
	if (lBodySize + lHeaderSize != (int)pRawLength)
	{
		return false;
	}
	const int lDataSize = lBodySize - lFooterSize;
	uint8 lEncryptedFooter[lFooterSize];
	::memcpy(lEncryptedFooter, &pRawData[lHeaderSize+lDataSize], lFooterSize);
	DES lDes;
	lDes.SetKey(MASTER_SERVER_CRC32_DES_KEY);
	lDes.Decrypt(lEncryptedFooter, lFooterSize);
	int32 lPadding = 0;
	PackerInt32::Unpack(lPadding, &lEncryptedFooter[4], 4);
	if (lPadding != MASTER_SERVER_FOOTER_PADDING32)
	{
		return false;
	}
	const uint32 lCrc32 = Checksum::CalculateCrc32(pRawData, lHeaderSize+lDataSize);
	int32 lReceivedCrc32 = 0;
	PackerInt32::Unpack(lReceivedCrc32, lEncryptedFooter, 4);
	if (lReceivedCrc32 != (int32)lCrc32)
	{
		return false;
	}
	PackerUnicodeString::Unpack(pStr, &pRawData[lHeaderSize], lDataSize);
	return true;
}



}
