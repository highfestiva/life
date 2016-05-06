
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

//NOTES:
//
//There are five different events that can occur to a button.
//The button can be...
//
//1. Pressed
//2. Released
//3. Clicked
//4. Dragged
//
//As soon as the user presses the left mouse button over a button
//the button is considered "Pressed". Keeping the
//mouse button down, moving the mouse cursor "Drags" the button.
//This can be used to create scroll bars and slide bars etc.
//
//Still keeping the mouse button down, moving the cursor outside
//the button triggers a "Released"-event (but not "Clicked").
//Releasing the mouse button outside the button will not result
//in any other events.
//
//Moving the cursor back over the button (still keeping the mouse
//button down) will generate a new "Pressed"-event (but not "Clicked").
//Finally, releaseing the mouse button over the button will generate
//the "Clicked"- and the "Released"-event.



#pragma once

#include "../../../thirdparty/FastDelegate/FastDelegate.h"
#include "uitextcomponent.h"
#include "uiwindow.h"



namespace uitbc {



class Button;

template <class _TButton> class ButtonType {
public:
	typedef fastdelegate::FastDelegate1<_TButton*, void> Delegate;
	typedef fastdelegate::FastDelegate3<_TButton*, int, int, bool> DelegateXY;
	typedef fastdelegate::FastDelegate5<_TButton*, int, int, int, int, bool> DelegateXYXY;
};

#define SetOnPress(_class, _func) \
	SetOnPressDelegate(uitbc::ButtonType<uitbc::Button>::Delegate(this, &_class::_func))

#define SetOnRelease(_class, _func) \
	SetOnReleaseDelegate(uitbc::ButtonType<uitbc::Button>::Delegate(this, &_class::_func))

#define SetOnClick(_class, _func) \
	SetOnClickDelegate(uitbc::ButtonType<uitbc::Button>::Delegate(this, &_class::_func))

#define SetOnDrag(_class, _func) \
	SetOnDragDelegate(uitbc::ButtonType<uitbc::Button>::DelegateXYXY(this, &_class::_func))


class Button: public Window, public TextComponent {
	typedef Window Parent;
public:
	typedef ButtonType<Button>::Delegate Delegate;
	typedef ButtonType<Button>::DelegateXY DelegateXY;
	typedef ButtonType<Button>::DelegateXYXY DelegateXYXY;

	enum State {
		kReleased = 0,
		kReleasedHoover,
		kReleasing,
		kPressed,
		kPressedHoover,
		kPressing,
	};

	Button(const wstr& text);
	Button(const Color& color,
		   const wstr& text);
	Button(BorderComponent::BorderShadeFunc shade_func,
		   int border_width,
		   const Color& color,
		   const wstr& text);
	Button(Painter::ImageID released_image_id,
		   Painter::ImageID pressed_image_id,
		   Painter::ImageID released_active_image_id,	// Mouse over.
		   Painter::ImageID pressed_active_image_id,
		   Painter::ImageID releasing_image_id,
		   Painter::ImageID pressing_image_id,
		   const wstr& text);

	virtual ~Button();

	virtual void SetBaseColor(const Color& color);
	virtual void SetPressColor(const Color& color);

	virtual void SetPressed(bool pressed);
	bool GetPressed();

	// These functors will be called every time the button is pressed and released
	// GRAPHICALLY.
	void SetOnPressDelegate(const Delegate& on_press);
	void SetOnReleaseDelegate(const Delegate& on_release);
	// Actual click.
	void SetOnClickDelegate(const Delegate& on_click);
	void SetOnDragDelegate(const DelegateXYXY& on_drag);

	void SetIcon(Painter::ImageID icon_id, IconAlignment alignment);
	Painter::ImageID GetIconCanvas() const;
	void SetHighlightedIcon(Painter::ImageID icon_id);
	void SetDisabledIcon(Painter::ImageID icon_id);
	virtual Painter::ImageID GetCurrentIcon() const;

	void SetText(const wstr& text, const Color& text_color = lepra::WHITE, const Color& backg_color = lepra::BLACK);
	const wstr& GetText();

	virtual void Repaint(Painter* painter);
	virtual void RepaintBackground(Painter* painter);
	virtual void RepaintComponents(Painter* painter);

	virtual Type GetType() const;

	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnLButtonUp(int mouse_x, int mouse_y);
	virtual bool OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y);
	virtual bool Click(bool depress);

	void PrintText(Painter* painter, int x, int y);

	void SetExtraData(void* data);
	void* GetExtraData() const;
	void SetTag(int tag);
	int GetTag() const;

	State GetState() const;
	void SetState(State state);

	virtual void ForceRepaint();
	virtual void OnTextChanged();
	virtual StateComponentList GetStateList(ComponentState state);

	Delegate* on_press_;
	Delegate* on_release_;
	Delegate* on_click_;
	DelegateXYXY* on_drag_;

protected:
	Painter::ImageID released_image_id_;
	Painter::ImageID pressed_image_id_;
	Painter::ImageID released_active_image_id_;
	Painter::ImageID pressed_active_image_id_;
	Painter::ImageID releasing_image_id_;
	Painter::ImageID pressing_image_id_;

	Painter::ImageID icon_id_;
	Painter::ImageID highlighted_icon_id_;
	Painter::ImageID disabled_icon_id_;
	IconAlignment icon_alignment_;

	wstr text_;
	int text_margin_;
	Color text_backg_color_;

	Color hoover_color_;
	Color press_color_;

	bool pressed_;
	bool image_button_;

	State state_;

	void* extra_data_;

	logclass();
};



}
