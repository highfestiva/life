
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiWindow.h"
#include "../../Include/GUI/UiCaption.h"
#include "../../Include/GUI/UiFloatingLayout.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../../Lepra/Include/Log.h"



namespace UiTbc
{



Window::Window(const str& pName,
			   Layout* pLayout) :
	Component(pName, new GridLayout(1, 1)),
	mTLBorder(0),
	mTRBorder(0),
	mBRBorder(0),
	mBLBorder(0),
	mTBorder(0),
	mBBorder(0),
	mLBorder(0),
	mRBorder(0),
	mCenterComponent(0),
	mClientRect(0),
	mCaption(0),
	mBorder(true),
	mBorderWidth(0),
	mBodyColor(0, 0, 0),
	mBorderStyle(0)
{
	mClientRect = new RectComponent(_T("ClientRect"), pLayout);
	mCenterComponent = new RectComponent(_T("CenterComponent"), new GridLayout(2, 1));
	mCenterComponent->AddChild(mClientRect, 1, 0);
	Init();
}

Window::Window(unsigned pBorderStyle,
			   int pBorderWidth,
			   const Color& pColor,
			   const str& pName,
			   Layout* pLayout) :
	Component(pName, new GridLayout(3, 3)),
	mTLBorder(0),
	mTRBorder(0),
	mBRBorder(0),
	mBLBorder(0),
	mTBorder(0),
	mBBorder(0),
	mLBorder(0),
	mRBorder(0),
	mCenterComponent(0),
	mClientRect(0),
	mCaption(0),
	mBorder(true),
	mBorderWidth(pBorderWidth),
	mBodyColor(pColor),
	mBorderStyle(pBorderStyle)
{
	mClientRect = new RectComponent(pColor, _T("ClientRect"), pLayout);
	//mClientRect->SetPreferredSize(0, 0, true);
	mCenterComponent = new RectComponent(_T("CenterComponent"), new GridLayout(2, 1));
	mCenterComponent->AddChild(mClientRect, 1, 0);
	InitBorder();
}

Window::Window(unsigned pBorderStyle,
			 int pBorderWidth,
			 Painter::ImageID pImageID,
			 const str& pName,
			 Layout* pLayout) :
	Component(pName, new GridLayout(3, 3)),
	mTLBorder(0),
	mTRBorder(0),
	mBRBorder(0),
	mBLBorder(0),
	mTBorder(0),
	mBBorder(0),
	mLBorder(0),
	mRBorder(0),
	mCenterComponent(0),
	mClientRect(0),
	mCaption(0),
	mBorder(true),
	mBorderWidth(pBorderWidth),
	mBodyColor(192, 192, 192),
	mBorderStyle(pBorderStyle)
{
	mClientRect = new RectComponent(pImageID, _T("ClientRect"), pLayout);
	//mClientRect->SetPreferredSize(0, 0, true);
	mCenterComponent = new RectComponent(_T("CenterComponent"), new GridLayout(2, 1));
	mCenterComponent->AddChild(mClientRect, 1, 0);
	InitBorder();
}

Window::Window(const Color& pColor,
			 const str& pName,
			 Layout* pLayout) :
	Component(pName, new GridLayout(1, 1)),
	mTLBorder(0),
	mTRBorder(0),
	mBRBorder(0),
	mBLBorder(0),
	mTBorder(0),
	mBBorder(0),
	mLBorder(0),
	mRBorder(0),
	mCenterComponent(0),
	mClientRect(0),
	mCaption(0),
	mBorder(false),
	mBorderWidth(0),
	mBodyColor(pColor),
	mBorderStyle(0)
{
	mClientRect = new RectComponent(pColor, _T("ClientRect"), pLayout);
	//mClientRect->SetPreferredSize(0, 0, true);
	mCenterComponent = new RectComponent(_T("CenterComponent"), new GridLayout(2, 1));
	mCenterComponent->AddChild(mClientRect, 1, 0);
	InitBorder();
}

Window::Window(Painter::ImageID pImageID,
			 const str& pName,
			 Layout* pLayout) :
	Component(pName, new GridLayout(1, 1)),
	mTLBorder(0),
	mTRBorder(0),
	mBRBorder(0),
	mBLBorder(0),
	mTBorder(0),
	mBBorder(0),
	mLBorder(0),
	mRBorder(0),
	mCenterComponent(0),
	mClientRect(0),
	mCaption(0),
	mBorder(false),
	mBorderWidth(0),
	mBodyColor(192, 192, 192),
	mBorderStyle(0)
{
	mClientRect = new RectComponent(pImageID, _T("ClientRect"), pLayout);
	//mClientRect->SetPreferredSize(0, 0, true);
	mCenterComponent = new RectComponent(_T("CenterComponent"), new GridLayout(2, 1));
	mCenterComponent->AddChild(mClientRect, 1, 0);
	Parent::AddChild(mCenterComponent);
	InitBorder();
}

Window::~Window()
{
	delete mTRBorder;
	delete mBRBorder;
	delete mBLBorder;
	delete mBBorder;
	delete mRBorder;
	delete mTLBorder;
	delete mTBorder;
	delete mLBorder;
}

void Window::Init()
{
	Parent::AddChild(mCenterComponent, 0, 0);

	SetColor(mBodyColor);
	SetNeedsRepaint(true);
}

void Window::InitBorder()
{
	BorderComponent::BorderShadeFunc lFunc = BorderComponent::ZIGZAG;
	if (Check(mBorderStyle, BORDER_LINEARSHADING) == true)
	{
		lFunc = BorderComponent::LINEAR;
	}

	if (Check(mBorderStyle, BORDER_HALF) == false)
	{
		mTRBorder = new BorderComponent(BorderComponent::TOPRIGHT_CORNER,    mBodyColor, lFunc, GetName() + _T(":TRBorder"));
		mBRBorder = new BorderComponent(BorderComponent::BOTTOMRIGHT_CORNER, mBodyColor, lFunc, GetName() + _T(":BRBorder"));
		mBLBorder = new BorderComponent(BorderComponent::BOTTOMLEFT_CORNER,  mBodyColor, lFunc, GetName() + _T(":BLBorder"));
		mBBorder  = new BorderComponent(BorderComponent::BOTTOM_BORDER,      mBodyColor, lFunc, GetName() + _T(":BBorder"));
		mRBorder  = new BorderComponent(BorderComponent::RIGHT_BORDER,       mBodyColor, lFunc, GetName() + _T(":RBorder"));
	}
	mTLBorder = new BorderComponent(BorderComponent::TOPLEFT_CORNER,     mBodyColor, lFunc, GetName() + _T(":TLBorder"));
	mTBorder  = new BorderComponent(BorderComponent::TOP_BORDER,         mBodyColor, lFunc, GetName() + _T(":TBorder"));
	mLBorder  = new BorderComponent(BorderComponent::LEFT_BORDER,        mBodyColor, lFunc, GetName() + _T(":LBorder"));

	if (Check(mBorderStyle, BORDER_SUNKEN) == true)
	{
		if (Check(mBorderStyle, BORDER_HALF) == false)
		{
			mTRBorder->SetSunken(true);
			mBRBorder->SetSunken(true);
			mBLBorder->SetSunken(true);
			mBBorder->SetSunken(true);
			mRBorder->SetSunken(true);
		}
		mTLBorder->SetSunken(true);
		mTBorder->SetSunken(true);
		mLBorder->SetSunken(true);
	}

	if (Check(mBorderStyle, BORDER_HALF) == false)
	{
		mTRBorder->SetMinSize(mBorderWidth, mBorderWidth);
		mBRBorder->SetMinSize(mBorderWidth, mBorderWidth);
		mBLBorder->SetMinSize(mBorderWidth, mBorderWidth);
		mBBorder->SetMinSize(0, mBorderWidth);
		mRBorder->SetMinSize(mBorderWidth, 0);
	}
	mTLBorder->SetMinSize(mBorderWidth, mBorderWidth);
	mTBorder->SetMinSize(0, mBorderWidth);
	mLBorder->SetMinSize(mBorderWidth, 0);

	if (Check(mBorderStyle, BORDER_HALF) == false)
	{
		mTRBorder->SetPreferredSize(mBorderWidth, mBorderWidth);
		mBRBorder->SetPreferredSize(mBorderWidth, mBorderWidth);
		mBLBorder->SetPreferredSize(mBorderWidth, mBorderWidth);
		mBBorder->SetPreferredSize(0, mBorderWidth);
		mRBorder->SetPreferredSize(mBorderWidth, 0);
	}
	mTLBorder->SetPreferredSize(mBorderWidth, mBorderWidth);
	mTBorder->SetPreferredSize(0, mBorderWidth);
	mLBorder->SetPreferredSize(mBorderWidth, 0);

	if (Check(mBorderStyle, BORDER_RESIZABLE) == true)
	{
		if (Check(mBorderStyle, BORDER_HALF) == false)
		{
			mTRBorder->SetResizable(true);
			mBRBorder->SetResizable(true);
			mBLBorder->SetResizable(true);
			mBBorder->SetResizable(true);
			mRBorder->SetResizable(true);
		}
		mTLBorder->SetResizable(true);
		mTBorder->SetResizable(true);
		mLBorder->SetResizable(true);
	}

//	mCenterComponent->SetPreferredSize(0, 0);
	if (Check(mBorderStyle, BORDER_HALF) == false)
	{
		Parent::AddChild(mTRBorder, 0, 2);
		Parent::AddChild(mBRBorder, 2, 2);
		Parent::AddChild(mBLBorder, 2, 0);
		Parent::AddChild(mBBorder, 2, 1);
		Parent::AddChild(mRBorder, 1, 2);
	}

	Parent::AddChild(mTLBorder, 0, 0);
	Parent::AddChild(mTBorder, 0, 1);
	Parent::AddChild(mLBorder, 1, 0);

	Init();
}

void Window::SetBorder(unsigned pBorderStyle, int pWidth)
{
	if (mBorder == false || (pBorderStyle == mBorderStyle && pWidth == mBorderWidth))
	{
		return;
	}

	mBorderStyle = pBorderStyle;

	BorderComponent::BorderShadeFunc lFunc = BorderComponent::ZIGZAG;
	if (Check(mBorderStyle, BORDER_LINEARSHADING) == true)
	{
		lFunc = BorderComponent::LINEAR;
	}

	mBorderWidth = pWidth;

	if (mTLBorder)
	{
		mTLBorder->SetPreferredSize(pWidth, pWidth);
		mTRBorder->SetPreferredSize(pWidth, pWidth);
		mBRBorder->SetPreferredSize(pWidth, pWidth);
		mBLBorder->SetPreferredSize(pWidth, pWidth);
		mTBorder->SetPreferredSize(0, pWidth);
		mBBorder->SetPreferredSize(0, pWidth);
		mLBorder->SetPreferredSize(pWidth, 0);
		mRBorder->SetPreferredSize(pWidth, 0);

		mTLBorder->Set(mBodyColor, lFunc);
		mTRBorder->Set(mBodyColor, lFunc);
		mBRBorder->Set(mBodyColor, lFunc);
		mBLBorder->Set(mBodyColor, lFunc);
		mTBorder->Set(mBodyColor, lFunc);
		mBBorder->Set(mBodyColor, lFunc);
		mLBorder->Set(mBodyColor, lFunc);
		mRBorder->Set(mBodyColor, lFunc);

		if (Check(mBorderStyle, BORDER_SUNKEN) == true)
		{
			mTLBorder->SetSunken(true);
			mTRBorder->SetSunken(true);
			mBRBorder->SetSunken(true);
			mBLBorder->SetSunken(true);
			mTBorder->SetSunken(true);
			mBBorder->SetSunken(true);
			mLBorder->SetSunken(true);
			mRBorder->SetSunken(true);
		}
	}

	SetNeedsRepaint(true);
}

void Window::SetBorder(unsigned pBorderStyle,
		       Painter::ImageID pTopLeftID,
		       Painter::ImageID pTopRightID,
		       Painter::ImageID pBottomLeftID,
		       Painter::ImageID pBottomRightID,
		       Painter::ImageID pTopID,
		       Painter::ImageID pBottomID,
		       Painter::ImageID pLeftID,
		       Painter::ImageID pRightID)
{
	if (mBorder == false)
	{
		return;
	}

	mBorderStyle = pBorderStyle;

	GUIImageManager* lIMan = GetImageManager();

	mTLBorder->SetPreferredSize(lIMan->GetImageSize(pTopLeftID));
	mTRBorder->SetPreferredSize(lIMan->GetImageSize(pTopRightID));
	mBLBorder->SetPreferredSize(lIMan->GetImageSize(pBottomLeftID));
	mBRBorder->SetPreferredSize(lIMan->GetImageSize(pBottomRightID));
	mTBorder->SetPreferredSize(0, lIMan->GetImageSize(pTopID).y);
	mBBorder->SetPreferredSize(0, lIMan->GetImageSize(pBottomID).y);
	mLBorder->SetPreferredSize(lIMan->GetImageSize(pLeftID).x, 0);
	mRBorder->SetPreferredSize(lIMan->GetImageSize(pRightID).x, 0);

	mTLBorder->Set(pTopLeftID);
	mTRBorder->Set(pTopRightID);
	mBRBorder->Set(pBottomRightID);
	mBLBorder->Set(pBottomLeftID);
	mTBorder->Set(pTopID);
	mBBorder->Set(pBottomID);
	mLBorder->Set(pLeftID);
	mRBorder->Set(pRightID);
}

bool Window::IsOver(int pScreenX, int pScreenY)
{
	return GetScreenRect().IsInside(pScreenX, pScreenY);
	/*if (GetScreenRect().IsInside(pScreenX, pScreenY) == true)
	{
		Layout* lLayout = GetLayout();
		if (lLayout != 0)
		{
			Component* lChild = lLayout->GetFirst();
			while (lChild != 0)
			{
				if (lChild->GetScreenRect().IsInside(pScreenX, pScreenY) == true)
				{
					return lChild->IsOver(pScreenX, pScreenY);
				}
				lChild = lLayout->GetNext();
			}
		}
	}
	return false;*/
}

bool Window::OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY)
{
	Parent::OnMouseMove(pMouseX, pMouseY, pDeltaX, pDeltaY);
	return IsOver(pMouseX, pMouseY);
}

void Window::SetCaption(Caption* pCaption)
{
	if (mCaption != 0)
	{
		mCenterComponent->RemoveChild(mCaption, 0);
	}

	mCaption = pCaption;

	if (mCaption != 0)
	{
		mCenterComponent->AddChild(mCaption, 0, 0);
	}
}

PixelRect Window::GetClientRect() const
{
	return mCenterComponent->GetScreenRect();
}

RectComponent* Window::GetClientRectComponent() const
{
	return mClientRect;
}

void Window::AddChild(Component* pChild, int pParam1, int pParam2)
{
	mClientRect->AddChild(pChild, pParam1, pParam2);
}

void Window::RemoveChild(Component* pChild, int pLayer)
{
	mClientRect->RemoveChild(pChild, pLayer);
}

Component* Window::GetChild(const str& pName, int pLayer)
{
	return mClientRect->GetChild(pName, pLayer);
}

int Window::GetNumChildren() const
{
	return mClientRect->GetNumChildren();
}

void Window::SetActive(bool pActive)
{
	if (IsVisible())
	{
		if (pActive)
		{
			if (mCaption != 0)
			{
				mCaption->SetActive(true);
			}

			Component* lParent = Parent::GetParent();

			if (lParent != 0)
			{
				Layout* lParentLayout = lParent->GetLayout();

				if (lParentLayout->GetType() == Layout::FLOATINGLAYOUT)
				{
					((FloatingLayout*)lParentLayout)->MoveToTop(this);
				}
			}
		}
		else
		{
			if (mCaption != 0)
			{
				if (HasKeyboardFocus() ==  true)
				{
					GetTopParent()->ReleaseKeyboardFocus(RECURSE_DOWN);
				}

				mCaption->SetActive(false);
			}
		}
	}
}

bool Window::OnChar(tchar pChar)
{
	bool lOk;
	if (pChar == _T('\t'))
	{
		// Focus next component. May wrap.
		const DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(Component::DESKTOPWINDOW);
		UiLepra::InputManager* lInputManager = lDesktopWindow->GetInputManager();
		bool lFocusPrevious = false;
		bool lFocusNext = false;
		StateComponentList lComponentList = GetStateList(STATE_FOCUSABLE);
		StateComponentList::iterator x = lComponentList.begin();
		for (; x != lComponentList.end(); ++x)
		{
			if (lFocusNext)
			{
				break;
			}
			if (x->first == 2)
			{
				return (Parent::OnChar(pChar));	// RAII simplifies.
			}
			else if (x->first)
			{
				if (lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_LSHIFT) ||
					lInputManager->ReadKey(UiLepra::InputManager::IN_KBD_RSHIFT))
				{
					lFocusPrevious = true;
					break;
				}
				lFocusNext = true;
			}
		}
		if (lFocusPrevious)
		{
			if (x == lComponentList.begin())
			{
				x = lComponentList.end();
			}
			--x;
		}
		else if (!lFocusNext || x == lComponentList.end())
		{
			x = lComponentList.begin();
		}
		if (x != lComponentList.end())	// Any available focusable children?
		{
			x->second->SetKeyboardFocus();
		}
		lOk = true;
	}
	else if (pChar == _T('\r'))
	{
		StateComponentList lComponentList = mClientRect->GetStateList(STATE_CLICKABLE);
		StateComponentList::iterator x = lComponentList.begin();
		if (x != lComponentList.end())
		{
			((Button*)x->second)->Click(true);
		}
		else
		{
			return (Parent::OnChar(pChar));	// RAII simplifies.
		}
		lOk = true;
	}
	else
	{
		lOk = Parent::OnChar(pChar);
	}
	return (lOk);
}

bool Window::OnLButtonDown(int pMouseX, int pMouseY)
{
	bool lReturn = Parent::OnLButtonDown(pMouseX, pMouseY);
	SetActive(true);
	return lReturn;
}

bool Window::OnRButtonDown(int pMouseX, int pMouseY)
{
	bool lReturn = Parent::OnRButtonDown(pMouseX, pMouseY);
	SetActive(true);
	return lReturn;
}

bool Window::OnMButtonDown(int pMouseX, int pMouseY)
{
	bool lReturn = Parent::OnMButtonDown(pMouseX, pMouseY);
	SetActive(true);
	return lReturn;
}



void Window::DoSetSize(int pWidth, int pHeight)
{
	Parent::DoSetSize(pWidth, pHeight);
	mCenterComponent->SetSize(pWidth, pHeight);
	mCenterComponent->UpdateLayout();
}



bool Window::Check(unsigned pFlags, unsigned pFlag)
{
	return ((pFlags & pFlag) != 0);
}

Caption* Window::GetCaption()
{
	return mCaption;
}

void Window::SetBackgroundImage(Painter::ImageID pImageID)
{
	SetNeedsRepaint(pImageID != mClientRect->GetImage());
	mClientRect->SetImage(pImageID);
}

void Window::SetBaseColor(const Color& pColor)
{
	mBodyColor = pColor;
	SetColor(pColor);
}

void Window::SetColor(const Color& pColor)
{
	SetNeedsRepaint(pColor != mClientRect->GetColor());
	mClientRect->SetColor(pColor);

	if (mTLBorder)
	{
		BorderComponent::BorderShadeFunc lFunc =
			Check(mBorderStyle, BORDER_LINEARSHADING)? BorderComponent::LINEAR : BorderComponent::ZIGZAG;
		mTLBorder->Set(pColor, lFunc);
		mTRBorder->Set(pColor, lFunc);
		mBRBorder->Set(pColor, lFunc);
		mBLBorder->Set(pColor, lFunc);
		mTBorder->Set(pColor, lFunc);
		mBBorder->Set(pColor, lFunc);
		mLBorder->Set(pColor, lFunc);
		mRBorder->Set(pColor, lFunc);
	}
}

Painter::ImageID Window::GetBackgroundImage()
{
	return mClientRect->GetImage();
}

const Color& Window::GetColor()
{
	return mBodyColor;
}

unsigned Window::GetBorderStyle()
{
	return mBorderStyle;
}

int Window::GetBorderWidth()
{
	return mBorderWidth;
}

int Window::GetTotalBorderWidth()
{
	if (Check(mBorderStyle, BORDER_HALF) == true)
		return mBorderWidth;
	else
		return mBorderWidth * 2;
}

void Window::SetRoundedStyle(int pRadius)
{
	GetClientRectComponent()->SetCornerRadius(pRadius);
	GetClientRectComponent()->SetIsHollow(false);
}

Component::Type Window::GetType() const
{
	return Component::WINDOW;
}



}
