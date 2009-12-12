
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include <string>
#include <vector>
#include "../../Lepra/Include/Socket.h"
#include "Cure.h"
#include "PositionalData.h"
#include "RemoteStatus.h"
#include "UserAccount.h"



namespace Cure
{



class ObjectPositionalData;
class Message;
class MessageFactory;



class Packet: protected std::string
{
public:
	enum ParseResult
	{
		PARSE_OK,
		PARSE_NO_DATA,
		PARSE_ERROR,
	};

	Packet(MessageFactory* pMessageFactory);
	virtual ~Packet();
	void Release();
	void Clear();

	ParseResult ParseMore();
	ParseResult Parse(unsigned pOffset = 0);	// Offset used when overriding packet, if a header is added.
	void StoreHeader(unsigned pOffset = 0);
	void AddMessage(Message* pMessage);
	bool AppendToPacketBuffer(Lepra::Datagram& pWriteBuffer) const;
	unsigned GetPacketSize() const;
	void SetPacketSize(int pSize);
	void AddPacketSize(int pSize);
	int GetMessageCount() const;
	Message* GetMessageAt(int pIndex) const;
	const Lepra::uint8* GetReadBuffer() const;
	Lepra::uint8* GetWriteBuffer() const;
	int GetBufferSize() const;

	static int Receive(Lepra::TcpSocket* pSocket, void* pBuffer, int pMaxSize);

protected:
	static bool ReadHeader(int& pThisPacketSize, const Lepra::uint8* pBuffer, int pByteCount);

	static const int PACKET_SIZE_MARKER_LENGTH = 2;
	static const int PACKET_LENGTH = Lepra::SocketBase::BUFFER_SIZE;

	MessageFactory* mMessageFactory;
	std::vector<Message*> mMessageVector;
	unsigned mPacketSize;
	unsigned mParsedPacketSize;

private:
	LOG_CLASS_DECLARE();
};



enum MessageType
{
	MESSAGE_TYPE_LOGIN_REQUEST = 2,
	MESSAGE_TYPE_STATUS,
	MESSAGE_TYPE_NUMBER,
	MESSAGE_TYPE_CREATE_OBJECT,
	MESSAGE_TYPE_DELETE_OBJECT,
	MESSAGE_TYPE_OBJECT_POSITION,
	MESSAGE_TYPE_OBJECT_ATTACH,
	MESSAGE_TYPE_OBJECT_DETACH,
};



class Message
{
public:
	Message();
	~Message();
	virtual MessageType GetType() const = 0;
	virtual int Parse(const Lepra::uint8* pData, int pSize) = 0;
	void SetStorage(Lepra::uint8* pData);

protected:
	union
	{
		const Lepra::uint8* mData;
		Lepra::uint8* mWritableData;
	};
	bool mIsDataOwner;
};

class MessageLoginRequest: public Message
{
public:
	MessageLoginRequest();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, const Lepra::UnicodeString& pLoginName, const MangledPassword& pPassword);

	void GetLoginName(Lepra::UnicodeString& pLoginName);
	MangledPassword GetPassword();

private:
	const Lepra::uint8* mPasswordData;
};

class MessageStatus: public Message
{
public:
	MessageStatus();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, RemoteStatus pStatus, Lepra::int32 pInteger, const Lepra::UnicodeString& pMessage);

	RemoteStatus GetRemoteStatus() const;
	Lepra::int32 GetInteger() const;
	void GetMessageString(Lepra::UnicodeString& pMessage) const;
};

class MessageNumber: public Message
{
public:
	enum InfoType
	{
		INFO_SET_TIME	= 1,
		INFO_ADJUST_TIME,
		INFO_PING,
		INFO_PONG,
		INFO_AVATAR,
	};

	MessageNumber();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, InfoType pInfo, Lepra::int32 pInteger, Lepra::float32 pFloat);

	InfoType GetInfo() const;
	Lepra::int32 GetInteger() const;
	Lepra::float32 GetFloat() const;
};

class MessageObject: public Message
{
protected:
	MessageObject();
	int Parse(const Lepra::uint8* pData, int pSize);

public:
	int Store(Packet* pPacket, Lepra::uint32 pInstanceId);

	Lepra::uint32 GetObjectId() const;
};

class MessageCreateObject: public MessageObject
{
public:
	typedef MessageObject Parent;

	MessageCreateObject();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId, const Lepra::UnicodeString& pClassId);

	void GetClassId(Lepra::UnicodeString& pClassId) const;
};

class MessageDeleteObject: public MessageObject
{
public:
	typedef MessageObject Parent;

	MessageDeleteObject();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId);
};

class MessageObjectMovement: public MessageObject
{
	typedef MessageObject Parent;
protected:
	MessageObjectMovement();
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId, Lepra::int32 pFrameIndex);

public:
	virtual MessageObjectMovement* CloneToStandalone() = 0;

	Lepra::int32 GetFrameIndex() const;
};

class MessageObjectPosition: public MessageObjectMovement
{
	typedef MessageObjectMovement Parent;
public:
	MessageObjectPosition();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, Lepra::uint32 pInstanceId, Lepra::int32 pFrameIndex, const ObjectPositionalData& pData);
	MessageObjectMovement* CloneToStandalone();

	ObjectPositionalData& GetPositionalData();

private:
	MessageObjectPosition(const ObjectPositionalData& pPosition, Lepra::uint8* pData, int pSize);

	ObjectPositionalData mPosition;
};

class MessageObjectAttach: public MessageObject
{
	typedef MessageObject Parent;
public:
	MessageObjectAttach();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, Lepra::uint32 pObject1Id, Lepra::uint32 pObject2Id, Lepra::uint16 pBody1Id, Lepra::uint16 pBody2Id);
	Lepra::uint32 GetObject2Id() const;
	Lepra::uint16 GetBody1Id() const;
	Lepra::uint16 GetBody2Id() const;
};

class MessageObjectDetach: public MessageObject
{
	typedef MessageObject Parent;
public:
	MessageObjectDetach();
	MessageType GetType() const;
	int Parse(const Lepra::uint8* pData, int pSize);
	int Store(Packet* pPacket, Lepra::uint32 pObject1Id, Lepra::uint32 pObject2Id);
	Lepra::uint32 GetObject2Id() const;
};



class PacketFactory: public Lepra::DatagramReceiver
{
public:
	PacketFactory(MessageFactory* pMessageFactory);
	virtual ~PacketFactory();
	virtual Packet* Allocate();
	virtual void Release(Packet* pPacket);
	void SetMessageFactory(MessageFactory* pMessageFactory);
	MessageFactory* GetMessageFactory() const;

	int Receive(Lepra::TcpSocket* pSocket, void* pBuffer, int pMaxSize);

private:
	MessageFactory* mMessageFactory;
};

class MessageFactory
{
public:
	virtual Message* Allocate(MessageType pType);
	virtual void Release(Message* pMessage);

private:
	LOG_CLASS_DECLARE();
};



}
