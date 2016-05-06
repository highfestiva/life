
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/graphics2d.h"
#include "../../uitbc/include/uifontmanager.h"
#include "../life.h"

namespace uitbc {
class Component;
class TextArea;
class TextField;
}
namespace UiCure {
class GameUiManager;
}



namespace life {



class ConsoleManager;




class UiConsole {
public:
	UiConsole(ConsoleManager* manager, UiCure::GameUiManager* ui_manager, const PixelRect& area);
	virtual ~UiConsole();

	void Open();
	void Close();

	void SetRenderArea(const PixelRect& render_area);
	void SetColor(const Color& color);
	bool ToggleVisible();
	void SetVisible(bool visible);
	bool IsVisible() const;
	void Tick();
	UiCure::GameUiManager* GetUiManager() const;
	uitbc::FontManager::FontId GetFontId() const;

private:
	void InitGraphics();
	void CloseGraphics();
	void OnConsoleChange();
	void PrintHelp();

	ConsoleManager* manager_;
	UiCure::GameUiManager* ui_manager_;
	PixelRect area_;
	Color color_;
	uitbc::Component* console_component_;
	uitbc::TextArea* console_output_;
	uitbc::TextField* console_input_;
	bool is_console_visible_;
	bool is_first_console_use_;
	float console_target_position_;
	uitbc::FontManager::FontId font_id_;

	logclass();
};



}
