/*
	File:   Texture.h
	Class:  Texture
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

#include "../../lepra/include/canvas.h"

namespace uitbc {

class Texture {
public:

	friend class TEXLoader;

	enum { // Map.
		kColorMap = 0,
		kAlphaMap,
		kNormalMap,
		kSpecularMap,
		kCubeMap,

		kNumMaps
	};

	// The constructor makes a copy of the data, and is not responsible
	// for deleting the canvas.
	Texture();
	Texture(const Canvas& color_map, Canvas::ResizeHint resize_hint, int generate_mip_map_levels);
	Texture(Texture* texture);
	virtual ~Texture();

	void ClearAll();

	void ConvertBitDepth(Canvas::BitDepth bit_depth);

	// Normal texture set. If both specular and normal maps are used, they will be merged
	// such that the specular value will be stored in the alpha-channel of the normal map.
	void Set(const Canvas& color_map, Canvas::ResizeHint resize_hint = Canvas::kResizeFast, int num_levels = 0, const Canvas* alpha_map = 0, const Canvas* normal_map = 0,
		const Canvas* specular_map = 0, bool merge_color_with_alpha = true);

	// Set cube map.
	void Set(const Canvas& cube_map_pos_x,			// Required.
		 const Canvas& cube_map_neg_x,			// Required.
		 const Canvas& cube_map_pos_y,			// Required.
		 const Canvas& cube_map_neg_y,			// Required.
		 const Canvas& cube_map_pos_z,			// Required.
		 const Canvas& cube_map_neg_z);			// Required.

	void Copy(Texture* texture);

	// Level 0 is the largest one...
	inline int GetNumMipMapLevels() const;

	inline bool IsCubeMap() const;

	// Color map contains alpha channel.
	const Canvas* GetColorMap   (unsigned int mip_map_level) const;
	const Canvas* GetAlphaMap   (unsigned int mip_map_level) const;
	const Canvas* GetNormalMap  (unsigned int mip_map_level) const;
	const Canvas* GetSpecularMap(unsigned int mip_map_level) const;

	const Canvas* GetCubeMapPosX (unsigned int mip_map_level) const;
	const Canvas* GetCubeMapNegX (unsigned int mip_map_level) const;
	const Canvas* GetCubeMapPosY (unsigned int mip_map_level) const;
	const Canvas* GetCubeMapNegY (unsigned int mip_map_level) const;
	const Canvas* GetCubeMapPosZ (unsigned int mip_map_level) const;
	const Canvas* GetCubeMapNegZ (unsigned int mip_map_level) const;

	inline unsigned int GetWidth(int mip_map_level = 0) const;
	inline unsigned int GetHeight(int mip_map_level = 0) const;
	unsigned int GetPixelByteSize() const;	// Returns the pixel size in bytes.

	// Generate a normal map from a grayscale bump map. The scale parameter is multiplied
	// with the bumpmap, which can control the steepness of the edges in the bumpmap.
	static void GenerateNormalMap(Canvas& normal_map, const Canvas& bump_map, double scale = 1.0);

	void SwapRGBOrder();
private:

	// Functions used by TEXLoader.
	void Prepare(bool color_map,
		     bool alpha_map,
		     bool normal_map,
		     bool specular_map,
		     bool cube_map,
		     unsigned int width,
		     unsigned int height);

	Canvas* _GetColorMap   (unsigned int mip_map_level);
	Canvas* _GetAlphaMap   (unsigned int mip_map_level);
	Canvas* _GetNormalMap  (unsigned int mip_map_level);
	Canvas* _GetSpecularMap(unsigned int mip_map_level);

	Canvas* _GetCubeMapPosX (unsigned int mip_map_level);
	Canvas* _GetCubeMapNegX (unsigned int mip_map_level);
	Canvas* _GetCubeMapPosY (unsigned int mip_map_level);
	Canvas* _GetCubeMapNegY (unsigned int mip_map_level);
	Canvas* _GetCubeMapPosZ (unsigned int mip_map_level);
	Canvas* _GetCubeMapNegZ (unsigned int mip_map_level);



	int num_mip_map_levels_;
	bool is_cube_map_;

	Canvas* color_map_;
	Canvas* alpha_map_;
	Canvas* normal_map_;
	Canvas* specular_map_;

	Canvas* cube_map_pos_x_;
	Canvas* cube_map_neg_x_;
	Canvas* cube_map_pos_y_;
	Canvas* cube_map_neg_y_;
	Canvas* cube_map_pos_z_;
	Canvas* cube_map_neg_z_;
};

int Texture::GetNumMipMapLevels() const {
	return num_mip_map_levels_;
}

bool Texture::IsCubeMap() const {
	return is_cube_map_;
}

unsigned int Texture::GetWidth(int mip_map_level) const {
	if (mip_map_level >= 0 && mip_map_level < num_mip_map_levels_)
		return color_map_[mip_map_level].GetWidth();
	else
		return 0;
}

unsigned int Texture::GetHeight(int mip_map_level) const {
	if (mip_map_level >= 0 && mip_map_level < num_mip_map_levels_)
		return color_map_[mip_map_level].GetHeight();
	else
		return 0;
}


}
