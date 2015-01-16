
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include <string>
#include <vector>
#include "../../Lepra/Include/Datagram.h"
#include "Cure.h"
#include "PositionalData.h"
#include "RemoteStatus.h"
#include "UserAccount.h"



namespace Lepra
{
class TcpSocket;
}



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
		PARSE_SHIFT,
	};

	Packet(MessageFactory* pMessageFactory);
	virtual ~Packet();
	void Release();
	void Clear();

	ParseResult ParseMore();
	ParseResult Parse(unsigned pOffset = 0);	// Offset used when overriding packet, if a header is added.
	void StoreHeader(unsigned pOffset = 0);
	void AddMessage(Message* pMessage);
	bool AppendToPacketBuffer(Datagram& pWriteBuffer) const;
	unsigned GetPacketSize() const;
	void SetPacketSize(int pSize);
	void AddPacketSize(int pSize);
	int GetMessageCount() const;
	Message* GetMessageAt(int pIndex) const;
	const uint8* GetReadBuffer() const;
	uint8* GetWriteBuffer() const;
	int GetBufferSize() const;

	static int Receive(TcpSocket* pSocket, void* pBuffer, int pMaxSize);

protected:
	static bool ReadHeader(int& pThisPacketSize, const uint8* pBuffer, int pByteCount);

	static const int PACKET_SIZE_MARKER_LENGTH = 2;
	static const int PACKET_LENGTH = Datagram::BUFFER_SIZE;

	MessageFactory* mMessageFactory;
	std::vector<Message*> mMessageVector;
	unsigned mPacketSize;
	unsigned mParsedPacketSize;

private:
	logclass();
};



enum MessageType
{
	MESSAGE_TYPE_LOGIN_REQUEST = 2,
	MESSAGE_TYPE_STATUS,
	MESSAGE_TYPE_NUMBER,
	MESSAGE_TYPE_CREATE_OBJECT,
	MESSAGE_TYPE_CREATE_OWNED_OBJECT,
	MESSAGE_TYPE_DELETE_OBJECT,
	MESSAGE_TYPE_OBJECT_POSITION,
	MESSAGE_TYPE_OBJECT_ATTACH,
	MESSAGE_TYPE_OBJECT_DETACH,
	MESSAGE_TYPE_OBJECT_ATTRIBUTE,
};



class Message
{
public:
	Message();
	~Message();
	virtual MessageType GetType() const = 0;
	virtual int Parse(const uint8* pData, int pSize) = 0;
	void SetStorage(uint8* pData);

protected:
	union
	{
		const uint8* mData;
		uint8* mWritableData;
	};
	bool mIsDataOwner;
};

class MessageLoginRequest: public Message
{
public:
	MessageLoginRequest();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, const wstr& pLoginName, const MangledPassword& pPassword);

	void GetLoginName(wstr& pLoginName);
	MangledPassword GetPassword();

private:
	const uint8* mPasswordData;
};

class MessageStatus: public Message
{
public:
	enum InfoType
	{
		INFO_LOGIN = 1,
		INFO_CHAT,
		INFO_AVATAR,
		INFO_COMMAND,
	};

	MessageStatus();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, RemoteStatus pStatus, InfoType pInfoType, int32 pInteger, const wstr& pMessage);

	RemoteStatus GetRemoteStatus() const;
	InfoType GetInfo() const;
	int32 GetInteger() const;
	void GetMessageString(wstr& pMessage) const;
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
		INFO_RECREATE_OBJECT,
		INFO_REQUEST_LOAN,
		INFO_GRANT_LOAN,
		INFO_FALL_APART,
		INFO_APPLICATION_0 = 1000,
		INFO_APPLICATION_1,
		INFO_TOOL_0 = 2000,
		INFO_TOOL_1,
	};

	MessageNumber();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, InfoType pInfo, int32 pInteger, float32 pFloat);

	InfoType GetInfo() const;
	int32 GetInteger() const;
	float32 GetFloat() const;
};

class MessageObject: public Message
{
protected:
	MessageObject();
	int Parse(const uint8* pData, int pSize);

public:
	int Store(Packet* pPacket, uint32 pInstanceId);

	uint32 GetObjectId() const;
};

class MessageCreateObject: public MessageObject
{
public:
	typedef MessageObject Parent;

	MessageCreateObject();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId, const xform& pTransformation, const wstr& pClassId);

	void GetTransformation(xform& pTransformation) const;
	void GetClassId(wstr& pClassId) const;
};

class MessageCreateOwnedObject: public MessageCreateObject
{
public:
	typedef MessageCreateObject Parent;

	MessageCreateOwnedObject();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId, const xform& pTransformation, const wstr& pClassId, GameObjectId pOwnerInstanceId);

	uint32 GetOwnerInstanceId() const;

private:
	int mOwnerOffset;
};

class MessageDeleteObject: public MessageObject
{
public:
	typedef MessageObject Parent;

	MessageDeleteObject();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId);
};

class MessageObjectMovement: public MessageObject
{
	typedef MessageObject Parent;
protected:
	MessageObjectMovement();
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, GameObjectId pInstanceId, int32 pFrameIndex);

public:
	virtual MessageObjectMovement* CloneToStandalone() = 0;

	int32 GetFrameIndex() const;
};

class MessageObjectPosition: public MessageObjectMovement
{
	typedef MessageObjectMovement Parent;
public:
	MessageObjectPosition();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, uint32 pInstanceId, int32 pFrameIndex, const ObjectPositionalData& pData);
	MessageObjectMovement* CloneToStandalone();

	ObjectPositionalData& GetPositionalData();

private:
	MessageObjectPosition(const ObjectPositionalData& pPosition, uint8* pData, int pSize);

	ObjectPositionalData mPosition;
};

class MessageObjectAttach: public MessageObject
{
	typedef MessageObject Parent;
public:
	MessageObjectAttach();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, uint32 pObject1Id, uint32 pObject2Id, uint16 pBody1Index, uint16 pBody2Index);
	uint32 GetObject2Id() const;
	uint16 GetBody1Index() const;
	uint16 GetBody2Index() const;
};

class MessageObjectDetach: public MessageObject
{
	typedef MessageObject Parent;
public:
	MessageObjectDetach();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	int Store(Packet* pPacket, uint32 pObject1Id, uint32 pObject2Id);
	uint32 GetObject2Id() const;
};

class MessageObjectAttribute: public MessageObject
{
public:
	typedef MessageObject Parent;

	MessageObjectAttribute();
	MessageType GetType() const;
	int Parse(const uint8* pData, int pSize);
	uint8* GetWriteBuffer(Packet* pPacket, uint32 pInstanceId, unsigned pSize);
	const uint8* GetReadBuffer(unsigned& pSize) const;
};



class PacketFactory//: public DatagramReceiver
{
public:
	PacketFactory(MessageFactory* pMessageFactory);
	virtual ~PacketFactory();
	virtual Packet* Allocate();
	virtual void Release(Packet* pPacket);
	void SetMessageFactory(MessageFactory* pMessageFactory);
	MessageFactory* GetMessageFactory() const;

	//int Receive(TcpSocket* pSocket, void* pBuffer, int pMaxSize);

private:
	MessageFactory* mMessageFactory;
};

class MessageFactory
{
public:
	virtual Message* Allocate(MessageType pType);
	virtual void Release(Message* pMessage);

private:
	logclass();
};



}
