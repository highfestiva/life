
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiRectComponent.h"
#include "UiTextComponent.h"



namespace UiTbc
{



class Label: public RectComponent, public TextComponent
{
	typedef RectComponent Parent;
public:

	Label(const Color& pColor, const str& pText);
	virtual ~Label();

	void SetIcon(Painter::ImageID pIconID, IconAlignment pAlignment);
	void SetText(const str& pText);
	const str& GetText() const;
	virtual void SetSelected(bool pSelected);

	virtual void Repaint(Painter* pPainter);
	virtual void ForceRepaint();

	virtual PixelCoord GetPreferredSize(bool pForceAdaptive);

	virtual Type GetType() const;

protected:
	void UpdateBackground();

	Painter::ImageID mIconId;
	IconAlignment mIconAlignment;

	str mText;

	int mTextWidth;
	int mTextHeight;

	bool mSelectable;
};



}
