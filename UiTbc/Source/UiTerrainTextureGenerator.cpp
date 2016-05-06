/*
	File:   TerrainTextureGenerator.cpp
	Class:  TerrainTextureGenerator
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/uiterraintexturegenerator.h"

namespace uitbc {

TerrainTextureGenerator::TerrainTextureGenerator() {
}

TerrainTextureGenerator::~TerrainTextureGenerator() {
	RangeList::iterator iter;
	for (iter = range_list_.begin(); iter != range_list_.end(); ++iter) {
		delete (*iter);
	}
}

bool TerrainTextureGenerator::AddNewRange(float max_level, float fade_range, Canvas* images,
	float* min_normal_y, float* max_normal_y, float* angular_fade_range, int num_images) {
	if (range_list_.empty() == true) {
		Range* _range = new Range(max_level, fade_range, images, min_normal_y, max_normal_y, angular_fade_range, num_images);
		range_list_.push_back(_range);
		return true;
	} else {
		Range* last_range = *(--range_list_.end());
		if (last_range->max_level_ >= max_level) {
			return false;
		}

		Range* _range = new Range(last_range->max_level_, max_level, fade_range, images, min_normal_y, max_normal_y, angular_fade_range, num_images);
		range_list_.push_back(_range);
		return true;
	}
}

/*void TerrainTextureGenerator::RenderTexture(const TerrainPatch& patch, Canvas& canvas) {
	canvas.CreateBuffer();
	canvas.Clear();

	float one_over_width = 1.0f / ((float)canvas.GetWidth() - 1);
	float one_over_height = 1.0f / ((float)canvas.GetHeight() - 1);

	vec3* _pos = 0;
	vec3* _normal = 0;

	for (unsigned y = 0; y < canvas.GetHeight(); y++) {
		float _v = (float)y * one_over_height;

		for (unsigned x = 0; x < canvas.GetWidth(); x++) {
			float _u = (float)x * one_over_width;

			vec3 _pos;
			vec3 _normal;

			patch.GetPosAndNormal(_u, _v, _pos, _normal);

			float r = 0;
			float g = 0;
			float b = 0;
			GetHeightDependentRGB(_pos, _normal, _u, _v, r, g, b);

			Color color((uint8)floor(r * 255.0f), (uint8)floor(g * 255.0f), (uint8)floor(b * 255.0f));
			canvas.SetPixelColor(x, y, color);
		}
	}

	if (_pos != 0) {
		delete[] _pos;
	}
	if (_normal != 0) {
		delete[] _normal;
	}
}*/

void TerrainTextureGenerator::GetHeightDependentRGB(const vec3& pos,
													const vec3& normal,
													float u, float v,
													float& r, float& g, float& b) {
	RangeList::iterator iter;
	for (iter = range_list_.begin(); iter != range_list_.end(); ++iter) {
		Range* _range = *iter;
		float min_min = _range->min_level_ - _range->fade_range_;
		float min_max = _range->min_level_ + _range->fade_range_;
		float max_min = _range->max_level_ - _range->fade_range_;
		float max_max = _range->max_level_ + _range->fade_range_;

		if (pos.y < _range->min_level_ && iter != range_list_.begin()) {
			RangeList::iterator prev(iter);
			--prev;
			Range* temp = *prev;
			min_min = _range->min_level_ - temp->fade_range_;
			min_max = _range->min_level_ + temp->fade_range_;
		}
		if (pos.y > _range->max_level_ && iter != range_list_.end()) {
			RangeList::iterator next(iter);
			++next;
			Range* temp = *next;
			max_min = _range->max_level_ - temp->fade_range_;
			max_max = _range->max_level_ + temp->fade_range_;
		}

		float min_diff = (pos.y - min_min) / (min_max - min_min);
		float max_diff = 1.0f - (pos.y - max_min) / (max_max - max_min);

		float red   = 0;
		float green = 0;
		float blue  = 0;
		GetAngleDependentRGB(_range, normal, u, v, red, green, blue);

		if (max_diff >= 0 && max_diff <= 1) {
			if (iter == range_list_.begin()) {
				r = red;
				g = green;
				b = blue;
			} else {
				float t = max_diff;
				r = t * red   + (1.0f - t) * r;
				g = t * green + (1.0f - t) * g;
				b = t * blue  + (1.0f - t) * b;
			}
		} else if(min_diff >= 0 && min_diff <= 1) {
			if (iter == range_list_.begin()) {
				r = red;
				g = green;
				b = blue;
			} else {
				float t = min_diff;
				r = t * red   + (1.0f - t) * r;
				g = t * green + (1.0f - t) * g;
				b = t * blue  + (1.0f - t) * b;
			}
		} else if(pos.y >= _range->min_level_ && pos.y < _range->max_level_) {
			r = red;
			g = green;
			b = blue;
		}
	}
}

void TerrainTextureGenerator::GetAngleDependentRGB(Range* range,
												   const vec3& normal,
												   float u, float v,
												   float& r, float& g, float& b) {
	for (int i = 0; i < range->num_images_; i++) {
		Color color;
		range->images_[i].GetPixelColor((unsigned)(u * (float)(range->images_[i].GetWidth() - 1)),
											 (unsigned)(v * (float)(range->images_[i].GetHeight() - 1)),
											 color);
		float min_min = range->min_normal_y_[i] - range->angular_fade_range_[i];
		float min_max = range->min_normal_y_[i] + range->angular_fade_range_[i];
		float max_min = range->max_normal_y_[i] - range->angular_fade_range_[i];
		float max_max = range->max_normal_y_[i] + range->angular_fade_range_[i];

		float min_diff = (normal.y - min_min) / (min_max - min_min);
		float max_diff = 1.0f - (normal.y - max_min) / (max_max - max_min);
		if (max_diff >= 0 && max_diff <= 1) {
			if (i == 0) {
				r = (float)color.red_   / 255.0f;
				g = (float)color.green_ / 255.0f;
				b = (float)color.blue_  / 255.0f;
			} else {
				float t = max_diff;
				r = t * (float)color.red_   / 255.0f + (1.0f - t) * r;
				g = t * (float)color.green_ / 255.0f + (1.0f - t) * g;
				b = t * (float)color.blue_  / 255.0f + (1.0f - t) * b;
			}
		} else if(min_diff >= 0 && min_diff <= 1) {
			if (i == 0) {
				r = (float)color.red_   / 255.0f;
				g = (float)color.green_ / 255.0f;
				b = (float)color.blue_  / 255.0f;
			} else {
				float t = min_diff;
				r = t * (float)color.red_   / 255.0f + (1.0f - t) * r;
				g = t * (float)color.green_ / 255.0f + (1.0f - t) * g;
				b = t * (float)color.blue_  / 255.0f + (1.0f - t) * b;
			}
		} else if(normal.y >= range->min_normal_y_[i] && normal.y < range->max_normal_y_[i]) {
			r = (float)color.red_   / 255.0f;
			g = (float)color.green_ / 255.0f;
			b = (float)color.blue_  / 255.0f;
		}
	}
}

TerrainTextureGenerator::Range::Range(float max_level,
									  float fade_range,
									  Canvas* images,
									  float* min_normal_y,
									  float* max_normal_y,
									  float* angular_fade_range,
									  int num_images) {
	// The first range begins at negative infinity.
	unsigned bits = 0xFF800000;
	min_level_ = *(float*)&bits;
	max_level_ = max_level;
	fade_range_ = fade_range;
	num_images_ = num_images;

	images_ = new Canvas[num_images_];
	min_normal_y_ = new float[num_images_];
	max_normal_y_ = new float[num_images_];
	angular_fade_range_ = new float[num_images_];

	for (int i = 0; i < num_images_; i++) {
		images_[i].Copy(images[i]);
		min_normal_y_[i] = min_normal_y[i];
		max_normal_y_[i] = max_normal_y[i];
		angular_fade_range_[i] = angular_fade_range[i];
	}
}

TerrainTextureGenerator::Range::Range(float min_level,
									  float max_level,
									  float fade_range,
									  Canvas* images,
									  float* min_normal_y,
									  float* max_normal_y,
									  float* angular_fade_range,
									  int num_images) {
	min_level_ = min_level;
	max_level_ = max_level;
	fade_range_ = fade_range;
	num_images_ = num_images;

	images_ = new Canvas[num_images_];
	min_normal_y_ = new float[num_images_];
	max_normal_y_ = new float[num_images_];
	angular_fade_range_ = new float[num_images_];

	for (int i = 0; i < num_images_; i++) {
		images_[i].Copy(images[i]);
		min_normal_y_[i] = min_normal_y[i];
		max_normal_y_[i] = max_normal_y[i];
		angular_fade_range_[i] = angular_fade_range[i];
	}
}

TerrainTextureGenerator::Range::~Range() {
	delete[] images_;
	delete[] min_normal_y_;
	delete[] max_normal_y_;
	delete[] angular_fade_range_;
}


}

