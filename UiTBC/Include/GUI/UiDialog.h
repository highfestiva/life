
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once
#include "UiComponent.h"
#include "UiFloatingLayout.h"



namespace UiTbc
{



template<class _Target>
class Dialog: public Component
{
	typedef Component Parent;
public:
	typedef Button::Delegate Action;

	Dialog(Component* pParent, const str& pText, Action pTarget);
	virtual ~Dialog();
	void AddButton(int pTag, const str& pText);
	void FireAndForget();

	virtual void Repaint(Painter* pPainter);
	virtual void UpdateLayout();

private:
	void OnClick(Button* pButton);

	typedef std::vector<Button*> ButtonList;

	Color mBackground;
	Color mForeground;
	str mText;
	Action mTarget;
	ButtonList mButtonList;
};



#include "UiDialog.inl"



}
