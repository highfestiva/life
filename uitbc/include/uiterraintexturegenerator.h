/*
	Class:  TerrainTextureGenerator
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#pragma once

//#include "UiTerrainPatch.h"	// TRICKY: this must be before uitbc include due to a bug in MSVC8.
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/vector3d.h"
#include "uitbc.h"
#include <list>

namespace uitbc {

class TerrainTextureGenerator {
public:

	TerrainTextureGenerator();
	virtual ~TerrainTextureGenerator();

	// Adds a new range on top of the previous one. max_level must be higher than
	// the previous range's max level. Returns false if that's not the case.
	// The number of images, and the numbers of elements in the two arrays min_normal_y
	// and max_normal_y, must be the same as the value of num_images.
	// min_normal_y and max_normal_y specifies the range within which the corresponding
	// image should be rendered, and FadeRange specifies the range within which to
	// fade between two images. Setting this to zero will result in very sharp edges
	// between two textures, while a higher value will result in smoother transitions.
	bool AddNewRange(float max_level,          // Maximum height for this range
					 float fade_range,         // Height dependent fading.
					 Canvas* images,
					 float* min_normal_y,       // Defines the angle for each image.
					 float* max_normal_y,       // ...
					 float* angular_fade_range, // Angular fading between images.
					 int num_images);

	// Renders the final texture to match patch. The canvas must be initialized with
	// the desired resolution of the texture.
//	void RenderTexture(const TerrainPatch& patch, Canvas& canvas);

private:

	class Range {
	public:
		Range(float max_level,
			  float fade_range,
			  Canvas* images,
			  float* min_normal_y,
			  float* max_normal_y,
			  float* angular_fade_range,
			  int num_images);
		Range(float min_level,
			  float max_level,
			  float fade_range,
			  Canvas* images,
			  float* min_normal_y,
			  float* max_normal_y,
			  float* angular_fade_range,
			  int num_images);
		~Range();

		float min_level_;
		float max_level_;
		float fade_range_;
		float* min_normal_y_;
		float* max_normal_y_;
		float* angular_fade_range_;
		Canvas* images_;
		int num_images_;
	};

	typedef std::list<Range*> RangeList;

	void GetAngleDependentRGB(Range* range, const vec3& normal,
		float u, float v, float& r, float& g, float& b);
	void GetHeightDependentRGB(const vec3& pos, const vec3& normal,
		float u, float v, float& r, float& g, float& b);

	RangeList range_list_;
	RangeList::iterator current_range_iter_;
};

}
