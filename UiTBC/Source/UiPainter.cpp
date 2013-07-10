
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiPainter.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Canvas.h"
#include "../Include/UiFontManager.h"



namespace UiTbc
{



Painter::Painter() :
	mFontManager(0),
	mDisplayListIDManager(1, 100000, 0),
	mCurrentDisplayList(0),
	mRenderMode(RM_NORMAL),
	mOrigoX(0),
	mOrigoY(0),
	mXDir(X_RIGHT),
	mYDir(Y_DOWN),
	mCanvas(0),
	mAlphaValue(255),
	mTabSize(0)
{
	mDefaultDisplayList = NewDisplayList();
}

Painter::~Painter()
{
	DeleteDisplayList(mDefaultDisplayList);
	mFontManager = 0;
}

void Painter::DefineCoordinates(int pOrigoX, int pOrigoY, XDir pXDir, YDir pYDir)
{
	mOrigoX = pOrigoX;
	mOrigoY = pOrigoY;
	mXDir = pXDir;
	mYDir = pYDir;
}

void Painter::SetDestCanvas(Canvas* pCanvas)
{
	mCanvas = pCanvas;
}

bool Painter::PushAttrib(unsigned pAttrib)
{
	bool pReturnValue = true;

	const size_t lMaxCount = 1024;

	if ((pAttrib & ATTR_RENDERMODE) != 0)
	{
		mAttribRMStack.push_back(mRenderMode);
		if (mAttribRMStack.size() > lMaxCount)
		{
			mAttribRMStack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_ALPHAVALUE) != 0)
	{
		mAttribAlphaStack.push_back(GetAlphaValue());
		if (mAttribAlphaStack.size() > lMaxCount)
		{
			mAttribAlphaStack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR0) != 0)
	{
		mAttribColor0Stack.push_back(GetColor(0));
		if (mAttribColor0Stack.size() > lMaxCount)
		{
			mAttribColor0Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR1) != 0)
	{
		mAttribColor1Stack.push_back(GetColor(1));
		if (mAttribColor1Stack.size() > lMaxCount)
		{
			mAttribColor1Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR2) != 0)
	{
		mAttribColor2Stack.push_back(GetColor(2));
		if (mAttribColor2Stack.size() > lMaxCount)
		{
			mAttribColor2Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_COLOR3) != 0)
	{
		mAttribColor3Stack.push_back(GetColor(3));
		if (mAttribColor3Stack.size() > lMaxCount)
		{
			mAttribColor3Stack.pop_front();
			pReturnValue = false;
		}
	}

	if ((pAttrib & ATTR_CLIPRECT) != 0)
	{
		PixelRect lClippingRect;
		GetClippingRect(lClippingRect);
		mAttribClipRectStack.push_back(lClippingRect);
		if (mAttribClipRectStack.size() > lMaxCount)
		{
			mAttribClipRectStack.pop_front();
			pReturnValue = false;
		}
	}

	mAttribStack.push_back(pAttrib);

	return pReturnValue;
}

bool Painter::PopAttrib()
{
	if (mAttribStack.empty())
	{
		return false;
	}

	unsigned lAttrib = mAttribStack.back();
	mAttribStack.pop_back();

	if ((lAttrib & ATTR_RENDERMODE) != 0)
	{
		if (!mAttribRMStack.empty())
		{
			SetRenderMode(mAttribRMStack.back());
			mAttribRMStack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_ALPHAVALUE) != 0)
	{
		if (!mAttribAlphaStack.empty())
		{
			SetAlphaValue(mAttribAlphaStack.back());
			mAttribAlphaStack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR0) != 0)
	{
		if (!mAttribColor0Stack.empty())
		{
			SetColor(mAttribColor0Stack.back(), 0);
			mAttribColor0Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR1) != 0)
	{
		if (!mAttribColor1Stack.empty())
		{
			SetColor(mAttribColor1Stack.back(), 1);
			mAttribColor1Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR2) != 0)
	{
		if (!mAttribColor2Stack.empty())
		{
			SetColor(mAttribColor2Stack.back(), 2);
			mAttribColor2Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_COLOR3) != 0)
	{
		if (!mAttribColor3Stack.empty())
		{
			SetColor(mAttribColor3Stack.back(), 3);
			mAttribColor3Stack.pop_back();
		}
		else
		{
			return false;
		}
	}

	if ((lAttrib & ATTR_CLIPRECT) != 0)
	{
		if (!mAttribClipRectStack.empty())
		{
			PixelRect lRect = mAttribClipRectStack.back();
			mAttribClipRectStack.pop_back();
			SetClippingRect(lRect.mLeft, lRect.mTop, lRect.mRight, lRect.mBottom);
		}
		else
		{
			return false;
		}
	}

	return true;
}

void Painter::SetRenderMode(RenderMode pRM)
{
	mRenderMode = pRM;
}

void Painter::SetAlphaValue(uint8 pAlpha)
{
	mAlphaValue = pAlpha;
}

void Painter::SetClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	mClippingRect.Set(pLeft, pTop, pRight, pBottom);
}

void Painter::ReduceClippingRect(int pLeft, int pTop, int pRight, int pBottom)
{
	PixelRect lClippingRect(mClippingRect);

	if (XLT(lClippingRect.mLeft, pLeft) == true)
	{
		lClippingRect.mLeft = pLeft;
	}

	if (XGT(lClippingRect.mRight, pRight) == true)
	{
		lClippingRect.mRight = pRight;
	}

	if (YLT(lClippingRect.mTop, pTop) == true)
	{
		lClippingRect.mTop = pTop;
	}

	if (YGT(lClippingRect.mBottom, pBottom) == true)
	{
		lClippingRect.mBottom = pBottom;
	}

	SetClippingRect(lClippingRect);
}

void Painter::SetColor(const Color& pColor, unsigned pColorIndex)
{
	mColor[pColorIndex] = pColor;
}

void Painter::SetFontManager(FontManager* pFontManager)
{
	mFontManager = pFontManager;
}

FontManager* Painter::GetFontManager() const
{
	deb_assert(mFontManager);
	return (mFontManager);
}

int Painter::GetStringWidth(const str& pString) const
{
	if (!mFontManager)
	{
		return (0);
	}
	return (mFontManager->GetStringWidth(pString));
}

int Painter::GetFontHeight() const
{
	if (!mFontManager)
	{
		return (0);
	}
	return (mFontManager->GetFontHeight());
}

int Painter::GetLineHeight() const
{
	if (!mFontManager)
	{
		return (0);
	}
	return (mFontManager->GetLineHeight());
}

int Painter::GetTabSize() const
{
	return mTabSize;
}

void Painter::SetTabSize(int pSize)
{
	mTabSize = pSize;
}

Painter::DisplayListID Painter::NewDisplayList()
{
	int lID = mDisplayListIDManager.GetFreeId();
	if(lID != mDisplayListIDManager.GetInvalidId())
	{
		mDisplayListMap.insert(DisplayListMap::value_type(lID, new std::vector<DisplayEntity*>));
	}
	return (DisplayListID)lID;
}

void Painter::DeleteDisplayList(DisplayListID pDisplayListID)
{
	DisplayListMap::iterator it = mDisplayListMap.find(pDisplayListID);
	if(it != mDisplayListMap.end())
	{
		std::vector<DisplayEntity*>* lDisplayList = (*it).second;
		mDisplayListMap.erase(it);

		std::vector<DisplayEntity*>::iterator lListIter;
		for(lListIter = lDisplayList->begin(); lListIter != lDisplayList->end(); ++lListIter)
		{
			delete *lListIter;
		}
		delete lDisplayList;
	}
}

void Painter::BeginDisplayList(DisplayListID pDisplayListID)
{
	DisplayListMap::iterator it = mDisplayListMap.find(pDisplayListID);
	if (it != mDisplayListMap.end())
	{
		mCurrentDisplayList = (*it).second;
		mDisplayListIter = mCurrentDisplayList->begin();
	}
}

void Painter::EndDisplayList()
{
	if (mCurrentDisplayList)
	{
		std::vector<DisplayEntity*>::iterator lListIter;
		for (lListIter = mCurrentDisplayList->begin(); lListIter != mCurrentDisplayList->end(); ++lListIter)
		{
			DisplayEntity* lEntity = *lListIter;
			lEntity->mGeometry.Reset();
		}
		mDisplayListIter = mCurrentDisplayList->begin();
		mCurrentDisplayList = 0;
	}
}

void Painter::RenderDisplayList(DisplayListID pDisplayListID)
{
	DisplayListMap::iterator it = mDisplayListMap.find(pDisplayListID);
	if(it != mDisplayListMap.end())
	{
		std::vector<DisplayEntity*>* lDisplayList = (*it).second;
		DoRenderDisplayList(lDisplayList);
	}
}

void Painter::ClearFontBuffers()
{
}

Geometry2D* Painter::FetchDisplayEntity(unsigned pVertexFormat, ImageID pImageID)
{
	AdjustVertexFormat(pVertexFormat);

	DisplayEntity* lEntity = 0;
	if(mDisplayListIter != mCurrentDisplayList->end())
	{
		lEntity = *mDisplayListIter;
	}

	PixelRect lClippingRect;
	GetClippingRect(lClippingRect);
	if(lEntity == 0 ||
	   lEntity->mRM != mRenderMode ||
	   lEntity->mGeometry.GetVertexFormat() != pVertexFormat ||
	   lEntity->mAlpha != GetAlphaValue() ||
	   lEntity->mImageID != pImageID ||
	   lEntity->mClippingRect.mTop != lClippingRect.mTop ||
	   lEntity->mClippingRect.mBottom != lClippingRect.mBottom ||
	   lEntity->mClippingRect.mLeft != lClippingRect.mLeft ||
	   lEntity->mClippingRect.mRight != lClippingRect.mRight)
	{
		if(mDisplayListIter != mCurrentDisplayList->end())
			++mDisplayListIter;
		if(mDisplayListIter != mCurrentDisplayList->end())
		{
			lEntity = *mDisplayListIter;
			lEntity->Init(mRenderMode, GetAlphaValue(), pImageID, lClippingRect, pVertexFormat);
		}
		else
		{
			lEntity = new DisplayEntity(mRenderMode, GetAlphaValue(), pImageID, lClippingRect, pVertexFormat);
			mCurrentDisplayList->push_back(lEntity);
			mDisplayListIter = mCurrentDisplayList->end();
			--mDisplayListIter;
		}
	}
	return &lEntity->mGeometry;
}

void Painter::CreateLine(int pX1, int pY1, int pX2, int pY2)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lX1 = (float)pX1 - 0.5f;
	float lY1 = (float)pY1 - 0.5f;
	float lX2 = (float)pX2 - 0.5f;
	float lY2 = (float)pY2 - 0.5f;

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;

	Vector2DF lNormal(lY2 - lY1, lX1 - lX2);
	lNormal.Normalize();

	uint32 lV0 = lGeometry->SetVertex(lX1 - lNormal.x, lY1 - lNormal.y, r, g, b);
	uint32 lV1 = lGeometry->SetVertex(lX1 + lNormal.x, lY1 + lNormal.y, r, g, b);
	uint32 lV2 = lGeometry->SetVertex(lX2 + lNormal.x, lY2 + lNormal.y, r, g, b);
	uint32 lV3 = lGeometry->SetVertex(lX2 - lNormal.x, lY2 - lNormal.y, r, g, b);

	lGeometry->SetTriangle(lV0, lV1, lV2);
	lGeometry->SetTriangle(lV0, lV2, lV3);
}

void Painter::CreateRectFrame(int pLeft, int pTop, int pRight, int pBottom, int pWidth)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;

	uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r, g, b);     // Outer top left.
	uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r, g, b);    // Outer top right.
	uint32 lV2 = lGeometry->SetVertex(lRight, lBottom, r, g, b); // Outer bottom right.
	uint32 lV3 = lGeometry->SetVertex(lLeft, lBottom, r, g, b);  // Outer bottom left.

	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;

	uint32 lV4 = lGeometry->SetVertex(lLeft, lTop, r, g, b);     // Inner top left.
	uint32 lV5 = lGeometry->SetVertex(lRight, lTop, r, g, b);    // Inner top right.
	uint32 lV6 = lGeometry->SetVertex(lRight, lBottom, r, g, b); // Inner bottom right.
	uint32 lV7 = lGeometry->SetVertex(lLeft, lBottom, r, g, b);  // Inner bottom left.

	lGeometry->SetTriangle(lV0, lV4, lV7);
	lGeometry->SetTriangle(lV0, lV7, lV3);
	lGeometry->SetTriangle(lV0, lV1, lV5);
	lGeometry->SetTriangle(lV0, lV5, lV4);
	lGeometry->SetTriangle(lV1, lV2, lV6);
	lGeometry->SetTriangle(lV1, lV6, lV5);
	lGeometry->SetTriangle(lV7, lV6, lV2);
	lGeometry->SetTriangle(lV7, lV2, lV3);
}

void Painter::Create3DRectFrame(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r[4];
	float g[4];
	float b[4];
	int lI[4] = {0, 1, 2, 3};
	
	if(pSunken)
	{
		lI[0] = 1;
		lI[1] = 0;
		lI[2] = 3;
		lI[3] = 2;
	}
	
	for(int i = 0; i < 4; i++)
	{
		r[i] = (float)mColor[lI[i]].mRed / 255.0f;
		g[i] = (float)mColor[lI[i]].mGreen / 255.0f;
		b[i] = (float)mColor[lI[i]].mBlue / 255.0f;
	}

	uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r[0], g[0], b[0]);     // Outer top left.
	uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r[0], g[0], b[0]);    // Outer top right #1.
	uint32 lV2 = lGeometry->SetVertex(lRight, lTop, r[1], g[1], b[1]);    // Outer top right #2.
	uint32 lV3 = lGeometry->SetVertex(lRight, lBottom, r[1], g[1], b[1]); // Outer bottom right.
	uint32 lV4 = lGeometry->SetVertex(lLeft, lBottom, r[0], g[0], b[0]);  // Outer bottom left #1.
	uint32 lV5 = lGeometry->SetVertex(lLeft, lBottom, r[1], g[1], b[1]);  // Outer bottom left #2.

	lLeft += pWidth;
	lTop += pWidth;
	lRight -= pWidth;
	lBottom -= pWidth;

	uint32 lV6 = lGeometry->SetVertex(lLeft, lTop, r[2], g[2], b[2]);     // Inner top left.
	uint32 lV7 = lGeometry->SetVertex(lRight, lTop, r[2], g[2], b[2]);    // Inner top right #1.
	uint32 lV8 = lGeometry->SetVertex(lRight, lTop, r[3], g[3], b[3]);    // Inner top right #2.
	uint32 lV9 = lGeometry->SetVertex(lRight, lBottom, r[3], g[3], b[3]); // Inner bottom right.
	uint32 lV10 = lGeometry->SetVertex(lLeft, lBottom, r[2], g[2], b[2]);  // Inner bottom left #1.
	uint32 lV11 = lGeometry->SetVertex(lLeft, lBottom, r[3], g[3], b[3]);  // Inner bottom left #2.

	//     0--------------------------1,2
	//     |                           |
	//     |   6------------------7,8  |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     |   |                   |   |
	//     | 10,11-----------------9   |
	//     |                           |
	//    4,5--------------------------3

	lGeometry->SetTriangle(lV0, lV1, lV7);  // Top
	lGeometry->SetTriangle(lV0, lV7, lV6);  // ...
	lGeometry->SetTriangle(lV0, lV6, lV10); // Left
	lGeometry->SetTriangle(lV0, lV10, lV4); // ...
	lGeometry->SetTriangle(lV8, lV2, lV3);  // Right
	lGeometry->SetTriangle(lV8, lV3, lV9);  // ...
	lGeometry->SetTriangle(lV11, lV9, lV3); // Bottom
	lGeometry->SetTriangle(lV11, lV3, lV5); // ...
}

void Painter::CreateRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;

	uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r, g, b);
	uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r, g, b);
	uint32 lV2 = lGeometry->SetVertex(lRight, lBottom, r, g, b);
	uint32 lV3 = lGeometry->SetVertex(lLeft, lBottom, r, g, b);

	lGeometry->SetTriangle(lV0, lV1, lV2);
	lGeometry->SetTriangle(lV0, lV2, lV3);
}

void Painter::CreateShadedRect(int pLeft, int pTop, int pRight, int pBottom)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float lLeft   = (float)pLeft - 0.5f;
	float lRight  = (float)pRight - 0.5f;
	float lTop    = (float)pTop - 0.5f;
	float lBottom = (float)pBottom - 0.5f;

	float r[4];
	float g[4];
	float b[4];
	for(int i = 0; i < 4; i++)
	{
		r[i] = (float)mColor[i].mRed / 255.0f;
		g[i] = (float)mColor[i].mGreen / 255.0f;
		b[i] = (float)mColor[i].mBlue / 255.0f;
	}

	uint32 lV0 = lGeometry->SetVertex(lLeft, lTop, r[0], g[0], b[0]);
	uint32 lV1 = lGeometry->SetVertex(lRight, lTop, r[1], g[1], b[1]);
	uint32 lV2 = lGeometry->SetVertex(lRight, lBottom, r[2], g[2], b[2]);
	uint32 lV3 = lGeometry->SetVertex(lLeft, lBottom, r[3], g[3], b[3]);
	uint32 lV4 = lGeometry->SetVertex((lLeft + lRight) * 0.5f, (lTop + lBottom) * 0.5f, 
		(r[0] + r[1] + r[2] + r[3]) * 0.25f, 
		(g[0] + g[1] + g[2] + g[3]) * 0.25f,
		(b[0] + b[1] + b[2] + b[3]) * 0.25f);

	lGeometry->SetTriangle(lV0, lV1, lV4);
	lGeometry->SetTriangle(lV1, lV2, lV4);
	lGeometry->SetTriangle(lV2, lV3, lV4);
	lGeometry->SetTriangle(lV3, lV0, lV4);
}

void Painter::CreateTriangle(float pX1, float pY1, float pX2, float pY2, float pX3, float pY3)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float r = (float)mColor[0].mRed / 255.0f;
	float g = (float)mColor[0].mGreen / 255.0f;
	float b = (float)mColor[0].mBlue / 255.0f;
	
	uint32 lV0 = lGeometry->SetVertex(pX1, pY1, r, g, b);
	uint32 lV1 = lGeometry->SetVertex(pX2, pY2, r, g, b);
	uint32 lV2 = lGeometry->SetVertex(pX3, pY3, r, g, b);
	lGeometry->SetTriangle(lV0, lV1, lV2);
}

void Painter::CreateShadedTriangle(float pX1, float pY1, float pX2, float pY2, float pX3, float pY3)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_RGB);

	float r[3];
	float g[3];
	float b[3];
	for(int i = 0; i < 3; i++)
	{
		r[i] = (float)mColor[i].mRed / 255.0f;
		g[i] = (float)mColor[i].mGreen / 255.0f;
		b[i] = (float)mColor[i].mBlue / 255.0f;
	}

	uint32 lV0 = lGeometry->SetVertex(pX1, pY1, r[0], g[0], b[0]);
	uint32 lV1 = lGeometry->SetVertex(pX2, pY2, r[1], g[1], b[1]);
	uint32 lV2 = lGeometry->SetVertex(pX3, pY3, r[2], g[2], b[2]);
	lGeometry->SetTriangle(lV0, lV1, lV2);
}

void Painter::CreateTriangle(float pX1, float pY1, float pU1, float pV1,
                             float pX2, float pY2, float pU2, float pV2,
                             float pX3, float pY3, float pU3, float pV3,
                             ImageID pImageID)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_UV, pImageID);

	uint32 lV0 = lGeometry->SetVertex(pX1, pY1, pU1, pV1);
	uint32 lV1 = lGeometry->SetVertex(pX2, pY2, pU2, pV2);
	uint32 lV2 = lGeometry->SetVertex(pX3, pY3, pU3, pV3);
	lGeometry->SetTriangle(lV0, lV1, lV2);
}

void Painter::CreateImage(ImageID pImageID, int x, int y)
{
	int w;
	int h;
	GetImageSize(pImageID, w, h);

	PixelRect lRect(x, y, x + w, y + h);
	PixelRect lSubpatchRect(0, 0, w, h);
	CreateImage(pImageID, lRect, lSubpatchRect);
}

void Painter::CreateImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect)
{
	PixelRect lRect(x, y, x + pSubpatchRect.GetWidth(), y + pSubpatchRect.GetHeight());
	CreateImage(pImageID, lRect, pSubpatchRect);
}

void Painter::CreateImage(ImageID pImageID, const PixelRect& pRect)
{
	PixelRect lSubpatchRect(0, 0, 0, 0);
	GetImageSize(pImageID, lSubpatchRect.mRight, lSubpatchRect.mBottom);
	CreateImage(pImageID, pRect, lSubpatchRect);
}

void Painter::CreateImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect)
{
	Geometry2D* lGeometry = FetchDisplayEntity(Geometry2D::VTX_UV, pImageID);

	float lLeft   = (float)pRect.mLeft - 0.5f;
	float lRight  = (float)pRect.mRight - 0.5f;
	float lTop    = (float)pRect.mTop - 0.5f;
	float lBottom = (float)pRect.mBottom - 0.5f;

	int lWidth;
	int lHeight;
	GetImageSize(pImageID, lWidth, lHeight);

	float lU1 = (float)pSubpatchRect.mLeft / (float)lWidth;
	float lV1 = (float)pSubpatchRect.mTop / (float)lHeight;
	float lU2 = (float)pSubpatchRect.mRight / (float)lWidth;
	float lV2 = (float)pSubpatchRect.mBottom / (float)lHeight;

	uint32 lVtx0 = lGeometry->SetVertex(lLeft, lTop, lU1, lV1);
	uint32 lVtx1 = lGeometry->SetVertex(lRight, lTop, lU2, lV1);
	uint32 lVtx2 = lGeometry->SetVertex(lRight, lBottom, lU2, lV2);
	uint32 lVtx3 = lGeometry->SetVertex(lLeft, lBottom, lU1, lV2);

	lGeometry->SetTriangle(lVtx0, lVtx1, lVtx2);
	lGeometry->SetTriangle(lVtx0, lVtx2, lVtx3);	
}

unsigned Painter::GetClosestPowerOf2(unsigned pNumber, bool pGreater)
{
	if (pNumber == 0)
	{
		return 0;
	}

	unsigned lExp = GetExponent(pNumber);
	unsigned lPow = 1 << lExp;
	
	if(pGreater && lPow < pNumber)
		return (lPow << 1);
	else
		return lPow;
}

unsigned Painter::GetExponent(unsigned pPowerOf2)
{
	if (pPowerOf2 == 0)
	{
		// Error.
		return (unsigned)-1;
	}

	unsigned lExp = 0;
	
	while ((pPowerOf2 >> lExp) > 1)
	{
		lExp++;
	}

	return lExp;
}

uint8 Painter::FindMatchingColor(const Color& pColor)
{
	long lTargetR = pColor.mRed;
	long lTargetG = pColor.mGreen;
	long lTargetB = pColor.mBlue;

	long lMinError = 0x7FFFFFFF;
	uint8 lBestMatch = 0;

	const Color* lPalette = GetCanvas()->GetPalette();

	for (int i = 0; i < 256; i++)
	{
		long lDR = lTargetR - lPalette[i].mRed;
		long lDG = lTargetG - lPalette[i].mGreen;
		long lDB = lTargetB - lPalette[i].mBlue;

		long lError = lDR * lDR + lDG * lDG + lDB * lDB;

		if (i == 0 || lError < lMinError)
		{
			lMinError = lError;
			lBestMatch = (uint8)i;
		}
	}

	return lBestMatch;
}

void Painter::GetScreenCoordClippingRect(PixelRect& pClippingRect) const
{
	pClippingRect = mClippingRect;
	ToScreenCoords(pClippingRect.mLeft, pClippingRect.mTop);
	ToScreenCoords(pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::AdjustVertexFormat(unsigned&)
{
	// Default behaviour. Do nothing.
}


int Painter::GetOrigoX() const
{
	return mOrigoX;
}

int Painter::GetOrigoY() const
{
	return mOrigoY;
}

Painter::XDir Painter::GetXDir() const
{
	return mXDir;
}

Painter::YDir Painter::GetYDir() const
{
	return mYDir;
}

Canvas* Painter::GetCanvas() const
{
	return mCanvas;
}

Painter::RenderMode Painter::GetRenderMode() const
{
	return mRenderMode;
}

uint8 Painter::GetAlphaValue() const
{
	return mAlphaValue;
}

void Painter::SetClippingRect(const PixelRect& pClippingRect)
{
	SetClippingRect(pClippingRect.mLeft, pClippingRect.mTop, pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::ReduceClippingRect(const PixelRect& pClippingRect)
{
	ReduceClippingRect(pClippingRect.mLeft, pClippingRect.mTop, pClippingRect.mRight, pClippingRect.mBottom);
}

void Painter::GetClippingRect(PixelRect& pClippingRect) const
{
	pClippingRect = mClippingRect;
}

void Painter::SetColor(uint8 pRed, uint8 pGreen, uint8 pBlue, uint8 pPaletteIndex, unsigned pColorIndex)
{
	SetColor(Color(pRed, pGreen, pBlue, pPaletteIndex), pColorIndex);
}

Color Painter::GetColor(unsigned pColorIndex) const
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

void Painter::DrawPixel(const PixelCoord& pCoords)
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

void Painter::DrawLine(const PixelCoord& pPoint1, const PixelCoord& pPoint2)
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

void Painter::DrawRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight, int pWidth)
{
	DrawRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y, pWidth);
}

void Painter::DrawRect(const PixelRect& pRect, int pWidth)
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

void Painter::FillRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight)
{
	FillRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y);
}

void Painter::FillRect(const PixelRect& pRect)
{
	FillRect(pRect.mLeft, pRect.mTop, pRect.mRight, pRect.mBottom);
}

void Painter::Draw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken)
{
	//if(mCurrentDisplayList == 0)
		DoDraw3DRect(pLeft, pTop, pRight, pBottom, pWidth, pSunken);
	//else
	//	Create3DRectFrame(pLeft, pTop, pRight, pBottom, pWidth, pSunken);
}

void Painter::Draw3DRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight, int pWidth, bool pSunken)
{
	Draw3DRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y, pWidth, pSunken);
}

void Painter::Draw3DRect(const PixelRect& pRect, int pWidth, bool pSunken)
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

void Painter::FillShadedRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight)
{
	FillShadedRect(pTopLeft.x, pTopLeft.y, pBottomRight.x, pBottomRight.y);
}

void Painter::FillShadedRect(const PixelRect& pRect)
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

void Painter::FillTriangle(const PixelCoord& pPoint1,
			   const PixelCoord& pPoint2,
			   const PixelCoord& pPoint3)
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

void Painter::FillShadedTriangle(const PixelCoord& pPoint1,
			         const PixelCoord& pPoint2,
			         const PixelCoord& pPoint3)
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

void Painter::FillTriangle(const PixelCoord& pPoint1, float pU1, float pV1,
			   const PixelCoord& pPoint2, float pU2, float pV2,
			   const PixelCoord& pPoint3, float pU3, float pV3,
			   ImageID pImageID)
{
	FillTriangle((float)pPoint1.x, (float)pPoint1.y, pU1, pV1, 
	             (float)pPoint2.x, (float)pPoint2.y, pU2, pV2, 
	             (float)pPoint3.x, (float)pPoint3.y, pU3, pV3, pImageID);
}

void Painter::DrawArc(int x, int y, int dx, int dy, int a1, int a2, bool pFill)
{
	if (dx <= 0 || dy <= 0)
	{
		return;
	}
	const size_t lCurveCount = ((dx*2 + dy*2) / 20 + std::abs(a1-a2)/20 + 12) & (~7);
	std::vector<Vector2DF> lCoords;
	const float lXRadius = dx*0.5f;
	const float lYRadius = dy*0.5f;
	const float lMidX = x + dx*0.5f;
	const float lMidY = y + dy*0.5f;
	if (pFill)
	{
		lCoords.push_back(Vector2DF(lMidX, lMidY));
	}
	const float lStartAngle = Lepra::Math::Deg2Rad((float)a1);
	const float lEndAngle = Lepra::Math::Deg2Rad((float)a2);
	const float lDeltaAngle = (lEndAngle-lStartAngle)/(lCurveCount-1);
	float lAngle = lStartAngle;
	for (size_t i = 0; i < lCurveCount; ++i)
	{
		lCoords.push_back(Vector2DF(
			lMidX + cos(lAngle)*lXRadius,
			lMidY - sin(lAngle)*lYRadius));
		lAngle += lDeltaAngle;
	}
	DrawFan(lCoords, pFill);
}

void Painter::DrawImage(ImageID pImageID, int x, int y)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, x, y);
	else
		CreateImage(pImageID, x, y);
}

void Painter::DrawImage(ImageID pImageID, const PixelCoord& pTopLeft)
{
	DrawImage(pImageID, pTopLeft.x, pTopLeft.y);
}

void Painter::DrawImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, x, y, pSubpatchRect);
	else
		CreateImage(pImageID, x, y, pSubpatchRect);
}

void Painter::DrawImage(ImageID pImageID, const PixelCoord& pTopLeft, const PixelRect& pSubpatchRect)
{
	DrawImage(pImageID, pTopLeft.x, pTopLeft.y, pSubpatchRect);
}

void Painter::DrawImage(ImageID pImageID, const PixelRect& pRect)
{
	if(mCurrentDisplayList == 0)
		DoDrawImage(pImageID, pRect);
	else
		CreateImage(pImageID, pRect);
}

void Painter::DrawImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect)
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
		CreateImage(pImageID, x, y);
}

void Painter::DrawAlphaImage(ImageID pImageID, const PixelCoord& pTopLeft)
{
	DrawAlphaImage(pImageID, pTopLeft.x, pTopLeft.y);
}

Painter::RenderMode Painter::DisplayEntity::GetRenderMode() const
{
	return mRM;
}

uint8 Painter::DisplayEntity::GetAlpha() const
{
	return mAlpha;
}

Painter::ImageID Painter::DisplayEntity::GetImageID() const
{
	return mImageID;
}

const PixelRect& Painter::DisplayEntity::GetClippingRect() const
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

Color& Painter::GetColorInternal(int pColorIndex)
{
	return mColor[pColorIndex];
}



}
