
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/Graphics2D.h"
#include "../Life.h"

namespace UiTbc
{
class Component;
class TextArea;
class TextField;
}
namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class ConsoleManager;




class UiConsole
{
public:
	UiConsole(ConsoleManager* pManager, UiCure::GameUiManager* pUiManager, const PixelRect& pArea);
	virtual ~UiConsole();

	void Open();
	void Close();

	void SetRenderArea(const PixelRect& pRenderArea);
	void SetColor(const Color& pColor);
	bool ToggleVisible();
	void SetVisible(bool pVisible);
	void Tick();
	UiCure::GameUiManager* GetUiManager() const;

private:
	void InitGraphics();
	void CloseGraphics();
	void OnConsoleChange();
	void PrintHelp();

	ConsoleManager* mManager;
	UiCure::GameUiManager* mUiManager;
	PixelRect mArea;
	Color mColor;
	UiTbc::Component* mConsoleComponent;
	UiTbc::TextArea* mConsoleOutput;
	UiTbc::TextField* mConsoleInput;
	bool mIsConsoleVisible;
	bool mIsFirstConsoleUse;
	float mConsoleTargetPosition;

	LOG_CLASS_DECLARE();
};



}
