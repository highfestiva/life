/*
	Class:  Painter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	The Painter is the base class, interface or API to render 2D graphics.
	
	Usage:
	1. Create an instance and call DefineCoordinates() in order to define
	   the coordinate system.
	2. Call SetDestCanvas() to set the destination buffer which represents
	   the surface to which the Painter should draw.
	3. Upload the images to use to the Painter through AddImage().
	3. Configure the state of the Painter using the functions:
		* SetRenderMode()
		* SetAlphaValue()
		* Reset/Reduce/SetClippingRect()
		* SetColor()
	4. Render some graphics by calling the various Draw- or Fill-functions.
	5. Print text using the function PrintText().
	6. For better performance on hardware accelerated Painters, you may create
	   a static display list using the various Create-functions.
*/

#ifndef UIPAINTER_H
#define UIPAINTER_H

#include "../../Lepra/Include/Graphics2D.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/IdManager.h"
#include "UiTBC.h"
#include "UiGeometry2D.h"
#include <list>

namespace UiTbc
{

class SystemPainter;

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

	enum FontID
	{
		INVALID_FONTID = 0,
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
	inline int GetOrigoX();
	inline int GetOrigoY();
	inline XDir GetXDir();
	inline YDir GetYDir();

	// Set the drawing surface.
	virtual void SetDestCanvas(Lepra::Canvas* pCanvas);
	inline Lepra::Canvas* GetCanvas();

	// Returns false only if stack owerflow/underflow.
	bool PushAttrib(unsigned pAttrib);
	bool PopAttrib();

	virtual void SetRenderMode(RenderMode pRM);
	inline RenderMode GetRenderMode();
	
	// Set the current alpha value. Overrides alpha buffer...
	// In 8-bit color mode, this is a specific color that will "be" transparent.
	virtual void SetAlphaValue(Lepra::uint8 pAlpha);
	inline Lepra::uint8 GetAlphaValue();

	virtual void ResetClippingRect() = 0;
	virtual void SetClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	inline  void SetClippingRect(const Lepra::PixelRect& pClippingRect);
	        void ReduceClippingRect(int pLeft, int pTop, int pRight, int pBottom);
	inline  void ReduceClippingRect(const Lepra::PixelRect& pClippingRect);
	inline  void GetClippingRect(Lepra::PixelRect& pClippingRect);

	virtual void SetColor(const Lepra::Color& pColor, unsigned pColorIndex = 0);
	inline  void SetColor(Lepra::uint8 pRed, Lepra::uint8 pGreen, Lepra::uint8 pBlue, Lepra::uint8 pPaletteIndex, unsigned pColorIndex = 0);
	inline  Lepra::Color GetColor(unsigned pColorIndex);

	inline  void DrawPixel(int x, int y);
	inline  void DrawPixel(const Lepra::PixelCoords& pCoords);

	inline  void DrawLine(int pX1, int pY1, int pX2, int pY2);
	inline  void DrawLine(const Lepra::PixelCoords& pPoint1, const Lepra::PixelCoords& pPoint2);

	// Renders a rectangular frame. The given coordinates defines the outer edges of the frame,
	// expanding inwards 'pWidth' pixels.
	// Color 0 defines the outer color.
	// Color 1 defines the inner color.
	inline  void DrawRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth);
	inline  void DrawRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight, int pWidth);
	inline  void DrawRect(const Lepra::PixelRect& pRect, int pWidth);

	inline  void FillRect(int pLeft, int pTop, int pRight, int pBottom);
	inline  void FillRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight);
	inline  void FillRect(const Lepra::PixelRect& pRect);

	// Same as DrawRect(), but with two different color gradients. Good for GUI rendering.
	// Color 0 and 1 are the outer top left and bottom right colors, respectively.
	// Color 2 and 3 are the inner ditos.
	inline  void Draw3DRect(int pLeft, int pTop, int pRight, int pBottom, int pWidth, bool pSunken);
	inline  void Draw3DRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight, int pWidth, bool pSunken);
	inline  void Draw3DRect(const Lepra::PixelRect& pRect, int pWidth, bool pSunken);

	// FillShadeRect uses all colors...
	inline  void FillShadedRect(int pLeft, int pTop, int pRight, int pBottom);
	inline  void FillShadedRect(const Lepra::PixelCoords& pTopLeft, const Lepra::PixelCoords& pBottomRight);
	inline  void FillShadedRect(const Lepra::PixelRect& pRect);

	inline  void FillTriangle(float pX1, float pY1,
				  float pX2, float pY2,
				  float pX3, float pY3);
	inline  void FillTriangle(const Lepra::PixelCoords& pPoint1,
				  const Lepra::PixelCoords& pPoint2,
				  const Lepra::PixelCoords& pPoint3);

	inline  void FillShadedTriangle(float pX1, float pY1,
					float pX2, float pY2,
					float pX3, float pY3);
	inline  void FillShadedTriangle(const Lepra::PixelCoords& pPoint1,
					const Lepra::PixelCoords& pPoint2,
					const Lepra::PixelCoords& pPoint3);

	inline  void FillTriangle(float pX1, float pY1, float pU1, float pV1,
				  float pX2, float pY2, float pU2, float pV2,
				  float pX3, float pY3, float pU3, float pV3,
				  ImageID pImageID);
	inline  void FillTriangle(const Lepra::PixelCoords& pPoint1, float pU1, float pV1,
				  const Lepra::PixelCoords& pPoint2, float pU2, float pV2,
				  const Lepra::PixelCoords& pPoint3, float pU3, float pV3,
				  ImageID pImageID);


	// Returns an ID to the added bitmap. Returns 0 if error. 
	// Alpha buffer or bitmap may be NULL.
	virtual ImageID AddImage(const Lepra::Canvas* pImage, const Lepra::Canvas* pAlphaBuffer) = 0;

	// If pHint == UPDATE_ACCURATE, this function replaces the contents of pImageID with the 
	// contents of pImage.
	// If pHint == UPDATE_FAST, the behaviour depends on the implementation. In general,
	// pImage and pAlphaBuffer must have the same dimensions and pixel format as the current
	// image.
	virtual void UpdateImage(ImageID pImageID, 
				 const Lepra::Canvas* pImage, 
				 const Lepra::Canvas* pAlphaBuffer,
				 UpdateHint pHint = UPDATE_ACCURATE) = 0;
	virtual void RemoveImage(ImageID pImageID) = 0;

	// Renders a bitmap.
	inline  void DrawImage(ImageID pImageID, int x, int y);
	inline  void DrawImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft);
	// Renders a subsquare of a bitmap.
	inline  void DrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect);
	inline  void DrawImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft, const Lepra::PixelRect& pSubpatchRect);
	// Renders and stretches a bitmap.
	inline  void DrawImage(ImageID pImageID, const Lepra::PixelRect& pRect);
	inline  void DrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect);

	// Renders the associated alpha buffer, if there is one.
	inline  void DrawAlphaImage(ImageID pImageID, int x, int y);
	inline  void DrawAlphaImage(ImageID pImageID, const Lepra::PixelCoords& pTopLeft);

	inline  void DrawDefaultMouseCursor(int x, int y);

	/*
		Image Font Functions.
	*/

	void SetFontPainter(SystemPainter* pFontPainter);
	SystemPainter* GetFontPainter() const;

	//
	// Param 1: pFont - the image that contains all characters, numbers e.t.c.
	// Param 2: pTileWidth  - the width of one tile which contains one character.
	// Param 3: pTileHeight - the height of one tile...
	// Param 4: pCharWidth  - the width of the characters when displayed.
	// Param 5: pCharHeight - the height of the characters when displayed (height of one line).
	// Param 6: pFirstChar  - the first character code (in ascii or unicode) defined by
	//                          the font.
	// Param 7: pLastChar   - the last character code (in ascii or unicode) defined by
	//                          the font.
	FontID AddImageFont(const Lepra::Canvas& pFont,
			    int pTileWidth,
			    int pTileHeight,
			    int pCharWidth,
			    int pCharHeight,
			    int pFirstChar,
			    int pLastChar,
			    int pDefaultSpaceWidth = 0,
			    int pNewLineOffset = 0,
			    int pCharOffset = 0,
			    int pTabWidth = 32);

	// Almost same as above, but each character may have a unique width.
	// pCharWidth contains pLastChar - pFirstChar + 1 entries.
	FontID AddImageFont(const Lepra::Canvas& pFont, 
			    int pTileWidth,
			    int pTileHeight,
			    int* pCharWidth,
			    int pCharHeight,
			    int pFirstChar,
			    int pLastChar,
			    int pDefaultSpaceWidth = 0,
			    int pNewLineOffset = 0,
			    int pCharOffset = 0,
			    int pTabWidth = 32);

	virtual void SetActiveFont(FontID pFontID);
	FontID GetStandardFont(int pFontIndex);
	FontID GetCurrentFont();

	inline void SetTabOriginX(int pTabOriginX);

	// Returns the width of the rendered string in pixels.
	virtual int GetCharWidth(const Lepra::tchar pChar);
	virtual int GetStringWidth(const Lepra::String& pString);
	virtual int GetFontHeight();
	virtual int GetLineHeight();

	// Draws a null-terminated C-string at coordinates (x, y). If the method encounters a '\n',
	// the text followed after that will be drawn at (x, y + CharHeight).
	// The return value is the x-coordinate where the next character should be written.
	int PrintText(const Lepra::String& pString, int x, int y);

	Lepra::uint8 FindMatchingColor(const Lepra::Color& pColor);
	virtual void ReadPixels(Lepra::Canvas& pDestCanvas, const Lepra::PixelRect& pRect) = 0;

	// Returns the internal RGB order.
	virtual RGBOrder GetRGBOrder() = 0;

	//
	// Display list creation functions. 
	//
	DisplayListID NewDisplayList();
	void DeleteDisplayList(DisplayListID pDisplayListID);

	void BeginDisplayList(DisplayListID pDisplayListID);
	void EndDisplayList();

	void RenderDisplayList(DisplayListID pDisplayListID);
protected:
	class DisplayEntity
	{
	public:
		friend class Painter;
		inline RenderMode GetRenderMode() const;
		inline Lepra::uint8 GetAlpha() const;
		inline ImageID GetImageID() const;
		inline const Lepra::PixelRect& GetClippingRect() const;
		inline Geometry2D& GetGeometry();
	private:
		DisplayEntity(RenderMode pRM,
		              Lepra::uint8 pAlpha,
		              ImageID pImageID,
		              const Lepra::PixelRect& pClippingRect,
			      Lepra::uint16 pVertexFormat) :
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
		          const Lepra::PixelRect& pClippingRect,
			  Lepra::uint16 pVertexFormat)
		{
			mRM = pRM;
			mAlpha = pAlpha;
			mImageID = pImageID;
			mClippingRect = pClippingRect;
			mGeometry = pVertexFormat;
		}

		RenderMode mRM;
		Lepra::uint8 mAlpha;
		ImageID mImageID;
		Lepra::PixelRect mClippingRect;
		Geometry2D mGeometry;
	};

	// The following class represents the base class of all image fonts.
	// It is however used as the base class for system fonts as well (see 
	// SystemPainter and GDIPainter) in order to avoid duplicated handling
	// of fonts. 
	class Font
	{
	public:
		Font(int pFirstChar, int pLastChar) :
			mTextureID(0),
			mTileWidth(0),
			mTileHeight(0),
			mCharWidth(new int[pLastChar - pFirstChar + 1]),
			mCharHeight(0),
			mFirstChar(pFirstChar),
			mLastChar(pLastChar),
			mDefaultSpaceWidth(0),
			mNewLineOffset(0),
			mCharOffset(0),
			mTabWidth(0),
			mAlphaImage(false)
		{
			::memset(mCharWidth, 0, (pLastChar - pFirstChar + 1) * sizeof(int));
		}

		virtual ~Font()
		{
			delete[] mCharWidth;
		}

		// Only makes sense in image fonts.
		virtual void GetUVRect(const Lepra::tchar& pChar, float& pU1, float& pV1, float& pU2, float& pV2) const = 0;
		virtual bool IsSystemFont();

		int mTextureID;
		int mTileWidth;
		int mTileHeight;
		int* mCharWidth;
		int mCharHeight;
		int mFirstChar;
		int mLastChar;

		// Misc settings.
		int mDefaultSpaceWidth;
		int mNewLineOffset;
		int mCharOffset;
		int mTabWidth;

		FontID mFontID;
		bool mAlphaImage;
	};

	typedef std::list<unsigned> AttribList;
	typedef std::list<RenderMode> RMList;
	typedef std::list<Lepra::uint8> UCharList;
	typedef std::list<Lepra::Color> ColorList;
	typedef std::list<Lepra::PixelRect> RectList;
	typedef Lepra::HashTable<Lepra::String, ImageID> FilenameToImageTable;
	typedef Lepra::HashTable<int, Font*> FontTable;

	struct GlyphInfo
	{
		GlyphInfo(ImageID pImageId, Lepra::PixelRect& pRect):
			mImageId(pImageId),
			mRect(pRect)
		{
		}
		ImageID mImageId;
		Lepra::PixelRect mRect;
	};
	bool CacheCharImage(Lepra::tchar pChar, ImageID& pImageId, Lepra::PixelRect& pCharSquare);
	void DeleteCharCache();

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
	virtual void DoDrawImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect) = 0;
	virtual void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect) = 0;
	virtual void DoDrawImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect) = 0;
	virtual void DoDrawAlphaImage(ImageID pImageID, int x, int y) = 0;

	virtual Font* NewFont(int pFirstChar, int pLastChar) const = 0;
	virtual void InitFont(Font* pFont, const Lepra::Canvas& pFontImage) = 0;
	virtual void GetImageSize(ImageID pImageID, int& pWidth, int& pHeight) = 0;
	virtual int  DoPrintText(const Lepra::String& pString, int x, int y) = 0;

	virtual void DoRenderDisplayList(std::vector<DisplayEntity*>* pDisplayList) = 0;

	// Returns pVertexFormat | whatever-flags-the-painter-needs. The flags
	// are defined in Geometry2D, and the default behaviour is to do nothing.
	virtual void AdjustVertexFormat(Lepra::uint16& pVertexFormat);

	// This function will return the geometry of either a newly create DisplayEntity 
	// or the current (last) one in the current display list, depending on wether the 
	// last entity matches the given parameters. The newly created DisplayEntity will 
	// be  appended at the end of the display list. Note that pImageID and pFontID 
	// mustn't be valid at the same time.
	Geometry2D* FetchDisplayEntity(Lepra::uint16 pVertexFormat = 0,
		ImageID pImageID = INVALID_IMAGEID, FontID pFontID = INVALID_FONTID);

	// Only works with image fonts.
	void CreateText(const Lepra::String& pString, int x, int y);
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
	void CreateImage(ImageID pImageID, int x, int y, const Lepra::PixelRect& pSubpatchRect);
	void CreateImage(ImageID pImageID, const Lepra::PixelRect& pRect);
	void CreateImage(ImageID pImageID, const Lepra::PixelRect& pRect, const Lepra::PixelRect& pSubpatchRect);

	// Some helper functions.
	static int CalcAverageWidth(int pCharCount, int* pCharWidthArray);
	void CalcCharWidths(Lepra::uint8* pFont, int* pWidth);

	inline static bool IsPowerOf2(unsigned pNumber);
	static unsigned GetClosestPowerOf2(unsigned pNumber, bool pGreater = false);
	static unsigned GetExponent(unsigned pPowerOf2);

	inline static Lepra::uint8* GetStandardFont1();
	inline static Lepra::uint8* GetStandardFont2();
	inline static int* GetCharWidthStdFont1();
	inline static int* GetCharWidthStdFont2();
	inline static Lepra::uint8* GetStandardMouseCursor();

	// Coordinate convertion...
	inline void ToScreenCoords(int& x, int& y) const;
	inline void ToUserCoords(int& x, int& y) const;
	inline void ToScreenCoords(float& x, float& y) const;
	inline void ToUserCoords(float& x, float& y) const;

	// The coordinate to the following functions are given in user coordinate space.
	inline bool XLT(int x1, int x2); // True if x1 is left of x2.
	inline bool XLE(int x1, int x2); // True if x1 is left of or equal to x2.
	inline bool XGT(int x1, int x2); // True if x1 is right of x2.
	inline bool XGE(int x1, int x2); // True if x1 is right of or equal to x2.
	inline bool YLT(int y1, int y2); // True if y1 is above y2.
	inline bool YLE(int y1, int y2); // True if y1 is above or equal to y2.
	inline bool YGT(int y1, int y2); // True if y1 is below y2.
	inline bool YGE(int y1, int y2); // True if y1 is below or equal to y2.

	// Internal access to private members.
	void GetScreenCoordClippingRect(Lepra::PixelRect& pClippingRect) const;

	inline Font* GetCurrentFontInternal() const;
	inline int GetTabOriginX() const;
	inline Lepra::Color& GetColorInternal(int pColorIndex);

	// Used by SystemPainters that need to instantiate own font types. Will set
	// Font::mFontID.
	void AddFont(Font* pFont);
private:
	typedef std::hash_map<unsigned, GlyphInfo> GlyphMap;
	typedef std::hash_map<unsigned, std::vector<DisplayEntity*>* > DisplayListMap;

	SystemPainter* mFontPainter;
	GlyphMap mGlyphMap;

	DisplayListMap mDisplayListMap;
	Lepra::IdManager<int> mDisplayListIDManager;
	std::vector<DisplayEntity*>* mCurrentDisplayList;
	std::vector<DisplayEntity*>::iterator mDisplayListIter;

	// Two standard bitmap fonts, mStandardFont1 and mStandardFont2.
	// mStandardFont1 is actually a copy of the system font (the font used in text mode, MS-DOS).
	// mStandardFont2 is a little bit more "cool".
	static Lepra::uint8 smStandardFont1[];
	static Lepra::uint8 smStandardFont2[];
	static Lepra::uint8 smStandardMouseCursor[];

	static int smFont1CharWidth[256];
	static int smFont2CharWidth[256];

	static bool smCharWidthsCalculated;

	int mTabOriginX;

	bool m8BitColorSearchMode;
	bool mFontsAndCursorInitialized;

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

	Lepra::IdManager<int> mFontIDManager;
	FontTable    mFontTable;
	Font*        mCurrentFont;

	FontID mStandardFontID0;
	FontID mStandardFontID1;
	ImageID mMouseCursorID;

	Lepra::Canvas* mCanvas;
	Lepra::PixelRect mClippingRect;

	Lepra::Color mColor[4];
	Lepra::uint8 mAlphaValue;
};

#include "../Source/UiPainter.inl"

} // End namespace.

#endif
