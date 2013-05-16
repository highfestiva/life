
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../Lepra/Include/DiskFile.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/Launcher.h"
#include "../UiCure/Include/UiResourceManager.h"
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
class CppContextObject;
}
namespace UiTbc
{
class BillboardGeometry;
class Button;
}
namespace Life
{
class GameClientMasterTicker;
class Level;
}



namespace HeliForce
{



class Autopilot;
class Level;
class Sunlight;



class HeliForceManager: public Life::GameClientSlaveManager, private Life::Launcher
{
	typedef Life::GameClientSlaveManager Parent;
public:
	HeliForceManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~HeliForceManager();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Render();
	virtual bool Paint();

	virtual void OnLoginSuccess();

	virtual bool IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const;
	Cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);

	virtual bool DidFinishLevel();
	virtual void NextLevel();

	virtual Level* GetLevel() const;
	virtual Cure::ContextObject* GetAvatar() const;

protected:
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;

	Cure::RuntimeVariableScope* GetVariableScope() const;

	virtual bool Reset();
	virtual bool InitializeUniverse();
	void CreateChopper(const str& pClassId);
	void UpdateChopperColor(float pLerp);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
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

	Vector3DF GetLandingTriggerPosition(Cure::ContextObject* pLevel) const;
	float EaseDown(Cure::ContextObject* pObject, const Vector3DF* pStartPosition);
	TransformationF GetMainRotorTransform(const UiCure::CppContextObject* pChopper) const;

	void Shoot(Cure::ContextObject*, int);

	void OnFireButton(UiTbc::Button*);

	void DrawStick(Touchstick* pStick);

	virtual void UpdateCameraPosition(bool pUpdateMicPosition);

	void RendererTextureLoadCallback(UiCure::UserRendererImageResource* pResource);

	void DisableDepth();

	UiCure::CollisionSoundManager* mCollisionSoundManager;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	StopWatch mTooFarAwayTimer;
	StopWatch mAvatarCreateTimer;
	StopWatch mAvatarDied;
	bool mHadAvatar;
	bool mUpdateCameraForAvatar;
	Life::Options::Steering mLastSteering;
	int mActiveWeapon;
	HiResTimer mFireTimeout;

	Level* mLevel;
	Level* mOldLevel;
	Autopilot* mAutopilot;
	UiCure::CppContextObject* mHemisphere;
	TBC::BoneAnimator* mHemisphereUvTransform;
	Sunlight* mSunlight;
	ObjectArray mMassObjectArray;

	TransformationF mCameraTransform;
	Vector3DF mCameraPreviousPosition;
	float mCameraSpeed;
	bool mZoomPlatform;
	int mPostZoomPlatformFrameCount;
	int mHitGroundFrameCount;
	bool mIsHitThisFrame;
	Vector3DF mMicrophoneSpeed;
#if defined(LEPRA_TOUCH) || defined(EMULATE_TOUCH)
	UiTbc::Button* mFireButton;
#endif // Touch or emulated touch.

	HiResTimer mTouchstickTimer;
	Touchstick* mStick;

	UiCure::UserRendererImageResource* mArrow;
	UiTbc::BillboardGeometry* mArrowBillboard;
	UiTbc::Renderer::GeometryID mArrowBillboardId;
	float mArrowTotalPower;
	float mArrowAngle;

	LOG_CLASS_DECLARE();
};



}
