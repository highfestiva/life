
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games
//
// NOTES:
//
// The Painter is the base class, interface or API to render 2D graphics.
//
// Usage:
// 1. Create an instance and call DefineCoordinates() in order to define
//    the coordinate system.
// 2. Call SetDestCanvas() to set the destination buffer which represents
//    the surface to which the Painter should draw.
// 3. Upload the images to use to the Painter through AddImage().
// 3. Configure the state of the Painter using the functions:
//	* SetRenderMode()
//	* SetAlphaValue()
//	* Reset/Reduce/SetClippingRect()
//	* SetColor()
// 4. Render some graphics by calling the various Draw- or Fill-functions.
// 5. Print text using the function PrintText().
// 6. For better performance on hardware accelerated Painters, you may create
//    a static display list using the various Create-functions.



#pragma once

#include <hash_map>
#include <list>
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Vector2D.h"
#include "UiTBC.h"
#include "UiGeometry2D.h"



namespace UiTbc
{



class FontManager;



class Painter
{
public:

	enum RenderMode
	{
		RM_NORMAL = 0,
		RM_ALPHATEST,
		RM_ALPHABLEND,
		RM_XOR,
		RM_ADD,
	};

	enum // Attributes
	{
		ATTR_RENDERMODE    = (1 << 0),
		ATTR_ALPHAVALUE    = (1 << 1),
		ATTR_COLOR0        = (1 << 2),
		ATTR_COLOR1        = (1 << 3),
		ATTR_COLOR2        = (1 << 4),
		ATTR_COLOR3        = (1 << 5),
		ATTR_CLIPRECT      = (1 << 7),

		ATTR_COLORS        = ATTR_COLOR0 |
		                     ATTR_COLOR1 |
				     ATTR_COLOR2 |
				     ATTR_COLOR3,

		ATTR_ALL = ATTR_RENDERMODE    |
		           ATTR_ALPHAVALUE    |
			   ATTR_COLORS        |
			   ATTR_CLIPRECT,
	};

	enum UpdateHint
	{
		UPDATE_ACCURATE = 0,
		UPDATE_FAST,
	};

	// Coordinate X-axis direction.
	enum XDir
	{
		X_RIGHT = 1,
		X_LEFT  = -1,
	};

	enum YDir
	{
		Y_UP   = -1,
		Y_DOWN = 1,
	};

	enum RGBOrder
	{
		RGB = 0,
		BGR,
	};

	enum ImageID
	{
		INVALID_IMAGEID = 0,
	};

	enum DisplayListID
	{
		INVALID_DISPLAYLISTID = 0,
	};

	Painter();
	virtual ~Painter();

	// Defines the coordinate system. The origo coordinates are given in "standard
	// screen coordinates" - relative to the top left corner, x increasing to the right
	// and y increasing downwards.
	void DefineCoordinates(int pOrigoX, int pOrigoY, XDir pXDir, YDir pYDir);
	int GetOrigoX() const;
	int GetOrigoY() const;
	XDir GetXDir() const;
	YDir GetYDir() const;

	// Set the drawing surface.
	virtual void SetDestCanvas(Canvas* pCanvas);
	Canvas* GetCanvas() const;

	virtual void Clear(const Color& pColor) = 0;
	virtual void PrePaint(bool pClearDepthBuffer) = 0;

	// Returns false only if stack owerflow/underflow.
	bool PushAttrib(unsigned pAttrib);
	bool PopAttrib();

	virtual void SetRenderMode(RenderMode pRM);
	RenderMode GetRenderMode() const;
	
	// Set the current alpha value. Overrides alpha buffer...
	// In 8-bit color mode, this is a specific color that will "be" transparent.
	virtual void SetAlphaValue(Lepra::uint8 pAlpha);
	Lepra::uint8 GetAlphaValue() const;

	virtual void ResetClippingRect() = 0;
	virtual void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	void SetClippingRect(const PixelRect& pClippingRect);
	void ReduceClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	void ReduceClippingRect(const PixelRect& pClippingRect);
	void GetClippingRect(PixelRect& pClippingRect) const;

	virtual void SetColor(const Color& pColor, unsigned pColorIndex = 0);
	void SetColor(Lepra::uint8 pRed, Lepra::uint8 pGreen, Lepra::uint8 pBlue, Lepra::uint8 pPaletteIndex, unsigned pColorIndex = 0);
	Color GetColor(unsigned pColorIndex) const;

	void DrawPixel(int x, int y);
	void DrawPixel(const PixelCoord& pCoords);

	void DrawLine(int pX1, int pY1, int pX2, int pY2);
	void DrawLine(const PixelCoord& pPoint1, const PixelCoord& pPoint2);

	// Renders a rectangular frame. The given coordinates defines the outer edges of the frame,
	// expanding inwards 'pWidth' pixels.
	// Color 0 defines the outer color.
	// Color 1 defines the inner color.
	void DrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth);
	void DrawRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight, int pWidth);
	void DrawRect(const PixelRect& pRect, int pWidth);

	void FillRect(int pLeft, int pTop, int pRight, int pBottom);
	void FillRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight);
	void FillRect(const PixelRect& pRect);

	// Same as DrawRect(), but with two different color gradients. Good for GUI rendering.
	// Color 0 and 1 are the outer top left and bottom right colors, respectively.
	// Color 2 and 3 are the inner ditos.
	void Draw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken);
	void Draw3DRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight, int pWidth, bool pSunken);
	void Draw3DRect(const PixelRect& pRect, int pWidth, bool pSunken);

	// FillShadeRect uses all colors...
	void FillShadedRect(int pLeft, int pTop, int pRight, int pBottom);
	void FillShadedRect(const PixelCoord& pTopLeft, const PixelCoord& pBottomRight);
	void FillShadedRect(const PixelRect& pRect);

	void FillTriangle(float pX1, float pY1, float pX2, float pY2, float pX3, float pY3);
	void FillTriangle(const PixelCoord& pPoint1, const PixelCoord& pPoint2, const PixelCoord& pPoint3);

	void FillShadedTriangle(float pX1, float pY1, float pX2, float pY2, float pX3, float pY3);
	void FillShadedTriangle(const PixelCoord& pPoint1, const PixelCoord& pPoint2, const PixelCoord& pPoint3);

	void FillTriangle(float pX1, float pY1, float pU1, float pV1, float pX2, float pY2, float pU2, float pV2,
		float pX3, float pY3, float pU3, float pV3, ImageID pImageID);
	void FillTriangle(const PixelCoord& pPoint1, float pU1, float pV1,
		const PixelCoord& pPoint2, float pU2, float pV2,
		const PixelCoord& pPoint3, float pU3, float pV3, ImageID pImageID);

	void DrawArc(int x, int y, int dx, int dy, int a1, int a2, bool pFill);
	virtual void DrawFan(const std::vector<Vector2DF> pCoords, bool pFill) = 0;
	virtual void DrawImageFan(ImageID pImageID, const std::vector<Vector2DF>& pCoords, const std::vector<Vector2DF>& pTexCoords) = 0;

	// Returns an ID to the added bitmap. Returns 0 if error. 
	// Alpha buffer or bitmap may be NULL.
	virtual ImageID AddImage(const Canvas* pImage, const Canvas* pAlphaBuffer) = 0;

	// If pHint == UPDATE_ACCURATE, this function replaces the contents of pImageID with the 
	// contents of pImage.
	// If pHint == UPDATE_FAST, the behaviour depends on the implementation. In general,
	// pImage and pAlphaBuffer must have the same dimensions and pixel format as the current
	// image.
	virtual void UpdateImage(ImageID pImageID, 
				 const Canvas* pImage, 
				 const Canvas* pAlphaBuffer,
				 UpdateHint pHint = UPDATE_ACCURATE) = 0;
	virtual void RemoveImage(ImageID pImageID) = 0;

	// Renders a bitmap.
	void DrawImage(ImageID pImageID, int x, int y);
	void DrawImage(ImageID pImageID, const PixelCoord& pTopLeft);
	// Renders a subsquare of a bitmap.
	void DrawImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect);
	void DrawImage(ImageID pImageID, const PixelCoord& pTopLeft, const PixelRect& pSubpatchRect);
	// Renders and stretches a bitmap.
	void DrawImage(ImageID pImageID, const PixelRect& pRect);
	void DrawImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect);

	// Renders the associated alpha buffer, if there is one.
	void DrawAlphaImage(ImageID pImageID, int x, int y);
	void DrawAlphaImage(ImageID pImageID, const PixelCoord& pTopLeft);

	void SetFontManager(FontManager* pFontManager);
	FontManager* GetFontManager() const;
	virtual void SetFontSmoothness(bool pSmooth) = 0;
	int GetStringWidth(const str& pString) const;
	int GetFontHeight() const;
	int GetLineHeight() const;
	int GetTabSize() const;
	void SetTabSize(int pSize);
	virtual void PrintText(const str& pString, int x, int y) = 0;

	Lepra::uint8 FindMatchingColor(const Color& pColor);
	virtual void ReadPixels(Canvas& pDestCanvas, const PixelRect& pRect) = 0;

	// Returns the internal RGB order.
	virtual RGBOrder GetRGBOrder() const = 0;

	//
	// Display list creation functions. 
	//
	DisplayListID NewDisplayList();
	void DeleteDisplayList(DisplayListID pDisplayListID);

	void BeginDisplayList(DisplayListID pDisplayListID);
	void EndDisplayList();

	void RenderDisplayList(DisplayListID pDisplayListID);

	virtual void ClearFontBuffers();

protected:
	class DisplayEntity
	{
	public:
		friend class Painter;
		RenderMode GetRenderMode() const;
		Lepra::uint8 GetAlpha() const;
		ImageID GetImageID() const;
		const PixelRect& GetClippingRect() const;
		Geometry2D& GetGeometry();
	private:
		DisplayEntity(RenderMode pRM,
		              Lepra::uint8 pAlpha,
		              ImageID pImageID,
		              const PixelRect& pClippingRect,
			      unsigned pVertexFormat) :
			mRM(pRM),
			mAlpha(pAlpha),
			mImageID(pImageID),
			mClippingRect(pClippingRect),
			mGeometry(pVertexFormat)
		{
		}

		void Init(RenderMode pRM,
		          Lepra::uint8 pAlpha,
		          ImageID pImageID,
		          const PixelRect& pClippingRect,
			  unsigned pVertexFormat)
		{
			mRM = pRM;
			mAlpha = pAlpha;
			mImageID = pImageID;
			mClippingRect = pClippingRect;
			mGeometry.Init(pVertexFormat);
		}

		RenderMode mRM;
		Lepra::uint8 mAlpha;
		ImageID mImageID;
		PixelRect mClippingRect;
		Geometry2D mGeometry;
	};

	typedef std::list<unsigned> AttribList;
	typedef std::list<RenderMode> RMList;
	typedef std::list<Lepra::uint8> UCharList;
	typedef std::list<Color> ColorList;
	typedef std::list<PixelRect> RectList;

	virtual void DoDrawPixel(int x, int y) = 0;
	virtual void DoDrawLine(int pX1, int pY1, int pX2, int pY2) = 0;
	virtual void DoDrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth) = 0;
	virtual void DoFillRect(int pLeft, int pTop, int pRight, int pBottom) = 0;
	virtual void DoDraw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken) = 0;
	virtual void DoFillShadedRect(int pLeft, int pTop, int pRight, int pBottom) = 0;
	virtual void DoFillTriangle(float pX1, float pY1,
				    float pX2, float pY2,
				    float pX3, float pY3) = 0;
	virtual void DoFillShadedTriangle(float pX1, float pY1,
					  float pX2, float pY2,
					  float pX3, float pY3) = 0;
	virtual void DoFillTriangle(float pX1, float pY1, float pU1, float pV1,
				    float pX2, float pY2, float pU2, float pV2,
				    float pX3, float pY3, float pU3, float pV3,
				    ImageID pImageID) = 0;
	virtual void DoDrawImage(ImageID pImageID, int x, int y) = 0;
	virtual void DoDrawImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect) = 0;
	virtual void DoDrawImage(ImageID pImageID, const PixelRect& pRect) = 0;
	virtual void DoDrawImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect) = 0;
	virtual void DoDrawAlphaImage(ImageID pImageID, int x, int y) = 0;

	virtual void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight) const = 0;

	virtual void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList) = 0;

	// Returns pVertexFormat | whatever-flags-the-painter-needs. The flags
	// are defined in Geometry2D, and the default behaviour is to do nothing.
	virtual void AdjustVertexFormat(unsigned& pVertexFormat);

	// This function will return the geometry of either a newly create DisplayEntity 
	// or the current (last) one in the current display list, depending on wether the 
	// last entity matches the given parameters. The newly created DisplayEntity will 
	// be  appended at the end of the display list.
	Geometry2D* FetchDisplayEntity(unsigned pVertexFormat = 0, ImageID pImageID = INVALID_IMAGEID);

	void CreateLine(int pX1, int pY1, int pX2, int pY2);
	void CreateRectFrame(int pLeft, int pTop, int pRight, int pBottom, int pWidth);
	void Create3DRectFrame(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken);
	void CreateRect(int pLeft, int pTop, int pRight, int pBottom);
	void CreateShadedRect(int pLeft, int pTop, int pRight, int pBottom);

	// Equivalent to FillTriangle().
	void CreateTriangle(float pX1, float pY1,
	                    float pX2, float pY2,
	                    float pX3, float pY3);
	// Equivalent to FillShadedTriangle().
	void CreateShadedTriangle(float pX1, float pY1,
	                          float pX2, float pY2,
	                          float pX3, float pY3);
	// Triangle with a texture.
	void CreateTriangle(float pX1, float pY1, float pU1, float pV1,
	                    float pX2, float pY2, float pU2, float pV2,
	                    float pX3, float pY3, float pU3, float pV3,
	                    ImageID pImageID);

	void CreateImage(ImageID pImageID, int x, int y);
	void CreateImage(ImageID pImageID, int x, int y, const PixelRect& pSubpatchRect);
	void CreateImage(ImageID pImageID, const PixelRect& pRect);
	void CreateImage(ImageID pImageID, const PixelRect& pRect, const PixelRect& pSubpatchRect);

	static bool IsPowerOf2(unsigned pNumber);
	static unsigned GetClosestPowerOf2(unsigned pNumber, bool pGreater = false);
	static unsigned GetExponent(unsigned pPowerOf2);

	// Coordinate convertion...
	void ToScreenCoords(int& x, int& y) const;
	void ToUserCoords(int& x, int& y) const;
	void ToScreenCoords(float& x, float& y) const;
	void ToUserCoords(float& x, float& y) const;

	// The coordinate to the following functions are given in user coordinate space.
	bool XLT(int x1, int x2); // True if x1 is left of x2.
	bool XLE(int x1, int x2); // True if x1 is left of or equal to x2.
	bool XGT(int x1, int x2); // True if x1 is right of x2.
	bool XGE(int x1, int x2); // True if x1 is right of or equal to x2.
	bool YLT(int y1, int y2); // True if y1 is above y2.
	bool YLE(int y1, int y2); // True if y1 is above or equal to y2.
	bool YGT(int y1, int y2); // True if y1 is below y2.
	bool YGE(int y1, int y2); // True if y1 is below or equal to y2.

	// Internal access to private members.
	void GetScreenCoordClippingRect(PixelRect& pClippingRect) const;

	Color& GetColorInternal(int pColorIndex);

private:
	typedef std::hash_map<unsigned, std::vector<DisplayEntity*>* > DisplayListMap;

	FontManager* mFontManager;

	DisplayListMap mDisplayListMap;
	IdManager<int> mDisplayListIDManager;
	std::vector<DisplayEntity*>* mCurrentDisplayList;
	std::vector<DisplayEntity*>::iterator mDisplayListIter;
	DisplayListID mDefaultDisplayList;

	RenderMode mRenderMode;

	AttribList mAttribStack;
	RMList     mAttribRMStack;
	UCharList  mAttribAlphaStack;
	ColorList  mAttribColor0Stack;
	ColorList  mAttribColor1Stack;
	ColorList  mAttribColor2Stack;
	ColorList  mAttribColor3Stack;
	RectList   mAttribClipRectStack;

	int mOrigoX;
	int mOrigoY;
	XDir mXDir;
	YDir mYDir;

	Canvas* mCanvas;
	PixelRect mClippingRect;

	Color mColor[4];
	Lepra::uint8 mAlphaValue;

	int mTabSize;
};



}
