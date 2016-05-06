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
	produce high quality mip maps in kResizeNicest-mode. This function
	is slow though, and should only be used once to produce the mip
	map images. Store them on disc for later use.
*/

#pragma once

#include "lepratypes.h"
#include "graphics2d.h"



namespace lepra {



class ColorOctreeNode;	// Declared further down...
class ColorOctreeNodeListNode;

class Canvas {
public:

	friend void Init();
	friend void Shutdown();

	enum BitDepth {
		kBitdepth8Bit = 0,
		kBitdepth15Bit,
		kBitdepth16Bit,
		kBitdepth24Bit,
		kBitdepth32Bit,
		kBitdepth16BitPerChannel,
		kBitdepth32BitPerChannel, // 32 bit floating point.
	};

	enum ResizeHint {
		kResizeFast = 0,
		kResizeNicest,
		kResizeCanvas,	// No scaling, simply crop or expand.
	};

	enum CmpFunc {
		kCmpAlphaGreater = 0,
		kCmpAlphaGreaterOrEqual,
		kCmpAlphaLess,
		kCmpAlphaLessOrEqual,
		kCmpAlphaEqual,

		kCmpRedGreater,
		kCmpRedGreaterOrEqual,
		kCmpRedLess,
		kCmpRedLessOrEqual,
		kCmpRedEqual,

		kCmpGreenGreater,
		kCmpGreenGreaterOrEqual,
		kCmpGreenLess,
		kCmpGreenLessOrEqual,
		kCmpGreenEqual,

		kCmpBlueGreater,
		kCmpBlueGreaterOrEqual,
		kCmpBlueLess,
		kCmpBlueLessOrEqual,
		kCmpBlueEqual,

		// Converts the (r, g, b) channels to gray before comparing.
		kCmpGrayGreater,
		kCmpGrayGreaterOrEqual,
		kCmpGrayLess,
		kCmpGrayLessOrEqual,
		kCmpGrayEqual,

		kCmpAlwaysTrue,
	};

	Canvas();
	Canvas(unsigned width, unsigned height, BitDepth bit_depth);
	Canvas(const Canvas& canvas, bool copy = false);
	virtual ~Canvas();

	void Reset(unsigned width, unsigned height, BitDepth bit_depth);
	void Reset(const Canvas& canvas);

	void Copy(const Canvas& canvas);
	void PartialCopy(int _x, int _y, const Canvas& canvas);
	void operator= (const Canvas& canvas);

	void SetBuffer(void* buffer, bool copy = false, bool take_ownership = false);
	void CreateBuffer();
	void* GetBuffer() const;

	static BitDepth IntToBitDepth(unsigned bit_depth);
	static unsigned BitDepthToInt(BitDepth bit_depth);

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
	// Returns the closest higher power of 2 of value. Returns value
	// if it's already a power of 2.
	static unsigned PowerUp(unsigned value);

	void SetPitch(unsigned pitch);	// Set the actual width of the memorybuffer, in pixels.
	unsigned GetPitch() const;

	const Color* GetPalette() const;
	void SetPalette(const Color* palette);
	void SetGrayscalePalette();

	void ConvertBitDepth(BitDepth bit_depth);
	void ConvertToGrayscale(bool convert_to8_bit = true);

	// Converts this canvas to 32-bit, and sets the alpha channel.
	// The alpha buffer must be 8-bit, and have the same dimensions as this canvas.
	void ConvertTo32BitWithAlpha(const Canvas& alpha_buffer);

	// Copy the alpha channel into alpha_buffer. This canvas must be 32-bit.
	void GetAlphaChannel(Canvas& alpha_buffer) const;

	// Generates an alpha channel from an 8-bit palettized image. The transparent
	// colors are given in the array, which must contain num_colors indices.
	void GetAlphaChannel(Canvas& alpha, uint8* transparent_colors, unsigned num_colors);

	// Swaps between kRgb and kBgr, or between RGBA and BGRA.
	void SwapRGBOrder();

	// Rotates the channels steps steps to the right. If a negative value is given,
	// the channels will be rotated to the left. E.g. if steps = 1, RGBA becomes ARGB.
	// In 8-bit color mode, all 4 channels in the palette will be rotated.
	void RotateChannels(int steps);

	void GetPixelColor(unsigned x, unsigned y, Color& color) const;
	void SetPixelColor(unsigned x, unsigned y, Color& color);
	Color GetPixelColor(unsigned x, unsigned y) const;

	// Set all bytes to zero.
	void Clear();

	// Only works on 32 bit images. Compares the pixel value (which channel depends on func)
	// on each pixel against compare_value using the function specified by func. If the
	// function returns true, the alpha value is set to the value of if_true. If the function
	// returns false, the alpha value is set to the value of if_false.
	void SetAlpha(uint8 if_true, uint8 if_false, uint8 compare_value, CmpFunc func);

	// This function will cut out a rect of the image, and discard everything outside this
	// rect. If the given rect exceeds the dimensions of the image, the area "outside" will
	// be filled with zeros (black). If the rect is set to (0, 0, width, height), where 'width'
	// and 'height' equals the dimensions of the current image, an internal buffer copy will
	// be performed.
	void Crop(int top_left_x,
		  int top_left_y,
		  int bottom_right_x,
		  int bottom_right_y);

	void FlipHorizontal();
	void FlipVertical();

	void Rotate90DegClockWise(int steps);

	void Resize(unsigned new_width, unsigned new_height, ResizeHint resize_hint = kResizeFast);

	// Only works if the canvas has a 32-bit bitrate and buffer responsibility.
	// Premultiplies the alpha channel with the rgb color values.
	void PremultiplyAlpha();

	int GetOutputRotation() const;
	void SetOutputRotation(int rotation);
	int GetDeviceOutputRotation() const;
	void SetDeviceRotation(int rotation);
	unsigned GetActualWidth() const;
	unsigned GetActualHeight() const;

private:

	void* CreateBuffer(unsigned width, unsigned height, BitDepth bit_depth, unsigned& pixel_size);

	void Resize8BitFast(unsigned new_width, unsigned new_height);
	void Resize16BitFast(unsigned new_width, unsigned new_height);
	void Resize24BitFast(unsigned new_width, unsigned new_height);
	void Resize32BitFast(unsigned new_width, unsigned new_height);
	void Resize48BitFast(unsigned new_width, unsigned new_height);
	void Resize96BitFast(unsigned new_width, unsigned new_height);

	void Resize8BitSmooth(unsigned new_width, unsigned new_height);
	void Resize15BitSmooth(unsigned new_width, unsigned new_height);
	void Resize16BitSmooth(unsigned new_width, unsigned new_height);
	void Resize24BitSmooth(unsigned new_width, unsigned new_height);
	void Resize32BitSmooth(unsigned new_width, unsigned new_height);
	void Resize48BitSmooth(unsigned new_width, unsigned new_height);
	void Resize96BitSmooth(unsigned new_width, unsigned new_height);

	int GenerateOctreeAndList(ColorOctreeNode* oct_top_node, ColorOctreeNodeListNode** first_link);
	void SortLinkedList(ColorOctreeNodeListNode** first_link, int num_links);
	void CrunchOctree(ColorOctreeNode* oct_top_node, ColorOctreeNodeListNode** first_link);
//	uint8 Canvas::GetPaletteColor(int red, int green, int blue)


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

	void RotateChannels8Bit(int steps);
	void RotateChannels15Bit(int steps);
	void RotateChannels16Bit(int steps);
	void RotateChannels24Bit(int steps);
	void RotateChannels32Bit(int steps);
	void RotateChannels48Bit(int steps);
	void RotateChannels96Bit(int steps);

	bool Compare(CmpFunc cmp_func, uint8 value1, uint8 value2);

	bool buffer_responsibility_;

	BitDepth bit_depth_;

	unsigned width_;
	unsigned height_;
	unsigned pitch_;
	unsigned pixel_size_;
	int output_rotation_;
	int device_rotation_;

	void* buffer_;

	Color palette_[256];

	static void InitSamplingGrid();

	// Resize data.
	static bool resize_data_initialized_;

	static const int sampling_grid_size_;
	static const float32 sampling_grid_point_space_;	// Distance between two adjacent grid points.
	static const float32 resize_sharpness_;

	static float32 sampling_grid_weight_[];
	static float32 sampling_grid_coord_look_up_[];
};



class ColorOctreeNode {
public:
	ColorOctreeNode();
	~ColorOctreeNode();

	ColorOctreeNode* InsertColor(uint8 r,
										uint8 g,
										uint8 b,
										int level = 0);

	bool InsertUniqueColor(uint8 r,
								  uint8 g,
								  uint8 b,
								  int level = 0);

	bool RemoveColor(uint8 r, uint8 g, uint8 b);

	inline int GetLevel(){return level_;}
	inline int GetReferenceCount(){return reference_count_;}
	inline ColorOctreeNode* GetParent(){return parent_;}
	inline int GetNumChildren(){return num_children_;}

	int GetNumLevelXChildren(int level);
	int CrunchChildren();

	int GetPalette(Color* palette, int index);
	int GetPaletteEntry(uint8 r, uint8 g, uint8 b);

	bool tag_;

private:

	uint8 red_;
	uint8 green_;
	uint8 blue_;
	uint8 palette_index_;


	int reference_count_;
	int level_;
	int num_children_;

	ColorOctreeNode* children_[8];
	ColorOctreeNode* parent_;

	int GetChildOffset(uint8 r, uint8 g, uint8 b, int level);
};


class ColorOctreeNodeListNode {
public:
	ColorOctreeNodeListNode();

	ColorOctreeNode* color_octree_node_;
	ColorOctreeNodeListNode* next_;
};



}
