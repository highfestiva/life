
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once
#include "UiComponent.h"
#include "UiFloatingLayout.h"



namespace UiTbc
{



template<class _Target>
class Dialog: public RectComponent
{
	typedef RectComponent Parent;
public:
	typedef Button::Delegate Action;

	Dialog(Component* pParent, const str& pText, Action pTarget);
	virtual ~Dialog();
	void AddButton(int pTag, const str& pText);

private:
	virtual void UpdateLayout();
	void OnClick(Button* pButton);

	typedef std::vector<Button*> ButtonList;

	str mText;
	Action mTarget;
	ButtonList mButtonList;
};



#include "UiDialog.inl"



}
