/*
	Class:  BorderComponent
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#ifndef UIBORDERCOMPONENT_H
#define UIBORDERCOMPONENT_H

#include "UiRectComponent.h"

namespace UiTbc
{

class BorderComponent: public RectComponent
{
	typedef RectComponent Parent;
public:

	enum BorderPart
	{
		INVALID_PART = 0,

		TOPLEFT_CORNER,
		TOPRIGHT_CORNER,
		BOTTOMRIGHT_CORNER,
		BOTTOMLEFT_CORNER,
		TOP_BORDER,
		BOTTOM_BORDER,
		LEFT_BORDER,
		RIGHT_BORDER,
	};

	enum BorderShadeFunc
	{
		LINEAR = 0,
		ZIGZAG,
	};

	BorderComponent(BorderPart pPart, const Color& pColor, BorderShadeFunc pShadeFunc);
	BorderComponent(BorderPart pPart, Painter::ImageID pImageID);
	virtual ~BorderComponent();

	void SetSunken(bool pSunken);

	inline void SetResizable(bool pResizable);

	void Set(const Color& pColor, BorderShadeFunc pShadeFunc);
	void Set(Painter::ImageID pImageID);

	virtual void Repaint(Painter* pPainter);

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pMouseDX, int pMouseDY);

	inline virtual Type GetType() const;

protected:
private:

	void CalcColors();

	bool mImageBorder;
	bool mResizable;
	bool mResizing;

	BorderPart mPart;
	BorderShadeFunc mShading;

	Color mBodyColor;
	Color mLightColor;
	Color mDarkColor;
};

Component::Type BorderComponent::GetType() const
{
	return Component::BORDER;
}

void BorderComponent::SetResizable(bool pResizable)
{
	mResizable = pResizable;
}

} // End namespace.

#endif
