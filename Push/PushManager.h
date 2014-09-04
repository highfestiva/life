
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/DiskFile.h"
#include "../Tbc/Include/PhysicsEngine.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/Launcher.h"
#include "LoginView.h"
#include "Push.h"
#include "Version.h"



namespace UiLepra
{
namespace Touch
{
class TouchstickInputDevice;
}
}
namespace UiCure
{
class CollisionSoundManager;
}
namespace Life
{
class GameClientMasterTicker;
class Level;
}



namespace Push
{



class RoadSignButton;



class PushManager: public Life::GameClientSlaveManager, private ClientLoginObserver
{
	typedef Life::GameClientSlaveManager Parent;
public:
	PushManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~PushManager();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Paint();

	virtual void RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken);
	virtual void OnLoginSuccess();

	void SelectAvatar(const Cure::UserAccount::AvatarId& pAvatarId);
	void AddLocalObjects(std::unordered_set<Cure::GameObjectId>& pLocalObjectSet);
	virtual bool IsObjectRelevant(const vec3& pPosition, float pDistance) const;
	Cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

protected:
	typedef std::unordered_map<Cure::GameObjectId, RoadSignButton*> RoadSignMap;
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;

	Cure::RuntimeVariableScope* GetVariableScope() const;

	virtual bool Reset();
	virtual void CreateLoginView();
	virtual bool InitializeUniverse();
	void CloseLoginGui();
	void ClearRoadSigns();
	void SetRoadSignsVisible(bool pVisible);

	virtual void ScriptPhysicsTick();
	virtual void MoveCamera();
	virtual void TickInput();

	void UpdateTouchstickPlacement();
	virtual void TickUiInput();
	bool SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower);
	virtual void TickUiUpdate();
	virtual bool UpdateMassObjects(const vec3& pPosition);
	virtual void SetLocalRender(bool pRender);
	void SetMassRender(bool pRender);

	virtual void ProcessNetworkInputMessage(Cure::Message* pMessage);
	virtual void ProcessNetworkStatusMessage(Cure::MessageStatus* pMessage);
	virtual void ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id);

	void CancelLogin();
	void OnVehicleSelect(UiTbc::Button* pButton);
	void OnAvatarSelect(UiTbc::Button* pButton);
	void DropAvatar();

	void DrawStick(Touchstick* pStick);

	virtual void UpdateCameraPosition(bool pUpdateMicPosition);
	quat GetCameraQuaternion() const;

	UiCure::CollisionSoundManager* mCollisionSoundManager;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	bool mHadAvatar;
	bool mUpdateCameraForAvatar;
	Life::Options::Steering mLastSteering;
	float mCamRotateExtra;

	RoadSignButton* mPickVehicleButton;
	int mAvatarInvisibleCount;
	int mRoadSignIndex;
	RoadSignMap mRoadSignMap;

	Cure::GameObjectId mLevelId;
	mutable Life::Level* mLevel;
	ObjectArray mMassObjectArray;
	Cure::ContextObject* mSun;
	std::vector<Cure::ContextObject*> mCloudArray;

	vec3 mCameraPosition;
	vec3 mCameraPreviousPosition;
	vec3 mCameraFollowVelocity;
	vec3 mCameraUp;
	vec3 mCameraOrientation;
	vec3 mCameraPivotPosition;
	vec3 mCameraPivotVelocity;
	float mCameraTargetXyDistance;
	float mCameraMaxSpeed;
	float mCameraMouseAngle;
	HiResTimer mCameraMouseAngleTimer;
	float mCameraTargetAngle;
	float mCameraTargetAngleFactor;
	vec3 mMicrophoneSpeed;
	UiTbc::Window* mLoginWindow;

	HiResTimer mTouchstickTimer;
	Touchstick* mStickLeft;
	Touchstick* mStickRight;

	struct EnginePower	// Used for recording vechile steering playback.
	{
		float mPower;
	};
	DiskFile mEnginePlaybackFile;	// Used for recording vechile steering playback.
	float mEnginePlaybackTime;	// Used for recording vechile steering playback.
	EnginePower mEnginePowerShadow[Tbc::PhysicsEngine::ASPECT_COUNT];	// Used for recording vechile steering playback.

#ifdef PUSH_DEMO
	HiResTimer mDemoTime;
#endif // Demo version

	logclass();
};



}
