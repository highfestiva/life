
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uiguiimagemanager.h"
#include "../../../lepra/include/log.h"



namespace uitbc {



GUIImageManager::GUIImageManager():
	painter_(0),
	swap_rgb_(false) {
}

GUIImageManager::~GUIImageManager() {
	ClearImageTable();
}

void GUIImageManager::ClearImageTable() {
	ImageTable::Iterator iter;
	for (iter = image_table_.First(); iter != image_table_.End(); ++iter) {
		Image* _image = *iter;

		if (painter_ != 0) {
			painter_->RemoveImage(_image->id_);
		}

		delete _image;
	}

	image_table_.RemoveAll();
}

void GUIImageManager::SetPainter(Painter* painter) {
	if (painter_ != painter) {
		ClearImageTable();
	}

	painter_ = painter;
}

Painter::ImageID GUIImageManager::AddImage(const Canvas& image, ImageStyle style, BlendFunc blend_func, uint8 alpha_value) {
	Canvas _image(image, true);
	if (swap_rgb_ == true) {
		_image.SwapRGBOrder();
	}
	Painter::ImageID id = painter_->AddImage(&_image, 0);
	AddLoadedImage(_image, id, style, blend_func, alpha_value);
	return id;
}

void GUIImageManager::AddLoadedImage(const Canvas& image, Painter::ImageID image_id, ImageStyle style, BlendFunc blend_func, uint8 alpha_value) {
	deb_assert(!HasImage(image_id));
	image_table_.Insert(image_id, new Image(image_id, image, style, blend_func, alpha_value));
}

bool GUIImageManager::RemoveImage(Painter::ImageID image_id) {
	const bool dropped = DropImage(image_id);
	if (dropped) {
		painter_->RemoveImage(image_id);
	}
	return dropped;
}

bool GUIImageManager::DropImage(Painter::ImageID image_id) {
	ImageTable::Iterator iter = image_table_.Find(image_id);
	if (iter != image_table_.End()) {
		Image* _image = *iter;
		image_table_.Remove(iter);
		delete _image;
		return true;
	}
	deb_assert(false);
	return false;
}

bool GUIImageManager::HasImage(Painter::ImageID image_id) const {
	return (image_table_.Find(image_id) != image_table_.End());
}

void GUIImageManager::SetImageOffset(Painter::ImageID image_id, int x_offset, int y_offset) {
	ImageTable::Iterator iter = image_table_.Find(image_id);
	if (iter != image_table_.End()) {
		Image* _image = *iter;
		_image->x_offset_ = x_offset;
		_image->y_offset_ = y_offset;
	}
}

Painter::ImageID GUIImageManager::GetImageID(const str& image_name) {
	IDTable::Iterator iter(id_table_.Find(image_name));
	if (iter == id_table_.End()) {
		return Painter::kInvalidImageid;
	}

	return *iter;
}

void GUIImageManager::DrawImage(Painter::ImageID image_id, int x, int y) {
	if (image_id != Painter::kInvalidImageid) {
		ImageTable::Iterator iter = image_table_.Find(image_id);
		deb_assert(iter != image_table_.End());	// We need this, otherwise components won't be able to know how big the image is, etc.
		if (iter != image_table_.End()) {
			Image* _image = *iter;

			if (_image->blend_func_ == kAlphatest) {
				painter_->SetRenderMode(Painter::kRmAlphatest);
				painter_->SetAlphaValue(_image->alpha_value_);
			} else if(_image->blend_func_ == kAlphablend) {
				painter_->SetRenderMode(Painter::kRmAlphablend);
				painter_->SetAlphaValue(255);
			} else {
				painter_->SetRenderMode(Painter::kRmNormal);
			}

			painter_->DrawImage(image_id, x + _image->x_offset_, y + _image->y_offset_);
		}
	}
}

void GUIImageManager::DrawImage(Painter::ImageID image_id, const PixelRect& rect) {
	if (image_id != Painter::kInvalidImageid) {
		ImageTable::Iterator iter = image_table_.Find(image_id);
		if (iter != image_table_.End()) {
			Image* _image = *iter;

			if (_image->blend_func_ == kAlphatest) {
				painter_->SetRenderMode(Painter::kRmAlphatest);
				painter_->SetAlphaValue(_image->alpha_value_);
			} else if(_image->blend_func_ == kAlphablend) {
				painter_->SetRenderMode(Painter::kRmAlphablend);
				painter_->SetAlphaValue(255);
			} else {
				painter_->SetRenderMode(Painter::kRmNormal);
			}

			PixelRect _rect(rect);
			_rect.Offset(_image->x_offset_, _image->y_offset_);
			PixelCoord pos(_rect.left_, _rect.top_);

			switch(_image->style_) {
			case kTiled:
				painter_->DrawImage(image_id, pos, PixelRect(0, 0, _rect.GetWidth(), _rect.GetHeight()));
				break;
			case kCentered:
				painter_->DrawImage(image_id,
					(int)pos.x + ((int)_rect.GetWidth()  - (int)_image->canvas_.GetWidth())  / 2,
					(int)pos.y + ((int)_rect.GetHeight() - (int)_image->canvas_.GetHeight()) / 2);
				break;
			case kStretched:
				painter_->DrawImage(image_id, _rect);
				break;
			default:
				break;
			}
		}
	}
}

PixelCoord GUIImageManager::GetImageSize(Painter::ImageID image_id) {
	PixelCoord size(0, 0);

	if (image_id != Painter::kInvalidImageid) {
		ImageTable::Iterator iter = image_table_.Find(image_id);
		if (iter != image_table_.End()) {
			Image* _image = *iter;
			size.x = _image->canvas_.GetWidth();
			size.y = _image->canvas_.GetHeight();
		}
	}

	return size;
}

bool GUIImageManager::IsOverImage(Painter::ImageID image_id, int screen_x, int screen_y, const PixelRect& screen_rect) {
	if (image_id != Painter::kInvalidImageid) {
		ImageTable::Iterator iter = image_table_.Find(image_id);
		if (iter != image_table_.End()) {
			Image* _image = *iter;
			Canvas* canvas = &_image->canvas_;

			if (_image->blend_func_ == kNoBlend) {
				return screen_rect.IsInside(screen_x, screen_y);
			}

			// Calculate the image's pixel coordinates.
			int x = screen_x - screen_rect.left_;
			int y = screen_y - screen_rect.top_;

			switch(_image->style_) {
			case kTiled:
				x %= canvas->GetWidth();
				y %= canvas->GetHeight();
				break;
			case kCentered: {
				PixelCoord top_left(screen_rect.left_ + (screen_rect.GetWidth()  - canvas->GetWidth()) / 2,
									  screen_rect.top_  + (screen_rect.GetHeight() - canvas->GetHeight()) / 2);
				PixelRect image_rect(top_left, top_left + PixelCoord(canvas->GetWidth(), canvas->GetHeight()));
				if (image_rect.IsInside(screen_x, screen_y) == false) {
					return false;
				}

				x = screen_x - top_left.x;
				y = screen_y - top_left.y;
			} break;
			case kStretched: {
				x = (x * canvas->GetWidth())  / screen_rect.GetWidth();
				y = (y * canvas->GetHeight()) / screen_rect.GetHeight();
			} break;
			default:
				return false;
			}

			Color color;
			canvas->GetPixelColor(x, y, color);

			return (color.alpha_ >= _image->alpha_value_);
		}
	}

	return false;
}



Painter* GUIImageManager::GetPainter() const {
	return painter_;
}

void GUIImageManager::SwapRGB() {
	swap_rgb_ = !swap_rgb_;
}



}
