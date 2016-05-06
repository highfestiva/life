
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine
//
// NOTES:
//
// This class manages all images used in the GUI. Since it is so
// tightly connected to the GUI, there are some things I want to mention
// about how it is supposed to be used:
//
// Assumption 1: All GUI components always have exactly ONE DesktopWindow at
//               the top level.
// Assumption 2: The DesktopWindow contains one and only one instance of this
//               class, OR (exclusively or) one and only one pointer to it.
// Assumption 3: All components reach their corresponding GUIImageManager
//               through their one and only parent DesktopWindow.
//
// The GUIImageManager must be created before any GUI components are created.
// The DesktopWindow must be created either directly after it or at the same
// time (by creating the GUIImageManager in the constructor of DesktopWindow).
// Due to how DesktopWindow handles rendering, it must be responsible of setting
// the pointer to the painter. To be 100% safe from bugs, this should be done
// before any other GUI components are created.



#pragma once

#include "../../../lepra/include/hashtable.h"
#include "../uipainter.h"
#include "../uitbc.h"




namespace uitbc {



class GUIImageManager {
public:
	enum ImageStyle {
		kTiled = 0,
		kCentered,
		kStretched,
	};

	enum BlendFunc {
		kNoBlend = 0,
		kAlphatest,
		kAlphablend
	};

	GUIImageManager();
	~GUIImageManager();

	void SetPainter(Painter* painter);
	Painter* GetPainter() const;

	// TODO: come up with a way to avoid using an image loader directly in a
	// void LoadImages(const char* image_definition_file, const char* archive = 0);
	Painter::ImageID AddImage(const Canvas& image, ImageStyle style, BlendFunc blend_func, uint8 alpha_value);
	void AddLoadedImage(const Canvas& image, Painter::ImageID image_id, ImageStyle style, BlendFunc blend_func, uint8 alpha_value);
	bool RemoveImage(Painter::ImageID image_id);
	bool DropImage(Painter::ImageID image_id);
	bool HasImage(Painter::ImageID image_id) const;

	void SetImageOffset(Painter::ImageID image_id, int x_offset, int y_offset);

	// Translates the name of the image to the corresponding ImageID.
	// The name is the name of the structure in the image definition file.
	Painter::ImageID GetImageID(const str& image_name);

	void DrawImage(Painter::ImageID image_id, int x, int y);
	void DrawImage(Painter::ImageID image_id, const PixelRect& rect);

	PixelCoord GetImageSize(Painter::ImageID image_id);
	bool IsOverImage(Painter::ImageID image_id,
			 int screen_x, int screen_y,
			 const PixelRect& screen_rect);

	void SwapRGB();

private:
	void ClearImageTable();

	class Image {
	public:
		inline Image(Painter::ImageID id,
					 const Canvas& canvas,
					 ImageStyle style,
					 BlendFunc blend_func,
					 uint8 alpha_value) {
			id_ = id;
			canvas_.Copy(canvas);
			style_ = style;
			blend_func_ = blend_func;
			alpha_value_ = alpha_value;
			x_offset_ = 0;
			y_offset_ = 0;
		}

		Painter::ImageID id_;
		Canvas canvas_;
		ImageStyle style_;
		BlendFunc blend_func_;
		uint8 alpha_value_;

		int x_offset_;
		int y_offset_;
	};

	// An image table used by all components that need to store icons and
	// other common images.
	typedef HashTable<Painter::ImageID, Image*, std::hash<int> > ImageTable;
	typedef HashTable<str, Painter::ImageID> IDTable;
	ImageTable image_table_;
	IDTable id_table_;

	Painter* painter_;

	bool swap_rgb_;
};



}
