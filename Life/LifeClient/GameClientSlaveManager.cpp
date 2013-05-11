
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientSlaveManager.h"
#include <algorithm>
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/ContextObjectAttribute.h"
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/StringUtility.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiCure/Include/UiDebugRenderer.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../LifeServer/MasterServerConnection.h"
#include "../LifeApplication.h"
#include "ClientConsoleManager.h"
#include "GameClientMasterTicker.h"
#include "RtVar.h"
#include "UiConsole.h"
#include "UiGameServerManager.h"



namespace Life
{



GameClientSlaveManager::GameClientSlaveManager(GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Cure::GameManager(pTime, pVariableScope, pResourceManager),
	mUiManager(pUiManager),
	mSlaveIndex(pSlaveIndex),
	mRenderArea(pRenderArea),
	mMasterServerConnection(0),
	mIsReset(false),
	mIsResetComplete(false),
	mQuit(false),
	mLastSentByteCount(0),
	mPingAttemptCount(0),
	mAllowMovementInput(true),
	mOptions(pVariableScope, pSlaveIndex)
{
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_CTRL_MOUSESENSITIVITY, 4.0f);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_CTRL_MOUSEFILTER, 0.5f);
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);

	SetTicker(pMaster);

	SetNetworkAgent(new Cure::NetworkClient(GetVariableScope()));
}

GameClientSlaveManager::~GameClientSlaveManager()
{
	Close();

	SetMasterServerConnection(0);
	SetTicker(0);

	GetConsoleManager()->Join();
}

void GameClientSlaveManager::SetMasterServerConnection(MasterServerConnection* pConnection)
{
	delete mMasterServerConnection;
	mMasterServerConnection = pConnection;
}

void GameClientSlaveManager::LoadSettings()
{
	str lExternalServerAddress;
	CURE_RTVAR_GET(lExternalServerAddress, =, UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
	GetConsoleManager()->ExecuteCommand(_T("alias gfx-lo \"#") _T(RTVAR_UI_3D_PIXELSHADERS) _T(" false; #") _T(RTVAR_UI_3D_SHADOWS) _T(" No; #") _T(RTVAR_UI_3D_ENABLEMASSOBJECTS) _T(" false; #") _T(RTVAR_UI_3D_ENABLEMASSOBJECTFADING) _T(" false; #") _T(RTVAR_UI_3D_ENABLEPARTICLES) _T(" false\""));
	GetConsoleManager()->ExecuteCommand(_T("alias gfx-hi \"#") _T(RTVAR_UI_3D_PIXELSHADERS) _T(" true; #") _T(RTVAR_UI_3D_SHADOWS) _T(" Force:Volumes; #") _T(RTVAR_UI_3D_ENABLEMASSOBJECTS) _T(" true; #") _T(RTVAR_UI_3D_ENABLEMASSOBJECTFADING) _T(" true; #") _T(RTVAR_UI_3D_ENABLEPARTICLES) _T(" true\""));
	GetConsoleManager()->ExecuteCommand(_T("execute-file -i ")+GetApplicationCommandFilename());
	mOptions.DoRefreshConfiguration();
	// Always default these settings, to avoid that the user can't get rid of undesired behavior.
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_INPUT_PRINT, false);
	bool lIsServerSelected;
	CURE_RTVAR_TRYGET(lIsServerSelected, =, UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, false);
	if (lIsServerSelected)
	{
		CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, lExternalServerAddress);
	}
	else
	{
		str lServerAddress;
		CURE_RTVAR_GET(lServerAddress, =, UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
		if (Network::IsLocalAddress(lServerAddress))
		{
			bool lIsOpenServer;
			CURE_RTVAR_GET(lIsOpenServer, =, GetVariableScope(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
			const bool lIsCurrentlyLocalhost = (strutil::StartsWith(lServerAddress, _T("localhost:")) || strutil::StartsWith(lServerAddress, _T("127.0.0.1:")));
			if (lIsOpenServer)
			{
				if (lIsCurrentlyLocalhost)
				{
					CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("0.0.0.0:16650"));
				}
			}
			else
			{
				if (!lIsCurrentlyLocalhost)
				{
					CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
				}
			}
		}
	}
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_PHYSICS_FPS, PHYSICS_FPS);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
}

void GameClientSlaveManager::SetRenderArea(const PixelRect& pRenderArea)
{
	mRenderArea = pRenderArea;
	((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->SetRenderArea(pRenderArea);
}

bool GameClientSlaveManager::Open()
{
	Close();
	ScopeLock lLock(GetTickLock());
	bool lOk = Reset();
	if (lOk)
	{
		lOk = GetConsoleManager()->Start();
	}
	return (lOk);
}

void GameClientSlaveManager::Close()
{
	ScopeLock lLock(GetTickLock());
	// Drop all physics and renderer objects.
	GetContext()->ClearObjects();
	GetConsoleManager()->Join();
}

bool GameClientSlaveManager::IsQuitting() const
{
	return (mQuit);
}

void GameClientSlaveManager::SetIsQuitting()
{
	mLog.Headlinef(_T("Slave %i will quit."), GetSlaveIndex());
	if (Thread::GetCurrentThread()->GetThreadName() == "MainThread")
	{
		GetResourceManager()->Tick();
	}
	mQuit = true;
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, false);
}



GameClientMasterTicker* GameClientSlaveManager::GetMaster() const
{
	return (GameClientMasterTicker*)GetTicker();
}



bool GameClientSlaveManager::Render()
{
	ScopeLock lLock(GetTickLock());

	UpdateCameraPosition(true);

	float lFov;
	CURE_RTVAR_GET(lFov, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
	UpdateFrustum(lFov);

	LEPRA_MEASURE_SCOPE(SlaveRender);
	bool lOutline;
	bool lWireFrame;
	bool lPixelShaders;
	CURE_RTVAR_GET(lOutline, =, GetVariableScope(), RTVAR_UI_3D_OUTLINEMODE, false);
	CURE_RTVAR_GET(lWireFrame, =, GetVariableScope(), RTVAR_UI_3D_WIREFRAMEMODE, false);
	CURE_RTVAR_GET(lPixelShaders, =, GetVariableScope(), RTVAR_UI_3D_PIXELSHADERS, false);
	SetLocalRender(true);
	mUiManager->GetRenderer()->EnableOutlineRendering(lOutline);
	mUiManager->GetRenderer()->EnableWireframe(lWireFrame);
	mUiManager->GetRenderer()->EnablePixelShaders(lPixelShaders);
	mUiManager->Render(mRenderArea);
	SetLocalRender(false);	// Hide sun and mass objects from other cameras.

	return (true);
}

bool GameClientSlaveManager::Paint()
{
	return true;
}

bool GameClientSlaveManager::IsPrimaryManager() const
{
	if (GetMaster()->IsLocalServer())
	{
		return false;
	}
	return GetMaster()->IsFirstSlave(this);
}

void GameClientSlaveManager::PreEndTick()
{
	Parent::PreEndTick();

	HandleUnusedRelativeAxis();
}

bool GameClientSlaveManager::EndTick()
{
	bool lIsDebugDrawing = mUiManager->CanRender();
	CURE_RTVAR_GET(lIsDebugDrawing, &=, GetVariableScope(), RTVAR_DEBUG_ENABLE, false);

	if (lIsDebugDrawing)
	{
		DrawAsyncDebugInfo();
	}

	bool lOk = Parent::EndTick();
	if (lOk)
	{
		if (lIsDebugDrawing)
		{
			DrawSyncDebugInfo();
		}
		TickUiUpdate();
	}
	return (lOk);
}

void GameClientSlaveManager::TickNetworkInput()
{
	if (GetNetworkClient()->GetSocket() == 0)
	{
		if (!GetNetworkClient()->IsConnecting() && !mIsReset)
		{
			Reset();
		}
		return;	// TRICKY: easy way out.
	}

	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	Cure::NetworkAgent::ReceiveStatus lReceived = GetNetworkClient()->ReceiveNonBlocking(lPacket);
	switch (lReceived)
	{
		case Cure::NetworkAgent::RECEIVE_OK:
		{
			//log_volatile(mLog.Debugf(_T("%s received data from server."), GetName().c_str()));
			Cure::Packet::ParseResult lParseResult;
			// Walk packets.
			do
			{
				// Walk messages.
				const int lMessageCount = lPacket->GetMessageCount();
				for (int x = 0; x < lMessageCount; ++x)
				{
					Cure::Message* lMessage = lPacket->GetMessageAt(x);
					//log_volatile(mLog.Tracef(_T("Received message of type %i."), lMessage->GetType()));
					ProcessNetworkInputMessage(lMessage);
				}
				lParseResult = lPacket->ParseMore();
				if (lParseResult == Cure::Packet::PARSE_SHIFT)
				{
					lReceived = GetNetworkClient()->ReceiveMore(lPacket);
					if (lReceived == Cure::NetworkAgent::RECEIVE_OK)
					{
						lParseResult = lPacket->ParseMore();
					}
					else
					{
						lParseResult = Cure::Packet::PARSE_ERROR;
					}
				}
			}
			while (lParseResult == Cure::Packet::PARSE_OK);
			if (lParseResult == Cure::Packet::PARSE_NO_DATA)
			{
				mLastUnsafeReceiveTime.ClearTimeDiff();
			}
			else
			{
				mLog.AError("Problem with receiving crap extra packet!");
			}
		}
		break;
		case Cure::NetworkAgent::RECEIVE_PARSE_ERROR:
		{
			mLog.AError("Problem with receiving crap data!");
		}
		break;
		case Cure::NetworkAgent::RECEIVE_CONNECTION_BROKEN:
		{
			mDisconnectReason = _T("Server abrubtly disconnected!");
			mLog.AError("Disconnected from server!");
			mIsReset = false;
			GetNetworkClient()->Disconnect(false);
		}
		break;
		case Cure::NetworkAgent::RECEIVE_NO_DATA:
		{
			// Nothing, really.
		}
		break;
	}
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

bool GameClientSlaveManager::TickNetworkOutput()
{
	bool lSendOk = true;
	bool lIsSent = false;
	if (GetNetworkClient()->GetSocket())
	{
		// Check if we should send client keepalive (keepalive is simply a position update).
		bool lForceSendUnsafeClientKeepalive = false;
		mLastSendTime.UpdateTimer();
		double lKeepaliveInterval;
		CURE_RTVAR_GET(lKeepaliveInterval, =, GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_SENDINTERVAL, 1.0);
		if (mLastSentByteCount != GetNetworkAgent()->GetSentByteCount())
		{
			mLastSentByteCount = GetNetworkAgent()->GetSentByteCount();
			mLastSendTime.ClearTimeDiff();
		}
		else if (mLastSendTime.GetTimeDiff() >= lKeepaliveInterval)
		{
			lForceSendUnsafeClientKeepalive = true;
		}

		// Check if we should send updates. Send all owned objects at the same time to avoid penetration.
		float lPosSendIntervalLimit;
		CURE_RTVAR_GET(lPosSendIntervalLimit, =(float), GetVariableScope(), RTVAR_NETPHYS_POSSENDINTERVALLIMIT, 0.5);
		lPosSendIntervalLimit *= 0.5f;	// Sampling theorem.
		bool lSend = false;
		ObjectIdSet::iterator x = mOwnedObjectList.begin();
		for (; x != mOwnedObjectList.end(); ++x)
		{
			Cure::ContextObject* lObject = GetContext()->GetObject(*x);
			if (lObject)
			{
				lObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
				if (!lObject->QueryResendTime(lPosSendIntervalLimit, false))
				{
					continue;
				}
				const Cure::ObjectPositionalData* lPositionalData = 0;
				if (lObject->UpdateFullPosition(lPositionalData))
				{
					if (!lPositionalData->IsSameStructure(*lObject->GetNetworkOutputGhost()))
					{
						lObject->GetNetworkOutputGhost()->CopyData(lPositionalData);
					}
					const bool lIsCollisionExpired = mCollisionExpireAlarm.PopExpired(0.6);
					const bool lIsInputExpired = mInputExpireAlarm.PopExpired(0.0);
					const bool lIsPositionExpired = (lIsCollisionExpired || lIsInputExpired);
					if (lIsPositionExpired)
					{
						log_atrace("Position expires.");
					}
					const bool lIsAllwedDiffSend = mSendExpireAlarm.IsExpired(0.5);

					float lResyncOnDiff;
					CURE_RTVAR_GET(lResyncOnDiff, =(float), GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 0.2);
					if (lForceSendUnsafeClientKeepalive ||
						lIsPositionExpired ||
						(lIsAllwedDiffSend &&
						lPositionalData->GetScaledDifference(lObject->GetNetworkOutputGhost()) > lResyncOnDiff))
					{
						//if (lForceSendUnsafeClientKeepalive)	mLog.AInfo("POS_SEND: Force pos send!");
						//if (lIsPositionExpired)			mLog.AInfo("POS_SEND: Pos expired causing pos send!");
						//if (lIsAllwedDiffSend)			mLog.AInfo("POS_SEND: Diff causing pos send!");
						mSendExpireAlarm.Set();
						lSend = true;
						break;
					}
				}
			}
		}
		if (lSend)
		{
			ObjectIdSet::iterator x = mOwnedObjectList.begin();
			for (; x != mOwnedObjectList.end(); ++x)
			{
				Cure::ContextObject* lObject = GetContext()->GetObject(*x);
				if (lObject)
				{
					const Cure::ObjectPositionalData* lPositionalData = 0;
					if (lObject->UpdateFullPosition(lPositionalData))
					{
						lObject->GetNetworkOutputGhost()->CopyData(lPositionalData);
						lSendOk = GetNetworkAgent()->SendObjectFullPosition(GetNetworkClient()->GetSocket(),
							lObject->GetInstanceId(), GetTimeManager()->GetCurrentPhysicsFrame(), *lObject->GetNetworkOutputGhost());
						lIsSent = true;

						CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_DEBUG_NET_SENDPOSCNT, int, +, 1, 0, 1000000);

						/*for (int x = 0; x < lObject->GetPhysics()->GetEngineCount(); ++x)
						{
							TBC::PhysicsEngine* lEngine = lObject->GetPhysics()->GetEngine(x);
							log_volatile(mLog.Debugf(_T("Sync'ed engine of type %i with value %f."), lEngine->GetEngineType(), lEngine->GetValue()));
						}*/
					}
				}
			}
		}

		// Check if we should send server check-up (uses message similar to ping).
		if (lSendOk && !GetNetworkClient()->IsLoggingIn())
		{
			mLastUnsafeReceiveTime.UpdateTimer();
			double lPingInterval;
			CURE_RTVAR_GET(lPingInterval, =, GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_PINGINTERVAL, 7.0);
			if ((!lIsSent && lForceSendUnsafeClientKeepalive) ||
				mLastUnsafeReceiveTime.GetTimeDiff() >= lPingInterval)
			{
				int lPingRetryCount;
				CURE_RTVAR_GET(lPingRetryCount, =, GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_PINGRETRYCOUNT, 4);
				if (++mPingAttemptCount <= lPingRetryCount)
				{
					mLastUnsafeReceiveTime.ReduceTimeDiff(lPingInterval);
					log_volatile(mLog.Debugf(_T("Slave %i sending ping."), mSlaveIndex));
					lSendOk = GetNetworkAgent()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
						Cure::MessageNumber::INFO_PING, GetTimeManager()->GetCurrentPhysicsFrame(), 0);
				}
				else
				{
					mDisconnectReason = _T("Server not responding!");
					mLog.AError("Server is not responding to ping! Disconnecting now!");
					GetNetworkClient()->Disconnect(true);
				}
			}
		}
	}
	if (lSendOk)
	{
		lSendOk = Parent::TickNetworkOutput();
	}

	if (mMasterServerConnection)
	{
		float lConnectTimeout;
		CURE_RTVAR_GET(lConnectTimeout, =(float), GetVariableScope(), RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0);
		mMasterServerConnection->SetSocketInfo(lSendOk? GetNetworkClient() : 0, lConnectTimeout);
		mMasterServerConnection->Tick();
	}

	// If we were unable to send to server, we conclude that it has silently died.
	if (!lSendOk)
	{
		mDisconnectReason = _T("Connection to server died!");
		mLog.AError("Server seems dead! Disconnecting silently.");
		GetNetworkClient()->Disconnect(false);
	}
	return (lSendOk);
}

void GameClientSlaveManager::TickNetworkOutputGhosts()
{
	const int lStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	const float lPhysicsFrameTime = GetTimeManager()->GetAffordedPhysicsStepTime();
	ObjectIdSet::iterator x = mOwnedObjectList.begin();
	for (; x != mOwnedObjectList.end(); ++x)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(*x);
		if (lObject)
		{
			lObject->GetNetworkOutputGhost()->GhostStep(lStepCount, lPhysicsFrameTime);
		}
	}
}



void GameClientSlaveManager::ToggleConsole()
{
	mAllowMovementInput = !((ClientConsoleManager*)GetConsoleManager())->ToggleVisible();
}



void GameClientSlaveManager::RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken)
{
	GetMaster()->PreLogin(pServerAddress);

	ScopeLock lLock(GetTickLock());

	mIsReset = false;

	str lPortRange = CURE_RTVAR_SLOW_GET(GetVariableScope(), RTVAR_NETWORK_CONNECT_LOCALPORTRANGE, _T("1025-65535"));
	str lLocalName;
	if (strutil::StartsWith(pServerAddress, _T("localhost:")) || strutil::StartsWith(pServerAddress, _T("127.0.0.1:")))
	{
		lLocalName = _T("localhost");
	}
	str lLocalAddress = lLocalName + _T(':') + lPortRange;
	if (!GetNetworkClient()->Open(lLocalAddress))
	{
		mDisconnectReason = _T("Could not use local sockets.");
		return;
	}

	float lConnectTimeout;
	CURE_RTVAR_GET(lConnectTimeout, =(float), GetVariableScope(), RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0);
	str lServerAddress = pServerAddress;

	// Open firewall path from server.
	if (mMasterServerConnection)
	{
		mMasterServerConnection->SetSocketInfo(GetNetworkClient(), lConnectTimeout);
		bool lIsLocalAddress = false;
		{
			SocketAddress lResolvedAddress;
			if (lResolvedAddress.Resolve(lServerAddress))
			{
				str lIp = lResolvedAddress.GetIP().GetAsString();
				if (lIp == _T("127.0.0.1") || lIp == _T("::1") || lIp == _T("0:0:0:0:0:0:0:1"))
				{
					lIsLocalAddress = true;
				}
			}
		}
		if (!GetMaster()->IsLocalServer() && !lIsLocalAddress)
		{
			mMasterServerConnection->RequestOpenFirewall(pServerAddress);
			const double lTimeout = 1.2;
			const double lWaitTime = mMasterServerConnection->WaitUntilDone(lTimeout, true);
			if (lWaitTime < lTimeout)
			{
				if (mMasterServerConnection->GetFirewallOpenStatus() == MasterServerConnection::FIREWALL_OPENED)
				{
					mLog.AInfo("Master seems to have asked game server to open firewall OK.");
					Thread::Sleep(lWaitTime+0.02);	// Let's pray. Since we got through in a certain time, perhaps the server will too.
				}
				else if (mMasterServerConnection->GetFirewallOpenStatus() == MasterServerConnection::FIREWALL_USE_LAN)
				{
					mLog.AInfo("Master seems to think the game server is on our LAN. Using that instead.");
					lServerAddress = mMasterServerConnection->GetLanServerConnectAddress();
				}
			}
			else
			{
				mLog.AWarning("Master did not reply in time to if it asked game server to open firewall. Trying anyway.");
			}
		}
	}

	mConnectUserName = strutil::Encode(pLoginToken.GetName());
	mConnectServerAddress = pServerAddress;
	mDisconnectReason = _T("Connect failed.");
	GetNetworkClient()->StartConnectLogin(lServerAddress, lConnectTimeout, pLoginToken);
}

void GameClientSlaveManager::Logout()
{
	if (GetNetworkClient()->IsActive())
	{
		mDisconnectReason = _T("User requested logout.");
		mLog.Warning(mDisconnectReason);
		mIsResetComplete = false;
		mIsReset = false;
		GetNetworkClient()->Disconnect(true);
		for (int x = 0; !mIsResetComplete && x < 10; ++x)
		{
			Thread::Sleep(0.05);
		}
	}
}

bool GameClientSlaveManager::IsLoggingIn() const
{
	return (GetNetworkClient()->IsConnecting() || GetNetworkClient()->IsLoggingIn());
}

bool GameClientSlaveManager::IsUiMoveForbidden(Cure::GameObjectId pObjectId) const
{
	const bool lMoveAllowed = (IsOwned(pObjectId) || GetContext()->IsLocalGameObjectId(pObjectId) ||
		(!GetMaster()->IsLocalObject(pObjectId) && GetMaster()->IsFirstSlave(this)));
	return !lMoveAllowed;
}

void GameClientSlaveManager::AddLocalObjects(std::hash_set<Cure::GameObjectId>& pLocalObjectSet)
{
	pLocalObjectSet.insert(mOwnedObjectList.begin(), mOwnedObjectList.end());
}

bool GameClientSlaveManager::IsOwned(Cure::GameObjectId pObjectId) const
{
	return (mOwnedObjectList.find(pObjectId) != mOwnedObjectList.end());
}



bool GameClientSlaveManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.RefreshConfiguration();

	mOptions.Options::OptionsManager::UpdateInput(pKeyCode, true);
	if (mOptions.IsToggleConsole())
	{
		mOptions.ResetToggles();
		ToggleConsole();
		return (true);	// This key ends here.
	}
	return (false);
}

bool GameClientSlaveManager::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.Options::OptionsManager::UpdateInput(pKeyCode, false);
	return (false);
}

void GameClientSlaveManager::OnInput(UiLepra::InputElement* pElement)
{
	bool lOutputInput;
	CURE_RTVAR_GET(lOutputInput, =, GetVariableScope(), RTVAR_DEBUG_INPUT_PRINT, false);
	if (lOutputInput)
	{
		mLog.Infof(_T("Input %s: %f."), pElement->GetFullName().c_str(), pElement->GetValue());
	}
	
	mOptions.RefreshConfiguration();

	if (mOptions.UpdateInput(pElement))
	{
		if (pElement->GetInterpretation() == UiLepra::InputElement::RELATIVE_AXIS)
		{
			mRelativeAxis.insert(pElement);
			mUnusedRelativeAxis.erase(pElement);
		}
	}
	if (mOptions.IsToggleConsole())
	{
		mOptions.ResetToggles();
		ToggleConsole();
	}
}

void GameClientSlaveManager::HandleUnusedRelativeAxis()
{
	float lMouseFilter;
	CURE_RTVAR_GET(lMouseFilter, =(float), GetVariableScope(), RTVAR_CTRL_MOUSEFILTER, 0.5f);

	InputElementSet lUnusedSet = mUnusedRelativeAxis;
	InputElementSet::iterator x = lUnusedSet.begin();
	for (; x != lUnusedSet.end(); ++x)
	{
		UiLepra::InputElement* lAxis = (*x);

		const str lSuffixes[] = { _T(""), _T("+"), _T("-") };
		for (int y = 0; y < LEPRA_ARRAY_COUNT(lSuffixes); ++y)
		{
			str lAxisName = lAxis->GetFullName() + lSuffixes[y];
			bool lIsSteering = false;
			Options::OptionsManager::ValueArray* lValuePointers = mOptions.GetValuePointers(lAxisName, lIsSteering);
			if (!lValuePointers)
			{
				continue;
			}
			Options::OptionsManager::ValueArray::iterator x = lValuePointers->begin();
			for (; x != lValuePointers->end(); ++x)
			{
				if (std::abs(**x) > 0.02f)
				{
					**x *= lMouseFilter;
				}
				else if (**x != 0)
				{
					**x = 0;
					mRelativeAxis.erase(lAxis);
				}
			}
		}
	}

	// Done. For next loop we assume all relative axis' haven't been triggered.
	mUnusedRelativeAxis = mRelativeAxis;
}



int GameClientSlaveManager::GetSlaveIndex() const
{
	return (mSlaveIndex);
}



PixelRect GameClientSlaveManager::GetRenderArea() const
{
	return (mRenderArea);
}

float GameClientSlaveManager::UpdateFrustum(float pFov)
{
	return GetMaster()->UpdateFrustum(pFov, mRenderArea);
}



str GameClientSlaveManager::GetApplicationCommandFilename() const
{
	return (Application::GetIoFile(
		_T("ClientApplication")+strutil::IntToString(mSlaveIndex, 10),
		_T("lsh")));
}



bool GameClientSlaveManager::Reset()	// Run when disconnected.
{
	ScopeLock lLock(GetTickLock());

	mIsReset = true;

	GetNetworkClient()->Disconnect(true);
	mPingAttemptCount = 0;

	mObjectFrameIndexMap.clear();

	GetContext()->ClearObjects();
	bool lOk = InitializeUniverse();

	mIsResetComplete = true;

	return (lOk);
}



void GameClientSlaveManager::ProcessNetworkInputMessage(Cure::Message* pMessage)
{
	Cure::MessageType lType = pMessage->GetType();
	switch (lType)
	{
		case Cure::MESSAGE_TYPE_STATUS:
		{
			Cure::MessageStatus* lMessageStatus = (Cure::MessageStatus*)pMessage;
			Cure::RemoteStatus lRemoteStatus = lMessageStatus->GetRemoteStatus();
			//mLog.Infof(_T("Got remote status %i with ID %u."), lRemoteStatus, lMessageStatus->GetInteger());
			if (lRemoteStatus != Cure::REMOTE_OK)
			{
				wstr lErrorMessage;
				lMessageStatus->GetMessageString(lErrorMessage);
				mDisconnectReason = strutil::Encode(lErrorMessage);
				mLog.Warning(mDisconnectReason);
				GetNetworkClient()->Disconnect(false);
			}
			else if (GetNetworkClient()->IsLoggingIn())
			{
				GetNetworkClient()->SetLoginAccountId(lMessageStatus->GetInteger());
				mDisconnectReason.clear();
				// A successful login: lets store these parameters for next time!
				CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_LOGIN_USERNAME, mConnectUserName);
				CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, mConnectServerAddress);
				mMasterServerConnection->GraceClose(0.1, false);

				OnLoginSuccess();
			}
			else
			{
				ProcessNetworkStatusMessage(lMessageStatus);
			}
		}
		break;
		case Cure::MESSAGE_TYPE_NUMBER:
		{
			Cure::MessageNumber* lMessageNumber = (Cure::MessageNumber*)pMessage;
			ProcessNumber(lMessageNumber->GetInfo(), lMessageNumber->GetInteger(), lMessageNumber->GetFloat());
		}
		break;
		case Cure::MESSAGE_TYPE_CREATE_OBJECT:
		case Cure::MESSAGE_TYPE_CREATE_OWNED_OBJECT:
		{
			Cure::MessageCreateObject* lMessageCreateObject = (Cure::MessageCreateObject*)pMessage;
			wstr lClassId;
			Lepra::TransformationF lTransformation;
			lMessageCreateObject->GetTransformation(lTransformation);
			//const float a = 1.0f/::sqrt(2.0f);
			//lTransformation.SetOrientation(QuaternionF(0, 0, -a, -a));
			lMessageCreateObject->GetClassId(lClassId);
			/*mLog.Infof(_T("Creating network instance %u of type %s at pos (%f; %f; %f), q (%f, %f, %f, %f)."),
				lMessageCreateObject->GetObjectId(), lClassId.c_str(),
				lTransformation.GetPosition().x, lTransformation.GetPosition().y, lTransformation.GetPosition().z,
				lTransformation.GetOrientation().mA, lTransformation.GetOrientation().mB, lTransformation.GetOrientation().mC, lTransformation.GetOrientation().mD);*/
			Cure::ContextObject* lObject = CreateObject(lMessageCreateObject->GetObjectId(),
				strutil::Encode(lClassId), Cure::NETWORK_OBJECT_REMOTE_CONTROLLED, &lTransformation);
			if (lType == Cure::MESSAGE_TYPE_CREATE_OWNED_OBJECT)
			{
				Cure::MessageCreateOwnedObject* lMessageCreateOwnedObject = (Cure::MessageCreateOwnedObject*)lMessageCreateObject;
				lObject->SetOwnerInstanceId(lMessageCreateOwnedObject->GetOwnerInstanceId());
				//assert(GetContext()->GetObject(lObject->GetOwnerInstanceId(), true));	Owning object may have been destroyed.
			}

		}
		break;
		case Cure::MESSAGE_TYPE_DELETE_OBJECT:
		{
			Cure::MessageDeleteObject* lMessageDeleteObject = (Cure::MessageDeleteObject*)pMessage;
			Cure::GameObjectId lId = lMessageDeleteObject->GetObjectId();
			GetContext()->DeleteObject(lId);
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_POSITION:
		{
			Cure::MessageObjectPosition* lMessageMovement = (Cure::MessageObjectPosition*)pMessage;
			Cure::GameObjectId lInstanceId = lMessageMovement->GetObjectId();
			int32 lFrameIndex = lMessageMovement->GetFrameIndex();
			Cure::ObjectPositionalData& lData = lMessageMovement->GetPositionalData();
			Cure::ContextObject* lObject = GetContext()->GetObject(lInstanceId, true);
			if (!lObject)
			{
				log_volatile(mLog.Debugf(_T("Object %u is missing, can't set pos from network data. Asking server for re-creation."), lInstanceId));
				GetNetworkAgent()->SendNumberMessage(true, GetNetworkClient()->GetSocket(),
					Cure::MessageNumber::INFO_RECREATE_OBJECT, lInstanceId, 0);
			}
			else
			{
				if (!lObject->IsLoaded())
				{
					// Aha! Set positional information so it's there when the object gets loaded.
					lObject->SetInitialPositionalData(lData);
				}

				if (!GetMaster()->IsLocalServer())	// Server will have set the position already.
				{
					if (GetMaster()->IsLocalObject(lInstanceId))
					{
						// Only set changes to locally controlled objects if the change is big.
						SetMovement(lInstanceId, lFrameIndex, lData, 0.5f);
					}
					else
					{
						SetMovement(lInstanceId, lFrameIndex, lData, 0);
					}
				}
			}
			CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_DEBUG_NET_RECVPOSCNT, int, +, 1, 0, 1000000);
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_ATTACH:
		{
			Cure::MessageObjectAttach* lMessageAttach = (Cure::MessageObjectAttach*)pMessage;
			Cure::GameObjectId lObject1Id = lMessageAttach->GetObjectId();
			Cure::GameObjectId lObject2Id = lMessageAttach->GetObject2Id();
			unsigned lBody1Id = lMessageAttach->GetBody1Id();
			unsigned lBody2Id = lMessageAttach->GetBody2Id();
			AttachObjects(lObject1Id, lBody1Id, lObject2Id, lBody2Id);
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_DETACH:
		{
			Cure::MessageObjectDetach* lMessageDetach = (Cure::MessageObjectDetach*)pMessage;
			Cure::GameObjectId lObject1Id = lMessageDetach->GetObjectId();
			Cure::GameObjectId lObject2Id = lMessageDetach->GetObject2Id();
			DetachObjects(lObject1Id, lObject2Id);
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_ATTRIBUTE:
		{
			Cure::MessageObjectAttribute* lMessageAttrib = (Cure::MessageObjectAttribute*)pMessage;
			Cure::GameObjectId lObjectId = lMessageAttrib->GetObjectId();
			unsigned lByteSize = 0;
			const uint8* lBuffer = lMessageAttrib->GetReadBuffer(lByteSize);
			GetContext()->UnpackObjectAttribute(lObjectId, lBuffer, lByteSize);
		}
		break;
		default:
		{
			mLog.AError("Got bad message type from server.");
		}
		break;
	}
}

void GameClientSlaveManager::ProcessNetworkStatusMessage(Cure::MessageStatus* pMessage)
{
	switch (pMessage->GetInfo())
	{
		case Cure::MessageStatus::INFO_CHAT:
		{
			wstr lChatMessage;
			pMessage->GetMessageString(lChatMessage);
			if (pMessage->GetInteger() == 0)
			{
				lChatMessage = L"ServerAdmin: "+lChatMessage;
			}
			else
			{
				lChatMessage = L"<Player?>: "+lChatMessage;
			}
			mLog.Headline(strutil::Encode(lChatMessage));
		}
		break;
		case Cure::MessageStatus::INFO_LOGIN:
		{
			assert(false);
		}
		break;
		case Cure::MessageStatus::INFO_COMMAND:
		{
			if (pMessage->GetRemoteStatus() == Cure::REMOTE_OK)
			{
				wstr lCommand;
				pMessage->GetMessageString(lCommand);
				ClientConsoleManager* lConsole = ((ClientConsoleManager*)GetConsoleManager());
				const int lPreviousSecurityLevel = lConsole->GetSecurityLevel();
				lConsole->SetSecurityLevel(1);
				int lResult = lConsole->FilterExecuteCommand(strutil::Encode(lCommand));
				lConsole->SetSecurityLevel(lPreviousSecurityLevel);
				if (lResult != 0)
				{
					mDisconnectReason = _T("Server not safe! Please join some other game.");
					GetNetworkClient()->Disconnect(false);
				}
			}
		}
		break;
	}
}

void GameClientSlaveManager::ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	switch (pType)
	{
		case Cure::MessageNumber::INFO_SET_TIME:
		{
			log_volatile(mLog.Tracef(_T("Setting physics frame to %i."), pInteger));
			if (!GetMaster()->IsLocalServer())
			{
				GetMaster()->GetTimeManager()->SetCurrentPhysicsFrame(pInteger);
			}
		}
		break;
		case Cure::MessageNumber::INFO_ADJUST_TIME:
		{
			log_atrace("Adjusting time.");
			if (!GetMaster()->IsLocalServer())
			{
				GetMaster()->GetTimeManager()->SetPhysicsSpeedAdjustment(pFloat, pInteger);
			}
		}
		break;
		case Cure::MessageNumber::INFO_PONG:
		{
			if (GetNetworkClient()->GetSocket())
			{
				mPingAttemptCount = 0;
				log_volatile(const float lPingTime = GetTimeManager()->ConvertPhysicsFramesToSeconds(GetTimeManager()->GetCurrentPhysicsFrameDelta(pInteger)));
				log_volatile(const float lServerStriveTime = GetTimeManager()->ConvertPhysicsFramesToSeconds((int)pFloat)*2);
				log_volatile(mLog.Debugf(_T("Pong: this=%ss, server sim strives to be x2=%ss ahead, (self=%s)."),
					Number::ConvertToPostfixNumber(lPingTime, 2).c_str(),
					Number::ConvertToPostfixNumber(lServerStriveTime, 2).c_str(),
					GetNetworkClient()->GetSocket()->GetLocalAddress().GetAsString().c_str()));
			}
		}
		break;
		case Cure::MessageNumber::INFO_GRANT_LOAN:
		{
			const Cure::GameObjectId lInstanceId = pInteger;
			mOwnedObjectList.insert(lInstanceId);
			Cure::ContextObject* lObject = GetContext()->GetObject(lInstanceId);
			if (lObject)
			{
				const int lOwnershipFrames = GetTimeManager()->GetPhysicsFrameDelta((int)pFloat, GetTimeManager()->GetCurrentPhysicsFrame());
				const float lOwnershipSeconds = GetTimeManager()->ConvertPhysicsFramesToSeconds(lOwnershipFrames-2);
				lObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
				GetContext()->AddAlarmCallback(lObject, Cure::ContextManager::SYSTEM_ALARM_ID_OWNERSHIP_LOAN_EXPIRES, lOwnershipSeconds, 0);
				log_volatile(mLog.Debugf(_T("Got control over object with ID %i for %f seconds."), pInteger, lOwnershipSeconds));
			}
		}
		break;
		default:
		{
			assert(false);
		}
		break;
	}
}

Cure::ContextObject* GameClientSlaveManager::CreateObject(Cure::GameObjectId pInstanceId, const str& pClassId,
	Cure::NetworkObjectType pNetworkType, TransformationF* pTransform)
{
	Cure::ContextObject* lObject = GetContext()->GetObject(pInstanceId, true);
	if (lObject && lObject->GetClassId() != pClassId)
	{
		// Ouch, this object has been killed and we weren't informed.
		mLog.Warningf(_T("Ouch, slave %i creating context object %s, but already had old instance of type %s (deleting that first)."),
			mSlaveIndex, pClassId.c_str(), lObject->GetClassId().c_str());
		GetContext()->DeleteObject(pInstanceId);
		lObject = 0;
	}
	if (!lObject)
	{
		log_volatile(mLog.Debugf(_T("Slave %i creating context object %s."), mSlaveIndex, pClassId.c_str()));
		lObject = Parent::CreateContextObject(pClassId, pNetworkType, pInstanceId);
		if (pTransform)
		{
			lObject->SetInitialTransform(*pTransform);
		}
		if (!lObject->IsLoaded())
		{
			lObject->StartLoading();
		}
	}
	return lObject;
}

void GameClientSlaveManager::SetMovement(Cure::GameObjectId pInstanceId, int32 pFrameIndex, Cure::ObjectPositionalData& pData, float pDeltaThreshold)
{
	ObjectFrameIndexMap::iterator x = mObjectFrameIndexMap.find(pInstanceId);
	if (x == mObjectFrameIndexMap.end())
	{
		mObjectFrameIndexMap.insert(ObjectFrameIndexMap::value_type(pInstanceId, pFrameIndex-1));
		x = mObjectFrameIndexMap.find(pInstanceId);
	}
	const int lLastSetFrameIndex = x->second;	// Last set frame index.
	const int lDeltaFrames = GetTimeManager()->GetPhysicsFrameDelta(pFrameIndex, lLastSetFrameIndex);
	if (lDeltaFrames >= 0 || lDeltaFrames < -1000)	// Either it's newer, or it's long, long ago (meaning time wrap).
	{
		x->second = pFrameIndex;

		//str s = strutil::Format(_T("client %i at frame %i"), pClientIndex, pFrameIndex);
		//log_debug(_T("Client set pos of other client"), s);
		UiCure::CppContextObject* lObject = (UiCure::CppContextObject*)GetContext()->GetObject(pInstanceId, true);
		if (lObject)
		{
			// Client has moved forward in time since the server sent us this positional info
			// some frames ago. Extrapolate forward the number of micro-frames that diff.
			float lExtrapolationFactor;
			CURE_RTVAR_GET(lExtrapolationFactor, =(float), GetVariableScope(), RTVAR_NETPHYS_EXTRAPOLATIONFACTOR, 0.0);
			if (lExtrapolationFactor)
			{
				int lMicroSteps;
				CURE_RTVAR_GET(lMicroSteps, =, GetVariableScope(), RTVAR_PHYSICS_MICROSTEPS, 3);
				const int lFutureStepCount = GetTimeManager()->GetCurrentPhysicsFrameDelta(pFrameIndex) * lMicroSteps;
				const float lStepIncrement = GetTimeManager()->GetAffordedPhysicsStepTime() / lMicroSteps;
				pData.GhostStep(lFutureStepCount, lStepIncrement*lExtrapolationFactor);
			}
			/*bool lSetPosition = true;
			if (pInstanceId == mAvatarId)
			{
				const Cure::ObjectPositionalData* lCurrentPos;
				if (lObject->UpdateFullPosition(lCurrentPos))
				{
					float lResyncOnDiff;
					CURE_RTVAR_GET(lResyncOnDiff, =(float), GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 0.2);
					if (pData.GetScaledDifference(lCurrentPos) < lResyncOnDiff)
					{
						lSetPosition = false;	// Not enough change to take notice. Would just yield a jerky movement, not much more.
					}
				}
			}*/
			lObject->SetFullPosition(pData, pDeltaThreshold);
			bool lEnableSmoothing;
			CURE_RTVAR_GET(lEnableSmoothing, =, GetVariableScope(), RTVAR_NETPHYS_ENABLESMOOTHING, true);
			if (lEnableSmoothing)
			{
				lObject->ActivateLerp();
			}
		}
		else
		{
			mLog.Warningf(_T("Slave %i could not set position for object %i."), mSlaveIndex, pInstanceId);
		}
	}
	else
	{
		log_volatile(mLog.Debugf(_T("Throwing away out-of-order positional data (%i frames late)."), lLastSetFrameIndex-pFrameIndex));
	}
}

bool GameClientSlaveManager::OnPhysicsSend(Cure::ContextObject*)
{
	return (true);	// Say true to drop us from sender list.
}

bool GameClientSlaveManager::OnAttributeSend(Cure::ContextObject* pObject)
{
	log_adebug("Sending attribute(s) for a context object...");
	typedef Cure::ContextObject::AttributeArray AttributeArray;
	const AttributeArray& lAttributes = pObject->GetAttributes();
	AttributeArray::const_iterator x = lAttributes.begin();
	for (; x != lAttributes.end(); ++x)
	{
		Cure::ContextObjectAttribute* lAttribute = *x;
		const int lSendSize = lAttribute->QuerySend();
		const bool lIsAllowedSend = (lAttribute->GetNetworkType() == Cure::ContextObjectAttribute::TYPE_BOTH ||
				lAttribute->GetNetworkType() == Cure::ContextObjectAttribute::TYPE_BOTH_BROADCAST);
		if (lSendSize > 0 && lIsAllowedSend)
		{
			Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
			Cure::MessageObjectAttribute* lAttribMessage = (Cure::MessageObjectAttribute*)GetNetworkAgent()->
				GetPacketFactory()->GetMessageFactory()->Allocate(Cure::MESSAGE_TYPE_OBJECT_ATTRIBUTE);
			lPacket->AddMessage(lAttribMessage);
			lAttribute->Pack(lAttribMessage->GetWriteBuffer(lPacket, pObject->GetInstanceId(), lSendSize));

			assert(!GetNetworkAgent()->GetLock()->IsOwner());
			ScopeLock lTickLock(GetTickLock());
			GetNetworkAgent()->PlaceInSendBuffer(true, GetNetworkClient()->GetSocket(), lPacket);

			GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
		}
	}
	return true;	// Say true to drop us from sender list.
}

bool GameClientSlaveManager::IsServer()
{
	return (false);
}

void GameClientSlaveManager::SendAttach(Cure::ContextObject*, unsigned, Cure::ContextObject*, unsigned)
{
	// Server manages this.
	assert(false);
}

void GameClientSlaveManager::SendDetach(Cure::ContextObject*, Cure::ContextObject*)
{
	// Server manages this.
}

void GameClientSlaveManager::OnAlarm(int pAlarmId, Cure::ContextObject* pObject, void* pExtraData)
{
	if (pAlarmId == Cure::ContextManager::SYSTEM_ALARM_ID_OWNERSHIP_LOAN_EXPIRES)
	{
		assert(IsOwned(pObject->GetInstanceId()));
		mOwnedObjectList.erase(pObject->GetInstanceId());
		pObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
		pObject->DeleteNetworkOutputGhost();
	}
	else
	{
		Parent::OnAlarm(pAlarmId, pObject, pExtraData);
	}
}

void GameClientSlaveManager::AttachObjects(Cure::GameObjectId pObject1Id, unsigned pBody1Id,
	Cure::GameObjectId pObject2Id, unsigned pBody2Id)
{
	Cure::ContextObject* lObject1 = GetContext()->GetObject(pObject1Id);
	Cure::ContextObject* lObject2 = GetContext()->GetObject(pObject2Id);
	if (lObject1 && lObject2)
	{
		lObject1->AttachToObject(pBody1Id, lObject2, pBody2Id);
	}
	else
	{
		assert(false);
	}
}

void GameClientSlaveManager::DetachObjects(Cure::GameObjectId pObject1Id, Cure::GameObjectId pObject2Id)
{
	Cure::ContextObject* lObject1 = GetContext()->GetObject(pObject1Id);
	Cure::ContextObject* lObject2 = GetContext()->GetObject(pObject2Id);
	if (lObject1 && lObject2)
	{
		if (!lObject1->DetachFromObject(lObject2))
		{
			assert(false);
		}
	}
	else
	{
		assert(false);
	}
}



Cure::NetworkClient* GameClientSlaveManager::GetNetworkClient() const
{
	return ((Cure::NetworkClient*)GetNetworkAgent());
}



void GameClientSlaveManager::DrawAsyncDebugInfo()
{
	mUiManager->GetPainter()->ResetClippingRect();
	mUiManager->GetPainter()->SetClippingRect(mRenderArea);

	// Draw send and receive staples.
	int lSendCount;
	CURE_RTVAR_TRYGET(lSendCount, =, GetVariableScope(), RTVAR_DEBUG_NET_SENDPOSCNT, 0);
	if (lSendCount > 0)
	{
		CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_DEBUG_NET_SENDPOSCNT, 0);
	}
	DrawDebugStaple(0, lSendCount*10, Color(255, 0, 0));
	int lRecvCount;
	CURE_RTVAR_TRYGET(lRecvCount, =, GetVariableScope(), RTVAR_DEBUG_NET_RECVPOSCNT, 0);
	if (lRecvCount > 0)
	{
		CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_DEBUG_NET_RECVPOSCNT, 0);
	}
	DrawDebugStaple(1, lRecvCount*10, Color(0, 255, 0));
}

void GameClientSlaveManager::DrawDebugStaple(int pIndex, int pHeight, const Color& pColor)
{
	if (pHeight > 0)
	{
		mUiManager->GetPainter()->SetColor(pColor, 0);
		const int lUnit = 10;
		const int x = mRenderArea.mLeft + lUnit + pIndex*lUnit*2;
		const int y = mRenderArea.mBottom - lUnit - pHeight;
		mUiManager->GetPainter()->FillRect(x, y, x+lUnit, y+pHeight);
	}
}

void GameClientSlaveManager::DrawSyncDebugInfo()
{
	UpdateCameraPosition(false);
	float lFov;
	CURE_RTVAR_GET(lFov, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
	UpdateFrustum(lFov);

	bool lDrawLocalServer;
	CURE_RTVAR_GET(lDrawLocalServer, =, GetVariableScope(), RTVAR_DEBUG_3D_DRAWLOCALSERVER, true);
	const Cure::ContextManager* lServerContext = (lDrawLocalServer && GetMaster()->IsLocalServer())? GetMaster()->GetLocalServer()->GetContext() : 0;
	UiCure::DebugRenderer lDebugRenderer(GetVariableScope(), GetContext(), lServerContext, GetTickLock());
	lDebugRenderer.Render(mUiManager, mRenderArea);
}



LOG_CLASS_DEFINE(GAME, GameClientSlaveManager);



}
