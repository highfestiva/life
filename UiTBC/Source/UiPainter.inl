/*
	Class:  Painter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

int Painter::GetOrigoX()
{
	return mOrigoX;
}

int Painter::GetOrigoY()
{
	return mOrigoY;
}

Painter::XDir Painter::GetXDir()
{
	return mXDir;
}

Painter::YDir Painter::GetYDir()
{
	return mYDir;
}

Lepra::Canvas* Painter::GetCanvas()
{
	return mCanvas;
}

Painter::RenderMode Painter::GetRenderMode()
{
	return mRenderMode;
}

Lepra::uint8 Painter::GetAlphaValue()
{
	return mAlphaValue;
}

void Painter::SetClippingRect(const Lepra::PixelRect& pClippingRect)
{
	SetClippingRect(pClippingRect.mLeft, pClippingRect.mTop, pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::ReduceClippingRect(const Lepra::PixelRect& pClippingRect)
{
	ReduceClippingRect(pClippingRect.mLeft, pClippingRect.mTop, pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::GetClippingRect(Lepra::PixelRect& pClippingRect)
{
	pClippingRect = mClippingRect;
}

void Painter::SetColor(Lepra::uint8 pRed, Lepra::uint8 pGreen, Lepra::uint8 pBlue, Lepra::uint8 pPaletteIndex, unsigned pColorIndex)
{
	SetColor(Lepra::Color(pRed, pGreen, pBlue, pPaletteIndex), pColorIndex);
}

Lepra::Color Painter::GetColor(unsigned pColorIndex)
{
	return mColor[pColorIndex];
}

void Painter::DrawPixel(int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawPixel(x, y);
	else
		CreateRect(x, y, x, y);
}

void Painter::DrawPixel(const Lepra::PixelCoords& pCoords)
{
	DrawPixel(pCoords.x, pCoords.y);
}

void Painter::DrawLine(int pX1, int pY1, int pX2, int pY2)
{
	if(mCurrentDisplayList == 0)
		DoDrawLine(pX1, pY1, pX2, pY2);
	else
		CreateLine(pX1, pY1, pX2, pY2);
}

void Painter::DrawLine(const Lepra::PixelCoords& pPoint1, const Lepra::PixelCoords& pPoint2)
{
	DrawLine(pPoint1.x, pPoint1.y, pPoint2.x, pPoint2.y);
}

void Painter::DrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	if(mCurrentDisplayList == 0)
		DoDrawRect(pLeft, pTop, pRight, pBottom, pWidth);
	else
		CreateRectFrame(pLeft, pTop, pRight, pBottom, pWidth);
}

void Painter::DrawRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight, int pWidth)
{
	DrawRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y, pWidth);
}

void Painter::DrawRect(const Lepra::PixelRect& pRect, int pWidth)
{
	DrawRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom, pWidth);
}

void Painter::FillRect(int pLeft, int pTop, int pRight, int pBottom)
{
	if(mCurrentDisplayList == 0)
		DoFillRect(pLeft, pTop, pRight, pBottom);
	else
		CreateRect(pLeft, pTop, pRight, pBottom);
}

void Painter::FillRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight)
{
	FillRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y);
}

void Painter::FillRect(const Lepra::PixelRect& pRect)
{
	FillRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom);
}

void Painter::Draw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	if(mCurrentDisplayList == 0)
		DoDraw3DRect(pLeft, pTop, pRight, pBottom, pWidth, pSunken);
	else
		Create3DRectFrame(pLeft, pTop, pRight, pBottom, pWidth, pSunken);
}

void Painter::Draw3DRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight, int pWidth, bool pSunken)
{
	Draw3DRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y, pWidth, pSunken);
}

void Painter::Draw3DRect(const Lepra::PixelRect& pRect, int pWidth, bool pSunken)
{
	Draw3DRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom, pWidth, pSunken);
}

void Painter::FillShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	if(mCurrentDisplayList == 0)
		DoFillShadedRect(pLeft, pTop, pRight, pBottom);
	else
		CreateShadedRect(pLeft, pTop, pRight, pBottom);
}

void Painter::FillShadedRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight)
{
	FillShadedRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y);
}

void Painter::FillShadedRect(const Lepra::PixelRect& pRect)
{
	FillShadedRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom);
}

void Painter::FillTriangle(float pX1, float pY1,
			   float pX2, float pY2,
			   float pX3, float pY3)
{
	if(mCurrentDisplayList == 0)
		DoFillTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
	else
		CreateTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
}

void Painter::FillTriangle(const Lepra::PixelCoords& pPoint1,
			   const Lepra::PixelCoords& pPoint2,
			   const Lepra::PixelCoords& pPoint3)
{
	FillTriangle((float)pPoint1.x, (float)pPoint1.y,
	             (float)pPoint2.x, (float)pPoint2.y,
	             (float)pPoint3.x, (float)pPoint3.y);
}

void Painter::FillShadedTriangle(float pX1, float pY1,
				 float pX2, float pY2,
				 float pX3, float pY3)
{
	if(mCurrentDisplayList == 0)
		DoFillShadedTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
	else
		CreateShadedTriangle(pX1, pY1, pX2, pY2, pX3, pY3);
}

void Painter::FillShadedTriangle(const Lepra::PixelCoords& pPoint1,
			         const Lepra::PixelCoords& pPoint2,
			         const Lepra::PixelCoords& pPoint3)
{
	FillShadedTriangle((float)pPoint1.x, (float)pPoint1.y, 
	                   (float)pPoint2.x, (float)pPoint2.y, 
	                   (float)pPoint3.x, (float)pPoint3.y);
}

void Painter::FillTriangle(float pX1, float pY1, float pU1, float pV1,
			   float pX2, float pY2, float pU2, float pV2,
			   float pX3, float pY3, float pU3, float pV3,
			   ImageID pImageID)
{
	if(mCurrentDisplayList == 0)
		DoFillTriangle(pX1, pY1, pU1, pV1, pX2, pY2, pU2, pV2, pX3, pY3, pU3, pV3, pImageID);
	else
		CreateTriangle(pX1, pY1, pU1, pV1, pX2, pY2, pU2, pV2, pX3, pY3, pU3, pV3, pImageID);
}

void Painter::FillTriangle(const Lepra::PixelCoords& pPoint1, float pU1, float pV1,
			   const Lepra::PixelCoords& pPoint2, float pU2, float pV2,
			   const Lepra::PixelCoords& pPoint3, float pU3, float pV3,
			   ImageID pImageID)
{
	FillTriangle((float)pPoint1.x, (float)pPoint1.y, pU1, pV1, 
	             (float)pPoint2.x, (float)pPoint2.y, pU2, pV2, 
	             (float)pPoint3.x, (float)pPoint3.y, pU3, pV3, pImageID);
}

void Painter::DrawImage(ImageID pImageID, int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, x, y);
	else
		CreateImage(pImageID, x, y);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft)
{
	DrawImage(pImageID, pTopLeft.x, pTopLeft.y);
}

void Painter::DrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, x, y, pSubpatchRect);
	else
		CreateImage(pImageID, x, y, pSubpatchRect);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft, const Lepra::PixelRect& pSubpatchRect)
{
	DrawImage(pImageID, pTopLeft.x, pTopLeft.y, pSubpatchRect);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelRect& pRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, pRect);
	else
		CreateImage(pImageID, pRect);
}

void Painter::DrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, pRect, pSubpatchRect);
	else
		CreateImage(pImageID, pRect, pSubpatchRect);
}

void Painter::DrawAlphaImage(ImageID pImageID, int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawAlphaImage(pImageID, x, y);
	else
		CreateImage(pImageID, x, y); // TODO: Verify that this works.
}

void Painter::DrawAlphaImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft)
{
	DrawAlphaImage(pImageID, pTopLeft.x, pTopLeft.y);
}

void Painter::DrawDefaultMouseCursor(int x, int y)
{
	DrawImage(mMouseCursorID, x, y);
}

void Painter::SetTabOriginX(int pTabOriginX)
{
	mTabOriginX = pTabOriginX;
}

Painter::RenderMode Painter::DisplayEntity::GetRenderMode() const
{
	return mRM;
}

Lepra::uint8 Painter::DisplayEntity::GetAlpha() const
{
	return mAlpha;
}

Painter::ImageID Painter::DisplayEntity::GetImageID() const
{
	return mImageID;
}

const Lepra::PixelRect& Painter::DisplayEntity::GetClippingRect() const
{
	return mClippingRect;
}

Geometry2D& Painter::DisplayEntity::GetGeometry()
{
	return mGeometry;
}

bool Painter::IsPowerOf2(unsigned pNumber)
{
	return (pNumber == GetClosestPowerOf2(pNumber));
}

Lepra::uint8* Painter::GetStandardFont1()
{
	return smStandardFont1;
}

Lepra::uint8* Painter::GetStandardFont2()
{
	return smStandardFont2;
}

int* Painter::GetCharWidthStdFont1()
{
	return smFont1CharWidth;
}

int* Painter::GetCharWidthStdFont2()
{
	return smFont2CharWidth;
}

Lepra::uint8* Painter::GetStandardMouseCursor()
{
	return smStandardMouseCursor;
}

void Painter::ToScreenCoords(int& x, int& y) const
{
	x = x * (int)mXDir + mOrigoX;
	y = y * (int)mYDir + mOrigoY;
}

void Painter::ToUserCoords(int& x, int& y) const
{
	x = (x - mOrigoX) * (int)mXDir;
	y = (y - mOrigoY) * (int)mYDir;
}

void Painter::ToScreenCoords(float& x, float& y) const
{
	x = x * (float)mXDir + (float)mOrigoX;
	y = y * (float)mYDir + (float)mOrigoY;
}

void Painter::ToUserCoords(float& x, float& y) const
{
	x = (x - (float)mOrigoX) * (float)mXDir;
	y = (y - (float)mOrigoY) * (float)mYDir;
}

bool Painter::XLT(int x1, int x2)
{
	return (x1 * (int)mXDir) <  (x2 * (int)mXDir);
}

bool Painter::XLE(int x1, int x2)
{
	return (x1 * (int)mXDir) <= (x2 * (int)mXDir);
}

bool Painter::XGT(int x1, int x2)
{
	return (x1 * (int)mXDir) >  (x2 * (int)mXDir);
}

bool Painter::XGE(int x1, int x2)
{
	return (x1 * (int)mXDir) >= (x2 * (int)mXDir);
}

bool Painter::YLT(int y1, int y2)
{
	return (y1 * (int)mYDir) <  (y2 * (int)mYDir);
}

bool Painter::YLE(int y1, int y2)
{
	return (y1 * (int)mYDir) <= (y2 * (int)mYDir);
}

bool Painter::YGT(int y1, int y2)
{
	return (y1 * (int)mYDir) >  (y2 * (int)mYDir);
}

bool Painter::YGE(int y1, int y2)
{
	return (y1 * (int)mYDir) >= (y2 * (int)mYDir);
}

Painter::Font* Painter::GetCurrentFontInternal() const
{
	return mCurrentFont;
}

int Painter::GetTabOriginX() const
{
	return mTabOriginX;
}

Lepra::Color& Painter::GetColorInternal(int pColorIndex)
{
	return mColor[pColorIndex];
}
