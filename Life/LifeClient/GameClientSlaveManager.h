
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/Packet.h"
#include "../../Cure/Include/PositionalData.h"
#include "../../Lepra/Include/Alarm.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../TBC/Include/PhysicsEngine.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "../../UiTBC/Include/UiFontManager.h"
#include "ClientConsoleManager.h"
#include "ClientLoginView.h"
#include "ClientOptionsManager.h"
#include "InputObserver.h"
#include "ScreenPart.h"



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



class CollisionSoundManager;
class GameClientMasterTicker;
class MasterServerConnection;
class RoadSignButton;



class GameClientSlaveManager: public Cure::GameManager, public InputObserver, private ClientLoginObserver, public ScreenPart
{
	typedef Cure::GameManager Parent;
public:
	GameClientSlaveManager(GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~GameClientSlaveManager();
	void LoadSettings();
	void SetRenderArea(const PixelRect& pRenderArea);
	bool Open();
	void Close();
	bool IsQuitting() const;
	void SetIsQuitting();
	void SetFade(float pFadeAmount);

	GameClientMasterTicker* GetMaster() const;

	bool Render();
	virtual bool Paint();
	virtual bool EndTick();
	void TickNetworkInput();
	bool TickNetworkOutput();

	void ToggleConsole();

	void RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken);
	void Logout();
	bool IsLoggingIn() const;
	void SelectAvatar(const Cure::UserAccount::AvatarId& pAvatarId);
	bool IsUiMoveForbidden(Cure::GameObjectId pObjectId) const;
	virtual void GetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const;
	void DoGetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const;
	void AddLocalObjects(std::hash_set<Cure::GameObjectId>& pLocalObjectSet);
	bool IsInCameraRange(const Vector3DF& pPosition, float pDistance) const;
	bool IsOwned(Cure::GameObjectId pObjectId) const;
	Cure::GameObjectId GetAvatarInstanceId() const;

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);

	bool SetAvatarEnginePower(unsigned pAspect, float pPower, float pAngle);

	int GetSlaveIndex() const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum(float pFov);

protected:
	typedef std::vector<Cure::GameObjectId> ObjectArray;
	enum SteeringPlaybackMode
	{
		PLAYBACK_NONE = 0,
		PLAYBACK_RECORD,
		PLAYBACK_PLAY,
	};

	str GetApplicationCommandFilename() const;

	bool Reset();
	virtual void CreateLoginView();
	virtual bool InitializeTerrain();
	void CloseLoginGui();
	void ClearRoadSigns();
	void SetRoadSignsVisible(bool pVisible);

	virtual void TickInput();

	virtual void TickUiInput();
	bool SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower, float pAngle);
	virtual void TickUiUpdate();
	bool UpdateMassObjects(const Vector3DF& pPosition);
	void SetLocalRender(bool pRender);
	void SetMassRender(bool pRender);

	void PhysicsTick();

	void ProcessNetworkInputMessage(Cure::Message* pMessage);
	void ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);
	bool CreateObject(Cure::GameObjectId pInstanceId, const str& pClassId, Cure::NetworkObjectType pNetworkType,
		TransformationF* pTransform = 0);
	Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void SetMovement(Cure::GameObjectId pInstanceId, int32 pFrameIndex, Cure::ObjectPositionalData& pData);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);
	virtual bool OnPhysicsSend(Cure::ContextObject* pObject);
	virtual bool OnAttributeSend(Cure::ContextObject* pObject);
	bool IsServer();
	void SendAttach(Cure::ContextObject*, unsigned, Cure::ContextObject*, unsigned);
	void SendDetach(Cure::ContextObject*, Cure::ContextObject*);
	virtual void OnAlarm(int pAlarmId, Cure::ContextObject* pObject, void* pExtraData);
	void AttachObjects(Cure::GameObjectId pObject1Id, unsigned pBody1Id, Cure::GameObjectId pObject2Id, unsigned pBody2Id);
	void DetachObjects(Cure::GameObjectId pObject1Id, Cure::GameObjectId pObject2Id);

	void CancelLogin();
	void OnAvatarSelect(UiTbc::Button* pButton);
	void DropAvatar();
	bool QuerySetIsChild(Cure::ContextObject* pAvatar) const;
	Cure::RuntimeVariableScope* GetVariableScope() const;

	Cure::NetworkClient* GetNetworkClient() const;

	void UpdateCameraPosition(bool pUpdateMicPosition);
	QuaternionF GetCameraQuaternion() const;

	UiTbc::FontManager::FontId SetFontHeight(double pHeight);

	void DrawAsyncDebugInfo();
	void DrawDebugStaple(int pIndex, int pHeight, const Color& pColor);
	void DrawSyncDebugInfo();

	typedef std::hash_map<Cure::GameObjectId, int> ObjectFrameIndexMap;
	typedef std::pair<Cure::GameObjectId, int> ObjectFrameIndexPair;
	typedef std::hash_map<Cure::GameObjectId, RoadSignButton*> RoadSignMap;
	typedef std::hash_set<Cure::GameObjectId> ObjectIdSet;

	GameClientMasterTicker* mMaster;
	UiCure::GameUiManager* mUiManager;
	CollisionSoundManager* mCollisionSoundManager;
	const int mSlaveIndex;
	PixelRect mRenderArea;

	MasterServerConnection* mMasterServerConnection;

	ObjectFrameIndexMap mObjectFrameIndexMap;

	bool mFirstInitialize;
	bool mIsReset;
	bool mIsResetComplete;
	bool mQuit;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	bool mHadAvatar;
	ObjectIdSet mOwnedObjectList;
	uint64 mLastSentByteCount;
	Timer mLastSendTime;
	int mPingAttemptCount;
	Timer mLastUnsafeReceiveTime;
	Alarm mCollisionExpireAlarm;
	Alarm mInputExpireAlarm;
	Options::Steering mLastSteering;
	float mCamRotateExtra;

	HiResTimer mAvatarSelectTime;
	HiResTimer mAvatarMightSelectTime;
	bool mJustLookingAtAvatars;
	int mAvatarInvisibleCount;
	int mRoadSignIndex;
	RoadSignMap mRoadSignMap;

	Cure::GameObjectId mLevelId;
	ObjectArray mMassObjectArray;
	Cure::ContextObject* mSun;	// TODO: remove hack (should context object controlled)!
	std::vector<Cure::ContextObject*> mCloudArray;	// TODO: remove hack (should context object controlled)!

	Vector3DF mCameraPosition;		// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraPreviousPosition;	// TODO: remove hack (should context object controlled)!
	//Vector3DF mCameraFollowVelocity;	// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraUp;			// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraOrientation;		// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraPivotPosition;		// TODO: remove hack (should context object controlled)!
	Vector3DF mCameraPivotVelocity;		// TODO: remove hack (should context object controlled)!
	float mCameraTargetXyDistance;		// TODO: remove hack (should context object controlled)!
	float mCameraMaxSpeed;			// TODO: remove hack (should context object controlled)!
	Vector3DF mMicrophoneSpeed;		// TODO: remove hack (should context object controlled)!
	bool mAllowMovementInput;
	Options::ClientOptionsManager mOptions;
	UiTbc::Window* mLoginWindow;
	str mConnectUserName;
	str mConnectServerAddress;
	str mDisconnectReason;

	struct EnginePower	// Used for recording vechile steering playback.
	{
		float mPower;
		float mAngle;
	};
	DiskFile mEnginePlaybackFile;	// Used for recording vechile steering playback.
	float mEnginePlaybackTime;	// Used for recording vechile steering playback.
	EnginePower mEnginePowerShadow[TBC::PhysicsEngine::ASPECT_COUNT];	// Used for recording vechile steering playback.

#ifdef LIFE_DEMO
	HiResTimer mDemoTime;
#endif // Demo version

	LOG_CLASS_DECLARE();
};



}
