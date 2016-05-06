
// Author: Jonas Byström, Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uipainter.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/canvas.h"
#include "../include/uifontmanager.h"



namespace uitbc {



Painter::Painter() :
	font_manager_(0),
	display_list_id_manager_(1, 100000, 0),
	current_display_list_(0),
	render_mode_(kRmNormal),
	origo_x_(0),
	origo_y_(0),
	x_dir_(kXRight),
	y_dir_(kYDown),
	canvas_(0),
	alpha_value_(255),
	tab_size_(0) {
	default_display_list_ = NewDisplayList();
}

Painter::~Painter() {
	DeleteDisplayList(default_display_list_);
	font_manager_ = 0;
}

void Painter::DefineCoordinates(int origo_x, int origo_y, XDir x_dir, YDir y_dir) {
	origo_x_ = origo_x;
	origo_y_ = origo_y;
	x_dir_ = x_dir;
	y_dir_ = y_dir;
}

void Painter::SetDestCanvas(Canvas* canvas) {
	canvas_ = canvas;
}

bool Painter::PushAttrib(unsigned attrib) {
	bool return_value = true;

	const size_t max_count = 1024;

	if ((attrib & kAttrRendermode) != 0) {
		attrib_rm_stack_.push_back(render_mode_);
		if (attrib_rm_stack_.size() > max_count) {
			attrib_rm_stack_.pop_front();
			return_value = false;
		}
	}

	if ((attrib & kAttrAlphavalue) != 0) {
		attrib_alpha_stack_.push_back(GetAlphaValue());
		if (attrib_alpha_stack_.size() > max_count) {
			attrib_alpha_stack_.pop_front();
			return_value = false;
		}
	}

	if ((attrib & kAttrColor0) != 0) {
		attrib_color0_stack_.push_back(GetColor(0));
		if (attrib_color0_stack_.size() > max_count) {
			attrib_color0_stack_.pop_front();
			return_value = false;
		}
	}

	if ((attrib & kAttrColor1) != 0) {
		attrib_color1_stack_.push_back(GetColor(1));
		if (attrib_color1_stack_.size() > max_count) {
			attrib_color1_stack_.pop_front();
			return_value = false;
		}
	}

	if ((attrib & kAttrColor2) != 0) {
		attrib_color2_stack_.push_back(GetColor(2));
		if (attrib_color2_stack_.size() > max_count) {
			attrib_color2_stack_.pop_front();
			return_value = false;
		}
	}

	if ((attrib & kAttrColor3) != 0) {
		attrib_color3_stack_.push_back(GetColor(3));
		if (attrib_color3_stack_.size() > max_count) {
			attrib_color3_stack_.pop_front();
			return_value = false;
		}
	}

	if ((attrib & kAttrCliprect) != 0) {
		PixelRect _clipping_rect;
		GetClippingRect(_clipping_rect);
		attrib_clip_rect_stack_.push_back(_clipping_rect);
		if (attrib_clip_rect_stack_.size() > max_count) {
			attrib_clip_rect_stack_.pop_front();
			return_value = false;
		}
	}

	attrib_stack_.push_back(attrib);

	return return_value;
}

bool Painter::PopAttrib() {
	if (attrib_stack_.empty()) {
		return false;
	}

	unsigned _attrib = attrib_stack_.back();
	attrib_stack_.pop_back();

	if ((_attrib & kAttrRendermode) != 0) {
		if (!attrib_rm_stack_.empty()) {
			SetRenderMode(attrib_rm_stack_.back());
			attrib_rm_stack_.pop_back();
		} else {
			return false;
		}
	}

	if ((_attrib & kAttrAlphavalue) != 0) {
		if (!attrib_alpha_stack_.empty()) {
			SetAlphaValue(attrib_alpha_stack_.back());
			attrib_alpha_stack_.pop_back();
		} else {
			return false;
		}
	}

	if ((_attrib & kAttrColor0) != 0) {
		if (!attrib_color0_stack_.empty()) {
			SetColor(attrib_color0_stack_.back(), 0);
			attrib_color0_stack_.pop_back();
		} else {
			return false;
		}
	}

	if ((_attrib & kAttrColor1) != 0) {
		if (!attrib_color1_stack_.empty()) {
			SetColor(attrib_color1_stack_.back(), 1);
			attrib_color1_stack_.pop_back();
		} else {
			return false;
		}
	}

	if ((_attrib & kAttrColor2) != 0) {
		if (!attrib_color2_stack_.empty()) {
			SetColor(attrib_color2_stack_.back(), 2);
			attrib_color2_stack_.pop_back();
		} else {
			return false;
		}
	}

	if ((_attrib & kAttrColor3) != 0) {
		if (!attrib_color3_stack_.empty()) {
			SetColor(attrib_color3_stack_.back(), 3);
			attrib_color3_stack_.pop_back();
		} else {
			return false;
		}
	}

	if ((_attrib & kAttrCliprect) != 0) {
		if (!attrib_clip_rect_stack_.empty()) {
			PixelRect _rect = attrib_clip_rect_stack_.back();
			attrib_clip_rect_stack_.pop_back();
			SetClippingRect(_rect.left_, _rect.top_, _rect.right_, _rect.bottom_);
		} else {
			return false;
		}
	}

	return true;
}

void Painter::SetRenderMode(RenderMode rm) {
	render_mode_ = rm;
}

void Painter::SetAlphaValue(uint8 alpha) {
	alpha_value_ = alpha;
}

void Painter::SetClippingRect(int left, int top, int right, int bottom) {
	clipping_rect_.Set(left, top, right, bottom);
}

void Painter::ReduceClippingRect(int left, int top, int right, int bottom) {
	PixelRect _clipping_rect(clipping_rect_);

	if (XLT(_clipping_rect.left_, left) == true) {
		_clipping_rect.left_ = left;
	}

	if (XGT(_clipping_rect.right_, right) == true) {
		_clipping_rect.right_ = right;
	}

	if (YLT(_clipping_rect.top_, top) == true) {
		_clipping_rect.top_ = top;
	}

	if (YGT(_clipping_rect.bottom_, bottom) == true) {
		_clipping_rect.bottom_ = bottom;
	}

	SetClippingRect(_clipping_rect);
}

void Painter::SetColor(const Color& color, unsigned color_index) {
	color_[color_index] = color;
}

void Painter::SetFontManager(FontManager* font_manager) {
	font_manager_ = font_manager;
}

FontManager* Painter::GetFontManager() const {
	deb_assert(font_manager_);
	return (font_manager_);
}

int Painter::GetStringWidth(const wstr& s) const {
	if (!font_manager_) {
		return (0);
	}
	return (font_manager_->GetStringWidth(s));
}

int Painter::GetFontHeight() const {
	if (!font_manager_) {
		return (0);
	}
	return (font_manager_->GetFontHeight());
}

int Painter::GetLineHeight() const {
	if (!font_manager_) {
		return (0);
	}
	return (font_manager_->GetLineHeight());
}

int Painter::GetTabSize() const {
	return tab_size_;
}

void Painter::SetTabSize(int _size) {
	tab_size_ = _size;
}

Painter::DisplayListID Painter::NewDisplayList() {
	int id = display_list_id_manager_.GetFreeId();
	if(id != display_list_id_manager_.GetInvalidId()) {
		display_list_map_.insert(DisplayListMap::value_type(id, new std::vector<DisplayEntity*>));
	}
	return (DisplayListID)id;
}

void Painter::DeleteDisplayList(DisplayListID display_list_id) {
	DisplayListMap::iterator it = display_list_map_.find(display_list_id);
	if(it != display_list_map_.end()) {
		std::vector<DisplayEntity*>* display_list = (*it).second;
		display_list_map_.erase(it);

		std::vector<DisplayEntity*>::iterator list_iter;
		for(list_iter = display_list->begin(); list_iter != display_list->end(); ++list_iter) {
			delete *list_iter;
		}
		delete display_list;
	}
}

void Painter::BeginDisplayList(DisplayListID display_list_id) {
	DisplayListMap::iterator it = display_list_map_.find(display_list_id);
	if (it != display_list_map_.end()) {
		current_display_list_ = (*it).second;
		display_list_iter_ = current_display_list_->begin();
	}
}

void Painter::EndDisplayList() {
	if (current_display_list_) {
		std::vector<DisplayEntity*>::iterator list_iter;
		for (list_iter = current_display_list_->begin(); list_iter != current_display_list_->end(); ++list_iter) {
			DisplayEntity* entity = *list_iter;
			entity->geometry_.Reset();
		}
		display_list_iter_ = current_display_list_->begin();
		current_display_list_ = 0;
	}
}

void Painter::RenderDisplayList(DisplayListID display_list_id) {
	DisplayListMap::iterator it = display_list_map_.find(display_list_id);
	if(it != display_list_map_.end()) {
		std::vector<DisplayEntity*>* display_list = (*it).second;
		DoRenderDisplayList(display_list);
	}
}

void Painter::ClearFontBuffers() {
}

Geometry2D* Painter::FetchDisplayEntity(unsigned vertex_format, ImageID image_id) {
	AdjustVertexFormat(vertex_format);

	DisplayEntity* entity = 0;
	if(display_list_iter_ != current_display_list_->end()) {
		entity = *display_list_iter_;
	}

	PixelRect _clipping_rect;
	GetClippingRect(_clipping_rect);
	if(entity == 0 ||
	   entity->rm_ != render_mode_ ||
	   entity->geometry_.GetVertexFormat() != vertex_format ||
	   entity->alpha_ != GetAlphaValue() ||
	   entity->image_id_ != image_id ||
	   entity->clipping_rect_.top_ != _clipping_rect.top_ ||
	   entity->clipping_rect_.bottom_ != _clipping_rect.bottom_ ||
	   entity->clipping_rect_.left_ != _clipping_rect.left_ ||
	   entity->clipping_rect_.right_ != _clipping_rect.right_) {
		if(display_list_iter_ != current_display_list_->end())
			++display_list_iter_;
		if(display_list_iter_ != current_display_list_->end()) {
			entity = *display_list_iter_;
			entity->Init(render_mode_, GetAlphaValue(), image_id, _clipping_rect, vertex_format);
		} else {
			entity = new DisplayEntity(render_mode_, GetAlphaValue(), image_id, _clipping_rect, vertex_format);
			current_display_list_->push_back(entity);
			display_list_iter_ = current_display_list_->end();
			--display_list_iter_;
		}
	}
	return &entity->geometry_;
}

void Painter::CreateLine(int _x1, int _y1, int _x2, int _y2) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float __x1 = (float)_x1 - 0.5f;
	float __y1 = (float)_y1 - 0.5f;
	float __x2 = (float)_x2 - 0.5f;
	float __y2 = (float)_y2 - 0.5f;

	float r = (float)color_[0].red_ / 255.0f;
	float g = (float)color_[0].green_ / 255.0f;
	float b = (float)color_[0].blue_ / 255.0f;

	vec2 normal(__y2 - __y1, __x1 - __x2);
	normal.Normalize();

	uint32 v0 = geometry->SetVertex(__x1 - normal.x, __y1 - normal.y, r, g, b);
	uint32 _v1 = geometry->SetVertex(__x1 + normal.x, __y1 + normal.y, r, g, b);
	uint32 _v2 = geometry->SetVertex(__x2 + normal.x, __y2 + normal.y, r, g, b);
	uint32 _v3 = geometry->SetVertex(__x2 - normal.x, __y2 - normal.y, r, g, b);

	geometry->SetTriangle(v0, _v1, _v2);
	geometry->SetTriangle(v0, _v2, _v3);
}

void Painter::CreateRectFrame(int left, int top, int right, int bottom, int width) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float _left   = (float)left - 0.5f;
	float _right  = (float)right - 0.5f;
	float _top    = (float)top - 0.5f;
	float _bottom = (float)bottom - 0.5f;

	float r = (float)color_[0].red_ / 255.0f;
	float g = (float)color_[0].green_ / 255.0f;
	float b = (float)color_[0].blue_ / 255.0f;

	uint32 v0 = geometry->SetVertex(_left, _top, r, g, b);     // Outer top left.
	uint32 _v1 = geometry->SetVertex(_right, _top, r, g, b);    // Outer top right.
	uint32 _v2 = geometry->SetVertex(_right, _bottom, r, g, b); // Outer bottom right.
	uint32 _v3 = geometry->SetVertex(_left, _bottom, r, g, b);  // Outer bottom left.

	_left += width;
	_top += width;
	_right -= width;
	_bottom -= width;

	uint32 v4 = geometry->SetVertex(_left, _top, r, g, b);     // Inner top left.
	uint32 v5 = geometry->SetVertex(_right, _top, r, g, b);    // Inner top right.
	uint32 v6 = geometry->SetVertex(_right, _bottom, r, g, b); // Inner bottom right.
	uint32 v7 = geometry->SetVertex(_left, _bottom, r, g, b);  // Inner bottom left.

	geometry->SetTriangle(v0, v4, v7);
	geometry->SetTriangle(v0, v7, _v3);
	geometry->SetTriangle(v0, _v1, v5);
	geometry->SetTriangle(v0, v5, v4);
	geometry->SetTriangle(_v1, _v2, v6);
	geometry->SetTriangle(_v1, v6, v5);
	geometry->SetTriangle(v7, v6, _v2);
	geometry->SetTriangle(v7, _v2, _v3);
}

void Painter::Create3DRectFrame(int left, int top, int right, int bottom, int width, bool sunken) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float _left   = (float)left - 0.5f;
	float _right  = (float)right - 0.5f;
	float _top    = (float)top - 0.5f;
	float _bottom = (float)bottom - 0.5f;

	float r[4];
	float g[4];
	float b[4];
	int __i[4] = {0, 1, 2, 3};

	if(sunken) {
		__i[0] = 1;
		__i[1] = 0;
		__i[2] = 3;
		__i[3] = 2;
	}

	for(int i = 0; i < 4; i++) {
		r[i] = (float)color_[__i[i]].red_ / 255.0f;
		g[i] = (float)color_[__i[i]].green_ / 255.0f;
		b[i] = (float)color_[__i[i]].blue_ / 255.0f;
	}

	uint32 v0 = geometry->SetVertex(_left, _top, r[0], g[0], b[0]);     // Outer top left.
	uint32 _v1 = geometry->SetVertex(_right, _top, r[0], g[0], b[0]);    // Outer top right #1.
	uint32 _v2 = geometry->SetVertex(_right, _top, r[1], g[1], b[1]);    // Outer top right #2.
	uint32 _v3 = geometry->SetVertex(_right, _bottom, r[1], g[1], b[1]); // Outer bottom right.
	uint32 v4 = geometry->SetVertex(_left, _bottom, r[0], g[0], b[0]);  // Outer bottom left #1.
	uint32 v5 = geometry->SetVertex(_left, _bottom, r[1], g[1], b[1]);  // Outer bottom left #2.

	_left += width;
	_top += width;
	_right -= width;
	_bottom -= width;

	uint32 v6 = geometry->SetVertex(_left, _top, r[2], g[2], b[2]);     // Inner top left.
	uint32 v7 = geometry->SetVertex(_right, _top, r[2], g[2], b[2]);    // Inner top right #1.
	uint32 v8 = geometry->SetVertex(_right, _top, r[3], g[3], b[3]);    // Inner top right #2.
	uint32 v9 = geometry->SetVertex(_right, _bottom, r[3], g[3], b[3]); // Inner bottom right.
	uint32 v10 = geometry->SetVertex(_left, _bottom, r[2], g[2], b[2]);  // Inner bottom left #1.
	uint32 v11 = geometry->SetVertex(_left, _bottom, r[3], g[3], b[3]);  // Inner bottom left #2.

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

	geometry->SetTriangle(v0, _v1, v7);  // Top
	geometry->SetTriangle(v0, v7, v6);  // ...
	geometry->SetTriangle(v0, v6, v10); // Left
	geometry->SetTriangle(v0, v10, v4); // ...
	geometry->SetTriangle(v8, _v2, _v3);  // Right
	geometry->SetTriangle(v8, _v3, v9);  // ...
	geometry->SetTriangle(v11, v9, _v3); // Bottom
	geometry->SetTriangle(v11, _v3, v5); // ...
}

void Painter::CreateRect(int left, int top, int right, int bottom) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float _left   = (float)left - 0.5f;
	float _right  = (float)right - 0.5f;
	float _top    = (float)top - 0.5f;
	float _bottom = (float)bottom - 0.5f;

	float r = (float)color_[0].red_ / 255.0f;
	float g = (float)color_[0].green_ / 255.0f;
	float b = (float)color_[0].blue_ / 255.0f;

	uint32 v0 = geometry->SetVertex(_left, _top, r, g, b);
	uint32 _v1 = geometry->SetVertex(_right, _top, r, g, b);
	uint32 _v2 = geometry->SetVertex(_right, _bottom, r, g, b);
	uint32 _v3 = geometry->SetVertex(_left, _bottom, r, g, b);

	geometry->SetTriangle(v0, _v1, _v2);
	geometry->SetTriangle(v0, _v2, _v3);
}

void Painter::CreateShadedRect(int left, int top, int right, int bottom) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float _left   = (float)left - 0.5f;
	float _right  = (float)right - 0.5f;
	float _top    = (float)top - 0.5f;
	float _bottom = (float)bottom - 0.5f;

	float r[4];
	float g[4];
	float b[4];
	for(int i = 0; i < 4; i++) {
		r[i] = (float)color_[i].red_ / 255.0f;
		g[i] = (float)color_[i].green_ / 255.0f;
		b[i] = (float)color_[i].blue_ / 255.0f;
	}

	uint32 v0 = geometry->SetVertex(_left, _top, r[0], g[0], b[0]);
	uint32 _v1 = geometry->SetVertex(_right, _top, r[1], g[1], b[1]);
	uint32 _v2 = geometry->SetVertex(_right, _bottom, r[2], g[2], b[2]);
	uint32 _v3 = geometry->SetVertex(_left, _bottom, r[3], g[3], b[3]);
	uint32 v4 = geometry->SetVertex((_left + _right) * 0.5f, (_top + _bottom) * 0.5f,
		(r[0] + r[1] + r[2] + r[3]) * 0.25f,
		(g[0] + g[1] + g[2] + g[3]) * 0.25f,
		(b[0] + b[1] + b[2] + b[3]) * 0.25f);

	geometry->SetTriangle(v0, _v1, v4);
	geometry->SetTriangle(_v1, _v2, v4);
	geometry->SetTriangle(_v2, _v3, v4);
	geometry->SetTriangle(_v3, v0, v4);
}

void Painter::CreateTriangle(float _x1, float _y1, float _x2, float _y2, float x3, float y3) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float r = (float)color_[0].red_ / 255.0f;
	float g = (float)color_[0].green_ / 255.0f;
	float b = (float)color_[0].blue_ / 255.0f;

	uint32 v0 = geometry->SetVertex(_x1, _y1, r, g, b);
	uint32 _v1 = geometry->SetVertex(_x2, _y2, r, g, b);
	uint32 _v2 = geometry->SetVertex(x3, y3, r, g, b);
	geometry->SetTriangle(v0, _v1, _v2);
}

void Painter::CreateShadedTriangle(float _x1, float _y1, float _x2, float _y2, float x3, float y3) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxRgb);

	float r[3];
	float g[3];
	float b[3];
	for(int i = 0; i < 3; i++) {
		r[i] = (float)color_[i].red_ / 255.0f;
		g[i] = (float)color_[i].green_ / 255.0f;
		b[i] = (float)color_[i].blue_ / 255.0f;
	}

	uint32 v0 = geometry->SetVertex(_x1, _y1, r[0], g[0], b[0]);
	uint32 _v1 = geometry->SetVertex(_x2, _y2, r[1], g[1], b[1]);
	uint32 _v2 = geometry->SetVertex(x3, y3, r[2], g[2], b[2]);
	geometry->SetTriangle(v0, _v1, _v2);
}

void Painter::CreateTriangle(float _x1, float _y1, float u1, float v1,
                             float _x2, float _y2, float u2, float v2,
                             float x3, float y3, float u3, float v3,
                             ImageID image_id) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxUv, image_id);

	uint32 v0 = geometry->SetVertex(_x1, _y1, u1, v1);
	uint32 _v1 = geometry->SetVertex(_x2, _y2, u2, v2);
	uint32 _v2 = geometry->SetVertex(x3, y3, u3, v3);
	geometry->SetTriangle(v0, _v1, _v2);
}

void Painter::CreateImage(ImageID image_id, int x, int y) {
	int w;
	int h;
	GetImageSize(image_id, w, h);

	PixelRect _rect(x, y, x + w, y + h);
	PixelRect _subpatch_rect(0, 0, w, h);
	CreateImage(image_id, _rect, _subpatch_rect);
}

void Painter::CreateImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect) {
	PixelRect _rect(x, y, x + subpatch_rect.GetWidth(), y + subpatch_rect.GetHeight());
	CreateImage(image_id, _rect, subpatch_rect);
}

void Painter::CreateImage(ImageID image_id, const PixelRect& rect) {
	PixelRect _subpatch_rect(0, 0, 0, 0);
	GetImageSize(image_id, _subpatch_rect.right_, _subpatch_rect.bottom_);
	CreateImage(image_id, rect, _subpatch_rect);
}

void Painter::CreateImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect) {
	Geometry2D* geometry = FetchDisplayEntity(Geometry2D::kVtxUv, image_id);

	float _left   = (float)rect.left_ - 0.5f;
	float _right  = (float)rect.right_ - 0.5f;
	float _top    = (float)rect.top_ - 0.5f;
	float _bottom = (float)rect.bottom_ - 0.5f;

	int _width;
	int height;
	GetImageSize(image_id, _width, height);

	float _u1 = (float)subpatch_rect.left_ / (float)_width;
	float _v1 = (float)subpatch_rect.top_ / (float)height;
	float _u2 = (float)subpatch_rect.right_ / (float)_width;
	float _v2 = (float)subpatch_rect.bottom_ / (float)height;

	uint32 vtx0 = geometry->SetVertex(_left, _top, _u1, _v1);
	uint32 vtx1 = geometry->SetVertex(_right, _top, _u2, _v1);
	uint32 vtx2 = geometry->SetVertex(_right, _bottom, _u2, _v2);
	uint32 vtx3 = geometry->SetVertex(_left, _bottom, _u1, _v2);

	geometry->SetTriangle(vtx0, vtx1, vtx2);
	geometry->SetTriangle(vtx0, vtx2, vtx3);
}

unsigned Painter::GetClosestPowerOf2(unsigned number, bool greater) {
	if (number == 0) {
		return 0;
	}

	unsigned exp = GetExponent(number);
	unsigned pow = 1 << exp;

	if(greater && pow < number)
		return (pow << 1);
	else
		return pow;
}

unsigned Painter::GetExponent(unsigned power_of2) {
	if (power_of2 == 0) {
		// Error.
		return (unsigned)-1;
	}

	unsigned exp = 0;

	while ((power_of2 >> exp) > 1) {
		exp++;
	}

	return exp;
}

uint8 Painter::FindMatchingColor(const Color& color) {
	long target_r = color.red_;
	long target_g = color.green_;
	long target_b = color.blue_;

	long min_error = 0x7FFFFFFF;
	uint8 best_match = 0;

	const Color* palette = GetCanvas()->GetPalette();

	for (int i = 0; i < 256; i++) {
		long dr = target_r - palette[i].red_;
		long dg = target_g - palette[i].green_;
		long db = target_b - palette[i].blue_;

		long error = dr * dr + dg * dg + db * db;

		if (i == 0 || error < min_error) {
			min_error = error;
			best_match = (uint8)i;
		}
	}

	return best_match;
}

void Painter::GetScreenCoordClippingRect(PixelRect& clipping_rect) const {
	clipping_rect = clipping_rect_;
	ToScreenCoords(clipping_rect.left_, clipping_rect.top_);
	ToScreenCoords(clipping_rect.right_, clipping_rect.bottom_);
}

void Painter::AdjustVertexFormat(unsigned&) {
	// Default behaviour. Do nothing.
}


int Painter::GetOrigoX() const {
	return origo_x_;
}

int Painter::GetOrigoY() const {
	return origo_y_;
}

Painter::XDir Painter::GetXDir() const {
	return x_dir_;
}

Painter::YDir Painter::GetYDir() const {
	return y_dir_;
}

Canvas* Painter::GetCanvas() const {
	return canvas_;
}

Painter::RenderMode Painter::GetRenderMode() const {
	return render_mode_;
}

uint8 Painter::GetAlphaValue() const {
	return alpha_value_;
}

void Painter::SetClippingRect(const PixelRect& clipping_rect) {
	SetClippingRect(clipping_rect.left_, clipping_rect.top_, clipping_rect.right_, clipping_rect.bottom_);
}

void Painter::ReduceClippingRect(const PixelRect& clipping_rect) {
	ReduceClippingRect(clipping_rect.left_, clipping_rect.top_, clipping_rect.right_, clipping_rect.bottom_);
}

void Painter::GetClippingRect(PixelRect& clipping_rect) const {
	clipping_rect = clipping_rect_;
}

void Painter::SetColor(uint8 red, uint8 green, uint8 blue, uint8 palette_index, unsigned color_index) {
	SetColor(Color(red, green, blue, palette_index), color_index);
}

Color Painter::GetColor(unsigned color_index) const {
	return color_[color_index];
}

void Painter::DrawPixel(int x, int y) {
	if(current_display_list_ == 0)
		DoDrawPixel(x, y);
	else
		CreateRect(x, y, x, y);
}

void Painter::DrawPixel(const PixelCoord& coords) {
	DrawPixel(coords.x, coords.y);
}

void Painter::DrawLine(int _x1, int _y1, int _x2, int _y2) {
	if(current_display_list_ == 0)
		DoDrawLine(_x1, _y1, _x2, _y2);
	else
		CreateLine(_x1, _y1, _x2, _y2);
}

void Painter::DrawLine(const PixelCoord& point1, const PixelCoord& point2) {
	DrawLine(point1.x, point1.y, point2.x, point2.y);
}

void Painter::DrawRect(const PixelRect& rect) {
	if(current_display_list_ == 0)
		DoDrawRect(rect.left_, rect.top_, rect.right_, rect.bottom_);
	else
		CreateRectFrame(rect.left_, rect.top_, rect.right_, rect.bottom_, 1);
}

void Painter::FillRect(int left, int top, int right, int bottom) {
	if(current_display_list_ == 0)
		DoFillRect(left, top, right, bottom);
	else
		CreateRect(left, top, right, bottom);
}

void Painter::FillRect(const PixelCoord& top_left, const PixelCoord& bottom_right) {
	FillRect(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
}

void Painter::FillRect(const PixelRect& rect) {
	FillRect(rect.left_, rect.top_, rect.right_, rect.bottom_);
}

void Painter::Draw3DRect(int left, int top, int right, int bottom, int width, bool sunken) {
	//if(current_display_list_ == 0)
		DoDraw3DRect(left, top, right, bottom, width, sunken);
	//else
	//	Create3DRectFrame(left, top, right, bottom, width, sunken);
}

void Painter::Draw3DRect(const PixelCoord& top_left, const PixelCoord& bottom_right, int width, bool sunken) {
	Draw3DRect(top_left.x, top_left.y, bottom_right.x, bottom_right.y, width, sunken);
}

void Painter::Draw3DRect(const PixelRect& rect, int width, bool sunken) {
	Draw3DRect(rect.left_, rect.top_, rect.right_, rect.bottom_, width, sunken);
}

void Painter::FillShadedRect(int left, int top, int right, int bottom) {
	if(current_display_list_ == 0)
		DoFillShadedRect(left, top, right, bottom);
	else
		CreateShadedRect(left, top, right, bottom);
}

void Painter::FillShadedRect(const PixelCoord& top_left, const PixelCoord& bottom_right) {
	FillShadedRect(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
}

void Painter::FillShadedRect(const PixelRect& rect) {
	FillShadedRect(rect.left_, rect.top_, rect.right_, rect.bottom_);
}

void Painter::FillTriangle(float _x1, float _y1,
			   float _x2, float _y2,
			   float x3, float y3) {
	if(current_display_list_ == 0)
		DoFillTriangle(_x1, _y1, _x2, _y2, x3, y3);
	else
		CreateTriangle(_x1, _y1, _x2, _y2, x3, y3);
}

void Painter::FillTriangle(const PixelCoord& point1,
			   const PixelCoord& point2,
			   const PixelCoord& point3) {
	FillTriangle((float)point1.x, (float)point1.y,
	             (float)point2.x, (float)point2.y,
	             (float)point3.x, (float)point3.y);
}

void Painter::FillShadedTriangle(float _x1, float _y1,
				 float _x2, float _y2,
				 float x3, float y3) {
	if(current_display_list_ == 0)
		DoFillShadedTriangle(_x1, _y1, _x2, _y2, x3, y3);
	else
		CreateShadedTriangle(_x1, _y1, _x2, _y2, x3, y3);
}

void Painter::FillShadedTriangle(const PixelCoord& point1,
			         const PixelCoord& point2,
			         const PixelCoord& point3) {
	FillShadedTriangle((float)point1.x, (float)point1.y,
	                   (float)point2.x, (float)point2.y,
	                   (float)point3.x, (float)point3.y);
}

void Painter::FillTriangle(float _x1, float _y1, float u1, float v1,
			   float _x2, float _y2, float u2, float v2,
			   float x3, float y3, float u3, float v3,
			   ImageID image_id) {
	if(current_display_list_ == 0)
		DoFillTriangle(_x1, _y1, u1, v1, _x2, _y2, u2, v2, x3, y3, u3, v3, image_id);
	else
		CreateTriangle(_x1, _y1, u1, v1, _x2, _y2, u2, v2, x3, y3, u3, v3, image_id);
}

void Painter::FillTriangle(const PixelCoord& point1, float u1, float v1,
			   const PixelCoord& point2, float u2, float v2,
			   const PixelCoord& point3, float u3, float v3,
			   ImageID image_id) {
	FillTriangle((float)point1.x, (float)point1.y, u1, v1,
	             (float)point2.x, (float)point2.y, u2, v2,
	             (float)point3.x, (float)point3.y, u3, v3, image_id);
}

void Painter::AddRadius(std::vector<vec2>& vertex_list, int x, int y, int r, float start_angle, float end_angle) {
	const float angle_diff = end_angle-start_angle;
	const int count = (int)(r * ::fabs(angle_diff) * 0.256f) + 3;
	const float angle_step = angle_diff/(count-1);
	float a = start_angle;
	for (int i = 0; i < count; ++i, a+=angle_step) {
		vertex_list.push_back(vec2(x-r*::sin(a), y-r*::cos(a)));
	}
}

void Painter::TryAddRadius(std::vector<vec2>& vertex_list, int x, int y, int r, float start_angle, float end_angle, int current_corner_bit, int corner_mask) {
	if (corner_mask&current_corner_bit) {
		AddRadius(vertex_list, x, y, r, start_angle, end_angle);
	} else {
		switch (current_corner_bit) {
			case 1:	x -= r;	y -= r;	break;
			case 2:	x += r;	y -= r;	break;
			case 4:	x += r;	y += r;	break;
			case 8:	x -= r;	y += r;	break;
		}
		vertex_list.push_back(vec2((float)x, (float)y));
	}
}

void Painter::DrawArc(int x, int y, int dx, int dy, int a1, int a2, bool fill) {
	if (dx <= 0 || dy <= 0) {
		return;
	}
	const size_t curve_count = ((dx*2 + dy*2) / 20 + std::abs(a1-a2)/20 + 12) & (~7);
	std::vector<vec2> _coords;
	const float x_radius = dx*0.5f;
	const float y_radius = dy*0.5f;
	const float mid_x = x + dx*0.5f;
	const float mid_y = y + dy*0.5f;
	if (fill) {
		_coords.push_back(vec2(mid_x, mid_y));
	}
	const float _start_angle = lepra::Math::Deg2Rad((float)a1);
	const float _end_angle = lepra::Math::Deg2Rad((float)a2);
	const float delta_angle = (_end_angle-_start_angle)/(curve_count-1);
	float angle = _start_angle;
	for (size_t i = 0; i < curve_count; ++i) {
		_coords.push_back(vec2(
			mid_x + cos(angle)*x_radius,
			mid_y - sin(angle)*y_radius));
		angle += delta_angle;
	}
	DrawFan(_coords, fill);
}

void Painter::DrawRoundedRect(const PixelRect& rect, int radius, int corner_mask, bool fill) {
	const int x = rect.GetCenterX();
	const int y = rect.GetCenterY();
	const int dx = rect.GetWidth()/2;
	const int dy = rect.GetHeight()/2;
	std::vector<vec2> _coords;
	_coords.push_back(vec2((float)x, (float)y));
	TryAddRadius(_coords, x-dx+radius, y-dy+radius, radius, +PIF/2, 0,      0x1, corner_mask);
	TryAddRadius(_coords, x+dx-radius, y-dy+radius, radius, 0,      -PIF/2, 0x2, corner_mask);
	TryAddRadius(_coords, x+dx-radius, y+dy-radius, radius, -PIF/2, -PIF,   0x4, corner_mask);
	TryAddRadius(_coords, x-dx+radius, y+dy-radius, radius, +PIF,   +PIF/2, 0x8, corner_mask);
	// Back to start.
	_coords.push_back(_coords[1]);
	DrawFan(_coords, fill);
}

void Painter::DrawImage(ImageID image_id, int x, int y) {
	if(current_display_list_ == 0)
		DoDrawImage(image_id, x, y);
	else
		CreateImage(image_id, x, y);
}

void Painter::DrawImage(ImageID image_id, const PixelCoord& top_left) {
	DrawImage(image_id, top_left.x, top_left.y);
}

void Painter::DrawImage(ImageID image_id, int x, int y, const PixelRect& subpatch_rect) {
	if(current_display_list_ == 0)
		DoDrawImage(image_id, x, y, subpatch_rect);
	else
		CreateImage(image_id, x, y, subpatch_rect);
}

void Painter::DrawImage(ImageID image_id, const PixelCoord& top_left, const PixelRect& subpatch_rect) {
	DrawImage(image_id, top_left.x, top_left.y, subpatch_rect);
}

void Painter::DrawImage(ImageID image_id, const PixelRect& rect) {
	if(current_display_list_ == 0)
		DoDrawImage(image_id, rect);
	else
		CreateImage(image_id, rect);
}

void Painter::DrawImage(ImageID image_id, const PixelRect& rect, const PixelRect& subpatch_rect) {
	if(current_display_list_ == 0)
		DoDrawImage(image_id, rect, subpatch_rect);
	else
		CreateImage(image_id, rect, subpatch_rect);
}

void Painter::DrawAlphaImage(ImageID image_id, int x, int y) {
	if(current_display_list_ == 0)
		DoDrawAlphaImage(image_id, x, y);
	else
		CreateImage(image_id, x, y);
}

void Painter::DrawAlphaImage(ImageID image_id, const PixelCoord& top_left) {
	DrawAlphaImage(image_id, top_left.x, top_left.y);
}

Painter::RenderMode Painter::DisplayEntity::GetRenderMode() const {
	return rm_;
}

uint8 Painter::DisplayEntity::GetAlpha() const {
	return alpha_;
}

Painter::ImageID Painter::DisplayEntity::GetImageID() const {
	return image_id_;
}

const PixelRect& Painter::DisplayEntity::GetClippingRect() const {
	return clipping_rect_;
}

Geometry2D& Painter::DisplayEntity::GetGeometry() {
	return geometry_;
}

bool Painter::IsPowerOf2(unsigned number) {
	return (number == GetClosestPowerOf2(number));
}

void Painter::ToScreenCoords(int& x, int& y) const {
	x = x * (int)x_dir_ + origo_x_;
	y = y * (int)y_dir_ + origo_y_;
}

void Painter::ToUserCoords(int& x, int& y) const {
	x = (x - origo_x_) * (int)x_dir_;
	y = (y - origo_y_) * (int)y_dir_;
}

void Painter::ToScreenCoords(float& x, float& y) const {
	x = x * (float)x_dir_ + (float)origo_x_;
	y = y * (float)y_dir_ + (float)origo_y_;
}

void Painter::ToUserCoords(float& x, float& y) const {
	x = (x - (float)origo_x_) * (float)x_dir_;
	y = (y - (float)origo_y_) * (float)y_dir_;
}

bool Painter::XLT(int x1, int x2) {
	return (x1 * (int)x_dir_) <  (x2 * (int)x_dir_);
}

bool Painter::XLE(int x1, int x2) {
	return (x1 * (int)x_dir_) <= (x2 * (int)x_dir_);
}

bool Painter::XGT(int x1, int x2) {
	return (x1 * (int)x_dir_) >  (x2 * (int)x_dir_);
}

bool Painter::XGE(int x1, int x2) {
	return (x1 * (int)x_dir_) >= (x2 * (int)x_dir_);
}

bool Painter::YLT(int y1, int y2) {
	return (y1 * (int)y_dir_) <  (y2 * (int)y_dir_);
}

bool Painter::YLE(int y1, int y2) {
	return (y1 * (int)y_dir_) <= (y2 * (int)y_dir_);
}

bool Painter::YGT(int y1, int y2) {
	return (y1 * (int)y_dir_) >  (y2 * (int)y_dir_);
}

bool Painter::YGE(int y1, int y2) {
	return (y1 * (int)y_dir_) >= (y2 * (int)y_dir_);
}

Color& Painter::GetColorInternal(int color_index) {
	return color_[color_index];
}



}
