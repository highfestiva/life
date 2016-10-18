
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../../include/mac/uimacfontmanager.h"
#ifdef LEPRA_IOS
#import <UIKit/UIKit.h>
#define TBC_APPLE_FONT UIFont
#else // !iOS
#import <Cocoa/Cocoa.h>
#define TBC_APPLE_FONT NSFont
#endif // iOS/!iOS
#include "../../../uilepra/include/mac/uimacdisplaymanager.h"
#include "../../../lepra/include/posix/maclog.h"



#define FONT_SIZE_FACTOR 0.8f



namespace uitbc {



FontManager* FontManager::Create(uilepra::DisplayManager* display_manager) {
	return (new MacFontManager((uilepra::MacDisplayManager*)display_manager));
}



MacFontManager::MacFontManager(uilepra::MacDisplayManager* display_manager):
	display_manager_(display_manager) {
}

MacFontManager::~MacFontManager() {
	FontTable::iterator x = font_table_.begin();
	if (x != font_table_.end()) {
		MacFont* font = (MacFont*)x->second;
		delete (font);
	}
	font_table_.clear();

	display_manager_ = 0;
}



MacFontManager::FontId MacFontManager::AddFont(const str& font_name, double _size, int flags) {
	FontId id = kInvalidFontid;
	{
		MacFont* font = new MacFont();
		font->name_ = font_name;
		font->size_ = _size;
		font->actual_size_ = _size * FONT_SIZE_FACTOR;
		font->flags_ = flags;
		if (!InternalAddFont(font)) {
			delete (font);
		} else {
			id = font->font_id_;
		}
	}

	return id;
}

bool MacFontManager::RenderGlyph(wchar_t c, Canvas& image, const PixelRect& _rect) {
	image.Reset(_rect.GetWidth(), _rect.GetHeight(), Canvas::kBitdepth32Bit);
	image.CreateBuffer();
	::memset(image.GetBuffer(), 0, image.GetWidth()*image.GetPixelByteSize()*image.GetHeight());

	const float corrected_font_size = ((MacFont*)current_font_)->actual_size_;	// Similar to other platforms...
	CGContextRef textcontext; // This is our rendering context.
	CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB(); // We need some colorspace.
	// Create a bitmap context.
	textcontext = CGBitmapContextCreate(image.GetBuffer(), image.GetWidth(), image.GetHeight(), 8,
		image.GetWidth()*image.GetPixelByteSize(), colorspace, kCGImageAlphaPremultipliedLast);
	CGFloat rect[4] = { (CGFloat)_rect.left_, (CGFloat)_rect.top_, (CGFloat)_rect.right_, (CGFloat)_rect.bottom_ };
	CGFloat transparent[4] = { 1, 1, 1, 0 };
	CGFloat text_color[4] = { 1, 1, 1, 1 };
	// if you do this a lot store the color somewhere and release it when you are done with it.
	CGColorRef backgroundColor = CGColorCreate(colorspace, transparent);
	CGContextSetFillColorWithColor(textcontext, backgroundColor);
	CGContextFillRect(textcontext, *(CGRect*)rect);
	CGColorRef foregroundColor = CGColorCreate(colorspace, text_color);
	CGContextSetFillColorWithColor(textcontext, foregroundColor);
	CGContextSelectFont(textcontext, current_font_->name_.c_str(), corrected_font_size, kCGEncodingMacRoman);
	CGContextSetLineWidth(textcontext, 0.3);
	CGContextSetTextDrawingMode(textcontext, kCGTextFillStroke);
	CGContextSetRGBStrokeColor(textcontext, 1,1,1,0.5);
#ifdef LEPRA_IOS
	int y = (_rect.GetHeight() - ((MacFont*)current_font_)->actual_size_) / 8;
	UIGraphicsPushContext(textcontext);
	char tmp_string[2] = {c, 0};
	NSString* _c = MacLog::Encode(tmp_string);
	NSString* _font_name = MacLog::Encode(current_font_->name_);
	[_c drawAtPoint:CGPointMake(0, y) withFont:[TBC_APPLE_FONT fontWithName:_font_name size:corrected_font_size]];
	CGContextFlush(textcontext);
	UIGraphicsPopContext();
#else // !iOS
	int y = _rect.GetHeight() - ((MacFont*)current_font_)->actual_size_;
	const char ch = (char)c;
	CGContextShowTextAtPoint(textcontext, 0, y, &ch, 1);
	CGContextFlush(textcontext);
	image.FlipVertical();
#endif // iOS / Mac
	CFRelease(backgroundColor);
	CFRelease(foregroundColor);
	CFRelease(colorspace);
	CFRelease(textcontext);

	// Strengthen the colors!
	for (int y = 0; y < (int)image.GetHeight(); ++y) {
		for (int x = 0; x < (int)image.GetWidth(); ++x) {
			Color color = image.GetPixelColor(x, y);
			if (color.alpha_) {
				color.red_	= 255;
				color.green_	= 255;
				color.blue_	= 255;
				image.SetPixelColor(x, y, color);
			}
		}
	}

	/*printf("Rendering glyph '%c' (height=%i):\n", c, image.GetHeight());
	for (int y = 0; y < (int)image.GetHeight(); ++y) {
		for (int x = 0; x < (int)image.GetWidth(); ++x) {
			if (image.GetPixelColor(x, y).To32() == 0) {
				printf("         ");
			} else {
				printf("%8.8X ", image.GetPixelColor(x, y).To32());
			}
		}
		printf("\n");
	}*/

	return (true);
}



int MacFontManager::GetCharWidth(wchar_t c) const {
	NSString* _font_name = MacLog::Encode(current_font_->name_);
	const float corrected_font_size = ((MacFont*)current_font_)->actual_size_;	// Similar to other platforms...
	TBC_APPLE_FONT* font = [TBC_APPLE_FONT fontWithName:_font_name size:corrected_font_size];
#ifdef LEPRA_IOS
	wchar_t temp_big_string[2] = {(wchar_t)c, 0};
	NSString* tmpString = MacLog::Encode(wstr(temp_big_string));
	CGSize __size = [tmpString sizeWithFont:font];
	--__size.width;
#else // !iOS
	UniChar temp_big_string[2] = {(UniChar)c, 0};
	CGGlyph glyph;
	CTFontGetGlyphsForCharacters((CTFontRef)font, temp_big_string, &glyph, 1);
	NSSize __size = [font advancementForGlyph:glyph];
#endif // iOS/!iOS
	return __size.width+1;
}

int MacFontManager::GetCharOffset(wchar_t c) const {
	return 0;	// TODO: implement! I.e. "j" should return a negative offset, since only the bottom bend of sans serifs goes left.
}



}
