
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiComponent.h"



namespace UiTbc
{



class TextComponent
{
public:
	TextComponent();
	virtual ~TextComponent();

	void SetFontColor(const Color& pTextColor);

protected:
	virtual void ForceRepaint() = 0;

	Color GetTextColor() const;

private:
	Color mTextColor;
};



}
