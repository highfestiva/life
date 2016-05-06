
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



namespace cure {



enum RemoteStatus {
	kRemoteOk			= 1,
	kRemoteNoConnection		= 2,
	kRemoteUnknown			= 3,
	kRemoteLoginAlready		= 4,
	kRemoteLoginErronousData	= 5,
	kRemoteLoginBan		= 6,
};



}
