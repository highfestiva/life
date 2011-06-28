/*
	Class:  RectComponent
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games

	NOTES:

	A simple component that consists of a single rectangle,
	which can have a single color, be shaded, or even have an image.
*/

#ifndef UIRECTCOMPONENT_H
#define UIRECTCOMPONENT_H

#include "UiComponent.h"
#include "../UiPainter.h"
#include "../../../Lepra/Include/Graphics2D.h"

namespace UiTbc
{

class RectComponent : public Component
{
public:
	
	RectComponent(const str& pName,
				  Layout* pLayout = 0);
	RectComponent(const Color& pColor,
				  const str& pName,
				  Layout* pLayout = 0);
	RectComponent(const Color& pTopLeftColor,
				  const Color& pTopRightColor,
				  const Color& pBottomRightColor,
				  const Color& pBottomLeftColor,
				  const str& pName,
				  Layout* pLayout = 0);
	RectComponent(Painter::ImageID pImageID,
				  const str& pName,
				  Layout* pLayout = 0);

	virtual ~RectComponent();

	virtual void Repaint(Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	void SetImage(Painter::ImageID pImageID);
	void SetColor(const Color& pColor);
	void SetColor(const Color& pTopLeftColor,
			     const Color& pTopRightColor,
			     const Color& pBottomRightColor,
			     const Color& pBottomLeftColor);

	Painter::ImageID GetImage();
	Color GetColor();
	void GetColor(Color& pTopLeftColor,
			     Color& pTopRightColor,
			     Color& pBottomRightColor,
			     Color& pBottomLeftColor);

	virtual Type GetType() const;

	void SetIsHollow(bool pIsHollow);
	void SetBehaveSolid(bool pBehaveSolid);
	bool GetBehaveSolid() const;

protected:
	bool IsHollow();
	bool IsShaded();

private:

	Color mColor[4];
	bool mShaded;
	bool mHollow;
	bool mBehaveSolid;

	Painter::ImageID mImageID;
};

} // End namespace.

#endif
