
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../Include/LepraOS.h"
#include "../Include/LepraTypes.h"
#include "../Include/IPAddress.h"
#include "../Include/String.h"

namespace Lepra
{

IPAddress::IPAddress() :
	mByteCount(0),
	mHashCode(0),
	mIsHashValid(false)
{
}

IPAddress::IPAddress(const IPAddress& pIPAddress) :
	mByteCount(pIPAddress.mByteCount),
	mHashCode(pIPAddress.mHashCode),
	mIsHashValid(pIPAddress.mIsHashValid)
{
	Set(pIPAddress.mAddress, mByteCount);
}

IPAddress::IPAddress(unsigned pIP4Address)
{
	// Store address in big endian format.
	mAddress[0] = (uint8)((pIP4Address >> 24) & 0x000000FF);
	mAddress[1] = (uint8)((pIP4Address >> 16) & 0x000000FF);
	mAddress[2] = (uint8)((pIP4Address >> 8) & 0x000000FF);
	mAddress[3] = (uint8)((pIP4Address >> 0) & 0x000000FF);

	mByteCount = 4;
	mIsHashValid = false;
}

IPAddress::IPAddress(const uint8* pIPAddress, int pNumBytes)
{
	Set(pIPAddress, pNumBytes);
}

IPAddress::IPAddress(const str& pIPv4Address)
{
	unsigned lIpInt = inet_addr(pIPv4Address.c_str());
	uint8* lIp = (uint8*)&lIpInt;
	mAddress[0] = lIp[0];
	mAddress[1] = lIp[1];
	mAddress[2] = lIp[2];
	mAddress[3] = lIp[3];
	mByteCount = 4;
	mIsHashValid = false;
}

IPAddress::~IPAddress()
{
}

void IPAddress::Set(const uint8* pIPAddress, int pNumBytes)
{
	mByteCount = pNumBytes;

	if (mByteCount > 16)
	{
		mByteCount = 16;
	}

	if (mByteCount < 0)
	{
		mByteCount = 0;
	}

	for (int i = 0; i < mByteCount; i++)
	{
		mAddress[i] = pIPAddress[i];
	}

	mIsHashValid = false;
}

void IPAddress::Get(uint8* pIPAddress) const
{
	for (int i = 0; i < mByteCount; i++)
	{
		pIPAddress[i] = mAddress[i];
	}
}

str IPAddress::GetAsString() const
{
	str lString;

	if (mByteCount == 4)
	{
		lString = strutil::Format("%i.%i.%i.%i", mAddress[0], mAddress[1], mAddress[2], mAddress[3]);
	}
	else if(mByteCount == 16)
	{
		// TODO: Implement IPv6 text representation of the IP-address.
	}

	return (lString);
}

void IPAddress::CalcHashCode() const
{
	mHashCode = 0;
	for (int i = 0; i < mByteCount; i++)
	{
		mHashCode += (unsigned)mAddress[i] << ((i & 3) * 8);
	}
}

}
