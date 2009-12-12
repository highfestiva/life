
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../../UiLepra/Include/UiInput.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/ListUtil.h"
#include "../../Include/GUI/UiDesktopWindow.h"



namespace UiTbc
{



DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, 
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile,
	const Lepra::tchar* pArchive):
	RectComponent(_T("DesktopWindow"), pLayout),
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

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, const Lepra::Color& pColor,
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile, const Lepra::tchar* pArchive):
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
	const Lepra::Color& pTopLeftColor, const Lepra::Color& pTopRightColor,
	const Lepra::Color& pBottomRightColor, const Lepra::Color& pBottomLeftColor,
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile,
	const Lepra::tchar* pArchive):
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
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile, const Lepra::tchar* pArchive):
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

	Lepra::ListUtil::DeleteAll(mCleanerList);

	mInputManager->RemoveTextInputObserver(this);
	mInputManager->RemoveKeyCodeInputObserver(this);
	mInputManager->RemoveMouseInputObserver(this);
	mInputManager = 0;
}

UiLepra::InputManager* DesktopWindow::GetInputManager() const
{
	return (mInputManager);
}

void DesktopWindow::Init(const Lepra::tchar* /*pImageDefinitionFile*/, const Lepra::tchar* /*pArchive*/)
{
	// Let all components access the image manager.
	SetImageManager(&mImageManager);

	mImageManager.SetPainter(mPainter);

	mInputManager->AddTextInputObserver(this);
	mInputManager->AddKeyCodeInputObserver(this);
	mInputManager->AddMouseInputObserver(this);

	UiLepra::InputDevice* lMouse = mInputManager->GetMouse();

	if (lMouse != 0)
	{
		UiLepra::InputElement* lButton1 = lMouse->GetButton(0);
		UiLepra::InputElement* lButton2 = lMouse->GetButton(1);
		UiLepra::InputElement* lButton3 = lMouse->GetButton(2);

		if (lButton1 != 0)
		{
			BIND_INPUT(lButton1, OnButton1, DesktopWindow);
		}
		if (lButton2 != 0)
		{
			BIND_INPUT(lButton2, OnButton2, DesktopWindow);
		}
		if (lButton3 != 0)
		{
			BIND_INPUT(lButton3, OnButton3, DesktopWindow);
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
	}
	// Delete all queued components.
	Lepra::ListUtil::DeleteAll(mDeleteQueue);
}

void DesktopWindow::DoSetSize(int pWidth, int pHeight)
{
	Parent::DoSetSize(pWidth, pHeight);
	Lepra::PixelCoords lPos(GetScreenPos());
	mMouseArea.Set(lPos.x, lPos.y, lPos.x + pWidth, lPos.y + pHeight);
}

void DesktopWindow::SetMouseEnabled(bool pEnabled)
{
	mMouseEnabled = pEnabled;
}

void DesktopWindow::ClampMouse(int& x, int& y)
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
	Lepra::float64 lWidth  = (Lepra::float64)mMouseArea.GetWidth();
	Lepra::float64 lHeight = (Lepra::float64)mMouseArea.GetHeight();

	mMouseX = (int)((mInputManager->GetCursorX() + 1.0) * lWidth / 2.0);
	mMouseY = (int)((mInputManager->GetCursorY() + 1.0) * lHeight / 2.0);

	ClampMouse(mMouseX, mMouseY);

	if (mMouseEnabled == true)
	{
		if (mMousePrevX != mMouseX || mMousePrevY != mMouseY)
		{
			OnMouseMove(mMouseX, mMouseY, mMouseX - mMousePrevX, mMouseY - mMousePrevY);
		}
		mMousePrevX = mMouseX;
		mMousePrevY = mMouseY;
	}

	// Some components (like TreeNode) needs one extra
	// repainting initially - hence the loop!
	int lLoopCount = 0;
	do
	{
		if (NeedsRepaint() == true || mUpdateLayout)
		{
			UpdateLayout();
			mUpdateLayout = false;
		}

		Parent::Repaint(mPainter);
		lLoopCount++;
	} while (NeedsRepaint() == true && lLoopCount < 4);
}

void DesktopWindow::RepaintChild(Component* pChild, Painter* pPainter)
{
	pChild->Repaint(pPainter);
}

void DesktopWindow::OnButton1(UiLepra::InputElement* pElement)
{
	if (mMouseEnabled == true)
	{
		Lepra::float64 lWidth  = (Lepra::float64)mMouseArea.GetWidth();
		Lepra::float64 lHeight = (Lepra::float64)mMouseArea.GetHeight();

		int lMouseX = (int)((mInputManager->GetCursorX() + 1.0) * lWidth / 2.0);
		int lMouseY = (int)((mInputManager->GetCursorY() + 1.0) * lHeight / 2.0);

		ClampMouse(lMouseX, lMouseY);
		
		if (pElement->GetBooleanValue() == true)
		{
			OnLButtonDown(lMouseX, lMouseY);
		}
		else
		{
			OnLButtonUp(lMouseX, lMouseY);
		}
	}
}

void DesktopWindow::OnButton2(UiLepra::InputElement* pElement)
{
	if (mMouseEnabled == true)
	{
		Lepra::float64 lWidth  = (Lepra::float64)mMouseArea.GetWidth();
		Lepra::float64 lHeight = (Lepra::float64)mMouseArea.GetHeight();

		int lMouseX = (int)((mInputManager->GetCursorX() + 1.0) * lWidth / 2.0);
		int lMouseY = (int)((mInputManager->GetCursorY() + 1.0) * lHeight / 2.0);

		ClampMouse(lMouseX, lMouseY);
		
		if (pElement->GetBooleanValue() == true)
		{
			OnRButtonDown(lMouseX, lMouseY);
		}
		else
		{
			OnRButtonUp(lMouseX, lMouseY);
		}
	}
}

void DesktopWindow::OnButton3(UiLepra::InputElement* pElement)
{
	if (mMouseEnabled == true)
	{
		Lepra::float64 lWidth  = (Lepra::float64)mMouseArea.GetWidth();
		Lepra::float64 lHeight = (Lepra::float64)mMouseArea.GetHeight();

		int lMouseX = (int)((mInputManager->GetCursorX() + 1.0) * lWidth / 2.0);
		int lMouseY = (int)((mInputManager->GetCursorY() + 1.0) * lHeight / 2.0);

		ClampMouse(lMouseX, lMouseY);
		
		if (pElement->GetBooleanValue() == true)
		{
			OnMButtonDown(lMouseX, lMouseY);
		}
		else
		{
			OnMButtonUp(lMouseX, lMouseY);
		}
	}
}

bool DesktopWindow::OnChar(Lepra::tchar pChar)
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
		Lepra::float64 lWidth  = (Lepra::float64)mMouseArea.GetWidth();
		Lepra::float64 lHeight = (Lepra::float64)mMouseArea.GetHeight();

		int lMouseX = (int)((mInputManager->GetCursorX() + 1.0) * lWidth / 2.0);
		int lMouseY = (int)((mInputManager->GetCursorY() + 1.0) * lHeight / 2.0);

		ClampMouse(lMouseX, lMouseY);
		
		Parent::OnDoubleClick(lMouseX, lMouseY);
	}
	return (false);
}

void DesktopWindow::DeleteComponent(Component* pComponent, int pLayer)
{
	RemoveChild(pComponent, pLayer);
	mDeleteQueue.push_back(pComponent);
}

Painter* DesktopWindow::GetPainter()
{
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

void DesktopWindow::AddCleaner(Cleaner* pCleaner)
{
	mCleanerList.push_back(pCleaner);
	mCleanerList.unique();
}

Component::Type DesktopWindow::GetType()
{
	return Component::DESKTOPWINDOW;
}



LOG_CLASS_DEFINE(UI_GFX_2D, DesktopWindow);



}
