
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/NetworkClient.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/PositionalData.h"
#include "../../Lepra/Include/Alarm.h"
#include "../../Lepra/Include/Timer.h"
#include "../../UiTbc/Include/GUI/UiTextArea.h"
#include "../../UiTbc/Include/GUI/UiConsoleLogListener.h"
#include "ClientConsoleManager.h"
#include "ClientLoginView.h"
#include "ClientOptionsManager.h"
#include "InputObserver.h"



namespace Life
{



class GameClientMasterTicker;



class GameClientSlaveManager: public Cure::GameManager, public InputObserver, private ClientLoginObserver
{
	typedef Cure::GameManager Parent;
public:
	GameClientSlaveManager(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
		const Lepra::PixelRect& pRenderArea);
	virtual ~GameClientSlaveManager();
	void LoadSettings();
	void SetRenderArea(const Lepra::PixelRect& pRenderArea);
	bool Open();
	void Close();
	bool IsQuitting() const;
	void SetIsQuitting();

	GameClientMasterTicker* GetMaster() const;

	bool Render();
	bool EndTick();

	void ToggleConsole();

	void RequestLogin(const Lepra::String& pServerAddress, const Cure::LoginId& pLoginToken);
	void Logout();
	bool IsLoggingIn() const;
	bool IsUiMoveForbidden(Cure::GameObjectId pObjectId) const;
	void AddLocalObjects(std::set<Cure::GameObjectId>& pLocalObjectSet) const;

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void OnInput(UiLepra::InputElement* pElement);

	int GetSlaveIndex() const;

private:
	Lepra::String GetApplicationCommandFilename() const;

	bool Reset();
	bool InitializeTerrain();
	void CloseLoginGui();

	void TickInput();

	void TickUiInput();
	void TickUiUpdate();

	void TickNetworkInput();
	bool TickNetworkOutput();

	void PhysicsTick();

	void ProcessNetworkInputMessage(Cure::Message* pMessage);
	void ProcessNumber(Cure::MessageNumber::InfoType pType, Lepra::int32 pInteger, Lepra::float32 pFloat);
	bool CreateObject(Cure::GameObjectId pInstanceId, const Lepra::String& pClassId, Cure::NetworkObjectType pNetworkType);
	Cure::ContextObject* CreateContextObject(const Lepra::String& pClassId) const;
	void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void SetMovement(Cure::GameObjectId pInstanceId, Lepra::int32 pFrameIndex, Cure::ObjectPositionalData& pData);
	void OnCollision(const Lepra::Vector3DF& pForce, const Lepra::Vector3DF& pTorque,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	void OnStopped(Cure::ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId);
	bool OnPhysicsSend(Cure::ContextObject* pObject);
	bool IsConnectAuthorized();
	void SendAttach(Cure::ContextObject*, unsigned, Cure::ContextObject*, unsigned);
	void SendDetach(Cure::ContextObject*, Cure::ContextObject*);
	void AttachObjects(Cure::GameObjectId pObject1Id, unsigned pBody1Id, Cure::GameObjectId pObject2Id, unsigned pBody2Id);
	void DetachObjects(Cure::GameObjectId pObject1Id, Cure::GameObjectId pObject2Id);

	void CancelLogin();
	Cure::RuntimeVariableScope* GetVariableScope() const;

	Cure::NetworkClient* GetNetworkClient() const;

	void UpdateCameraPosition();

	void DrawAsyncDebugInfo();
	void DrawDebugStaple(int pIndex, int pHeight, const Lepra::Color& pColor);
	void DrawSyncDebugInfo();

	typedef std::hash_map<Cure::GameObjectId, int> ObjectFrameIndexMap;
	typedef std::pair<Cure::GameObjectId, int> ObjectFrameIndexPair;

	GameClientMasterTicker* mMaster;
	UiCure::GameUiManager* mUiManager;
	const int mSlaveIndex;
	Lepra::PixelRect mRenderArea;

	ObjectFrameIndexMap mObjectFrameIndexMap;

	bool mFirstInitialize;
	bool mIsReset;
	bool mIsResetComplete;
	bool mQuit;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	Lepra::uint64 mLastUnsafeSentByteCount;
	Lepra::Timer mLastUnsafeSendTime;
	int mPingAttemptCount;
	Lepra::Timer mLastUnsafeReceiveTime;
	Lepra::Alarm mCollisionExpireAlarm;
	Lepra::Alarm mInputExpireAlarm;

	Lepra::Vector3DF mCameraPosition;		// TODO: remove hack (should context object controlled)!
	Lepra::Vector3DF mCameraPreviousPosition;	// TODO: remove hack (should context object controlled)!
	Lepra::Vector3DF mCameraOrientation;		// TODO: remove hack (should context object controlled)!
	Cure::ObjectPositionalData mNetworkOutputGhost;	// TODO: remove hack (should be one per controllable object)!
	bool mAllowMovementInput;
	Options::ClientOptionsManager mOptions;
	UiTbc::Window* mLoginWindow;
	Lepra::String mConnectUserName;
	Lepra::String mConnectServerAddress;
	Lepra::String mDisconnectReason;

	LOG_CLASS_DECLARE();
};



}
