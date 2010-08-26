
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <vector>
#include "../Life.h"



namespace Life
{



struct ServerInfo
{
	str mCommand;
	str mName;
	str mAddress;
	str mId;
	int mPort;
	int mPlayerCount;
	bool mRemove;
	double mPing;

	ServerInfo():
		mPort(-1),
		mPlayerCount(-1),
		mRemove(false),
		mPing(-1)
	{
	}

	bool operator!=(const ServerInfo& pOther)
	{
		return mCommand != pOther.mCommand ||
			mName != pOther.mName ||
			mAddress != pOther.mAddress ||
			mPort != pOther.mPort ||
			mPlayerCount != pOther.mPlayerCount ||
			mId != pOther.mId ||
			mRemove != pOther.mRemove ||
			mPing != pOther.mPing;
	};
};

typedef std::vector<ServerInfo> ServerInfoList;



}
