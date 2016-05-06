
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once

#include "lepratypes.h"
#include "string.h"

namespace lepra {

class IPAddress {
public:
	IPAddress();
	IPAddress(const IPAddress& ip_address);
	IPAddress(unsigned i_p4_address);
	IPAddress(const uint8* ip_address, int num_bytes);
	IPAddress(const str& i_pv4_address);
	~IPAddress();

	void Set(const uint8* ip_address, int num_bytes);
	inline void Set(const IPAddress& ip);

	void Get(uint8* ip_address) const;
	inline int GetNumBytes() const;
	inline uint8 Read(int index);

	str GetAsString() const;

	inline size_t operator() (const IPAddress& addr) const;

	inline bool operator== (const IPAddress& ip) const;
	inline bool operator!= (const IPAddress& ip) const;

protected:
private:

	void CalcHashCode() const;

	// 16 bytes to support IPv6.
	uint8 address_[16];
	int byte_count_;
	mutable unsigned hash_code_;
	bool is_hash_valid_;
};

int IPAddress::GetNumBytes() const {
	return byte_count_;
}

uint8 IPAddress::Read(int index) {
	return address_[index];
}

void IPAddress::Set(const IPAddress& ip) {
	Set(ip.address_, ip.byte_count_);
}

bool IPAddress::operator== (const IPAddress& ip) const {
	if (byte_count_ != ip.byte_count_) {
		return false;
	}

	for (int i = 0; i < byte_count_; i++) {
		if (address_[i] != ip.address_[i]) {
			return false;
		}
	}

	return true;
}

bool IPAddress::operator!= (const IPAddress& ip) const {
	return !(*this == ip);
}

size_t IPAddress::operator() (const IPAddress& addr) const {
	if (!addr.is_hash_valid_) {
		((IPAddress&)addr).CalcHashCode();
	}
	return (addr.hash_code_);
}

}
