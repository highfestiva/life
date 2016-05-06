
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/packet.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/packer.h"
#include "../../lepra/include/socket.h"
#include "../include/positionaldata.h"



namespace cure {



Packet::Packet(MessageFactory* message_factory):
	message_factory_(message_factory),
	packet_size_(PACKET_SIZE_MARKER_LENGTH),
	parsed_packet_size_(0) {
	resize(PACKET_LENGTH);
}

Packet::~Packet() {
	Release();
}

void Packet::Release() {
	for (unsigned x = 0; x < message_vector_.size(); ++x) {
		Message* _message = message_vector_[x];
		message_factory_->Release(_message);
	}
	Clear();
}

void Packet::Clear() {
	message_vector_.clear();

	packet_size_ = PACKET_SIZE_MARKER_LENGTH;
	parsed_packet_size_ = 0;
}

Packet::ParseResult Packet::ParseMore() {
	unsigned next_packet_offset = parsed_packet_size_;
	for (unsigned x = 0; x < message_vector_.size(); ++x) {
		Message* _message = message_vector_[x];
		message_factory_->Release(_message);
	}
	message_vector_.clear();
	return (Parse(next_packet_offset));
}

Packet::ParseResult Packet::Parse(unsigned offset) {
	if (offset == packet_size_) {
		return (kParseNoData);	// TRICKY: RAII simplifies code here.
	}

	const uint8* _data = GetReadBuffer();
	bool ok;
	{
		int this_packet_size;
		ok = ReadHeader(this_packet_size, &_data[offset], packet_size_-offset);
		if (ok) {
			this_packet_size += offset+PACKET_SIZE_MARKER_LENGTH;
			ok = ((unsigned)this_packet_size <= packet_size_);
			if (!ok && packet_size_ == PACKET_LENGTH && (unsigned)(this_packet_size-offset) <= PACKET_LENGTH) {
				// This means we cut a packet in half (only happens in TCP-backed sockets).
				::memmove(GetWriteBuffer(), &_data[offset], packet_size_-offset);
				parsed_packet_size_ = 0;
				packet_size_ -= offset;
				return kParseShift;
			}
		}
		if (ok) {
			parsed_packet_size_ = this_packet_size;
		}
	}
	unsigned x;
	for (x = offset+PACKET_SIZE_MARKER_LENGTH; ok && x < parsed_packet_size_;) {
		MessageType _type = (MessageType)_data[x];
		Message* _message = message_factory_->Allocate(_type);
		ok = (_message != 0);
		int message_size;
		if (ok) {
			message_size = _message->Parse(&_data[x], parsed_packet_size_-x);
			ok = (message_size > 0);
			x += message_size;
			//log_volatile(log_.Debugf("Received message type %i of size %i.", _type, message_size));
		}
		if (ok) {
			message_vector_.push_back(_message);
		} else if (_message) {
			message_factory_->Release(_message);
		}
	}
	if (x != parsed_packet_size_) {
		ok = false;
	}
	if (!ok) {
		log_.Errorf("Received a bad network packet (length=%u, offset=%u)!", packet_size_, offset);
		if (packet_size_ < 100) {
			log_.Errorf("  DATA: %s\n  STR:  %s",
				strutil::DumpData(_data, packet_size_).c_str(),
				strutil::ReplaceCtrlChars(str((const char*)_data, packet_size_), '.').c_str());
		}
	}
	return (ok? kParseOk : kParseError);
}

void Packet::StoreHeader(unsigned offset) {
	const int packet_data_size = packet_size_-PACKET_SIZE_MARKER_LENGTH;
	PackerInt16::Pack(&GetWriteBuffer()[offset], packet_data_size);
}

void Packet::AddMessage(Message* message) {
	message_vector_.push_back(message);
	message->SetStorage(GetWriteBuffer()+packet_size_);
	//log_volatile(log_.Debugf("Sending message type %i from offset %i.", message->GetType(), packet_size_));
}

bool Packet::AppendToPacketBuffer(Datagram& write_buffer) const {
	bool ok = false;
	// We only try appending if there is already a packet in place. Otherwise we will route the normal
	// way and will be copied by the socket class.
	if (write_buffer.data_size_ > 0) {
		const int this_data_length = GetPacketSize()-PACKET_SIZE_MARKER_LENGTH;
		const int total_new_length = write_buffer.data_size_+this_data_length;
		ok = (total_new_length <= Datagram::kBufferSize);
		if (ok) {
			::memcpy(&write_buffer.data_buffer_[write_buffer.data_size_], GetReadBuffer()+PACKET_SIZE_MARKER_LENGTH, this_data_length);
			write_buffer.data_size_ = total_new_length;
			PackerInt16::Pack(write_buffer.data_buffer_, total_new_length-PACKET_SIZE_MARKER_LENGTH);
		}
	}
	return (ok);
}

unsigned Packet::GetPacketSize() const {
	return (packet_size_);
}

void Packet::SetPacketSize(int _size) {
	deb_assert(_size > 0);
	packet_size_ = _size;
	deb_assert((int)packet_size_ <= PACKET_LENGTH);
}

void Packet::AddPacketSize(int _size) {
	deb_assert(_size > 0);
	packet_size_ += _size;
	deb_assert((int)packet_size_ <= PACKET_LENGTH);
}

int Packet::GetMessageCount() const {
	return ((int)message_vector_.size());
}

Message* Packet::GetMessageAt(int index) const {
	deb_assert((int)message_vector_.size() > index);
	return (message_vector_[index]);
}

const uint8* Packet::GetReadBuffer() const {
	return ((const uint8*)c_str());
}

uint8* Packet::GetWriteBuffer() const {
	return ((uint8*)c_str());
}

int Packet::GetBufferSize() const {
	return ((int)length());
}

int Packet::Receive(TcpSocket* socket, void* buffer, int max_size) {
	// Split up the TCP stream into packets that all fit into the buffer.

	uint8* _buffer = (uint8*)buffer;
	int current_offset = 0;
	const int read_quick_length = 4;
	deb_assert(read_quick_length >= PACKET_SIZE_MARKER_LENGTH);
	int receive_count = 0;
	bool ok = (max_size > current_offset+read_quick_length);
	if (ok) {
		int header_receive_count = socket->Receive(_buffer+current_offset, read_quick_length);
		ok = (header_receive_count == read_quick_length);
		if (ok) {
			receive_count += header_receive_count;
		}
	}
	while (ok) {
		int _packet_size = -1;
		if (ok) {
			ok = ReadHeader(_packet_size, _buffer+current_offset, read_quick_length);
		}
		if (ok) {
			ok = (_packet_size >= 1 && _packet_size+current_offset < (int)max_size-PACKET_SIZE_MARKER_LENGTH);
			if (!ok && current_offset > 0) {
				socket->Unreceive(_buffer+current_offset, read_quick_length);
				receive_count -= read_quick_length;
			}
		}
		if (ok) {
			const int content_size = _packet_size-read_quick_length+PACKET_SIZE_MARKER_LENGTH;
			int extra_header_size = content_size;
			if (current_offset+PACKET_SIZE_MARKER_LENGTH+_packet_size+read_quick_length < max_size) {
				// Try and read the NEXT header, if available.
				extra_header_size += read_quick_length;
			}
			int extra_receive_count = socket->Receive(_buffer+current_offset+read_quick_length, extra_header_size);
			if (extra_receive_count == content_size) {
				// No more header available or wanted, settle with what we've got.
				receive_count += extra_receive_count;
				break;
			} else if (extra_receive_count == extra_header_size) {
				// Found next header, take next packet onboard (if space available).
				receive_count += extra_receive_count;
				current_offset += PACKET_SIZE_MARKER_LENGTH+_packet_size;
			} else {
				ok = false;
			}
		}
	}
	return (receive_count);
}

bool Packet::ReadHeader(int& packet_size, const uint8* buffer, int byte_count) {
	bool ok = (PackerInt16::Unpack(packet_size, buffer, byte_count) == PACKET_SIZE_MARKER_LENGTH);
	if (ok) {
		ok = (packet_size > 0);
	}
	return (ok);
}

loginstance(kNetwork, Packet);



Message::Message():
	writable_data_(0),
	is_data_owner_(false) {
}

Message::~Message() {
	if (is_data_owner_) {
		delete[] (writable_data_);
	}
	writable_data_ = 0;
}

void Message::SetStorage(uint8* data) {
	writable_data_ = data;
}



MessageLoginRequest::MessageLoginRequest():
	password_data_(0) {
}

MessageType MessageLoginRequest::GetType() const {
	return (kMessageTypeLoginRequest);
}

int MessageLoginRequest::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeLoginRequest) {
		data_ = data;

		total_size = 1;
		int __size = -1;
		if (_size >= 1+4+22) {
			__size = PackerUnicodeString::UnpackRaw(0, &data_[total_size], _size-1-22);
			total_size += __size;
		}
		if (__size > 0) {
			__size = PackerOctetString::Unpack(0, &data_[total_size], 22);
			if (__size > 0) {
				password_data_ = &data_[total_size+2];
			}
			total_size += __size;
		}
		if (__size <= 0) {
			total_size = -1;
		}
	}
	return (total_size);
}

int MessageLoginRequest::Store(Packet* packet, const str& login_name, const MangledPassword& password) {
	writable_data_[0] = (uint8)GetType();
	unsigned __size = 1;
	__size += PackerUnicodeString::Pack(&writable_data_[__size], login_name);
	__size += PackerOctetString::Pack(&writable_data_[__size], (const uint8*)password.Get().c_str(), 20);
	packet->AddPacketSize(__size);
	return (__size);
}

void MessageLoginRequest::GetLoginName(str& login_name) {
	PackerUnicodeString::Unpack(login_name, &data_[1], 1024);
}

MangledPassword MessageLoginRequest::GetPassword() {
	std::string mangled_data((const char*)password_data_, 20);
	MangledPassword mangled_password;
	mangled_password.SetUnmodified(mangled_data);
	return (mangled_password);
}



MessageStatus::MessageStatus() {
}

MessageType MessageStatus::GetType() const {
	return (kMessageTypeStatus);
}

int MessageStatus::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeStatus) {
		data_ = data;

		total_size = 1+sizeof(uint32)*3;
		int __size = -1;
		if (_size >= (int)(1+sizeof(uint32)*3+4)) {
			__size = PackerUnicodeString::UnpackRaw(0, &data_[total_size], _size-total_size);
			total_size += __size;
		}
		if (__size <= 0) {
			total_size = -1;
		}
	}
	return (total_size);
}

int MessageStatus::Store(Packet* packet, RemoteStatus status, InfoType info_type, int32 integer, const str& message) {
	int32 _status = (int32)status;
	int32 _info_type = (int32)info_type;
	writable_data_[0] = (uint8)GetType();
	unsigned __size = 1;
	__size += PackerInt32::Pack(&writable_data_[__size], _status);
	__size += PackerInt32::Pack(&writable_data_[__size], _info_type);
	__size += PackerInt32::Pack(&writable_data_[__size], integer);
	__size += PackerUnicodeString::Pack(&writable_data_[__size], message);
	packet->AddPacketSize(__size);
	return (__size);
}

RemoteStatus MessageStatus::GetRemoteStatus() const {
	int32 _status = -1;
	PackerInt32::Unpack(_status, data_+1, 4);
	return ((RemoteStatus)_status);
}

MessageStatus::InfoType MessageStatus::GetInfo() const {
	int32 _info_type = -1;
	PackerInt32::Unpack(_info_type, data_+1+sizeof(int32), 4);
	return ((InfoType)_info_type);
}

int32 MessageStatus::GetInteger() const {
	int32 _integer = -1;
	PackerInt32::Unpack(_integer, data_+1+sizeof(int32)*2, 4);
	return (_integer);
}

void MessageStatus::GetMessageString(str& message) const {
	PackerUnicodeString::Unpack(message, &data_[1+sizeof(int32)*3], 1024);
}



MessageNumber::MessageNumber() {
}

MessageType MessageNumber::GetType() const {
	return (kMessageTypeNumber);
}

int MessageNumber::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeNumber) {
		data_ = data;

		total_size = 1+sizeof(int32)*2+sizeof(float32);
		if (_size < total_size) {
			total_size = -1;
		}
	}
	return (total_size);
}

int MessageNumber::Store(Packet* packet, InfoType info, int32 integer, float32 f) {
	int32 _info = info;

	writable_data_[0] = (uint8)GetType();
	unsigned __size = 1;
	__size += PackerInt32::Pack(&writable_data_[__size], _info);
	__size += PackerInt32::Pack(&writable_data_[__size], integer);
	__size += PackerReal::Pack(&writable_data_[__size], f);
	packet->AddPacketSize(__size);
	return (__size);
}

MessageNumber::InfoType MessageNumber::GetInfo() const {
	int32 _info = -1;
	PackerInt32::Unpack(_info, data_+1, 4);
	return ((InfoType)_info);
}

int32 MessageNumber::GetInteger() const {
	int32 _integer = -1;
	PackerInt32::Unpack(_integer, data_+1+sizeof(int32), 4);
	return (_integer);
}

float32 MessageNumber::GetFloat() const {
	float32 _f = -1;
	PackerReal::Unpack(_f, data_+1+sizeof(int32)*2, 4);
	return (_f);
}



MessageObject::MessageObject() {
}

int MessageObject::Parse(const uint8* data, int _size) {
	data_ = data;

	int total_size = 1+sizeof(uint32);
	if (_size < total_size) {
		total_size = -1;
	}
	return (total_size);
}

int MessageObject::Store(Packet*, uint32 instance_id) {
	writable_data_[0] = (uint8)GetType();
	unsigned __size = 1;
	__size += PackerInt32::Pack(&writable_data_[__size], instance_id);
	return (__size);
}

uint32 MessageObject::GetObjectId() const {
	int32 _instance_id;
	PackerInt32::Unpack(_instance_id, &data_[1], sizeof(_instance_id));
	return (_instance_id);
}



MessageCreateObject::MessageCreateObject() {
}

MessageType MessageCreateObject::GetType() const {
	return (kMessageTypeCreateObject);
}

int MessageCreateObject::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeCreateObject ||
		(MessageType)data[0] == kMessageTypeCreateOwnedObject) {
		total_size = Parent::Parse(data, _size);
		if (total_size > 0) {
			total_size += 7*sizeof(float);
			int __size = -1;
			if (_size >= total_size+4) {
				__size = PackerUnicodeString::UnpackRaw(0, &data_[total_size], _size-total_size);
				total_size += __size;
			}
			if (__size <= 0) {
				total_size = -1;
			}
		}
	}
	return (total_size);
}

int MessageCreateObject::Store(Packet* packet, GameObjectId instance_id, const xform& transformation, const str& class_id) {
	int32 _instance_id = (int32)instance_id;
	unsigned __size = Parent::Store(packet, _instance_id);
	__size += PackerTransformation::Pack(&writable_data_[__size], transformation);
	__size += PackerUnicodeString::Pack(&writable_data_[__size], class_id);
	packet->AddPacketSize(__size);
	return (__size);
}

void MessageCreateObject::GetTransformation(xform& transformation) const {
	PackerTransformation::Unpack(transformation, &data_[1+sizeof(int32)], 1024);
}

void MessageCreateObject::GetClassId(str& class_id) const {
	PackerUnicodeString::Unpack(class_id, &data_[1+sizeof(int32)+7*sizeof(float)], 1024);
}



MessageCreateOwnedObject::MessageCreateOwnedObject():
	owner_offset_(0) {
}

MessageType MessageCreateOwnedObject::GetType() const {
	return kMessageTypeCreateOwnedObject;
}

int MessageCreateOwnedObject::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeCreateOwnedObject) {
		total_size = Parent::Parse(data, _size);
		if (total_size > 0) {
			if (total_size < _size-(int)sizeof(uint32)) {
				owner_offset_ = total_size;
				total_size += sizeof(uint32);
			} else {
				total_size = -1;
			}
		}
	}
	return total_size;
}

int MessageCreateOwnedObject::Store(Packet* packet, GameObjectId instance_id, const xform& transformation, const str& class_id, GameObjectId owner_instance_id) {
	int32 _instance_id = (int32)instance_id;
	unsigned __size = Parent::Store(packet, _instance_id, transformation, class_id);
	__size += PackerInt32::Pack(&writable_data_[__size], owner_instance_id);
	packet->AddPacketSize(sizeof(int32));
	return __size;
}

uint32 MessageCreateOwnedObject::GetOwnerInstanceId() const {
	int32 _instance_id;
	PackerInt32::Unpack(_instance_id, &data_[owner_offset_], sizeof(_instance_id));
	return _instance_id;
}



MessageDeleteObject::MessageDeleteObject() {
}

MessageType MessageDeleteObject::GetType() const {
	return (kMessageTypeDeleteObject);
}

int MessageDeleteObject::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeDeleteObject) {
		total_size = Parent::Parse(data, _size);
	}
	return (total_size);
}

int MessageDeleteObject::Store(Packet* packet, GameObjectId instance_id) {
	unsigned __size = Parent::Store(packet, instance_id);
	packet->AddPacketSize(__size);
	return (__size);
}


MessageObjectMovement::MessageObjectMovement() {
}

int MessageObjectMovement::Parse(const uint8* data, int _size) {
	int total_size = Parent::Parse(data, _size);
	if (total_size > 0) {
		total_size += sizeof(int32);
		if (_size < total_size) {
			total_size = -1;
		}
	}
	return (total_size);
}

int MessageObjectMovement::Store(Packet* packet, uint32 instance_id, int32 frame_index) {
	int __size = Parent::Store(packet, instance_id);
	__size += PackerInt32::Pack(&writable_data_[__size], frame_index);
	return (__size);
}

int32 MessageObjectMovement::GetFrameIndex() const {
	int32 _frame_index;
	PackerInt32::Unpack(_frame_index, &data_[1+sizeof(uint32)], sizeof(_frame_index));
	return (_frame_index);
}



MessageObjectPosition::MessageObjectPosition() {
}

MessageType MessageObjectPosition::GetType() const {
	return (kMessageTypeObjectPosition);
}

int MessageObjectPosition::Parse(const uint8* data, int _size) {
	int total_size = -1;
	position_.Clear();
	if ((MessageType)data[0] == kMessageTypeObjectPosition) {
		total_size = Parent::Parse(data, _size);
		if (total_size > 0) {
			int position_size = position_.Unpack(&data[total_size], _size-total_size);
			if (position_size <= 0) {
				total_size = -1;
			} else {
				total_size += position_size;
				deb_assert(total_size <= _size);
			}
		}
	}
	return (total_size);
}

int MessageObjectPosition::Store(Packet* packet, uint32 instance_id, int32 frame_index, const ObjectPositionalData& data) {
	int __size = Parent::Store(packet, instance_id, frame_index);
	__size += data.Pack(&writable_data_[__size]);
	packet->AddPacketSize(__size);
	return (__size);
}

MessageObjectMovement* MessageObjectPosition::CloneToStandalone() {
	return (new MessageObjectPosition(position_, writable_data_, Parent::Parse(data_, 1024)));
}

ObjectPositionalData& MessageObjectPosition::GetPositionalData() {
	return (position_);
}

MessageObjectPosition::MessageObjectPosition(const ObjectPositionalData& position, uint8* data, int _size) {
	position_.CopyData(&position);

	is_data_owner_ = true;
	writable_data_ = new uint8[_size];
	::memcpy(writable_data_, data, _size);
}



MessageObjectAttach::MessageObjectAttach() {
}

MessageType MessageObjectAttach::GetType() const {
	return (kMessageTypeObjectAttach);
}

int MessageObjectAttach::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeObjectAttach) {
		total_size = Parent::Parse(data, _size);
		if (total_size > 0) {
			total_size += sizeof(uint32)+sizeof(uint16)*2;
			if (_size < total_size) {
				total_size = -1;
			}
		}
	}
	return (total_size);
}

int MessageObjectAttach::Store(Packet* packet, uint32 object1_id,
	uint32 object2_id, uint16 body1_id, uint16 body2_id) {
	int __size = Parent::Store(packet, object1_id);
	__size += PackerInt32::Pack(&writable_data_[__size], object2_id);
	__size += PackerInt16::Pack(&writable_data_[__size], body1_id);
	__size += PackerInt16::Pack(&writable_data_[__size], body2_id);
	packet->AddPacketSize(__size);
	return (__size);
}

uint32 MessageObjectAttach::GetObject2Id() const {
	int32 _object2_id;
	PackerInt32::Unpack(_object2_id, &data_[1+sizeof(uint32)], sizeof(_object2_id));
	return (_object2_id);
}

uint16 MessageObjectAttach::GetBody1Index() const {
	int _body1_id;
	PackerInt16::Unpack(_body1_id, &data_[1+sizeof(uint32)*2], sizeof(_body1_id));
	return ((uint16)_body1_id);
}

uint16 MessageObjectAttach::GetBody2Index() const {
	int _body2_id;
	PackerInt16::Unpack(_body2_id, &data_[1+sizeof(uint32)*2+sizeof(uint16)], sizeof(_body2_id));
	return ((int16)_body2_id);
}



MessageObjectDetach::MessageObjectDetach() {
}

MessageType MessageObjectDetach::GetType() const {
	return (kMessageTypeObjectDetach);
}

int MessageObjectDetach::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeObjectDetach) {
		total_size = Parent::Parse(data, _size);
		if (total_size > 0) {
			total_size += sizeof(uint32);
			if (_size < total_size) {
				total_size = -1;
			}
		}
	}
	return (total_size);
}

int MessageObjectDetach::Store(Packet* packet, uint32 object1_id, uint32 object2_id) {
	int __size = Parent::Store(packet, object1_id);
	__size += PackerInt32::Pack(&writable_data_[__size], object2_id);
	packet->AddPacketSize(__size);
	return (__size);
}

uint32 MessageObjectDetach::GetObject2Id() const {
	int32 _object2_id;
	PackerInt32::Unpack(_object2_id, &data_[1+sizeof(uint32)], sizeof(_object2_id));
	return (_object2_id);
}



MessageObjectAttribute::MessageObjectAttribute() {
}

MessageType MessageObjectAttribute::GetType() const {
	return (kMessageTypeObjectAttribute);
}

int MessageObjectAttribute::Parse(const uint8* data, int _size) {
	int total_size = -1;
	if ((MessageType)data[0] == kMessageTypeObjectAttribute) {
		total_size = Parent::Parse(data, _size);
		if (total_size > 0) {
			int byte_size;
			const int int16_unpack_size = PackerInt16::Unpack(byte_size, &data[total_size], _size);
			if (int16_unpack_size > 0) {
				total_size += int16_unpack_size + byte_size;
				if (_size < total_size) {
					total_size = -1;
				}
			}
		}
	}
	return (total_size);
}

uint8* MessageObjectAttribute::GetWriteBuffer(Packet* packet, uint32 instance_id, unsigned _size) {
	int _offset = Store(packet, instance_id);
	_offset += PackerInt16::Pack(&writable_data_[_offset], _size);
	packet->AddPacketSize(_offset+_size);
	return &writable_data_[_offset];
}

const uint8* MessageObjectAttribute::GetReadBuffer(unsigned& _size) const {
	PackerInt16::Unpack((int&)_size, &data_[1+sizeof(uint32)], 2);
	return &data_[1+sizeof(uint32)+sizeof(int16)];
}



PacketFactory::PacketFactory(MessageFactory* message_factory):
	message_factory_(message_factory) {
}

PacketFactory::~PacketFactory() {
	SetMessageFactory(0);
}

Packet* PacketFactory::Allocate() {
	// TODO: optimize when needed.
	return (new Packet(message_factory_));
}

void PacketFactory::Release(Packet* packet) {
	// TODO: optimize when needed.
	delete (packet);
}

void PacketFactory::SetMessageFactory(MessageFactory* message_factory) {
	delete (message_factory_);
	message_factory_ = message_factory;
}

MessageFactory* PacketFactory::GetMessageFactory() const {
	return (message_factory_);
}

/*int PacketFactory::Receive(TcpSocket* socket, void* buffer, int max_size) {
	return (Packet::Receive(socket, buffer, max_size));
}*/


Message* MessageFactory::Allocate(MessageType type) {
	// TODO: optimize when needed.
	Message* _message = 0;
	switch (type) {
		case kMessageTypeLoginRequest:	_message = new MessageLoginRequest();		break;
		case kMessageTypeStatus:		_message = new MessageStatus();			break;
		case kMessageTypeNumber:		_message = new MessageNumber();			break;
		case kMessageTypeCreateObject:	_message = new MessageCreateObject();		break;
		case kMessageTypeCreateOwnedObject:	_message = new MessageCreateOwnedObject();	break;
		case kMessageTypeDeleteObject:	_message = new MessageDeleteObject();		break;
		case kMessageTypeObjectPosition:	_message = new MessageObjectPosition();		break;
		case kMessageTypeObjectAttach:	_message = new MessageObjectAttach();		break;
		case kMessageTypeObjectDetach:	_message = new MessageObjectDetach();		break;
		case kMessageTypeObjectAttribute:	_message = new MessageObjectAttribute();	break;
	}
	if (!_message) {
		log_.Errorf("Got invalid network message of type %i!", type);
	}
	return (_message);
}

void MessageFactory::Release(Message* message) {
	// TODO: optimize when needed.
	delete (message);
}

loginstance(kNetwork, MessageFactory);



}
