
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine
 


#include "pch.h"
#include "MasterServerNetworkParser.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Checksum.h"
#include "../../Lepra/Include/DES.h"
#include "../../Lepra/Include/Packer.h"
#include "../ServerInfo.h"



namespace Life
{



#define MASTER_SERVER_CRC32_DES_KEY	LEPRA_ULONGLONG(0x8923479832746565)
#define MASTER_SERVER_FOOTER_PADDING32	LEPRA_ULONGLONG(0x65132154)



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
	LEPRA_DEBUG_CODE(deb_assert(MasterServerNetworkParser::RawToStr(lTextStr, pRawData, lSize)));
	LEPRA_DEBUG_CODE(deb_assert(lTextStr == pStr));
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



bool MasterServerNetworkParser::ExtractServerInfo(const str& pServerString, ServerInfo& pInfo, const SocketAddress* pRemoteAddress)
{
	strutil::strvec lCommandList = strutil::BlockSplit(pServerString, _T(" \t\r\n"), false, false);
	if (lCommandList.empty())
	{
		return false;
	}
	unsigned lStartIndex = 0;
	if (!strutil::StartsWith(lCommandList[0], _T("--")))
	{
		pInfo.mCommand = lCommandList[0];
		lStartIndex = 1;
	}
	for (unsigned x = lStartIndex; x < lCommandList.size(); x += 2)
	{
		if (lCommandList.size() < x+2)
		{
			mLog.Error(_T("Got too few parameters!"));
			return false;
		}
		if (lCommandList[x] == _T("--name"))
		{
			pInfo.mName = lCommandList[x+1];
		}
		else if (lCommandList[x] == _T("--id"))
		{
			pInfo.mId = lCommandList[x+1];
		}
		else if (lCommandList[x] == _T("--address"))
		{
			pInfo.mGivenIpAddress = lCommandList[x+1];
		}
		else if (lCommandList[x] == _T("--internal-address"))
		{
			pInfo.mInternalIpAddress = lCommandList[x+1];
		}
		else if (lCommandList[x] == _T("--port") || lCommandList[x] == _T("--internal-port"))
		{
			int lPort = -1;
			if (!strutil::StringToInt(lCommandList[x+1], lPort))
			{
				mLog.Error(_T("Got non-integer port parameter!"));
				return false;
			}
			if (lPort < 0 || lPort > 65535)
			{
				mLog.Errorf(_T("Got invalid port number (%i)!"), lPort);
				return false;
			}
			if (lCommandList[x] == _T("--port"))
			{
				pInfo.mGivenPort = lPort;
			}
			else
			{
				pInfo.mInternalPort = lPort;
			}
		}
		else if (lCommandList[x] == _T("--player-count"))
		{
			if (!strutil::StringToInt(lCommandList[x+1], pInfo.mPlayerCount))
			{
				mLog.Error(_T("Got non-integer player count parameter!"));
				return false;
			}
			if (pInfo.mPlayerCount < 0 || pInfo.mPlayerCount > 512)
			{
				mLog.Errorf(_T("Got invalid player count number (%i)!"), pInfo.mPlayerCount);
				return false;
			}
		}
		else if (lCommandList[x] == _T("--remove"))
		{
			if (lCommandList[x+1] != _T("true"))
			{
				mLog.Errorf(_T("Got bad --remove argument (%s)!"), lCommandList[x+1].c_str());
				return false;
			}
			pInfo.mRemove = true;
		}
		else
		{
			mLog.Errorf(_T("Got bad parameter (%s)!"), lCommandList[x].c_str());
			return false;
		}
	}
	if (pRemoteAddress)
	{
		pInfo.mRemoteIpAddress = pRemoteAddress->GetIP().GetAsString();
		pInfo.mRemotePort = pRemoteAddress->GetPort();
	}
	return true;
}



loginstance(NETWORK, MasterServerNetworkParser);



}
