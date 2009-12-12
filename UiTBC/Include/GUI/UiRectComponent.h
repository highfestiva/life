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
	
	RectComponent(const Lepra::String& pName,
				  Layout* pLayout = 0);
	RectComponent(const Lepra::Color& pColor,
				  const Lepra::String& pName,
				  Layout* pLayout = 0);
	RectComponent(const Lepra::Color& pTopLeftColor,
				  const Lepra::Color& pTopRightColor,
				  const Lepra::Color& pBottomRightColor,
				  const Lepra::Color& pBottomLeftColor,
				  const Lepra::String& pName,
				  Layout* pLayout = 0);
	RectComponent(Painter::ImageID pImageID,
				  const Lepra::String& pName,
				  Layout* pLayout = 0);

	virtual ~RectComponent();

	virtual void Repaint(Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	inline void SetImage(Painter::ImageID pImageID);
	inline void SetColor(const Lepra::Color& pColor);
	inline void SetColor(const Lepra::Color& pTopLeftColor,
			     const Lepra::Color& pTopRightColor,
			     const Lepra::Color& pBottomRightColor,
			     const Lepra::Color& pBottomLeftColor);

	inline Painter::ImageID GetImage();
	inline Lepra::Color GetColor();
	inline void GetColor(Lepra::Color& pTopLeftColor,
			     Lepra::Color& pTopRightColor,
			     Lepra::Color& pBottomRightColor,
			     Lepra::Color& pBottomLeftColor);

	inline virtual Component::Type GetType();

	void SetIsHollow(bool pIsHollow);
	inline void SetBehaveSolid(bool pBehaveSolid);
	inline bool GetBehaveSolid() const;

protected:

	inline bool IsHollow();
	inline bool IsShaded();

private:

	Lepra::Color mColor[4];
	bool mShaded;
	bool mHollow;
	bool mBehaveSolid;

	Painter::ImageID mImageID;
};

void RectComponent::SetImage(Painter::ImageID pImageID)
{
	mImageID = pImageID;
	SetNeedsRepaint(true);
}

void RectComponent::SetColor(const Lepra::Color& pColor)
{
	mShaded = false;
	SetNeedsRepaint(mColor[0] != pColor);
	mColor[0] = pColor;
}

void RectComponent::SetColor(const Lepra::Color& pTopLeftColor,
			     const Lepra::Color& pTopRightColor,
			     const Lepra::Color& pBottomRightColor,
			     const Lepra::Color& pBottomLeftColor)
{
	mShaded = true;
	SetNeedsRepaint(mColor[0] != pTopLeftColor || mColor[1] != pTopRightColor || 
			mColor[2] != pBottomRightColor || mColor[3] != pBottomLeftColor);
	mColor[0] = pTopLeftColor;
	mColor[1] = pTopRightColor;
	mColor[2] = pBottomRightColor;
	mColor[3] = pBottomLeftColor;
}

Painter::ImageID RectComponent::GetImage()
{
	return mImageID;
}

Lepra::Color RectComponent::GetColor()
{
	return mColor[0];
}

void RectComponent::GetColor(Lepra::Color& pTopLeftColor,
			     Lepra::Color& pTopRightColor,
			     Lepra::Color& pBottomRightColor,
			     Lepra::Color& pBottomLeftColor)
{
	pTopLeftColor     = mColor[0];
	pTopRightColor    = mColor[1];
	pBottomRightColor = mColor[2];
	pBottomLeftColor  = mColor[3];
}

Component::Type RectComponent::GetType()
{
	return Component::RECTCOMPONENT;
}

bool RectComponent::IsHollow()
{
	return mHollow;
}

bool RectComponent::IsShaded()
{
	return mShaded;
}

void RectComponent::SetBehaveSolid(bool pBehaveSolid)
{
	mBehaveSolid = pBehaveSolid;
}

bool RectComponent::GetBehaveSolid() const
{
	return mBehaveSolid;
}

} // End namespace.

#endif
