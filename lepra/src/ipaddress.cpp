
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "pch.h"
#include "../include/lepraos.h"
#include "../include/lepratypes.h"
#include "../include/ipaddress.h"
#include "../include/string.h"

namespace lepra {

IPAddress::IPAddress() :
	byte_count_(0),
	hash_code_(0),
	is_hash_valid_(false) {
}

IPAddress::IPAddress(const IPAddress& ip_address) :
	byte_count_(ip_address.byte_count_),
	hash_code_(ip_address.hash_code_),
	is_hash_valid_(ip_address.is_hash_valid_) {
	Set(ip_address.address_, byte_count_);
}

IPAddress::IPAddress(unsigned i_p4_address) {
	// Store address in big endian format.
	address_[0] = (uint8)((i_p4_address >> 24) & 0x000000FF);
	address_[1] = (uint8)((i_p4_address >> 16) & 0x000000FF);
	address_[2] = (uint8)((i_p4_address >> 8) & 0x000000FF);
	address_[3] = (uint8)((i_p4_address >> 0) & 0x000000FF);

	byte_count_ = 4;
	is_hash_valid_ = false;
}

IPAddress::IPAddress(const uint8* ip_address, int num_bytes) {
	Set(ip_address, num_bytes);
}

IPAddress::IPAddress(const str& i_pv4_address) {
	unsigned ip_int = inet_addr(i_pv4_address.c_str());
	uint8* ip = (uint8*)&ip_int;
	address_[0] = ip[0];
	address_[1] = ip[1];
	address_[2] = ip[2];
	address_[3] = ip[3];
	byte_count_ = 4;
	is_hash_valid_ = false;
}

IPAddress::~IPAddress() {
}

void IPAddress::Set(const uint8* ip_address, int num_bytes) {
	byte_count_ = num_bytes;

	if (byte_count_ > 16) {
		byte_count_ = 16;
	}

	if (byte_count_ < 0) {
		byte_count_ = 0;
	}

	for (int i = 0; i < byte_count_; i++) {
		address_[i] = ip_address[i];
	}

	is_hash_valid_ = false;
}

void IPAddress::Get(uint8* ip_address) const {
	for (int i = 0; i < byte_count_; i++) {
		ip_address[i] = address_[i];
	}
}

str IPAddress::GetAsString() const {
	str s;

	if (byte_count_ == 4) {
		s = strutil::Format("%i.%i.%i.%i", address_[0], address_[1], address_[2], address_[3]);
	} else if(byte_count_ == 16) {
		// TODO: Implement IPv6 text representation of the IP-address.
	}

	return (s);
}

void IPAddress::CalcHashCode() const {
	hash_code_ = 0;
	for (int i = 0; i < byte_count_; i++) {
		hash_code_ += (unsigned)address_[i] << ((i & 3) * 8);
	}
}

}
