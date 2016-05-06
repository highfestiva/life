
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// NOTES:
//
// This class can only render bitmaps with dimensions of a power of 2.
// The openGL texture "names" (or IDs) between 1-10000 are reserved to bitmap rendering.
// 1 is reserved for text, and 2 is reserved for the default mouse cursor.



#pragma once

#include "uitbc.h"
#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/vector3d.h"
#include "../../uilepra/include/uiopenglextensions.h"	// Included to get the gl-headers.
#include "uipainter.h"
#include <list>



namespace uitbc {



class FontTexture;



class OpenGLPainter: public Painter {
public:
	OpenGLPainter();
	virtual ~OpenGLPainter();

	// Set the drawing surface.
	void SetDestCanvas(Canvas* canvas);
	void SetRenderMode(RenderMode rm);
	virtual void Clear(const Color& color);
	virtual void PrePaint(bool clear_depth_buffer);

	// Set the current alpha value. Overrides alpha buffer...
	// In 8-bit color mode, this is a specific color that will "be" transparent.
	void SetAlphaValue(lepra::uint8 alpha);

	void SetClippingRect(int left, int top, int right, int bottom);
	void ResetClippingRect();

	void SetColor(const Color& color, unsigned color_index = 0);
	virtual void SetLineWidth(float pixels);

	ImageID AddImage(const Canvas* image, const Canvas* alpha_buffer);
	void UpdateImage(ImageID image_id,
			 const Canvas* image,
			 const Canvas* alpha_buffer,
			 UpdateHint hint = kUpdateAccurate);
	void RemoveImage(ImageID image_id);

	void ReadPixels(Canvas& dest_canvas, const PixelRect& rect);

	RGBOrder GetRGBOrder() const;

protected:
	void DoSetRenderMode() const;

	void DoDrawPixel(int x, int y);
	void DoDrawLine(int x1, int y1, int x2, int y2);
	void DoFillTriangle(float x1, float y1,
			    float x2, float y2,
			    float x3, float y3);
	void DoFillShadedTriangle(float x1, float y1,
				  float x2, float y2,
				  float x3, float y3);
	void DoFillTriangle(float x1, float y1, float u1, float v1,
			    float x2, float y2, float u2, float v2,
			    float x3, float y3, float u3, float v3,
			    ImageID image_id);
	void DoDrawRect(int left, int top, int right, int bottom);
	void DoFillRect(int left, int top, int right, int bottom);
	void DoDraw3DRect(int left, int top, int right, int bottom, int width, bool sunken);
	void DoFillShadedRect(int left, int top, int right, int bottom);
	void DrawFan(const std::vector<vec2>& coords, bool fill);
	void DrawImageFan(ImageID image_id, const std::vector<vec2>& coords, const std::vector<vec2>& tex_coords);
	void DoDrawImage(ImageID image_id, int x, int y);
	void DoDrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect);
	void DoDrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect);
	void DoDrawImage(ImageID image_id, const PixelRect& rect);
	void DoDrawAlphaImage(ImageID image_id, int x, int y);

	void GetImageSize(ImageID image_id, int& width, int& height) const;
	void PrintText(const wstr& s, int x, int y);
	void SetFontSmoothness(bool smooth);

	virtual void AdjustVertexFormat(unsigned& vertex_format);
	void DoRenderDisplayList(std::vector<DisplayEntity*>* display_list);

	virtual void ClearFontBuffers();
	FontTexture* SelectGlyphs(uint32 font_hash, int font_height, const wstr& s);

private:
	vec3 r_col_[4];

	class Texture {
	public:
		Texture() :
			width_(0),
			height_(0) {
		}

		int width_;
		int height_;
	};

	typedef HashTable<int, Texture*> TextureTable;
	typedef std::unordered_map<uint32, FontTexture*> FontTextureTable;

	IdManager<int> texture_id_manager_;
	TextureTable texture_table_;
	FontTextureTable font_texture_table_;

	bool smooth_font_;
};



}
