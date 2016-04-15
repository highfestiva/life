
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "ServerMessageProcessor.h"
#include "GameServerManager.h"



namespace Life
{



const float NETWORK_POSITIONAL_RESEND_INTERVAL = 0.6f;
const int NETWORK_POSITIONAL_AHEAD_BUFFER_SIZE = PHYSICS_FPS/2;



ServerMessageProcessor::ServerMessageProcessor(GameServerManager* pGameServerManager):
	mGameServerManager(pGameServerManager)
{
}

ServerMessageProcessor::~ServerMessageProcessor()
{
	mGameServerManager = 0;
}



void ServerMessageProcessor::ProcessNetworkInputMessage(Client* pClient, Cure::Message* pMessage)
{
	Cure::MessageType lType = pMessage->GetType();
	switch (lType)
	{
		case Cure::MESSAGE_TYPE_STATUS:
		{
			Cure::MessageStatus* lStatus = (Cure::MessageStatus*)pMessage;
			if (lStatus->GetRemoteStatus() == Cure::REMOTE_OK)
			{
				switch (lStatus->GetInfo())
				{
					case Cure::MessageStatus::INFO_CHAT:
					{
						log_debug("Chat...");
					}
					break;
					case Cure::MessageStatus::INFO_AVATAR:
					{
						log_debug("Avatar selected...");
						str lAvatarName;
						lStatus->GetMessageString(lAvatarName);
						const Cure::UserAccount::AvatarId lAvatarId = lAvatarName;
						mGameServerManager->OnSelectAvatar(pClient, lAvatarId);
					}
					break;
					default:
					{
						deb_assert(false);
					}
					break;
				}
			}
			else //if (lStatus->GetRemoteStatus() == Cure::REMOTE_NO_CONNECTION)
			{
				log_trace("User disconnects.");
				mGameServerManager->GetNetworkServer()->Disconnect(pClient->GetUserConnection()->GetAccountId(), "", false);
			}
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_POSITION:
		{
			Cure::MessageObjectPosition* lMovement = (Cure::MessageObjectPosition*)pMessage;
			// TODO: make sure client is authorized to control object with given ID.
			Cure::GameObjectId lInstanceId = lMovement->GetObjectId();
			Cure::ContextObject* lObject = mGameServerManager->GetContext()->GetObject(lInstanceId);
			if (lObject && (pClient->GetAvatarId() == lInstanceId || pClient->GetAvatarId() == lObject->GetBorrowerInstanceId()))
			{
				int32 lClientFrameIndex = lMovement->GetFrameIndex();
				mGameServerManager->AdjustClientSimulationSpeed(pClient, lClientFrameIndex);
				// Pass on to other clients.
				const Cure::ObjectPositionalData& lPosition = lMovement->GetPositionalData();
				mGameServerManager->BroadcastObjectPosition(lInstanceId, lPosition, pClient, false);
				mGameServerManager->StoreMovement(lClientFrameIndex, lMovement);
			}
			else
			{
				mLog.Warningf("Client %s tried to control instance ID %i, but was not in possession.",
					pClient->GetUserConnection()->GetLoginName().c_str(),
					lInstanceId);
			}
		}
		break;
		case Cure::MESSAGE_TYPE_NUMBER:
		{
			Cure::MessageNumber* lMessageNumber = (Cure::MessageNumber*)pMessage;
			ProcessNumber(pClient, lMessageNumber->GetInfo(), lMessageNumber->GetInteger(), lMessageNumber->GetFloat());
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_ATTRIBUTE:
		{
			Cure::MessageObjectAttribute* lMessageAttrib = (Cure::MessageObjectAttribute*)pMessage;
			Cure::GameObjectId lObjectId = lMessageAttrib->GetObjectId();
			unsigned lByteSize = 0;
			const uint8* lBuffer = lMessageAttrib->GetReadBuffer(lByteSize);
			mGameServerManager->GetContext()->UnpackObjectAttribute(lObjectId, lBuffer, lByteSize);
		}
		break;
		default:
		{
			mLog.Error("Got bad message type from client.");
		}
		break;
	}
}

void ServerMessageProcessor::ProcessNumber(Client* pClient, Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	(void)pFloat;

	switch (pType)
	{
		case Cure::MessageNumber::INFO_PING:
		{
			mGameServerManager->GetNetworkServer()->SendNumberMessage(false, pClient->GetUserConnection()->GetSocket(),
				Cure::MessageNumber::INFO_PONG, pInteger, pClient->GetPhysicsFrameAheadCount());
			log_volatile(mLog.Debugf("Replying to PING (PONG) to %s (ptr=%p).",
				pClient->GetUserConnection()->GetSocket()->GetTargetAddress().GetAsString().c_str(),
				pClient->GetUserConnection()->GetSocket()));
		}
		break;
		case Cure::MessageNumber::INFO_RECREATE_OBJECT:
		{
			const Cure::GameObjectId lInstanceId = pInteger;
			Cure::ContextObject* lObject = mGameServerManager->GetContext()->GetObject(lInstanceId);
			if (lObject)
			{
				ContextTable lTable;
				lTable.insert(ContextTable::value_type(lInstanceId, lObject));
				mGameServerManager->SendObjects(pClient, true, lTable);
				log_volatile(mLog.Debugf("Recreating %s (%i) on client.",
					lObject->GetClassId().c_str(),
					lInstanceId));
			}
			else
			{
				log_volatile(mLog.Debugf("User %s tried to fetch unknown object with ID %i.", pClient->GetUserConnection()->GetLoginName().c_str(), lInstanceId));
			}
		}
		break;
		case Cure::MessageNumber::INFO_REQUEST_LOAN:
		{
			const Cure::GameObjectId lInstanceId = pInteger;
			mGameServerManager->LoanObject(pClient, lInstanceId);
		}
		break;
		default:
		{
			mLog.Error("Received an invalid MessageNumber from client.");
		}
		break;
	}
}



loginstance(GAME, ServerMessageProcessor);



}
