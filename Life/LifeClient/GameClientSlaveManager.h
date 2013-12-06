
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/PositionalData.h"
#include "../../Cure/Include/UserAccount.h"
#include "../../Lepra/Include/Alarm.h"
#include "ClientOptionsManager.h"
#include "InputObserver.h"
#include "ScreenPart.h"



namespace Cure
{
class NetworkClient;
class ResourceManager;
}
namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class GameClientMasterTicker;
class MasterServerConnection;



class GameClientSlaveManager: public Cure::GameManager, public InputObserver, public ScreenPart
{
	typedef Cure::GameManager Parent;
public:
	GameClientSlaveManager(GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~GameClientSlaveManager();
	void SetMasterServerConnection(MasterServerConnection* pConnection);
	virtual void Suspend();
	virtual void LoadSettings();
	virtual void SaveSettings() = 0;
	virtual void RefreshOptions();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	bool IsQuitting() const;
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount) = 0;

	GameClientMasterTicker* GetMaster() const;

	virtual bool Render();
	virtual bool Paint();
	virtual bool IsPrimaryManager() const;
	virtual void PreEndTick();
	virtual bool EndTick();
	void TickNetworkInput();
	bool TickNetworkOutput();
	void TickNetworkOutputGhosts();

	void ToggleConsole();

	virtual void RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken);
	virtual void OnLoginSuccess();
	void Logout();
	bool IsLoggingIn() const;
	bool IsUiMoveForbidden(Cure::GameObjectId pObjectId) const;
	void AddLocalObjects(std::unordered_set<Cure::GameObjectId>& pLocalObjectSet);
	virtual bool IsOwned(Cure::GameObjectId pObjectId) const;

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);
	virtual void HandleUnusedRelativeAxis();

	int GetSlaveIndex() const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum(float pFov);

protected:
	typedef std::vector<Cure::GameObjectId> ObjectArray;
	typedef std::unordered_set<UiLepra::InputElement*, LEPRA_VOIDP_HASHER> InputElementSet;
	enum SteeringPlaybackMode
	{
		PLAYBACK_NONE = 0,
		PLAYBACK_RECORD,
		PLAYBACK_PLAY,
	};

	str GetApplicationCommandFilename() const;

	virtual bool Reset();
	virtual bool InitializeUniverse() = 0;

	virtual void TickUiUpdate() = 0;
	virtual void SetLocalRender(bool pRender) = 0;

	virtual void ProcessNetworkInputMessage(Cure::Message* pMessage);
	virtual void ProcessNetworkStatusMessage(Cure::MessageStatus* pMessage);
	virtual void ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);
	Cure::ContextObject* CreateObject(Cure::GameObjectId pInstanceId, const str& pClassId, Cure::NetworkObjectType pNetworkType,
		TransformationF* pTransform = 0);
	void SetMovement(Cure::GameObjectId pInstanceId, int32 pFrameIndex, Cure::ObjectPositionalData& pData, float pDeltaThreshold);
	virtual bool OnPhysicsSend(Cure::ContextObject* pObject);
	virtual bool OnAttributeSend(Cure::ContextObject* pObject);
	bool IsServer();
	void SendAttach(Cure::ContextObject*, unsigned, Cure::ContextObject*, unsigned);
	void SendDetach(Cure::ContextObject*, Cure::ContextObject*);
	virtual void OnAlarm(int pAlarmId, Cure::ContextObject* pObject, void* pExtraData);
	void AttachObjects(Cure::GameObjectId pObject1Id, unsigned pBody1Id, Cure::GameObjectId pObject2Id, unsigned pBody2Id);
	void DetachObjects(Cure::GameObjectId pObject1Id, Cure::GameObjectId pObject2Id);

	Cure::NetworkClient* GetNetworkClient() const;

	virtual void UpdateCameraPosition(bool pUpdateMicPosition) = 0;

	virtual void DrawAsyncDebugInfo();
	void DrawDebugStaple(int pIndex, int pHeight, const Color& pColor);
	virtual void DrawSyncDebugInfo();

	typedef std::unordered_map<Cure::GameObjectId, int> ObjectFrameIndexMap;
	typedef std::unordered_set<Cure::GameObjectId> ObjectIdSet;

	UiCure::GameUiManager* mUiManager;
	const int mSlaveIndex;
	PixelRect mRenderArea;

	MasterServerConnection* mMasterServerConnection;

	ObjectFrameIndexMap mObjectFrameIndexMap;

	bool mIsReset;
	bool mIsResetComplete;
	bool mQuit;

	ObjectIdSet mOwnedObjectList;
	uint64 mLastSentByteCount;
	Timer mLastSendTime;
	int mPingAttemptCount;
	Timer mLastUnsafeReceiveTime;
	Alarm mCollisionExpireAlarm;
	Alarm mInputExpireAlarm;
	Alarm mSendExpireAlarm;

	InputElementSet mRelativeAxis;
	InputElementSet mUnusedRelativeAxis;

	bool mAllowMovementInput;
	Options::ClientOptionsManager mOptions;
	str mConnectUserName;
	str mConnectServerAddress;
	str mDisconnectReason;

	LOG_CLASS_DECLARE();
};



}
