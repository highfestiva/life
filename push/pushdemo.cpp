
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "pushdemo.h"
#ifdef PUSH_DEMO
#include "../../uitbc/include/gui/uidesktopwindow.h"
#include "../../cure/include/contextmanager.h"
#include "../../cure/include/runtimevariable.h"
#include "../../cure/include/timemanager.h"
#include "../../lepra/include/random.h"
#include "../../lepra/include/systemmanager.h"
#include "../../tbc/include/chunkyphysics.h"
#include "../../uicure/include/uiruntimevariablename.h"
#include "../../uitbc/include/uifontmanager.h"
#include "gameclientmasterticker.h"
#include "Machine.h"



namespace Push {



PushDemo::PushDemo(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	camera_angle_(0),
	info_text_x_(-10000),
	info_text_target_y_(-100),
	info_text_slide_y_(-100),
	current_info_text_index_(0) {
	ui_manager->GetPainter()->ClearFontBuffers();
}

PushDemo::~PushDemo() {
}



Merge OnOpen stuff from PushViewer.cpp.



bool PushDemo::Paint() {
	if (!ui_manager_->GetDisplayManager()->IsVisible()) {
		return true;
	}

	const double font_height = 70.0;
	const uitbc::FontManager::FontId old_font_id = SetFontHeight(font_height);

	if (ui_manager_->GetFontManager()->GetStringWidth(info_text_)+info_text_x_ < 0) {
		info_text_x_ = ui_manager_->GetDisplayManager()->GetWidth() * 1.5f;
		info_text_target_y_ = Random::Uniform(20.0f, (float)(ui_manager_->GetDisplayManager()->GetHeight()-font_height-60));
		const int text_count = sizeof(info_text_array_)/sizeof(void*);
		if (info_text_.empty()) {
			current_info_text_index_ = Random::GetRandomNumber() % text_count;
		}
		if (++current_info_text_index_ >= text_count) {
			current_info_text_index_ = 0;
		}
		info_text_ = info_text_array_[current_info_text_index_];
	}
	const float frame_time = GetTimeManager()->GetRealNormalFrameTime();
	info_text_slide_y_ = Math::Lerp(info_text_slide_y_, info_text_target_y_, Math::GetIterateLerpTime(0.8f, frame_time));

	const int width = ui_manager_->GetDisplayManager()->GetWidth();
	const int bar_margin = 5;
	ui_manager_->GetPainter()->PushAttrib(uitbc::Painter::kAttrRendermode);
	ui_manager_->GetPainter()->SetRenderMode(uitbc::Painter::kRmAdd);
	ui_manager_->GetPainter()->SetColor(Color(160, 160, 160, 180), 0);
	ui_manager_->GetPainter()->FillRect(0, (int)info_text_slide_y_-bar_margin, width-1, (int)(info_text_slide_y_+ui_manager_->GetFontManager()->GetFontHeight()+bar_margin*2));
	ui_manager_->GetPainter()->PopAttrib();
	info_text_x_ -= frame_time * 640;
	//info_text_x_ -= 12;
	ui_manager_->GetPainter()->SetColor(Color(30, 10, 20, 220), 0);
	ui_manager_->GetPainter()->SetColor(Color(0, 0, 0, 0), 1);
	ui_manager_->GetPainter()->PrintText(info_text_, (int)info_text_x_, (int)info_text_target_y_);

	ui_manager_->GetFontManager()->SetActiveFont(old_font_id);

	return (true);
}

void PushDemo::TickUiInput() {
}

void PushDemo::TickUiUpdate() {
	camera_previous_position_ = camera_position_;
	cure::ContextObject* _object = GetContext()->GetObject(avatar_id_);
	if (!_object) {
		return;
	}
	camera_pivot_position_ = _object->GetPosition();
	quat rotation;
	rotation.RotateAroundWorldZ(camera_angle_);
	camera_position_ = camera_pivot_position_ - rotation*vec3(10+sin(camera_angle_*4.3f), 0, 0);
	camera_previous_position_ = camera_position_;
	camera_orientation_ = vec3(camera_angle_, PIF/2, 0);
	camera_angle_ += GetTimeManager()->GetNormalFrameTime();
}

void PushDemo::CreateLoginView() {
	uitbc::Button* button = new uitbc::Button(uitbc::BorderComponent::kLinear, 3, GRAY, "FullInfo");
	button->SetText("Click here for more information on the full version", BLUE, BLUE);
	const int button_width = 370;
	const int button_height = 30;
	const int margin = 20;
	button->SetPreferredSize(button_width, button_height);
	button->SetMinSize(button_width, button_height);
	button->SetSize(button_width, button_height);
	button->UpdateLayout();
	button->SetOnClick(PushDemo, BrowseFullInfo);
	GetUiManager()->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
	ui_manager_->GetDesktopWindow()->AddChild(button, 0, 0, 0);
	button->SetPos(margin, ui_manager_->GetDisplayManager()->GetHeight()-button_height-margin);
}

bool PushDemo::InitializeUniverse() {
	Merge level stuff from PushViewer.cpp.

	cure::ContextObject* vehicle = new Machine(GetResourceManager(), "saucer_01", ui_manager_);
	GetContext()->AddLocalObject(vehicle);
	avatar_id_ = vehicle->GetInstanceId();
	vehicle->SetPhysicsTypeOverride(cure::kPhysicsOverrideStatic);
	vehicle->StartLoading();
	return (true);
}

void PushDemo::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (ok) {
	} else {
		Parent::OnLoadCompleted(object, ok);
	}
}

void PushDemo::BrowseFullInfo(uitbc::Button*) {
	SystemManager::WebBrowseTo("http://trialepicfail.blogspot.com");
	SystemManager::AddQuitRequest(+1);
}



bool PushDemo::OnKeyDown(uilepra::InputManager::KeyCode) {
	return (false);
}

bool PushDemo::OnKeyUp(uilepra::InputManager::KeyCode) {
	return (false);
}

void PushDemo::OnInput(uilepra::InputElement*) {
}



uitbc::FontManager::FontId PushDemo::SetFontHeight(double height) {
	const uitbc::FontManager::FontId previous_font_id = ui_manager_->GetFontManager()->GetActiveFontId();
	const str font_name = ui_manager_->GetFontManager()->GetActiveFontName();
	ui_manager_->GetFontManager()->QueryAddFont(font_name, height, uitbc::FontManager::kBold);
	return (previous_font_id);
}



const tchar* PushDemo::info_text_array_[6] =
{
	"The vehicle on display is only playable in the full version.",
	"In the full version you can play online. Playing with your kids from the other side of the planet has never been easier.",
	"Play with up to four simultanous players ON ONE SCREEN in the full version - this free demo version adds annoying texts for 3/4 players.",
	"Ten more groovy vehicles are available in the full version.",
	"Host a dedicated game server for family and friends. Only in the full version.",
	"The development of this game was a huge undertaking by a single individual. Hope you enjoyed it!",
};



}
#endif //Demo
