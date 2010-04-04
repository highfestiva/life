
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/PositionalData.h"
#include "../../Lepra/Include/Alarm.h"
#include "../../Lepra/Include/Timer.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "../GameManager.h"
#include "ClientConsoleManager.h"
#include "ClientLoginView.h"
#include "ClientOptionsManager.h"
#include "InputObserver.h"



namespace UiTbc
{
class CustomButton;
}
namespace Cure
{
class NetworkClient;
}



namespace Life
{



class GameClientMasterTicker;
class RoadSignButton;



class GameClientSlaveManager: public GameManager, public InputObserver, private ClientLoginObserver
{
	typedef Cure::GameManager Parent;
public:
	GameClientSlaveManager(GameClientMasterTicker* pMaster, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager, int pSlaveIndex,
		const PixelRect& pRenderArea);
	virtual ~GameClientSlaveManager();
	void LoadSettings();
	void SetRenderArea(const PixelRect& pRenderArea);
	bool Open();
	void Close();
	bool IsQuitting() const;
	void SetIsQuitting();

	GameClientMasterTicker* GetMaster() const;

	bool Render();
	bool EndTick();

	void ToggleConsole();

	void RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken);
	void Logout();
	bool IsLoggingIn() const;
	bool IsUiMoveForbidden(Cure::GameObjectId pObjectId) const;
	void AddLocalObjects(std::set<Cure::GameObjectId>& pLocalObjectSet) const;

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void OnInput(UiLepra::InputElement* pElement);

	int GetSlaveIndex() const;

	UiCure::GameUiManager* GetUiManager() const;
	const PixelRect& GetRenderArea() const;
	float UpdateFrustum();

private:
	str GetApplicationCommandFilename() const;

	bool Reset();
	bool InitializeTerrain();
	void CloseLoginGui();
	void ClearRoadSigns();

	void TickInput();

	void TickUiInput();
	void TickUiUpdate();

	void TickNetworkInput();
	bool TickNetworkOutput();

	void PhysicsTick();

	void ProcessNetworkInputMessage(Cure::Message* pMessage);
	void ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);
	bool CreateObject(Cure::GameObjectId pInstanceId, const str& pClassId, Cure::NetworkObjectType pNetworkType,
		TransformationF* pTransform = 0);
	Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void SetMovement(Cure::GameObjectId pInstanceId, int32 pFrameIndex, Cure::ObjectPositionalData& pData);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2);
	void OnStopped(Cure::ContextObject* pObject, TBC::PhysicsManager::BodyID pBodyId);
	bool OnPhysicsSend(Cure::ContextObject* pObject);
	bool IsConnectAuthorized();
	void SendAttach(Cure::ContextObject*, unsigned, Cure::ContextObject*, unsigned);
	void SendDetach(Cure::ContextObject*, Cure::ContextObject*);
	void AttachObjects(Cure::GameObjectId pObject1Id, unsigned pBody1Id, Cure::GameObjectId pObject2Id, unsigned pBody2Id);
	void DetachObjects(Cure::GameObjectId pObject1Id, Cure::GameObjectId pObject2Id);

	void CancelLogin();
	void OnAvatarSelect(UiTbc::Button* pButton);
	Cure::RuntimeVariableScope* GetVariableScope() const;

	Cure::NetworkClient* GetNetworkClient() const;

	void UpdateCameraPosition();

	void DrawAsyncDebugInfo();
	void DrawDebugStaple(int pIndex, int pHeight, const Color& pColor);
	void DrawSyncDebugInfo();

	typedef std::hash_map<Cure::GameObjectId, int> ObjectFrameIndexMap;
	typedef std::pair<Cure::GameObjectId, int> ObjectFrameIndexPair;
	typedef std::hash_map<Cure::GameObjectId, RoadSignButton*> RoadSignMap;

	GameClientMasterTicker* mMaster;
	UiCure::GameUiManager* mUiManager;
	const int mSlaveIndex;
	PixelRect mRenderArea;

	ObjectFrameIndexMap mObjectFrameIndexMap;

	bool mFirstInitialize;
	bool mIsReset;
	bool mIsResetComplete;
	bool mQuit;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	uint64 mLastSentByteCount;
	Timer mLastSendTime;
	int mPingAttemptCount;
	Timer mLastUnsafeReceiveTime;
	Alarm mCollisionExpireAlarm;
	Alarm mInputExpireAlarm;

	Timer mAvatarSelectTime;
	Timer mAvatarMightSelectTime;
	bool mJustLookingAtAvatars;
	int mRoadSignIndex;
	RoadSignMap mRoadSignMap;

	Vector3DF mCameraPosition;		// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraPreviousPosition;	// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraOrientation;		// TODO: remove hack (should context object controlled)!
	Cure::ObjectPositionalData mNetworkOutputGhost;	// TODO: remove hack (should be one per controllable object)!
	bool mAllowMovementInput;
	Options::ClientOptionsManager mOptions;
	UiTbc::Window* mLoginWindow;
	str mConnectUserName;
	str mConnectServerAddress;
	str mDisconnectReason;

	LOG_CLASS_DECLARE();
};



}
