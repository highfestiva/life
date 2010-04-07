
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiComponent.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include "../../../Lepra/Include/Log.h"



namespace UiTbc
{



Component::Component(const str& pName, Layout* pLayout) :
	mName(pName),
	mParent(0),
	mMouseFocusChild(0),
	mKeyboardFocusChild(0),
	mLayout(0),
	mPos(0, 0),
	mPreferredSize(0, 0),
	mSize(0, 0),
	mNeedsRepaint(true),
	mVisible(true),
	mAdaptivePreferredSize(false),
	mSelected(false),
	mImageID(Painter::INVALID_IMAGEID),
	mLayerCount(0)
{
	CreateLayer(pLayout);
}

Component::~Component()
{
	ReleaseKeyboardFocus();
	ReleaseMouseFocus();
	mKeyboardFocusChild = 0;
	mMouseFocusChild = 0;

	for (int i = 0; i < mLayerCount; i++)
	{
		DeleteLayout(i);
	}
	delete[] mLayout;
	mLayout = 0;
}

void Component::DeleteLayout(int pLayer)
{
	if (pLayer >= 0 && pLayer < mLayerCount)
	{
		if (mLayout[pLayer] != 0)
		{
			Component* lChild = mLayout[pLayer]->GetFirst();
			while (lChild != 0)
			{
				delete lChild;
				lChild = mLayout[pLayer]->GetNext();
			}

			delete mLayout[pLayer];
			mLayout[pLayer] = 0;
		}
	}
}

int Component::CreateLayer(Layout* pLayout)
{
	Layout** lLayout = new Layout*[mLayerCount + 1];
	for (int i = 0; i < mLayerCount; i++)
	{
		lLayout[i] = mLayout[i];
	}
	lLayout[mLayerCount] = pLayout;
	if (pLayout)
	{
		pLayout->SetOwner(this);
	}

	if (mLayout != 0)
	{
		delete[] mLayout;
	}

	mLayout = lLayout;
	++mLayerCount;

	return (mLayerCount - 1);
}

void Component::DeleteLayer(int pLayer)
{
	if (pLayer >= 0 && pLayer < mLayerCount)
	{
		DeleteLayout(pLayer);

		if (mLayerCount > 1)
		{
			Layout** lLayout = new Layout*[mLayerCount - 1];

			int i;
			for (i = 0; i < pLayer; i++)
			{
				lLayout[i] = mLayout[i];
			}

			int j;
			for (j = i + 1; j < mLayerCount; i++, j++)
			{
				lLayout[i] = mLayout[j];
			}

			delete[] mLayout;
			mLayout = lLayout;
		}
		else // if (mLayerCount == 1)
		{
			delete[] mLayout;
			mLayout = 0;
		}

		mLayerCount--;
	}
}

void Component::ReplaceLayer(int pLayer, Layout* pLayout)
{
	if (pLayer >= 0 && pLayer < mLayerCount)
	{
		delete (mLayout[pLayer]);
		mLayout[pLayer] = pLayout;
		pLayout->SetOwner(this);
	}
}

PixelCoord Component::GetPreferredSize(bool pForceAdaptive)
{
	PixelCoord lSize(mPreferredSize);
	
	if ((pForceAdaptive == true || mAdaptivePreferredSize == true) && mLayout[0] != 0 && 
	   (lSize.x == 0 || lSize.y == 0))
	{
		PixelCoord lTemp(mLayout[0]->GetPreferredSize(pForceAdaptive));

		if (lSize.x == 0)
		{
			lSize.x = lTemp.x;
		}
		if (lSize.y == 0)
		{
			lSize.y = lTemp.y;
		}
	}

	return lSize;
}

PixelCoord Component::GetMinSize() const
{
	PixelCoord lSize(mMinSize);
	
	if (mLayout[0] != 0 && (lSize.x == 0 || lSize.y == 0))
	{
		PixelCoord lTemp(mLayout[0]->GetMinSize());

		if (lSize.x == 0)
		{
			lSize.x = lTemp.x;
		}
		if (lSize.y == 0)
		{
			lSize.y = lTemp.y;
		}
	}

	return lSize;
}

void Component::AddChild(Component* pChild, int pParam1, int pParam2, int pLayer)
{
	if (pLayer >= 0 && pLayer < mLayerCount && mLayout[pLayer] != 0)
	{
		pChild->SetParent(this);
		mLayout[pLayer]->Add(pChild, pParam1, pParam2);
	}
	else
	{
		mLog.Errorf(_T("Could not add child to layer %i."), pLayer);
	}
}

void Component::RemoveChild(Component* pChild, int pLayer)
{
	if (pLayer >= 0 && pLayer < mLayerCount && mLayout[pLayer] != 0)
	{
		mLayout[pLayer]->Remove(pChild);
		if (pChild == mMouseFocusChild)
		{
			pChild->ReleaseMouseFocus();
		}
		if (pChild == mKeyboardFocusChild)
		{
			pChild->ReleaseKeyboardFocus();
		}
		pChild->SetParent(0);
	}
}

Component* Component::GetChild(const str& pName, int pLayer)
{
	if (pLayer >= 0 && pLayer < mLayerCount && mLayout[pLayer] != 0)
	{
		return mLayout[pLayer]->Find(pName);
	}

	return 0;
}

int Component::GetNumChildren() const
{
	int lNumChildren = 0;

	for (int i = 0; i < mLayerCount; i++)
	{
		if (mLayout[i] != 0)
		{
			lNumChildren = mLayout[i]->GetNumComponents();
		}
	}

	return lNumChildren;
}

void Component::AddTextListener(UiLepra::TextInputObserver* pListener)
{
	mTextListenerSet.insert(pListener);
}

void Component::RemoveTextListener(UiLepra::TextInputObserver* pListener)
{
	mTextListenerSet.erase(pListener);
}

void Component::AddKeyListener(UiLepra::KeyCodeInputObserver* pListener)
{
	mKeyListenerSet.insert(pListener);
}

void Component::RemoveKeyListener(UiLepra::KeyCodeInputObserver* pListener)
{
	mKeyListenerSet.erase(pListener);
}

Component* Component::GetChild(int pScreenX, int pScreenY, int pLevelsDown)
{
	int i;
	for (i = mLayerCount - 1; i >= 0; i--)
	{
		if (mLayout[i] != 0)
		{
			Component* lChild = mLayout[i]->GetLast();

			while (lChild != 0)
			{
				if (lChild->IsVisible() == true)
				{
					if (lChild->IsOver(pScreenX, pScreenY) == true)
					{
						if (pLevelsDown <= 0)
						{
							return lChild;
						}
						else
						{
							return lChild->GetChild(pScreenX, pScreenY, pLevelsDown - 1);
						}
					}
				}

				lChild = mLayout[i]->GetPrev();
			}
		}
	}

	return 0;
}

void Component::UpdateLayout()
{
	for (int i = 0; i < mLayerCount; i++)
	{
		if (mLayout[i] != 0)
		{
			mLayout[i]->UpdateLayout();
			Component* lChild = mLayout[i]->GetFirst();

			while (lChild != 0)
			{
				if (lChild->IsVisible() == true &&
				    lChild->NeedsRepaint() == true)
				{
					lChild->UpdateLayout();
				}

				lChild = mLayout[i]->GetNext();
			}
		}
	}
}

void Component::Repaint(Painter* pPainter)
{
	mNeedsRepaint = false;

	for (int i = 0; i < mLayerCount; i++)
	{
		if (mLayout[i] != 0)
		{
			Component* lChild = mLayout[i]->GetFirst();

			while (lChild != 0)
			{
				if (lChild->IsVisible() == true)
				{
					RepaintChild(lChild, pPainter);
				}

				lChild = mLayout[i]->GetNext();
			}
		}
	}
}

bool Component::OnDoubleClick(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnDoubleClick(pMouseX, pMouseY);
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnDoubleClick(pMouseX, pMouseY);
		}
	}
	return (false);
}

bool Component::OnLButtonDown(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnLButtonDown(pMouseX, pMouseY);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnLButtonDown(pMouseX, pMouseY);
			return true;
		}

		return false;
	}
}

bool Component::OnRButtonDown(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnRButtonDown(pMouseX, pMouseY);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnRButtonDown(pMouseX, pMouseY);
			return true;
		}

		return false;
	}
}

bool Component::OnMButtonDown(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnMButtonDown(pMouseX, pMouseY);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnMButtonDown(pMouseX, pMouseY);
			return true;
		}

		return false;
	}
}

bool Component::OnLButtonUp(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnLButtonUp(pMouseX, pMouseY);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnLButtonUp(pMouseX, pMouseY);
			return true;
		}

		return false;
	}
}

bool Component::OnRButtonUp(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnRButtonUp(pMouseX, pMouseY);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnRButtonUp(pMouseX, pMouseY);
			return true;
		}

		return false;
	}
}

bool Component::OnMButtonUp(int pMouseX, int pMouseY)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnMButtonUp(pMouseX, pMouseY);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnMButtonUp(pMouseX, pMouseY);
			return true;
		}

		return false;
	}
}

bool Component::OnMouseWheel(int pMouseX, int pMouseY, int pChange, bool pDown)
{
	if (mMouseFocusChild != 0)
	{
		mMouseFocusChild->OnMouseWheel(pMouseX, pMouseY, pChange, pDown);
		return true;
	}
	else
	{
		Component* lChild = GetChild(pMouseX, pMouseY);

		if (lChild != 0)
		{
			lChild->OnMouseWheel(pMouseX, pMouseY, pChange, pDown);
			return true;
		}

		return false;
	}
}

bool Component::OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY)
{
	for (int i = 0; i < mLayerCount; i++)
	{
		if (mLayout[i] != 0)
		{
			Component* lChild = mLayout[i]->GetFirst();
			for (; lChild; lChild = mLayout[i]->GetNext())
			{
				lChild->OnMouseMove(pMouseX, pMouseY, pDeltaX, pDeltaY);
			}
		}
	}
	return (true);
}

bool Component::OnChar(tchar pChar)
{
	DispatchChar(pChar);

	if (mKeyboardFocusChild != 0)
	{
		mKeyboardFocusChild->OnChar(pChar);
	}
	return (false);
}

bool Component::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	KeyListenerSet::iterator x = mKeyListenerSet.begin();
	for (; x != mKeyListenerSet.end(); ++x)
	{
		(*x)->OnKeyDown(pKeyCode);
	}

	if (mKeyboardFocusChild != 0)
	{
		mKeyboardFocusChild->OnKeyDown(pKeyCode);
	}
	return (false);
}

bool Component::OnKeyUp(UiLepra::InputManager::KeyCode pKeyCode)
{
	KeyListenerSet::iterator x = mKeyListenerSet.begin();
	for (; x != mKeyListenerSet.end(); ++x)
	{
		(*x)->OnKeyUp(pKeyCode);
	}

	if (mKeyboardFocusChild != 0)
	{
		mKeyboardFocusChild->OnKeyUp(pKeyCode);
	}
	return (false);
}

bool Component::OnDoubleClick()
{
	return (false);
}

void Component::OnConnectedToDesktopWindow()
{
	// Notify children.
	for(int i = 0; i < mLayerCount; i++)
	{
		Layout* lLayout = mLayout[i];
		if(lLayout != 0)
		{
			Component* lChild = lLayout->GetFirst();
			while(lChild != 0)
			{
				lChild->OnConnectedToDesktopWindow();
				lChild = lLayout->GetNext();
			}
		}
	}
}

Component* Component::GetParentOfType(Type pType)
{
	Component* lParent = this;

	while (lParent != 0)
	{
		if (lParent->GetType() == pType)
		{
			return lParent;
		}

		lParent = lParent->GetParent();
	}

	return 0;
}

Component* Component::GetTopParent()
{
	Component* lTopParent = this;
	Component* lParent = this;

	while (lParent != 0)
	{
		lTopParent = lParent;
		lParent = lParent->GetParent();
	}

	return lTopParent;
}

bool Component::IsChildOf(Component* pParent)
{
	Component* lParent = GetParent();

	while(lParent != 0 && lParent != pParent)
	{
		lParent = lParent->GetParent();
	}

	return (lParent != 0);
}

bool Component::IsOver(int pScreenX, int pScreenY)
{
	PixelCoord lPos(GetScreenPos());
	PixelRect lRect(lPos, lPos + GetSize());
	return lRect.IsInside(pScreenX, pScreenY);
}

Component::StateComponentList Component::GetStateList(ComponentState pState)
{
	StateComponentList lList;
	for (int i = 0; i < mLayerCount; i++)
	{
		if (mLayout[i] != 0)
		{
			Component* c = mLayout[i]->GetFirst();
			for (; c; c = mLayout[i]->GetNext())
			{
				StateComponentList lChildList = c->GetStateList(pState);
				lList.splice(lList.end(), lChildList);
			}
		}
	}
	return (lList);
}

void Component::RepaintChild(Component* pChild, Painter* pPainter)
{
	// Simply let the child repaint itself.
	pChild->Repaint(pPainter);
}

void Component::SetPos(int x, int y)
{
	if (x != mPos.x || y != mPos.y)
	{
		DoSetPos(x, y);
	}
}

void Component::DoSetPos(int x, int y)
{
	// Can't repaint just because of a position change. That will spoil 
	// the rendering optimization in DesktopWindow.
	// SetNeedsRepaint(true);
	if (GetParent())
	{
		if (GetParent()->GetType() == Component::DESKTOPWINDOW)
		{
			((DesktopWindow*)GetParent())->SetUpdateLayout(true);
		}
	}
	else if (GetType() == Component::DESKTOPWINDOW)
	{
		((DesktopWindow*)this)->SetUpdateLayout(true);
	}

	mPos.x = x;
	mPos.y = y;
}

void Component::SetSize(int pWidth, int pHeight)
{
	if (mSize.x != pWidth || mSize.y != pHeight)
	{
		DoSetSize(pWidth, pHeight);
	}
}

void Component::DoSetSize(int pWidth, int pHeight)
{
	SetNeedsRepaint(true);
	mSize.x = pWidth;
	mSize.y = pHeight;
}

void Component::SetMinSize(int pWidth, int pHeight)
{
	DoSetMinSize(pWidth, pHeight);
}

void Component::DoSetMinSize(int pWidth, int pHeight)
{
	mMinSize.x = pWidth  < 0 ? 0 : pWidth;
	mMinSize.y = pHeight < 0 ? 0 : pHeight;
}

void Component::SetPreferredSize(const PixelCoord& pSize, bool pAdaptive)
{
	if (mPreferredSize != pSize)
	{
		SetNeedsRepaint(true);
	}

	mPreferredSize = pSize;

	if (mPreferredSize.x != 0 && mPreferredSize.x < mMinSize.x)
	{
		mPreferredSize.x = mMinSize.x;
	}

	if (mPreferredSize.y != 0 && mPreferredSize.y < mMinSize.y)
	{
		mPreferredSize.y = mMinSize.y;
	}

	if (mParent == 0)
	{
		SetSize(mPreferredSize);
	}


	mAdaptivePreferredSize = pAdaptive;
}

void Component::SetPreferredSize(int pWidth, int pHeight, bool pAdaptive)
{
	PixelCoord lSize(pWidth, pHeight);
	SetPreferredSize(lSize, pAdaptive);
}

void Component::SetSelected(bool pSelected)
{
	mSelected = pSelected;

	if (mSelected)
	{
		SetKeyboardFocus();
	}
}

void Component::SetMouseFocus()
{
	if (!HasMouseFocus())
	{
		GetTopParent()->ReleaseMouseFocus(RECURSE_DOWN, this);

		if (mParent != 0)
		{
			mParent->SetMouseFocus(this);
		}
	}
}

void Component::SetMouseFocus(Component* pChild)
{
	mMouseFocusChild = pChild;
	if (mParent != 0)
	{
		mParent->SetMouseFocus(this);
	}
}

void Component::DispatchChar(tchar pChar)
{
	TextListenerSet::iterator x = mTextListenerSet.begin();
	for (; x != mTextListenerSet.end(); ++x)
	{
		(*x)->OnChar(pChar);
	}
}

bool Component::IsDispatcher() const
{
	return (!mTextListenerSet.empty());
}

void Component::ReleaseMouseFocus(RecurseDir pDir, Component* pFocusedComponent)
{
	if (pDir == RECURSE_UP)
	{
		if (mParent != 0 && mParent->mMouseFocusChild == this)
		{
			mParent->mMouseFocusChild = 0;
			mParent->ReleaseMouseFocus(pDir, pFocusedComponent);
		}
	}
	else
	{
		if (mMouseFocusChild != 0)
		{
			mMouseFocusChild->ReleaseMouseFocus(pDir, pFocusedComponent);
			mMouseFocusChild = 0;
		}
	}
}

void Component::SetKeyboardFocus()
{
	if (!HasKeyboardFocus())
	{
		GetTopParent()->ReleaseKeyboardFocus(RECURSE_DOWN, this);

		if (mParent != 0)
		{
			mParent->SetKeyboardFocus(this);
		}
	}
}

void Component::SetKeyboardFocus(Component* pChild)
{
	mKeyboardFocusChild = pChild;
	if (mParent != 0)
	{
		mParent->SetKeyboardFocus(this);
	}
}

void Component::ReleaseKeyboardFocus(RecurseDir pDir, Component* pFocusedComponent)
{
	if (pDir == RECURSE_UP)
	{
		if (mParent != 0 && mParent->mKeyboardFocusChild == this)
		{
			mParent->mKeyboardFocusChild = 0;
			mParent->ReleaseKeyboardFocus(pDir, pFocusedComponent);
		}
	}
	else
	{
		if (mKeyboardFocusChild != 0)
		{
			mKeyboardFocusChild->ReleaseKeyboardFocus(pDir, pFocusedComponent);
			mKeyboardFocusChild = 0;
		}
	}
}

const str& Component::GetName()
{
	return mName;
}

Component* Component::GetParent()
{
	return mParent;
}

void Component::SetParent(Component* pParent)
{
/*	if (mParent != 0)
	{
		mParent->RemoveChild(this);
	}
*/
	mParent = pParent;

	// OnNewTopParentConnected();
}

const PixelCoord& Component::GetPos() const
{
	return mPos;
}

PixelCoord Component::GetScreenPos() const
{
	PixelCoord lPos(mPos);

	if (mParent != 0)
	{
		lPos += mParent->GetScreenPos();
	}

	return lPos;
}

void Component::SetPos(const PixelCoord& pPos)
{
	SetPos(pPos.x, pPos.y);
}

void Component::SetSize(const PixelCoord& pSize)
{
	SetSize(pSize.x, pSize.y);
}

void Component::SetMinSize(const PixelCoord& pSize)
{
	SetMinSize(pSize.x, pSize.y);
}

const PixelCoord& Component::GetSize() const
{
	return mSize;
}

PixelRect Component::GetScreenRect() const
{
	PixelCoord lPos(GetScreenPos());
	return PixelRect(lPos, lPos + GetSize());
}


void Component::SetPreferredWidth(int pWidth)
{
	SetPreferredSize(pWidth, mPreferredSize.y, mAdaptivePreferredSize);
}

void Component::SetPreferredHeight(int pHeight)
{
	SetPreferredSize(mPreferredSize.x, pHeight, mAdaptivePreferredSize);
}

int Component::GetPreferredWidth(bool pForceAdaptive)
{
	return GetPreferredSize(pForceAdaptive).x;
}

int Component::GetPreferredHeight(bool pForceAdaptive)
{
	return GetPreferredSize(pForceAdaptive).y;
}

void Component::SetAdaptive(bool pAdaptive)
{
	mAdaptivePreferredSize = pAdaptive;
}

bool Component::IsAdaptive()
{
	return mAdaptivePreferredSize;
}

void Component::SetVisible(bool pVisible)
{
	if (mVisible == false && pVisible == true)
	{
		SetNeedsRepaint(true);
	}

	mVisible = pVisible;
}

bool Component::IsVisible()
{
	bool lParentVisible = mVisible;
	if (mParent && mVisible)
	{
		lParentVisible = mParent->IsVisible();
	}
	return (lParentVisible);
}

PixelCoord Component::ClientToWindow(const PixelCoord& pCoords)
{
	return pCoords + mPos;
}

PixelCoord Component::WindowToClient(const PixelCoord& pCoords)
{
	return pCoords - mPos;
}

PixelCoord Component::WindowToScreen(const PixelCoord& pCoords)
{
	return pCoords + GetScreenPos();
}

PixelCoord Component::ScreenToWindow(const PixelCoord& pCoords)
{
	return pCoords - GetScreenPos();
}

PixelCoord Component::ClientToScreen(const PixelCoord& pCoords)
{
	return WindowToScreen(ClientToWindow(pCoords));
}

PixelCoord Component::ScreenToClient(const PixelCoord& pCoords)
{
	return WindowToClient(ScreenToWindow(pCoords));
}

PixelRect Component::ClientToWindow(const PixelRect& pRect)
{
	PixelRect lRect(pRect);
	lRect.Offset(mPos.x, mPos.y);
	return lRect;
}

PixelRect Component::WindowToClient(const PixelRect& pRect)
{
	PixelRect lRect(pRect);
	lRect.Offset(-mPos.x, -mPos.y);
	return lRect;
}

PixelRect Component::WindowToScreen(const PixelRect& pRect)
{
	PixelRect lRect(pRect);
	PixelCoord lPos(GetScreenPos());
	lRect.Offset(lPos.x, lPos.y);
	return lRect;
}

PixelRect Component::ScreenToWindow(const PixelRect& pRect)
{
	PixelRect lRect(pRect);
	PixelCoord lPos(GetScreenPos());
	lRect.Offset(-lPos.x, -lPos.y);
	return lRect;
}

PixelRect Component::ClientToScreen(const PixelRect& pRect)
{
	return WindowToScreen(ClientToWindow(pRect));
}

PixelRect Component::ScreenToClient(const PixelRect& pRect)
{
	return WindowToClient(ScreenToWindow(pRect));
}

void Component::RequestRepaint()
{
	if (IsVisible())
	{
		SetNeedsRepaint(true);
	}
}

bool Component::NeedsRepaint()
{
	return mNeedsRepaint;
}

void Component::SetNeedsRepaint(bool pNeedsRepaint)
{
	if (mParent != 0 && 
	   mNeedsRepaint == false && 
	   pNeedsRepaint == true)
	{
		mParent->SetNeedsRepaint(true);
	}

	mNeedsRepaint = pNeedsRepaint;
}

bool Component::GetSelected() const
{
	return mSelected;
}

bool Component::HasMouseFocus() const
{
	return (mParent->mMouseFocusChild == this);
}

bool Component::HasKeyboardFocus() const
{
	bool lFocused = (mParent == 0 || mParent->mKeyboardFocusChild == this);
	if (lFocused && mParent)
	{
		lFocused = mParent->HasKeyboardFocus();
	}
	return (lFocused);
}

Layout* Component::GetLayout(int pLayer) const
{
	Layout* lLayout = 0;
	if (pLayer >= 0 && pLayer < mLayerCount)
	{
		lLayout = mLayout[pLayer];
	}
	return lLayout;
}

Component::Type Component::GetType() const
{
	return COMPONENT;
}

GUIImageManager* Component::GetImageManager()
{
	return smImageManager;
}

void Component::SetImageManager(GUIImageManager* pImageManager)
{
	smImageManager = pImageManager;
}



GUIImageManager* Component::smImageManager = 0;

LOG_CLASS_DEFINE(UI_GFX_2D, Component);



}
