
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../uiinput.h"
#include "../x11/uix11displaymanager.h"
#include "../uilepra.h"



namespace uilepra {



class X11DisplayManager;
class X11InputDevice;



class X11InputElement: public InputElement {
public:
	X11InputElement(InputElement::Type type, Interpretation interpretation, int type_index,
		X11InputDevice* parent_device);
	virtual ~X11InputElement();

protected:
private:
	logclass();
};



class X11InputDevice: public InputDevice {
public:
	X11InputDevice(InputManager* manager);
	virtual ~X11InputDevice();

	void AddElement(X11InputElement* element);

	virtual void Activate();
	virtual void Release();

	virtual void PollEvents();

protected:
private:
	logclass();
};



class X11InputManager: public InputManager, public X11Observer {
	typedef InputManager Parent;
public:
	// Declared as friend in order to get access to screen width and height.
	friend class X11InputElement;

	X11InputManager(X11DisplayManager* display_manager);
	virtual ~X11InputManager();

	virtual void PreProcessEvents();
	virtual void PollEvents();
	virtual void Refresh();

	X11DisplayManager* GetDisplayManager() const;

	virtual const InputDevice* GetKeyboard() const;
	virtual InputDevice* GetKeyboard();

	// Returns the mouse device, if there is one... NULL otherwise.
	virtual const InputDevice* GetMouse() const;
	virtual InputDevice* GetMouse();

	virtual void SetCursorVisible(bool visible);

	virtual float GetCursorX();
	virtual float GetCursorY();
	virtual void SetMousePosition(int x, int y);

	bool IsInitialized();

protected:
	bool OnMessage(const XEvent& event);
	static KeyCode TranslateKey(int state, KeySym key_sym, bool& is_special_key);

private:
	void AddObserver();
	void RemoveObserver();

	X11DisplayManager* display_manager_;

	bool initialized_;

	// The entire display area (not just the user's window).
	int screen_width_;
	int screen_height_;

	// Mouse related stuff.
	float cursor_x_;
	float cursor_y_;
	bool ignore_next_mouse_move_;	// Leave+enter.
	bool grab_cursor_;	// Used when invisible but lost focus.
	int mouse_grab_x_;
	int mouse_grab_y_;
	int mouse_grab_delta_x_;
	int mouse_grab_delta_y_;

	// Default devices.
	InputDevice* keyboard_;
	InputDevice* mouse_;
	int type_count_[InputDevice::kTypeCount];

	XIC input_context_;

	logclass();
};



}
