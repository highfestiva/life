
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "LepraTypes.h"
#include "String.h"

namespace Lepra
{

class IPAddress
{
public:
	IPAddress();
	IPAddress(const IPAddress& pIPAddress);
	IPAddress(unsigned pIP4Address);
	IPAddress(const uint8* pIPAddress, int pNumBytes);
	IPAddress(const str& pIPv4Address);
	~IPAddress();

	void Set(const uint8* pIPAddress, int pNumBytes);
	inline void Set(const IPAddress& pIP);

	void Get(uint8* pIPAddress) const;
	inline int GetNumBytes() const;
	inline uint8 Read(int pIndex);

	str GetAsString() const;

	inline size_t operator() (const IPAddress& pAddr) const;

	inline bool operator== (const IPAddress& pIP) const;
	inline bool operator!= (const IPAddress& pIP) const;

protected:
private:

	void CalcHashCode() const;

	// 16 bytes to support IPv6.
	uint8 mAddress[16];
	int mByteCount;
	mutable unsigned mHashCode;
	bool mIsHashValid;
};

int IPAddress::GetNumBytes() const
{
	return mByteCount;
}

uint8 IPAddress::Read(int pIndex)
{
	return mAddress[pIndex];
}

void IPAddress::Set(const IPAddress& pIP)
{
	Set(pIP.mAddress, pIP.mByteCount);
}

bool IPAddress::operator== (const IPAddress& pIP) const
{
	if (mByteCount != pIP.mByteCount)
	{
		return false;
	}

	for (int i = 0; i < mByteCount; i++)
	{
		if (mAddress[i] != pIP.mAddress[i])
		{
			return false;
		}
	}

	return true;
}

bool IPAddress::operator!= (const IPAddress& pIP) const
{
	return !(*this == pIP);
}

size_t IPAddress::operator() (const IPAddress& pAddr) const
{
	if (!pAddr.mIsHashValid)
	{
		((IPAddress&)pAddr).CalcHashCode();
	}
	return (pAddr.mHashCode);
}

}
