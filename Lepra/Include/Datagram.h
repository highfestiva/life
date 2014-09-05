
// Author: Jonas Bystr�m, Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once



namespace Lepra
{



// Simple data storage, used with both UDP and TCP protocol.
class Datagram
{
public:
	enum
	{
		BUFFER_SIZE = 1024,
	};

	inline void Init()
	{
		mDataSize = 0;
	}

	uint8 mDataBuffer[BUFFER_SIZE];
	int mDataSize;
};



}
