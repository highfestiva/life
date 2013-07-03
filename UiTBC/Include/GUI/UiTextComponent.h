
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "../UiFontManager.h"



namespace UiTbc
{



class Painter;



class TextComponent
{
public:
	enum VAlign
	{
		VALIGN_TOP,
		VALIGN_CENTER,
		VALIGN_BOTTOM,
	};

	TextComponent();
	virtual ~TextComponent();

	void SetFontColor(const Color& pTextColor);
	void SetFontId(const FontManager::FontId& pFontId);
	void ActivateFont(Painter* pPainter);
	void DeactivateFont(Painter* pPainter);
	void SetVericalAlignment(VAlign pAlignment);

protected:
	virtual void ForceRepaint() = 0;

	void DoPrintText(Painter* pPainter, const str& pText, int x, int y);
	void PrintTextDeactivate(Painter* pPainter, const str& pText, int x, int y);
	Color GetTextColor() const;
	VAlign GetVAlign() const;

private:
	Color mTextColor;
	bool mIsFontActive;
	FontManager::FontId mFontId;
	VAlign mVAlignment;
};



}
