
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uicomponent.h"
#include "../../include/gui/uidesktopwindow.h"
#include "../../../lepra/include/log.h"



namespace uitbc {



Component::Component(Layout* layout) :
	parent_(0),
	mouse_focus_child_(0),
	keyboard_focus_child_(0),
	layout_(0),
	parent_layout_(0),
	pos_(0, 0),
	preferred_size_(0, 0),
	size_(0, 0),
	needs_repaint_(true),
	visible_(true),
	adaptive_preferred_size_(false),
	selected_(false),
	enabled_(true),
	image_id_(Painter::kInvalidImageid),
	layer_count_(0) {
	CreateLayer(layout);
}

Component::~Component() {
	ReleaseKeyboardFocus();
	ReleaseMouseFocus();
	keyboard_focus_child_ = 0;
	mouse_focus_child_ = 0;

	DeleteAllLayers();

	if (parent_) {
		Component* _parent = parent_;
		for (int x = 0; x < 5; ++x) {
			_parent->RemoveChild(this, x);
		}
	}
}

void Component::DeleteAllLayers() {
	for (int i = 0; i < layer_count_; i++) {
		DeleteLayout(i);
	}
	layer_count_ = 0;
	delete[] layout_;
	layout_ = 0;
}

void Component::DeleteLayout(int layer) {
	if (layer >= 0 && layer < layer_count_) {
		if (layout_[layer] != 0) {
			DeleteChildrenInLayer(layer);
			delete layout_[layer];
			layout_[layer] = 0;
		}
	}
}

int Component::CreateLayer(Layout* layout) {
	Layout** _layout = new Layout*[layer_count_ + 1];
	for (int i = 0; i < layer_count_; i++) {
		_layout[i] = layout_[i];
	}
	_layout[layer_count_] = layout;
	if (layout) {
		layout->SetOwner(this);
	}

	delete[] layout_;
	layout_ = _layout;
	++layer_count_;

	return (layer_count_ - 1);
}

void Component::DeleteLayer(int layer) {
	if (layer >= 0 && layer < layer_count_) {
		DeleteLayout(layer);

		if (layer_count_ > 1) {
			Layout** _layout = new Layout*[layer_count_ - 1];

			int i;
			for (i = 0; i < layer; i++) {
				_layout[i] = layout_[i];
			}

			int j;
			for (j = i + 1; j < layer_count_; i++, j++) {
				_layout[i] = layout_[j];
			}

			delete[] layout_;
			layout_ = _layout;
		} else { // if (layer_count_ == 1)
			delete[] layout_;
			layout_ = 0;
		}

		layer_count_--;
	}
}

void Component::DeleteChildrenInLayer(int layer) {
	if (layer >= 0 && layer < layer_count_) {
		Component* _child;
		while ((_child = layout_[layer]->GetFirst()) != 0) {
			delete _child;
		}
	}
}

void Component::ReplaceLayer(int layer, Layout* layout) {
	if (layer >= 0 && layer < layer_count_) {
		delete (layout_[layer]);
		layout_[layer] = layout;
		layout->SetOwner(this);
	}
}

PixelCoord Component::GetPreferredSize(bool force_adaptive) {
	PixelCoord _size(preferred_size_);

	if ((force_adaptive == true || adaptive_preferred_size_ == true) && layout_[0] != 0 &&
	   (_size.x == 0 || _size.y == 0)) {
		PixelCoord temp(layout_[0]->GetPreferredSize(force_adaptive));

		if (_size.x == 0) {
			_size.x = temp.x;
		}
		if (_size.y == 0) {
			_size.y = temp.y;
		}
	}

	return _size;
}

PixelCoord Component::GetMinSize() const {
	PixelCoord _size(min_size_);

	if (layout_[0] != 0 && (_size.x == 0 || _size.y == 0)) {
		PixelCoord temp(layout_[0]->GetMinSize());

		if (_size.x == 0) {
			_size.x = temp.x;
		}
		if (_size.y == 0) {
			_size.y = temp.y;
		}
	}

	return _size;
}

void Component::AddChild(Component* child, int param1, int param2, int layer) {
	if (layer >= 0 && layer < layer_count_ && layout_[layer] != 0) {
		deb_assert(child->GetParentLayout() == 0);
		child->SetParent(this);
		layout_[layer]->Add(child, param1, param2);
		child->SetParentLayout(layout_[layer]);
		UpdateLayout();
		SetNeedsRepaint(true);
	} else {
		log_.Errorf("Could not add child to layer %i.", layer);
	}
}

void Component::RemoveChild(Component* child, int layer) {
	if (layer >= 0 && layer < layer_count_ && layout_[layer] != 0) {
		layout_[layer]->Remove(child);
		child->SetParentLayout(0);
		if (child == mouse_focus_child_) {
			child->ReleaseMouseFocus();
		}
		if (child == keyboard_focus_child_) {
			child->ReleaseKeyboardFocus();
		}
		child->SetParent(0);
	}
}

int Component::GetNumChildren() const {
	int num_children = 0;

	for (int i = 0; i < layer_count_; i++) {
		if (layout_[i] != 0) {
			num_children = layout_[i]->GetNumComponents();
		}
	}

	return num_children;
}

void Component::AddTextListener(uilepra::TextInputObserver* listener) {
	text_listener_set_.insert(listener);
}

void Component::RemoveTextListener(uilepra::TextInputObserver* listener) {
	text_listener_set_.erase(listener);
}

void Component::AddKeyListener(uilepra::KeyCodeInputObserver* listener) {
	key_listener_set_.insert(listener);
}

void Component::RemoveKeyListener(uilepra::KeyCodeInputObserver* listener) {
	key_listener_set_.erase(listener);
}

Component* Component::GetChild(int screen_x, int screen_y, int levels_down) {
	int i;
	for (i = layer_count_ - 1; i >= 0; --i) {
		if (layout_[i] != 0) {
			Component* _child = layout_[i]->GetLast();
			while (_child != 0) {
				if (_child->IsVisible() == true) {
					if (_child->IsOver(screen_x, screen_y) == true) {
						if (levels_down <= 0) {
							return _child;
						} else {
							return _child->GetChild(screen_x, screen_y, levels_down - 1);
						}
					}
				}

				_child = layout_[i]->GetPrev();
			}
		}
	}

	return 0;
}

void Component::UpdateLayout() {
	for (int i = 0; i < layer_count_; i++) {
		if (layout_[i] != 0) {
			Component* _child = layout_[i]->GetFirst();
			while (_child != 0) {
				if (_child->IsLocalVisible() &&
				    _child->NeedsRepaint()) {
					_child->UpdateLayout();
				}
				_child = layout_[i]->GetNext();
			}
			layout_[i]->UpdateLayout();
		}
	}
}

void Component::Repaint(Painter* painter) {
	RepaintBackground(painter);
	RepaintComponents(painter);
}

void Component::RepaintBackground(Painter* painter) {
	(void)painter;
}

void Component::RepaintComponents(Painter* painter) {
	for (int i = 0; i < layer_count_; i++) {
		if (layout_[i] != 0) {
			Component* _child = layout_[i]->GetFirst();

			while (_child != 0) {
				if (_child->IsVisible() == true) {
					RepaintChild(_child, painter);
				}

				_child = layout_[i]->GetNext();
			}
		}
	}
	needs_repaint_ = false;
}

bool Component::OnDoubleClick(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnDoubleClick(mouse_x, mouse_y);
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnDoubleClick(mouse_x, mouse_y);
		}
	}
	return (false);
}

bool Component::OnLButtonDown(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnLButtonDown(mouse_x, mouse_y);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnLButtonDown(mouse_x, mouse_y);
			return true;
		}

		return false;
	}
}

bool Component::OnRButtonDown(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnRButtonDown(mouse_x, mouse_y);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnRButtonDown(mouse_x, mouse_y);
			return true;
		}

		return false;
	}
}

bool Component::OnMButtonDown(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnMButtonDown(mouse_x, mouse_y);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnMButtonDown(mouse_x, mouse_y);
			return true;
		}

		return false;
	}
}

bool Component::OnLButtonUp(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnLButtonUp(mouse_x, mouse_y);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnLButtonUp(mouse_x, mouse_y);
			return true;
		}

		return false;
	}
}

bool Component::OnRButtonUp(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnRButtonUp(mouse_x, mouse_y);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnRButtonUp(mouse_x, mouse_y);
			return true;
		}

		return false;
	}
}

bool Component::OnMButtonUp(int mouse_x, int mouse_y) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnMButtonUp(mouse_x, mouse_y);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnMButtonUp(mouse_x, mouse_y);
			return true;
		}

		return false;
	}
}

bool Component::OnMouseWheel(int mouse_x, int mouse_y, int change, bool down) {
	if (mouse_focus_child_ != 0) {
		mouse_focus_child_->OnMouseWheel(mouse_x, mouse_y, change, down);
		return true;
	} else {
		Component* _child = GetChild(mouse_x, mouse_y);

		if (_child != 0) {
			_child->OnMouseWheel(mouse_x, mouse_y, change, down);
			return true;
		}

		return false;
	}
}

bool Component::OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y) {
	bool is_over = false;
	for (int i = layer_count_-1; !is_over && i >= 0; --i) {
		if (layout_[i] != 0) {
			Component* _child = layout_[i]->GetFirst();
			for (; _child; _child = layout_[i]->GetNext()) {
				is_over |= _child->OnMouseMove(mouse_x, mouse_y, delta_x, delta_y);
			}
		}
	}
	return is_over;
}

bool Component::OnChar(wchar_t _c) {
	DispatchChar(_c);

	if (keyboard_focus_child_ != 0) {
		keyboard_focus_child_->OnChar(_c);
	}
	return (false);
}

bool Component::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	KeyListenerSet::iterator x = key_listener_set_.begin();
	for (; x != key_listener_set_.end(); ++x) {
		(*x)->OnKeyDown(key_code);
	}

	if (keyboard_focus_child_ != 0) {
		keyboard_focus_child_->OnKeyDown(key_code);
	}
	return (false);
}

bool Component::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	KeyListenerSet::iterator x = key_listener_set_.begin();
	for (; x != key_listener_set_.end(); ++x) {
		(*x)->OnKeyUp(key_code);
	}

	if (keyboard_focus_child_ != 0) {
		keyboard_focus_child_->OnKeyUp(key_code);
	}
	return (false);
}

bool Component::OnDoubleClick() {
	return (false);
}

void Component::OnConnectedToDesktopWindow() {
	// Notify children.
	for(int i = 0; i < layer_count_; i++) {
		Layout* _layout = layout_[i];
		if(_layout != 0) {
			Component* _child = _layout->GetFirst();
			while(_child != 0) {
				_child->OnConnectedToDesktopWindow();
				_child = _layout->GetNext();
			}
		}
	}
}

Component* Component::GetParentOfType(Type type) {
	Component* _parent = this;

	while (_parent != 0) {
		if (_parent->GetType() == type) {
			return _parent;
		}

		_parent = _parent->GetParent();
	}

	return 0;
}

Component* Component::GetTopParent() {
	Component* top_parent = this;
	Component* _parent = this;

	while (_parent != 0) {
		top_parent = _parent;
		_parent = _parent->GetParent();
	}

	return top_parent;
}

bool Component::IsChildOf(Component* parent) {
	Component* _parent = GetParent();

	while(_parent != 0 && _parent != parent) {
		_parent = _parent->GetParent();
	}

	return (_parent != 0);
}

bool Component::IsOver(int screen_x, int screen_y) {
	PixelCoord _pos(GetScreenPos());
	PixelRect _rect(_pos, _pos + GetSize());
	return _rect.IsInside(screen_x, screen_y);
}

Component::StateComponentList Component::GetStateList(ComponentState state) {
	StateComponentList list;
	for (int i = 0; i < layer_count_; i++) {
		if (layout_[i] != 0) {
			Component* c = layout_[i]->GetFirst();
			for (; c; c = layout_[i]->GetNext()) {
				StateComponentList child_list = c->GetStateList(state);
				list.splice(list.end(), child_list);
			}
		}
	}
	return (list);
}

void Component::RepaintChild(Component* child, Painter* painter) {
	// Simply let the child repaint itself.
	child->Repaint(painter);
}

void Component::SetPos(int x, int y) {
	if (x != pos_.x || y != pos_.y) {
		DoSetPos(x, y);
	}
}

void Component::DoSetPos(int x, int y) {
	// Can't repaint just because of a position change. That will spoil
	// the rendering optimization in DesktopWindow.
	// SetNeedsRepaint(true);
	if (GetParent()) {
		if (GetParent()->GetType() == Component::kDesktopwindow) {
			((DesktopWindow*)GetParent())->SetUpdateLayout(true);
		}
	} else if (GetType() == Component::kDesktopwindow) {
		((DesktopWindow*)this)->SetUpdateLayout(true);
	}

	pos_.x = x;
	pos_.y = y;
}

void Component::SetSize(int width, int height) {
	if (size_.x != width || size_.y != height) {
		DoSetSize(width, height);
	}
}

void Component::DoSetSize(int width, int height) {
	SetNeedsRepaint(true);
	size_.x = width;
	size_.y = height;
}

void Component::SetMinSize(int width, int height) {
	DoSetMinSize(width, height);
}

void Component::DoSetMinSize(int width, int height) {
	min_size_.x = width  < 0 ? 0 : width;
	min_size_.y = height < 0 ? 0 : height;
}

void Component::SetPreferredSize(const PixelCoord& size, bool adaptive) {
	if (preferred_size_ == size) {
		return;
	}

	SetNeedsRepaint(true);

	preferred_size_ = size;

	if (preferred_size_.x != 0 && preferred_size_.x < min_size_.x) {
		preferred_size_.x = min_size_.x;
	}

	if (preferred_size_.y != 0 && preferred_size_.y < min_size_.y) {
		preferred_size_.y = min_size_.y;
	}

	if (parent_ == 0) {
		SetSize(preferred_size_);
	}


	adaptive_preferred_size_ = adaptive;
}

void Component::SetPreferredSize(int width, int height, bool adaptive) {
	PixelCoord _size(width, height);
	SetPreferredSize(_size, adaptive);
}

void Component::SetSelected(bool selected) {
	selected_ = selected;

	if (selected_) {
		SetKeyboardFocus();
	}
}

void Component::Enable(bool enable) {
	enabled_ = enable;
}

void Component::SetMouseFocus() {
	if (!HasMouseFocus()) {
		GetTopParent()->ReleaseMouseFocus(kRecurseDown, this);

		if (parent_ != 0) {
			parent_->SetMouseFocus(this);
		}
	}
}

void Component::SetMouseFocus(Component* child) {
	mouse_focus_child_ = child;
	if (parent_ != 0) {
		parent_->SetMouseFocus(this);
	}
}

void Component::DispatchChar(wchar_t _c) {
	TextListenerSet::iterator x = text_listener_set_.begin();
	for (; x != text_listener_set_.end(); ++x) {
		(*x)->OnChar(_c);
	}
}

bool Component::IsDispatcher() const {
	return (!text_listener_set_.empty());
}

void Component::ReleaseMouseFocus(RecurseDir dir, Component* focused_component) {
	if (dir == kRecurseUp) {
		if (parent_ != 0 && parent_->mouse_focus_child_ == this) {
			parent_->mouse_focus_child_ = 0;
			parent_->ReleaseMouseFocus(dir, focused_component);
		}
	} else {
		if (mouse_focus_child_ != 0) {
			mouse_focus_child_->ReleaseMouseFocus(dir, focused_component);
			mouse_focus_child_ = 0;
		}
	}
}

void Component::SetKeyboardFocus() {
	if (!HasKeyboardFocus()) {
		GetTopParent()->ReleaseKeyboardFocus(kRecurseDown, this);

		if (parent_ != 0) {
			parent_->SetKeyboardFocus(this);
		}
	}
}

void Component::SetKeyboardFocus(Component* child) {
	keyboard_focus_child_ = child;
	if (parent_ != 0) {
		parent_->SetKeyboardFocus(this);
	}
}

void Component::ReleaseKeyboardFocus(RecurseDir dir, Component* focused_component) {
	if (dir == kRecurseUp) {
		if (parent_ != 0 && parent_->keyboard_focus_child_ == this) {
			parent_->keyboard_focus_child_ = 0;
			parent_->ReleaseKeyboardFocus(dir, focused_component);
		}
	} else {
		if (keyboard_focus_child_ != 0) {
			keyboard_focus_child_->ReleaseKeyboardFocus(dir, focused_component);
			keyboard_focus_child_ = 0;
		}
	}
}

Component* Component::GetParent() {
	return parent_;
}

void Component::SetParent(Component* parent) {
/*	if (parent_ != 0) {
		parent_->RemoveChild(this);
	}
*/
	parent_ = parent;

	// OnNewTopParentConnected();
}

const PixelCoord& Component::GetPos() const {
	return pos_;
}

PixelCoord Component::GetScreenPos() const {
	PixelCoord _pos(pos_);

	if (parent_ != 0) {
		_pos += parent_->GetScreenPos();
	}

	return _pos;
}

void Component::SetPos(const PixelCoord& pos) {
	SetPos(pos.x, pos.y);
}

void Component::SetSize(const PixelCoord& size) {
	SetSize(size.x, size.y);
}

void Component::SetMinSize(const PixelCoord& size) {
	SetMinSize(size.x, size.y);
}

const PixelCoord& Component::GetSize() const {
	return size_;
}

PixelRect Component::GetScreenRect() const {
	PixelCoord _pos(GetScreenPos());
	return PixelRect(_pos, _pos + GetSize());
}


void Component::SetPreferredWidth(int width) {
	SetPreferredSize(width, preferred_size_.y, adaptive_preferred_size_);
}

void Component::SetPreferredHeight(int height) {
	SetPreferredSize(preferred_size_.x, height, adaptive_preferred_size_);
}

int Component::GetPreferredWidth(bool force_adaptive) {
	return GetPreferredSize(force_adaptive).x;
}

int Component::GetPreferredHeight(bool force_adaptive) {
	return GetPreferredSize(force_adaptive).y;
}

void Component::SetAdaptive(bool adaptive) {
	adaptive_preferred_size_ = adaptive;
}

bool Component::IsAdaptive() {
	return adaptive_preferred_size_;
}

void Component::SetVisible(bool visible) {
	if (visible_ == false && visible == true) {
		SetNeedsRepaint(true);
	} else if (visible_ == true && visible == false) {
		ReleaseKeyboardFocus();
		ReleaseMouseFocus();
	}

	visible_ = visible;
}

bool Component::IsVisible() const {
	bool parent_visible = visible_;
	if (parent_ && visible_) {
		parent_visible = parent_->IsVisible();
	}
	return (parent_visible);
}

bool Component::IsLocalVisible() const {
	return visible_;
}

PixelCoord Component::ClientToWindow(const PixelCoord& coords) {
	return coords + pos_;
}

PixelCoord Component::WindowToClient(const PixelCoord& coords) {
	return coords - pos_;
}

PixelCoord Component::WindowToScreen(const PixelCoord& coords) {
	return coords + GetScreenPos();
}

PixelCoord Component::ScreenToWindow(const PixelCoord& coords) {
	return coords - GetScreenPos();
}

PixelCoord Component::ClientToScreen(const PixelCoord& coords) {
	return WindowToScreen(ClientToWindow(coords));
}

PixelCoord Component::ScreenToClient(const PixelCoord& coords) {
	return WindowToClient(ScreenToWindow(coords));
}

PixelRect Component::ClientToWindow(const PixelRect& rect) {
	PixelRect _rect(rect);
	_rect.Offset(pos_.x, pos_.y);
	return _rect;
}

PixelRect Component::WindowToClient(const PixelRect& rect) {
	PixelRect _rect(rect);
	_rect.Offset(-pos_.x, -pos_.y);
	return _rect;
}

PixelRect Component::WindowToScreen(const PixelRect& rect) {
	PixelRect _rect(rect);
	PixelCoord _pos(GetScreenPos());
	_rect.Offset(_pos.x, _pos.y);
	return _rect;
}

PixelRect Component::ScreenToWindow(const PixelRect& rect) {
	PixelRect _rect(rect);
	PixelCoord _pos(GetScreenPos());
	_rect.Offset(-_pos.x, -_pos.y);
	return _rect;
}

PixelRect Component::ClientToScreen(const PixelRect& rect) {
	return WindowToScreen(ClientToWindow(rect));
}

PixelRect Component::ScreenToClient(const PixelRect& rect) {
	return WindowToClient(ScreenToWindow(rect));
}

void Component::RequestRepaint() {
	if (IsVisible()) {
		SetNeedsRepaint(true);
	}
}

bool Component::IsComplete() const {
	for (int i = 0; i < layer_count_; i++) {
		if (layout_[i] != 0) {
			Component* c = layout_[i]->GetFirst();
			for (; c; c = layout_[i]->GetNext()) {
				if (!c->IsComplete()) {
					return false;
				}
			}
		}
	}
	return true;
}

bool Component::NeedsRepaint() {
	return needs_repaint_;
}

void Component::SetNeedsRepaint(bool needs_repaint) {
	if (parent_ != 0 &&
	   needs_repaint_ == false &&
	   needs_repaint == true) {
		parent_->SetNeedsRepaint(true);
	}

	needs_repaint_ = needs_repaint;
}

bool Component::GetSelected() const {
	return selected_;
}

bool Component::HasMouseFocus() const {
	return (parent_ && parent_->mouse_focus_child_ == this);
}

bool Component::HasKeyboardFocus() const {
	bool focused = (parent_ == 0 || parent_->keyboard_focus_child_ == this);
	if (focused && parent_) {
		focused = parent_->HasKeyboardFocus();
	}
	return (focused);
}

Layout* Component::GetLayout(int layer) const {
	Layout* _layout = 0;
	if (layer >= 0 && layer < layer_count_) {
		_layout = layout_[layer];
	}
	return _layout;
}

void Component::SetParentLayout(Layout* layout) {
	parent_layout_ = layout;
}

Layout* Component::GetParentLayout() const {
	return parent_layout_;
}

Component::Type Component::GetType() const {
	return kComponent;
}

GUIImageManager* Component::GetImageManager() {
	return image_manager_;
}

void Component::SetImageManager(GUIImageManager* image_manager) {
	image_manager_ = image_manager;
}

Component* Component::GetChild(const str& name) const {
	for (int i = 0; i < layer_count_; ++i) {
		if (layout_[i]) {
			Component* _child = layout_[i]->GetFirst();
			while (_child != 0) {
				if (_child->GetName() == name) {
					return _child;
				}
				Component* grand_child = _child->GetChild(name);
				if (grand_child) {
					return grand_child;
				}
				_child = layout_[i]->GetNext();
			}
		}
	}
	return 0;
}

void Component::SetName(const str& name) {
	name_ = name;
}

const str& Component::GetName() const {
	return name_;
}



GUIImageManager* Component::image_manager_ = 0;

loginstance(kUiGfx2D, Component);



}
