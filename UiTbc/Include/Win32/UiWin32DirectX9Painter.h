/*
	Class:  DirectX9Painter
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

*/



#pragma once

#include <d3d9.h>
#include <list>
#include "../uitbc.h"
#include "../../../lepra/include/graphics2d.h"
#include "../../../lepra/include/canvas.h"
#include "../../../lepra/include/hashtable.h"
#include "../../../lepra/include/string.h"
#include "../../../lepra/include/idmanager.h"
#include "../uipainter.h"



namespace uilepra {
class DisplayManager;
}



namespace uitbc {



class DirectX9Painter: public Painter {
public:
	DirectX9Painter(uilepra::DisplayManager* display_manager);
	virtual ~DirectX9Painter();

	void SetDestCanvas(Canvas* canvas);
	void SetRenderMode(RenderMode rm);
	virtual void Clear(const Color& color);
	virtual void PrePaint(bool clear_depth_buffer);

	void SetAlphaValue(uint8 alpha);

	void ResetClippingRect();
	void SetClippingRect(int left, int top, int right, int bottom);

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
	void DoDrawRect(int left, int top, int right, int bottom);
	void DoFillRect(int left, int top, int right, int bottom);
	void DoDraw3DRect(int left, int top, int right, int bottom, int width, bool sunken);
	void DoFillShadedRect(int left, int top, int right, int bottom);
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
	void DrawFan(const std::vector<vec2>& coords, bool fill);
	void DoDrawImage(ImageID image_id, int x, int y);
	void DoDrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect);
	void DoDrawImage(ImageID image_id, const PixelRect& rect);
	void DoDrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect);
	void DoDrawAlphaImage(ImageID image_id, int x, int y);

	void DoRenderDisplayList(std::vector<DisplayEntity*>* display_list);
private:
	class Texture {
	public:
		Texture() :
			width_(0),
			height_(0),
			d3_d_texture_(0) {
		}

		int width_;
		int height_;
		IDirect3DTexture9* d3_d_texture_;
	};

	typedef HashTable<int, Texture*> TextureTable;

	void GetImageSize(ImageID image_id, int& width, int& height) const;
	void PrintText(const str& s, int x, int y);
	void SetFontSmoothness(bool smooth);

	void AdjustVertexFormat(uint16& vertex_format);

	IdManager<int> texture_id_manager_;
	TextureTable texture_table_;

	IDirect3DDevice9* d3_d_device_;
	IDirect3DTexture9* d3_d_default_mouse_cursor_;

	logclass();
};



}
