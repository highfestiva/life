
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uiscrollbar.h"
#include "../../include/gui/uigridlayout.h"
#include "../../include/gui/uibutton.h"
#include "../../include/gui/uidesktopwindow.h"
#include "../../../lepra/include/log.h"
#include <math.h>



namespace uitbc {



uint8 ScrollBar::icon_arrow_left_[] =

{	0  , 0  ,0  , 0  , 0  , 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 255, 255, 0  , 0  ,
	0  , 0  ,0  , 255, 255, 255, 0  , 0  ,
	0  , 0  ,255, 255, 255, 255, 0  , 0  ,
	0  , 0  ,0  , 255, 255, 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 255, 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 0  , 255, 0  , 0  ,
	0  , 0  ,0  , 0  , 0  , 0  , 0  , 0  ,
};

uint8 ScrollBar::icon_arrow_right_[] =

{	 0  , 0  ,255, 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 0  , 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 255, 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 255, 255, 0  , 0  ,
	 0  , 0  ,255, 255, 255, 0  , 0  , 0  ,
	 0  , 0  ,255, 255, 0  , 0  , 0  , 0  ,
	 0  , 0  ,255, 0  , 0  , 0  , 0  , 0  ,
	 0  , 0  ,0  , 0  , 0  , 0  , 0  , 0  ,
};

uint8 ScrollBar::icon_arrow_up_[] =

{	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 255, 0  , 0  , 0  , 0  ,
	0  , 0  , 255, 255, 255, 0  , 0  , 0  ,
	0  , 255, 255, 255, 255, 255, 0  , 0  ,
	255, 255, 255, 255, 255, 255, 255, 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
};

uint8 ScrollBar::icon_arrow_down_[] =

{	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	255, 255, 255, 255, 255, 255, 255, 0  ,
	0  , 255, 255, 255, 255, 255, 0  , 0  ,
	0  , 0  , 255, 255, 255, 0  , 0  , 0  ,
	0  , 0  , 0  , 255, 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
	0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,
};

Painter::ImageID ScrollBar::icon_left_id_  = Painter::kInvalidImageid;
Painter::ImageID ScrollBar::icon_right_id_ = Painter::kInvalidImageid;
Painter::ImageID ScrollBar::icon_up_id_    = Painter::kInvalidImageid;
Painter::ImageID ScrollBar::icon_down_id_  = Painter::kInvalidImageid;

Painter* ScrollBar::prev_painter_ = 0;
ScrollBar::ScrollbarCleaner* ScrollBar::cleaner_ = 0;

ScrollBar::ScrollBar(Style style, int size,  int button_size, const Color& body_color,
	const Color& backg_color, BorderComponent::BorderShadeFunc border_shade_func,
	int border_width):
	RectComponent(backg_color, CreateLayout(style)),
	style_(style),
	user_defined_gfx_(false),
	backg_image_id_(Painter::kInvalidImageid),
	tl_button_(0),
	br_button_(0),
	scroller_button_(0),
	tl_rect_(0),
	br_rect_(0),
	body_color_(body_color),
	visible_(1.0),
	max_(1.0),
	pos_(0.0),
	scroll_speed_(0),
	first_delay_(0.5),
	delay_(0.08),
	first_delay_done_(false),
	size_(size),
	button_size_(button_size),
	scroller_size_(0),
	border_shade_func_(border_shade_func),
	border_width_(border_width),
	owner_(0) {
	InitPreferredSize();

	LoadIcons();
	LoadButtons();
	SetupScrollButton();
	CheckAndSetSize();
	InitPreferredSize();

	DoLayout();
	UpdateLayout();
}

ScrollBar::ScrollBar(Style style, Painter::ImageID backg_image_id, Button* top_left_button,
	Button* bottom_right_button, Button* scroller_button):
	RectComponent(backg_image_id, CreateLayout(style)),
	style_(style),
	user_defined_gfx_(true),
	backg_image_id_(backg_image_id),
	tl_button_(top_left_button),
	br_button_(bottom_right_button),
	scroller_button_(scroller_button),
	tl_rect_(0),
	br_rect_(0),
	body_color_(0, 0, 0),
	visible_(1.0),
	max_(1.0),
	pos_(0.0),
	scroll_speed_(0),
	first_delay_(0.5),
	delay_(0.08),
	first_delay_done_(false),
	size_(0),
	button_size_(0),
	scroller_size_(0),
	border_shade_func_(BorderComponent::kLinear),
	border_width_(3),
	owner_(0) {
	SetupScrollButton();
	CheckAndSetSize();
	InitPreferredSize();
	DoLayout();
	UpdateLayout();
}

GridLayout* ScrollBar::CreateLayout(Style style) {
	if (style == kHorizontal) {
		return new GridLayout(1, 5);
	} else {
		return new GridLayout(5, 1);
	}
}



void ScrollBar::Repaint(Painter* painter) {
	DoLayout();
	Parent::Repaint(painter);
}

void ScrollBar::SetScrollRatio(float64 visible, float64 max) {
	if (max > 0.0) {
		max_ = max;
		visible_ = visible < 0 ? 0 : (visible > max_ ? max_ : visible);

		if (style_ == kHorizontal) {
			if (scroller_size_ == 0) {
				SetMinSize(2 * button_size_ + (int)((float64)border_width_ * 2.0), size_);
			} else {
				SetMinSize(2 * button_size_ + scroller_size_, size_);
			}
		} else {
			if (scroller_size_ == 0) {
				SetMinSize(size_, 2 * button_size_ + (int)((float64)border_width_ * 2.0));
			} else {
				SetMinSize(size_, 2 * button_size_ + scroller_size_);
			}
		}
	}

	SetNeedsRepaint(true);
}

float64 ScrollBar::GetScrollRatioVisible() const {
	return visible_;
}

float64 ScrollBar::GetScrollRatioMax() const {
	return max_;
}

void ScrollBar::SetScrollPos(float64 pos) {
	pos_ = pos < 0 ? 0 : (pos > 1 ? 1 : pos);
	SetNeedsRepaint(true);
	if (owner_ != 0) {
		owner_->UpdateLayout();
	}
}

void ScrollBar::DoSetSize(int width, int height) {
	Parent::DoSetSize(width, height);
	DoLayout();
}

void ScrollBar::LoadIcons() {
	if (style_ == kHorizontal) {
		AddImage(icon_left_id_, icon_arrow_left_, 8);
		AddImage(icon_right_id_, icon_arrow_right_, 8);
	} else {
		AddImage(icon_up_id_, icon_arrow_up_, 8);
		AddImage(icon_down_id_, icon_arrow_down_, 8);
	}
}

void ScrollBar::AddImage(Painter::ImageID& image_id, uint8 image[], int dim) {
	GUIImageManager* i_man = GetImageManager();

	if (prev_painter_ != i_man->GetPainter()) {
		icon_left_id_  = Painter::kInvalidImageid;
		icon_right_id_ = Painter::kInvalidImageid;
		icon_up_id_    = Painter::kInvalidImageid;
		icon_down_id_  = Painter::kInvalidImageid;
		prev_painter_ = i_man->GetPainter();
	}

	if (image_id == Painter::kInvalidImageid) {
		Canvas canvas(dim, dim, Canvas::kBitdepth8Bit);
		canvas.SetBuffer(image);
		Canvas alpha(canvas, true);
		canvas.ConvertTo32BitWithAlpha(alpha);
		image_id = i_man->AddImage(canvas, GUIImageManager::kCentered, GUIImageManager::kAlphatest, 128);
	}
}

void ScrollBar::SetupScrollButton() {
	if (scroller_button_ == 0) {
		if (style_ == kHorizontal)
			scroller_button_ = new Button(border_shade_func_, border_width_, body_color_, L"");
		else
			scroller_button_ = new Button(border_shade_func_, border_width_, body_color_, L"");
	}

	if (style_ == kHorizontal)
		AddChild(scroller_button_, 0, 2);
	else
		AddChild(scroller_button_, 2, 0);

	scroller_button_->SetOnPress(ScrollBar, OnScrollerDown);
	scroller_button_->SetOnDrag(ScrollBar, OnScrollerDragged);
}

void ScrollBar::LoadButtons() {
	if (tl_button_ == 0) {
		if (style_ == kHorizontal) {
			tl_button_ = new Button(border_shade_func_, border_width_, body_color_, L"");
			tl_button_->SetIcon(icon_left_id_, Button::kIconCenter);
		} else {
			tl_button_ = new Button(border_shade_func_, border_width_, body_color_, L"");
			tl_button_->SetIcon(icon_up_id_, Button::kIconCenter);
		}
	}

	if (style_ == kHorizontal)
		AddChild(tl_button_, 0, 0);
	else
		AddChild(tl_button_, 0, 0);

	if (br_button_ == 0) {
		if (style_ == kHorizontal) {
			br_button_ = new Button(border_shade_func_, border_width_, body_color_, L"");
			br_button_->SetIcon(icon_right_id_, Button::kIconCenter);
		} else {
			br_button_ = new Button(border_shade_func_, border_width_, body_color_, L"");
			br_button_->SetIcon(icon_down_id_, Button::kIconCenter);
		}
	}

	if (style_ == kHorizontal)
		AddChild(br_button_, 0, 4);
	else
		AddChild(br_button_, 4, 0);

	// Finally add the two dummy rects.
	tl_rect_ = new RectComponent;
	br_rect_ = new RectComponent;

	if (style_ == kHorizontal) {
		AddChild(tl_rect_, 0, 1);
		AddChild(br_rect_, 0, 3);

		SetMinSize(2 * button_size_ + border_width_ * 2, size_);
	} else {
		AddChild(tl_rect_, 1, 0);
		AddChild(br_rect_, 3, 0);

		SetMinSize(size_, 2 * button_size_ + border_width_ * 2);
	}

	tl_button_->SetOnPress(ScrollBar, OnScrollTL);
	br_button_->SetOnPress(ScrollBar, OnScrollBR);
	tl_button_->SetOnRelease(ScrollBar, OnStopScroll);
	br_button_->SetOnRelease(ScrollBar, OnStopScroll);
}

void ScrollBar::CheckButtonSize(Button* button) {
	if (button != 0) {
		if (style_ == kHorizontal) {
			if (button->GetPreferredHeight() > size_) {
				size_ = button->GetPreferredHeight();
			}

			if (button->GetPreferredWidth() > button_size_) {
				button_size_ = button->GetPreferredWidth();
			}
		} else {
			if (button->GetPreferredWidth() > size_) {
				size_ = button->GetPreferredWidth();
			}

			if (button->GetPreferredHeight() > button_size_) {
				button_size_ = button->GetPreferredHeight();
			}
		}
	}
}

void ScrollBar::CheckAndSetSize() {
	SetBehaveSolid(true);
	CheckButtonSize(tl_button_);
	CheckButtonSize(br_button_);

	if (scroller_button_ != 0) {
		if (style_ == kHorizontal) {
			if (scroller_button_->GetPreferredHeight() > size_) {
				size_ = scroller_button_->GetPreferredHeight();
			}
		} else {
			if (scroller_button_->GetPreferredWidth() > size_) {
				size_ = scroller_button_->GetPreferredWidth();
			}
		}
	}

	if (size_ == 0) {
		size_ = 16;
	}
	if (button_size_ == 0) {
		button_size_ = 16;
	}
}

void ScrollBar::InitPreferredSize() {
	if (style_ == kHorizontal) {
		if (tl_button_ != 0) {
			tl_button_->SetPreferredSize(button_size_, size_);
		}

		if (br_button_ != 0) {
			br_button_->SetPreferredSize(button_size_, size_);
		}

		if (scroller_button_ != 0) {
			scroller_button_->SetPreferredSize(GetSize().x - 2 * button_size_, size_);
		}
	} else {
		if (tl_button_ != 0) {
			tl_button_->SetPreferredSize(size_, button_size_);
		}

		if (br_button_ != 0) {
			br_button_->SetPreferredSize(size_, button_size_);
		}

		if (scroller_button_ != 0) {
			scroller_button_->SetPreferredSize(size_, GetSize().y - 2 * button_size_);
		}
	}

	if (tl_rect_ != 0) {
		tl_rect_->SetPreferredSize(0, 0);
	}

	if (br_rect_ != 0) {
		br_rect_->SetPreferredSize(0, 0);
	}
}

void ScrollBar::DoLayout() {
	PixelRect rect(Parent::GetScreenRect());
	float64 ratio = visible_ / max_;

	PixelCoord min_size(GetMinSize());

	if (style_ == kHorizontal) {
		/* TODO: complete intention.
		if (rect.GetWidth() < min_size.x) {
			int stophere = 0;
		}*/

		int _width = rect.GetWidth();
		if (tl_rect_) {
			_width -= button_size_ * 2;
		}
		int scroller_width = (int)floor(((float64)_width * ratio));

		if (scroller_size_ != 0) {
			scroller_width = scroller_size_;
		} else if(scroller_width < scroller_button_->GetMinSize().x) {
			scroller_width = scroller_button_->GetMinSize().x;
		}

		int rest = _width - scroller_width;
		int scroll_pos = (int)floor((float64)rest * pos_);

		scroller_button_->SetPos(scroll_pos, 0);
		scroller_button_->SetPreferredSize(scroller_width, size_);
		scroller_button_->SetSize(scroller_width, size_);

		if (tl_rect_) {
			tl_rect_->SetPreferredSize(scroll_pos, size_);
			if (scroll_pos == 0) {
				br_rect_->SetPreferredSize(rest - scroll_pos, size_);
			} else {
				br_rect_->SetPreferredSize(0, size_);
			}

			tl_button_->SetPreferredSize(button_size_, size_);
			br_button_->SetPreferredSize(button_size_, size_);
		}
	} else {
		int _height = rect.GetHeight();
		if (tl_rect_) {
			_height -= button_size_ * 2;
		}
		int scroller_height = (int)floor(((float64)_height * ratio));

		if (scroller_size_ != 0) {
			scroller_height = scroller_size_;
		} else if(scroller_height < scroller_button_->GetMinSize().y) {
			scroller_height = scroller_button_->GetMinSize().y;
		}

		int rest = _height - scroller_height;
		int scroll_pos = (int)floor((float64)rest * pos_);

		scroller_button_->SetPos(0, scroll_pos);
		scroller_button_->SetPreferredSize(size_, scroller_height);
		scroller_button_->SetSize(size_, scroller_height);

		if (tl_rect_) {
			tl_rect_->SetPreferredSize(size_, scroll_pos);
			br_rect_->SetPreferredSize(size_, rest - scroll_pos);

			tl_button_->SetPreferredSize(size_, button_size_);
			br_button_->SetPreferredSize(size_, button_size_);
		}
	}
}

void ScrollBar::OnScrollTL(Button* /*button*/) {
	scroll_speed_ = -1.0 / (max_ - visible_);
	SetScrollPos(GetScrollPos() + scroll_speed_);
	timer_.UpdateTimer();
	timer_.ClearTimeDiff();
	first_delay_done_ = false;

	DesktopWindow* d_win = (DesktopWindow*)GetParentOfType(kDesktopwindow);
	d_win->AddIdleSubscriber(this);
}

void ScrollBar::OnScrollBR(Button* /*button*/) {
	scroll_speed_ = 1.0 / (max_ - visible_);
	SetScrollPos(GetScrollPos() + scroll_speed_);
	timer_.UpdateTimer();
	timer_.ClearTimeDiff();
	first_delay_done_ = false;
	DesktopWindow* d_win = (DesktopWindow*)GetParentOfType(kDesktopwindow);
	d_win->AddIdleSubscriber(this);
}

void ScrollBar::OnStopScroll(Button* /*button*/) {
	scroll_speed_ = 0;
	first_delay_done_ = false;
	DesktopWindow* d_win = (DesktopWindow*)GetParentOfType(kDesktopwindow);
	d_win->RemoveIdleSubscriber(this);
}

void ScrollBar::OnScrollerDown(Button* button) {
	if (user_defined_gfx_ == false) {
		unsigned _style = button->GetBorderStyle();
		_style &= ~Window::kBorderSunken;
		button->SetBorder(_style, button->GetBorderWidth());
	}
}

bool ScrollBar::OnScrollerDragged(Button* button, int mouse_x, int mouse_y, int delta_x, int delta_y) {

	PixelRect rect(GetScreenRect());
	PixelCoord _scroller_size(scroller_button_->GetSize());

	if (tl_rect_) {
		float64 delta_pos;
		if (style_ == kHorizontal) {
			delta_pos = (float64)delta_x / (float64)(rect.GetWidth() - (_scroller_size.x + 2 * button_size_));
		} else {
			delta_pos = (float64)delta_y / (float64)(rect.GetHeight() - (_scroller_size.y + 2 * button_size_));
		}
		SetScrollPos(GetScrollPos() + delta_pos);
	} else {
		float64 _pos;
		if (style_ == kHorizontal) {
			_pos = (mouse_x - rect.left_ - size_/2.0f) / (rect.GetWidth() - size_ - 2.0f);
		} else {
			_pos = (mouse_y - rect.top_ - size_/2.0f) / (rect.GetHeight() - size_ - 2.0f);
		}
		SetScrollPos(_pos);
	}

	if (user_defined_gfx_ == false) {
		unsigned _style = button->GetBorderStyle();
		_style &= ~Window::kBorderSunken;
		button->SetBorder(_style, button->GetBorderWidth());
	}
	return (false);
}

void ScrollBar::OnConnectedToDesktopWindow() {
	if(cleaner_ == 0) {
		DesktopWindow* desktop_window = (DesktopWindow*)GetParentOfType(kDesktopwindow);
		if(desktop_window != 0) {
			cleaner_ = new ScrollbarCleaner();
			desktop_window->AddCleaner(cleaner_);
		}
	}
}

void ScrollBar::OnIdle() {
	if (scroll_speed_ != 0) {
		timer_.UpdateTimer();
		float64 _delay = first_delay_;

		if (first_delay_done_ == true) {
			_delay = delay_;
		}

		while (timer_.GetTimeDiff() > _delay) {
			SetScrollPos(GetScrollPos() + scroll_speed_);
			timer_.ReduceTimeDiff(_delay);
			first_delay_done_ = true;
			_delay = delay_;
		}

		SetNeedsRepaint(true);
	}
}



float64 ScrollBar::GetScrollPos() {
	return pos_;
}

void ScrollBar::SetScrollDelay(float64 first_delay, float64 delay) {
	first_delay_ = first_delay;
	delay_ = delay;
}

void ScrollBar::SetStaticScrollerSize(int scroller_size) {
	scroller_size_ = scroller_size;
	SetNeedsRepaint(true);
}

Component::Type ScrollBar::GetType() const {
	return Component::kScrollbar;
}

void ScrollBar::SetOwner(Component* owner) {
	owner_ = owner;
}



}
