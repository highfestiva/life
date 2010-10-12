
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "Life.h"



namespace Life
{



struct ServerInfo
{
	str mCommand;
	str mName;
	str mId;
	str mGivenIpAddress;
	str mInternalIpAddress;
	str mRemoteIpAddress;
	int mGivenPort;
	int mInternalPort;
	int mRemotePort;
	int mPlayerCount;
	bool mRemove;
	double mPing;

	ServerInfo():
		mGivenPort(-1),
		mRemotePort(-1),
		mPlayerCount(-1),
		mRemove(false),
		mPing(-1)
	{
	}

	bool operator!=(const ServerInfo& pOther)
	{
		return mCommand != pOther.mCommand ||
			mName != pOther.mName ||
			mId != pOther.mId ||
			mGivenIpAddress != pOther.mGivenIpAddress ||
			mRemoteIpAddress != pOther.mRemoteIpAddress ||
			mGivenPort != pOther.mGivenPort ||
			mRemotePort != pOther.mRemotePort ||
			mPlayerCount != pOther.mPlayerCount ||
			mRemove != pOther.mRemove ||
			mPing != pOther.mPing;
	};
};

typedef std::vector<ServerInfo> ServerInfoList;



}
