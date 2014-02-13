
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

//#include "../TBC/Include/PhysicsEngine.h"
#include "../Life/LifeClient/GameClientSlaveManager.h"
#include "../Life/Launcher.h"
#include "../Lepra/Include/GameTimer.h"
#include "../UiCure/Include/UiResourceManager.h"
#include "Fire.h"
#include "Version.h"



namespace UiCure
{
class CollisionSoundManager;
class CppContextObject;
}
namespace UiTbc
{
class Button;
}
namespace Life
{
class GameClientMasterTicker;
class Menu;
}



namespace Fire
{



class Autopilot;
class Level;
class Sunlight;



class FireManager: public Life::GameClientSlaveManager, private Life::Launcher
{
	typedef Life::GameClientSlaveManager Parent;
public:
	FireManager(Life::GameClientMasterTicker* pMaster, const Cure::TimeManager* pTime,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	virtual ~FireManager();
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

	virtual void Shoot(Cure::ContextObject* pAvatar, int pWeapon);
	virtual void Detonate(Cure::ContextObject* pExplosive, const TBC::ChunkyBoneGeometry* pExplosiveGeometry, const Vector3DF& pPosition, const Vector3DF& pVelocity, const Vector3DF& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);

	virtual bool DidFinishLevel();
	virtual str StepLevel(int pCount);

	virtual Level* GetLevel() const;
	virtual int GetCurrentLevelNumber() const;

	Cure::RuntimeVariableScope* GetVariableScope() const;

protected:
	virtual bool InitializeUniverse();
	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera();
	virtual void UpdateCameraPosition(bool pUpdateMicPosition);
	virtual void HandleShooting();

	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool pRender);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	virtual void OnLevelLoadCompleted();
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

	void OnPauseButton(UiTbc::Button*);
	void OnMenuAlternative(UiTbc::Button* pButton);

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	bool DisableAmbient();
	void EnableAmbient();

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	Life::Menu* mMenu;
	Level* mLevel;
	Sunlight* mSunlight;
	TransformationF mCameraTransform;
	UiTbc::Button* mPauseButton;
	Vector3DF mShootDirection;
	StopWatch mAllLoadedTimer;
	StopWatch mSlowmoTimer;
	GameTimer mFireDelayTimer;
	Vector3DF mStoreAmbient;
	bool mStoreLightsEnabled;
	int mKills;

	LOG_CLASS_DECLARE();
};



}
