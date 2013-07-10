/*
	Class:  Canvas
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	This class represents a plain uncompressed image buffer.
	(TODO: Rename to Image?)
	
	It is intended to be used in combination with a software renderer
	(2D as well as 3D) in order to manipulate images.

	The Canvas-class provides some basic functionality like resizing,
	cropping, flipping and rotating images. It also supports the five
	most common pixel formats (8, 15, 16, 24 and 32-bit). There are
	two additional pixel formats (16 and 32 bits per channel), which
	should be used in applications used by proffesional photographers.

	In combination with hardware accelerated 3D-graphics, this class
	may be used to represent the screen area (but without any actual
	pixel data - the "image buffer") or textures.

	Textures in hardware accelerated graphics usually require mip maps.
	These can be generated using the Resize() function, which will
	produce high quality mip maps in RESIZE_NICEST-mode. This function
	is slow though, and should only be used once to produce the mip
	map images. Store them on disc for later use.
*/

#pragma once

#include "LepraTypes.h"
#include "Graphics2D.h"



namespace Lepra
{



class ColorOctreeNode;	// Declared further down...
class ColorOctreeNodeListNode;

class Canvas
{
public:

	friend void Init();
	friend void Shutdown();

	enum BitDepth
	{
		BITDEPTH_8_BIT = 0,
		BITDEPTH_15_BIT,
		BITDEPTH_16_BIT,
		BITDEPTH_24_BIT,
		BITDEPTH_32_BIT,
		BITDEPTH_16_BIT_PER_CHANNEL,
		BITDEPTH_32_BIT_PER_CHANNEL, // 32 bit floating point.
	};

	enum ResizeHint
	{
		RESIZE_FAST = 0,
		RESIZE_NICEST,
	};

	enum CmpFunc
	{
		CMP_ALPHA_GREATER = 0,
		CMP_ALPHA_GREATER_OR_EQUAL,
		CMP_ALPHA_LESS,
		CMP_ALPHA_LESS_OR_EQUAL,
		CMP_ALPHA_EQUAL,

		CMP_RED_GREATER,
		CMP_RED_GREATER_OR_EQUAL,
		CMP_RED_LESS,
		CMP_RED_LESS_OR_EQUAL,
		CMP_RED_EQUAL,

		CMP_GREEN_GREATER,
		CMP_GREEN_GREATER_OR_EQUAL,
		CMP_GREEN_LESS,
		CMP_GREEN_LESS_OR_EQUAL,
		CMP_GREEN_EQUAL,

		CMP_BLUE_GREATER,
		CMP_BLUE_GREATER_OR_EQUAL,
		CMP_BLUE_LESS,
		CMP_BLUE_LESS_OR_EQUAL,
		CMP_BLUE_EQUAL,

		// Converts the (r, g, b) channels to gray before comparing.
		CMP_GRAY_GREATER,
		CMP_GRAY_GREATER_OR_EQUAL,
		CMP_GRAY_LESS,
		CMP_GRAY_LESS_OR_EQUAL,
		CMP_GRAY_EQUAL,

		CMP_ALWAYS_TRUE,
	};

	Canvas();
	Canvas(unsigned pWidth, unsigned pHeight, BitDepth pBitDepth);
	Canvas(const Canvas& pCanvas, bool pCopy = false);
	virtual ~Canvas();

	void Reset(unsigned pWidth, unsigned pHeight, BitDepth pBitDepth);
	void Reset(const Canvas& pCanvas);

	void Copy(const Canvas& pCanvas);
	void PartialCopy(int pX, int pY, const Canvas& pCanvas);
	void operator= (const Canvas& pCanvas);

	void SetBuffer(void* pBuffer, bool pCopy = false, bool pTakeOwnership = false);
	void CreateBuffer();
	void* GetBuffer() const;

	static BitDepth IntToBitDepth(unsigned pBitDepth);
	static unsigned BitDepthToInt(BitDepth pBitDepth);

	BitDepth GetBitDepth() const;
	unsigned GetWidth() const;
	unsigned GetHeight() const;
	unsigned GetPixelByteSize() const;	// Returns the pixel size in bytes.
	unsigned GetBufferByteSize() const;

	// Returns the closest higher power of two of the width and height respectively.
	// Use these to resize the canvas to dimensions compatible with graphics hardware.
	unsigned GetPow2Width();
	unsigned GetPow2Height();

	// Sometimes you have to have some fun naming your functions... :)
	// Returns the closest higher power of 2 of pValue. Returns pValue
	// if it's already a power of 2.
	static unsigned PowerUp(unsigned pValue);

	void SetPitch(unsigned pPitch);	// Set the actual width of the memorybuffer, in pixels.
	unsigned GetPitch() const;

	const Color* GetPalette() const;
	void SetPalette(const Color* pPalette);
	void SetGrayscalePalette();

	void ConvertBitDepth(BitDepth pBitDepth);
	void ConvertToGrayscale(bool pConvertTo8Bit = true);

	// Converts this canvas to 32-bit, and sets the alpha channel.
	// The alpha buffer must be 8-bit, and have the same dimensions as this canvas.
	void ConvertTo32BitWithAlpha(const Canvas& pAlphaBuffer);

	// Copy the alpha channel into pAlphaBuffer. This canvas must be 32-bit.
	void GetAlphaChannel(Canvas& pAlphaBuffer) const;

	// Generates an alpha channel from an 8-bit palettized image. The transparent 
	// colors are given in the array, which must contain pNumColors indices.
	void GetAlphaChannel(Canvas& pAlpha, uint8* pTransparentColors, unsigned pNumColors);

	// Swaps between RGB and BGR, or between RGBA and BGRA.
	void SwapRGBOrder();

	// Rotates the channels pSteps steps to the right. If a negative value is given,
	// the channels will be rotated to the left. E.g. if pSteps = 1, RGBA becomes ARGB.
	// In 8-bit color mode, all 4 channels in the palette will be rotated.
	void RotateChannels(int pSteps);

	void GetPixelColor(unsigned x, unsigned y, Color& pColor) const;
	void SetPixelColor(unsigned x, unsigned y, Color& pColor);
	Color GetPixelColor(unsigned x, unsigned y) const;

	// Set all bytes to zero.
	void Clear();

	// Only works on 32 bit images. Compares the pixel value (which channel depends on pFunc) 
	// on each pixel against pCompareValue using the function specified by pFunc. If the 
	// function returns true, the alpha value is set to the value of pTrue. If the function 
	// returns false, the alpha value is set to the value of pFalse.
	void SetAlpha(uint8 pTrue, uint8 pFalse, uint8 pCompareValue, CmpFunc pFunc);

	// This function will cut out a rect of the image, and discard everything outside this 
	// rect. If the given rect exceeds the dimensions of the image, the area "outside" will
	// be filled with zeros (black). If the rect is set to (0, 0, width, height), where 'width' 
	// and 'height' equals the dimensions of the current image, an internal buffer copy will
	// be performed.
	void Crop(int pTopLeftX,
		  int pTopLeftY,
		  int pBottomRightX,
		  int pBottomRightY);

	void FlipHorizontal();
	void FlipVertical();

	void Rotate90DegClockWise(int pSteps);
	
	void Resize(unsigned pNewWidth, unsigned pNewHeight, ResizeHint pResizeHint = RESIZE_FAST);

	// Only works if the canvas has a 32-bit bitrate and buffer responsibility. 
	// Premultiplies the alpha channel with the rgb color values.
	void PremultiplyAlpha();

	int GetOutputRotation() const;
	void SetOutputRotation(int pRotation);
	unsigned GetActualWidth() const;
	unsigned GetActualHeight() const;

private:

	void* CreateBuffer(unsigned pWidth, unsigned pHeight, BitDepth pBitDepth, unsigned& pPixelSize);

	void Resize8BitFast(unsigned pNewWidth, unsigned pNewHeight);
	void Resize16BitFast(unsigned pNewWidth, unsigned pNewHeight);
	void Resize24BitFast(unsigned pNewWidth, unsigned pNewHeight);
	void Resize32BitFast(unsigned pNewWidth, unsigned pNewHeight);
	void Resize48BitFast(unsigned pNewWidth, unsigned pNewHeight);
	void Resize96BitFast(unsigned pNewWidth, unsigned pNewHeight);

	void Resize8BitSmooth(unsigned pNewWidth, unsigned pNewHeight);
	void Resize15BitSmooth(unsigned pNewWidth, unsigned pNewHeight);
	void Resize16BitSmooth(unsigned pNewWidth, unsigned pNewHeight);
	void Resize24BitSmooth(unsigned pNewWidth, unsigned pNewHeight);
	void Resize32BitSmooth(unsigned pNewWidth, unsigned pNewHeight);
	void Resize48BitSmooth(unsigned pNewWidth, unsigned pNewHeight);
	void Resize96BitSmooth(unsigned pNewWidth, unsigned pNewHeight);

	int GenerateOctreeAndList(ColorOctreeNode* pOctTopNode, ColorOctreeNodeListNode** pFirstLink);
	void SortLinkedList(ColorOctreeNodeListNode** pFirstLink, int pNumLinks);
	void CrunchOctree(ColorOctreeNode* pOctTopNode, ColorOctreeNodeListNode** pFirstLink);
//	uint8 Canvas::GetPaletteColor(int pRed, int pGreen, int pBlue) 


	void ConvertTo8();
	void ConvertTo15();
	void ConvertTo16();
	void ConvertTo24();
	void ConvertTo32();
	void ConvertTo48();
	void ConvertTo96();
	void ConvertTo8BitGrayscale();
	void ConvertToGray();

	void SwapRGB8Bit();
	void SwapRGB15Bit();
	void SwapRGB16Bit();
	void SwapRGB24Bit();
	void SwapRGB32Bit();
	void SwapRGB48Bit();
	void SwapRGB96Bit();

	void RotateChannels8Bit(int pSteps);
	void RotateChannels15Bit(int pSteps);
	void RotateChannels16Bit(int pSteps);
	void RotateChannels24Bit(int pSteps);
	void RotateChannels32Bit(int pSteps);
	void RotateChannels48Bit(int pSteps);
	void RotateChannels96Bit(int pSteps);

	bool Compare(CmpFunc pCmpFunc, uint8 pValue1, uint8 pValue2);

	bool mBufferResponsibility;

	BitDepth mBitDepth;

	unsigned mWidth;
	unsigned mHeight;
	unsigned mPitch;
	unsigned mPixelSize;
	int mOutputRotation;

	void* mBuffer;

	Color mPalette[256];

	static void InitSamplingGrid();

	// Resize data.
	static bool smResizeDataInitialized;

	static const int smSamplingGridSize;
	static const float32 smSamplingGridPointSpace;	// Distance between two adjacent grid points.
	static const float32 smResizeSharpness;

	static float32 smSamplingGridWeight[];
	static float32 smSamplingGridCoordLookUp[];
};



class ColorOctreeNode
{
public:
	ColorOctreeNode();
	~ColorOctreeNode();

	ColorOctreeNode* InsertColor(uint8 r, 
										uint8 g, 
										uint8 b,
										int pLevel = 0);

	bool InsertUniqueColor(uint8 r, 
								  uint8 g, 
								  uint8 b,
								  int pLevel = 0);

	bool RemoveColor(uint8 r, uint8 g, uint8 b);

	inline int GetLevel(){return mLevel;}
	inline int GetReferenceCount(){return mReferenceCount;}
	inline ColorOctreeNode* GetParent(){return mParent;}
	inline int GetNumChildren(){return mNumChildren;}
	
	int GetNumLevelXChildren(int pLevel);
	int CrunchChildren();

	int GetPalette(Color* pPalette, int pIndex);
	int GetPaletteEntry(uint8 r, uint8 g, uint8 b);

	bool mTag;

private:

	uint8 mRed;
	uint8 mGreen;
	uint8 mBlue;
	uint8 mPaletteIndex;


	int mReferenceCount;
	int mLevel;
	int mNumChildren;

	ColorOctreeNode* mChildren[8];
	ColorOctreeNode* mParent;

	int GetChildOffset(uint8 r, uint8 g, uint8 b, int pLevel);
};


class ColorOctreeNodeListNode
{
public:
	ColorOctreeNodeListNode();

	ColorOctreeNode* mColorOctreeNode;
	ColorOctreeNodeListNode* mNext;
};



}
