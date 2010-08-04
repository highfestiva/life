
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "GameClientSlaveManager.h"
#include "../../Cure/Include/ContextManager.h"
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
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTBC/Include/GUI/UiCustomButton.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFloatingLayout.h"
#include "../LifeApplication.h"
#include "GameClientMasterTicker.h"
#include "MassObject.h"
#include "RoadSignButton.h"
#include "RtVar.h"
#include "UiConsole.h"
#include "Vehicle.h"



namespace Life
{



GameClientSlaveManager::GameClientSlaveManager(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
	const PixelRect& pRenderArea):
	Cure::GameManager(pVariableScope, pResourceManager),
	mMaster(pMaster),
	mUiManager(pUiManager),
	mSlaveIndex(pSlaveIndex),
	mRenderArea(pRenderArea),
	mFirstInitialize(true),
	mIsReset(false),
	mIsResetComplete(false),
	mQuit(false),
	mAvatarId(0),
	mLastSentByteCount(0),
	mPingAttemptCount(0),
	mJustLookingAtAvatars(false),
	mRoadSignIndex(0),
	mLevelId(0),
	mCameraPosition(0, -200, 100),
	//mCameraFollowVelocity(0, 1, 0),
	mCameraUp(0, 0, 1),
	mCameraOrientation(PIF/2, acos(mCameraPosition.z/mCameraPosition.y), 0),
	mCameraTargetXyDistance(20),
	mCameraMaxSpeed(200),
	mCameraPivotSpeed(0),
	mAllowMovementInput(true),
	mOptions(pVariableScope, pSlaveIndex),
	mLoginWindow(0),
	mEnginePlaybackTime(0)
{
	::memset(mEnginePowerShadow, 0, sizeof(mEnginePowerShadow));

	mCameraPivotPosition = mCameraPosition + GetCameraQuaternion() * Vector3DF(0, mCameraTargetXyDistance*3, 0);

	SetNetworkAgent(new Cure::NetworkClient(GetVariableScope()));

	SetConsoleManager(new ClientConsoleManager(this, mUiManager, GetVariableScope(), mRenderArea));
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
	GetConsoleManager()->ExecuteCommand(_T("execute-file -i ")+GetApplicationCommandFilename());
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
	GetResourceManager()->Tick();
	mQuit = true;
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
	bool lMass;
	bool lOutline;
	bool lWireFrame;
	CURE_RTVAR_GET(lMass, =, GetVariableScope(), RTVAR_UI_3D_ENABLEMASSOBJECTS, false);
	CURE_RTVAR_GET(lOutline, =, GetVariableScope(), RTVAR_UI_3D_OUTLINEMODE, false);
	CURE_RTVAR_GET(lWireFrame, =, GetVariableScope(), RTVAR_UI_3D_WIREFRAMEMODE, false);
	SetMassRender(lMass);
	mUiManager->GetRenderer()->EnableOutlineRendering(lOutline);
	mUiManager->GetRenderer()->EnableWireframe(lWireFrame);
	mUiManager->Render(mRenderArea);

	SetMassRender(false);	// Hide mass objects from other cameras.

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
		else if (mLastSendTime.GetTimeDiffF() >= lKeepaliveInterval)
		{
			lForceSendUnsafeClientKeepalive = true;
		}

		// Check if we should send updates.
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
			lObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
			const Cure::ObjectPositionalData* lPositionalData = 0;
			lObject->UpdateFullPosition(lPositionalData);
			if (lPositionalData && lObject->QueryResendTime(0.1f, true))
			{
				if (!lPositionalData->IsSameStructure(mNetworkOutputGhost))
				{
					mNetworkOutputGhost.CopyData(lPositionalData);
				}
				const bool lIsCollisionExpired = mCollisionExpireAlarm.PopExpired(0.6);
				const bool lIsInputExpired = mInputExpireAlarm.PopExpired(0.0);
				const bool lIsPositionExpired = (lIsCollisionExpired || lIsInputExpired);
				if (lIsPositionExpired)
				{
					log_atrace("Position expires.");
				}

				double lResyncOnDiff;
				CURE_RTVAR_GET(lResyncOnDiff, =, GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 100.0);
				if (lForceSendUnsafeClientKeepalive ||
					lIsPositionExpired ||
					lPositionalData->GetScaledDifference(&mNetworkOutputGhost) > lResyncOnDiff)
				{
					mNetworkOutputGhost.CopyData(lPositionalData);
					lSendOk = GetNetworkAgent()->SendObjectFullPosition(GetNetworkClient()->GetSocket(),
						lObject->GetInstanceId(), GetTimeManager()->GetCurrentPhysicsFrame(), mNetworkOutputGhost);
					lIsSent = true;

					CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_DEBUG_NET_SENDPOSCNT, int, +, 1, 0);
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
				mLastUnsafeReceiveTime.GetTimeDiffF() >= lPingInterval)
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

	mConnectUserName = strutil::Encode(pLoginToken.GetName());
	mConnectServerAddress = pServerAddress;
	mDisconnectReason = _T("Connect failed.");
	mIsReset = false;
	GetNetworkClient()->StartConnectLogin(pServerAddress, CURE_RTVAR_SLOW_GET(GetVariableScope(), RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0), pLoginToken);
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
	const bool lMoveAllowed = (pObjectId == mAvatarId || GetContext()->IsLocalGameObjectId(pObjectId) ||
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

void GameClientSlaveManager::AddLocalObjects(std::hash_set<Cure::GameObjectId>& pLocalObjectSet) const
{
	pLocalObjectSet.insert(mAvatarId);
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
	assert(pAspect >= 0 && pAspect < TBC::PhysicsEngine::MAX_CONTROLLER_COUNT);
	if (pAspect < 0 && pAspect >= TBC::PhysicsEngine::MAX_CONTROLLER_COUNT)
	{
		return false;
	}

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
	GetTimeManager()->Clear(0);

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
			str lServerName = _T("0.0.0.0:16650");
			CURE_RTVAR_TRYGET(lServerName, =, Cure::GetSettings(), RTVAR_NETWORK_SERVERADDRESS, lServerName);
			const wstr lUsername = wstrutil::Format(L"User%u", mSlaveIndex);
			wstr lReadablePassword = L"CarPassword";
			const Cure::MangledPassword lPassword(lReadablePassword);
			const Cure::LoginId lLoginToken(lUsername, lPassword);
			RequestLogin(lServerName, lLoginToken);
		}
		else
		{
			mLoginWindow = new ClientLoginView(this, mDisconnectReason);
			mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout());
			mUiManager->GetDesktopWindow()->AddChild(mLoginWindow);
			mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
				mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
			mLoginWindow->GetChild(_T("User"), 0)->SetKeyboardFocus();
		}
	}
}

bool GameClientSlaveManager::InitializeTerrain()
{
	mLevelId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	bool lOk = CreateObject(mLevelId, _T("level_01"), Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
	assert(lOk);
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

			const Options::Vehicle& v = mOptions.GetControl();
			const bool lIsMovingForward = lObject->GetForwardSpeed() > 8.0f;
#define V(dir) v.mControl[Options::Vehicle::CONTROL_##dir]
			const float lForward = V(FORWARD);
			const float lBack = V(BACKWARD);
			const float lBreakAndBack = V(BREAKANDBACK);
			float lPowerFwdRev = lForward - std::max(lBack, lIsMovingForward? 0.0f : lBreakAndBack);
			SetAvatarEnginePower(lObject, 0, lPowerFwdRev, mCameraOrientation.x);
			float lPowerLR = V(RIGHT)-V(LEFT);
			SetAvatarEnginePower(lObject, 1, lPowerLR, mCameraOrientation.x);
			float lPower = V(HANDBREAK) - std::max(V(BREAK), lIsMovingForward? lBreakAndBack : 0.0f);
			if (!SetAvatarEnginePower(lObject, 2, lPower, mCameraOrientation.x) &&
				lBreakAndBack > 0 && Math::IsEpsEqual(lBack, 0.0f, 0.01f))
			{
				// Someone is apparently trying to stop/break, but no engine configured for breaking.
				// Just apply it as a reverse motion.
				lPowerFwdRev = lForward - lBreakAndBack;
				SetAvatarEnginePower(lObject, 0, lPowerFwdRev, mCameraOrientation.x);
			}
			{
				// Children have the possibility of just pressing left/right which will cause a forward
				// motion in the currently used vehicle.
				bool lIsChild;
				CURE_RTVAR_TRYGET(lIsChild, =, GetVariableScope(), RTVAR_GAME_ISCHILD, false);
				if (lIsChild && Math::IsEpsEqual(lPowerFwdRev, 0.0f, 0.05f) && !Math::IsEpsEqual(lPowerLR, 0.0f, 0.05f))
				{
					TBC::PhysicsEngine* lEngine = lObject->GetPhysics()->GetEngine(0);
					if (lEngine)
					{
						const float lIntensity = lEngine->GetIntensity();
						lEngine->SetValue(0, Math::Clamp(10.0f*(0.2f-lIntensity), 0.0f, 1.0f), mCameraOrientation.x);
					}
				}
			}
			lPower = V(UP)-V(DOWN);
			SetAvatarEnginePower(lObject, 3, lPower, mCameraOrientation.x);
			lPower = V(FORWARD3D) - V(BACKWARD3D);
			SetAvatarEnginePower(lObject, 4, lPower, mCameraOrientation.x);
			lPower = V(LEFT3D) - V(RIGHT3D);
			SetAvatarEnginePower(lObject, 5, lPower, mCameraOrientation.x);
			lPower = V(UP3D) - V(DOWN3D);
			SetAvatarEnginePower(lObject, 6, lPower, mCameraOrientation.x);
			const float lSteeringChange = mLastSteering-v;
			if (lSteeringChange > 0.5f)
			{
				mInputExpireAlarm.Set();
			}
			else if (!Math::IsEpsEqual(lSteeringChange, 0.0f, 0.01f))
			{
				mInputExpireAlarm.Push(0.1f);
			}
			mLastSteering = v;
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

	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float lPhysicsTime = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (lPhysicsTime < 1e-5)
	{
		return;
	}

	// TODO: remove camera hack (camera position should be context object controlled).
	mCameraPreviousPosition = mCameraPosition;
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		// Target position is <cam> distance from the avatar along a straight line
		// (in the XY plane) to where the camera currently is.
		mCameraPivotPosition = lObject->GetPosition();
		mCameraPivotSpeed = Math::Lerp(mCameraPivotSpeed, lObject->GetVelocity().GetLength(), 0.1f);

		UpdateMassObjects(mCameraPivotPosition);
	}
	const Vector3DF lPivotXyPosition(mCameraPivotPosition.x, mCameraPivotPosition.y, mCameraPosition.z);
	Vector3DF lTargetCameraPosition(mCameraPosition-lPivotXyPosition);
	const float lCurrentCameraXyDistance = lTargetCameraPosition.GetLength();
	float lRotationFactor;
	CURE_RTVAR_GET(lRotationFactor, =(float), GetVariableScope(), RTVAR_UI_3D_CAMROTATE, 0.0);
	QuaternionF lRotation;
	lRotation.RotateAroundOwnZ(lRotationFactor * lPhysicsTime);
	lTargetCameraPosition = lRotation * lTargetCameraPosition;
	const float lSpeedDependantCameraXyDistance = mCameraTargetXyDistance + mCameraTargetXyDistance*mCameraPivotSpeed*0.03f;
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

	if (lTargetCameraPosition.z < -20)
	{
		lTargetCameraPosition.z = -20.0f;
	}
	else if (lTargetCameraPosition.z > 200)
	{
		lTargetCameraPosition.z = 200.0f;
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
}

bool GameClientSlaveManager::UpdateMassObjects(const Vector3DF& pPosition)
{
	bool lOk = true;

	if (mMassObjectArray.empty())
	{
		if (lOk)
		{
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			lOk = CreateObject(lMassObjectId, _T("flower"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
		}
		if (lOk)
		{
			Cure::GameObjectId lMassObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
			mMassObjectArray.push_back(lMassObjectId);
			lOk = CreateObject(lMassObjectId, _T("bush_01"), Cure::NETWORK_OBJECT_LOCAL_ONLY, 0);
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
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		const int lStepCount = GetTimeManager()->GetAffordedPhysicsStepCount();
		const float lPhysicsFrameTime = GetTimeManager()->GetAffordedPhysicsStepTime();
		mNetworkOutputGhost.GhostStep(lStepCount, lPhysicsFrameTime);
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
				CURE_RTVAR_OVERRIDE(GetVariableScope(), RTVAR_LOGIN_USERNAME, mConnectUserName);
				CURE_RTVAR_SET(GetVariableScope(), RTVAR_NETWORK_SERVERADDRESS, mConnectServerAddress);
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
						str lTextureId = strutil::Format(_T("Data/road_sign_%s.png"), lAvatarId.c_str());
						if (!DiskFile::Exists(lTextureId))
						{
							lTextureId = _T("Data/road_sign_car.png");
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
			GetContext()->DeleteObject(lMessageDeleteObject->GetObjectId());
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
			CURE_RTVAR_INTERNAL_ARITHMETIC(GetVariableScope(), RTVAR_DEBUG_NET_RECVPOSCNT, int, +, 1, 0);
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
			GetTimeManager()->SetCurrentPhysicsFrame(pInteger);
		}
		break;
		case Cure::MessageNumber::INFO_ADJUST_TIME:
		{
			log_atrace("Adjusting time.");
			GetTimeManager()->SetPhysicsSpeedAdjustment(pFloat, pInteger);
		}
		break;
		case Cure::MessageNumber::INFO_PONG:
		{
			if (GetNetworkClient()->GetSocket())
			{
				mPingAttemptCount = 0;
				const float lPingTime = GetTimeManager()->ConvertPhysicsFramesToSeconds(GetTimeManager()->GetCurrentPhysicsFrameDelta(pInteger));
				const float lServerStriveTime = GetTimeManager()->ConvertPhysicsFramesToSeconds((int)pFloat)*2;
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
			mLog.Infof(_T("Got control over avatar with ID %i."), pInteger);
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
	bool lMassObject = false;
	int lInstanceCount = 600;
	float lSide = 170;
	if (pClassId.find(_T("flower")) != str::npos || pClassId.find(_T("bush_01")) != str::npos)
	{
		lMassObject = true;
		if (pClassId.find(_T("bush_01")) != str::npos)
		{
			lInstanceCount = 150;
			lSide = 290;
		}
	}
	if (lMassObject)
	{
		Cure::ContextObject* lLevel = GetContext()->GetObject(mLevelId);
		assert(lLevel);
		if (lLevel)
		{
			const TBC::PhysicsManager::BodyID lTerrainBodyId = lLevel->GetPhysics()->GetBoneGeometry(0)->GetBodyId();
			return new MassObject(GetResourceManager(), pClassId, mUiManager, lTerrainBodyId, lInstanceCount, lSide);
		}
		return 0;
	}
	Cure::CppContextObject* lObject = new Vehicle(GetResourceManager(), pClassId, mUiManager);
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
			log_volatile(mLog.Debugf(_T("Loaded object %s."), pObject->GetClassId().c_str()));
		}
	}
	else
	{
		mLog.Errorf(_T("Could not load object of type %s."), pObject->GetClassId().c_str());
		assert(false);
		delete (pObject);
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
	const int lDeltaFrames = GetConstTimeManager()->GetPhysicsFrameDelta(pFrameIndex, lLastSetFrameIndex);
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

void GameClientSlaveManager::OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	if (pObject2 && pObject1 != pObject2 && pObject1->GetInstanceId() == mAvatarId &&
		pObject1->GetNetworkObjectType() != Cure::NETWORK_OBJECT_LOCAL_ONLY &&
		pObject2->GetMass() > 0)
	{
		if (IsHighImpact(12.0f, pObject1, pForce, pTorque))
		{
			pObject1->QueryResendTime(0, false);
		}
		mCollisionExpireAlarm.Set();
	}
}

void GameClientSlaveManager::OnStopped(Cure::ContextObject* pObject, TBC::PhysicsManager::BodyID)
{
	(void)pObject;
	log_volatile(mLog.Debugf(_T("Object %u/%s stopped."), pObject->GetInstanceId(), pObject->GetClassId().c_str()));
}

bool GameClientSlaveManager::OnPhysicsSend(Cure::ContextObject*)
{
	return (true);	// Say true to drop us from sender list.
}

bool GameClientSlaveManager::IsConnectAuthorized()
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
	mAvatarId = 0;

	Cure::UserAccount::AvatarId lAvatarId = pButton->GetName();
	log_volatile(mLog.Debugf(_T("Clicked avatar %s."), lAvatarId.c_str()));
	Cure::Packet* lPacket = GetNetworkAgent()->GetPacketFactory()->Allocate();
	GetNetworkAgent()->SendStatusMessage(GetNetworkClient()->GetSocket(), 0, Cure::REMOTE_OK,
		Cure::MessageStatus::INFO_AVATAR, wstrutil::Encode(lAvatarId), lPacket);
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);

	SetRoadSignsVisible(false);
	mAvatarSelectTime.ClearTimeDiff();
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
			const float lLerpTime = Math::GetIterateLerpTime(0.1f, lFrameTime);
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
	CURE_RTVAR_GET(lDebugAxes, =, GetVariableScope(), RTVAR_DEBUG_3D_ENABLEAXES, false);
	CURE_RTVAR_GET(lDebugJoints, =, GetVariableScope(), RTVAR_DEBUG_3D_ENABLEJOINTS, false);
	CURE_RTVAR_GET(lDebugShapes, =, GetVariableScope(), RTVAR_DEBUG_3D_ENABLESHAPES, false);
	if (lDebugAxes || lDebugJoints || lDebugShapes)
	{
		ScopeLock lLock(GetTickLock());
		mUiManager->GetRenderer()->ResetClippingRect();
		mUiManager->GetRenderer()->SetClippingRect(mRenderArea);
		mUiManager->GetRenderer()->SetViewport(mRenderArea);
		UpdateCameraPosition(false);
		float lFov;
		CURE_RTVAR_GET(lFov, =(float), GetVariableScope(), RTVAR_UI_3D_FOV, 45.0);
		UpdateFrustum(lFov);

		const Cure::ContextManager::ContextObjectTable& lObjectTable = GetContext()->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			UiCure::CppContextObject* lObject = dynamic_cast<UiCure::CppContextObject*>(x->second);
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
