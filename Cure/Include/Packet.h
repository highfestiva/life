
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <string>
#include <vector>
#include "../../lepra/include/datagram.h"
#include "cure.h"
#include "positionaldata.h"
#include "remotestatus.h"
#include "useraccount.h"



namespace lepra {
class TcpSocket;
}



namespace cure {



class ObjectPositionalData;
class Message;
class MessageFactory;



class Packet: protected std::string {
public:
	enum ParseResult {
		kParseOk,
		kParseNoData,
		kParseError,
		kParseShift,
	};

	Packet(MessageFactory* message_factory);
	virtual ~Packet();
	void Release();
	void Clear();

	ParseResult ParseMore();
	ParseResult Parse(unsigned offset = 0);	// Offset used when overriding packet, if a header is added.
	void StoreHeader(unsigned offset = 0);
	void AddMessage(Message* message);
	bool AppendToPacketBuffer(Datagram& write_buffer) const;
	unsigned GetPacketSize() const;
	void SetPacketSize(int size);
	void AddPacketSize(int size);
	int GetMessageCount() const;
	Message* GetMessageAt(int index) const;
	const uint8* GetReadBuffer() const;
	uint8* GetWriteBuffer() const;
	int GetBufferSize() const;

	static int Receive(TcpSocket* socket, void* buffer, int max_size);

protected:
	static bool ReadHeader(int& this_packet_size, const uint8* buffer, int byte_count);

	static const int PACKET_SIZE_MARKER_LENGTH = 2;
	static const int PACKET_LENGTH = Datagram::kBufferSize;

	MessageFactory* message_factory_;
	std::vector<Message*> message_vector_;
	unsigned packet_size_;
	unsigned parsed_packet_size_;

private:
	logclass();
};



enum MessageType {
	kMessageTypeLoginRequest = 2,
	kMessageTypeStatus,
	kMessageTypeNumber,
	kMessageTypeCreateObject,
	kMessageTypeCreateOwnedObject,
	kMessageTypeDeleteObject,
	kMessageTypeObjectPosition,
	kMessageTypeObjectAttach,
	kMessageTypeObjectDetach,
	kMessageTypeObjectAttribute,
};



class Message {
public:
	Message();
	~Message();
	virtual MessageType GetType() const = 0;
	virtual int Parse(const uint8* data, int size) = 0;
	void SetStorage(uint8* data);

protected:
	union {
		const uint8* data_;
		uint8* writable_data_;
	};
	bool is_data_owner_;
};

class MessageLoginRequest: public Message {
public:
	MessageLoginRequest();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, const str& login_name, const MangledPassword& password);

	void GetLoginName(str& login_name);
	MangledPassword GetPassword();

private:
	const uint8* password_data_;
};

class MessageStatus: public Message {
public:
	enum InfoType {
		kInfoLogin = 1,
		kInfoChat,
		kInfoAvatar,
		kInfoCommand,
	};

	MessageStatus();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, RemoteStatus status, InfoType info_type, int32 integer, const str& message);

	RemoteStatus GetRemoteStatus() const;
	InfoType GetInfo() const;
	int32 GetInteger() const;
	void GetMessageString(str& message) const;
};

class MessageNumber: public Message {
public:
	enum InfoType {
		kInfoSetTime	= 1,
		kInfoAdjustTime,
		kInfoPing,
		kInfoPong,
		kInfoAvatar,
		kInfoRecreateObject,
		kInfoRequestLoan,
		kInfoGrantLoan,
		kInfoFallApart,
		kInfoApplication0 = 1000,
		kInfoApplication1,
		kInfoTool0 = 2000,
		kInfoTool1,
	};

	MessageNumber();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, InfoType info, int32 integer, float32 f);

	InfoType GetInfo() const;
	int32 GetInteger() const;
	float32 GetFloat() const;
};

class MessageObject: public Message {
protected:
	MessageObject();
	int Parse(const uint8* data, int size);

public:
	int Store(Packet* packet, uint32 instance_id);

	uint32 GetObjectId() const;
};

class MessageCreateObject: public MessageObject {
public:
	typedef MessageObject Parent;

	MessageCreateObject();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, GameObjectId instance_id, const xform& transformation, const str& class_id);

	void GetTransformation(xform& transformation) const;
	void GetClassId(str& class_id) const;
};

class MessageCreateOwnedObject: public MessageCreateObject {
public:
	typedef MessageCreateObject Parent;

	MessageCreateOwnedObject();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, GameObjectId instance_id, const xform& transformation, const str& class_id, GameObjectId owner_instance_id);

	uint32 GetOwnerInstanceId() const;

private:
	int owner_offset_;
};

class MessageDeleteObject: public MessageObject {
public:
	typedef MessageObject Parent;

	MessageDeleteObject();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, GameObjectId instance_id);
};

class MessageObjectMovement: public MessageObject {
	typedef MessageObject Parent;
protected:
	MessageObjectMovement();
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, GameObjectId instance_id, int32 frame_index);

public:
	virtual MessageObjectMovement* CloneToStandalone() = 0;

	int32 GetFrameIndex() const;
};

class MessageObjectPosition: public MessageObjectMovement {
	typedef MessageObjectMovement Parent;
public:
	MessageObjectPosition();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, uint32 instance_id, int32 frame_index, const ObjectPositionalData& data);
	MessageObjectMovement* CloneToStandalone();

	ObjectPositionalData& GetPositionalData();

private:
	MessageObjectPosition(const ObjectPositionalData& position, uint8* data, int size);

	ObjectPositionalData position_;
};

class MessageObjectAttach: public MessageObject {
	typedef MessageObject Parent;
public:
	MessageObjectAttach();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, uint32 object1_id, uint32 object2_id, uint16 body1_index, uint16 body2_index);
	uint32 GetObject2Id() const;
	uint16 GetBody1Index() const;
	uint16 GetBody2Index() const;
};

class MessageObjectDetach: public MessageObject {
	typedef MessageObject Parent;
public:
	MessageObjectDetach();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	int Store(Packet* packet, uint32 object1_id, uint32 object2_id);
	uint32 GetObject2Id() const;
};

class MessageObjectAttribute: public MessageObject {
public:
	typedef MessageObject Parent;

	MessageObjectAttribute();
	MessageType GetType() const;
	int Parse(const uint8* data, int size);
	uint8* GetWriteBuffer(Packet* packet, uint32 instance_id, unsigned size);
	const uint8* GetReadBuffer(unsigned& size) const;
};



class PacketFactory {//: public DatagramReceiver
public:
	PacketFactory(MessageFactory* message_factory);
	virtual ~PacketFactory();
	virtual Packet* Allocate();
	virtual void Release(Packet* packet);
	void SetMessageFactory(MessageFactory* message_factory);
	MessageFactory* GetMessageFactory() const;

	//int Receive(TcpSocket* socket, void* buffer, int max_size);

private:
	MessageFactory* message_factory_;
};

class MessageFactory {
public:
	virtual Message* Allocate(MessageType type);
	virtual void Release(Message* message);

private:
	logclass();
};



}
