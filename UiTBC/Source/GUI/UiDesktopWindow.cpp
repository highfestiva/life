
// Author: Alexander Hugestrand
// Copyright (c) 2002-2006, Righteous Games



#include "../../../UiLepra/Include/UiInput.h"
#include "../../../Lepra/Include/Log.h"
#include "../../../Lepra/Include/ListUtil.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../Include/UiMouseTheme.h"
#include "../../Include/UiStandardMouseTheme.h"



namespace UiTbc
{



DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, 
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile,
	const Lepra::tchar* pArchive, RenderMode pRenderMode):
	RectComponent(_T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseTheme(0),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mRenderMode(pRenderMode),
	mUserPainter(pPainter)
{
	mPainter.SetFontPainter(pPainter->GetFontPainter());
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, const Lepra::Color& pColor,
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile, const Lepra::tchar* pArchive,
	RenderMode pRenderMode):
	RectComponent(pColor, _T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseTheme(0),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mRenderMode(pRenderMode),
	mUserPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter,
	const Lepra::Color& pTopLeftColor, const Lepra::Color& pTopRightColor,
	const Lepra::Color& pBottomRightColor, const Lepra::Color& pBottomLeftColor,
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile,
	const Lepra::tchar* pArchive, RenderMode pRenderMode):
	RectComponent(pTopLeftColor, pTopRightColor, pBottomRightColor, pBottomLeftColor, _T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseTheme(0),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mRenderMode(pRenderMode),
	mUserPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::DesktopWindow(UiLepra::InputManager* pInputManager, Painter* pPainter, Painter::ImageID pImageID,
	Layout* pLayout, const Lepra::tchar* pImageDefinitionFile, const Lepra::tchar* pArchive,
	RenderMode pRenderMode):
	RectComponent(pImageID, _T("DesktopWindow"), pLayout),
	mInputManager(pInputManager),
	mMouseEnabled(true),
	mKeyboardEnabled(true),
	mMouseTheme(0),
	mMouseArea(0, 0, 0, 0),
	mMouseX(0),
	mMouseY(0),
	mMousePrevX(0),
	mMousePrevY(0),
	mRenderMode(pRenderMode),
	mUserPainter(pPainter)
{
	Init(pImageDefinitionFile, pArchive);
}

DesktopWindow::~DesktopWindow()
{
	PurgeDeleted();

	Lepra::ListUtil::DeleteAll(mCleanerList);

	if (mMouseTheme != 0)
	{
		delete mMouseTheme;
	}

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

	if (mRenderMode == RM_EVERY_FRAME)
	{
		mImageManager.SetPainter(mUserPainter);
	}
	else
	{
		if (mPainter.GetRGBOrder() != mUserPainter->GetRGBOrder())
		{
			mPainter.SwapRGB();
			mImageManager.SwapRGB();
		}

		mImageManager.SetPainter(&mPainter);
	}

	/* TODO: come up with a way to avoid invoking the image loader from basic TBC.
	if (pImageDefinitionFile != 0)
	{
		mImageManager.LoadImages(pImageDefinitionFile, pArchive);
	}*/

	if (mMouseTheme == 0)
	{
		mMouseTheme = new StandardMouseTheme();
	}

	mMouseTheme->LoadArrowCursor();

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
			mUserPainter->RemoveImage(lChild->mImageID);
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

void DesktopWindow::SetMouseTheme(MouseTheme* pMouseTheme)
{
	if (mMouseTheme != 0)
	{
		delete mMouseTheme;
	}

	mMouseTheme = pMouseTheme;

	if (mMouseTheme != 0)
	{
		mMouseTheme->LoadArrowCursor();
	}
}

void DesktopWindow::SetMouseEnabled(bool pEnabled)
{
	mMouseEnabled = pEnabled;

	if (mMouseEnabled == false)
	{
		mMouseTheme->UnloadCursor();
	}
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

	mMouseTheme->SetPainter(mUserPainter);

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

		Parent::Repaint(mUserPainter);
		lLoopCount++;
	} while (NeedsRepaint() == true && lLoopCount < 4);

	// Finally, render the mouse cursor on top of everything.
	if (mMouseEnabled == true)
	{
		mMouseTheme->DrawCursor(mMouseX, mMouseY);
	}
}

void DesktopWindow::RepaintChild(Component* pChild, Painter* pPainter)
{
	if (mRenderMode == RM_EVERY_FRAME)
	{
		pChild->Repaint(pPainter);
	}
	else // mRenderMode == RM_OPTIMIZE_STATIC
	{
		const Lepra::PixelCoords lPos(pChild->GetPos());

		if (pChild->NeedsRepaint() == true)
		{
			// TODO: Check if pPainter has changed. Create a new image in that case.

			const Lepra::PixelCoords lSize(pChild->GetSize());
			// Check if we need to allocate an image.
			if (pChild->mImageID == Painter::INVALID_IMAGEID)
			{
				// Create the image...
				pChild->mImage.Reset(Lepra::Canvas::PowerUp(lSize.x), 
							Lepra::Canvas::PowerUp(lSize.y), 
							Lepra::Canvas::BITDEPTH_32_BIT);
				pChild->mImage.CreateBuffer();
				pChild->mImage.Clear();
				pChild->mImageID = pPainter->AddImage(&pChild->mImage, 0);
			}
			else
			{
				unsigned int lNewWidth  = (unsigned int)Lepra::Canvas::PowerUp(lSize.x);
				unsigned int lNewHeight = (unsigned int)Lepra::Canvas::PowerUp(lSize.y);

				if (lNewWidth != pChild->mImage.GetWidth() || lNewHeight != pChild->mImage.GetHeight())
				{
					pChild->mImage.Crop(0, 0, lNewWidth, lNewHeight);
				}
			}

			mPainter.DefineCoordinates(-lPos.x, -lPos.y, Painter::X_RIGHT, Painter::Y_DOWN);
			mPainter.SetDestCanvas(&pChild->mImage);	// TRICKY: must be after DefineCoordinates()!
			mPainter.SetIncrementalAlpha(true);
			pChild->mImage.Clear();
			pChild->Repaint(&mPainter);
			pPainter->UpdateImage(pChild->mImageID, &pChild->mImage, 0);

		}

		pPainter->SetRenderMode(Painter::RM_ALPHABLEND);
		pPainter->SetAlphaValue(255);
		pPainter->DrawImage(pChild->mImageID, lPos.x, lPos.y);
	}
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
	return mUserPainter;
}

Painter* DesktopWindow::GetInternalPainter()
{
	Painter* lPainter = &mPainter;

	if (mRenderMode == RM_EVERY_FRAME)
	{
		lPainter = mUserPainter;
	}

	return lPainter;
}



LOG_CLASS_DEFINE(UI_GFX_2D, DesktopWindow);



}
