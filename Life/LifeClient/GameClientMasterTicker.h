
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
class UiGameServerManager;



class GameClientMasterTicker: public Cure::GameTicker, public InputObserver, public ScreenPart
{
	typedef Cure::GameTicker Parent;
public:
	GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, float pPhysicsRadius, int pPhysicsLevels, float pPhysicsSensitivity);
	virtual ~GameClientMasterTicker();

	MasterServerConnection* GetMasterServerConnection() const;
	void SetMasterServerConnection(MasterServerConnection* pConnection);

	virtual bool CreateSlave() = 0;
	virtual void PrepareQuit();

	bool Tick();
	void PollRoundTrip();
	virtual void PreWaitPhysicsTick();

	bool StartResetUi();
	bool WaitResetUi();

	bool IsFirstSlave(const GameClientSlaveManager* pSlave) const;
	void GetSlaveInfo(const GameClientSlaveManager* pSlave, int& pIndex, int& pCount) const;
	bool IsLocalObject(Cure::GameObjectId pInstanceId) const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum(float pFov);
	float UpdateFrustum(float pFov, const PixelRect& pRenderArea);

	void PreLogin(const str& pServerAddress);
	bool IsLocalServer() const;
	UiGameServerManager* GetLocalServer() const;
	void OnExit();
	void OnSetPlayerCount(int pPlayerCount);

	void DownloadServerList();

protected:
	typedef GameClientSlaveManager* (*SlaveFactoryMethod)(GameClientMasterTicker* pMaster,
		Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope,
		Cure::ResourceManager* pResourceManager, UiCure::GameUiManager* pUiManager,
		int pSlaveIndex, const PixelRect& pRenderArea);

	bool CreateSlave(SlaveFactoryMethod pCreate);
	virtual void OnSlavesKilled() = 0;
	virtual void OnServerCreated(Life::UiGameServerManager* pServer) = 0;

	Cure::ContextObjectAttribute* CreateObjectAttribute(Cure::ContextObject* pObject, const str& pAttributeName);
	void AddSlave(GameClientSlaveManager* pSlave);
	void DeleteSlave(GameClientSlaveManager* pSlave, bool pAllowMainMenu);
	void DeleteServer();

	virtual bool Initialize();
	virtual bool Reinitialize();
	virtual bool OpenUiManager();
	void UpdateSlaveLayout();
	void SlideSlaveLayout();
	int GetSlaveAnimationTarget(int pSlaveIndex) const;
	float GetSlavesVerticalAnimationTarget() const;
	void MeasureLoad();
	void Profile();
	virtual void PhysicsTick();
	virtual void WillMicroTick(float pTimeDelta);
	virtual void DidPhysicsTick();
	virtual void BeginRender(Vector3DF& pColor);
	void DrawDebugData() const;
	void DrawPerformanceLineGraph2d() const;

	virtual float GetTickTimeReduction() const;
	virtual float GetPowerSaveAmount() const;

	virtual void OnTrigger(TBC::PhysicsManager::TriggerID pTrigger, int pTriggerListenerId, int pOtherBodyId, const Vector3DF& pNormal);
	virtual void OnForceApplied(int pObjectId, int pOtherObjectId, TBC::PhysicsManager::BodyID pBodyId, TBC::PhysicsManager::BodyID pOtherBodyId,
		const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition, const Vector3DF& pRelativeVelocity);

	int OnCommandLocal(const str& pCommand, const strutil::strvec& pParameterVector);
	void OnCommandError(const str& pCommand, const strutil::strvec& pParameterVector, int pResult);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnInput(UiLepra::InputElement* pElement);

	virtual void CloseMainMenu() = 0;

	bool ApplyCalibration();
	void StashCalibration();

	typedef UiLepra::TInputFunctor<GameClientMasterTicker> MasterInputFunctor;
	typedef std::vector<GameClientSlaveManager*> SlaveArray;

	Lock mLock;
	UiCure::GameUiManager* mUiManager;
	Cure::ResourceManager* mResourceManager;

	UiGameServerManager* mServer;
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
	std::vector<UiCure::LineGraph2d> mPerformanceGraphList;
	std::hash_set<Cure::GameObjectId> mLocalObjectSet;

	LOG_CLASS_DECLARE();
};



}
