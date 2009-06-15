
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "../../Cure/Include/GameManager.h"
#include "../../Lepra/Include/Thread.h"
#include "InputObserver.h"
#include "PlayerCountView.h"



namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class GameClientSlaveManager;



class GameClientMasterTicker: public Cure::GameTicker, public InputObserver, public PlayerCountObserver
{
public:
	GameClientMasterTicker(UiCure::GameUiManager* pUiManager, Cure::ResourceManager* mResourceManager);
	virtual ~GameClientMasterTicker();

	bool CreateSlave();

	bool Tick();

	bool StartResetUi();
	bool WaitResetUi();

private:
	void AddSlave(GameClientSlaveManager* pSlave);
	void RemoveSlave(GameClientSlaveManager* pSlave);

	bool Initialize();
	void CreatePlayerCountWindow();
	bool Reinitialize();
	void UpdateSlaveLayout();

	bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	bool OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode);
	void OnInput(UiLepra::InputElement* pElement);

	void OnExit(View* pPlayerCountView);
	void OnSetPlayerCount(View* pPlayerCountView, int pPlayerCount);

	class MasterInputFunctor: public UiLepra::InputFunctor
	{
	public:
		MasterInputFunctor(GameClientMasterTicker* pManager);
	private:
		void Call(UiLepra::InputElement* pElement);
		UiLepra::InputFunctor* CreateCopy() const;
		GameClientMasterTicker* mManager;
	};

	typedef Lepra::OrderedMap<GameClientSlaveManager*, GameClientSlaveManager*, std::hash<void*> > SlaveMap;

	Lepra::Lock mLock;
	UiCure::GameUiManager* mUiManager;
	Lepra::PerformanceData mResourceTime;
	Cure::ResourceManager* mResourceManager;
	bool mRestartUi;
	bool mInitialized;
	unsigned mActiveWidth;
	unsigned mActiveHeight;
	SlaveMap mSlaveSet;

	LOG_CLASS_DECLARE();
};



}
