
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



namespace lepra {



// Simple data storage, used with both UDP and TCP protocol.
class Datagram {
public:
	enum {
		kBufferSize = 1024,
	};

	inline void Init() {
		data_size_ = 0;
	}

	uint8 data_buffer_[kBufferSize];
	int data_size_;
};



}
