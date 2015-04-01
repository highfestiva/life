
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../../UiLepra/Include/UiInput.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/ListUtil.h"
#include "../../Include/GUI/UiDesktopWindow.h"



namespace UiTbc
{



DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, 
	Layout* pLayout, const tchar* pImageDefinitionFile,
	const tchar* pArchive):
	RectComponent(_T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mMouseButtonFlags(0),
	mPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, const Color& pColor,
	Layout* pLayout, const tchar* pImageDefinitionFile, const tchar* pArchive):
	RectComponent(pColor, _T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter,
	const Color& pTopLeftColor, const Color& pTopRightColor,
	const Color& pBottomRightColor, const Color& pBottomLeftColor,
	Layout* pLayout, const tchar* pImageDefinitionFile,
	const tchar* pArchive):
	RectComponent(pTopLeftColor, pTopRightColor, pBottomRightColor, pBottomLeftColor, _T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, Painter::ImageID pImageID,
	Layout* pLayout, const tchar* pImageDefinitionFile, const tchar* pArchive):
	RectComponent(pImageID, _T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::~DesktopWindow()
{
	PurgeDeleted();

	// TRICKY: this has gotta be done here, too. Component parent
	// does this, but only after the image manager is deleted, which
	// causes the component still holding images to blow up when they
	// get killed.
	DeleteAllLayers();

	ListUtil::DeleteAll(mCleanerList);

	mInputManager->RemoveTextInputObserver(this);
	mInputManager->RemoveKeyCodeInputObserver(this);
	mInputManager->RemoveMouseInputObserver(this);
	mInputManager = 0;
}

UiLepra::InputManager* DesktopWindow::GetInputManager() const
{
	return (mInputManager);
}

void DesktopWindow::Init(const tchar* /*pImageDefinitionFile*/, const tchar* /*pArchive*/)
{
	// Let all components access the image manager.
	SetImageManager(&mImageManager);

	mImageManager.SetPainter(mPainter);

	UiLepra::InputDevice* lMouse = 0;
	if (mInputManager)
	{
		mInputManager->AddTextInputObserver(this);
		mInputManager->AddKeyCodeInputObserver(this);
		mInputManager->AddMouseInputObserver(this);
		lMouse = mInputManager->GetMouse();
	}

	if (lMouse != 0)
	{
		UiLepra::InputElement* lButton1 = lMouse->GetButton(0);
		UiLepra::InputElement* lButton2 = lMouse->GetButton(1);
		UiLepra::InputElement* lButton3 = lMouse->GetButton(2);

		if (lButton1 != 0)
		{
			ADD_INPUT_CALLBACK(lButton1, OnButton1, DesktopWindow);
		}
		if (lButton2 != 0)
		{
			ADD_INPUT_CALLBACK(lButton2, OnButton2, DesktopWindow);
		}
		if (lButton3 != 0)
		{
			ADD_INPUT_CALLBACK(lButton3, OnButton3, DesktopWindow);
		}
	}

//	This code is left as a comment to remind you that reading the keyboard
//	shouldn't be done the same way as the mouse. It is possible of course,
//	but it would require huge amounts of code and isn't practial. 
//
//	UiLepra::InputDevice* lKeyboard = mInputManager->GetKeyboard();
//
//	if (lKeyboard != 0)
//	{
//
//	}

}

void DesktopWindow::PurgeDeleted()
{
	// Delete all prerendered images from the painter.
	ComponentList::iterator lIter;
	for (lIter = mDeleteQueue.begin(); lIter != mDeleteQueue.end(); ++lIter)
	{
		Component* lChild = *lIter;
		if (lChild->mImageID != Painter::INVALID_IMAGEID)
		{
			mPainter->RemoveImage(lChild->mImageID);
			lChild->ReleaseKeyboardFocus();
		}
		RemoveChild(lChild, 0);
	}
	// Delete all queued components.
	ListUtil::DeleteAll(mDeleteQueue);
}

void DesktopWindow::DoSetSize(int pWidth, int pHeight)
{
	Parent::DoSetSize(pWidth, pHeight);
	PixelCoord lPos(GetScreenPos());
	mMouseArea.Set(lPos.x, lPos.y, lPos.x + pWidth, lPos.y + pHeight);
}

void DesktopWindow::SetMouseEnabled(bool pEnabled)
{
	mMouseEnabled = pEnabled;
}

void DesktopWindow::ClampMouse(int& x, int& y) const
{
	bool lUpdateMouse = false;

	if (x < mMouseArea.mLeft)
	{
		x = mMouseArea.mLeft;
		lUpdateMouse = true;
	}

	if (x >= mMouseArea.mRight)
	{
		x = mMouseArea.mRight - 1;
		lUpdateMouse = true;
	}

	if (y < mMouseArea.mTop)
	{
		y = mMouseArea.mTop;
		lUpdateMouse = true;
	}

	if (y >= mMouseArea.mBottom)
	{
		y = mMouseArea.mBottom - 1;
		lUpdateMouse = true;
	}
}

void DesktopWindow::AddChild(Component* pChild, int pParam1, int pParam2, int pLayer)
{
	Parent::AddChild(pChild, pParam1, pParam2, pLayer);
	pChild->OnConnectedToDesktopWindow();
}

void DesktopWindow::Repaint(Painter* /*pPainter*/)
{
	PurgeDeleted();

	// Call OnIdle() on all subscribers.
	ComponentList::iterator lIter;
	for (lIter = mIdleSubscribers.begin(); 
		lIter != mIdleSubscribers.end(); ++lIter)
	{
		(*lIter)->OnIdle();
	}

	// Handle mouse...
	GetCursorPosition(mMouseX, mMouseY);
	DispatchMouseMove(mMouseX, mMouseY);

	if (NeedsRepaint() == true || mUpdateLayout)
	{
		UpdateLayout();
		mUpdateLayout = false;
	}
	Parent::Repaint(mPainter);
}

void DesktopWindow::RepaintChild(Component* pChild, Painter* pPainter)
{
	pChild->Repaint(pPainter);
}

void DesktopWindow::OnButton1(UiLepra::InputElement* pElement)
{
	if (mMouseEnabled == true)
	{
		int lMouseX;
		int lMouseY;
		GetCursorPosition(lMouseX, lMouseY);
		if (pElement->GetBooleanValue() == true)
		{
			if (OnLButtonDown(lMouseX, lMouseY))
			{
				mMouseButtonFlags |= CONSUMED_MOUSE_BUTTON1;
			}
		}
		else
		{
			mMouseButtonFlags &= ~CONSUMED_MOUSE_BUTTON1;
			DispatchMouseMove(lMouseX, lMouseY);
			OnLButtonUp(lMouseX, lMouseY);
		}
	}
}

void DesktopWindow::OnButton2(UiLepra::InputElement* pElement)
{
	if (mMouseEnabled == true)
	{
		int lMouseX;
		int lMouseY;
		GetCursorPosition(lMouseX, lMouseY);
		if (pElement->GetBooleanValue() == true)
		{
			if (OnRButtonDown(lMouseX, lMouseY))
			{
				mMouseButtonFlags |= CONSUMED_MOUSE_BUTTON2;
			}
		}
		else
		{
			mMouseButtonFlags &= ~CONSUMED_MOUSE_BUTTON2;
			DispatchMouseMove(lMouseX, lMouseY);
			OnRButtonUp(lMouseX, lMouseY);
		}
	}
}

void DesktopWindow::OnButton3(UiLepra::InputElement* pElement)
{
	if (mMouseEnabled == true)
	{
		int lMouseX;
		int lMouseY;
		GetCursorPosition(lMouseX, lMouseY);
		if (pElement->GetBooleanValue() == true)
		{
			if (OnMButtonDown(lMouseX, lMouseY))
			{
				mMouseButtonFlags |= CONSUMED_MOUSE_BUTTON3;
			}
		}
		else
		{
			mMouseButtonFlags &= ~CONSUMED_MOUSE_BUTTON3;
			DispatchMouseMove(lMouseX, lMouseY);
			OnMButtonUp(lMouseX, lMouseY);
		}
	}
}

void DesktopWindow::GetCursorPosition(int& pMouseX, int& pMouseY) const
{
	if (!mInputManager)
	{
		return;
	}

	float64 lWidth  = (float64)mMouseArea.GetWidth();
	float64 lHeight = (float64)mMouseArea.GetHeight();

	pMouseX = (int)((mInputManager->GetCursorX() + 1.0) * lWidth / 2.0);
	pMouseY = (int)((mInputManager->GetCursorY() + 1.0) * lHeight / 2.0);

	ClampMouse(pMouseX, pMouseY);
}

void DesktopWindow::DispatchMouseMove(int pMouseX, int pMouseY)
{
	if (!mMouseEnabled)
	{
		return;
	}

	if (mMousePrevX != pMouseX || mMousePrevY != pMouseY)
	{
		OnMouseMove(pMouseX, pMouseY, pMouseX - mMousePrevX, pMouseY - mMousePrevY);
		mMousePrevX = pMouseX;
		mMousePrevY = pMouseY;
	}
}

bool DesktopWindow::OnChar(tchar pChar)
{
	return (Parent::OnChar(pChar));
}

bool DesktopWindow::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	return (Parent::OnKeyDown(pKeyCode));
}

bool DesktopWindow::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	return (Parent::OnKeyUp(pKeyCode));
}

bool DesktopWindow::OnDoubleClick()
{
	Parent::OnDoubleClick();

	if (mMouseEnabled == true)
	{
		int lMouseX;
		int lMouseY;
		GetCursorPosition(lMouseX, lMouseY);
		Parent::OnDoubleClick(lMouseX, lMouseY);
	}
	return (false);
}

DesktopWindow::MouseButtonFlags DesktopWindow::GetMouseButtonFlags() const
{
	return (MouseButtonFlags)mMouseButtonFlags;
}

void DesktopWindow::PostDeleteComponent(Component* pComponent, int /*pLayer*/)
{
	mDeleteQueue.push_back(pComponent);
}

Painter* DesktopWindow::GetPainter()
{
	deb_assert(this);
	return (mPainter);
}

void DesktopWindow::SetKeyboardEnabled(bool pEnabled)
{
	mKeyboardEnabled = pEnabled;
}

void DesktopWindow::SetUpdateLayout(bool pUpdateLayout)
{
	mUpdateLayout = pUpdateLayout;
}

void DesktopWindow::AddIdleSubscriber(Component* pComponent)
{
	mIdleSubscribers.push_back(pComponent);
	mIdleSubscribers.unique();
}

void DesktopWindow::RemoveIdleSubscriber(Component* pComponent)
{
	mIdleSubscribers.remove(pComponent);
}

void DesktopWindow::ActivateKeyboard()
{
	mInputManager->ActivateKeyboard();
}

void DesktopWindow::DeactivateKeyboard()
{
	mInputManager->ReleaseKeyboard();
}

void DesktopWindow::AddCleaner(Cleaner* pCleaner)
{
	mCleanerList.push_back(pCleaner);
	mCleanerList.unique();
}

Component::Type DesktopWindow::GetType() const
{
	return Component::DESKTOPWINDOW;
}



loginstance(UI_GFX_2D, DesktopWindow);



}
