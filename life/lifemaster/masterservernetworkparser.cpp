
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "masterservernetworkparser.h"
#include "../../lepra/include/checksum.h"
#include "../../lepra/include/des.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/lepraos.h"
#include "../../lepra/include/packer.h"
#include "../serverinfo.h"



namespace life {



#define MASTER_SERVER_CRC32_DES_KEY	LEPRA_ULONGLONG(0x8923479832746565)
#define MASTER_SERVER_FOOTER_PADDING32	LEPRA_ULONGLONG(0x65132154)



unsigned MasterServerNetworkParser::StrToRaw(uint8* raw_data, const str& s) {
	const int header_size = 4;
	const int footer_size = 8;
	const int data_size = PackerUnicodeString::Pack(raw_data+header_size, s);
	PackerInt32::Pack(raw_data, data_size+footer_size);
	const uint32 crc32 = Checksum::CalculateCrc32(raw_data, header_size+data_size);
	PackerInt32::Pack(&raw_data[header_size+data_size], crc32);
	PackerInt32::Pack(&raw_data[header_size+data_size+4], MASTER_SERVER_FOOTER_PADDING32);
	DES des;
	des.SetKey(MASTER_SERVER_CRC32_DES_KEY);
	des.Encrypt(&raw_data[header_size+data_size], footer_size);
	const int __size = header_size + data_size + footer_size;
	LEPRA_DEBUG_CODE(str text_str);
	LEPRA_DEBUG_CODE(deb_assert(MasterServerNetworkParser::RawToStr(text_str, raw_data, __size)));
	LEPRA_DEBUG_CODE(deb_assert(text_str == s));
	return __size;
}

bool MasterServerNetworkParser::RawToStr(str& s, const uint8* raw_data, unsigned raw_length) {
	const int header_size = 4;
	const int footer_size = 8;
	const int minimum_command_size = 5;
	if (raw_length < header_size + minimum_command_size + footer_size) {
		return false;
	}
	int32 body_size;
	if (PackerInt32::Unpack(body_size, raw_data, raw_length) != 4) {
		return false;
	}
	if (body_size + header_size != (int)raw_length) {
		return false;
	}
	const int data_size = body_size - footer_size;
	uint8 encrypted_footer[footer_size];
	::memcpy(encrypted_footer, &raw_data[header_size+data_size], footer_size);
	DES des;
	des.SetKey(MASTER_SERVER_CRC32_DES_KEY);
	des.Decrypt(encrypted_footer, footer_size);
	int32 padding = 0;
	PackerInt32::Unpack(padding, &encrypted_footer[4], 4);
	if (padding != MASTER_SERVER_FOOTER_PADDING32) {
		return false;
	}
	const uint32 crc32 = Checksum::CalculateCrc32(raw_data, header_size+data_size);
	int32 received_crc32 = 0;
	PackerInt32::Unpack(received_crc32, encrypted_footer, 4);
	if (received_crc32 != (int32)crc32) {
		return false;
	}
	PackerUnicodeString::Unpack(s, &raw_data[header_size], data_size);
	return true;
}



bool MasterServerNetworkParser::ExtractServerInfo(const str& server_string, ServerInfo& info, const SocketAddress* remote_address) {
	strutil::strvec command_list = strutil::BlockSplit(server_string, " \t\r\n", false, false);
	if (command_list.empty()) {
		return false;
	}
	unsigned start_index = 0;
	if (!strutil::StartsWith(command_list[0], "--")) {
		info.command_ = command_list[0];
		start_index = 1;
	}
	for (unsigned x = start_index; x < command_list.size(); x += 2) {
		if (command_list.size() < x+2) {
			log_.Error("Got too few parameters!");
			return false;
		}
		if (command_list[x] == "--name") {
			info.name_ = command_list[x+1];
		} else if (command_list[x] == "--id") {
			info.id_ = command_list[x+1];
		} else if (command_list[x] == "--address") {
			info.given_ip_address_ = command_list[x+1];
		} else if (command_list[x] == "--internal-address") {
			info.internal_ip_address_ = command_list[x+1];
		} else if (command_list[x] == "--port" || command_list[x] == "--internal-port") {
			int port = -1;
			if (!strutil::StringToInt(command_list[x+1], port)) {
				log_.Error("Got non-integer port parameter!");
				return false;
			}
			if (port < 0 || port > 65535) {
				log_.Errorf("Got invalid port number (%i)!", port);
				return false;
			}
			if (command_list[x] == "--port") {
				info.given_port_ = port;
			} else {
				info.internal_port_ = port;
			}
		} else if (command_list[x] == "--player-count") {
			if (!strutil::StringToInt(command_list[x+1], info.player_count_)) {
				log_.Error("Got non-integer player count parameter!");
				return false;
			}
			if (info.player_count_ < 0 || info.player_count_ > 512) {
				log_.Errorf("Got invalid player count number (%i)!", info.player_count_);
				return false;
			}
		} else if (command_list[x] == "--remove") {
			if (command_list[x+1] != "true") {
				log_.Errorf("Got bad --remove argument (%s)!", command_list[x+1].c_str());
				return false;
			}
			info.remove_ = true;
		} else {
			log_.Errorf("Got bad parameter (%s)!", command_list[x].c_str());
			return false;
		}
	}
	if (remote_address) {
		info.remote_ip_address_ = remote_address->GetIP().GetAsString();
		info.remote_port_ = remote_address->GetPort();
	}
	return true;
}



loginstance(kNetwork, MasterServerNetworkParser);



}
