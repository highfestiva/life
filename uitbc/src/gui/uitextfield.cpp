
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uitextfield.h"
#include "../../include/gui/uidesktopwindow.h"
#include "../../include/gui/uipopuplist.h"
#include "../../include/gui/uifloatinglayout.h"
#include "../../../uilepra/include/uiinput.h"



namespace uitbc {



TextField::TextField(Component* top_parent):
	Window(),
	is_read_only_(false),
	password_character_(0),
	text_x_(0),
	marker_id_(Painter::kInvalidImageid),
	marker_pos_(0),
	marker_visible_(false),
	marker_visible_time_(0.5),
	marker_invisible_time_(0.5),
	update_marker_pos_on_next_repaint_(false),
	click_x_(0),
	top_parent_(top_parent),
	desktop_window_(0),
	list_layer_(0),
	list_control_(0),
	delete_list_control_(false) {
	Init();
}

TextField::TextField(Component* top_parent, unsigned border_style, int border_width, const Color& color):
	Window(border_style, border_width, color),
	is_read_only_(false),
	password_character_(0),
	text_x_(0),
	marker_id_(Painter::kInvalidImageid),
	marker_pos_(0),
	marker_visible_(false),
	marker_visible_time_(0.5),
	marker_invisible_time_(0.5),
	update_marker_pos_on_next_repaint_(false),
	click_x_(0),
	top_parent_(top_parent),
	desktop_window_(0),
	list_layer_(0),
	list_control_(0),
	delete_list_control_(false) {
	Init();
}

TextField::TextField(Component* top_parent, unsigned border_style, int border_width, Painter::ImageID image_id) :
	Window(border_style, border_width, image_id),
	is_read_only_(false),
	password_character_(0),
	text_x_(0),
	marker_id_(Painter::kInvalidImageid),
	marker_pos_(0),
	marker_visible_(false),
	marker_visible_time_(0.5),
	marker_invisible_time_(0.5),
	update_marker_pos_on_next_repaint_(false),
	click_x_(0),
	top_parent_(top_parent),
	desktop_window_(0),
	list_layer_(0),
	list_control_(0),
	delete_list_control_(false) {
	Init();
}

TextField::TextField(Component* top_parent, const Color& color) :
	Window(color),
	is_read_only_(false),
	password_character_(0),
	text_x_(0),
	marker_id_(Painter::kInvalidImageid),
	marker_pos_(0),
	marker_visible_(false),
	marker_visible_time_(0.5),
	marker_invisible_time_(0.5),
	update_marker_pos_on_next_repaint_(false),
	click_x_(0),
	top_parent_(top_parent),
	desktop_window_(0),
	list_layer_(0),
	list_control_(0),
	delete_list_control_(false) {
	Init();
}

TextField::TextField(Component* top_parent, Painter::ImageID image_id):
	Window(image_id),
	is_read_only_(false),
	password_character_(0),
	text_x_(0),
	marker_id_(Painter::kInvalidImageid),
	marker_pos_(0),
	marker_visible_(false),
	marker_visible_time_(0.5),
	marker_invisible_time_(0.5),
	update_marker_pos_on_next_repaint_(false),
	click_x_(0),
	top_parent_(top_parent),
	desktop_window_(0),
	list_layer_(0),
	list_control_(0),
	delete_list_control_(false) {
	Init();
}

TextField::~TextField() {
	if (desktop_window_ != 0) {
		// We are still registered as a subscriber...
		ReleaseKeyboardFocus();
	}

	// The list control is deleted when the top parent is deleted.
}

Component* TextField::GetTopParent() const {
	return top_parent_;
}

void TextField::SetTopParent(Component* top_parent) {
	top_parent_ = top_parent;
}

PopupList* TextField::CreatePopupList() {
	return 0;
}

void TextField::SpawnPopupList() {
	if (list_control_ == 0) {
		if (top_parent_ != 0) {
			list_control_ = CreatePopupList();
		}

		if (list_control_ != 0) {
			PixelRect screen_rect(GetScreenRect());
			PixelRect client_screen_rect(top_parent_->GetScreenRect());

			PixelRect rect(top_parent_->ScreenToWindow(GetScreenRect()));

			list_control_->UpdateLayout();
			int _height = (int)std::min(list_control_->GetPreferredHeight(), std::min(list_control_->GetContentSize().y, client_screen_rect.bottom_ - screen_rect.bottom_));

			list_control_->SetPreferredSize(GetSize().x, _height);

			if (list_layer_ == 0) {
				list_layer_ = top_parent_->CreateLayer(new FloatingLayout());
			}
			top_parent_->AddChild(list_control_, 0, 0, list_layer_);
			top_parent_->RequestRepaint();

			list_control_->SetPos(rect.left_, rect.bottom_);

			// Select the first item in the list.
			//list_control_->SetKeyboardFocus();
			list_control_->AddListener(this);
			list_control_->SetItemSelected(0, true);
			list_control_->RequestRepaint();
			Parent::SetKeyboardFocus();
		}
	}
}

void TextField::DestroyPopupList() {
	// The list is destroyed with the layer.
	top_parent_->DeleteLayer(list_layer_);
	top_parent_->RequestRepaint();
	list_layer_ = 0;
	list_control_ = 0;
}

PopupList* TextField::GetPopupList() const {
	return list_control_;
}

void TextField::SetIsReadOnly(bool is_read_only) {
	is_read_only_ = is_read_only;
}

void TextField::SetPasswordCharacter(char character) {
	password_character_ = character;
}

wstr TextField::GetVisibleText() const {
	wstr _text;
	if (password_character_) {
		_text.assign(text_.length(), password_character_);
	} else {
		_text = text_;
	}
	return (_text);
}

void TextField::SetText(const wstr& text) {
	text_ = text;
	text_x_ = 0;
	SetMarkerPosition(text.length());
	SetNeedsRepaint(true);
}

const wstr& TextField::GetText() const {
	return text_;
}

void TextField::SetMarker(Painter::ImageID image_id) {
	marker_id_ = image_id;
	SetNeedsRepaint(true);
}

void TextField::SetMarkerBlinkRate(float64 visible_time, float64 invisible_time) {
	marker_visible_time_ = visible_time;
	marker_invisible_time_ = invisible_time;
}

void TextField::SetMarkerPosition(size_t index) {
	if (index <= text_.size()) {
		marker_pos_ = index;
		SetNeedsRepaint(true);
	}
}

bool TextField::OnChar(wchar_t c) {
	Parent::OnChar(c);
	if (is_read_only_) {
		return (false);	// RAII simplifies here.
	}

	bool regenerate_list = false;

	if (c == '\b') {
		if (marker_pos_ > 0) {
			SetMarkerPosition(marker_pos_-1);
			text_.erase(marker_pos_, 1);
			SetNeedsRepaint(true);
			regenerate_list = true;
		}
	} else if (c != '\r' && c != '\n' && c != 27) {
		text_.insert(marker_pos_, 1, c);
		SetMarkerPosition(marker_pos_+1);
		SetNeedsRepaint(true);
		regenerate_list = true;
	}

	if (regenerate_list == true && list_control_ != 0) {
		DestroyPopupList();
		SpawnPopupList();
	}
	return (false);
}

bool TextField::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	Parent::OnKeyDown(key_code);
	if (is_read_only_) {
		return (false);	// RAII simplifies here.
	}

	// TODO: Implement popup menu and marker control
	bool reset_marker = false;

	const DesktopWindow* desktop_window = (DesktopWindow*)GetParentOfType(Component::kDesktopwindow);
	uilepra::InputManager* input_manager = desktop_window->GetInputManager();

	bool ctrl_down = input_manager->ReadKey(uilepra::InputManager::kInKbdLCtrl) ||
		input_manager->ReadKey(uilepra::InputManager::kInKbdRCtrl);
	const wstr delimitors = L" \t";


	switch(key_code) {
		case uilepra::InputManager::kInKbdLeft: {
			if (marker_pos_ > 0) {
				if (ctrl_down == true) {
					marker_pos_ = wstrutil::FindPreviousWord(GetVisibleText(), delimitors, marker_pos_);
				} else {
					marker_pos_--;
				}

				reset_marker = true;
			}
		} break;
		case uilepra::InputManager::kInKbdRight: {
			if (marker_pos_ < text_.length()) {
				if (ctrl_down == true) {
					size_t _index = GetVisibleText().find_first_not_of(delimitors, marker_pos_);
					_index = GetVisibleText().find_first_of(delimitors, _index);
					if (_index == wstr::npos) {
						// We have reached the end.
						marker_pos_ = text_.length();
					} else {
						marker_pos_ = _index;
					}
				} else {
					marker_pos_++;
				}
				reset_marker = true;
			}
		} break;
		case uilepra::InputManager::kInKbdHome: {
			marker_pos_ = 0;
			reset_marker = true;
		} break;
		case uilepra::InputManager::kInKbdEnd: {
			marker_pos_ = text_.length();
			reset_marker = true;
		} break;
		case uilepra::InputManager::kInKbdDown: {
			if (list_control_ == 0) {
				SpawnPopupList();
			} else {
				list_control_->OnKeyDown(key_code);
				Parent::SetKeyboardFocus();
			}
		} break;
		case uilepra::InputManager::kInKbdUp: {
			if (list_control_ != 0) {
				list_control_->OnKeyDown(key_code);
				Parent::SetKeyboardFocus();
			}
		} break;
		case uilepra::InputManager::kInKbdDel: {
			if (marker_pos_ < text_.length()) {
				text_.erase(marker_pos_, 1);
				reset_marker = true;
			}
		} break;
		case uilepra::InputManager::kInKbdEsc: {
			if (list_control_ != 0) {
				DestroyPopupList();
				Parent::SetKeyboardFocus();
			}
		} break;
		default: break;
	}

	if (reset_marker == true) {
		marker_timer_.ClearTimeDiff();
		marker_visible_ = true;
		SetNeedsRepaint(true);
	}
	return (false);
}

void TextField::OnIdle() {
	Parent::OnIdle();

	marker_timer_.UpdateTimer();

	bool old_state = marker_visible_;
	bool loop = true;

	// Loop and consume the time difference in case the program has freezed
	// for a while.
	while (loop == true) {
		loop = false;

		if (marker_visible_ == true && marker_timer_.GetTimeDiff() > marker_visible_time_) {
			marker_visible_ = false;
			marker_timer_.ReduceTimeDiff(marker_visible_time_);
			loop = true;
		}

		if (marker_visible_ == false && marker_timer_.GetTimeDiff() > marker_invisible_time_) {
			marker_visible_ = true;
			marker_timer_.ReduceTimeDiff(marker_invisible_time_);
			loop = true;
		}
	}

	if (old_state != marker_visible_) {
		SetNeedsRepaint(true);
	}
}

bool TextField::OnLButtonDown(int mouse_x, int mouse_y) {
	SetKeyboardFocus();

	// The coordinate relative to the start of the text string's x-coordinate.
	update_marker_pos_on_next_repaint_ = true;
	click_x_ = mouse_x;

	return Parent::OnLButtonDown(mouse_x, mouse_y);
}

bool TextField::OnLButtonUp(int mouse_x, int mouse_y) {
	return Parent::OnLButtonUp(mouse_x, mouse_y);
}

bool TextField::OnMouseMove(int, int, int, int) {
	// TODO: set mouse cursor: text.
	return true;
}

void TextField::SetKeyboardFocus() {
	Parent::SetKeyboardFocus();
	SetupMarkerBlink();
}

void TextField::SetKeyboardFocus(Component* child) {
	Parent::SetKeyboardFocus(child);
	SetupMarkerBlink();
}

void TextField::SetupMarkerBlink() {
	DesktopWindow* desktop_win = (DesktopWindow*)GetParentOfType(kDesktopwindow);
	if (desktop_win != 0) {
		desktop_win->AddIdleSubscriber(this);
		desktop_win->ActivateKeyboard();
		marker_timer_.PopTimeDiff();
		marker_visible_ = true;
		SetNeedsRepaint(true);

		// Store the pointer to the desktop window in case the
		// TextField is deleted before the keyboard focus is released.
		desktop_window_ = desktop_win;
	}
}

void TextField::ReleaseKeyboardFocus(RecurseDir dir, Component* focused_component) {
	Parent::ReleaseKeyboardFocus(dir, focused_component);

	if (desktop_window_ != 0) {
		desktop_window_->RemoveIdleSubscriber(this);
		desktop_window_->DeactivateKeyboard();
		if (marker_visible_ == true) {
			SetNeedsRepaint(true);
		}
		marker_visible_ = false;

		desktop_window_ = 0;
	}
}

void TextField::SetCaption(Caption*) {
	// Do nothing. TextField can't have a caption.
}

void TextField::AddChild(Component*, int, int, int) {
	// Do nothing. TextField can't have children.
}

void TextField::DoSetPos(int x, int y) {
	Parent::DoSetPos(x, y);
	if (list_control_ != 0) {
		PixelRect rect(top_parent_->ScreenToWindow(GetScreenRect()));
		list_control_->SetPos(rect.left_, rect.bottom_);
	}
}

void TextField::DoSetSize(int width, int height) {
	Parent::DoSetSize(width, height);
	if (list_control_ != 0) {
		list_control_->SetPreferredWidth(width);
	}
}

void TextField::Repaint(Painter* painter) {
	ActivateFont(painter);

	Parent::Repaint(painter);

	if (update_marker_pos_on_next_repaint_ == true) {
		UpdateMarkerPos(painter);
		update_marker_pos_on_next_repaint_ = false;
	}

	int text_height = painter->GetLineHeight();

	painter->PushAttrib(Painter::kAttrAll);

	const PixelRect rect(GetClientRect());
#ifndef LEPRA_TOUCH
	painter->ReduceClippingRect(rect);
#endif // !touch

	int marker_x  = text_x_ + horizontal_margin_ + painter->GetStringWidth(GetVisibleText().substr(0, marker_pos_));
	int marker_width = 1;

	if (marker_id_ != Painter::kInvalidImageid) {
		marker_width = GetImageManager()->GetImageSize(marker_id_).x;
	}

	int max_x = rect.GetWidth() - marker_width;
	if (marker_x > max_x) {
		text_x_ += max_x - marker_x;
		marker_x = max_x;
	}
	if (marker_x < 0) {
		text_x_ += -marker_x;
		marker_x = 0;
	}

	int text_x = rect.left_ + text_x_ + horizontal_margin_;
	int text_y = rect.top_ + (rect.GetHeight() - text_height) / 2;

	PrintTextDeactivate(painter, GetVisibleText(), text_x, text_y);

	if (marker_visible_ == true) {
		//painter->PushAttrib(Painter::kAttrRendermode);
		//painter->SetRenderMode(Painter::kRmXor);
		if (marker_id_ != Painter::kInvalidImageid) {
			GetImageManager()->DrawImage(marker_id_, rect.left_ + marker_x, rect.top_);
		} else {
			const int marker_y = rect.top_ + (rect.GetHeight() - text_height) / 2;
			painter->DrawLine(rect.left_ + marker_x, marker_y, rect.left_ + marker_x, marker_y + text_height);
		}
		//painter->PopAttrib();
	}

	painter->PopAttrib();
}

Component::StateComponentList TextField::GetStateList(ComponentState state) {
	StateComponentList list;
	if (state == kStateFocusable) {
		int _state = 0;
		if (IsDispatcher()) {
			_state = 2;
		} else if (HasKeyboardFocus()) {
			_state = 1;
		}
		list.push_back(StateComponent(_state, (Component*)this));
	}
	return (list);
}

void TextField::UpdateMarkerPos(Painter* painter) {
	PixelRect rect(GetClientRect());
	int text_x = (click_x_ - rect.left_) - text_x_ - horizontal_margin_;

	// Search for the correct marker position using binary search.
	size_t __min = 0;
	size_t max = text_.length();

	int left = 0;
	int right = 0;

	while (max - __min > 1) {
		size_t mid = (__min + max) / 2;
		int string_width = painter->GetStringWidth(GetVisibleText().substr(0, mid));

		if (string_width < text_x) {
			__min = mid;
			left = text_x - string_width;
		} else if (string_width >= text_x) {
			max = mid;
			right = string_width - text_x;
		}
	}

	if (left < right) {
		marker_pos_ = __min;
	} else {
		marker_pos_ = max;
	}

	marker_timer_.ClearTimeDiff();
	marker_visible_ = true;
}

void TextField::SetMarkerPos(size_t pos) {
	if (pos <= text_.length()) {
		marker_pos_ = pos;
		marker_timer_.ClearTimeDiff();
		marker_visible_ = true;
		SetNeedsRepaint(true);
	}
}

void TextField::ForceRepaint() {
	SetNeedsRepaint(true);
}

bool TextField::NotifySetKeyboardFocus(PopupList*) {
	Parent::SetKeyboardFocus();
	return true;
}

bool TextField::NotifyReleaseKeyboardFocus(PopupList*, Component*) {
	return true;
}

bool TextField::NotifyKeyDown(PopupList*, uilepra::InputManager::KeyCode) {
	return true;
}

bool TextField::NotifyLButtonDown(PopupList*, int, int) {
	Parent::SetKeyboardFocus();
	return true;
}

bool TextField::NotifyDoubleClick(PopupList*, int, int) {
	// Let the subclass take care of this.
	return true;
}

}
