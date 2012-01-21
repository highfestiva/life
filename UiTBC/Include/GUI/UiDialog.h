
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once
#include "UiLabel.h"
#include "UiFloatingLayout.h"



namespace UiTbc
{



template<class _Target>
class Dialog: public RectComponent
{
	typedef RectComponent Parent;
public:
	typedef Button::Delegate Action;

	Dialog(Component* pParent, Action pTarget);
	virtual ~Dialog();
	void Dismiss();
	void SetDirection(int pDirection, bool pSetPos);
	void SetPreClickTarget(Action pPreClickTarget);
	virtual void Center();
	Label* SetQueryLabel(const str& pText, UiTbc::FontManager::FontId pFontId);
	void AddButton(int pTag, const str& pText);	// Tag < 0 to layout yourself.
	void AddButton(int pTag, Button* pButton);	// Tag < 0 to layout yourself.
	void SetOffset(PixelCoord pOffset);
	virtual void UpdateLayout();

private:
	virtual void Repaint(Painter* pPainter);
	void Animate();
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



#include "UiDialog.inl"



}
