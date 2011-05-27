
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Lepra/Include/SystemManager.h"
#include "../UiLepra/Include/UiCore.h"
#include "SlimeVolleyball.hpp"


namespace Slime
{



class App: public Application
{
public:
	typedef Application Parent;

	App(const strutil::strvec& pArgumentList);
	virtual ~App();
	virtual void Init();
	virtual int Run();

private:
	SlimeVolleyball* mGame;
};



}



LEPRA_RUN_APPLICATION(Slime::App, UiLepra::UiMain);



namespace Slime
{



App::App(const strutil::strvec& pArgumentList):
	Application(pArgumentList)
{
}

App::~App()
{
	//delete (mUiManager);
	//mUiManager = 0;
	UiLepra::Shutdown();
}

void App::Init()
{
}


int App::Run()
{
	StdioConsoleLogListener lConsoleLogger;
	DebuggerLogListener lDebugLogger;
	LogType::GetLog(LogType::SUB_ROOT)->SetupBasicListeners(&lConsoleLogger, &lDebugLogger, 0, 0, 0);

	UiLepra::Init();
	bool lOk = true;
	if (lOk)
	{
		//mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());
		//lOk = ...;
	}
	if (lOk)
	{
		//lOk = Network::Start();
	}
	if (lOk)
	{
		mGame = new SlimeVolleyball;
		lOk = mGame->init();
	}
	bool lQuit = false;
	while (lOk && !lQuit)
	{
		lOk = Poll(mGame);
		lQuit = (SystemManager::GetQuitRequest() != 0);
	}
	return lQuit? 0 : 1;
}

bool Poll()
{
	UiLepra::Core::ProcessMessages();
	return true;
}



}
