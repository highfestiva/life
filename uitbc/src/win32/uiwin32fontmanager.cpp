
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/win32/uiwin32fontmanager.h"
#include "../../../uilepra/include/win32/uiwin32displaymanager.h"



namespace uitbc {



FontManager* FontManager::Create(uilepra::DisplayManager* display_manager) {
	return (new Win32FontManager((uilepra::Win32DisplayManager*)display_manager));
}



Win32FontManager::Win32FontManager(uilepra::Win32DisplayManager* display_manager):
	display_manager_(display_manager) {
	HWND wnd = display_manager_->GetHWND();
	dc_ = ::GetDC(wnd);

	COLORREF color = RGB(255, 255, 255);
	color_ref_[0] = color;
	color = RGB(0, 0, 0);
	color_ref_[1] = color;
	color_ref_[2] = color;
	color_ref_[3] = color;
}

Win32FontManager::~Win32FontManager() {
	FontTable::iterator x = font_table_.begin();
	if (x != font_table_.end()) {
		Win32Font* font = (Win32Font*)x->second;
		::DeleteObject(font->win32_font_handle_);
		delete (font);
	}
	font_table_.clear();

	::ReleaseDC(display_manager_->GetHWND(), dc_);
	display_manager_ = 0;
}



Win32FontManager::FontId Win32FontManager::AddFont(const str& font_name, double size, int flags) {
	int weight  = ((flags & kBold) != 0) ? FW_BOLD : FW_NORMAL;
	DWORD italic = ((flags & kItalic) != 0) ? TRUE : FALSE;
	DWORD underline = ((flags & kUnderline) != 0) ? TRUE : FALSE;
	DWORD strike_out = ((flags & kStrikeout) != 0) ? TRUE : FALSE;

	HFONT font_handle = ::CreateFont((int)(size + 0.5),
					  0,
					  0, 0,
					  weight,
					  italic,
					  underline,
					  strike_out,
					  DEFAULT_CHARSET,
					  OUT_DEFAULT_PRECIS,
					  CLIP_DEFAULT_PRECIS,
					  DEFAULT_QUALITY,
					  DEFAULT_PITCH | FF_DONTCARE,
					  font_name.c_str());

	FontId id = kInvalidFontid;
	if (font_handle != NULL) {
		Win32Font* font = new Win32Font();
		font->win32_font_handle_ = font_handle;
		font->name_ = font_name;
		font->size_ = size;
		font->flags_ = flags;
		if (!InternalAddFont(font)) {
			delete (font);
			::DeleteObject(font_handle);
		} else {
			id = font->font_id_;
		}
	}

	return (FontId)id;
}

bool Win32FontManager::RenderGlyph(wchar_t c, Canvas& image, const PixelRect& rect) {
	bool ok = (current_font_ != 0);
	if (ok) {
		ok = (rect.top_ >= 0 && rect.left_ >= 0 &&
			rect.GetWidth() >= 1 && rect.GetHeight() >= 1 &&
			image.GetBitDepth() > 0);
	}
	HDC ram_dc = 0;
	if (ok) {
		ram_dc = ::CreateCompatibleDC(dc_);
		ok = (ram_dc != 0);
	}
	HBITMAP ram_bitmap = 0;
	if (ok) {
		ram_bitmap = ::CreateCompatibleBitmap(dc_, rect.GetWidth(), rect.GetHeight());
		ok = (ram_bitmap != 0);
	}
	HGDIOBJ default_bitmap = 0;
	BITMAPINFO bitmap_info;
	::memset(&bitmap_info, 0, sizeof(bitmap_info));
	if (ok) {
		default_bitmap = ::SelectObject(ram_dc, ram_bitmap);
		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		::GetDIBits(ram_dc, ram_bitmap, 0, 0, 0, &bitmap_info, DIB_RGB_COLORS);
		ok = (bitmap_info.bmiHeader.biWidth == rect.GetWidth() &&
			bitmap_info.bmiHeader.biHeight == rect.GetHeight());
	}
	if (ok) {
		HGDIOBJ default_object = ::SelectObject(ram_dc, ((Win32Font*)current_font_)->win32_font_handle_);
		::SetTextColor(ram_dc, color_ref_[0]);
		::SetBkColor(ram_dc, color_ref_[1]);
		::SetTextAlign(dc_, TA_BASELINE | TA_LEFT);
		ok = (::TextOutW(ram_dc, 0, 0, &c, 1) != FALSE);
		::SelectObject(ram_dc, default_object);
	}
	uint8* bitmap = 0;
	int bpp = -1;
	if (ok) {
		bitmap_info.bmiHeader.biHeight = -rect.GetHeight();
		bitmap_info.bmiHeader.biCompression = 0;
		bpp = bitmap_info.bmiHeader.biBitCount;
		int byte_per_pixel = (bpp+7)/8;
		int byte_count = rect.GetWidth()*rect.GetHeight()*byte_per_pixel;
		bitmap = new uint8[byte_count];
		ok = (::GetDIBits(ram_dc, ram_bitmap, 0, rect.GetHeight(), bitmap, &bitmap_info, DIB_RGB_COLORS) == rect.GetHeight());
		::SelectObject(ram_dc, default_bitmap);
		if (!ok) {
			delete[] bitmap;
		}
	}
	if (ok) {
		Canvas::BitDepth source_bit_depth = Canvas::IntToBitDepth(bpp);
		Canvas::BitDepth target_bit_depth = image.GetBitDepth();
		image.Reset(rect.GetWidth(), rect.GetHeight(), source_bit_depth);
		image.SetBuffer(bitmap, false, true);
		bitmap = 0;	// Don't delete it, ownership now taken by calling canvas.
		image.ConvertBitDepth(target_bit_depth);
		image.FlipVertical();	// Bitmap is upside down...
		image.SwapRGBOrder();	// ... and kBgr.
		for (int y = 0; y < rect.GetHeight(); ++y) {
			for (int x = 0; x < rect.GetWidth(); ++x) {
				Color color;
				image.GetPixelColor(x, y, color);
				color.alpha_ = (uint8)(color.SumRgb()/3);
				color.red_ = color.green_ = color.blue_ = 255;
				image.SetPixelColor(x, y, color);
			}
		}
	}
	delete[] (bitmap);
	bitmap = 0;
	if (ram_bitmap) {
		::DeleteObject(ram_bitmap);
		ram_bitmap = 0;
	}
	if (ram_dc) {
		::DeleteDC(ram_dc);
		ram_dc = 0;
	}

	/*printf("Rendering glyph '%c' (height=%i):\n", c, image.GetHeight());
	for (int y = 0; y < (int)image.GetHeight(); ++y) {
		for (int x = 0; x < (int)image.GetWidth(); ++x) {
			if (image.GetPixelColor(x, y).To32() == 0) {
				//printf("         ");
				printf(" ");
			} else {
				//printf("%8.8X ", image.GetPixelColor(x, y).To32());
				printf("*");
			}
		}
		printf("\n");
	}*/

	return (ok);
}



int Win32FontManager::GetCharWidth(wchar_t c) const {
	Win32Font* font = (Win32Font*)current_font_;
	HGDIOBJ default_object = ::SelectObject(dc_, font->win32_font_handle_);
	ABC abc;
	INT width;
	int char_width = 0;
	if (::GetCharABCWidths(dc_, (utchar)c, (utchar)c, &abc) != FALSE) {
		char_width = (int)(abc.abcA + abc.abcB + abc.abcC);
	} else if (::GetCharWidth32(dc_, (utchar)c, (utchar)c, &width) != FALSE) {
		char_width = (int)width;
	}
	::SelectObject(dc_, default_object);
	return char_width;
}

int Win32FontManager::GetCharOffset(wchar_t c) const {
	Win32Font* font = (Win32Font*)current_font_;
	::SelectObject(dc_, font->win32_font_handle_);
	ABC abc;
	if (::GetCharABCWidths(dc_, (utchar)c, (utchar)c, &abc) != FALSE) {
		return abc.abcA;
	}
	return 0;
}



}
