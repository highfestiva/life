
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/ResourceManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Network.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/StringUtility.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/Timer.h"
#include "../../UiCure/Include/UiCppContextObject.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiCure/Include/UiRuntimeVariableName.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "GameClientMasterTicker.h"
#include "GameClientSlaveManager.h"



namespace Life
{



GameClientSlaveManager::GameClientSlaveManager(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
	Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
	const Lepra::PixelRect& pRenderArea):
	Cure::GameManager(pVariableScope, pResourceManager, false),
	mMaster(pMaster),
	mUiManager(pUiManager),
	mSlaveIndex(pSlaveIndex),
	mRenderArea(pRenderArea),
	mFirstInitialize(true),
	mIsReset(false),
	mIsResetComplete(false),
	mQuit(false),
	mAvatarId(0),
	mLastUnsafeSentByteCount(0),
	mPingAttemptCount(0),
	mAllowMovementInput(true),
	mMoveKeys(0),
	mLoginWindow(0)
{
	SetNetworkAgent(new Cure::NetworkClient(pVariableScope));

	SetConsoleManager(new ClientConsoleManager(this, mUiManager, GetVariableScope(), mRenderArea));

	switch (pSlaveIndex)
	{
		case 0:
		{
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.ConsoleToggle", UiLepra::InputManager::KEY_PARAGRAPH);

			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Forward",	UiLepra::InputManager::KEY_W);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Backward", UiLepra::InputManager::KEY_S);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Left", UiLepra::InputManager::KEY_A);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Right", UiLepra::InputManager::KEY_D);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Up", UiLepra::InputManager::KEY_E);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Down", UiLepra::InputManager::KEY_Q);
		}
		break;
		case 1:
		{
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.ConsoleToggle", UiLepra::InputManager::KEY_SCROLL_LOCK);

			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Forward", UiLepra::InputManager::KEY_UP);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Backward", UiLepra::InputManager::KEY_DOWN);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Left", UiLepra::InputManager::KEY_LEFT);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Right", UiLepra::InputManager::KEY_RIGHT);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Up", UiLepra::InputManager::KEY_NUMPAD_1);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Down", UiLepra::InputManager::KEY_NUMPAD_0);
		}
		break;
		case 2:
		{
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.ConsoleToggle", UiLepra::InputManager::KEY_F12);

			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Forward", UiLepra::InputManager::KEY_K);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Backward", UiLepra::InputManager::KEY_COMMA);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Left", UiLepra::InputManager::KEY_M);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Right", UiLepra::InputManager::KEY_DOT);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Up", UiLepra::InputManager::KEY_L);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Down", UiLepra::InputManager::KEY_J);
		}
		break;
		case 3:
		{
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.ConsoleToggle", UiLepra::InputManager::KEY_PAUSE);

			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Forward", UiLepra::InputManager::KEY_NUMPAD_8);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Backward", UiLepra::InputManager::KEY_NUMPAD_5);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Left", UiLepra::InputManager::KEY_NUMPAD_4);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Right", UiLepra::InputManager::KEY_NUMPAD_6);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Up", UiLepra::InputManager::KEY_NUMPAD_9);
			CURE_RTVAR_SET(GetVariableScope(), "Control.Key.Down", UiLepra::InputManager::KEY_NUMPAD_7);
		}
		break;
	}
}

GameClientSlaveManager::~GameClientSlaveManager()
{
	while (!mLoadingContextSet.empty())
	{
		UserGfxContextObjectInfoResource* lResource = *mLoadingContextSet.begin();
		mLoadingContextSet.erase(mLoadingContextSet.begin());
		delete (lResource);
	}

	Close();

	mMaster = 0;

	GetConsoleManager()->ExecuteCommand(_T("save-application-config-file -no ")+GetApplicationCommandFilename());
	GetConsoleManager()->ExecuteCommand(_T("save-system-config-file -no ")+GetSystemCommandFilename());

	GetConsoleManager()->Join();
}

void GameClientSlaveManager::LoadSettings()
{
	GetConsoleManager()->ExecuteCommand(_T("execute-file ")+GetSystemCommandFilename());
	GetConsoleManager()->ExecuteCommand(_T("execute-file ")+GetApplicationCommandFilename());
}

void GameClientSlaveManager::SetRenderArea(const Lepra::PixelRect& pRenderArea)
{
	mRenderArea = pRenderArea;
	if (mLoginWindow)
	{
		mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
			mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
	}
	((ClientConsoleManager*)GetConsoleManager())->SetRenderArea(pRenderArea);

	// Register a local FOV variable.
	Cure::RuntimeVariableScope* lParent = GetVariableScope()->LockParentScope(0);
	double lFov = 30+15*pRenderArea.GetWidth()/pRenderArea.GetHeight();
	CURE_RTVAR_SET(GetVariableScope(), RTVAR_UI_3D_FOV, lFov);
	GetVariableScope()->LockParentScope(lParent);
}

bool GameClientSlaveManager::Open()
{
	Close();
	Lepra::ScopeLock lLock(GetTickLock());
	bool lOk = Reset();
	if (lOk)
	{
		lOk = GetConsoleManager()->Start();
	}
	return (lOk);
}

void GameClientSlaveManager::Close()
{
	Lepra::ScopeLock lLock(GetTickLock());

	// Drop all physics and renderer objects.
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
	GetResourceManager()->Tick();
	mQuit = true;
}



GameClientMasterTicker* GameClientSlaveManager::GetMaster() const
{
	return (mMaster);
}



bool GameClientSlaveManager::Render()
{
	Lepra::ScopeLock lLock(GetTickLock());
	mUiManager->SetCameraPosition(mCameraPosition.x, mCameraPosition.y, mCameraPosition.z);
	mUiManager->SetCameraOrientation(mCameraOrientation.x, mCameraOrientation.y, mCameraOrientation.z);

	double lFOV = CURE_RTVAR_GET(GetVariableScope(), RTVAR_UI_3D_FOV, 90.0);
	double lClipNear = CURE_RTVAR_GET(GetVariableScope(), RTVAR_UI_3D_CLIPNEAR, 0.1);
	double lClipFar = CURE_RTVAR_GET(GetVariableScope(), RTVAR_UI_3D_CLIPFAR, 1000.0);
	mUiManager->GetRenderer()->SetViewFrustum((float)lFOV, (float)lClipNear, (float)lClipFar);

	mUiManager->Render(mRenderArea);
	return (true);
}

bool GameClientSlaveManager::EndTick()
{
	DrawAsyncDebugInfo();

	Lepra::ScopeLock lLock(GetTickLock());
	bool lOk = Parent::EndTick();
	if (lOk)
	{
		DrawSyncDebugInfo();
		TickUiUpdate();
	}
	return (lOk);
}



void GameClientSlaveManager::ToggleConsole()
{
	mAllowMovementInput = !((ClientConsoleManager*)GetConsoleManager())->Toggle();
}



void GameClientSlaveManager::RequestLogin(const Lepra::String& pServerAddress, const Cure::LoginId& pLoginToken)
{
	//mMaster->RemoveSlave(this);

	Lepra::ScopeLock lLock(GetTickLock());

	CloseLoginGui();

	mDisconnectReason = _T("Connect failed.");
	mIsReset = false;
	GetNetworkClient()->StartConnectLogin(pServerAddress, CURE_RTVAR_GET(GetVariableScope(), RTVAR_NETWORK_CONNECT_TIMEOUT, 3.0), pLoginToken);
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
			Lepra::Thread::Sleep(0.05);
		}
	}
}

bool GameClientSlaveManager::IsLoggingIn() const
{
	return (GetNetworkClient()->IsConnecting() || GetNetworkClient()->IsLoggingIn());
}



bool GameClientSlaveManager::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
#define K(v)	GetVariableScope()->GetDefaultValue(_T("Control.Key.") _T(v), Cure::RuntimeVariableScope::READ_ONLY, -1)

	if (pKeyCode == K("ConsoleToggle"))
	{
		ToggleConsole();
		return (true);	// This key ends here.
	}

	if (mAllowMovementInput)
	{
		// TODO: remove movement hack (movement should be ContextObject controlled)!
		if (pKeyCode == K("Forward"))		mMoveKeys |= 0x01;
		if (pKeyCode == K("Backward"))		mMoveKeys |= 0x02;
		if (pKeyCode == K("Right"))		mMoveKeys |= 0x04;
		if (pKeyCode == K("Left"))		mMoveKeys |= 0x08;
		if (pKeyCode == K("Up"))		mMoveKeys |= 0x10;
		if (pKeyCode == K("Down"))		mMoveKeys |= 0x20;
	}

#undef K

	return (false);
}

bool GameClientSlaveManager::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
#define K(v)	GetVariableScope()->GetDefaultValue(_T("Control.Key.") _T(v), Cure::RuntimeVariableScope::READ_ONLY, -1)

	if (mAllowMovementInput)
	{
		// TODO: remove movement hack (movement should be ContextObject controlled)!
		if (pKeyCode == K("Forward"))		mMoveKeys &= ~0x01;
		if (pKeyCode == K("Backward"))		mMoveKeys &= ~0x02;
		if (pKeyCode == K("Right"))		mMoveKeys &= ~0x04;
		if (pKeyCode == K("Left"))		mMoveKeys &= ~0x08;
		if (pKeyCode == K("Up"))		mMoveKeys &= ~0x10;
		if (pKeyCode == K("Down"))		mMoveKeys &= ~0x20;
	}

#undef K

	return (false);
}

void GameClientSlaveManager::OnInput(UiLepra::InputElement*)
{
}



int GameClientSlaveManager::GetSlaveIndex() const
{
	return (mSlaveIndex);
}



Lepra::String GameClientSlaveManager::GetName() const
{
	return (_T("Client")+Lepra::StringUtility::IntToString(mSlaveIndex, 10));
}

Lepra::String GameClientSlaveManager::GetSystemCommandFilename() const
{
	return (Lepra::SystemManager::GetLoginName()+_T("ClientSystem") _TEXT_ALTERNATIVE("", L"U") _T(".LifeBatch"));
}

Lepra::String GameClientSlaveManager::GetApplicationCommandFilename() const
{
	return (Lepra::SystemManager::GetLoginName()+GetName()+_T("Application") _TEXT_ALTERNATIVE("", L"U") _T(".LifeBatch"));
}



bool GameClientSlaveManager::Reset()	// Run when disconnected. Removes all objects and displays login GUI.
{
	Lepra::ScopeLock lLock(GetTickLock());

	mIsReset = true;

	GetNetworkClient()->Disconnect(true);
	mPingAttemptCount = 0;
	GetTimeManager()->Clear(0);

	if (!mLoginWindow && !GetNetworkClient()->IsActive())
	{
		mLoginWindow = new ClientLoginView(mUiManager->GetPainter(), this, mDisconnectReason);
		mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout());
		mUiManager->GetDesktopWindow()->AddChild(mLoginWindow);
		mLoginWindow->SetPos(mRenderArea.GetCenterX()-mLoginWindow->GetSize().x/2,
			mRenderArea.GetCenterY()-mLoginWindow->GetSize().y/2);
		mLoginWindow->GetChild(_T("User"), 0)->SetKeyboardFocus();
	}

	mCameraPosition.Set(0, -100, 255);
	mCameraOrientation.Set(Lepra::PIF/2.0f, Lepra::PIF/2.0f, 0);

	mObjectFrameIndexMap.clear();

	GetContext()->ClearObjects();
	bool lOk = InitializeTerrain();

	mIsResetComplete = true;

	return (lOk);
}

bool GameClientSlaveManager::InitializeTerrain()
{
	Cure::GameObjectId lGameObjectId = GetContext()->AllocateGameObjectId(Cure::NETWORK_OBJECT_LOCAL_ONLY);
	bool lOk = CreateObject(lGameObjectId, Lepra::StringUtility::IntToString(GetSlaveIndex(), 10)+_T("/:ground_002"),
		Cure::NETWORK_OBJECT_LOCAL_ONLY);
	return (lOk);
}

void GameClientSlaveManager::CloseLoginGui()
{
	if (mLoginWindow)
	{
		Lepra::ScopeLock lLock(GetTickLock());
		mUiManager->GetDesktopWindow()->RemoveChild(mLoginWindow, 0);
		delete (mLoginWindow);
		mLoginWindow = 0;
	}
}



void GameClientSlaveManager::TickInput()
{
	TickNetworkInput();
	TickUiInput();
}



void GameClientSlaveManager::TickUiInput()
{
	// TODO: remove movement hack!
	int lPhysicsStepCount = GetTimeManager()->GetCurrentPhysicsStepCount();
	if (lPhysicsStepCount > 0)
	{
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
#define FWD_BACK(lo_bit)	((mMoveKeys&lo_bit)? 1.0f : 0.0f) + ((mMoveKeys&(lo_bit<<1))? -1.0f : 0.0f)
			lObject->SetEnginePower(0, FWD_BACK(0x01), mCameraOrientation.x);
			lObject->SetEnginePower(1, FWD_BACK(0x04), mCameraOrientation.x);
			lObject->SetEnginePower(2, FWD_BACK(0x10), mCameraOrientation.x);
		}
	}
}

void GameClientSlaveManager::TickUiUpdate()
{
	((ClientConsoleManager*)GetConsoleManager())->Tick();

	// TODO: update sound position and velocity.

	// TODO: remove camera hack (camera position should be context object controlled).
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		// Target position is <cam> distance from the avatar along a straight line
		// (in the XY plane) to where the camera currently is.
		const Lepra::Vector3DF lAvatarPosition(lObject->GetPosition());
		const Lepra::Vector3DF lAvatarXyPosition(lObject->GetPosition().x, lObject->GetPosition().y, mCameraPosition.z);
		Lepra::Vector3DF lTargetCameraPosition(mCameraPosition);
		const float lTargetCameraXyDistance = 20.0f;
		const float lCurrentCameraXyDistance = lTargetCameraPosition.GetDistance(lAvatarXyPosition);
		lTargetCameraPosition = lAvatarXyPosition + (lTargetCameraPosition-lAvatarXyPosition)*(lTargetCameraXyDistance/lCurrentCameraXyDistance);
		if (lAvatarPosition.z > 255)
		{
			lTargetCameraPosition.z = lAvatarPosition.z;
		}
		else
		{
			lTargetCameraPosition.z = 255.0f;
		}

		// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
		const float lHalfDistanceTime = 0.1f;	// Time it takes to half the distance from where it is now to where it should be.
		float lMovingAveragePart = 0.5f*GetTimeManager()->GetCurrentFrameTime()/lHalfDistanceTime;
		if (lMovingAveragePart > 0.8f)
		{
			lMovingAveragePart = 0.8f;
		}
		mCameraPosition = Lepra::Math::Lerp<Lepra::Vector3DF, float>(mCameraPosition, lTargetCameraPosition, lMovingAveragePart);

		// "Roll" camera towards avatar.
		const float lNewTargetCameraXyDistance = mCameraPosition.GetDistance(lAvatarXyPosition);
		const float lNewTargetCameraDistance = mCameraPosition.GetDistance(lAvatarPosition);
		Lepra::Vector3DF lTargetCameraOrientation;
		lTargetCameraOrientation.Set(::asin((mCameraPosition.x-lAvatarXyPosition.x)/lNewTargetCameraXyDistance) + Lepra::PIF/2,
			::acos((lAvatarPosition.z-mCameraPosition.z)/lNewTargetCameraDistance), 0);
		if (lAvatarXyPosition.y-mCameraPosition.y < 0)
		{
			lTargetCameraOrientation.x = -lTargetCameraOrientation.x;
		}
		float lYawChange = lTargetCameraOrientation.x-mCameraOrientation.x;
		lTargetCameraOrientation.z = -lYawChange;
		Lepra::Math::RangeAngles(mCameraOrientation.x, lTargetCameraOrientation.x);
		Lepra::Math::RangeAngles(mCameraOrientation.y, lTargetCameraOrientation.y);
		Lepra::Math::RangeAngles(mCameraOrientation.z, lTargetCameraOrientation.z);
		mCameraOrientation = Lepra::Math::Lerp<Lepra::Vector3DF, float>(mCameraOrientation, lTargetCameraOrientation, lMovingAveragePart);
	}
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
				const size_t lMessageCount = lPacket->GetMessageCount();
				for (size_t x = 0; x < lMessageCount; ++x)
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
	}
	GetNetworkAgent()->GetPacketFactory()->Release(lPacket);
}

bool GameClientSlaveManager::TickNetworkOutput()
{
	CURE_RTVAR_SET(GetVariableScope(), "Debug.Net.SentPosition", false);

	bool lSendOk = true;
	if (GetNetworkClient()->GetSocket())
	{
		// Check if we should send client keepalive (keepalive is simply a position update).
		bool lForceSendUnsafeClientKeepalive = false;
		if (mLastUnsafeSentByteCount != GetNetworkAgent()->GetSentByteCount(false))
		{
			mLastUnsafeSentByteCount = GetNetworkAgent()->GetSentByteCount(false);
			mLastUnsafeSendTime.ClearTimeDiff();
		}
		else if (mLastUnsafeSendTime.GetTimeDiffF() >= CURE_RTVAR_GET(GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_SENDINTERVAL, 5.0))
		{
			lForceSendUnsafeClientKeepalive = true;
		}
		mLastUnsafeSendTime.UpdateTimer();

		// Check if we should send updates.
		Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
		if (lObject)
		{
			lObject->SetNetworkObjectType(Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED);
			const Cure::ObjectPositionalData* lPositionalData = 0;
			lObject->UpdateFullPosition(lPositionalData);
			if (lPositionalData && !lPositionalData->IsSameStructure(mNetworkOutputGhost))
			{
				mNetworkOutputGhost.CopyData(lPositionalData);
			}
			if (lPositionalData && (lForceSendUnsafeClientKeepalive ||
				lPositionalData->GetScaledDifference(&mNetworkOutputGhost) > CURE_RTVAR_GET(GetVariableScope(), RTVAR_NETPHYS_RESYNCONDIFFGT, 100.0)))
			{
				CURE_RTVAR_SET(GetVariableScope(), "Debug.Net.SentPosition", true);
				mNetworkOutputGhost.CopyData(lPositionalData);
				lSendOk = GetNetworkAgent()->SendObjectFullPosition(GetNetworkClient()->GetSocket(),
					lObject->GetInstanceId(), GetTimeManager()->GetCurrentPhysicsFrame(), mNetworkOutputGhost);
			}
		}

		// Check if we should send server check-up (uses message similar to ping).
		if (lSendOk && !GetNetworkClient()->IsLoggingIn())
		{
			mLastUnsafeReceiveTime.UpdateTimer();
			if (mLastUnsafeReceiveTime.GetTimeDiffF() >= CURE_RTVAR_GET(GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_PINGINTERVAL, 7.0))
			{
				if (++mPingAttemptCount <= CURE_RTVAR_GET(GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_PINGRETRYCOUNT, 4))
				{
					mLastUnsafeReceiveTime.ReduceTimeDiff(CURE_RTVAR_GET(GetVariableScope(), RTVAR_NETWORK_KEEPALIVE_PINGINTERVAL, 7.0));
					log_volatile(mLog.Debugf(_T("%s sending ping."), GetName().c_str()));
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



void GameClientSlaveManager::PhysicsTick()
{
	Cure::ContextObject* lObject = GetContext()->GetObject(mAvatarId);
	if (lObject)
	{
		int lStepCount = GetTimeManager()->GetCurrentPhysicsStepCount();
		float lPhysicsStepTime = (float)GetTimeManager()->ConvertPhysicsFramesToSeconds(lStepCount);
		lObject->StepGhost(mNetworkOutputGhost, lPhysicsStepTime);
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
			mLog.Infof(_T("Got remote status %i with ID %u."), lRemoteStatus, lMessageStatus->GetInteger());
			if (lRemoteStatus != Cure::REMOTE_OK)
			{
				Lepra::UnicodeString lErrorMessage;
				lMessageStatus->GetMessageString(lErrorMessage);
				mDisconnectReason = Lepra::UnicodeStringUtility::ToCurrentCode(lErrorMessage);
				mLog.Warning(mDisconnectReason);
				GetNetworkClient()->Disconnect(false);
			}
			else if (GetNetworkClient()->IsLoggingIn())
			{
				GetNetworkClient()->SetLoginAccountId(lMessageStatus->GetInteger());
				mDisconnectReason.clear();
			}
			else
			{
				Lepra::UnicodeString lChatMessage;
				lMessageStatus->GetMessageString(lChatMessage);
				if (lMessageStatus->GetInteger() == 0)
				{
					lChatMessage = L"ServerAdmin: "+lChatMessage;
				}
				else
				{
					lChatMessage = L"<Player?>: "+lChatMessage;
				}
				mLog.Info(Lepra::UnicodeStringUtility::ToCurrentCode(lChatMessage));
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
			Lepra::UnicodeString lClassId;
			lMessageCreateObject->GetClassId(lClassId);
			assert(lClassId.find(_T(":")) == Lepra::String::npos);
			Lepra::String lObjectClass = Lepra::StringUtility::IntToString(GetSlaveIndex(), 10) +
				_T("/") + Lepra::StringUtility::IntToString(lMessageCreateObject->GetObjectId(), 10) +
				_T(":") + Lepra::UnicodeStringUtility::ToCurrentCode(lClassId);
			CreateObject(lMessageCreateObject->GetObjectId(), lObjectClass, Cure::NETWORK_OBJECT_REMOTE_CONTROLLED);
		}
		break;
		case Cure::MESSAGE_TYPE_DELETE_OBJECT:
		{
			Cure::MessageDeleteObject* lMessageDeleteObject = (Cure::MessageDeleteObject*)pMessage;
			DeleteObject(lMessageDeleteObject->GetObjectId());
		}
		break;
		case Cure::MESSAGE_TYPE_OBJECT_POSITION:
		{
			Cure::MessageObjectPosition* lMessageMovement = (Cure::MessageObjectPosition*)pMessage;
			Cure::GameObjectId lObjectId = lMessageMovement->GetObjectId();
			Lepra::int32 lFrameIndex = lMessageMovement->GetFrameIndex();
			const Cure::ObjectPositionalData& lData = lMessageMovement->GetPositionalData();
			SetMovement(lObjectId, lFrameIndex, lData);
		}
		break;
		default:
		{
			mLog.AError("Got bad message type from server.");
		}
		break;
	}
}

void GameClientSlaveManager::ProcessNumber(Cure::MessageNumber::InfoType pType, Lepra::int32 pInteger, Lepra::float32 pFloat)
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
				float lPingTime = GetTimeManager()->ConvertPhysicsFramesToSeconds(GetTimeManager()->GetCurrentPhysicsFrame()-pInteger);
				float lServerStriveTime = GetTimeManager()->ConvertPhysicsFramesToSeconds((int)pFloat)*2;
				mLog.Infof(_T("Pong: this=%ss, server sim strives to be x2=%ss ahead, (self=%s)."),
					Lepra::Number::ConvertToPostfixNumber(lPingTime, 2).c_str(),
					Lepra::Number::ConvertToPostfixNumber(lServerStriveTime, 2).c_str(),
					GetNetworkClient()->GetSocket()->GetLocalAddress().GetAsString().c_str());
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

bool GameClientSlaveManager::CreateObject(Cure::GameObjectId pInstanceId, const Lepra::String& pClassId, Cure::NetworkObjectType pNetworkType)
{
	Cure::ContextObject* lPreviousObject = GetContext()->GetObject(pInstanceId);
	//assert(!lPreviousObject);
	if (!lPreviousObject)
	{
		if (!GetResourceManager()->IsCreated(pClassId))
		{
			mLog.Infof(_T("%s creating context object %s."), GetName().c_str(), pClassId.c_str());
			ContextObjectInfo lObjectInfo(GetContext(), 0, pInstanceId, pNetworkType);
			UserGfxContextObjectInfoResource* lResource = new UserGfxContextObjectInfoResource(lObjectInfo);
			mLoadingContextSet.insert(lResource);
			lResource->GetExtraData().mLoadingResource = lResource;
			lResource->Load(GetResourceManager(), pClassId, UserGfxContextObjectInfoResource::TypeLoadCallback(
				this, &GameClientSlaveManager::ContextLoadCallback));
		}
		else
		{
			log_volatile(mLog.Debugf(_T("Context object %s currently loading, skipping doubles."), pClassId.c_str()));
		}
	}
	else
	{
		assert(lPreviousObject->GetClassId() == pClassId);
	}
	return (true);
}

bool GameClientSlaveManager::DeleteObject(Cure::GameObjectId pInstanceId)
{
	bool lOk = false;
	Cure::ContextObject* lObject = GetContext()->GetObject(pInstanceId);
	if (lObject)
	{
		log_volatile(mLog.Debugf(_T("Deleting context object %i."), pInstanceId));
		delete (lObject);
	}
	else
	{
		log_volatile(mLog.Debugf(_T("Could not delete context object %i, since not found."), pInstanceId));
	}
	return (lOk);
}

void GameClientSlaveManager::SetMovement(Cure::GameObjectId pInstanceId, Lepra::int32 pFrameIndex, const Cure::ObjectPositionalData& pData)
{
	ObjectFrameIndexMap::iterator x = mObjectFrameIndexMap.find(pInstanceId);
	if (x == mObjectFrameIndexMap.end())
	{
		mObjectFrameIndexMap.insert(ObjectFrameIndexPair(pInstanceId, pFrameIndex-1));
		x = mObjectFrameIndexMap.find(pInstanceId);
	}
	int lLastSetFrameIndex = x->second;	// Last set frame index.
	if (pFrameIndex-lLastSetFrameIndex >= 0)
	{
		x->second = pFrameIndex;
		//Lepra::String s = Lepra::StringUtility::Format(_T("client %i at frame %i"), pClientIndex, pFrameIndex);
		//log_debug(_T("Client set pos of other client"), s);
		Cure::ContextObject* lObject = GetContext()->GetObject(pInstanceId);
		if (lObject)
		{
			lObject->SetFullPosition(pData);
			//lObject->OnPhysicsTick();	// Reflect physics changes on graphics movement.
		}
		else
		{
			mLog.Warningf(_T("%s could not set position for object %i."), GetName().c_str(), pInstanceId);
		}
	}
	else
	{
		log_volatile(mLog.Debugf(_T("Throwing away out-of-order positional data (%i frames late)."), lLastSetFrameIndex-pFrameIndex));
	}
}

void GameClientSlaveManager::OnCollision(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque,
	Cure::ContextObject* pObject1, Cure::ContextObject* pObject2)
{
	pForce;
	pTorque;
	pObject1;
	pObject2;
	/*if (pObject->GetNetworkObjectType() == Cure::NETWORK_OBJECT_LOCALLY_CONTROLLED)
	{
		log_adebug("Collision.");
	}*/
}

void GameClientSlaveManager::OnStopped(Cure::ContextObject*)
{
}



void GameClientSlaveManager::CancelLogin()
{
	CloseLoginGui();
	SetIsQuitting();
}

Cure::RuntimeVariableScope* GameClientSlaveManager::GetVariableScope() const
{
	return (Parent::GetVariableScope());
}



Cure::NetworkClient* GameClientSlaveManager::GetNetworkClient() const
{
	return ((Cure::NetworkClient*)GetNetworkAgent());
}



void GameClientSlaveManager::ContextLoadCallback(UserGfxContextObjectInfoResource* pResource)
{
	assert(mLoadingContextSet.find(pResource) != mLoadingContextSet.end());
	mLoadingContextSet.erase(pResource);
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		Cure::ContextObject* lObject = pResource->GetRamData();
		pResource->ReleaseRamResource();
		GetContext()->AddObject(lObject);
		GetContext()->EnableTickCallback(lObject);
	}
	else
	{
		mLog.AError("An error arised when loading data for a context object.");
	}
	GetResourceManager()->SafeRelease(pResource);
}



void GameClientSlaveManager::DrawAsyncDebugInfo()
{
	if (CURE_RTVAR_GET(GetVariableScope(), "Debug.Net.SentPosition", false))
	{
		mUiManager->GetPainter()->SetColor(Lepra::Color(255, 0, 0), 0);
		const int x = mRenderArea.mLeft+10;
		const int y = mRenderArea.mTop+10;
		const int s = 10;
		mUiManager->GetPainter()->FillRect(x, y, x+s, y+s);
	}
}

void GameClientSlaveManager::DrawSyncDebugInfo()
{
	if (CURE_RTVAR_GETSET(GetVariableScope(), "Ui.3D.EnableAxis", false))
	{
		Lepra::ScopeLock lLock(GetTickLock());
		mUiManager->GetRenderer()->ResetClippingRect();
		mUiManager->GetRenderer()->SetClippingRect(mRenderArea);
		mUiManager->GetRenderer()->SetViewport(mRenderArea);
		mUiManager->SetCameraPosition(mCameraPosition.x, mCameraPosition.y, mCameraPosition.z);
		mUiManager->SetCameraOrientation(mCameraOrientation.x, mCameraOrientation.y, mCameraOrientation.z);
		double lFOV = CURE_RTVAR_GET(GetVariableScope(), RTVAR_UI_3D_FOV, 90.0);
		double lClipNear = CURE_RTVAR_GET(GetVariableScope(), RTVAR_UI_3D_CLIPNEAR, 0.1);
		double lClipFar = CURE_RTVAR_GET(GetVariableScope(), RTVAR_UI_3D_CLIPFAR, 1000.0);
		mUiManager->GetRenderer()->SetViewFrustum((float)lFOV, (float)lClipNear, (float)lClipFar);

		const Cure::ContextManager::ContextObjectTable& lObjectTable = GetContext()->GetObjectTable();
		Cure::ContextManager::ContextObjectTable::const_iterator x = lObjectTable.begin();
		for (; x != lObjectTable.end(); ++x)
		{
			UiCure::CppContextObject* lObject = (UiCure::CppContextObject*)x->second;	// Not very good to cast to a Cpp...
			lObject->DebugDrawAxis();
		}
	}
}



LOG_CLASS_DEFINE(GAME, GameClientSlaveManager);



}
