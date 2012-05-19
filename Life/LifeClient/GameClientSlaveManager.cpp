
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientSlaveManager.h"
#include <algorithm>
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/ContextObjectAttribute.h"
#include "../../Cure/Include/FloatAttribute.h"
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/StringUtility.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Time.h"
#include "../../Lepra/Include/Timer.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../UiCure/Include/UiCollisionSoundManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiMachine.h"
#include "../../UiCure/Include/UiProps.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTBC/Include/GUI/UiCustomButton.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../LifeServer/MasterServerConnection.h"
#include "../LifeApplication.h"
#include "GameClientMasterTicker.h"
#include "Level.h"
#include "MassObject.h"
#include "RoadSignButton.h"
#include "RtVar.h"
#include "Sunlight.h"
#include "UiConsole.h"
#include "UiGameServerManager.h"



namespace Life
{



GameClientSlaveManager::GameClientSlaveManager(GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
	Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
	UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea):
	Cure::GameManager(pTime, pVariableScope, pResourceManager, 2000, 7, 1),
	mMaster(pMaster),
	mUiManager(pUiManager),
	mCollisionSoundManager(0),
	mSlaveIndex(pSlaveIndex),
	mRenderArea(pRenderArea),
	mMasterServerConnection(new MasterServerConnection),
	mFirstInitialize(true),
	mIsReset(false),
	mIsResetComplete(false),
	mQuit(false),
	mAvatarId(0),
	mHadAvatar(false),
	mLastSentByteCount(0),
	mPingAttemptCount(0),
	mCamRotateExtra(0),
	mJustLookingAtAvatars(false),
	mAvatarInvisibleCount(0),
	mRoadSignIndex(0),
	mLevelId(0),
	mSun(0),
	mCameraPosition(0, -200, 100),
	//mCameraFollowVelocity(0, 1, 0),
	mCameraUp(0, 0, 1),
	mCameraOrientation(PIF/2, acos(mCameraPosition.z/mCameraPosition.y), 0),
	mCameraTargetXyDistance(20),
	mCameraMaxSpeed(500),
	mAllowMovementInput(true),
	mOptions(pVariableScope, pSlaveIndex),
	mLoginWindow(0),
	mEnginePlaybackTime(0)
{
	mCollisionSoundManager = new UiCure::CollisionSoundManager(this, pUiManager);
	mCollisionSoundManager->AddSound(_T("small_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(0.2f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("big_metal"),	UiCure::CollisionSoundManager::SoundResourceInfo(1.5f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("plastic"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.4f, 0));
	mCollisionSoundManager->AddSound(_T("rubber"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));
	mCollisionSoundManager->AddSound(_T("wood"),		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.5f, 0));

	::memset(mEnginePowerShadow, 0, sizeof(mEnginePowerShadow));

	mCameraPivotPosition = mCameraPosition + GetCameraQuaternion() * Vector3DF(0, mCameraTargetXyDistance*3, 0);

	SetNetworkAgent(new Cure::NetworkClient(GetVariableScope()));

	SetConsoleManager(new ClientConsoleManager(GetResourceManager(), this, mUiManager, GetVariableScope(), mRenderArea));
}

GameClientSlaveManager::~GameClientSlaveManager()
{
	Close();

	mMaster = 0;

	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file ")+GetApplicationCommandFilename());

	GetConsoleManager()->Join();
}

void GameClientSlaveManager::LoadSettings()
{
	str lExternalServerAddress;
	CURE_RTVAR_GET(lExternalServerAddress, =, UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
	GetConsoleManager()->ExecuteCommand(_T("alias gfx-lo \"#") _T(RTVAR_UI_3D_PIXELSHADERS) _T(" false; #") _T(RTVAR_UI_3D_SHADOWS) _T(" No; #") _T(RTVAR_UI_3D_ENABLEMASSOBJECTS) _T(" false; #") _T(RTVAR_UI_3D_ENABLEPARTICLES) _T(" false\""));
	GetConsoleManager()->ExecuteCommand(_T("alias gfx-hi \"#") _T(RTVAR_UI_3D_PIXELSHADERS) _T(" true; #") _T(RTVAR_UI_3D_SHADOWS) _T(" Force:Volumes; #") _T(RTVAR_UI_3D_ENABLEMASSOBJECTS) _T(" true; #") _T(RTVAR_UI_3D_ENABLEPARTICLES) _T(" true\""));
	GetConsoleManager()->ExecuteCommand(_T("execute-file -i ")+GetApplicationCommandFilename());
	mOptions.DoRefreshConfiguration();
	// Always default these settings, to avoid that the user can't get rid of undesired behavior.
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_ENABLE, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_3D_ENABLEAXES, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_3D_ENABLESHAPES, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_3D_DRAWLOCALSERVER, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_INPUT_PRINT, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_YOFFSET, 10);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_GRAPH, false);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_NAMES, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_DEBUG_PERFORMANCE_COUNT, true);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_GAME_TIMEOFDAYFACTOR, 1.0);
	bool lIsServerSelected;
	CURE_RTVAR_TRYGET(lIsServerSelected, =, UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, false);
	if (lIsServerSelected)
	{
		CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, lExternalServerAddress);
	}
	else
	{
		bool lIsOpenServer;
		CURE_RTVAR_GET(lIsOpenServer, =, GetVariableScope(), RTVAR_NETWORK_ENABLEOPENSERVER, false);
		if (lIsOpenServer)
		{
			CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("0.0.0.0:16650"));
		}
		else
		{
			CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
		}
	}
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_PHYSICS_FPS, PHYSICS_FPS);
	CURE_RTVAR_SET(UiCure::GetSettings(), RTVAR_PHYSICS_RTR, 1.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_UI_3D_CAMROTATE, 0.0);
	CURE_RTVAR_INTERNAL(GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
}

void GameClientSlaveManager::SetRenderArea(const PixelRect& pRenderArea)
{
	mRenderArea = pRenderArea;
	if (mLoginWindow)
	{
		mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
			mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
	}
	((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->SetRenderArea(pRenderArea);

	CURE_RTVAR_GET(mCameraTargetXyDistance, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
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
	ClearRoadSigns();
	GetContext()->ClearObjects();

	// Close GUI.
	GetConsoleManager()->Join();
	CloseLoginGui();
}

bool GameClientSlaveManager::IsQuitting() const
{
	return (mQuit);
}

void GameClientSlaveManager::SetIsQuitting()
{
	mLog.Headlinef(_T("Slave %i will quit."), GetSlaveIndex());
	CloseLoginGui();
	((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	SetRoadSignsVisible(false);
	if (Thread::GetCurrentThread()->GetThreadName() == "MainThread")
	{
		GetResourceManager()->Tick();
	}
	mQuit = true;
	CURE_RTVAR_INTERNAL(UiCure::GetSettings(), RTVAR_LOGIN_ISSERVERSELECTED, false);
}

void GameClientSlaveManager::SetFade(float pFadeAmount)
{
	mCameraMaxSpeed = 100000.0f;
	float lBaseDistance;
	CURE_RTVAR_GET(lBaseDistance, =(float), GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, 20.0);
	mCameraTargetXyDistance = lBaseDistance + pFadeAmount*400.0f;
}



GameClientMasterTicker* GameClientSlaveManager::GetMaster() const
{
	return (mMaster);
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
#ifdef LIFE_DEMO
	const double lTime = mDemoTime.QueryTimeDiff();
	if ((mSlaveIndex >= 2 || (mSlaveIndex == 1 && lTime > 10*60))
		&& !IsQuitting())
	{
		const UiTbc::FontManager::FontId lOldFontId = SetFontHeight(36.0);
		str lDemoText = strutil::Format(
			_T(" This is a free demo.\n")
			_T(" Buy the full version\n")
			_T("to loose this annoying\n")
			_T("  text for player %i."), mSlaveIndex+1);
		if ((int)lTime % 3*60 >= 3*60-2)
		{
			lDemoText =
				_T("     ])0n7 b3 B1FF\n")
				_T("g!t pwn4ge & teh kekeke\n")
				_T("     !3UYZORZ n0vv\n");
		}
		const int lTextWidth = mUiManager->GetFontManager()->GetStringWidth(lDemoText);
		const int lTextHeight = mUiManager->GetFontManager()->GetLineHeight()*4;
		const int lOffsetX = (int)(cos(lTime*4.3)*15);
		const int lOffsetY = (int)(sin(lTime*4.1)*15);
		mUiManager->GetPainter()->SetColor(Color(255, (uint8)(50*sin(lTime)+50), (uint8)(127*sin(lTime*0.9)+127), 200), 0);
		mUiManager->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
		mUiManager->GetPainter()->PrintText(lDemoText, mRenderArea.GetCenterX()-lTextWidth/2+lOffsetX, mRenderArea.GetCenterY()-lTextHeight/2+lOffsetY);
		mUiManager->GetFontManager()->SetActiveFont(lOldFontId);
	}
#endif // Demo
	return (true);
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
		bool lSend = false;
		ObjectIdSet::iterator x = mOwnedObjectList.begin();
		for (; x != mOwnedObjectList.end(); ++x)
		{
			Cure::ContextObject* lObject = GetContext()->GetObject(*x);
			if (lObject)
			{
				lObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
				if (!lObject->QueryResendTime(0.1f, true))
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
					CURE_RTVAR_GET(lResyncOnDiff, =(float), GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 100.0);
					if (lForceSendUnsafeClientKeepalive ||
						lIsPositionExpired ||
						(lIsAllwedDiffSend &&
						lPositionalData->GetScaledDifference(lObject->GetNetworkOutputGhost()) > lResyncOnDiff))
					{
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



void GameClientSlaveManager::ToggleConsole()
{
	mAllowMovementInput = !((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->ToggleVisible();
}



void GameClientSlaveManager::RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken)
{
	mMaster->PreLogin(pServerAddress);

	ScopeLock lLock(GetTickLock());

	CloseLoginGui();
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
	mMasterServerConnection->SetSocketInfo(GetNetworkClient(), lConnectTimeout);
	str lServerAddress = pServerAddress;
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
	if (!mMaster->IsLocalServer() && !lIsLocalAddress)
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

void GameClientSlaveManager::SelectAvatar(const Cure::UserAccount::AvatarId& pAvatarId)
{
	DropAvatar();

	log_volatile(mLog.Debugf(_T("Clicked avatar %s."), pAvatarId.c_str()));
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendStatusMessage(GetNetworkClient()->GetSocket(), 0, Cure::REMOTE_OK,
		Cure::MessageStatus::INFO_AVATAR, wstrutil::Encode(pAvatarId), lPacket);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	SetRoadSignsVisible(false);
	mAvatarSelectTime.ClearTimeDiff();
}

bool GameClientSlaveManager::IsUiMoveForbidden(Cure::GameObjectId pObjectId) const
{
	const bool lMoveAllowed = (IsOwned(pObjectId) || GetContext()->IsLocalGameObjectId(pObjectId) ||
		(!GetMaster()->IsLocalObject(pObjectId) && GetMaster()->IsFirstSlave(this)));
	return !lMoveAllowed;
}

void GameClientSlaveManager::GetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const
{
	mMaster->GetSiblings(pObjectId, pSiblingArray);
}

void GameClientSlaveManager::DoGetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const
{
	Cure::ContextObject* lSibling = GetContext()->GetObject(pObjectId);
	if (lSibling)
	{
		pSiblingArray.push_back(lSibling);
	}
}

void GameClientSlaveManager::AddLocalObjects(std::hash_set<Cure::GameObjectId>& pLocalObjectSet)
{
	if (mAvatarId)
	{
		Cure::ContextObject* lAvatar = GetContext()->GetObject(mAvatarId);
		if (mHadAvatar && !lAvatar)
		{
			DropAvatar();
		}
		else if (lAvatar)
		{
			mHadAvatar = true;
		}
	}

	pLocalObjectSet.insert(mOwnedObjectList.begin(), mOwnedObjectList.end());
}

bool GameClientSlaveManager::IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const
{
	return (pPosition.GetDistanceSquared(mCameraPosition) <= pDistance*pDistance);
}

bool GameClientSlaveManager::IsOwned(Cure::GameObjectId pObjectId) const
{
	return (mOwnedObjectList.find(pObjectId) != mOwnedObjectList.end());
}

Cure::GameObjectId GameClientSlaveManager::GetAvatarInstanceId() const
{
	return mAvatarId;
}



bool GameClientSlaveManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.RefreshConfiguration();

	mOptions.UpdateInput(pKeyCode, true);
	if (mOptions.GetConsoleToggle() >= 0.5f)
	{
		mOptions.ResetToggles();
		ToggleConsole();
		return (true);	// This key ends here.
	}
	return (false);
}

bool GameClientSlaveManager::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	mOptions.UpdateInput(pKeyCode, false);
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

	if (mAvatarSelectTime.QueryTimeDiff() > 1.0)
	{
		if (pElement->GetParentDevice() == mUiManager->GetInputManager()->GetMouse())
		{
			PixelCoord lPosition = mUiManager->GetMouseDisplayPosition();
			if (mRenderArea.IsInside(lPosition.x, lPosition.y))
			{
				SetRoadSignsVisible(true);
				mJustLookingAtAvatars = true;
				mAvatarMightSelectTime.PopTimeDiff();
			}
		}
	}
	if (mJustLookingAtAvatars && mAvatarMightSelectTime.GetTimeDiff() > 2.0)
	{
		SetRoadSignsVisible(false);
	}

	mOptions.UpdateInput(pElement);
	if (mOptions.GetConsoleToggle() >= 0.5f)
	{
		mOptions.ResetToggles();
		ToggleConsole();
	}
}



bool GameClientSlaveManager::SetAvatarEnginePower(unsigned pAspect, float pPower, float pAngle)
{
	assert(pAspect >= 0 && pAspect < TBC::PhysicsEngine::ASPECT_COUNT);
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		return SetAvatarEnginePower(lObject, pAspect, pPower, pAngle);
	}
	return false;
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
	return mMaster->UpdateFrustum(pFov, mRenderArea);
}



str GameClientSlaveManager::GetApplicationCommandFilename() const
{
	return (Application::GetIoFile(
		_T("ClientApplication")+strutil::IntToString(mSlaveIndex, 10),
		_T("lsh")));
}



bool GameClientSlaveManager::Reset()	// Run when disconnected. Removes all objects and displays login GUI.
{
	ScopeLock lLock(GetTickLock());

	mIsReset = true;

	GetNetworkClient()->Disconnect(true);
	mPingAttemptCount = 0;
	//GetTimeManager()->Clear(0);

	mObjectFrameIndexMap.clear();

	ClearRoadSigns();
	GetContext()->ClearObjects();
	bool lOk = InitializeTerrain();

	if (lOk)
	{
		CreateLoginView();
	}

	mIsResetComplete = true;

	return (lOk);
}

void GameClientSlaveManager::CreateLoginView()
{
	if (!mLoginWindow && !GetNetworkClient()->IsActive())
	{
		// If first attempt (i.e. no connection problems) just skip interactivity.
		if (mDisconnectReason.empty())
		{
			str lServerName;
			CURE_RTVAR_TRYGET(lServerName, =, Cure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, _T("localhost:16650"));
			if (strutil::StartsWith(lServerName, _T("0.0.0.0")))
			{
				lServerName = lServerName.substr(7);
			}
			const str lDefaultUserName = strutil::Format(_T("User%u"), mSlaveIndex);
			str lUserName;
			CURE_RTVAR_TRYGET(lUserName, =, GetVariableScope(), RTVAR_LOGIN_USERNAME, lDefaultUserName);
                        wstr lReadablePassword = L"CarPassword";
                        const Cure::MangledPassword lPassword(lReadablePassword);
			const Cure::LoginId lLoginToken(wstrutil::Encode(lUserName), lPassword);
			RequestLogin(lServerName, lLoginToken);
		}
		else
		{
			mLoginWindow = new ClientLoginView(this, mDisconnectReason);
			mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
			mUiManager->GetDesktopWindow()->AddChild(mLoginWindow, 0, 0, 0);
			mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
				mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
			mLoginWindow->GetChild(_T("User"), 0)->SetKeyboardFocus();
		}
	}
}

bool GameClientSlaveManager::InitializeTerrain()
{
	mSun = 0;
	mCloudArray.clear();

	mLevelId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	UiCure::CppContextObject* lLevel = new Level(GetResourceManager(), _T("level_01"), mUiManager);
	AddContextObject(lLevel, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED, mLevelId);
	bool lOk = (lLevel != 0);
	assert(lOk);
	if (lOk)
	{
		lLevel->DisableRootShadow();
		lLevel->SetAllowNetworkLogic(false);
		lLevel->StartLoading();
		mSun = new UiCure::Props(GetResourceManager(), _T("sun"), mUiManager);
		AddContextObject(mSun, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lOk = (mSun != 0);
		assert(lOk);
		if (lOk)
		{
			mSun->StartLoading();
		}
	}
	const int lPrimeCloudCount = 11;	// TRICKY: must be prime or clouds start moving in sync.
	for (int x = 0; lOk && x < lPrimeCloudCount; ++x)
	{
		Cure::ContextObject* lCloud = new UiCure::Props(GetResourceManager(), _T("cloud_01"), mUiManager);
		AddContextObject(lCloud, Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		lCloud->StartLoading();
		mCloudArray.push_back(lCloud);
	}
	mMassObjectArray.clear();
	return (lOk);
}

void GameClientSlaveManager::CloseLoginGui()
{
	if (mLoginWindow)
	{
		ScopeLock lLock(GetTickLock());
		mUiManager->GetDesktopWindow()->RemoveChild(mLoginWindow, 0);
		delete (mLoginWindow);
		mLoginWindow = 0;
	}
}

void GameClientSlaveManager::ClearRoadSigns()
{
	ScopeLock lLock(GetTickLock());

	mRoadSignIndex = 0;
	RoadSignMap::iterator x = mRoadSignMap.begin();
	for (; x != mRoadSignMap.end(); ++x)
	{
		GetContext()->DeleteObject(x->second->GetInstanceId());
	}
	mRoadSignMap.clear();
}

void GameClientSlaveManager::SetRoadSignsVisible(bool pVisible)
{
	RoadSignMap::iterator x = mRoadSignMap.begin();
	for (; x != mRoadSignMap.end(); ++x)
	{
		x->second->SetIsMovingIn(pVisible);
	}
}



void GameClientSlaveManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void GameClientSlaveManager::TickUiInput()
{
	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	const int lPhysicsStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
	if (lPlaybackMode != PLAYBACK_PLAY && lPhysicsStepCount > 0 && mAllowMovementInput)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
			mAvatarMightSelectTime.UpdateTimer();

			QuerySetChildishness(lObject);

			const Options::Steering& s = mOptions.GetSteeringControl();
			const bool lIsMovingForward = lObject->GetForwardSpeed() > 3.0f;
#define S(dir) s.mControl[Options::Steering::CONTROL_##dir]
			const float lForward = S(FORWARD);
			const float lBack = S(BACKWARD);
			const float lBreakAndBack = S(BREAKANDBACK);
			float lPowerFwdRev = lForward - std::max(lBack, lIsMovingForward? 0.0f : lBreakAndBack);
			SetAvatarEnginePower(lObject, 0, lPowerFwdRev, mCameraOrientation.x);
			float lPowerLR = S(RIGHT)-S(LEFT);
			SetAvatarEnginePower(lObject, 1, lPowerLR, mCameraOrientation.x);
			float lPower = S(HANDBREAK) - std::max(S(BREAK), lIsMovingForward? lBreakAndBack : 0.0f);
			if (!SetAvatarEnginePower(lObject, 2, lPower, mCameraOrientation.x) &&
				lBreakAndBack > 0 && Math::IsEpsEqual(lBack, 0.0f, 0.01f))
			{
				// Someone is apparently trying to stop/break, but no engine configured for breaking.
				// Just apply it as a reverse motion.
				lPowerFwdRev = lForward - lBreakAndBack;
				SetAvatarEnginePower(lObject, 0, lPowerFwdRev, mCameraOrientation.x);
			}
			lPower = S(UP)-S(DOWN);
			SetAvatarEnginePower(lObject, 3, lPower, mCameraOrientation.x);
			lPower = S(FORWARD3D) - S(BACKWARD3D);
			SetAvatarEnginePower(lObject, 4, lPower, mCameraOrientation.x);
			lPower = S(LEFT3D) - S(RIGHT3D);
			SetAvatarEnginePower(lObject, 5, lPower, mCameraOrientation.x);
			lPower = S(UP3D) - S(DOWN3D);
			SetAvatarEnginePower(lObject, 6, lPower, mCameraOrientation.x);
			const float lSteeringChange = mLastSteering-s;
			if (lSteeringChange > 0.5f)
			{
				mInputExpireAlarm.Set();
			}
			else if (!Math::IsEpsEqual(lSteeringChange, 0.0f, 0.01f))
			{
				mInputExpireAlarm.Push(0.1f);
			}
			mLastSteering = s;

			const float lScale = 50.0f * GetTimeManager()->GetAffordedPhysicsTotalTime();
			const Options::CamControl& c = mOptions.GetCamControl();
#define C(dir) c.mControl[Options::CamControl::CAMDIR_##dir]
			lPower = C(UP)-C(DOWN);
			CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, double, +, lPower*lScale, -5.0, 30.0);
			mCamRotateExtra = (C(RIGHT)-C(LEFT)) * lScale;
			lPower = C(BACKWARD)-C(FORWARD);
			CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_UI_3D_CAMDISTANCE, double, +, lPower*lScale, 3.0, 100.0);

			mAvatarInvisibleCount = 0;
		}
		else if (++mAvatarInvisibleCount > 10)
		{
			mJustLookingAtAvatars = false;
			SetRoadSignsVisible(true);
			mAvatarInvisibleCount = -10000;
		}
	}
}

bool GameClientSlaveManager::SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower, float pAngle)
{
	bool lSet = pAvatar->SetEnginePower(pAspect, pPower, pAngle);

	SteeringPlaybackMode lPlaybackMode;
	CURE_RTVAR_TRYGET(lPlaybackMode, =(SteeringPlaybackMode), GetVariableScope(), RTVAR_STEERING_PLAYBACKMODE, PLAYBACK_NONE);
	if (lPlaybackMode == PLAYBACK_RECORD)
	{
		if (!Math::IsEpsEqual(mEnginePowerShadow[pAspect].mPower, pPower)
			//|| !Math::IsEpsEqual(mEnginePowerShadow[pAspect].mAngle, pAngle, 0.3f)
			)
		{
			mEnginePowerShadow[pAspect].mPower = pPower;
			mEnginePowerShadow[pAspect].mAngle = pAngle;
			if (!mEnginePlaybackFile.IsOpen())
			{
				mEnginePlaybackFile.Open(_T("Data/Steering.rec"), DiskFile::MODE_TEXT_WRITE);
				wstr lComment = wstrutil::Format(L"// Recording %s at %s.\n", pAvatar->GetClassId().c_str(), Time().GetDateTimeAsString().c_str());
				mEnginePlaybackFile.WriteString(lComment);
				mEnginePlaybackFile.WriteString(wstrutil::Encode("#" RTVAR_STEERING_PLAYBACKMODE " 2\n"));
			}
			const float lTime = GetTimeManager()->GetAbsoluteTime();
			if (lTime != mEnginePlaybackTime)
			{
				wstr lCommand = wstrutil::Format(L"sleep %g\n", Cure::TimeManager::GetAbsoluteTimeDiff(lTime, mEnginePlaybackTime));
				mEnginePlaybackFile.WriteString(lCommand);
				mEnginePlaybackTime = lTime;
			}
			wstr lCommand = wstrutil::Format(L"set-avatar-engine-power %u %g %g\n", pAspect, pPower, pAngle);
			mEnginePlaybackFile.WriteString(lCommand);
		}
	}
	else if (lPlaybackMode == PLAYBACK_NONE)
	{
		if (mEnginePlaybackFile.IsOpen())
		{
			if (mEnginePlaybackFile.IsInMode(File::WRITE_MODE))
			{
				mEnginePlaybackFile.WriteString(wstrutil::Encode("#" RTVAR_STEERING_PLAYBACKMODE " 0\n"));
			}
			mEnginePlaybackFile.Close();
		}
		mEnginePlaybackTime = GetTimeManager()->GetAbsoluteTime();
		mEnginePowerShadow[pAspect].mPower = 0;
		mEnginePowerShadow[pAspect].mAngle = 0;
	}

	return lSet;
}

void GameClientSlaveManager::TickUiUpdate()
{
	((ClientConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();

	mCollisionSoundManager->Tick(mCameraPosition);

	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime < 1e-5)
	{
		return;
	}

	// TODO: remove camera hack (camera position should be context object controlled).
	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	Vector3DF lAvatarPosition = mCameraPivotPosition;
	float lCameraPivotSpeed = 0;
	if (lObject)
	{
		// Target position is <cam> distance from the avatar along a straight line
		// (in the XY plane) to where the camera currently is.
		lAvatarPosition = lObject->GetPosition();
		mCameraPivotPosition = lAvatarPosition;
		Vector3DF lAvatarVelocity = lObject->GetVelocity();
		lAvatarVelocity.z *= 0.2f;	// Don't take very much action on the up/down speed, that has it's own algo.
		mCameraPivotVelocity = Math::Lerp(mCameraPivotVelocity, lAvatarVelocity, 0.5f*lPhysicsTime/0.1f);
		mCameraPivotPosition += mCameraPivotVelocity * 0.6f;	// Look to where the avatar will be in a while.
		lCameraPivotSpeed = mCameraPivotVelocity.GetLength();

		UpdateMassObjects(mCameraPivotPosition);
	}
	const Vector3DF lPivotXyPosition(mCameraPivotPosition.x, mCameraPivotPosition.y, mCameraPosition.z);
	Vector3DF lTargetCameraPosition(mCameraPosition-lPivotXyPosition);
	const float lCurrentCameraXyDistance = lTargetCameraPosition.GetLength();
	const float lSpeedDependantCameraXyDistance = mCameraTargetXyDistance + lCameraPivotSpeed*0.6f;
	lTargetCameraPosition = lPivotXyPosition + lTargetCameraPosition*(lSpeedDependantCameraXyDistance/lCurrentCameraXyDistance);
	float lCamHeight;
	CURE_RTVAR_GET(lCamHeight, =(float), GetVariableScope(), RTVAR_UI_3D_CAMHEIGHT, 10.0);
	lTargetCameraPosition.z = mCameraPivotPosition.z + lCamHeight;

	if (lObject)
	{
		/* Almost tried out "stay behind velocity". Was too jerky, since velocity varies too much.
		Vector3DF lVelocity = lObject->GetVelocity();
		mCameraFollowVelocity = lVelocity;
		float lSpeed = lVelocity.GetLength();
		if (lSpeed > 0.1f)
		{
			lVelocity.Normalize();
			mCameraFollowVelocity = Math::Lerp(mCameraFollowVelocity, lVelocity, 0.1f).GetNormalized();
		}
		// Project previous "camera up" onto plane orthogonal to the velocity to get new "up".
		Vector3DF lCameraUp = mCameraUp.ProjectOntoPlane(mCameraFollowVelocity) + Vector3DF(0, 0, 0.01f);
		if (lCameraUp.GetLengthSquared() > 0.1f)
		{
			mCameraUp = lCameraUp;
		}
		lSpeed *= 0.05f;
		lSpeed = (lSpeed > 0.4f)? 0.4f : lSpeed;
		mCameraUp.Normalize();
		lTargetCameraPosition = Math::Lerp(lTargetCameraPosition, mCameraPivotPosition - 
			mCameraFollowVelocity * mCameraTargetXyDistance +
			mCameraUp * mCameraTargetXyDistance * 0.3f, 0.0f);*/

		/*// Temporary: changed to "cam stay behind" mode.
		lTargetCameraPosition = lObject->GetOrientation() *
			Vector3DF(0, -mCameraTargetXyDistance, mCameraTargetXyDistance/4) +
			mCameraPivotPosition;*/
	}

	lTargetCameraPosition.x = Math::Clamp(lTargetCameraPosition.x, -1000.0f, 1000.0f);
	lTargetCameraPosition.y = Math::Clamp(lTargetCameraPosition.y, -1000.0f, 1000.0f);
	lTargetCameraPosition.z = Math::Clamp(lTargetCameraPosition.z, -20.0f, 200.0f);

	// Now that we've settled where we should be, it's time to check where we actually can see our avatar.
	// TODO: currently only checks against terrain. Add a ray to world, that we can use for this kinda thing.
	Cure::ContextObject* lLevel = GetContext()->GetObject(mLevelId);
	if (lLevel)
	{
		const float lCameraAboveGround = 0.3f;
		lTargetCameraPosition.z -= lCameraAboveGround;
		const TBC::PhysicsManager::BodyID lTerrainBodyId = lLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		Vector3DF lCollisionPoint;
		float lStepSize = (lTargetCameraPosition - lAvatarPosition).GetLength() * 0.5f;
		for (int y = 0; y < 5; ++y)
		{
			int x;
			for (x = 0; x < 2; ++x)
			{
				const Vector3DF lRay = lTargetCameraPosition - lAvatarPosition;
				const bool lIsCollision = (GetPhysicsManager()->QueryRayCollisionAgainst(
					lAvatarPosition, lRay, lRay.GetLength(), lTerrainBodyId, &lCollisionPoint, 1) > 0);
				if (lIsCollision)
				{
					lTargetCameraPosition.z += lStepSize;
				}
				else
				{
					if (x != 0)
					{
						lTargetCameraPosition.z -= lStepSize;
					}
					break;
				}
			}
			if (x == 0 && y == 0)
			{
				break;
			}
			lStepSize *= 1/3.0f;
			//lTargetCameraPosition.z += lStepSize;
		}
		lTargetCameraPosition.z += lCameraAboveGround;
	}

	const float lHalfDistanceTime = 0.1f;	// Time it takes to half the distance from where it is now to where it should be.
	float lMovingAveragePart = 0.5f*lPhysicsTime/lHalfDistanceTime;
	if (lMovingAveragePart > 0.8f)
	{
		lMovingAveragePart = 0.8f;
	}
	//lMovingAveragePart = 1;
	const Vector3DF lNewPosition = Math::Lerp<Vector3DF, float>(mCameraPosition,
		lTargetCameraPosition, lMovingAveragePart);
	const Vector3DF lDirection = lNewPosition-mCameraPosition;
	const float lDistance = lDirection.GetLength();
	if (lDistance > mCameraMaxSpeed*lPhysicsTime)
	{
		mCameraPosition += lDirection*(mCameraMaxSpeed*lPhysicsTime/lDistance);
	}
	else
	{
		mCameraPosition = lNewPosition;
	}
	if (lNewPosition.z > mCameraPosition.z)	// Dolly cam up pretty quick to avoid looking "through the ground."
	{
		mCameraPosition.z = Math::Lerp(mCameraPosition.z, lNewPosition.z, lHalfDistanceTime);
	}

	// "Roll" camera towards avatar.
	const float lNewTargetCameraXyDistance = mCameraPosition.GetDistance(lPivotXyPosition);
	const float lNewTargetCameraDistance = mCameraPosition.GetDistance(mCameraPivotPosition);
	Vector3DF lTargetCameraOrientation;
	lTargetCameraOrientation.Set(::asin((mCameraPosition.x-lPivotXyPosition.x)/lNewTargetCameraXyDistance) + PIF/2,
		::acos((mCameraPivotPosition.z-mCameraPosition.z)/lNewTargetCameraDistance), 0);
	if (lPivotXyPosition.y-mCameraPosition.y < 0)
	{
		lTargetCameraOrientation.x = -lTargetCameraOrientation.x;
	}
	Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
	float lYawChange = (lTargetCameraOrientation.x-mCameraOrientation.x)*3;
	lYawChange = (lYawChange < -PIF*3/7)? -PIF*3/7 : lYawChange;
	lYawChange = (lYawChange > PIF*3/7)? PIF*3/7 : lYawChange;
	lTargetCameraOrientation.z = -lYawChange;
	Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
	Math::RangeAngles(mCameraOrientation.y, lTargetCameraOrientation.y);
	Math::RangeAngles(mCameraOrientation.z, lTargetCameraOrientation.z);
	mCameraOrientation = Math::Lerp<Vector3DF, float>(mCameraOrientation, lTargetCameraOrientation, lMovingAveragePart);

	float lRotationFactor;
	CURE_RTVAR_GET(lRotationFactor, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	lRotationFactor += mCamRotateExtra;
	if (lRotationFactor)
	{
		TransformationF lTransform(GetCameraQuaternion(), mCameraPosition);
		lTransform.RotateAroundAnchor(mCameraPivotPosition, Vector3DF(0, 0, 1), lRotationFactor * lPhysicsTime);
		mCameraPosition = lTransform.GetPosition();
		float lTheta;
		float lPhi;
		float lGimbal;
		lTransform.GetOrientation().GetEulerAngles(lTheta, lPhi, lGimbal);
		mCameraOrientation.x = lTheta+PIF/2;
		mCameraOrientation.y = PIF/2-lPhi;
		mCameraOrientation.z = lGimbal;
	}
}

bool GameClientSlaveManager::UpdateMassObjects(const Vector3DF& pPosition)
{
	bool lOk = true;

	const Cure::ContextObject* lLevel = GetContext()->GetObject(mLevelId);
	if (lLevel && mMassObjectArray.empty())
	{
		const TBC::PhysicsManager::BodyID lTerrainBodyId = lLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
		if (lOk)
		{
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			Cure::ContextObject* lFlowers = new MassObject(GetResourceManager(), _T("flower"), mUiManager, lTerrainBodyId, 600, 170);
			AddContextObject(lFlowers, Cure::NETWORK_OBJECT_LOCAL_ONLY, lMassObjectId);
			lFlowers->StartLoading();
		}
		if (lOk)
		{
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			Cure::ContextObject* lBushes = new MassObject(GetResourceManager(), _T("bush_01"), mUiManager, lTerrainBodyId, 150, 290);
			AddContextObject(lBushes, Cure::NETWORK_OBJECT_LOCAL_ONLY, lMassObjectId);
			lBushes->StartLoading();
		}
	}

	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		MassObject* lObject = (MassObject*)GetContext()->GetObject(*x, true);
		assert(lObject);
		lObject->SetRootPosition(pPosition);
	}
	return lOk;
}

void GameClientSlaveManager::SetLocalRender(bool pRender)
{
	if (pRender)
	{
		// Update light and sun according to this slave's camera.
		const float lSunDistance = 1700;
		mSun->SetRootPosition(mCameraPosition + lSunDistance * mMaster->GetSunlight()->GetDirection());

		const float lCloudDistance = 600;
		size_t x = 0;
		for (; x < mCloudArray.size(); ++x)
		{
			Cure::ContextObject* lCloud = mCloudArray[x];
			float lTod = mMaster->GetSunlight()->GetTimeOfDay();
			lTod += x / (float)mCloudArray.size();
			lTod *= 2 * PIF;
			const float x = sin(lTod*2) * lCloudDistance;
			const float y = cos(lTod) * lCloudDistance;
			const float z = cos(lTod*3) * lCloudDistance * 0.2f + lCloudDistance * 0.4f;
			lCloud->SetRootPosition(Vector3DF(x, y, z));
		}

		bool lMass;
		CURE_RTVAR_GET(lMass, =, GetVariableScope(), RTVAR_UI_3D_ENABLEMASSOBJECTS, false);
		SetMassRender(lMass);
	}
	else
	{
		SetMassRender(false);
	}
}

void GameClientSlaveManager::SetMassRender(bool pRender)
{
	ObjectArray::const_iterator x = mMassObjectArray.begin();
	for (; x != mMassObjectArray.end(); ++x)
	{
		MassObject* lObject = (MassObject*)GetContext()->GetObject(*x);
		if (lObject)
		{
			lObject->SetRender(pRender);
		}
	}
}

void GameClientSlaveManager::PhysicsTick()
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

	Parent::PhysicsTick();
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
				ClearRoadSigns();
				// A successful login: lets store these parameters for next time!
				CURE_RTVAR_SYS_OVERRIDE(GetVariableScope(), RTVAR_LOGIN_USERNAME, mConnectUserName);
				CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, mConnectServerAddress);
				mMasterServerConnection->GraceClose(0.1, false);
			}
			else
			{
				switch (lMessageStatus->GetInfo())
				{
					case Cure::MessageStatus::INFO_CHAT:
					{
						wstr lChatMessage;
						lMessageStatus->GetMessageString(lChatMessage);
						if (lMessageStatus->GetInteger() == 0)
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
					case Cure::MessageStatus::INFO_AVATAR:
					{
						wstr lAvatarName;
						lMessageStatus->GetMessageString(lAvatarName);
						Cure::UserAccount::AvatarId lAvatarId = strutil::Encode(lAvatarName);
						log_adebug("Status: INFO_AVATAR...");
						str lTextureId = strutil::Format(_T("road_sign_%s.png"), lAvatarId.c_str());
						if (!GetResourceManager()->QueryFileExists(lTextureId))
						{
							lTextureId = _T("road_sign_car.png");
						}
						RoadSignButton* lButton = new RoadSignButton(this, GetResourceManager(),
							mUiManager, lAvatarId, _T("road_sign_01"), lTextureId, RoadSignButton::SHAPE_ROUND);
						GetContext()->AddLocalObject(lButton);
						const int SIGN_COUNT_X = 5;
						const int SIGN_COUNT_Y = 5;
						const float lDeltaX = 1 / (float)SIGN_COUNT_X;
						const float lDeltaY = 1 / (float)SIGN_COUNT_Y;
						const float x = (mRoadSignIndex % SIGN_COUNT_X) * lDeltaX - 0.5f + 0.5f*lDeltaX;
						const float y = (mRoadSignIndex / SIGN_COUNT_X) * lDeltaY - 0.5f + 0.5f*lDeltaY;
						++mRoadSignIndex;
						lButton->SetTrajectory(Vector2DF(x, y), 8);
						lButton->GetButton().SetOnClick(GameClientSlaveManager, OnAvatarSelect);
						mRoadSignMap.insert(RoadSignMap::value_type(lButton->GetInstanceId(), lButton));
						lButton->StartLoading();
						mJustLookingAtAvatars = false;
					}
					break;
					case Cure::MessageStatus::INFO_LOGIN:
					{
						assert(false);
					}
					break;
					case Cure::MessageStatus::INFO_COMMAND:
					{
						if (lMessageStatus->GetRemoteStatus() == Cure::REMOTE_OK)
						{
							wstr lCommand;
							lMessageStatus->GetMessageString(lCommand);
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
		}
		break;
		case Cure::MESSAGE_TYPE_NUMBER:
		{
			Cure::MessageNumber* lMessageNumber = (Cure::MessageNumber*)pMessage;
			ProcessNumber(lMessageNumber->GetInfo(), lMessageNumber->GetInteger(), lMessageNumber->GetFloat());
		}
		break;
		case Cure::MESSAGE_TYPE_CREATE_OBJECT:
		{
			Cure::MessageCreateObject* lMessageCreateObject = (Cure::MessageCreateObject*)pMessage;
			wstr lClassId;
			Lepra::TransformationF lTransformation;
			lMessageCreateObject->GetTransformation(lTransformation);
			lMessageCreateObject->GetClassId(lClassId);
			CreateObject(lMessageCreateObject->GetObjectId(),
				strutil::Encode(lClassId),
				Cure::NETWORK_OBJECT_REMOTE_CONTROLLED, &lTransformation);
		}
		break;
		case Cure::MESSAGE_TYPE_DELETE_OBJECT:
		{
			Cure::MessageDeleteObject* lMessageDeleteObject = (Cure::MessageDeleteObject*)pMessage;
			Cure::GameObjectId lId = lMessageDeleteObject->GetObjectId();
			GetContext()->DeleteObject(lId);
			if (lId == mAvatarId)
			{
				DropAvatar();
			}
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_POSITION:
		{
			Cure::MessageObjectPosition* lMessageMovement = (Cure::MessageObjectPosition*)pMessage;
			Cure::GameObjectId lInstanceId = lMessageMovement->GetObjectId();
			int32 lFrameIndex = lMessageMovement->GetFrameIndex();
			Cure::ObjectPositionalData& lData = lMessageMovement->GetPositionalData();
			if (GetContext()->GetObject(lInstanceId, true) == 0)
			{
				GetNetworkAgent()->SendNumberMessage(true, GetNetworkClient()->GetSocket(),
					Cure::MessageNumber::INFO_RECREATE_OBJECT, lInstanceId, 0);
			}
			else
			{
				SetMovement(lInstanceId, lFrameIndex, lData);
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

void GameClientSlaveManager::ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat)
{
	switch (pType)
	{
		case Cure::MessageNumber::INFO_SET_TIME:
		{
			log_volatile(mLog.Tracef(_T("Setting physics frame to %i."), pInteger));
			mMaster->GetTimeManager()->SetCurrentPhysicsFrame(pInteger);
		}
		break;
		case Cure::MessageNumber::INFO_ADJUST_TIME:
		{
			log_atrace("Adjusting time.");
			mMaster->GetTimeManager()->SetPhysicsSpeedAdjustment(pFloat, pInteger);
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
		case Cure::MessageNumber::INFO_AVATAR:
		{
			mAvatarId = pInteger;
			mOwnedObjectList.insert(mAvatarId);
			mLog.Infof(_T("Got control over avatar with ID %i."), pInteger);
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
				GetContext()->AddAlarmCallback(lObject, Cure::ContextManager::SYSTEM_ALARM_ID, lOwnershipSeconds, 0);
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

bool GameClientSlaveManager::CreateObject(Cure::GameObjectId pInstanceId, const str& pClassId,
	Cure::NetworkObjectType pNetworkType, TransformationF* pTransform)
{
	Cure::ContextObject* lPreviousObject = GetContext()->GetObject(pInstanceId, true);
	//assert(!lPreviousObject);
	if (!lPreviousObject)
	{
		log_volatile(mLog.Debugf(_T("Slave %i creating context object %s."), mSlaveIndex, pClassId.c_str()));
		Cure::ContextObject* lObject = Parent::CreateContextObject(pClassId, pNetworkType, pInstanceId);
		if (pTransform)
		{
			lObject->SetInitialTransform(*pTransform);
		}
		lObject->StartLoading();
	}
	else
	{
		assert(lPreviousObject->GetClassId() == pClassId);
	}
	return (true);
}

Cure::ContextObject* GameClientSlaveManager::CreateContextObject(const str& pClassId) const
{
	Cure::CppContextObject* lObject;
	if (pClassId == _T("stone") || pClassId == _T("cube"))	// TODO: remove hard-coding?
	{
		lObject = new UiCure::CppContextObject(GetResourceManager(), pClassId, mUiManager);
	}
	else
	{
		lObject = new UiCure::Machine(GetResourceManager(), pClassId, mUiManager);
	}
	lObject->SetAllowNetworkLogic(false);	// Only server gets to control logic.
	return (lObject);
}

void GameClientSlaveManager::OnLoadCompleted(Cure::ContextObject* pObject, bool pOk)
{
	if (pOk)
	{
		if (pObject->GetInstanceId() == mAvatarId)
		{
			log_volatile(mLog.Debug(_T("Yeeha! Loaded avatar!")));
		}
		else
		{
			log_volatile(mLog.Tracef(_T("Loaded object %s."), pObject->GetClassId().c_str()));
		}
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		GetContext()->PostKillObject(pObject->GetInstanceId());
	}
}

void GameClientSlaveManager::SetMovement(Cure::GameObjectId pInstanceId, int32 pFrameIndex, Cure::ObjectPositionalData& pData)
{
	ObjectFrameIndexMap::iterator x = mObjectFrameIndexMap.find(pInstanceId);
	if (x == mObjectFrameIndexMap.end())
	{
		mObjectFrameIndexMap.insert(ObjectFrameIndexPair(pInstanceId, pFrameIndex-1));
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
					CURE_RTVAR_GET(lResyncOnDiff, =(float), GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 100.0);
					if (pData.GetScaledDifference(lCurrentPos) < lResyncOnDiff)
					{
						lSetPosition = false;	// Not enough change to take notice. Would just yield a jerky movement, not much more.
					}
				}
			}*/
			lObject->SetFullPosition(pData);
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

void GameClientSlaveManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
	TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID)
{
	mCollisionSoundManager->OnCollision(pForce, pTorque, pPosition, pObject1, pObject2, pBody1Id, 200, false);

	const bool lObject1Dynamic = (pObject1->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	const bool lObject2Dynamic = (pObject2->GetPhysics()->GetPhysicsType() == TBC::ChunkyPhysics::DYNAMIC);
	if (!lObject1Dynamic || !lObject2Dynamic)
	{
		return;
	}

	if (IsOwned(pObject1->GetInstanceId()))
	{
		if (pObject1->GetImpact(GetPhysicsManager()->GetGravity(), pForce, pTorque) >= 2.0f)
		{
			pObject1->QueryResendTime(0, false);
		}
		mCollisionExpireAlarm.SetIfNotSet();
	}
	else if (pObject2->GetInstanceId() == mAvatarId &&
		pObject1->GetNetworkObjectType() == Cure::NETWORK_OBJECT_REMOTE_CONTROLLED)
	{
		if (!mMaster->IsLocalObject(pObject1->GetInstanceId()) &&
			pObject1->GetImpact(GetPhysicsManager()->GetGravity(), pForce, pTorque) >= 0.5f)
		{
			if (pObject1->QueryResendTime(1.0, false))
			{
				GetNetworkAgent()->SendNumberMessage(false, GetNetworkClient()->GetSocket(),
					Cure::MessageNumber::INFO_REQUEST_LOAN, pObject1->GetInstanceId(), 0, 0);
				log_adebug("Sending loan request to server.");
			}
		}
	}
}

bool GameClientSlaveManager::OnPhysicsSend(Cure::ContextObject*)
{
	return (true);	// Say true to drop us from sender list.
}

bool GameClientSlaveManager::OnAttributeSend(Cure::ContextObject* pObject)
{
	mLog.AInfo("Sending attribute(s) for a context object...");
	typedef Cure::ContextObject::AttributeArray AttributeArray;
	const AttributeArray& lAttributes = pObject->GetAttributes();
	AttributeArray::const_iterator x = lAttributes.begin();
	for (; x != lAttributes.end(); ++x)
	{
		Cure::ContextObjectAttribute* lAttribute = *x;
		const int lSendSize = lAttribute->QuerySend();
		if (lSendSize > 0)
		{
			assert(lAttribute->GetNetworkType() == Cure::ContextObjectAttribute::TYPE_BOTH ||
				lAttribute->GetNetworkType() == Cure::ContextObjectAttribute::TYPE_BOTH_BROADCAST);
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

void GameClientSlaveManager::OnAlarm(int pAlarmId, Cure::ContextObject* pObject, void*)
{
	if (pAlarmId == Cure::ContextManager::SYSTEM_ALARM_ID)
	{
		assert(IsOwned(pObject->GetInstanceId()));
		mOwnedObjectList.erase(pObject->GetInstanceId());
		pObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
		pObject->DeleteNetworkOutputGhost();
	}
	else
	{
		assert(false);
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



void GameClientSlaveManager::CancelLogin()
{
	CloseLoginGui();
	SetIsQuitting();
}

void GameClientSlaveManager::OnAvatarSelect(UiTbc::Button* pButton)
{
	Cure::UserAccount::AvatarId lAvatarId = pButton->GetName();
	SelectAvatar(lAvatarId);
}

void GameClientSlaveManager::DropAvatar()
{
	mOwnedObjectList.erase(mAvatarId);
	mAvatarId = 0;
	mHadAvatar = false;
}

float GameClientSlaveManager::QuerySetChildishness(Cure::ContextObject* pAvatar) const
{
	const str lName = _T("float_childishness");
	Cure::FloatAttribute* lAttribute = (Cure::FloatAttribute*)pAvatar->GetAttribute(lName);
	if (!lAttribute)
	{
		lAttribute = new Cure::FloatAttribute(pAvatar, lName, 0);
	}
	float lChildishness;
	CURE_RTVAR_GET(lChildishness, =(float), GetVariableScope(), RTVAR_GAME_CHILDISHNESS, 1.0);
	lAttribute->SetValue(lChildishness);
	return lChildishness;
}

Cure::RuntimeVariableScope* GameClientSlaveManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



Cure::NetworkClient* GameClientSlaveManager::GetNetworkClient() const
{
	return ((Cure::NetworkClient*)GetNetworkAgent());
}



void GameClientSlaveManager::UpdateCameraPosition(bool pUpdateMicPosition)
{
	TransformationF lCameraTransform(GetCameraQuaternion(), mCameraPosition);
	mUiManager->SetCameraPosition(lCameraTransform);
	if (pUpdateMicPosition)
	{
		const float lFrameTime = GetTimeManager()->GetNormalFrameTime();
		if (lFrameTime > 1e-4)
		{
			Vector3DF lVelocity = (mCameraPosition-mCameraPreviousPosition) / lFrameTime;
			const float lMicrophoneMaxVelocity = 100.0f;
			if (lVelocity.GetLength() > lMicrophoneMaxVelocity)
			{
				lVelocity.Normalize(lMicrophoneMaxVelocity);
			}
			const float lLerpTime = Math::GetIterateLerpTime(0.9f, lFrameTime);
			mMicrophoneSpeed = Math::Lerp(mMicrophoneSpeed, lVelocity, lLerpTime);
			mUiManager->SetMicrophonePosition(lCameraTransform, mMicrophoneSpeed);
		}
	}
}

QuaternionF GameClientSlaveManager::GetCameraQuaternion() const
{
	const float lTheta = mCameraOrientation.x;
	const float lPhi = mCameraOrientation.y;
	const float lGimbal = mCameraOrientation.z;
	QuaternionF lOrientation;
	lOrientation.SetEulerAngles(lTheta-PIF/2, PIF/2-lPhi, lGimbal);
	return (lOrientation);
}



UiTbc::FontManager::FontId GameClientSlaveManager::SetFontHeight(double pHeight)
{
	const UiTbc::FontManager::FontId lPreviousFontId = mUiManager->GetFontManager()->GetActiveFontId();
	const str lFontName = mUiManager->GetFontManager()->GetActiveFontName();
	mUiManager->GetFontManager()->QueryAddFont(lFontName, pHeight, UiTbc::FontManager::BOLD);
	return (lPreviousFontId);
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
	bool lDebugAxes;
	bool lDebugJoints;
	bool lDebugShapes;
	bool lDrawLocalServer;
	CURE_RTVAR_GET(lDebugAxes, =, GetVariableScope(), RTVAR_DEBUG_3D_ENABLEAXES, false);
	CURE_RTVAR_GET(lDebugJoints, =, GetVariableScope(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	CURE_RTVAR_GET(lDebugShapes, =, GetVariableScope(), RTVAR_DEBUG_3D_ENABLESHAPES, false);
	CURE_RTVAR_GET(lDrawLocalServer, =, GetVariableScope(), RTVAR_DEBUG_3D_DRAWLOCALSERVER, true);
	if (lDebugAxes || lDebugJoints || lDebugShapes)
	{
		ScopeLock lLock(GetTickLock());
		const Cure::ContextManager* lServerContext = GetMaster()->IsLocalServer()? GetMaster()->GetLocalServer()->GetContext() : 0;
		mUiManager->GetRenderer()->ResetClippingRect();
		mUiManager->GetRenderer()->SetClippingRect(mRenderArea);
		mUiManager->GetRenderer()->SetViewport(mRenderArea);
		UpdateCameraPosition(false);
		float lFov;
		CURE_RTVAR_GET(lFov, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
		UpdateFrustum(lFov);

		const Cure::ContextManager* lContext = GetContext();
		const Cure::ContextManager::ContextObjectTable& lObjectTable = lContext->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			UiCure::CppContextObject* lObject;
			if (lDrawLocalServer && lServerContext)
			{
				if (lContext->IsLocalGameObjectId(x->first))
				{
					continue;
				}
				lObject = dynamic_cast<UiCure::CppContextObject*>(lServerContext->GetObject(x->first));
			}
			else
			{
				lObject = dynamic_cast<UiCure::CppContextObject*>(x->second);
			}
			if (!lObject)
			{
				continue;
			}
			if (lDebugAxes)
			{
				lObject->DebugDrawPrimitive(UiCure::CppContextObject::DEBUG_AXES);
			}
			if (lDebugJoints)
			{
				lObject->DebugDrawPrimitive(UiCure::CppContextObject::DEBUG_JOINTS);
			}
			if (lDebugShapes)
			{
				lObject->DebugDrawPrimitive(UiCure::CppContextObject::DEBUG_SHAPES);
			}
		}
	}
}



LOG_CLASS_DEFINE(GAME, GameClientSlaveManager);



}
