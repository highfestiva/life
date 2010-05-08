
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/Thread.h"
#include "../../UiCure/Include/UiLineGraph2d.h"
#include "InputObserver.h"
#include "ScreenPart.h"



namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class ConsoleManager;
class GameClientSlaveManager;
class RoadSignButton;



class GameClientMasterTicker: public Cure::GameTicker, public InputObserver, public ScreenPart
{
public:
	GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager);
	virtual ~GameClientMasterTicker();

	bool CreateSlave();

	bool Tick();

	bool StartResetUi();
	bool WaitResetUi();

	bool IsLocalObject(Cure::GameObjectId pInstanceId) const;
	void GetSiblings(Cure::GameObjectId pObjectId, Cure::ContextObject::Array& pSiblingArray) const;

	virtual PixelRect GetRenderArea() const;
	virtual float UpdateFrustum();

	void OnExit();
	void OnSetPlayerCount(int pPlayerCount);

private:
	typedef GameClientSlaveManager* (*SlaveFactoryMethod)(GameClientMasterTicker* pMaster,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	static GameClientSlaveManager* CreateSlaveManager(GameClientMasterTicker* pMaster,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	static GameClientSlaveManager* CreateViewer(GameClientMasterTicker* pMaster,
		Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager,
		UiCure::GameUiManager* pUiManager, int pSlaveIndex, const PixelRect& pRenderArea);
	bool CreateSlave(SlaveFactoryMethod pCreate);
	void AddSlave(GameClientSlaveManager* pSlave);
	void DeleteSlave(GameClientSlaveManager* pSlave, bool pAllowMainMenu);

	bool Initialize();
	void CreatePlayerCountWindow();
	bool Reinitialize();
	void UpdateSlaveLayout();
	void SlideSlaveLayout();
	int GetSlaveAnimationTarget(int pSlaveIndex) const;
	float GetSlavesVerticalAnimationTarget() const;
	void Profile();
	void DrawFps() const;
	void DrawPerformanceLineGraph2d() const;

	float GetPowerSaveAmount() const;

	int OnCommandLocal(const str& pCommand, const strutil::strvec& pParameterVector);
	void OnCommandError(const str& pCommand, const strutil::strvec& pParameterVector, int pResult);

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void OnInput(UiLepra::InputElement* pElement);

	void ClosePlayerCountGui();

	bool ApplyCalibration();
	void StashCalibration();

	class MasterInputFunctor: public UiLepra::InputFunctor
	{
	public:
		MasterInputFunctor(GameClientMasterTicker* pManager);
	private:
		void Call(UiLepra::InputElement* pElement);
		UiLepra::InputFunctor* CreateCopy() const;
		GameClientMasterTicker* mManager;
	};

	typedef std::vector<GameClientSlaveManager*> SlaveArray;

	Lock mLock;
	UiCure::GameUiManager* mUiManager;
	Cure::ResourceManager* mResourceManager;
	bool mIsPlayerCountViewActive;

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
