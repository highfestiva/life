
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "uiconsole.h"
#include "../../cure/include/gamemanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uitbc/include/gui/uiconsoleloglistener.h"
#include "../../uitbc/include/gui/uiconsoleprompt.h"
#include "../../uitbc/include/gui/uidesktopwindow.h"
#include "../../uitbc/include/gui/uifilenamefield.h"
#include "../../uitbc/include/gui/uifloatinglayout.h"
#include "../../uitbc/include/gui/uitextarea.h"
#include "../consolemanager.h"
#include "rtvar.h"



namespace life {



UiConsole::UiConsole(ConsoleManager* manager, UiCure::GameUiManager* ui_manager, const PixelRect& area):
	manager_(manager),
	ui_manager_(ui_manager),
	area_(area),
	color_(10, 20, 30, 230),
	console_component_(0),
	console_output_(0),
	console_input_(0),
	is_console_visible_(false),
	is_first_console_use_(true),
	console_target_position_(0) {
	if (manager_->GetConsoleLogger()) {
#ifdef NO_LOG_DEBUG_INFO
		const LogLevel allowed_level = kLevelInfo;
#else // Allow debug logging.
		const LogLevel allowed_level = kLevelLowestType;
#endif // Disallow/allow debug logging.
		for (int x = allowed_level; x < kLevelTypeCount; ++x) {
			LogType::GetLogger(LogType::kRoot)->AddListener(manager_->GetConsoleLogger(), (LogLevel)x);
		}
	}

	font_id_ = uitbc::FontManager::kInvalidFontid;
	const char* font_names[] =
	{
		"Courier New",
		"LiberationMono",
		"DejaVuSansMono",
		"Mono",
		"",
		0
	};
	uitbc::FontManager::FontId default_font_id = ui_manager_->GetFontManager()->GetActiveFontId();
	for (int x = 0; font_names[x] && font_id_ == uitbc::FontManager::kInvalidFontid; ++x) {
		font_id_ = ui_manager_->GetFontManager()->QueryAddFont(font_names[x], 14.0f);
	}
	ui_manager_->GetFontManager()->SetActiveFont(default_font_id);
}

UiConsole::~UiConsole() {
	Close();
	ui_manager_ = 0;
	manager_ = 0;
}



void UiConsole::Open() {
	InitGraphics();

	((uitbc::ConsoleLogListener*)manager_->GetConsoleLogger())->SetOutputComponent(console_output_);
	((uitbc::ConsolePrompt*)manager_->GetConsolePrompt())->SetInputComponent(console_input_);

	OnConsoleChange();
}

void UiConsole::Close() {
	if (manager_->GetConsoleLogger()) {
		((uitbc::ConsoleLogListener*)manager_->GetConsoleLogger())->SetOutputComponent(0);
	}
	((uitbc::ConsolePrompt*)manager_->GetConsolePrompt())->SetInputComponent(0);
	CloseGraphics();
}



void UiConsole::SetRenderArea(const PixelRect& render_area) {
	area_ = render_area;

	if (console_component_) {
		PixelCoord __size = area_.GetSize();
		__size.y = (int)(__size.y*0.6);	// TODO: use setting for how high console should be.
		console_component_->SetPreferredSize(__size);
		console_input_->ActivateFont(ui_manager_->GetPainter());
		int input_height = ui_manager_->GetPainter()->GetFontHeight()+4;
		console_input_->DeactivateFont(ui_manager_->GetPainter());
		__size.y -= input_height;
		console_output_->SetPreferredSize(__size);
		__size.y = input_height;
		console_input_->SetPreferredSize(__size);
	}
}

void UiConsole::SetColor(const Color& color) {
	color_ = color;
}

bool UiConsole::ToggleVisible() {
	SetVisible(!is_console_visible_);
	return (is_console_visible_);
}

void UiConsole::SetVisible(bool visible) {
	is_console_visible_ = visible;
	OnConsoleChange();
}

bool UiConsole::IsVisible() const {
	return is_console_visible_;
}

void UiConsole::Tick() {
	if (!console_component_) {
		return;
	}

	const float frame_time = manager_->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime();
	float console_speed;
	v_get(console_speed, =(float), manager_->GetVariableScope(), kRtvarCtrlUiConspeed, 2.7);
	const float frame_console_speed = std::min(1.0f, Math::GetIterateLerpTime(console_speed, frame_time));
	if (is_console_visible_) {
		if (area_.top_ == 0) {	// Slide down.
			console_target_position_ = Math::Lerp(console_target_position_, (float)area_.top_, frame_console_speed);
			console_component_->SetPos(area_.left_, (int)console_target_position_);
		} else {	// Slide sideways.
			console_target_position_ = Math::Lerp(console_target_position_, (float)area_.left_, frame_console_speed);
			console_component_->SetPos((int)console_target_position_, area_.top_);
		}
	} else {
		const int margin = 3;
		if (area_.top_ == 0) {	// Slide out top.
			const int target = -console_component_->GetSize().y-margin;
			console_target_position_ = Math::Lerp(console_target_position_, (float)target, frame_console_speed);
			console_component_->SetPos(area_.left_, (int)console_target_position_);
			if (console_component_->GetPos().y <= target+margin) {
				console_component_->SetVisible(false);
			}
		} else if (area_.left_ == 0) {	// Slide out left.
			const int target = -console_component_->GetSize().x-margin;
			console_target_position_ = Math::Lerp(console_target_position_, (float)target, frame_console_speed);
			console_component_->SetPos((int)console_target_position_, area_.top_);
			if (console_component_->GetPos().x <= target+margin) {
				console_component_->SetVisible(false);
			}
		} else {	// Slide out right.
			const int target = ui_manager_->GetDisplayManager()->GetWidth()+margin;
			console_target_position_ = Math::Lerp(console_target_position_, (float)target, frame_console_speed);
			console_component_->SetPos((int)console_target_position_, area_.top_);
			if (console_component_->GetPos().x >= target+margin) {
				console_component_->SetVisible(false);
			}
		}
	}
}

UiCure::GameUiManager* UiConsole::GetUiManager() const {
	return ui_manager_;
}

uitbc::FontManager::FontId UiConsole::GetFontId() const {
	return font_id_;
}


void UiConsole::InitGraphics() {
	CloseGraphics();

	console_component_ = new uitbc::Component(new uitbc::ListLayout());
	console_output_ = new uitbc::TextArea(color_);
	Color input_color = color_ - color_ * 0.3f;
	input_color.alpha_ = color_.alpha_;
	console_input_ = new uitbc::TextField(console_component_, input_color);

	SetRenderArea(area_);

	console_output_->SetHorizontalMargin(3);
	console_output_->SetFocusAnchor(uitbc::TextArea::kAnchorBottomLine);
	console_output_->SetFontId(font_id_);
	console_output_->SetFontColor(WHITE);
	console_input_->SetFontId(font_id_);
	console_input_->SetFontColor(WHITE);

	console_component_->AddChild(console_output_);
	console_component_->AddChild(console_input_);

	ui_manager_->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
	ui_manager_->GetDesktopWindow()->AddChild(console_component_, 0, 0, 0);
	console_component_->SetPos(area_.left_, area_.top_);
	console_component_->SetVisible(false);

	// This is just for getting some basic metrics (such as font height).
	console_component_->Repaint(ui_manager_->GetPainter());
}

void UiConsole::CloseGraphics() {
	if (ui_manager_ && console_component_) {
		ui_manager_->GetDesktopWindow()->RemoveChild(console_component_, 0);
		console_component_->RemoveChild(console_output_, 0);
		console_component_->RemoveChild(console_input_, 0);
		console_component_->SetVisible(false);
	}

	delete (console_component_);
	console_component_ = 0;
	delete (console_output_);
	console_output_ = 0;
	delete (console_input_);
	console_input_ = 0;
}

void UiConsole::OnConsoleChange() {
	if (!console_component_) {
		return;
	}

	if (is_console_visible_) {
		console_component_->SetVisible(true);
		ui_manager_->GetDesktopWindow()->UpdateLayout();
		manager_->GetConsolePrompt()->SetFocus(true);
		if (is_first_console_use_) {
			is_first_console_use_ = false;
			PrintHelp();
		}
	} else {
		manager_->GetConsolePrompt()->SetFocus(false);
	}
}

void UiConsole::PrintHelp() {
	str keys;
	v_get(keys, =, manager_->GetVariableScope(), kRtvarCtrlUiContoggle, "???");
	typedef strutil::strvec SV;
	SV key_array = strutil::Split(keys, ", \t");
	SV nice_keys;
	for (SV::iterator x = key_array.begin(); x != key_array.end(); ++x) {
		const str key = strutil::ReplaceAll(*x, "Key.", "");
		nice_keys.push_back(key);
	}
	str key_info;
	if (key_array.size() == 1) {
		key_info = "key ";
	} else {
		key_info = "any of the following keys: ";
	}
	key_info += strutil::Join(nice_keys, ", ");
	log_.Infof("To bring this console up again press %s.", key_info.c_str());
}



loginstance(kConsole, UiConsole);



}
