
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Lepra/Include/DiskFile.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/Launcher.h"
#include "HeliForce.h"
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
namespace UiTbc
{
class Button;
}
namespace Life
{
class GameClientMasterTicker;
class Level;
}



namespace HeliForce
{



class HeliForceManager: public Life::GameClientSlaveManager, private Life::Launcher
{
	typedef Life::GameClientSlaveManager Parent;
public:
	HeliForceManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~HeliForceManager();
	virtual void LoadSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Paint();

	virtual void OnLoginSuccess();

	virtual bool IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const;
	Cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);

	virtual void DidFinishLevel();
	virtual void NextLevel();

protected:
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;

	Cure::RuntimeVariableScope* GetVariableScope() const;

	virtual bool Reset();
	virtual bool InitializeUniverse();
	void CreateChopper(const str& pClassId);

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

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

	void Shoot(Cure::ContextObject*, int);

	void OnFireButton(UiTbc::Button*);

	void DrawStick(Touchstick* pStick);

	virtual void UpdateCameraPosition(bool pUpdateMicPosition);

	UiCure::CollisionSoundManager* mCollisionSoundManager;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	StopWatch mAvatarCreateTimer;
	StopWatch mAvatarDied;
	bool mHadAvatar;
	bool mUpdateCameraForAvatar;
	Life::Options::Steering mLastSteering;
	int mActiveWeapon;
	HiResTimer mFireTimeout;

	Life::Level* mLevel;
	ObjectArray mMassObjectArray;

	TransformationF mCameraTransform;
	Vector3DF mCameraPreviousPosition;
	float mCameraSpeed;
	int mHitGroundFrameCount;
	bool mIsHitThisFrame;
	Vector3DF mMicrophoneSpeed;
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	UiTbc::Button* mFireButton;
#endif // Touch or emulated touch.

	HiResTimer mTouchstickTimer;
	Touchstick* mStickLeft;
	Touchstick* mStickRight;

	LOG_CLASS_DECLARE();
};



}
