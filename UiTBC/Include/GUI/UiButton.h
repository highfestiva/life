
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
//4. Unclicked
//5. Dragged
//
//As soon as the user presses the left mouse button over a button
//the button is considered "Clicked" and "Pressed". Keeping the
//mouse button down, moving the mouse cursor "Drags" the button.
//This can be used to create scroll bars and slide bars etc.
//
//Still keeping the mouse button down, moving the cursor outside
//the button triggers a "Released"-event (but not "Unclicked").
//Releasing the mouse button outside the button will not result
//in any other events. 
//
//Moving the cursor back over the button (still keeping the mouse 
//button down) will generate a new "Pressed"-event (but not "Clicked").
//Finally, releaseing the	mouse button over the button will generate 
//the "Unclicked"- and the "Released"-event.



#pragma once

#include "UiWindow.h"



namespace UiTbc
{



class Button;



class ButtonFunctor
{
public:
	virtual void Call(Button* pButton) = 0;
	virtual ButtonFunctor* CreateCopy() const = 0;
};

class ButtonDraggedFunctor
{
public:
	virtual void Call(Button* pButton, int pDeltaX, int pDeltaY) = 0;
	virtual ButtonDraggedFunctor* CreateCopy() const = 0;
};

template<class _TClass> class TButtonTypeBase
{
public:
	typedef void (_TClass::*_TButtonFunc)(Button*);
	typedef void (_TClass::*_TButtonIndexFunc)(Button*, int);

	TButtonTypeBase(_TClass* pObject):
		mObject(pObject)
	{
	}
protected:
	_TClass* mObject;
};

template<class _TClass, class _TFunc> class TButtonFunctorBase: public TButtonTypeBase<_TClass>
{
public:
	typedef _TFunc FuncType;

	TButtonFunctorBase(_TClass* pObject, _TFunc pFunc):
		TButtonTypeBase(pObject),
		mFunc(pFunc)
	{
	}
protected:
	_TFunc mFunc;
};

template<class _TClass> class TButtonFunctor: public ButtonFunctor,
	public TButtonFunctorBase<_TClass, typename TButtonTypeBase<_TClass>::_TButtonFunc>
{
public:
	TButtonFunctor(_TClass* pObject, FuncType pFunc):
		TButtonFunctorBase(pObject, pFunc)
	{
	}
	virtual void Call(Button* pButton)
	{
		(mObject->*mFunc)(pButton);
	}
	ButtonFunctor* CreateCopy() const
	{
		return new TButtonFunctor(mObject, mFunc);
	}
};

template<class _TClass> class TButtonIndexFunctor: public ButtonFunctor,
	public TButtonFunctorBase<_TClass, typename TButtonTypeBase<_TClass>::_TButtonIndexFunc>
{
public:
	TButtonIndexFunctor(_TClass* pObject, FuncType pFunc, int pIndex):
		TButtonFunctorBase(pObject, pFunc),
		mIndex(pIndex)
	{
	}
	virtual void Call(Button* pButton)
	{
		(mObject->*mFunc)(pButton, mIndex);
	}
	ButtonFunctor* CreateCopy() const
	{
		return new TButtonIndexFunctor(mObject, mFunc, mIndex);
	}
protected:
	int mIndex;
};

template<class _TClass> class TButtonDraggedFunctor : public ButtonDraggedFunctor
{
public:
	TButtonDraggedFunctor(_TClass* pObject, 
				     void (_TClass::*pFunc)(Button* pButton, int pDeltaX, int pDeltaY)) :
		mObject(pObject),
		mFunc(pFunc)
	{
	}
	
	virtual void Call(Button* pButton, int pDeltaX, int pDeltaY)
	{
		(mObject->*mFunc)(pButton, pDeltaX, pDeltaY);
	}

	ButtonDraggedFunctor* CreateCopy() const
	{
		return new TButtonDraggedFunctor(mObject, mFunc);
	}

private:
	_TClass* mObject;
	void (_TClass::*mFunc)(Button* pButton, int pDeltaX, int pDeltaY);
};

#define SetOnPressedFunc(_class, _func) \
	SetOnPressedFunctor(UiTbc::TButtonFunctor<_class>(this, &_class::_func));

#define SetOnReleasedFunc(_class, _func) \
	SetOnReleasedFunctor(UiTbc::TButtonFunctor<_class>(this, &_class::_func));

#define SetOnDraggedFunc(_class, _func) \
	SetOnButtonDraggedFunctor(UiTbc::TButtonDraggedFunctor<_class>(this, &_class::_func));

#define SetOnClickedFunc(_class, _func) \
	SetOnClickedFunctor(UiTbc::TButtonFunctor<_class>(this, &_class::_func));

#define SetOnUnclickedFunc(_class, _func) \
	SetOnUnclickedFunctor(UiTbc::TButtonFunctor<_class>(this, &_class::_func));

#define SetOnUnclickedFuncIndex(_class, _func, _index) \
	SetOnUnclickedFunctor(UiTbc::TButtonIndexFunctor<_class>(this, &_class::_func, _index));



class Button : public Window
{
	typedef Window Parent;
public:

	enum IconAlignment
	{
		ICON_LEFT = 0,
		ICON_CENTER,
		ICON_RIGHT,
	};

	Button(const Lepra::String& pName);
	Button(const Lepra::Color& pColor,
		   const Lepra::String& pName);
	Button(BorderComponent::BorderShadeFunc pShadeFunc,
		   int pBorderWidth,
		   const Lepra::Color& pColor,
		   const Lepra::String& pName);
	Button(Painter::ImageID pReleasedImageID,
		   Painter::ImageID pPressedImageID,
		   Painter::ImageID pReleasedActiveImageID,	// Mouse over.
		   Painter::ImageID pPressedActiveImageID,
		   Painter::ImageID pReleasingImageID,
		   Painter::ImageID pPressingImageID,
		   const Lepra::String& pName);

	virtual ~Button();

	void InitializeHoover();

	virtual void SetPressed(bool pPressed);
	bool GetPressed();

	// These functors will be called every time the button is pressed and released
	// GRAPHICALLY.
	void SetOnPressedFunctor(const ButtonFunctor& pOnPressedFunctor);
	void SetOnReleasedFunctor(const ButtonFunctor& pOnReleasedFunctor);

	// These functors will be called every time the user actually clicks/releases
	// the mouse button.
	void SetOnClickedFunctor(const ButtonFunctor& pOnClickedFunctor);
	void SetOnUnclickedFunctor(const ButtonFunctor& pOnUnclickFunctor);

	void SetOnButtonDraggedFunctor(const ButtonDraggedFunctor& pOnButtonDraggedFunctor);

	void SetIcon(Painter::ImageID pIconID,
			    IconAlignment pAlignment);

	void SetText(const Lepra::String& pText,
			    const Lepra::Color& pTextColor,
			    const Lepra::Color& pBackgColor);
	const Lepra::String& GetText();

	virtual void Repaint(Painter* pPainter);

	virtual Component::Type GetType();

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);
	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);
	virtual bool Click(bool pDepress);

protected:

	enum State
	{
		RELEASED = 0,
		RELEASED_HOOVER,
		RELEASING,
		PRESSED,
		PRESSED_HOOVER,
		PRESSING,
	};

	void PrintText(Painter* pPainter, int x, int y);

	virtual void OnTextChanged();
	State GetState();
	void SetState(State pState);

	virtual StateComponentList GetStateList(ComponentState pState) const;

	ButtonFunctor* GetOnPressedFunctor();
	ButtonFunctor* GetOnReleasedFunctor();
	ButtonFunctor* GetOnClickedFunctor();
	ButtonFunctor* GetOnUnclickedFunctor();
	ButtonDraggedFunctor* GetOnButtonDraggedFunctor();


private:

	Painter::ImageID mReleasedImageID;
	Painter::ImageID mPressedImageID;
	Painter::ImageID mReleasedActiveImageID;
	Painter::ImageID mPressedActiveImageID;
	Painter::ImageID mReleasingImageID;
	Painter::ImageID mPressingImageID;

	Painter::ImageID mIconID;
	IconAlignment mIconAlignment;

	Lepra::String mText;
	Lepra::Color mTextColor;
	Lepra::Color mTextBackgColor;

	Lepra::Color mHooverColor;
	Lepra::Color mPressColor;
	
	bool mPressed;
	bool mImageButton;

	State mState;

	ButtonFunctor* mOnPressedFunctor;
	ButtonFunctor* mOnReleasedFunctor;
	ButtonFunctor* mOnClickedFunctor;
	ButtonFunctor* mOnUnclickedFunctor;
	ButtonDraggedFunctor* mOnButtonDraggedFunctor;
};



}
