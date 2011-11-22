
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games

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

#include "../../../ThirdParty/FastDelegate/FastDelegate.h"
#include "UiWindow.h"



namespace UiTbc
{



class Button;

template <class _TButton> class ButtonType
{
public:
	typedef fastdelegate::FastDelegate1<_TButton*, void> Delegate;
	typedef fastdelegate::FastDelegate3<_TButton*, int, int, bool> DelegateXY;
};

#define SetOnPress(_class, _func) \
	SetOnPressDelegate(UiTbc::ButtonType<UiTbc::Button>::Delegate(this, &_class::_func))

#define SetOnRelease(_class, _func) \
	SetOnReleaseDelegate(UiTbc::ButtonType<UiTbc::Button>::Delegate(this, &_class::_func))

#define SetOnClick(_class, _func) \
	SetOnClickDelegate(UiTbc::ButtonType<UiTbc::Button>::Delegate(this, &_class::_func))

#define SetOnDrag(_class, _func) \
	SetOnDragDelegate(UiTbc::ButtonType<UiTbc::Button>::DelegateXY(this, &_class::_func))


class Button: public Window
{
	typedef Window Parent;
public:
	typedef ButtonType<Button>::Delegate Delegate;
	typedef ButtonType<Button>::DelegateXY DelegateXY;

	enum IconAlignment
	{
		ICON_LEFT = 0,
		ICON_CENTER,
		ICON_RIGHT,
	};
	enum State
	{
		RELEASED = 0,
		RELEASED_HOOVER,
		RELEASING,
		PRESSED,
		PRESSED_HOOVER,
		PRESSING,
	};

	Button(const str& pName);
	Button(const Color& pColor,
		   const str& pName);
	Button(BorderComponent::BorderShadeFunc pShadeFunc,
		   int pBorderWidth,
		   const Color& pColor,
		   const str& pName);
	Button(Painter::ImageID pReleasedImageID,
		   Painter::ImageID pPressedImageID,
		   Painter::ImageID pReleasedActiveImageID,	// Mouse over.
		   Painter::ImageID pPressedActiveImageID,
		   Painter::ImageID pReleasingImageID,
		   Painter::ImageID pPressingImageID,
		   const str& pName);

	virtual ~Button();

	virtual void SetBaseColor(const Color& pColor);

	virtual void SetPressed(bool pPressed);
	bool GetPressed();

	// These functors will be called every time the button is pressed and released
	// GRAPHICALLY.
	void SetOnPressDelegate(const Delegate& pOnPress);
	void SetOnReleaseDelegate(const Delegate& pOnRelease);
	// Actual click.
	void SetOnClickDelegate(const Delegate& pOnClick);
	void SetOnDragDelegate(const DelegateXY& pOnDrag);

	void SetIcon(Painter::ImageID pIconID, IconAlignment pAlignment);
	Painter::ImageID GetIconCanvas() const;
	void SetHighlightedIcon(Painter::ImageID pIconId);

	void SetText(const str& pText, const Color& pTextColor = Lepra::WHITE, const Color& pBackgColor = Lepra::BLACK);
	const str& GetText();

	virtual void Repaint(Painter* pPainter);

	virtual Type GetType() const;

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);
	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);
	virtual bool Click(bool pDepress);

	void PrintText(Painter* pPainter, int x, int y);

	void SetExtraData(void* pData);
	void* GetExtraData() const;
	void SetTag(int pTag);
	int GetTag() const;

	State GetState();
	void SetState(State pState);

protected:
	virtual void OnTextChanged();
	virtual StateComponentList GetStateList(ComponentState pState);

protected:
	Delegate* mOnPress;
	Delegate* mOnRelease;
	Delegate* mOnClick;
	DelegateXY* mOnDrag;

private:
	Painter::ImageID mReleasedImageID;
	Painter::ImageID mPressedImageID;
	Painter::ImageID mReleasedActiveImageID;
	Painter::ImageID mPressedActiveImageID;
	Painter::ImageID mReleasingImageID;
	Painter::ImageID mPressingImageID;

	Painter::ImageID mIconID;
	Painter::ImageID mHighlightedIconId;
	IconAlignment mIconAlignment;

	str mText;
	Color mTextColor;
	Color mTextBackgColor;

	Color mHooverColor;
	Color mPressColor;
	
	bool mPressed;
	bool mImageButton;

	State mState;

	void* mExtraData;
};



}
