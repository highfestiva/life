
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/GameTicker.h"
#include "../../Lepra/Include/Thread.h"
#include "../../UiCure/Include/UiLineGraph2d.h"
#include "../../UiTBC/Include/UiRenderer.h"
#include "InputObserver.h"
#include "ScreenPart.h"



namespace Cure
{
class NetworkFreeAgent;
}
namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class ConsoleManager;
class GameClientSlaveManager;
class MasterServerConnection;
class RoadSignButton;
class Sunlight;
class UiGameServerManager;



class GameClientMasterTicker: public Cure::GameTicker, public InputObserver, public ScreenPart
{
	typedef Cure::GameTicker Parent;
public:
	GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~GameClientMasterTicker();

	MasterServerConnection* GetMasterServerConnection() const;
	void SetMasterServerConnection(MasterServerConnection* pConnection);

	bool CreateSlave();

	bool Tick();
	void PollRoundTrip();

	bool StartResetUi();
	bool WaitResetUi();

	bool IsFirstSlave(const GameClientSlaveManager* pSlave) const;
	bool IsLocalObject(Cure::GameObjectId pInstanceId) const;
	void GetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum(float pFov);
	float UpdateFrustum(float pFov, const PixelRect& pRenderArea);

	void PreLogin(const str& pServerAddress);
	bool IsLocalServer() const;
	UiGameServerManager* GetLocalServer() const;
	void OnExit();
	void OnSetPlayerCount(int pPlayerCount);

	void DownloadServerList();

	Sunlight* GetSunlight() const;	// TODO: move this hard-coding to a context object or summat.

private:
	typedef GameClientSlaveManager* (*SlaveFactoryMethod)(GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);
	static GameClientSlaveManager* CreateSlaveManager(GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);
	static GameClientSlaveManager* CreateViewer(GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);
	static GameClientSlaveManager* CreateDemo(GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);
	Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName);
	bool CreateSlave(SlaveFactoryMethod pCreate);
	void AddSlave(GameClientSlaveManager* pSlave);
	void DeleteSlave(GameClientSlaveManager* pSlave, bool pAllowMainMenu);
	void DeleteServer();

	bool Initialize();
	void CreatePlayerCountWindow();
	bool Reinitialize();
	void UpdateSlaveLayout();
	void SlideSlaveLayout();
	int GetSlaveAnimationTarget(int pSlaveIndex) const;
	float GetSlavesVerticalAnimationTarget() const;
	void Profile();
	bool QueryQuit();
	virtual void PhysicsTick();
	virtual void WillMicroTick(float pTimeDelta);
	virtual void DidPhysicsTick();
	void DrawDebugData() const;
	void DrawPerformanceLineGraph2d() const;

	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	int OnCommandLocal(const str& pCommand, const strutil::strvec& pParameterVector);
	void OnCommandError(const str& pCommand, const strutil::strvec& pParameterVector, int pResult);

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void OnInput(UiLepra::InputElement* pElement);

	void ClosePlayerCountGui();

	bool ApplyCalibration();
	void StashCalibration();

	typedef UiLepra::TInputFunctor<GameClientMasterTicker> MasterInputFunctor;
	typedef std::vector<GameClientSlaveManager*> SlaveArray;

	Lock mLock;
	UiCure::GameUiManager* mUiManager;
	Cure::ResourceManager* mResourceManager;
	bool mIsPlayerCountViewActive;

	UiGameServerManager* mServer;
	Cure::TimeManager* mServerTimeManager;
	MasterServerConnection* mMasterConnection;
	Cure::NetworkFreeAgent* mFreeNetworkAgent;

	ConsoleManager* mConsole;
	bool mRestartUi;
	bool mInitialized;
	unsigned mActiveWidth;
	unsigned mActiveHeight;
	SlaveArray mSlaveArray;
	int mActiveSlaveCount;
	float mSlaveTopSplit;
	float mSlaveBottomSplit;
	float mSlaveVSplit;
	float mSlaveFade;
	HiResTimer* mDemoTime;
	std::vector<UiCure::LineGraph2d> mPerformanceGraphList;
	std::hash_set<Cure::GameObjectId> mLocalObjectSet;

	Sunlight* mSunlight;	// TODO: remove hack and come up with something better?

	LOG_CLASS_DECLARE();
};



}
