
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Packet.h"
#include <assert.h>
#include "../../Lepra/Include/Packer.h"
#include "../Include/PositionalData.h"



namespace Cure
{



Packet::Packet(MessageFactory* pMessageFactory):
	mMessageFactory(pMessageFactory),
	mPacketSize(PACKET_SIZE_MARKER_LENGTH),
	mParsedPacketSize(0)
{
	resize(PACKET_LENGTH);
}

Packet::~Packet()
{
	Release();
}

void Packet::Release()
{
	for (unsigned x = 0; x < mMessageVector.size(); ++x)
	{
		Message* lMessage = mMessageVector[x];
		mMessageFactory->Release(lMessage);
	}
	Clear();
}

void Packet::Clear()
{
	mMessageVector.clear();

	mPacketSize = PACKET_SIZE_MARKER_LENGTH;
	mParsedPacketSize = 0;
}

Packet::ParseResult Packet::ParseMore()
{
	unsigned lNextPacketOffset = mParsedPacketSize;
	for (unsigned x = 0; x < mMessageVector.size(); ++x)
	{
		Message* lMessage = mMessageVector[x];
		mMessageFactory->Release(lMessage);
	}
	mMessageVector.clear();
	return (Parse(lNextPacketOffset));
}

Packet::ParseResult Packet::Parse(unsigned pOffset)
{
	if (pOffset == mPacketSize)
	{
		return (PARSE_NO_DATA);	// TRICKY: RAII simplifies code here.
	}

	const uint8* lData = GetReadBuffer();
	bool lOk;
	{
		int lThisPacketSize;
		lOk = ReadHeader(lThisPacketSize, &lData[pOffset], mPacketSize-pOffset);
		if (lOk)
		{
			lThisPacketSize += pOffset+PACKET_SIZE_MARKER_LENGTH;
			lOk = ((unsigned)lThisPacketSize <= mPacketSize);
			if (!lOk && mPacketSize == PACKET_LENGTH && (unsigned)(lThisPacketSize-pOffset) <= PACKET_LENGTH)
			{
				// This means we cut a packet in half (only happens in TCP-backed sockets).
				::memmove(GetWriteBuffer(), &lData[pOffset], mPacketSize-pOffset);
				mParsedPacketSize = 0;
				mPacketSize -= pOffset;
				return PARSE_SHIFT;
			}
		}
		if (lOk)
		{
			mParsedPacketSize = lThisPacketSize;
		}
	}
	unsigned x;
	for (x = pOffset+PACKET_SIZE_MARKER_LENGTH; lOk && x < mParsedPacketSize;)
	{
		MessageType lType = (MessageType)lData[x];
		Message* lMessage = mMessageFactory->Allocate(lType);
		lOk = (lMessage != 0);
		int lMessageSize;
		if (lOk)
		{
			lMessageSize = lMessage->Parse(&lData[x], mParsedPacketSize-x);
			lOk = (lMessageSize > 0);
			x += lMessageSize;
			//log_volatile(mLog.Debugf(_T("Received message type %i of size %i."), lType, lMessageSize));
		}
		if (lOk)
		{
			mMessageVector.push_back(lMessage);
		}
		else if (lMessage)
		{
			mMessageFactory->Release(lMessage);
		}
	}
	if (x != mParsedPacketSize)
	{
		lOk = false;
	}
	if (!lOk)
	{
		mLog.Errorf(_T("Received a bad network packet (length=%u, offset=%u)!"), mPacketSize, pOffset);
		if (mPacketSize < 100)
		{
			mLog.Errorf(_T("  DATA: %s\n  STR:  %s"),
				strutil::DumpData(lData, mPacketSize).c_str(),
				strutil::Encode(astrutil::ReplaceCtrlChars(astr((const char*)lData, mPacketSize), '.')).c_str());
		}
	}
	return (lOk? PARSE_OK : PARSE_ERROR);
}

void Packet::StoreHeader(unsigned pOffset)
{
	const int lPacketDataSize = mPacketSize-PACKET_SIZE_MARKER_LENGTH;
	PackerInt16::Pack(&GetWriteBuffer()[pOffset], lPacketDataSize);
}

void Packet::AddMessage(Message* pMessage)
{
	mMessageVector.push_back(pMessage);
	pMessage->SetStorage(GetWriteBuffer()+mPacketSize);
	//log_volatile(mLog.Debugf(_T("Sending message type %i from offset %i."), pMessage->GetType(), mPacketSize));
}

bool Packet::AppendToPacketBuffer(Datagram& pWriteBuffer) const
{
	bool lOk = false;
	// We only try appending if there is already a packet in place. Otherwise we will route the normal
	// way and will be copied by the socket class.
	if (pWriteBuffer.mDataSize > 0)
	{
		const int lThisDataLength = GetPacketSize()-PACKET_SIZE_MARKER_LENGTH;
		const int lTotalNewLength = pWriteBuffer.mDataSize+lThisDataLength;
		lOk = (lTotalNewLength <= SocketBase::BUFFER_SIZE);
		if (lOk)
		{
			::memcpy(&pWriteBuffer.mDataBuffer[pWriteBuffer.mDataSize], GetReadBuffer()+PACKET_SIZE_MARKER_LENGTH, lThisDataLength);
			pWriteBuffer.mDataSize = lTotalNewLength;
			PackerInt16::Pack(pWriteBuffer.mDataBuffer, lTotalNewLength-PACKET_SIZE_MARKER_LENGTH);
		}
	}
	return (lOk);
}

unsigned Packet::GetPacketSize() const
{
	return (mPacketSize);
}

void Packet::SetPacketSize(int pSize)
{
	assert(pSize > 0);
	mPacketSize = pSize;
	assert((int)mPacketSize <= PACKET_LENGTH);
}

void Packet::AddPacketSize(int pSize)
{
	assert(pSize > 0);
	mPacketSize += pSize;
	assert((int)mPacketSize <= PACKET_LENGTH);
}

int Packet::GetMessageCount() const
{
	return ((int)mMessageVector.size());
}

Message* Packet::GetMessageAt(int pIndex) const
{
	assert((int)mMessageVector.size() > pIndex);
	return (mMessageVector[pIndex]);
}

const uint8* Packet::GetReadBuffer() const
{
	return ((const uint8*)c_str());
}

uint8* Packet::GetWriteBuffer() const
{
	return ((uint8*)c_str());
}

int Packet::GetBufferSize() const
{
	return ((int)length());
}

int Packet::Receive(TcpSocket* pSocket, void* pBuffer, int pMaxSize)
{
	// Split up the TCP stream into packets that all fit into the buffer.

	uint8* lBuffer = (uint8*)pBuffer;
	int lCurrentOffset = 0;
	const int lReadQuickLength = 4;
	assert(lReadQuickLength >= PACKET_SIZE_MARKER_LENGTH);
	int lReceiveCount = 0;
	bool lOk = (pMaxSize > lCurrentOffset+lReadQuickLength);
	if (lOk)
	{
		int lHeaderReceiveCount = pSocket->Receive(lBuffer+lCurrentOffset, lReadQuickLength);
		lOk = (lHeaderReceiveCount == lReadQuickLength);
		if (lOk)
		{
			lReceiveCount += lHeaderReceiveCount;
		}
	}
	while (lOk)
	{
		int lPacketSize = -1;
		if (lOk)
		{
			lOk = ReadHeader(lPacketSize, lBuffer+lCurrentOffset, lReadQuickLength);
		}
		if (lOk)
		{
			lOk = (lPacketSize >= 1 && lPacketSize+lCurrentOffset < (int)pMaxSize-PACKET_SIZE_MARKER_LENGTH);
			if (!lOk && lCurrentOffset > 0)
			{
				pSocket->Unreceive(lBuffer+lCurrentOffset, lReadQuickLength);
				lReceiveCount -= lReadQuickLength;
			}
		}
		if (lOk)
		{
			const int lContentSize = lPacketSize-lReadQuickLength+PACKET_SIZE_MARKER_LENGTH;
			int lExtraHeaderSize = lContentSize;
			if (lCurrentOffset+PACKET_SIZE_MARKER_LENGTH+lPacketSize+lReadQuickLength < pMaxSize)
			{
				// Try and read the NEXT header, if available.
				lExtraHeaderSize += lReadQuickLength;
			}
			int lExtraReceiveCount = pSocket->Receive(lBuffer+lCurrentOffset+lReadQuickLength, lExtraHeaderSize);
			if (lExtraReceiveCount == lContentSize)
			{
				// No more header available or wanted, settle with what we've got.
				lReceiveCount += lExtraReceiveCount;
				break;
			}
			else if (lExtraReceiveCount == lExtraHeaderSize)
			{
				// Found next header, take next packet onboard (if space available).
				lReceiveCount += lExtraReceiveCount;
				lCurrentOffset += PACKET_SIZE_MARKER_LENGTH+lPacketSize;
			}
			else
			{
				lOk = false;
			}
		}
	}
	return (lReceiveCount);
}

bool Packet::ReadHeader(int& pPacketSize, const uint8* pBuffer, int pByteCount)
{
	bool lOk = (PackerInt16::Unpack(pPacketSize, pBuffer, pByteCount) == PACKET_SIZE_MARKER_LENGTH);
	if (lOk)
	{
		lOk = (pPacketSize > 0);
	}
	return (lOk);
}

LOG_CLASS_DEFINE(NETWORK, Packet);



Message::Message():
	mWritableData(0),
	mIsDataOwner(false)
{
}

Message::~Message()
{
	if (mIsDataOwner)
	{
		delete[] (mWritableData);
	}
	mWritableData = 0;
}

void Message::SetStorage(uint8* pData)
{
	mWritableData = pData;
}



MessageLoginRequest::MessageLoginRequest():
	mPasswordData(0)
{
}

MessageType MessageLoginRequest::GetType() const
{
	return (MESSAGE_TYPE_LOGIN_REQUEST);
}

int MessageLoginRequest::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_LOGIN_REQUEST)
	{
		mData = pData;

		lTotalSize = 1;
		int lSize = -1;
		if (pSize >= 1+4+22)
		{
			lSize = PackerUnicodeString::UnpackRaw(0, &mData[lTotalSize], pSize-1-22);
			lTotalSize += lSize;
		}
		if (lSize > 0)
		{
			lSize = PackerOctetString::Unpack(0, &mData[lTotalSize], 22);
			if (lSize > 0)
			{
				mPasswordData = &mData[lTotalSize+2];
			}
			lTotalSize += lSize;
		}
		if (lSize <= 0)
		{
			lTotalSize = -1;
		}
	}
	return (lTotalSize);
}

int MessageLoginRequest::Store(Packet* pPacket, const wstr& pLoginName, const MangledPassword& pPassword)
{
	mWritableData[0] = (uint8)GetType();
	unsigned lSize = 1;
	lSize += PackerUnicodeString::Pack(&mWritableData[lSize], pLoginName);
	lSize += PackerOctetString::Pack(&mWritableData[lSize], (const uint8*)pPassword.Get().c_str(), 20);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

void MessageLoginRequest::GetLoginName(wstr& pLoginName)
{
	PackerUnicodeString::Unpack(pLoginName, &mData[1], 1024);
}

MangledPassword MessageLoginRequest::GetPassword()
{
	std::string lMangledData((const char*)mPasswordData, 20);
	MangledPassword lMangledPassword;
	lMangledPassword.SetUnmodified(lMangledData);
	return (lMangledPassword);
}



MessageStatus::MessageStatus()
{
}

MessageType MessageStatus::GetType() const
{
	return (MESSAGE_TYPE_STATUS);
}

int MessageStatus::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_STATUS)
	{
		mData = pData;

		lTotalSize = 1+sizeof(uint32)*3;
		int lSize = -1;
		if (pSize >= (int)(1+sizeof(uint32)*3+4))
		{
			lSize = PackerUnicodeString::UnpackRaw(0, &mData[lTotalSize], pSize-lTotalSize);
			lTotalSize += lSize;
		}
		if (lSize <= 0)
		{
			lTotalSize = -1;
		}
	}
	return (lTotalSize);
}

int MessageStatus::Store(Packet* pPacket, RemoteStatus pStatus, InfoType pInfoType, int32 pInteger, const wstr& pMessage)
{
	int32 lStatus = (int32)pStatus;
	int32 lInfoType = (int32)pInfoType;
	mWritableData[0] = (uint8)GetType();
	unsigned lSize = 1;
	lSize += PackerInt32::Pack(&mWritableData[lSize], lStatus);
	lSize += PackerInt32::Pack(&mWritableData[lSize], lInfoType);
	lSize += PackerInt32::Pack(&mWritableData[lSize], pInteger);
	lSize += PackerUnicodeString::Pack(&mWritableData[lSize], pMessage);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

RemoteStatus MessageStatus::GetRemoteStatus() const
{
	int32 lStatus = -1;
	PackerInt32::Unpack(lStatus, mData+1, 4);
	return ((RemoteStatus)lStatus);
}

MessageStatus::InfoType MessageStatus::GetInfo() const
{
	int32 lInfoType = -1;
	PackerInt32::Unpack(lInfoType, mData+1+sizeof(int32), 4);
	return ((InfoType)lInfoType);
}

int32 MessageStatus::GetInteger() const
{
	int32 lInteger = -1;
	PackerInt32::Unpack(lInteger, mData+1+sizeof(int32)*2, 4);
	return (lInteger);
}

void MessageStatus::GetMessageString(wstr& pMessage) const
{
	PackerUnicodeString::Unpack(pMessage, &mData[1+sizeof(int32)*3], 1024);
}



MessageNumber::MessageNumber()
{
}

MessageType MessageNumber::GetType() const
{
	return (MESSAGE_TYPE_NUMBER);
}

int MessageNumber::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_NUMBER)
	{
		mData = pData;

		lTotalSize = 1+sizeof(int32)*2+sizeof(float32);
		if (pSize < lTotalSize)
		{
			lTotalSize = -1;
		}
	}
	return (lTotalSize);
}

int MessageNumber::Store(Packet* pPacket, InfoType pInfo, int32 pInteger, float32 pFloat)
{
	int32 lInfo = pInfo;

	mWritableData[0] = (uint8)GetType();
	unsigned lSize = 1;
	lSize += PackerInt32::Pack(&mWritableData[lSize], lInfo);
	lSize += PackerInt32::Pack(&mWritableData[lSize], pInteger);
	lSize += PackerReal::Pack(&mWritableData[lSize], pFloat);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

MessageNumber::InfoType MessageNumber::GetInfo() const
{
	int32 lInfo = -1;
	PackerInt32::Unpack(lInfo, mData+1, 4);
	return ((InfoType)lInfo);
}

int32 MessageNumber::GetInteger() const
{
	int32 lInteger = -1;
	PackerInt32::Unpack(lInteger, mData+1+sizeof(int32), 4);
	return (lInteger);
}

float32 MessageNumber::GetFloat() const
{
	float32 lFloat = -1;
	PackerReal::Unpack(lFloat, mData+1+sizeof(int32)*2, 4);
	return (lFloat);
}



MessageObject::MessageObject()
{
}

int MessageObject::Parse(const uint8* pData, int pSize)
{
	mData = pData;

	int lTotalSize = 1+sizeof(uint32);
	if (pSize < lTotalSize)
	{
		lTotalSize = -1;
	}
	return (lTotalSize);
}

int MessageObject::Store(Packet*, uint32 pInstanceId)
{
	mWritableData[0] = (uint8)GetType();
	unsigned lSize = 1;
	lSize += PackerInt32::Pack(&mWritableData[lSize], pInstanceId);
	return (lSize);
}

uint32 MessageObject::GetObjectId() const
{
	int32 lInstanceId;
	PackerInt32::Unpack(lInstanceId, &mData[1], sizeof(lInstanceId));
	return (lInstanceId);
}



MessageCreateObject::MessageCreateObject()
{
}

MessageType MessageCreateObject::GetType() const
{
	return (MESSAGE_TYPE_CREATE_OBJECT);
}

int MessageCreateObject::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_CREATE_OBJECT)
	{
		lTotalSize = Parent::Parse(pData, pSize);
		if (lTotalSize > 0)
		{
			lTotalSize += 7*sizeof(float);
			int lSize = -1;
			if (pSize >= lTotalSize+4)
			{
				lSize = PackerUnicodeString::UnpackRaw(0, &mData[lTotalSize], pSize-lTotalSize);
				lTotalSize += lSize;
			}
			if (lSize <= 0)
			{
				lTotalSize = -1;
			}
		}
	}
	return (lTotalSize);
}

int MessageCreateObject::Store(Packet* pPacket, GameObjectId pInstanceId, const TransformationF& pTransformation, const wstr& pClassId)
{
	int32 lInstanceId = (int32)pInstanceId;
	unsigned lSize = Parent::Store(pPacket, lInstanceId);
	lSize += PackerTransformation::Pack(&mWritableData[lSize], pTransformation);
	lSize += PackerUnicodeString::Pack(&mWritableData[lSize], pClassId);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

void MessageCreateObject::GetTransformation(TransformationF& pTransformation) const
{
	PackerTransformation::Unpack(pTransformation, &mData[1+sizeof(int32)], 1024);
}

void MessageCreateObject::GetClassId(wstr& pClassId) const
{
	PackerUnicodeString::Unpack(pClassId, &mData[1+sizeof(int32)+7*sizeof(float)], 1024);
}



MessageDeleteObject::MessageDeleteObject()
{
}

MessageType MessageDeleteObject::GetType() const
{
	return (MESSAGE_TYPE_DELETE_OBJECT);
}

int MessageDeleteObject::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_DELETE_OBJECT)
	{
		lTotalSize = Parent::Parse(pData, pSize);
	}
	return (lTotalSize);
}

int MessageDeleteObject::Store(Packet* pPacket, GameObjectId pInstanceId)
{
	unsigned lSize = Parent::Store(pPacket, pInstanceId);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}


MessageObjectMovement::MessageObjectMovement()
{
}

int MessageObjectMovement::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = Parent::Parse(pData, pSize);
	if (lTotalSize > 0)
	{
		lTotalSize += sizeof(int32);
		if (pSize < lTotalSize)
		{
			lTotalSize = -1;
		}
	}
	return (lTotalSize);
}

int MessageObjectMovement::Store(Packet* pPacket, uint32 pInstanceId, int32 pFrameIndex)
{
	int lSize = Parent::Store(pPacket, pInstanceId);
	lSize += PackerInt32::Pack(&mWritableData[lSize], pFrameIndex);
	return (lSize);
}

int32 MessageObjectMovement::GetFrameIndex() const
{
	int32 lFrameIndex;
	PackerInt32::Unpack(lFrameIndex, &mData[1+sizeof(uint32)], sizeof(lFrameIndex));
	return (lFrameIndex);
}



MessageObjectPosition::MessageObjectPosition()
{
}

MessageType MessageObjectPosition::GetType() const
{
	return (MESSAGE_TYPE_OBJECT_POSITION);
}

int MessageObjectPosition::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	mPosition.Clear();
	if ((MessageType)pData[0] == MESSAGE_TYPE_OBJECT_POSITION)
	{
		lTotalSize = Parent::Parse(pData, pSize);
		if (lTotalSize > 0)
		{
			int lPositionSize = mPosition.Unpack(&pData[lTotalSize], pSize-lTotalSize);
			if (lPositionSize <= 0)
			{
				lTotalSize = -1;
			}
			else
			{
				lTotalSize += lPositionSize;
				assert(lTotalSize <= pSize);
			}
		}
	}
	return (lTotalSize);
}

int MessageObjectPosition::Store(Packet* pPacket, uint32 pInstanceId, int32 pFrameIndex, const ObjectPositionalData& pData)
{
	int lSize = Parent::Store(pPacket, pInstanceId, pFrameIndex);
	lSize += pData.Pack(&mWritableData[lSize]);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

MessageObjectMovement* MessageObjectPosition::CloneToStandalone()
{
	return (new MessageObjectPosition(mPosition, mWritableData, Parent::Parse(mData, 1024)));
}

ObjectPositionalData& MessageObjectPosition::GetPositionalData()
{
	return (mPosition);
}

MessageObjectPosition::MessageObjectPosition(const ObjectPositionalData& pPosition, uint8* pData, int pSize)
{
	mPosition.CopyData(&pPosition);

	mIsDataOwner = true;
	mWritableData = new uint8[pSize];
	::memcpy(mWritableData, pData, pSize);
}



MessageObjectAttach::MessageObjectAttach()
{
}

MessageType MessageObjectAttach::GetType() const
{
	return (MESSAGE_TYPE_OBJECT_ATTACH);
}

int MessageObjectAttach::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_OBJECT_ATTACH)
	{
		lTotalSize = Parent::Parse(pData, pSize);
		if (lTotalSize > 0)
		{
			lTotalSize += sizeof(uint32)+sizeof(uint16)*2;
			if (pSize < lTotalSize)
			{
				lTotalSize = -1;
			}
		}
	}
	return (lTotalSize);
}

int MessageObjectAttach::Store(Packet* pPacket, uint32 pObject1Id,
	uint32 pObject2Id, uint16 pBody1Id, uint16 pBody2Id)
{
	int lSize = Parent::Store(pPacket, pObject1Id);
	lSize += PackerInt32::Pack(&mWritableData[lSize], pObject2Id);
	lSize += PackerInt16::Pack(&mWritableData[lSize], pBody1Id);
	lSize += PackerInt16::Pack(&mWritableData[lSize], pBody2Id);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

uint32 MessageObjectAttach::GetObject2Id() const
{
	int32 lObject2Id;
	PackerInt32::Unpack(lObject2Id, &mData[1+sizeof(uint32)], sizeof(lObject2Id));
	return (lObject2Id);
}

uint16 MessageObjectAttach::GetBody1Id() const
{
	int lBody1Id;
	PackerInt16::Unpack(lBody1Id, &mData[1+sizeof(uint32)*2], sizeof(lBody1Id));
	return ((uint16)lBody1Id);
}

uint16 MessageObjectAttach::GetBody2Id() const
{
	int lBody2Id;
	PackerInt16::Unpack(lBody2Id, &mData[1+sizeof(uint32)*2+sizeof(uint16)], sizeof(lBody2Id));
	return ((int16)lBody2Id);
}



MessageObjectDetach::MessageObjectDetach()
{
}

MessageType MessageObjectDetach::GetType() const
{
	return (MESSAGE_TYPE_OBJECT_DETACH);
}

int MessageObjectDetach::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_OBJECT_DETACH)
	{
		lTotalSize = Parent::Parse(pData, pSize);
		if (lTotalSize > 0)
		{
			lTotalSize += sizeof(uint32);
			if (pSize < lTotalSize)
			{
				lTotalSize = -1;
			}
		}
	}
	return (lTotalSize);
}

int MessageObjectDetach::Store(Packet* pPacket, uint32 pObject1Id, uint32 pObject2Id)
{
	int lSize = Parent::Store(pPacket, pObject1Id);
	lSize += PackerInt32::Pack(&mWritableData[lSize], pObject2Id);
	pPacket->AddPacketSize(lSize);
	return (lSize);
}

uint32 MessageObjectDetach::GetObject2Id() const
{
	int32 lObject2Id;
	PackerInt32::Unpack(lObject2Id, &mData[1+sizeof(uint32)], sizeof(lObject2Id));
	return (lObject2Id);
}



MessageObjectAttribute::MessageObjectAttribute()
{
}

MessageType MessageObjectAttribute::GetType() const
{
	return (MESSAGE_TYPE_OBJECT_ATTRIBUTE);
}

int MessageObjectAttribute::Parse(const uint8* pData, int pSize)
{
	int lTotalSize = -1;
	if ((MessageType)pData[0] == MESSAGE_TYPE_OBJECT_ATTRIBUTE)
	{
		lTotalSize = Parent::Parse(pData, pSize);
		if (lTotalSize > 0)
		{
			int lByteSize;
			const int lInt16UnpackSize = PackerInt16::Unpack(lByteSize, &pData[lTotalSize], pSize);
			if (lInt16UnpackSize > 0)
			{
				lTotalSize += lInt16UnpackSize + lByteSize;
				if (pSize < lTotalSize)
				{
					lTotalSize = -1;
				}
			}
		}
	}
	return (lTotalSize);
}

uint8* MessageObjectAttribute::GetWriteBuffer(Packet* pPacket, uint32 pInstanceId, unsigned pSize)
{
	int lOffset = Store(pPacket, pInstanceId);
	lOffset += PackerInt16::Pack(&mWritableData[lOffset], pSize);
	pPacket->AddPacketSize(lOffset+pSize);
	return &mWritableData[lOffset];
}

const uint8* MessageObjectAttribute::GetReadBuffer(unsigned& pSize) const
{
	PackerInt16::Unpack((int&)pSize, &mData[1+sizeof(uint32)], 2);
	return &mData[1+sizeof(uint32)+sizeof(int16)];
}



PacketFactory::PacketFactory(MessageFactory* pMessageFactory):
	mMessageFactory(pMessageFactory)
{
}

PacketFactory::~PacketFactory()
{
	SetMessageFactory(0);
}

Packet* PacketFactory::Allocate()
{
	// TODO: optimize when needed.
	return (new Packet(mMessageFactory));
}

void PacketFactory::Release(Packet* pPacket)
{
	// TODO: optimize when needed.
	delete (pPacket);
}

void PacketFactory::SetMessageFactory(MessageFactory* pMessageFactory)
{
	delete (mMessageFactory);
	mMessageFactory = pMessageFactory;
}

MessageFactory* PacketFactory::GetMessageFactory() const
{
	return (mMessageFactory);
}

int PacketFactory::Receive(TcpSocket* pSocket, void* pBuffer, int pMaxSize)
{
	return (Packet::Receive(pSocket, pBuffer, pMaxSize));
}


Message* MessageFactory::Allocate(MessageType pType)
{
	// TODO: optimize when needed.
	Message* lMessage = 0;
	switch (pType)
	{
		case MESSAGE_TYPE_LOGIN_REQUEST:	lMessage = new MessageLoginRequest();		break;
		case MESSAGE_TYPE_STATUS:		lMessage = new MessageStatus();			break;
		case MESSAGE_TYPE_NUMBER:		lMessage = new MessageNumber();			break;
		case MESSAGE_TYPE_CREATE_OBJECT:	lMessage = new MessageCreateObject();		break;
		case MESSAGE_TYPE_DELETE_OBJECT:	lMessage = new MessageDeleteObject();		break;
		case MESSAGE_TYPE_OBJECT_POSITION:	lMessage = new MessageObjectPosition();		break;
		case MESSAGE_TYPE_OBJECT_ATTACH:	lMessage = new MessageObjectAttach();		break;
		case MESSAGE_TYPE_OBJECT_DETACH:	lMessage = new MessageObjectDetach();		break;
		case MESSAGE_TYPE_OBJECT_ATTRIBUTE:	lMessage = new MessageObjectAttribute();	break;
	}
	if (!lMessage)
	{
		mLog.Errorf(_T("Got invalid network message of type %i!"), pType);
	}
	return (lMessage);
}

void MessageFactory::Release(Message* pMessage)
{
	// TODO: optimize when needed.
	delete (pMessage);
}

LOG_CLASS_DEFINE(NETWORK, MessageFactory);



}
