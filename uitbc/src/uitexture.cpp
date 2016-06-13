/*
	File:   Texture.cpp
	Class:  Texture
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uitbc.h"
#include "../include/uitexture.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/resourcetracker.h"
#include <math.h>

namespace uitbc {

Texture::Texture():
	num_mip_map_levels_(0),
	is_cube_map_(false),
	color_map_(0),
	alpha_map_(0),
	normal_map_(0),
	specular_map_(0),
	cube_map_pos_x_(0),
	cube_map_neg_x_(0),
	cube_map_pos_y_(0),
	cube_map_neg_y_(0),
	cube_map_pos_z_(0),
	cube_map_neg_z_(0) {
	LEPRA_ACQUIRE_RESOURCE(Texture);
}

Texture::Texture(const Canvas& color_map, Canvas::ResizeHint resize_hint, int generate_mip_map_levels):
	num_mip_map_levels_(0),
	is_cube_map_(false),
	color_map_(0),
	alpha_map_(0),
	normal_map_(0),
	specular_map_(0),
	cube_map_pos_x_(0),
	cube_map_neg_x_(0),
	cube_map_pos_y_(0),
	cube_map_neg_y_(0),
	cube_map_pos_z_(0),
	cube_map_neg_z_(0) {
	LEPRA_ACQUIRE_RESOURCE(Texture);
	Set(color_map, resize_hint, generate_mip_map_levels);
}

Texture::Texture(Texture* texture):
	num_mip_map_levels_(0),
	is_cube_map_(false),
	color_map_(0),
	alpha_map_(0),
	normal_map_(0),
	specular_map_(0),
	cube_map_pos_x_(0),
	cube_map_neg_x_(0),
	cube_map_pos_y_(0),
	cube_map_neg_y_(0),
	cube_map_pos_z_(0),
	cube_map_neg_z_(0) {
	LEPRA_ACQUIRE_RESOURCE(Texture);
	Copy(texture);
}

Texture::~Texture() {
	ClearAll();
	LEPRA_RELEASE_RESOURCE(Texture);
}

void Texture::ClearAll() {
	delete[] color_map_;
	color_map_ = 0;
	delete[] alpha_map_;
	alpha_map_ = 0;
	delete[] normal_map_;
	normal_map_ = 0;
	delete[] specular_map_;
	specular_map_ = 0;
	delete[] cube_map_pos_x_;
	cube_map_pos_x_ = 0;
	delete[] cube_map_neg_x_;
	cube_map_neg_x_ = 0;
	delete[] cube_map_pos_y_;
	cube_map_pos_y_ = 0;
	delete[] cube_map_neg_y_;
	cube_map_neg_y_ = 0;
	delete[] cube_map_pos_z_;
	cube_map_pos_z_ = 0;
	delete[] cube_map_neg_z_;
	cube_map_neg_z_ = 0;

	num_mip_map_levels_ = 0;
	is_cube_map_ = false;
}

void Texture::Copy(Texture* texture) {
	ClearAll();

	num_mip_map_levels_ = texture->num_mip_map_levels_;
	is_cube_map_       = texture->is_cube_map_;

	if (texture->color_map_ != 0) {
		color_map_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			color_map_[i].Copy(texture->color_map_[i]);
		}
	}

	if (texture->alpha_map_ != 0) {
		alpha_map_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			alpha_map_[i].Copy(texture->alpha_map_[i]);
		}
	}

	if (texture->normal_map_ != 0) {
		normal_map_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			normal_map_[i].Copy(texture->normal_map_[i]);
		}
	}

	if (texture->specular_map_ != 0) {
		specular_map_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			specular_map_[i].Copy(texture->specular_map_[i]);
		}
	}

	if (texture->cube_map_pos_x_ != 0) {
		cube_map_pos_x_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			cube_map_pos_x_[i].Copy(texture->cube_map_pos_x_[i]);
		}
	}

	if (texture->cube_map_neg_x_ != 0) {
		cube_map_neg_x_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			cube_map_neg_x_[i].Copy(texture->cube_map_neg_x_[i]);
		}
	}

	if (texture->cube_map_pos_y_ != 0) {
		cube_map_pos_y_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			cube_map_pos_y_[i].Copy(texture->cube_map_pos_y_[i]);
		}
	}

	if (texture->cube_map_neg_y_ != 0) {
		cube_map_neg_y_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			cube_map_neg_y_[i].Copy(texture->cube_map_neg_y_[i]);
		}
	}

	if (texture->cube_map_pos_z_ != 0) {
		cube_map_pos_z_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			cube_map_pos_z_[i].Copy(texture->cube_map_pos_z_[i]);
		}
	}

	if (texture->cube_map_neg_z_ != 0) {
		cube_map_neg_z_ = new Canvas[num_mip_map_levels_];
		for (int i = 0; i < num_mip_map_levels_; i++) {
			cube_map_neg_z_[i].Copy(texture->cube_map_neg_z_[i]);
		}
	}
}

void Texture::Set(const Canvas& color_map, Canvas::ResizeHint resize_hint, int num_levels, const Canvas* alpha_map, const Canvas* normal_map,
	const Canvas* specular_map, bool merge_color_with_alpha) {
	// The texture's dimensions must be a power of two.
	// Check if they are, otherwise set them to the nearest lower
	// power of two.
	int _width = color_map.GetWidth();
	int _height = color_map.GetHeight();
	int i;
	int num_width_bits = 0;
	int num_height_bits = 0;
	int nearest_pow2_width = 1;
	int nearest_pow2_height = 1;
	for (i = 0; i < (int)sizeof(int) * 8; i++) {
		if (nearest_pow2_width * 2 > _width)
			break;
		nearest_pow2_width <<= 1;
		num_width_bits++;
	}
	for (i = 0; i < (int)sizeof(int) * 8; i++) {
		if (nearest_pow2_height * 2 > _height)
			break;
		nearest_pow2_height <<= 1;
		num_height_bits++;
	}
	if (resize_hint == Canvas::kResizeCanvas) {
		if (nearest_pow2_width < _width) {
			nearest_pow2_width <<= 1;
		}
		if (nearest_pow2_height < _height) {
			nearest_pow2_height <<= 1;
		}
	}

	if (num_levels <= 0) {
		// Calculate how many levels there will be.
		num_levels = num_width_bits > num_height_bits ? num_width_bits : num_height_bits;
		num_levels++;
	}

	ClearAll();

	// Create all textures.
	color_map_ = new Canvas[num_levels];
	if (alpha_map != 0 && merge_color_with_alpha == false)
		alpha_map = new Canvas[num_levels];
	if (normal_map != 0)
		normal_map_ = new Canvas[num_levels];
	if (specular_map != 0)
		specular_map_ = new Canvas[num_levels];

	// And finally, resize them to their respective size.
	for (int level = 0; level < num_levels; level++) {
		_width  = (nearest_pow2_width  >> level);
		_height = (nearest_pow2_height >> level);

		// If the texture isn't square, either width or height
		// will become 0 at the smallest levels, which we can't
		// allow.
		if (_width == 0)
			_width = 1;
		if (_height == 0)
			_height = 1;

		color_map_[level].Copy(color_map);

		if (merge_color_with_alpha == true) {
			if (alpha_map != 0) {
				Canvas _alpha_map(*alpha_map);
				_alpha_map.Resize(color_map_[level].GetWidth(), color_map_[level].GetHeight(), resize_hint);
				color_map_[level].ConvertTo32BitWithAlpha(_alpha_map);
			} else
				color_map_[level].ConvertBitDepth(Canvas::kBitdepth32Bit);
		} else if(alpha_map != 0) {
			// Alpha map is stored separately and color map keeps its bitrate.
			alpha_map_[level].Copy(*alpha_map);
			alpha_map_[level].ConvertToGrayscale();
			alpha_map_[level].Resize(_width, _height, resize_hint);
		}

		color_map_[level].Resize(_width, _height, resize_hint);

		if (normal_map != 0) {
			normal_map_[level].Copy(*normal_map);
			normal_map_[level].ConvertBitDepth(Canvas::kBitdepth24Bit);
			normal_map_[level].Resize(_width, _height, resize_hint);
		}
		if (specular_map != 0) {
			specular_map_[level].Copy(*specular_map);
			specular_map_[level].ConvertToGrayscale();
			specular_map_[level].Resize(_width, _height, resize_hint);
		}
	}

	num_mip_map_levels_ = num_levels;
	is_cube_map_ = false;
}

void Texture::Set(const Canvas& cube_map_pos_x,
		  const Canvas& cube_map_neg_x,
		  const Canvas& cube_map_pos_y,
		  const Canvas& cube_map_neg_y,
		  const Canvas& cube_map_pos_z,
		  const Canvas& cube_map_neg_z) {
	ClearAll();

	// The texture's dimensions must be a power of two.
	// Check if they are, otherwise set them to the nearest lower
	// power of two.
	int _width = cube_map_pos_x.GetWidth();
	int _height = cube_map_pos_x.GetHeight();
	int i;
	int num_width_bits = 0;
	int num_height_bits = 0;
	int nearest_pow2_width = 1;
	int nearest_pow2_height = 1;
	for (i = 0; i < (int)sizeof(int) * 8; i++) {
		if (nearest_pow2_width * 2 > _width)
			break;
		nearest_pow2_width <<= 1;
		num_width_bits++;
	}
	for (i = 0; i < (int)sizeof(int) * 8; i++) {
		if (nearest_pow2_height * 2 > _height)
			break;
		nearest_pow2_height <<= 1;
		num_height_bits++;
	}

	// First calculate how many levels there will be.
	unsigned int _num_levels = num_width_bits > num_height_bits ? num_width_bits : num_height_bits;
	_num_levels++;

	// Now create all textures.
	cube_map_pos_x_ = new Canvas[_num_levels];
	cube_map_neg_x_ = new Canvas[_num_levels];
	cube_map_pos_y_ = new Canvas[_num_levels];
	cube_map_neg_y_ = new Canvas[_num_levels];
	cube_map_pos_z_ = new Canvas[_num_levels];
	cube_map_neg_z_ = new Canvas[_num_levels];

	// And finally, resize them to their respective size.
	for (unsigned int level = 0; level < _num_levels; level++) {
		_width  = (nearest_pow2_width  >> level);
		_height = (nearest_pow2_height >> level);

		// If the texture isn't square, either width or height
		// will become 0 at the smallest levels, which we can't
		// allow.
		if (_width == 0)
			_width = 1;
		if (_height == 0)
			_height = 1;

		cube_map_pos_x_[level].Copy(cube_map_pos_x);
		cube_map_neg_x_[level].Copy(cube_map_neg_x);
		cube_map_pos_y_[level].Copy(cube_map_pos_y);
		cube_map_neg_y_[level].Copy(cube_map_neg_y);
		cube_map_pos_z_[level].Copy(cube_map_pos_z);
		cube_map_neg_z_[level].Copy(cube_map_neg_z);

		cube_map_pos_x_[level].Resize(_width, _height, Canvas::kResizeNicest);
		cube_map_neg_x_[level].Resize(_width, _height, Canvas::kResizeNicest);
		cube_map_pos_y_[level].Resize(_width, _height, Canvas::kResizeNicest);
		cube_map_neg_y_[level].Resize(_width, _height, Canvas::kResizeNicest);
		cube_map_pos_z_[level].Resize(_width, _height, Canvas::kResizeNicest);
		cube_map_neg_z_[level].Resize(_width, _height, Canvas::kResizeNicest);
	}

	num_mip_map_levels_ = _num_levels;
	is_cube_map_ = true;
}

void Texture::Prepare(bool color_map,
		      bool alpha_map,
		      bool normal_map,
		      bool specular_map,
		      bool cube_map,
		      unsigned int width,
		      unsigned int height) {
	ClearAll();

	// The texture's dimensions must be a power of two.
	// Check if they are, otherwise set them to the nearest lower
	// power of two.
	unsigned int i;
	unsigned int num_width_bits = 0;
	unsigned int num_height_bits = 0;
	unsigned int nearest_pow2_width = 1;
	unsigned int nearest_pow2_height = 1;
	for (i = 0; i < sizeof(int) * 8; i++) {
		if (nearest_pow2_width * 2 > width)
			break;
		nearest_pow2_width <<= 1;
		num_width_bits++;
	}
	for (i = 0; i < sizeof(int) * 8; i++) {
		if (nearest_pow2_height * 2 > height)
			break;
		nearest_pow2_height <<= 1;
		num_height_bits++;
	}

	// First calculate how many levels there will be.
	unsigned int _num_levels = num_width_bits > num_height_bits ? num_width_bits : num_height_bits;
	_num_levels++;

	num_mip_map_levels_ = _num_levels;
	is_cube_map_ = cube_map;

	if (cube_map == false) {
		if (color_map == true)
			color_map_ = new Canvas[_num_levels];
		if (alpha_map == true)
			alpha_map_ = new Canvas[_num_levels];
		if (normal_map == true)
			normal_map_ = new Canvas[_num_levels];
		if (specular_map == true)
			specular_map_ = new Canvas[_num_levels];

		for (unsigned int level = 0; level < _num_levels; level++) {
			unsigned int _width  = (nearest_pow2_width  >> level);
			unsigned int _height = (nearest_pow2_height >> level);

			// If the texture isn't square, either width or height
			// will become 0 at the smallest levels, which we can't
			// allow.
			if (_width == 0)
				_width = 1;
			if (_height == 0)
				_height = 1;

			if (color_map == true) {
				color_map_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
				color_map_[level].CreateBuffer();
			}

			if (alpha_map == true) {
				alpha_map_[level].Reset(_width, _height, Canvas::kBitdepth8Bit);
				alpha_map_[level].CreateBuffer();
				alpha_map_[level].SetGrayscalePalette();
			}

			if (normal_map == true) {
				normal_map_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
				normal_map_[level].CreateBuffer();
			}

			if (specular_map == true) {
				specular_map_[level].Reset(_width, _height, Canvas::kBitdepth8Bit);
				specular_map_[level].CreateBuffer();
				specular_map_[level].SetGrayscalePalette();
			}
		}
	} else {
		cube_map_pos_x_ = new Canvas[_num_levels];
		cube_map_neg_x_ = new Canvas[_num_levels];
		cube_map_pos_y_ = new Canvas[_num_levels];
		cube_map_neg_y_ = new Canvas[_num_levels];
		cube_map_pos_z_ = new Canvas[_num_levels];
		cube_map_neg_z_ = new Canvas[_num_levels];

		for (unsigned int level = 0; level < _num_levels; level++) {
			unsigned int _width  = (nearest_pow2_width  >> level);
			unsigned int _height = (nearest_pow2_height >> level);

			// If the texture isn't square, either width or height
			// will become 0 at the smallest levels, which we can't
			// allow.
			if (_width == 0)
				_width = 1;
			if (_height == 0)
				_height = 1;

			cube_map_pos_x_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
			cube_map_neg_x_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
			cube_map_pos_y_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
			cube_map_neg_y_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
			cube_map_pos_z_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);
			cube_map_neg_z_[level].Reset(_width, _height, Canvas::kBitdepth24Bit);

			cube_map_pos_x_[level].CreateBuffer();
			cube_map_neg_x_[level].CreateBuffer();
			cube_map_pos_y_[level].CreateBuffer();
			cube_map_neg_y_[level].CreateBuffer();
			cube_map_pos_z_[level].CreateBuffer();
			cube_map_neg_z_[level].CreateBuffer();
		}
	}
}

const Canvas* Texture::GetColorMap(unsigned int mip_map_level) const {
	if (color_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &color_map_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetAlphaMap(unsigned int mip_map_level) const {
	if (alpha_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &alpha_map_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetNormalMap(unsigned int mip_map_level) const {
	if (normal_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &normal_map_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetSpecularMap(unsigned int mip_map_level) const {
	if (specular_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &specular_map_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetCubeMapPosX(unsigned int mip_map_level) const {
	if (cube_map_pos_x_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_pos_x_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetCubeMapNegX(unsigned int mip_map_level) const {
	if (cube_map_neg_x_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_neg_x_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetCubeMapPosY(unsigned int mip_map_level) const {
	if (cube_map_pos_y_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_pos_y_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetCubeMapNegY(unsigned int mip_map_level) const {
	if (cube_map_neg_y_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_neg_y_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetCubeMapPosZ(unsigned int mip_map_level) const {
	if (cube_map_pos_z_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_pos_z_[mip_map_level];

	return 0;
}

const Canvas* Texture::GetCubeMapNegZ(unsigned int mip_map_level) const {
	if (cube_map_neg_z_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_neg_z_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetColorMap(unsigned int mip_map_level) {
	if (color_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &color_map_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetAlphaMap(unsigned int mip_map_level) {
	if (alpha_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &alpha_map_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetNormalMap(unsigned int mip_map_level) {
	if (normal_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &normal_map_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetSpecularMap(unsigned int mip_map_level) {
	if (specular_map_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &specular_map_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetCubeMapPosX(unsigned int mip_map_level) {
	if (cube_map_pos_x_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_pos_x_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetCubeMapNegX(unsigned int mip_map_level) {
	if (cube_map_neg_x_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_neg_x_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetCubeMapPosY(unsigned int mip_map_level) {
	if (cube_map_pos_y_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_pos_y_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetCubeMapNegY(unsigned int mip_map_level) {
	if (cube_map_neg_y_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_neg_y_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetCubeMapPosZ(unsigned int mip_map_level) {
	if (cube_map_pos_z_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_pos_z_[mip_map_level];

	return 0;
}

Canvas* Texture::_GetCubeMapNegZ(unsigned int mip_map_level) {
	if (cube_map_neg_z_ != 0 && mip_map_level < (unsigned int)num_mip_map_levels_)
		return &cube_map_neg_z_[mip_map_level];

	return 0;
}

unsigned int Texture::GetPixelByteSize() const {
	if (color_map_ != 0)
		return color_map_[0].GetPixelByteSize();
	else
		return cube_map_pos_x_[0].GetPixelByteSize();
}

void Texture::SwapRGBOrder() {
	int i;

	if (color_map_ != 0) {
		for (i = 0; i < GetNumMipMapLevels(); i++) {
			color_map_[i].SwapRGBOrder();
			if (specular_map_ != 0)
				specular_map_[i].SwapRGBOrder();
			if (normal_map_ != 0)
				normal_map_[i].SwapRGBOrder();
		}
	} else {
		for (i = 0; i < GetNumMipMapLevels(); i++) {
			cube_map_pos_x_[i].SwapRGBOrder();
			cube_map_neg_x_[i].SwapRGBOrder();
			cube_map_pos_y_[i].SwapRGBOrder();
			cube_map_neg_y_[i].SwapRGBOrder();
			cube_map_pos_z_[i].SwapRGBOrder();
			cube_map_neg_z_[i].SwapRGBOrder();
		}
	}
}

void Texture::ConvertBitDepth(Canvas::BitDepth bit_depth) {
	if (color_map_ != 0) {
		for (int i = 0; i < num_mip_map_levels_; i++) {
			color_map_[i].ConvertBitDepth(bit_depth);
		}
	}
}

void Texture::GenerateNormalMap(Canvas& normal_map, const Canvas& bump_map, double scale) {
	Canvas _bump_map(bump_map, true);
	_bump_map.ConvertToGrayscale();

	normal_map.Reset(_bump_map.GetWidth(), _bump_map.GetHeight(), Canvas::kBitdepth24Bit);
	normal_map.CreateBuffer();

	for (unsigned y = 0; y < _bump_map.GetHeight(); y++) {
		for (unsigned x = 0; x < _bump_map.GetWidth(); x++) {
			double north_value = 0;
			double south_value = 0;
			double east_value  = 0;
			double west_value  = 0;

			if (y > 0) {
				Color color;
				_bump_map.GetPixelColor(x, y - 1, color);
				north_value = (double)color.red_ * scale;
			}
			if (y < _bump_map.GetHeight() - 1) {
				Color color;
				_bump_map.GetPixelColor(x, y + 1, color);
				south_value = (double)color.red_ * scale;
			}
			if (x > 0) {
				Color color;
				_bump_map.GetPixelColor(x - 1, y, color);
				west_value = (double)color.red_ * scale;
			}
			if (x < _bump_map.GetWidth() - 1) {
				Color color;
				_bump_map.GetPixelColor(x + 1, y, color);
				east_value = (double)color.red_ * scale;
			}

			double nx = sin(atan(-(east_value - west_value) / 255.0));
			double ny = sin(atan((south_value - north_value) / 255.0));
			double nz = 1.0 - (nx * nx + ny * ny);

			Color color((uint8)(nx * 127.0) + 128, (uint8)(ny * 127.0) + 128, (uint8)(nz * 127.0) + 128);
			normal_map.SetPixelColor(x, y, color);
		}
	}
}

}
