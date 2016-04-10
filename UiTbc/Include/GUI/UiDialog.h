
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
	void SetPostClickTarget(Action pPostClickTarget);
	virtual void Center();
	Label* SetQueryLabel(const wstr& pText, UiTbc::FontManager::FontId pFontId);
	void SetQueryLabel(Label* pLabel);
	void UpdateQueryLabel(const wstr& pText, const Color& pColor);
	void AddButton(int pTag, const wstr& pText, bool pAutoDismiss);	// Tag < 0 to layout yourself.
	void AddButton(int pTag, Button* pButton, bool pAutoDismiss);	// Tag < 0 to layout yourself.
	void SetButtonHandler(int pTag, Button* pButton, bool pAutoDismiss);	// Tag < 0 to layout yourself.
	bool IsAutoDismissButton(Button* pButton) const;
	void SetOffset(PixelCoord pOffset);
	virtual void UpdateLayout();

	virtual void Repaint(Painter* pPainter);
	void Animate();
	void OnDismissClick(Button* pButton);
	void OnClick(Button* pButton);
	void DoClick(Button* pButton);

protected:
	typedef std::vector<Button*> ButtonList;

	PixelCoord mOffset;
	Label* mLabel;
	Action mTarget;
	Action mPreClickTarget;
	Action mPostClickTarget;
	ButtonList mButtonList;
	Button* mClickedButton;
	bool mIsClosing;
	int mAnimationStep;
	int mDirection;
};



}
