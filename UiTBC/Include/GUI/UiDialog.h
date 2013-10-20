
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "UiButton.h"
#include "UiFloatingLayout.h"
#include "UiLabel.h"



namespace UiTbc
{



class Dialog: public RectComponent
{
	typedef RectComponent Parent;
public:
	typedef Button::Delegate Action;

	Dialog(Component* pParent, Action pTarget);
	virtual ~Dialog();
	void Dismiss();

	virtual void SetColor(const Color& pTopLeftColor, const Color& pTopRightColor, const Color& pBottomRightColor, const Color& pBottomLeftColor);
	void SetDirection(int pDirection, bool pSetPos);
	void SetPreClickTarget(Action pPreClickTarget);
	virtual void Center();
	Label* SetQueryLabel(const str& pText, UiTbc::FontManager::FontId pFontId);
	void AddButton(int pTag, const str& pText, bool pAutoDismiss);	// Tag < 0 to layout yourself.
	void AddButton(int pTag, Button* pButton, bool pAutoDismiss);	// Tag < 0 to layout yourself.
	void SetButtonHandler(int pTag, Button* pButton, bool pAutoDismiss);	// Tag < 0 to layout yourself.
	bool IsAutoDismissButton(Button* pButton) const;
	void SetOffset(PixelCoord pOffset);
	virtual void UpdateLayout();

protected:
	virtual void Repaint(Painter* pPainter);
	void Animate();
	void OnDismissClick(Button* pButton);
	void OnClick(Button* pButton);

	typedef std::vector<Button*> ButtonList;

	PixelCoord mOffset;
	Label* mLabel;
	Action mTarget;
	Action mPreClickTarget;
	ButtonList mButtonList;
	Button* mClickedButton;
	bool mIsClosing;
	int mAnimationStep;
	int mDirection;
};



}
