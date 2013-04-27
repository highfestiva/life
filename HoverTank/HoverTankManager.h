
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Lepra/Include/DiskFile.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/Launcher.h"
#include "LoginView.h"
#include "HoverTank.h"
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



namespace HoverTank
{



class RoadSignButton;



class HoverTankManager: public Life::GameClientSlaveManager, private ClientLoginObserver, private Life::Launcher
{
	typedef Life::GameClientSlaveManager Parent;
public:
	HoverTankManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~HoverTankManager();
	virtual void LoadSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Paint();

	virtual void RequestLogin(const str& pServerAddress, const Cure::LoginId& pLoginToken);
	virtual void OnLoginSuccess();

	void SelectAvatar(const Cure::UserAccount::AvatarId& pAvatarId);
	void AddLocalObjects(std::hash_set<Cure::GameObjectId>& pLocalObjectSet);
	virtual bool IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const;
	Cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);

protected:
	typedef std::hash_map<Cure::GameObjectId, RoadSignButton*> RoadSignMap;
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
	virtual bool UpdateMassObjects(const Vector3DF& pPosition);
	virtual void SetLocalRender(bool pRender);
	void SetMassRender(bool pRender);

	virtual void ProcessNetworkInputMessage(Cure::Message* pMessage);
	virtual void ProcessNetworkStatusMessage(Cure::MessageStatus* pMessage);
	virtual void ProcessNumber(Cure::MessageNumber::InfoType pType, int32 pInteger, float32 pFloat);
	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

	void OnFireButton(UiTbc::Button*);
	void AvatarShoot();
	void Shoot(Cure::ContextObject* pAvatar, int pWeapon);

	void CancelLogin();
	void OnVehicleSelect(UiTbc::Button* pButton);
	void OnAvatarSelect(UiTbc::Button* pButton);
	void DropAvatar();

	void DrawStick(Touchstick* pStick);
	void DrawScore();

	virtual void UpdateCameraPosition(bool pUpdateMicPosition);
	QuaternionF GetCameraQuaternion() const;

	UiCure::CollisionSoundManager* mCollisionSoundManager;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	bool mHadAvatar;
	bool mUpdateCameraForAvatar;
	Life::Options::Steering mLastSteering;
	float mCamRotateExtra;
	int mActiveWeapon;
	HiResTimer mFireTimeout;

	RoadSignButton* mPickVehicleButton;
	int mAvatarInvisibleCount;
	int mRoadSignIndex;
	RoadSignMap mRoadSignMap;

	Cure::GameObjectId mLevelId;
	Life::Level* mLevel;
	ObjectArray mMassObjectArray;
	Cure::ContextObject* mSun;
	std::vector<Cure::ContextObject*> mCloudArray;

	Cure::GameObjectId mScoreInfoId;

	Vector3DF mCameraPosition;
	Vector3DF mCameraPreviousPosition;
	Vector3DF mCameraUp;
	Vector3DF mCameraOrientation;
	Vector3DF mCameraPivotPosition;
	Vector3DF mCameraPivotVelocity;
	float mCameraTargetXyDistance;
	float mCameraMaxSpeed;
	float mCameraMouseAngle;
	HiResTimer mCameraMouseAngleTimer;
	float mCameraTargetAngle;
	float mCameraTargetAngleFactor;
	Vector3DF mMicrophoneSpeed;
	UiTbc::Window* mLoginWindow;
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	UiTbc::Button* mFireButton;
#endif // Touch or emulated touch.

	HiResTimer mTouchstickTimer;
	Touchstick* mStickLeft;
	Touchstick* mStickRight;

	struct EnginePower	// Used for recording vechile steering playback.
	{
		float mPower;
	};
	DiskFile mEnginePlaybackFile;	// Used for recording vechile steering playback.
	float mEnginePlaybackTime;	// Used for recording vechile steering playback.
	EnginePower mEnginePowerShadow[TBC::PhysicsEngine::ASPECT_COUNT];	// Used for recording vechile steering playback.

	LOG_CLASS_DECLARE();
};



}
