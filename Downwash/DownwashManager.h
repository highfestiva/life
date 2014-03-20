
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../Lepra/Include/DiskFile.h"
#include "../Lepra/Include/GameTimer.h"
#include "../TBC/Include/PhysicsEngine.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/Launcher.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "Downwash.h"
#include "Version.h"



namespace Cure
{
class HiscoreAgent;
}
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
class Menu;
}



namespace Downwash
{



class Autopilot;
class Level;
class Sunlight;



class DownwashManager: public Life::GameClientSlaveManager, private Life::Launcher
{
	typedef Life::GameClientSlaveManager Parent;
public:
	DownwashManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~DownwashManager();
	virtual void Suspend();
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	virtual bool Render();
	virtual bool Paint();
	void PrintTime(const str pPrefix, double pTime, bool lIsSloppy, int x, int y, const Color c, const Color bg);
	virtual void DrawSyncDebugInfo();

	virtual bool IsObjectRelevant(const Vector3DF& pPosition, float pDistance) const;
	Cure::GameObjectId GetAvatarInstanceId() const;

	bool SetAvatarEnginePower(unsigned pAspect, float pPower);

	virtual void Shoot(Cure::ContextObject* pCanon, int pAmmo);
	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);

	virtual bool DidFinishLevel();
	virtual str StepLevel(int pCount);

	virtual Level* GetLevel() const;
	virtual int GetCurrentLevelNumber() const;
	virtual double GetCurrentLevelBestTime(bool pWorld) const;
	virtual void SetLevelBestTime(int pLevelIndex, bool pWorld, double pTime);
	virtual Cure::ContextObject* GetAvatar() const;

	Cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	typedef UiLepra::Touch::TouchstickInputDevice Touchstick;

	virtual bool InitializeUniverse();
	void CreateChopper(const str& pClassId);
	void UpdateChopperColor(float pLerp);

	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera();
	virtual void TickInput();

	void UpdateCameraDistance();
	void UpdateTouchstickPlacement();
	int GetControlMode() const;
	void UpdateControlMode();
	virtual void TickUiInput();
	bool SetAvatarEnginePower(Cure::ContextObject* pAvatar, unsigned pAspect, float pPower);
	virtual void TickUiUpdate();
	virtual bool UpdateMassObjects(const Vector3DF& pPosition);
	virtual void SetLocalRender(bool pRender);
	void SetMassRender(bool pRender);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	virtual void OnLevelLoadCompleted();
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

	Vector3DF GetLandingTriggerPosition(Cure::ContextObject* pLevel) const;
	void EaseDown(Cure::ContextObject* pObject, const Vector3DF* pStartPosition);
	TransformationF GetMainRotorTransform(const UiCure::CppContextObject* pChopper) const;

	void OnPauseButton(UiTbc::Button*);
	void OnLastHiscoreButton(UiTbc::Button*);
	void ShowHiscoreDialog(int pDirection);
	void UpdateHiscoreDialog();
	void OnMenuAlternative(UiTbc::Button* pButton);
	void OnPreHiscoreAction(UiTbc::Button* pButton);

	void UpdateHiscoreDialogTitle();
	str GetHiscoreLevelTitle() const;

	void CreateHiscoreAgent();
	void TickHiscore();

	void DrawStick(Touchstick* pStick);

	virtual void UpdateCameraPosition(bool pUpdateMicPosition);

	void DrawImage(UiTbc::Painter::ImageID pImageId, float cx, float cy, float w, float h, float pAngle) const;

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);
	void RendererTextureLoadCallback(UiCure::UserRendererImageResource* pResource);

	bool DisableDepth();
	void EnableDepth();

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	Life::Menu* mMenu;

	// Network transmission and keepalive info.
	Cure::GameObjectId mAvatarId;
	StopWatch mAllLoadedTimer;
	StopWatch mTooFarAwayTimer;
	StopWatch mAvatarCreateTimer;
	StopWatch mAvatarDied;
	GameTimer mFlyTime;
	StopWatch mSlowmoTimer;
	HiResTimer mToyModeColorTimer;
	Vector3DF mLastVehicleColor;
	Vector3DF mLastChopperColor;
	bool mSetRandomChopperColor;
	bool mHadAvatar;
	bool mUpdateCameraForAvatar;
	Life::Options::Steering mLastSteering;
	int mActiveWeapon;

	Level* mLevel;
	Level* mOldLevel;
	Autopilot* mAutopilot;
	UiCure::CppContextObject* mHemisphere;
	TBC::BoneAnimator* mHemisphereUvTransform;
	bool mRenderHemisphere;
	Sunlight* mSunlight;
	ObjectArray mMassObjectArray;

	TransformationF mCameraTransform;
	Vector3DF mCameraPreviousPosition;
	Vector3DF mHelicopterPosition;
	mutable Vector3DF mLastLandingTriggerPosition;
	float mCameraSpeed;
	bool mZoomPlatform;
	int mPostZoomPlatformFrameCount;
	int mHitGroundFrameCount;
	bool mIsHitThisFrame;
	bool mLevelCompleted;
	Vector3DF mMicrophoneSpeed;
	UiTbc::Button* mPauseButton;
	UiTbc::Button* mLastHiscoreButton;

	HiResTimer mTouchstickTimer;
	Touchstick* mStick;

	UiCure::UserPainterKeepImageResource* mWrongDirectionImage;
	UiCure::UserPainterKeepImageResource* mWinImage;
	UiCure::UserPainterKeepImageResource* mCheckIcon;
	UiCure::UserPainterKeepImageResource* mLockIcon;
	StopWatch mDirectionImageTimer;
	StopWatch mWinImageTimer;
	UiCure::UserRendererImageResource* mArrow;
	UiTbc::BillboardGeometry* mArrowBillboard;
	UiTbc::Renderer::GeometryID mArrowBillboardId;
	float mArrowTotalPower;
	float mArrowAngle;
	int mSlowSystemCounter;

	int mHiscoreLevelIndex;
	int mMyHiscoreIndex;
	Cure::HiscoreAgent* mHiscoreAgent;
	StopWatch mHiscoreJustUploadedTimer;

	LOG_CLASS_DECLARE();
};



}
