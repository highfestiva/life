
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uirectcomponent.h"
#include <list>
#include "../../../uilepra/include/uiinput.h"
#include "uicleaner.h"
#include "uiguiimagemanager.h"



namespace uitbc {



class Cleaner;



class DesktopWindow: public RectComponent {
	typedef RectComponent Parent;
public:
	enum MouseButtonFlags {
		kConsumedMouseButton1 = 1<<0,
		kConsumedMouseButton2 = 1<<1,
		kConsumedMouseButton3 = 1<<2,
		kConsumedMouseButton  = (kConsumedMouseButton1 | kConsumedMouseButton2 | kConsumedMouseButton3),
	};

	DesktopWindow(uilepra::InputManager* input_manager,
		Painter* painter,
		Layout* layout = 0,
		const char* image_definition_file = 0,
		const char* archive = 0);
	DesktopWindow(uilepra::InputManager* input_manager,
		Painter* painter,
		const Color& color,
		Layout* layout = 0,
		const char* image_definition_file = 0,
		const char* archive = 0);
	DesktopWindow(uilepra::InputManager* input_manager,
		Painter* painter,
		const Color& top_left_color,
		const Color& top_right_color,
		const Color& bottom_right_color,
		const Color& bottom_left_color,
		Layout* layout = 0,
		const char* image_definition_file = 0,
		const char* archive = 0);
	DesktopWindow(uilepra::InputManager* input_manager,
		Painter* painter,
		Painter::ImageID image_id,
		Layout* layout = 0,
		const char* image_definition_file = 0,
		const char* archive = 0);

	virtual ~DesktopWindow();

	uilepra::InputManager* GetInputManager() const;

	void SetMouseEnabled(bool enabled);
	void SetKeyboardEnabled(bool enabled);

	void SetUpdateLayout(bool update_layout);

	// Note: If the component is a child component of one of the DesktopWindow's
	// children, you have to make sure that it removes itself from the subscriber
	// list before it is deleted. Otherwise you have added a crash bug to the code.
	void AddIdleSubscriber(Component* component);
	void RemoveIdleSubscriber(Component* component);

	void ActivateKeyboard();
	void DeactivateKeyboard();

	// Wait for destruction of the DesktopWindow, in which case all cleaners will
	// be deleted. This is a special solution to cope with the problem of cleaning up
	// among static variables stored in various GUI component classes.
	void AddCleaner(Cleaner* cleaner);

	virtual void AddChild(Component* child, int param1 = 0, int param2 = 0, int layer = 0);

	// The painter sent to Repaint() will be ignored, since the painter is already
	// given through the constructor.
	virtual void Repaint(Painter* painter = 0);
	virtual void RepaintChild(Component* child, Painter* painter);

	virtual bool OnChar(wchar_t c);
	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);
	virtual bool OnKeyUp(uilepra::InputManager::KeyCode key_code);
	virtual bool OnDoubleClick();
	MouseButtonFlags GetMouseButtonFlags() const;

	virtual void DoSetSize(int width, int height);

	virtual Type GetType() const;

	// This function will put the component in the delete queue
	// which is processed once for every call to Repaint().
	void PostDeleteComponent(Component* component, int layer);

	// Returns the painter given in the constructor.
	Painter* GetPainter();

protected:
private:
	typedef std::list<Component*> ComponentList;

	void OnButton1(uilepra::InputElement* element);
	void OnButton2(uilepra::InputElement* element);
	void OnButton3(uilepra::InputElement* element);
	void GetCursorPosition(int& mouse_x, int& mouse_y) const;
	void DispatchMouseMove(int mouse_x, int mouse_y);

	void Init(const char* image_definition_file, const char* archive);
	void ClampMouse(int& x, int& y) const;

	void PurgeDeleted();

	uilepra::InputManager* input_manager_;

	bool mouse_enabled_;
	bool keyboard_enabled_;
	bool render_rect_;
	bool update_layout_;

	ComponentList idle_subscribers_;
	std::list<Cleaner*> cleaner_list_;
	ComponentList delete_queue_;

	PixelRect mouse_area_;

	int mouse_x_;
	int mouse_y_;
	int mouse_prev_x_;
	int mouse_prev_y_;
	int mouse_button_flags_;

	Painter* painter_;

	GUIImageManager image_manager_;

	logclass();
};



}
