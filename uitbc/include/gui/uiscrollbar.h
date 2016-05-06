
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/timer.h"
#include "uirectcomponent.h"
#include "uibutton.h"
#include "uicleaner.h"



namespace uitbc {



class ScrollBar: public RectComponent {
	typedef RectComponent Parent;
public:

	enum Style {
		kHorizontal = 0,
		kVertical,
	};

	ScrollBar(Style style = kHorizontal,
		  int size = 16, int button_size = 16,
		  const Color& body_color = Color(192, 192, 192),
		  const Color& backg_color = Color(192, 192, 192),
		  BorderComponent::BorderShadeFunc border_shade_func = BorderComponent::kLinear,
		  int border_width = 3);

	ScrollBar(Style style,
		  Painter::ImageID backg_image_id,
		  Button* top_left_button,
		  Button* bottom_right_button,
		  Button* scroller_button);

	// 0 <= visible <= max.
	void SetScrollRatio(float64 visible, float64 max);
	float64 GetScrollRatioVisible() const;
	float64 GetScrollRatioMax() const;

	// Makes the scroller width/height static with the given size.
	void SetStaticScrollerSize(int scroller_size);

	// Scroll position is in the range [0, 1].
	float64 GetScrollPos();
	void SetScrollPos(float64 pos);

	void SetScrollDelay(float64 first_delay, float64 delay);

	virtual void Repaint(Painter* painter);

	virtual void DoSetSize(int width, int height);

	virtual void OnConnectedToDesktopWindow();
	virtual void OnIdle();

	virtual Type GetType() const;

	void SetOwner(Component* owner);

protected:

	GridLayout* CreateLayout(Style style);
	void LoadIcons();
	void SetupScrollButton();
	void LoadButtons();
	void CheckAndSetSize();
	void CheckButtonSize(Button* button);
	void InitPreferredSize();
	void DoLayout();

	void OnScrollTL(Button* button);
	void OnScrollBR(Button* button);
	void OnStopScroll(Button* button);
	void OnScrollerDown(Button* button);
	bool OnScrollerDragged(Button* button, int mouse_x, int mouse_y, int delta_x, int delta_y);

private:

	void AddImage(Painter::ImageID& image_id, uint8 image[], int dim);

	// Cleans up among the static variables.
	class ScrollbarCleaner : public Cleaner {
	public:
		~ScrollbarCleaner() {
			ScrollBar::icon_left_id_  = Painter::kInvalidImageid;
			ScrollBar::icon_right_id_ = Painter::kInvalidImageid;
			ScrollBar::icon_up_id_    = Painter::kInvalidImageid;
			ScrollBar::icon_down_id_  = Painter::kInvalidImageid;
			ScrollBar::prev_painter_ = 0;
			ScrollBar::cleaner_ = 0;
		}
	};

	static uint8 icon_arrow_left_[];
	static uint8 icon_arrow_right_[];
	static uint8 icon_arrow_up_[];
	static uint8 icon_arrow_down_[];

	static Painter::ImageID icon_left_id_;
	static Painter::ImageID icon_right_id_;
	static Painter::ImageID icon_up_id_;
	static Painter::ImageID icon_down_id_;

	static Painter* prev_painter_;
	static ScrollbarCleaner* cleaner_;

	Style style_;

	bool user_defined_gfx_;

	Painter::ImageID backg_image_id_;
	Button* tl_button_;
	Button* br_button_;
	Button* scroller_button_;
	RectComponent* tl_rect_;
	RectComponent* br_rect_;

	Color body_color_;

	float64 visible_;
	float64 max_;
	float64 pos_;
	float64 scroll_speed_;
	float64 first_delay_;
	float64 delay_;

	Timer timer_;
	bool first_delay_done_;

	int size_;
	int button_size_;
	int scroller_size_;
	BorderComponent::BorderShadeFunc border_shade_func_;
	int border_width_;

	Component* owner_;
};



}
