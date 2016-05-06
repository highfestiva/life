
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
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

#include "../../lepra/include/unordered.h"
#include <list>
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/graphics2d.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/string.h"
#include "../../lepra/include/vector2d.h"
#include "uitbc.h"
#include "uigeometry2d.h"



namespace uitbc {



class FontManager;



class Painter {
public:

	enum RenderMode {
		kRmNormal = 0,
		kRmAlphatest,
		kRmAlphablend,
		kRmXor,
		kRmAdd,
	};

	enum { // Attributes
		kAttrRendermode    = (1 << 0),
		kAttrAlphavalue    = (1 << 1),
		kAttrColor0        = (1 << 2),
		kAttrColor1        = (1 << 3),
		kAttrColor2        = (1 << 4),
		kAttrColor3        = (1 << 5),
		kAttrCliprect      = (1 << 7),

		kAttrColors        = kAttrColor0 |
		                     kAttrColor1 |
				     kAttrColor2 |
				     kAttrColor3,

		kAttrAll = kAttrRendermode    |
		           kAttrAlphavalue    |
			   kAttrColors        |
			   kAttrCliprect,
	};

	enum UpdateHint {
		kUpdateAccurate = 0,
		kUpdateFast,
	};

	// Coordinate X-axis direction.
	enum XDir {
		kXRight = 1,
		kXLeft  = -1,
	};

	enum YDir {
		kYUp   = -1,
		kYDown = 1,
	};

	enum RGBOrder {
		kRgb = 0,
		kBgr,
	};

	enum ImageID {
		kInvalidImageid = 0,
	};

	enum DisplayListID {
		kInvalidDisplaylistid = 0,
	};

	Painter();
	virtual ~Painter();

	// Defines the coordinate system. The origo coordinates are given in "standard
	// screen coordinates" - relative to the top left corner, x increasing to the right
	// and y increasing downwards.
	void DefineCoordinates(int origo_x, int origo_y, XDir x_dir, YDir y_dir);
	int GetOrigoX() const;
	int GetOrigoY() const;
	XDir GetXDir() const;
	YDir GetYDir() const;

	// Set the drawing surface.
	virtual void SetDestCanvas(Canvas* canvas);
	Canvas* GetCanvas() const;

	virtual void Clear(const Color& color) = 0;
	virtual void PrePaint(bool clear_depth_buffer) = 0;

	// Returns false only if stack owerflow/underflow.
	bool PushAttrib(unsigned attrib);
	bool PopAttrib();

	virtual void SetRenderMode(RenderMode rm);
	RenderMode GetRenderMode() const;

	// Set the current alpha value. Overrides alpha buffer...
	// In 8-bit color mode, this is a specific color that will "be" transparent.
	virtual void SetAlphaValue(lepra::uint8 alpha);
	lepra::uint8 GetAlphaValue() const;

	virtual void ResetClippingRect() = 0;
	virtual void SetClippingRect(int left, int top, int right, int bottom);
	void SetClippingRect(const PixelRect& clipping_rect);
	void ReduceClippingRect(int left, int top, int right, int bottom);
	void ReduceClippingRect(const PixelRect& clipping_rect);
	void GetClippingRect(PixelRect& clipping_rect) const;

	virtual void SetColor(const Color& color, unsigned color_index = 0);
	void SetColor(lepra::uint8 red, lepra::uint8 green, lepra::uint8 blue, lepra::uint8 palette_index, unsigned color_index = 0);
	Color GetColor(unsigned color_index) const;

	virtual void SetLineWidth(float pixels) = 0;

	void DrawPixel(int x, int y);
	void DrawPixel(const PixelCoord& coords);

	void DrawLine(int _x1, int _y1, int _x2, int _y2);
	void DrawLine(const PixelCoord& point1, const PixelCoord& point2);

	void DrawRect(const PixelRect& rect);

	void FillRect(int left, int top, int right, int bottom);
	void FillRect(const PixelCoord& top_left, const PixelCoord& bottom_right);
	void FillRect(const PixelRect& rect);

	// Same as DrawRect(), but with two different color gradients. Good for GUI rendering.
	// Color 0 and 1 are the outer top left and bottom right colors, respectively.
	// Color 2 and 3 are the inner ditos.
	void Draw3DRect(int left, int top, int right, int bottom, int width, bool sunken);
	void Draw3DRect(const PixelCoord& top_left, const PixelCoord& bottom_right, int width, bool sunken);
	void Draw3DRect(const PixelRect& rect, int width, bool sunken);

	// FillShadeRect uses all colors...
	void FillShadedRect(int left, int top, int right, int bottom);
	void FillShadedRect(const PixelCoord& top_left, const PixelCoord& bottom_right);
	void FillShadedRect(const PixelRect& rect);

	void FillTriangle(float _x1, float _y1, float _x2, float _y2, float x3, float y3);
	void FillTriangle(const PixelCoord& point1, const PixelCoord& point2, const PixelCoord& point3);

	void FillShadedTriangle(float _x1, float _y1, float _x2, float _y2, float x3, float y3);
	void FillShadedTriangle(const PixelCoord& point1, const PixelCoord& point2, const PixelCoord& point3);

	void FillTriangle(float _x1, float _y1, float u1, float v1, float _x2, float _y2, float u2, float v2,
		float x3, float y3, float u3, float v3, ImageID image_id);
	void FillTriangle(const PixelCoord& point1, float u1, float v1,
		const PixelCoord& point2, float u2, float v2,
		const PixelCoord& point3, float u3, float v3, ImageID image_id);

	virtual void DrawFan(const std::vector<vec2>& coords, bool fill) = 0;
	virtual void DrawImageFan(ImageID image_id, const std::vector<vec2>& coords, const std::vector<vec2>& tex_coords) = 0;
	static void AddRadius(std::vector<vec2>& vertex_list, int x, int y, int r, float start_angle, float end_angle);
	static void TryAddRadius(std::vector<vec2>& vertex_list, int x, int y, int r, float start_angle, float end_angle, int current_corner_bit, int corner_mask);
	void DrawArc(int x, int y, int dx, int dy, int a1, int a2, bool fill);
	void DrawRoundedRect(const PixelRect& rect, int radius, int corner_mask, bool fill);

	// Returns an ID to the added bitmap. Returns 0 if error.
	// Alpha buffer or bitmap may be NULL.
	virtual ImageID AddImage(const Canvas* image, const Canvas* alpha_buffer) = 0;

	// If hint == kUpdateAccurate, this function replaces the contents of image_id with the
	// contents of image.
	// If hint == kUpdateFast, the behaviour depends on the implementation. In general,
	// image and alpha_buffer must have the same dimensions and pixel format as the current
	// image.
	virtual void UpdateImage(ImageID image_id,
				 const Canvas* image,
				 const Canvas* alpha_buffer,
				 UpdateHint hint = kUpdateAccurate) = 0;
	virtual void RemoveImage(ImageID image_id) = 0;

	// Renders a bitmap.
	void DrawImage(ImageID image_id, int x, int y);
	void DrawImage(ImageID image_id, const PixelCoord& top_left);
	// Renders a subsquare of a bitmap.
	void DrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect);
	void DrawImage(ImageID image_id, const PixelCoord& top_left, const PixelRect& subpatch_rect);
	// Renders and stretches a bitmap.
	void DrawImage(ImageID image_id, const PixelRect& rect);
	void DrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect);

	// Renders the associated alpha buffer, if there is one.
	void DrawAlphaImage(ImageID image_id, int x, int y);
	void DrawAlphaImage(ImageID image_id, const PixelCoord& top_left);

	void SetFontManager(FontManager* font_manager);
	FontManager* GetFontManager() const;
	virtual void SetFontSmoothness(bool smooth) = 0;
	int GetStringWidth(const wstr& s) const;
	int GetFontHeight() const;
	int GetLineHeight() const;
	int GetTabSize() const;
	void SetTabSize(int size);
	virtual void PrintText(const wstr& s, int x, int y) = 0;

	lepra::uint8 FindMatchingColor(const Color& color);
	virtual void ReadPixels(Canvas& dest_canvas, const PixelRect& rect) = 0;

	// Returns the internal kRgb order.
	virtual RGBOrder GetRGBOrder() const = 0;

	//
	// Display list creation functions.
	//
	DisplayListID NewDisplayList();
	void DeleteDisplayList(DisplayListID display_list_id);

	void BeginDisplayList(DisplayListID display_list_id);
	void EndDisplayList();

	void RenderDisplayList(DisplayListID display_list_id);

	virtual void ClearFontBuffers();

protected:
	class DisplayEntity {
	public:
		friend class Painter;
		RenderMode GetRenderMode() const;
		lepra::uint8 GetAlpha() const;
		ImageID GetImageID() const;
		const PixelRect& GetClippingRect() const;
		Geometry2D& GetGeometry();
	private:
		DisplayEntity(RenderMode rm,
		              lepra::uint8 alpha,
		              ImageID image_id,
		              const PixelRect& clipping_rect,
			      unsigned vertex_format) :
			rm_(rm),
			alpha_(alpha),
			image_id_(image_id),
			clipping_rect_(clipping_rect),
			geometry_(vertex_format) {
		}

		void Init(RenderMode rm,
		          lepra::uint8 alpha,
		          ImageID image_id,
		          const PixelRect& clipping_rect,
			  unsigned vertex_format) {
			rm_ = rm;
			alpha_ = alpha;
			image_id_ = image_id;
			clipping_rect_ = clipping_rect;
			geometry_.Init(vertex_format);
		}

		RenderMode rm_;
		lepra::uint8 alpha_;
		ImageID image_id_;
		PixelRect clipping_rect_;
		Geometry2D geometry_;
	};

	typedef std::list<unsigned> AttribList;
	typedef std::list<RenderMode> RMList;
	typedef std::list<lepra::uint8> UCharList;
	typedef std::list<Color> ColorList;
	typedef std::list<PixelRect> RectList;

	virtual void DoDrawPixel(int x, int y) = 0;
	virtual void DoDrawLine(int _x1, int _y1, int _x2, int _y2) = 0;
	virtual void DoDrawRect(int left, int top, int right, int bottom) = 0;
	virtual void DoFillRect(int left, int top, int right, int bottom) = 0;
	virtual void DoDraw3DRect(int left, int top, int right, int bottom, int width, bool sunken) = 0;
	virtual void DoFillShadedRect(int left, int top, int right, int bottom) = 0;
	virtual void DoFillTriangle(float _x1, float _y1,
				    float _x2, float _y2,
				    float x3, float y3) = 0;
	virtual void DoFillShadedTriangle(float _x1, float _y1,
					  float _x2, float _y2,
					  float x3, float y3) = 0;
	virtual void DoFillTriangle(float _x1, float _y1, float u1, float v1,
				    float _x2, float _y2, float u2, float v2,
				    float x3, float y3, float u3, float v3,
				    ImageID image_id) = 0;
	virtual void DoDrawImage(ImageID image_id, int x, int y) = 0;
	virtual void DoDrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect) = 0;
	virtual void DoDrawImage(ImageID image_id, const PixelRect& rect) = 0;
	virtual void DoDrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect) = 0;
	virtual void DoDrawAlphaImage(ImageID image_id, int x, int y) = 0;

	virtual void GetImageSize(ImageID image_id, int& width, int& height) const = 0;

	virtual void DoRenderDisplayList(std::vector<DisplayEntity*>* display_list) = 0;

	// Returns vertex_format | whatever-flags-the-painter-needs. The flags
	// are defined in Geometry2D, and the default behaviour is to do nothing.
	virtual void AdjustVertexFormat(unsigned& vertex_format);

	// This function will return the geometry of either a newly create DisplayEntity
	// or the current (last) one in the current display list, depending on wether the
	// last entity matches the given parameters. The newly created DisplayEntity will
	// be  appended at the end of the display list.
	Geometry2D* FetchDisplayEntity(unsigned vertex_format = 0, ImageID image_id = kInvalidImageid);

	void CreateLine(int _x1, int _y1, int _x2, int _y2);
	void CreateRectFrame(int left, int top, int right, int bottom, int width);
	void Create3DRectFrame(int left, int top, int right, int bottom, int width, bool sunken);
	void CreateRect(int left, int top, int right, int bottom);
	void CreateShadedRect(int left, int top, int right, int bottom);

	// Equivalent to FillTriangle().
	void CreateTriangle(float _x1, float _y1,
	                    float _x2, float _y2,
	                    float x3, float y3);
	// Equivalent to FillShadedTriangle().
	void CreateShadedTriangle(float _x1, float _y1,
	                          float _x2, float _y2,
	                          float x3, float y3);
	// Triangle with a texture.
	void CreateTriangle(float _x1, float _y1, float u1, float v1,
	                    float _x2, float _y2, float u2, float v2,
	                    float x3, float y3, float u3, float v3,
	                    ImageID image_id);

	void CreateImage(ImageID image_id, int x, int y);
	void CreateImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect);
	void CreateImage(ImageID image_id, const PixelRect& rect);
	void CreateImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect);

	static bool IsPowerOf2(unsigned number);
	static unsigned GetClosestPowerOf2(unsigned number, bool greater = false);
	static unsigned GetExponent(unsigned power_of2);

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
	void GetScreenCoordClippingRect(PixelRect& clipping_rect) const;

	Color& GetColorInternal(int color_index);

private:
	typedef std::unordered_map<unsigned, std::vector<DisplayEntity*>* > DisplayListMap;

	FontManager* font_manager_;

	DisplayListMap display_list_map_;
	IdManager<int> display_list_id_manager_;
	std::vector<DisplayEntity*>* current_display_list_;
	std::vector<DisplayEntity*>::iterator display_list_iter_;
	DisplayListID default_display_list_;

	RenderMode render_mode_;

	AttribList attrib_stack_;
	RMList     attrib_rm_stack_;
	UCharList  attrib_alpha_stack_;
	ColorList  attrib_color0_stack_;
	ColorList  attrib_color1_stack_;
	ColorList  attrib_color2_stack_;
	ColorList  attrib_color3_stack_;
	RectList   attrib_clip_rect_stack_;

	int origo_x_;
	int origo_y_;
	XDir x_dir_;
	YDir y_dir_;

	Canvas* canvas_;
	PixelRect clipping_rect_;

	Color color_[4];
	lepra::uint8 alpha_value_;

	int tab_size_;
};



}
