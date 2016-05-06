
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "uibutton.h"
#include "uifloatinglayout.h"
#include "uilabel.h"



namespace uitbc {



class Dialog: public RectComponent {
	typedef RectComponent Parent;
public:
	typedef Button::Delegate Action;

	Dialog(Component* parent, Action target);
	virtual ~Dialog();
	void Dismiss();

	virtual void SetColor(const Color& top_left_color, const Color& top_right_color, const Color& bottom_right_color, const Color& bottom_left_color);
	void SetDirection(int direction, bool set_pos);
	void SetPreClickTarget(Action pre_click_target);
	void SetPostClickTarget(Action post_click_target);
	virtual void Center();
	Label* SetQueryLabel(const wstr& text, uitbc::FontManager::FontId font_id);
	void SetQueryLabel(Label* label);
	void UpdateQueryLabel(const wstr& text, const Color& color);
	void AddButton(int tag, const wstr& text, bool auto_dismiss);	// Tag < 0 to layout yourself.
	void AddButton(int tag, Button* button, bool auto_dismiss);	// Tag < 0 to layout yourself.
	void SetButtonHandler(int tag, Button* button, bool auto_dismiss);	// Tag < 0 to layout yourself.
	bool IsAutoDismissButton(Button* button) const;
	void SetOffset(PixelCoord offset);
	virtual void UpdateLayout();

	virtual void Repaint(Painter* painter);
	void Animate();
	void OnDismissClick(Button* button);
	void OnClick(Button* button);
	void DoClick(Button* button);

protected:
	typedef std::vector<Button*> ButtonList;

	PixelCoord offset_;
	Label* label_;
	Action target_;
	Action pre_click_target_;
	Action post_click_target_;
	ButtonList button_list_;
	Button* clicked_button_;
	bool is_closing_;
	int animation_step_;
	int direction_;
};



}
