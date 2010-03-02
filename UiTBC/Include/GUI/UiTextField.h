/*
	Class:  TextField
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	This class implements the functionality of a text field with some extra
	features. It also handles optional popup-lists which can be used to select
	one item from a set (and then set the contents of the text field to match 
	that item).


	This class is far from done... TODO:

	1. Implement text selection.
	2. Implement cut, copy & paste.
	3. Implement undo.
	4. Optimize rendering performance. It is far too slow.
*/



#pragma once

#include "../../../Lepra/Include/Timer.h"
#include "UiPopupList.h"
#include "UiTextComponent.h"
#include "UiWindow.h"



namespace UiTbc
{



class DesktopWindow;



class TextField: public Window, public PopupList::Listener, public TextComponent
{
	typedef Window Parent;
public:
	// pTopParent points to the window that contains this textfield.
	// If this pointer is null the popup list feature will be disabled.
	// The popup list will be a child of this parent, in an upper layer.
	TextField(Component* pTopParent, const str& pName);
	TextField(Component* pTopParent, 
		  unsigned pBorderStyle, int pBorderWidth, const Color& pColor,
		  const str& pName);
	TextField(Component* pTopParent, 
		  unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
		  const str& pName);
	TextField(Component* pTopParent, 
		  const Color& pColor, const str& pName);
	TextField(Component* pTopParent, 
		  Painter::ImageID pImageID, const str& pName);
	virtual ~TextField();

	Component* GetTopParent() const;
	void SetTopParent(Component* pTopParent);

	void SetIsReadOnly(bool pIsReadOnly);
	void SetPasswordCharacter(tchar pCharacter);

	str GetVisibleText() const;
	void SetText(const str& pText);
	const str& GetText() const;

	void SetMarker(Painter::ImageID pImageID);
	void SetMarkerBlinkRate(float64 pVisibleTime, float64 pInvisibleTime);
	void SetMarkerPosition(size_t pIndex);
	size_t GetMarkerPosition() const;

	virtual void Repaint(Painter* pPainter);

	virtual bool OnChar(tchar pChar);
	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);
	virtual void OnIdle();

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pMouseDX, int pMouseDY);

	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(RecurseDir pDir = RECURSE_UP, Component* pFocusedComponent = 0);

	// Override these and remove the functionality.
	virtual void SetCaption(Caption* pCaption);
	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0);

	virtual void DoSetPos(int x, int y);
	virtual void DoSetSize(int pWidth, int pHeight);

	// From PopupList::Listener.
	virtual bool NotifySetKeyboardFocus(PopupList* pList);
	virtual bool NotifyReleaseKeyboardFocus(PopupList* pList, Component* pFocusedComponent);
	virtual bool NotifyKeyDown(PopupList* pList, UiLepra::InputManager::KeyCode pKeyCode);
	virtual bool NotifyLButtonDown(PopupList* pList, int pMouseX, int pMouseY);
	virtual bool NotifyDoubleClick(PopupList* pList, int pMouseX, int pMouseY);
protected:
	// Default behaviour is to return a null pointer, in which case
	// the popup list feature is disabled.
	virtual PopupList* CreatePopupList();

	void SpawnPopupList();
	void DestroyPopupList();

	PopupList* GetPopupList() const;

	virtual StateComponentList GetStateList(ComponentState pState);

	void UpdateMarkerPos(Painter* pPainter);
	void SetMarkerPos(size_t pPos);

	virtual void SetKeyboardFocus(Component* pChild);
	void SetupMarkerBlink();

	void ForceRepaint();

private:
	str mText;
	bool mIsReadOnly;
	tchar mPasswordCharacter;
	int mTextX;

	Painter::ImageID mMarkerID;
	size_t mMarkerPos;
	bool mMarkerVisible;
	Timer mMarkerTimer;
	float64 mMarkerVisibleTime;
	float64 mMarkerInvisibleTime;

	bool mUpdateMarkerPosOnNextRepaint;
	int mClickX;

	Component* mTopParent;
	DesktopWindow* mDesktopWindow;

	// The layer in mTopParent where the popup list is.
	int mListLayer;
	PopupList* mListControl;
	bool mDeleteListControl;
};



}
