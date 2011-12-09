
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
	virtual void Center();
	Label* QueryLabel(const str& pText, UiTbc::FontManager::FontId pFontId);
	void AddButton(int pTag, const str& pText);
	void AddButton(int pTag, Button* pButton);
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
	ButtonList mButtonList;
	Button* mClickedButton;
	bool mIsClosing;
	int mAnimationStep;
};



#include "UiDialog.inl"



}
