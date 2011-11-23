
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once
#include "../UiFontManager.h"



namespace UiTbc
{



class Painter;



class TextComponent
{
public:
	TextComponent();
	virtual ~TextComponent();

	void SetFontColor(const Color& pTextColor);
	void SetFontId(const FontManager::FontId& pFontId);
	void ActivateFont(Painter* pPainter);
	void DeactivateFont(Painter* pPainter);

protected:
	virtual void ForceRepaint() = 0;

	void DoPrintText(Painter* pPainter, const str& pText, int x, int y);
	void PrintTextDeactivate(Painter* pPainter, const str& pText, int x, int y);
	Color GetTextColor() const;

private:
	Color mTextColor;
	bool mIsFontActive;
	FontManager::FontId mFontId;
};



}
