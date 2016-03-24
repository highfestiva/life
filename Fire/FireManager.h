
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

//#include "../Tbc/Include/PhysicsEngine.h"
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
class BaseMachine;
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
	virtual void Suspend(bool pHard);
	virtual void LoadSettings();
	virtual void SaveSettings();
	virtual void SetRenderArea(const PixelRect& pRenderArea);
	virtual bool Open();
	virtual void Close();
	virtual void SetIsQuitting();
	virtual void SetFade(float pFadeAmount);

	PixelRect GetRenderableArea() const;
	virtual bool Render();
	virtual bool Paint();
	void PrintTime(const str pPrefix, double pTime, bool lIsSloppy, int x, int y, const Color c, const Color bg);
	virtual void DrawSyncDebugInfo();

	virtual bool IsObjectRelevant(const vec3& pPosition, float pDistance) const;

	virtual void Shoot(Cure::ContextObject* pAvatar, int pWeapon);
	virtual void Detonate(Cure::ContextObject* pExplosive, const Tbc::ChunkyBoneGeometry* pExplosiveGeometry, const vec3& pPosition, const vec3& pVelocity, const vec3& pNormal, float pStrength);
	virtual void OnBulletHit(Cure::ContextObject* pBullet, Cure::ContextObject* pHitObject);
	void OnLetThroughTerrorist(BaseMachine* pTerrorist);

	virtual bool DidFinishLevel();
	virtual str StepLevel(int pCount);
	str StoreLevelIndex(int pLevelNumber);

	virtual Level* GetLevel() const;
	virtual int GetCurrentLevelNumber() const;

	Cure::RuntimeVariableScope* GetVariableScope() const;

	struct TargetInfo
	{
		TargetInfo(str pVillain, PixelCoord pXY, float pDangerousness, float pScale):
			mVillain(pVillain),
			xy(pXY),
			mDangerousness(pDangerousness),
			mIsActive(true),
			mTime(0),
			mScale(pScale)
		{
		}
		str mVillain;
		PixelCoord xy;
		float mDangerousness;
		bool mIsActive;
		float mTime;
		float mScale;
	};
	typedef std::unordered_map<void*, TargetInfo> VillainMap;
	typedef std::pair<void*, TargetInfo> VillainPair;
	typedef std::vector<VillainPair> VillainArray;

	virtual bool InitializeUniverse();
	virtual void ScriptPhysicsTick();
	virtual void HandleWorldBoundaries();
	virtual void MoveCamera();
	virtual void UpdateCameraPosition(bool pUpdateMicPosition);
	virtual void HandleShooting();
	virtual void HandleTargets(float pTime);

	virtual void TickInput();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void SetLocalRender(bool pRender);

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const;
	virtual Cure::ContextObject* CreateLogicHandler(const str& pType);
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk);
	virtual void OnLevelLoadCompleted();
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, Tbc::PhysicsManager::BodyID pBody2Id);

	void OnBombButton(UiTbc::Button*);
	void OnPauseButton(UiTbc::Button* pButton);
	void CreateNextLevelDialog();
	void OnMenuAlternative(UiTbc::Button* pButton);

	void PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* pResource);

	bool DisableAmbient();
	void EnableAmbient();

	UiCure::CollisionSoundManager* mCollisionSoundManager;
	Life::Menu* mMenu;
	Level* mLevel;
	bool mSteppedLevel;
	Sunlight* mSunlight;
	xform mCameraTransform;
	UiTbc::Button* mPauseButton;
	UiTbc::Button* mBombButton;
	vec3 mShootDirection;
	StopWatch mAllLoadedTimer;
	StopWatch mSlowmoTimer;
	GameTimer mFireDelayTimer;
	vec3 mStoreAmbient;
	bool mStoreLightsEnabled;
	int mKills;
	int mKillLimit;
	int mLevelTotalKills;
	VillainMap mVillainMap;

	logclass();
};



}
