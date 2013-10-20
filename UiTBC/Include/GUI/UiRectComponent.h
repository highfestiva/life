/*
	Class:  RectComponent
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

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
	typedef std::vector<Vector2DF> VertexList;
	
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

	virtual void RepaintBackground(Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	void SetImage(Painter::ImageID pImageID);
	virtual void SetColor(const Color& pColor);
	virtual void SetColor(const Color& pTopLeftColor, const Color& pTopRightColor, const Color& pBottomRightColor, const Color& pBottomLeftColor);

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

	int GetCornerRadius() const;
	void SetCornerRadius(int pRadius);
	void SetCornerRadiusMask(int pMask);
	static void AddRadius(VertexList& pVertexList, int x, int y, int r, float pStartAngle, float pEndAngle);

protected:
	void TryAddRadius(VertexList& pVertexList, int x, int y, int r, float pStartAngle, float pEndAngle, int pMask);
	bool IsHollow();
	bool IsShaded();

	Color mColor[4];

private:
	bool mShaded;
	bool mHollow;
	bool mBehaveSolid;
	int mCornerRadius;
	int mCornerRadiusMask;

	Painter::ImageID mImageID;
};

} // End namespace.

#endif
