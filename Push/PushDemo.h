
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "pushmanager.h"
#include "roadsignbutton.h"



#ifdef PUSH_DEMO
namespace Push {



// This is just a simple viewer that shows a background for menus and shows off in demo mode.
class PushDemo: public PushManager {
	typedef PushManager Parent;
public:
	PushDemo(GameClientMasterTicker* pMaster, const cure::TimeManager* time,
		cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
		UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area);
	virtual ~PushDemo();

private:
	virtual bool Paint();
	virtual void TickUiInput();
	virtual void TickUiUpdate();
	virtual void CreateLoginView();
	virtual bool InitializeUniverse();
	virtual void OnLoadCompleted(cure::ContextObject* object, bool ok);
	void BrowseFullInfo(uitbc::Button*);

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual void OnInput(uilepra::InputElement* element);

	uitbc::FontManager::FontId PushDemo::SetFontHeight(double height);

	float camera_angle_;

	float info_text_x_;
	float info_text_target_y_;
	float info_text_slide_y_;
	str info_text_;
	int current_info_text_index_;
	static const tchar* info_text_array_[6];
};



}
#endif // Demo
