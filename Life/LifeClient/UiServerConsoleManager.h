
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../LifeServer/ServerConsoleManager.h"

namespace Lepra
{
class PixelRect;
};
namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class UiConsole;



class UiServerConsoleManager: public ServerConsoleManager
{
	typedef ServerConsoleManager Parent;
public:
	UiServerConsoleManager(Cure::ResourceManager* pResourceManager, Cure::GameManager* pGameManager,
		UiCure::GameUiManager* pUiManager, Cure::RuntimeVariableScope* pVariableScope, const PixelRect& pArea,
		InteractiveConsoleLogListener* pConsoleLogger, ConsolePrompt* pConsolePrompt);
	virtual ~UiServerConsoleManager();

	virtual bool Start();
	virtual void Join();

	bool ToggleVisible();
	UiConsole* GetUiConsole() const;

private:
	UiConsole* mUiConsole;
	logclass();
};



}
