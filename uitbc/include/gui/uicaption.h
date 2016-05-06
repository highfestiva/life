
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uirectcomponent.h"
#include "uibutton.h"
#include "uilabel.h"
#include "uitextcomponent.h"



namespace uitbc {



class Caption: public RectComponent, public TextComponent {
public:

	friend class Window;

	enum {
		kMinimizeButton = (1 << 0),
		kMaximizeButton = (1 << 1),
		kCloseButton    = (1 << 2),
	};

	Caption(const Color& active_color, const Color& inactive_color, int height);
	Caption(const Color& active_top_left_color,
			const Color& active_top_right_color,
			const Color& active_bottom_left_color,
			const Color& active_bottom_right_color,
			const Color& inactive_top_left_color,
			const Color& inactive_top_right_color,
			const Color& inactive_bottom_left_color,
			const Color& inactive_bottom_right_color,
			int height);
	Caption(Painter::ImageID active_left_image_id,
			Painter::ImageID active_right_image_id,
			Painter::ImageID active_center_image_id,
			Painter::ImageID inactive_left_image_id,
			Painter::ImageID inactive_right_image_id,
			Painter::ImageID inactive_center_image_id,
			int height);

	virtual ~Caption();

	Button* SetLeftButton(Button* button);
	Button* SetRightButton(Button* button);
	Button* SetMiddleButton(Button* button);

	void SetIcon(Painter::ImageID icon_id);
	void SetText(const wstr& text,
						const Color& active_text_color,
						const Color& active_backg_color,
						const Color& inactive_text_color,
						const Color& inactive_backg_color);

	void SetActive(bool active);
	bool GetActive() const;

	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);

	virtual bool OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y);

	virtual Type GetType() const;

protected:
	Button* SetButton(Button*& member_button, Button* new_button);
	void ForceRepaint();

private:

	enum Style {
		kSinglecolorStyle = 0,
		kMulticolorStyle,
		kImageStyle,
	};

	void InitCaption();

	bool Check(unsigned flags, unsigned pFlag);

	Label* label_;
	RectComponent* caption_rect_;
	RectComponent* button_rect_;
	RectComponent* left_image_rect_;
	RectComponent* center_image_rect_;
	RectComponent* right_image_rect_;

	Button* left_button_;
	Button* right_button_;
	Button* middle_button_;

	Color active_tl_color_;
	Color active_tr_color_;
	Color active_bl_color_;
	Color active_br_color_;

	Color inactive_tl_color_;
	Color inactive_tr_color_;
	Color inactive_bl_color_;
	Color inactive_br_color_;

	Painter::ImageID active_left_image_id_;
	Painter::ImageID active_right_image_id_;
	Painter::ImageID active_center_image_id_;
	Painter::ImageID inactive_left_image_id_;
	Painter::ImageID inactive_right_image_id_;
	Painter::ImageID inactive_center_image_id_;

	wstr text_;
	Color active_text_backg_color_;
	Color inactive_text_color_;
	Color inactive_text_backg_color_;

	bool moving_window_;
	bool active_;
	Style style_;
};



}
