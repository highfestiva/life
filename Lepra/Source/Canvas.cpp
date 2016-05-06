/*
	Class:  Canvas
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include <math.h>
#include <memory.h>
#include "../include/canvas.h"
#include "../include/gammalookup.h"
#include "../include/math.h"

#ifdef LEPRA_MSVC
#pragma warning(disable: 4244)	// Compiler bug (2003 .NET), unable to circumvent, causes conversion - possible loss of data.
#endif // LEPRA_MSVC

namespace lepra {

bool Canvas::resize_data_initialized_ = false;

const int Canvas::sampling_grid_size_ = 8;
const float32 Canvas::sampling_grid_point_space_ = 0.25f;
const float32 Canvas::resize_sharpness_ = 0.5f;

float32 Canvas::sampling_grid_weight_[Canvas::sampling_grid_size_ * Canvas::sampling_grid_size_];
float32 Canvas::sampling_grid_coord_look_up_[Canvas::sampling_grid_size_];

Canvas::Canvas() {
	buffer_responsibility_ = false;
	buffer_ = 0;
	bit_depth_ = kBitdepth8Bit;
	width_ = 0;
	height_ = 0;
	pitch_ = 0;
	pixel_size_ = 1;
	output_rotation_ = 0;
	device_rotation_ = 0;
}

Canvas::Canvas(unsigned width, unsigned height, BitDepth bit_depth) {
	buffer_responsibility_ = false;
	buffer_ = 0;
	bit_depth_ = bit_depth;
	width_ = width;
	height_ = height;
	pitch_ = width;
	output_rotation_ = 0;
	device_rotation_ = 0;

	switch(bit_depth_) {
	case kBitdepth8Bit:
		pixel_size_ = 1;
		break;
	case kBitdepth15Bit:
	case kBitdepth16Bit:
		pixel_size_ = 2;
		break;
	case kBitdepth24Bit:
		pixel_size_ = 3;
		break;
	case kBitdepth32Bit:
		pixel_size_ = 4;
		break;
	case kBitdepth16BitPerChannel:
		pixel_size_ = 6;
		break;
	case kBitdepth32BitPerChannel:
		pixel_size_ = 12;
		break;
	}
}

Canvas::Canvas(const Canvas& canvas, bool copy) {
	bit_depth_ = canvas.GetBitDepth();
	width_ = canvas.GetWidth();
	height_ = canvas.GetHeight();
	pitch_ = canvas.GetPitch();
	output_rotation_ = 0;
	device_rotation_ = 0;

	SetPalette(canvas.GetPalette());

	if (copy == true) {
		unsigned _pixel_size;
		buffer_ = CreateBuffer(pitch_, height_, canvas.GetBitDepth(), _pixel_size);

		if (buffer_ != 0) {
			pixel_size_ = _pixel_size;
			memcpy(buffer_, canvas.GetBuffer(), height_ * pitch_ * pixel_size_);
			buffer_responsibility_ = true;
		}
	} else {
		buffer_ = canvas.GetBuffer();
		buffer_responsibility_ = false;
	}
}

Canvas::~Canvas() {
	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
	}
}

void* Canvas::CreateBuffer(unsigned width, unsigned height, BitDepth bit_depth, unsigned& pixel_size) {
	switch(bit_depth) {
	case kBitdepth8Bit:
		pixel_size = 1;
		return new char[width * height];
		break;
	case kBitdepth15Bit:
	case kBitdepth16Bit:
		pixel_size = 2;
		return new short[width * height];
		break;
	case kBitdepth24Bit:
		pixel_size = 3;
		return new char[width * height * 3];
		break;
	case kBitdepth32Bit:
		pixel_size = 4;
		return new uint32[width * height];
		break;
	case kBitdepth16BitPerChannel:
		pixel_size = 6;
		return new short[width * height * 3];
		break;
	case kBitdepth32BitPerChannel:
		pixel_size = 12;
		return new uint32[width * height * 3];
		break;
	}

	return 0;
}


void Canvas::Copy(const Canvas& canvas) {
	Reset(canvas.GetWidth(), canvas.GetHeight(), canvas.GetBitDepth());
	SetPitch(canvas.GetPitch());

	if (!canvas.GetBuffer()) {
		return;
	}

	unsigned _pixel_size;
	buffer_ = CreateBuffer(pitch_, height_, canvas.GetBitDepth(), _pixel_size);

	if (buffer_ != 0) {
		pixel_size_ = _pixel_size;
		memcpy(buffer_, canvas.GetBuffer(), height_ * pitch_ * pixel_size_);
		buffer_responsibility_ = true;
	}

	SetPalette(canvas.GetPalette());
}

void Canvas::PartialCopy(int _x, int _y, const Canvas& canvas) {
	for (unsigned y = 0; y < canvas.GetHeight(); ++y) {
		for (unsigned x = 0; x < canvas.GetWidth(); ++x) {
			lepra::Color _color = canvas.GetPixelColor(x, y);
			SetPixelColor(_x+x, _y+y, _color);
		}
	}
}

void Canvas::Reset(unsigned width, unsigned height, BitDepth bit_depth) {
	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
		buffer_ = 0;
	}
	buffer_responsibility_ = false;
	buffer_ = 0;
	bit_depth_ = bit_depth;
	width_ = width;
	height_ = height;
	pitch_ = width;

	switch(bit_depth_) {
	case kBitdepth8Bit:
		pixel_size_ = 1;
		break;
	case kBitdepth15Bit:
	case kBitdepth16Bit:
		pixel_size_ = 2;
		break;
	case kBitdepth24Bit:
		pixel_size_ = 3;
		break;
	case kBitdepth32Bit:
		pixel_size_ = 4;
		break;
	case kBitdepth16BitPerChannel:
		pixel_size_ = 6;
		break;
	case kBitdepth32BitPerChannel:
		pixel_size_ = 12;
		break;
	}
}

void Canvas::Reset(const Canvas& canvas) {
	Reset(canvas.GetWidth(), canvas.GetHeight(), canvas.GetBitDepth());
	SetPitch(canvas.GetPitch());
	SetPalette(canvas.GetPalette());
}

void Canvas::SetBuffer(void* buffer, bool copy, bool take_ownership) {
	if (copy == true) {
		// Only create a buffer if we don't already have one.
		if (buffer_responsibility_ == false) {
			unsigned _pixel_size;
			buffer_ = CreateBuffer(pitch_, height_, bit_depth_, _pixel_size);

			if (buffer_ != 0) {
				pixel_size_ = _pixel_size;
				buffer_responsibility_ = true;

				memcpy(buffer_, buffer, pitch_ * height_ * pixel_size_);
			}
		}
	} else {
		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}

		buffer_ = buffer;

		buffer_responsibility_ = take_ownership;
	}
}

void Canvas::CreateBuffer() {
	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
		buffer_responsibility_ = false;
	}

	unsigned _pixel_size;
	buffer_ = CreateBuffer(pitch_, height_, bit_depth_, _pixel_size);

	if (buffer_ != 0) {
		pixel_size_ = _pixel_size;
		buffer_responsibility_ = true;
	}
}

void Canvas::SetPalette(const Color* palette) {
	if (palette == 0) {
		return;
	}

	for (int i = 0; i < 256; i++) {
		palette_[i].red_   = palette[i].red_;
		palette_[i].green_ = palette[i].green_;
		palette_[i].blue_  = palette[i].blue_;
		palette_[i].alpha_ = palette[i].alpha_;
	}
}

void Canvas::SetGrayscalePalette() {
	for (int i = 0; i < 256; i++) {
		palette_[i].red_   = (uint8)i;
		palette_[i].green_ = (uint8)i;
		palette_[i].blue_  = (uint8)i;
		palette_[i].alpha_ = 255;
	}
}

unsigned Canvas::PowerUp(unsigned value) {
	if (value == 0) {
		return 1;
	}
	unsigned i = ((unsigned)1 << 31);
	while (i > value) {
		i >>= 1;
	}
	if (i == value) {
		// The height is already a power of two.
		return value;
	} else {
		// Return the next higher power of two.
		return (i << 1);
	}
}

void Canvas::Crop(int top_left_x,
		  int top_left_y,
		  int bottom_right_x,
		  int bottom_right_y) {
	if (bottom_right_x < top_left_x) {
		int t = bottom_right_x;
		bottom_right_x = top_left_x;
		top_left_x = t;
	}
	if (bottom_right_y < top_left_y) {
		int t = bottom_right_y;
		bottom_right_y = top_left_y;
		top_left_y = t;
	}

	unsigned _new_width = bottom_right_x - top_left_x;
	unsigned _new_height = bottom_right_y - top_left_y;
	unsigned old_width = width_;
	unsigned old_height = height_;

	unsigned _pixel_size;
	uint8* _buffer = (uint8*)CreateBuffer(_new_width, _new_height, bit_depth_, _pixel_size);

	if (_buffer == 0) {
		return;
	}

	// Clear the new buffer.
	if (bit_depth_ != kBitdepth32BitPerChannel) {
		memset(_buffer, 0, _new_width * _new_height * _pixel_size);
	} else {
		memset(_buffer, 0, _new_width * _new_height * 3 * sizeof(float32));
	}

	int src_start_x = top_left_x;
	int src_start_y = top_left_y;
	int dest_start_x = 0;
	int dest_start_y = 0;

	int loop_end_x = bottom_right_x;
	int loop_end_y = bottom_right_y;

	if (top_left_x < 0) {
		src_start_x = 0;
		dest_start_x = -top_left_x;
	}

	if (bottom_right_x > (int)old_width) {
		loop_end_x = old_width;
	}

	if (top_left_y < 0) {
		src_start_y = 0;
		dest_start_y = -top_left_y;
	}

	if (bottom_right_y > (int)old_height) {
		loop_end_y = old_height;
	}

	int loop_width  = (loop_end_x - src_start_x) * _pixel_size;
	int loop_height = loop_end_y - src_start_y;

	for (int y = 0; y < loop_height; y++) {
		unsigned src_y_offset = (src_start_y + y) * pitch_ * _pixel_size;
		unsigned dest_y_offset = (dest_start_y + y) * _new_width * _pixel_size;

		::memcpy(&_buffer[dest_y_offset + dest_start_x * _pixel_size], &((uint8*)buffer_)[src_y_offset + src_start_x * _pixel_size], loop_width);
	}

	Reset(_new_width, _new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::FlipHorizontal() {
	unsigned _pixel_size;
	char* _buffer = (char*)CreateBuffer(pitch_, height_, bit_depth_, _pixel_size);

	if (_buffer == 0) {
		return;
	}

	for (unsigned y = 0; y < height_; y++) {
		unsigned y_offset = y * pitch_;

		switch(bit_depth_) {
		case kBitdepth8Bit: {
				for (unsigned x = 0; x < width_; x++) {
					_buffer[y_offset + x] = ((uint8*)buffer_)[y_offset + (width_ - (x + 1))];
				} break;
			}
		case kBitdepth15Bit:
		case kBitdepth16Bit: {
				for (unsigned x = 0; x < width_; x++) {
					((short*)_buffer)[y_offset + x] = ((short*)buffer_)[y_offset + (width_ - (x + 1))];
				} break;
			}
		case kBitdepth24Bit: {
				y_offset *= 3;
				for (unsigned x = 0; x < width_; x++) {
					int src_offset = y_offset + (width_ - (x + 1)) * 3;
					int dest_offset = y_offset + x * 3;
					_buffer[dest_offset + 0] = ((uint8*)buffer_)[src_offset + 0];
					_buffer[dest_offset + 1] = ((uint8*)buffer_)[src_offset + 1];
					_buffer[dest_offset + 2] = ((uint8*)buffer_)[src_offset + 2];
				} break;
			}
		case kBitdepth32Bit: {
				for (unsigned x = 0; x < width_; x++) {
					((uint32*)_buffer)[y_offset + x] = ((uint32*)buffer_)[y_offset + (width_ - (x + 1))];
				} break;
			}
		case kBitdepth16BitPerChannel: {
				y_offset *= 3;

				for (unsigned x = 0; x < width_; x++) {
					int src_offset = y_offset + (width_ - (x + 1)) * 3;
					int dest_offset = y_offset + x * 3;
					((uint16*)_buffer)[dest_offset + 0] = ((uint16*)buffer_)[src_offset + 0];
					((uint16*)_buffer)[dest_offset + 1] = ((uint16*)buffer_)[src_offset + 1];
					((uint16*)_buffer)[dest_offset + 2] = ((uint16*)buffer_)[src_offset + 2];
				} break;
			}
		case kBitdepth32BitPerChannel: {
				y_offset *= 3;

				for (unsigned x = 0; x < width_; x++) {
					int src_offset = y_offset + (width_ - (x + 1)) * 3;
					int dest_offset = y_offset + x * 3;
					((float32*)_buffer)[dest_offset + 0] = ((float32*)buffer_)[src_offset + 0];
					((float32*)_buffer)[dest_offset + 1] = ((float32*)buffer_)[src_offset + 1];
					((float32*)_buffer)[dest_offset + 2] = ((float32*)buffer_)[src_offset + 2];
				} break;
			}
		}
	}

	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
	}

	pixel_size_ = _pixel_size;
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::FlipVertical() {
	unsigned _pixel_size;
	char* _buffer = (char*)CreateBuffer(pitch_, height_, bit_depth_, _pixel_size);

	if (_buffer == 0) {
		return;
	}

	for (unsigned y = 0; y < height_; y++) {
		unsigned src_y_offset = (height_ - (y + 1)) * pitch_;
		unsigned dest_y_offset = y * pitch_;

		switch(bit_depth_) {
		case kBitdepth8Bit: {
				for (unsigned x = 0; x < width_; x++) {
					_buffer[dest_y_offset + x] = ((uint8*)buffer_)[src_y_offset + x];
				} break;
			}
		case kBitdepth15Bit:
		case kBitdepth16Bit: {
				for (unsigned x = 0; x < width_; x++) {
					((short*)_buffer)[dest_y_offset + x] = ((short*)buffer_)[src_y_offset + x];
				} break;
			}
		case kBitdepth24Bit: {
				for (unsigned x = 0; x < width_; x++) {
					int src_offset = (src_y_offset + x) * 3;
					int dest_offset = (dest_y_offset + x) * 3;
					_buffer[dest_offset + 0] = ((uint8*)buffer_)[src_offset + 0];
					_buffer[dest_offset + 1] = ((uint8*)buffer_)[src_offset + 1];
					_buffer[dest_offset + 2] = ((uint8*)buffer_)[src_offset + 2];
				} break;
			}
		case kBitdepth32Bit: {
				for (unsigned x = 0; x < width_; x++) {
					((uint32*)_buffer)[dest_y_offset + x] = ((uint32*)buffer_)[src_y_offset + x];
				} break;
			}
		case kBitdepth16BitPerChannel: {
				for (unsigned x = 0; x < width_; x++) {
					int src_offset = (src_y_offset + x) * 3;
					int dest_offset = (dest_y_offset + x) * 3;
					((uint16*)_buffer)[dest_offset + 0] = ((uint16*)buffer_)[src_offset + 0];
					((uint16*)_buffer)[dest_offset + 1] = ((uint16*)buffer_)[src_offset + 1];
					((uint16*)_buffer)[dest_offset + 2] = ((uint16*)buffer_)[src_offset + 2];
				} break;
			}
		case kBitdepth32BitPerChannel: {
				for (unsigned x = 0; x < width_; x++) {
					int src_offset = (src_y_offset + x) * 3;
					int dest_offset = (dest_y_offset + x) * 3;
					((float32*)_buffer)[dest_offset + 0] = ((float32*)buffer_)[src_offset + 0];
					((float32*)_buffer)[dest_offset + 1] = ((float32*)buffer_)[src_offset + 1];
					((float32*)_buffer)[dest_offset + 2] = ((float32*)buffer_)[src_offset + 2];
				} break;
			}
		}
	}

	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
	}

	pixel_size_ = _pixel_size;
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Rotate90DegClockWise(int steps) {
	steps &= 3;

	if (steps == 0) {
		return;
	}

	unsigned _pixel_size;
	char* _buffer;

	unsigned dest_width = width_;
	unsigned dest_height = height_;
	unsigned dest_pitch = pitch_;

	if ((steps & 1) != 0) {
		dest_width = height_;
		dest_height = pitch_;
		dest_pitch = height_;
	}

	_buffer = (char*)CreateBuffer(dest_pitch, dest_height, bit_depth_, _pixel_size);

	if (_buffer == 0) {
		return;
	}

	switch(bit_depth_) {
	case kBitdepth8Bit:
		switch(steps) {
		case 1: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						_buffer[dest_y_offset + x] = ((uint8*)buffer_)[(height_ - (x + 1)) * pitch_ + y];
					}
				} break;
			}
		case 2: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						_buffer[dest_y_offset + x] = ((uint8*)buffer_)[(height_ - (y + 1)) * pitch_ + (width_ - (x + 1))];
					}
				} break;
			}
		case 3: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						_buffer[dest_y_offset + x] = ((uint8*)buffer_)[x * pitch_ + (width_ - (y + 1))];
					}
				} break;
			}
		};
		break;
	case kBitdepth15Bit:
	case kBitdepth16Bit:
		switch(steps) {
		case 1: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						((short*)_buffer)[dest_y_offset + x] = ((short*)buffer_)[(height_ - (x + 1)) * pitch_ + y];
					}
				} break;
			}
		case 2: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						((short*)_buffer)[dest_y_offset + x] = ((short*)buffer_)[(height_ - (y + 1)) * pitch_ + (width_ - (x + 1))];
					}
				} break;
			}
		case 3: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						((short*)_buffer)[dest_y_offset + x] = ((short*)buffer_)[x * pitch_ + (width_ - (y + 1))];
					}
				} break;
			}
		};
		break;
	case kBitdepth24Bit:
		switch(steps) {
		case 1: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = ((height_ - (x + 1)) * pitch_ + y) * 3;
						_buffer[dest_offset + 0] = ((uint8*)buffer_)[src_offset + 0];
						_buffer[dest_offset + 1] = ((uint8*)buffer_)[src_offset + 1];
						_buffer[dest_offset + 2] = ((uint8*)buffer_)[src_offset + 2];
					}
				} break;
			}
		case 2: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = ((height_ - (y + 1)) * pitch_ + (width_ - (x + 1))) * 3;
						_buffer[dest_offset + 0] = ((uint8*)buffer_)[src_offset + 0];
						_buffer[dest_offset + 1] = ((uint8*)buffer_)[src_offset + 1];
						_buffer[dest_offset + 2] = ((uint8*)buffer_)[src_offset + 2];
					}
				} break;
			}
		case 3: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = (x * pitch_ + (width_ - (y + 1))) * 3;
						_buffer[dest_offset + 0] = ((uint8*)buffer_)[src_offset + 0];
						_buffer[dest_offset + 1] = ((uint8*)buffer_)[src_offset + 1];
						_buffer[dest_offset + 2] = ((uint8*)buffer_)[src_offset + 2];
					}
				} break;
			}
		};
		break;
	case kBitdepth32Bit:
		switch(steps) {
		case 1: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						((uint32*)_buffer)[dest_y_offset + x] = ((uint32*)buffer_)[(height_ - (x + 1)) * pitch_ + y];
					}
				} break;
			}
		case 2: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						((uint32*)_buffer)[dest_y_offset + x] = ((uint32*)buffer_)[(height_ - (y + 1)) * pitch_ + (width_ - (x + 1))];
					}
				} break;
			}
		case 3: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						((uint32*)_buffer)[dest_y_offset + x] = ((uint32*)buffer_)[x * pitch_ + (width_ - (y + 1))];
					}
				} break;
			}
		};
		break;
	case kBitdepth16BitPerChannel:
		switch(steps) {
		case 1: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = ((height_ - (x + 1)) * pitch_ + y) * 3;
						((uint16*)_buffer)[dest_offset + 0] = ((uint16*)buffer_)[src_offset + 0];
						((uint16*)_buffer)[dest_offset + 1] = ((uint16*)buffer_)[src_offset + 1];
						((uint16*)_buffer)[dest_offset + 2] = ((uint16*)buffer_)[src_offset + 2];
					}
				} break;
			}
		case 2: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = ((height_ - (y + 1)) * pitch_ + (width_ - (x + 1))) * 3;
						((uint16*)_buffer)[dest_offset + 0] = ((uint16*)buffer_)[src_offset + 0];
						((uint16*)_buffer)[dest_offset + 1] = ((uint16*)buffer_)[src_offset + 1];
						((uint16*)_buffer)[dest_offset + 2] = ((uint16*)buffer_)[src_offset + 2];
					}
				} break;
			}
		case 3: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = (x * pitch_ + (width_ - (y + 1))) * 3;
						((uint16*)_buffer)[dest_offset + 0] = ((uint16*)buffer_)[src_offset + 0];
						((uint16*)_buffer)[dest_offset + 1] = ((uint16*)buffer_)[src_offset + 1];
						((uint16*)_buffer)[dest_offset + 2] = ((uint16*)buffer_)[src_offset + 2];
					}
				} break;
			}
		};
		break;
	case kBitdepth32BitPerChannel:
		switch(steps) {
		case 1: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = ((height_ - (x + 1)) * pitch_ + y) * 3;
						((float32*)_buffer)[dest_offset + 0] = ((float32*)buffer_)[src_offset + 0];
						((float32*)_buffer)[dest_offset + 1] = ((float32*)buffer_)[src_offset + 1];
						((float32*)_buffer)[dest_offset + 2] = ((float32*)buffer_)[src_offset + 2];
					}
				} break;
			}
		case 2: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = ((height_ - (y + 1)) * pitch_ + (width_ - (x + 1))) * 3;
						((float32*)_buffer)[dest_offset + 0] = ((float32*)buffer_)[src_offset + 0];
						((float32*)_buffer)[dest_offset + 1] = ((float32*)buffer_)[src_offset + 1];
						((float32*)_buffer)[dest_offset + 2] = ((float32*)buffer_)[src_offset + 2];
					}
				} break;
			}
		case 3: {
				for (unsigned y = 0; y < dest_height; y++) {
					unsigned dest_y_offset = y * dest_pitch;

					for (unsigned x = 0; x < dest_width; x++) {
						int dest_offset = (dest_y_offset + x) * 3;
						int src_offset = (x * pitch_ + (width_ - (y + 1))) * 3;
						((float32*)_buffer)[dest_offset + 0] = ((float32*)buffer_)[src_offset + 0];
						((float32*)_buffer)[dest_offset + 1] = ((float32*)buffer_)[src_offset + 1];
						((float32*)_buffer)[dest_offset + 2] = ((float32*)buffer_)[src_offset + 2];
					}
				} break;
			}
		};
		break;
	}

	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
	}

	pixel_size_ = _pixel_size;
	buffer_ = _buffer;
	buffer_responsibility_ = true;

	width_ = dest_width;
	height_ = dest_height;
	pitch_ = dest_pitch;
}

void Canvas::InitSamplingGrid() {
	if (resize_data_initialized_ == false) {
		resize_data_initialized_ = true;

		//
		// Setup the sample grid and its weights.
		//
		int i;
		for (i = 0; i < sampling_grid_size_; i++) {
			sampling_grid_coord_look_up_[i] = ((float32)(i - sampling_grid_size_ / 2) + 0.5f) * sampling_grid_point_space_;
		}

		const float32 e = 2.718281828f;
		const float32 div = resize_sharpness_ * (float32)sqrt(2.0f * PIF);
		const float32 s2 = resize_sharpness_ * resize_sharpness_;

		for (int y = 0; y < sampling_grid_size_; y++) {
			float32 grid_point_y = (float32)sampling_grid_coord_look_up_[y];

			for (int x = 0; x < sampling_grid_size_; x++) {
				float32 grid_point_x = sampling_grid_coord_look_up_[x];

				float32 dist = (float32)sqrt(grid_point_x * grid_point_x + grid_point_y * grid_point_y);

				float32 weight = (float32)pow(e, -(dist * dist) / s2) / div;
				sampling_grid_weight_[y * sampling_grid_size_ + x] = weight;
			}
		}
	}
}

void Canvas::Resize(unsigned new_width, unsigned new_height, ResizeHint resize_hint) {
	if (width_ == new_width && height_ == new_height)
		return;

	if (resize_hint == kResizeCanvas) {
		Crop(0, 0, new_width, new_height);
		return;
	}

	switch(bit_depth_) {
	case kBitdepth8Bit:
		switch(resize_hint) {
		case kResizeNicest:
			Resize8BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize8BitFast(new_width, new_height);
		break;
		} break;
	case kBitdepth15Bit:
		switch(resize_hint) {
		case kResizeNicest:
			Resize15BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize16BitFast(new_width, new_height);
		break;
		} break;
	case kBitdepth16Bit:
		switch(resize_hint) {
		case kResizeNicest:
			Resize16BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize16BitFast(new_width, new_height);
		break;
		} break;
	case kBitdepth24Bit:
		switch(resize_hint) {
		case kResizeNicest:
			Resize24BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize24BitFast(new_width, new_height);
		break;
		} break;
	case kBitdepth32Bit:
		switch(resize_hint) {
		case kResizeNicest:
			Resize32BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize32BitFast(new_width, new_height);
		break;
		} break;
	case kBitdepth16BitPerChannel:
		switch(resize_hint) {
		case kResizeNicest:
			Resize48BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize48BitFast(new_width, new_height);
		break;
		} break;
	case kBitdepth32BitPerChannel:
		switch(resize_hint) {
		case kResizeNicest:
			Resize96BitSmooth(new_width, new_height);
		break;
		case kResizeFast:
		default:
			Resize96BitFast(new_width, new_height);
		break;
		} break;
	}
}

void Canvas::Resize8BitFast(unsigned new_width, unsigned new_height) {
	uint8* _buffer = new uint8[new_width * new_height];

	int src_x = 0;
	int src_y = 0;
	int src_x_step = (width_ << 16) / new_width;
	int src_y_step = (height_ << 16) / new_height;

	for (unsigned y = 0; y < new_height; y++) {
		int src_y_offset = (src_y >> 16) * pitch_;
		int dest_y_offset = y * new_width;

		src_x = 0;

		for (unsigned x = 0; x < new_width; x++) {
			_buffer[dest_y_offset + x] = ((uint8*)buffer_)[src_y_offset + (src_x >> 16)];
			src_x += src_x_step;
		}

		src_y += src_y_step;
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize16BitFast(unsigned new_width, unsigned new_height) {
	uint16* _buffer = new uint16[new_width * new_height];

	int src_x = 0;
	int src_y = 0;
	int src_x_step = (width_ << 16) / new_width;
	int src_y_step = (height_ << 16) / new_height;

	for (unsigned y = 0; y < new_height; y++) {
		int src_y_offset = (src_y >> 16) * pitch_;
		int dest_y_offset = y * new_width;

		src_x = 0;

		for (unsigned x = 0; x < new_width; x++) {
			_buffer[dest_y_offset + x] = ((uint16*)buffer_)[src_y_offset + (src_x >> 16)];
			src_x += src_x_step;
		}

		src_y += src_y_step;
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize24BitFast(unsigned new_width, unsigned new_height) {
	uint8* _buffer = new uint8[new_width * new_height * 3];

	int src_x = 0;
	int src_y = 0;
	int src_x_step = (width_ << 16) / new_width;
	int src_y_step = (height_ << 16) / new_height;

	for (unsigned y = 0; y < new_height; y++) {
		int src_y_offset = (src_y >> 16) * pitch_;
		int dest_y_offset = y * new_width;

		src_x = 0;

		for (unsigned x = 0; x < new_width; x++) {
			int dest_offset = (dest_y_offset + x) * 3;
			int src_offset = (src_y_offset + (src_x >> 16)) * 3;

			_buffer[dest_offset + 0] = ((uint8*)buffer_)[src_offset + 0];
			_buffer[dest_offset + 1] = ((uint8*)buffer_)[src_offset + 1];
			_buffer[dest_offset + 2] = ((uint8*)buffer_)[src_offset + 2];

			src_x += src_x_step;
		}

		src_y += src_y_step;
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize32BitFast(unsigned new_width, unsigned new_height) {
	uint32* _buffer = new uint32[new_width * new_height];

	int src_x = 0;
	int src_y = 0;
	int src_x_step = (width_ << 16) / new_width;
	int src_y_step = (height_ << 16) / new_height;

	for (unsigned y = 0; y < new_height; y++) {
		int src_y_offset = (src_y >> 16) * pitch_;
		int dest_y_offset = y * new_width;

		src_x = 0;

		for (unsigned x = 0; x < new_width; x++) {
			_buffer[dest_y_offset + x] = ((uint32*)buffer_)[src_y_offset + (src_x >> 16)];
			src_x += src_x_step;
		}

		src_y += src_y_step;
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize48BitFast(unsigned new_width, unsigned new_height) {
	uint16* _buffer = new uint16[new_width * new_height * 3];

	int src_x = 0;
	int src_y = 0;
	int src_x_step = (width_ << 16) / new_width;
	int src_y_step = (height_ << 16) / new_height;

	for (unsigned y = 0; y < new_height; y++) {
		int src_y_offset = (src_y >> 16) * pitch_;
		int dest_y_offset = y * new_width;

		src_x = 0;

		for (unsigned x = 0; x < new_width; x++) {
			int dest_offset = (dest_y_offset + x) * 3;
			int src_offset = (src_y_offset + (src_x >> 16)) * 3;

			_buffer[dest_offset + 0] = ((uint16*)buffer_)[src_offset + 0];
			_buffer[dest_offset + 1] = ((uint16*)buffer_)[src_offset + 1];
			_buffer[dest_offset + 2] = ((uint16*)buffer_)[src_offset + 2];

			src_x += src_x_step;
		}

		src_y += src_y_step;
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize96BitFast(unsigned new_width, unsigned new_height) {
	float32* _buffer = new float32[new_width * new_height * 3];

	int src_x = 0;
	int src_y = 0;
	int src_x_step = (width_ << 16) / new_width;
	int src_y_step = (height_ << 16) / new_height;

	for (unsigned y = 0; y < new_height; y++) {
		int src_y_offset = (src_y >> 16) * pitch_;
		int dest_y_offset = y * new_width;

		src_x = 0;

		for (unsigned x = 0; x < new_width; x++) {
			int dest_offset = (dest_y_offset + x) * 3;
			int src_offset = (src_y_offset + (src_x >> 16)) * 3;

			_buffer[dest_offset + 0] = ((float32*)buffer_)[src_offset + 0];
			_buffer[dest_offset + 1] = ((float32*)buffer_)[src_offset + 1];
			_buffer[dest_offset + 2] = ((float32*)buffer_)[src_offset + 2];

			src_x += src_x_step;
		}

		src_y += src_y_step;
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize8BitSmooth(unsigned new_width, unsigned new_height) {
	// OBS! This function handles the bitmap as a grayscale bitmap!

	// Create the new buffer...
	uint8* _buffer = new uint8[new_width * new_height];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_);
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_;
		}

		for (int x = 0; x < (int)new_width; x++) {
			float32 _color = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							uint8 _color = ((uint8*)buffer_)[v_offset + u];

							_color += GammaLookup::GammaToLinearFloat(_color) * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			_buffer[dest_offset++] = GammaLookup::LinearToGamma((uint16)floor(_color * one_over_weight_sum));
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize15BitSmooth(unsigned new_width, unsigned new_height) {
	// Create the new buffer...
	uint16* _buffer = new uint16[new_width * new_height];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_);
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_;
		}

		for (int x = 0; x < (int)new_width; x++) {
			float32 __r = 0.0f;
			float32 __g = 0.0f;
			float32 __b = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							unsigned _color = ((uint16*)buffer_)[v_offset + u];

							__r += GammaLookup::GammaToLinearFloat((uint8)((_color >> 10) & 31) << 3) * (*sgw);
							__g += GammaLookup::GammaToLinearFloat((uint8)((_color >> 5) & 31) << 3)  * (*sgw);
							__b += GammaLookup::GammaToLinearFloat((uint8)(_color & 31) << 3) * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			uint16 r16 = GammaLookup::LinearToGamma((uint16)floor(__r * one_over_weight_sum));
			uint16 g16 = GammaLookup::LinearToGamma((uint16)floor(__g * one_over_weight_sum));
			uint16 b16 = GammaLookup::LinearToGamma((uint16)floor(__b * one_over_weight_sum));

			_buffer[dest_offset++] = ((r16 >> 3) << 10) | ((g16 >> 3) << 5) | (b16 >> 3);
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}


void Canvas::Resize16BitSmooth(unsigned new_width, unsigned new_height) {
	// Create the new buffer...
	uint16* _buffer = new uint16[new_width * new_height];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_);
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_;
		}

		for (int x = 0; x < (int)new_width; x++) {
			float32 __r = 0.0f;
			float32 __g = 0.0f;
			float32 __b = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							unsigned _color = ((uint16*)buffer_)[v_offset + u];

							__r += GammaLookup::GammaToLinearFloat((uint8)((_color >> 11) & 31) << 3) * (*sgw);
							__g += GammaLookup::GammaToLinearFloat((uint8)((_color >> 5) & 63) << 2)  * (*sgw);
							__b += GammaLookup::GammaToLinearFloat((uint8)(_color & 31) << 3) * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			uint16 r16 = GammaLookup::LinearToGamma((uint16)floor(__r * one_over_weight_sum));
			uint16 g16 = GammaLookup::LinearToGamma((uint16)floor(__g * one_over_weight_sum));
			uint16 b16 = GammaLookup::LinearToGamma((uint16)floor(__b * one_over_weight_sum));

			_buffer[dest_offset++] = ((r16 >> 3) << 11) | ((g16 >> 2) << 5) | (b16 >> 3);
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize24BitSmooth(unsigned new_width, unsigned new_height) {
	// Create the new buffer...
	uint8* _buffer = new uint8[new_width * new_height * 3];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_) * 3;
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_ * 3;
		}

		for (int x = 0; x < (int)new_width; x++) {
			float32 __r = 0.0f;
			float32 __g = 0.0f;
			float32 __b = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							uint8* _color = ((uint8*)buffer_) + v_offset + u * 3;

							__b += GammaLookup::GammaToLinearFloat(_color[0]) * (*sgw);
							__g += GammaLookup::GammaToLinearFloat(_color[1]) * (*sgw);
							__r += GammaLookup::GammaToLinearFloat(_color[2]) * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			uint8 r8 = GammaLookup::LinearToGamma((uint16)floor(__r * one_over_weight_sum));
			uint8 g8 = GammaLookup::LinearToGamma((uint16)floor(__g * one_over_weight_sum));
			uint8 b8 = GammaLookup::LinearToGamma((uint16)floor(__b * one_over_weight_sum));

			_buffer[dest_offset + 0] = b8;
			_buffer[dest_offset + 1] = g8;
			_buffer[dest_offset + 2] = r8;

			dest_offset += 3;
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize32BitSmooth(unsigned new_width, unsigned new_height) {
	// Create the new buffer...
	uint32* _buffer = new uint32[new_width * new_height];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_);
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_;
		}

		for (int x = 0; x < (int)new_width; x++) {

			float32 __r = 0.0f;
			float32 __g = 0.0f;
			float32 __b = 0.0f;
			float32 __a = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							uint32 _color = ((uint32*)buffer_)[v_offset + u];

							__a += GammaLookup::GammaToLinearFloat((uint8)((_color >> 24) & 0xFF)) * (*sgw);
							__r += GammaLookup::GammaToLinearFloat((uint8)((_color >> 16) & 0xFF)) * (*sgw);
							__g += GammaLookup::GammaToLinearFloat((uint8)((_color >> 8) & 0xFF))  * (*sgw);
							__b += GammaLookup::GammaToLinearFloat((uint8)(_color & 0xFF))         * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			uint32 a32 = GammaLookup::LinearToGamma((uint16)floor(__a * one_over_weight_sum));
			uint32 r32 = GammaLookup::LinearToGamma((uint16)floor(__r * one_over_weight_sum));
			uint32 g32 = GammaLookup::LinearToGamma((uint16)floor(__g * one_over_weight_sum));
			uint32 b32 = GammaLookup::LinearToGamma((uint16)floor(__b * one_over_weight_sum));

			_buffer[dest_offset++] = (a32 << 24) | (r32 << 16) | (g32 << 8) | b32;
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize48BitSmooth(unsigned new_width, unsigned new_height) {
	// Create the new buffer...
	uint16* _buffer = new uint16[new_width * new_height * 3];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_) * 3;
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_ * 3;
		}

		for (int x = 0; x < (int)new_width; x++) {
			float32 __r = 0.0f;
			float32 __g = 0.0f;
			float32 __b = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							uint16* _color = &((uint16*)buffer_)[v_offset + u * 3];

							__b += _color[0] * (*sgw);
							__g += _color[1] * (*sgw);
							__r += _color[2] * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			uint16 r16 = (uint16)floor(__r * one_over_weight_sum);
			uint16 g16 = (uint16)floor(__g * one_over_weight_sum);
			uint16 b16 = (uint16)floor(__b * one_over_weight_sum);

			_buffer[dest_offset + 0] = b16;
			_buffer[dest_offset + 1] = g16;
			_buffer[dest_offset + 2] = r16;

			dest_offset += 3;
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::Resize96BitSmooth(unsigned new_width, unsigned new_height) {
	// Create the new buffer...
	float32* _buffer = new float32[new_width * new_height * 3];

	float32 scale_x = (float32)width_ / (float32)new_width;
	float32 scale_y = (float32)height_ / (float32)new_height;

	// And here is the main loop.

	int dest_offset = 0;
	for (int y = 0; y < (int)new_height; y++) {
		// Precalc all possible V-coordinates.
		float32 sample_y = (float32)y + 0.5f;
		int v_lookup[sampling_grid_size_];
		const int buffer_size = (int)(height_ * pitch_) * 3;
		for (int i = 0; i < sampling_grid_size_; i++) {
			v_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_y) * scale_y) * pitch_ * 3;
		}

		for (int x = 0; x < (int)new_width; x++) {
			float32 __r = 0.0f;
			float32 __g = 0.0f;
			float32 __b = 0.0f;

			// Precalc all possible U-coordinates.
			float32 sample_x = (float32)x + 0.5f;
			int u_lookup[sampling_grid_size_];
			for (int i = 0; i < sampling_grid_size_; i++) {
				u_lookup[i] = (int)((sampling_grid_coord_look_up_[i] + sample_x) * scale_x);
			}

			// Time to take some samples.
			float32 weight_sum = 0.0f;
			float32* sgw = sampling_grid_weight_;

			for (int sgy = 0; sgy < sampling_grid_size_; sgy++) {
				int v_offset = v_lookup[sgy];

				if (v_offset >= 0 && v_offset < buffer_size) {
					for (int sgx = 0; sgx < sampling_grid_size_; sgx++) {
						int u = u_lookup[sgx];

						if (u >= 0 && u < (int)width_) {
							float32* _color = &((float32*)buffer_)[v_offset + u * 3];

							__b += _color[0] * (*sgw);
							__g += _color[1] * (*sgw);
							__r += _color[2] * (*sgw);

							weight_sum += (*sgw);
						}

						sgw++;
					}
				}
			}

			// Convert back to normal colors
			float32 one_over_weight_sum = 1.0f / weight_sum;
			_buffer[dest_offset + 0] = __b * one_over_weight_sum;
			_buffer[dest_offset + 1] = __g * one_over_weight_sum;
			_buffer[dest_offset + 2] = __r * one_over_weight_sum;

			dest_offset += 3;
		}
	}

	Reset(new_width, new_height, bit_depth_);
	buffer_ = _buffer;
	buffer_responsibility_ = true;
}

void Canvas::SwapRGBOrder() {
	switch(bit_depth_) {
		case kBitdepth8Bit:
			SwapRGB8Bit();
			break;
		case kBitdepth15Bit:
			SwapRGB15Bit();
			break;
		case kBitdepth16Bit:
			SwapRGB16Bit();
			break;
		case kBitdepth24Bit:
			SwapRGB24Bit();
			break;
		case kBitdepth32Bit:
			SwapRGB32Bit();
			break;
		case kBitdepth16BitPerChannel:
			SwapRGB48Bit();
			break;
		case kBitdepth32BitPerChannel:
			SwapRGB96Bit();
			break;
	};
}

void Canvas::SwapRGB8Bit() {
	// Swap rgb in the palette.
	for (int i = 0; i < 256; i++) {
		uint8 red = palette_[i].red_;
		uint8 blue = palette_[i].blue_;

		palette_[i].red_ = blue;
		palette_[i].blue_ = red;
	}
}

void Canvas::SwapRGB15Bit() {
	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint16 col = ((uint16*)buffer_)[offset + x];

			uint16 r = (col >> 10);
			uint16 g = (col >> 5) & 31;
			uint16 b =  col & 31;

			((uint16*)buffer_)[offset + x] = (b << 10) | (g << 5) | r;
		}
	}
}

void Canvas::SwapRGB16Bit() {
	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint16 col = ((uint16*)buffer_)[offset + x];

			uint16 r = (col >> 11);
			uint16 g = (col >> 5) & 63;
			uint16 b =  col & 31;

			((uint16*)buffer_)[offset + x] = (b << 11) | (g << 5) | r;
		}
	}
}

void Canvas::SwapRGB24Bit() {
	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_ * 3;
		uint8* _buffer = &((uint8*)buffer_)[offset];
		const uint8* end_buffer = _buffer + width_*3;
		for (; _buffer < end_buffer; _buffer += 3) {
			const uint8 r = *_buffer;
			*_buffer = _buffer[2];
			_buffer[2] = r;
		}
	}
}

void Canvas::SwapRGB32Bit() {
	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_ * 4;

		for (unsigned x = 0; x < width_; x++) {
			uint8 r = ((uint8*)buffer_)[offset + 0];
			uint8 b = ((uint8*)buffer_)[offset + 2];

			((uint8*)buffer_)[offset + 0] = b;
			((uint8*)buffer_)[offset + 2] = r;

			offset += 4;
		}
	}
}

void Canvas::SwapRGB48Bit() {
	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_ * 3;

		for (unsigned x = 0; x < width_; x++) {
			uint16 r = ((uint16*)buffer_)[offset + 0];
			uint16 b = ((uint16*)buffer_)[offset + 2];

			((uint16*)buffer_)[offset + 0] = b;
			((uint16*)buffer_)[offset + 2] = r;

			offset += 3;
		}
	}
}

void Canvas::SwapRGB96Bit() {
	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_ * 3;

		for (unsigned x = 0; x < width_; x++) {
			float32 r = ((float32*)buffer_)[offset + 0];
			float32 b = ((float32*)buffer_)[offset + 2];

			((float32*)buffer_)[offset + 0] = b;
			((float32*)buffer_)[offset + 2] = r;

			offset += 3;
		}
	}
}

void Canvas::RotateChannels(int steps) {
	if (steps == 0) {
		return;
	}

	switch(bit_depth_) {
		case kBitdepth8Bit:
			RotateChannels8Bit(steps);
			break;
		case kBitdepth15Bit:
			RotateChannels15Bit(steps);
			break;
		case kBitdepth16Bit:
			RotateChannels16Bit(steps);
			break;
		case kBitdepth24Bit:
			RotateChannels24Bit(steps);
			break;
		case kBitdepth32Bit:
			RotateChannels32Bit(steps);
			break;
		case kBitdepth16BitPerChannel:
			RotateChannels48Bit(steps);
			break;
		case kBitdepth32BitPerChannel:
			RotateChannels96Bit(steps);
			break;
	};
}

void Canvas::RotateChannels8Bit(int steps) {
	if (steps > 0) {
		steps &= 3;
	} else {
		steps = 4 - ((-steps) & 3);
	}

	int step0 = (steps + 0) & 3;
	int step1 = (steps + 1) & 3;
	int step2 = (steps + 2) & 3;
	int step3 = (steps + 3) & 3;

	// Swap rgb in the palette.
	for (int i = 0; i < 256; i++) {
		uint8  src[4];
		uint8* dst[4];
		src[0] = palette_[i].red_;
		src[1] = palette_[i].green_;
		src[2] = palette_[i].blue_;
		src[3] = palette_[i].alpha_;

		dst[0] = &palette_[i].red_;
		dst[1] = &palette_[i].green_;
		dst[2] = &palette_[i].blue_;
		dst[3] = &palette_[i].alpha_;

		(*dst[step0]) = src[0];
		(*dst[step1]) = src[1];
		(*dst[step2]) = src[2];
		(*dst[step3]) = src[3];
	}
}

void Canvas::RotateChannels15Bit(int steps) {
	if (steps > 0) {
		steps %= 3;
	} else {
		steps = 3 - ((-steps) % 3);
	}

	int step0 = (steps + 0) % 3;
	int step1 = (steps + 1) % 3;
	int step2 = (steps + 2) % 3;

	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint16 col = ((uint16*)buffer_)[offset + x];

			uint16 src[3];
			src[0] = (col >> 10);
			src[1] = (col >> 5) & 31;
			src[2] =  col & 31;

			uint16 dst[3];
			dst[step0] = src[0];
			dst[step1] = src[1];
			dst[step2] = src[2];

			((uint16*)buffer_)[offset + x] = (dst[2] << 10) | (dst[1] << 5) | dst[0];
		}
	}
}

void Canvas::RotateChannels16Bit(int steps) {
	if (steps > 0) {
		steps %= 3;
	} else {
		steps = 3 - ((-steps) % 3);
	}

	int step0 = (steps + 0) % 3;
	int step1 = (steps + 1) % 3;
	int step2 = (steps + 2) % 3;

	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint16 col = ((uint16*)buffer_)[offset + x];

			uint16 src[3];
			src[0] = (col >> 11) << 1;
			src[1] = (col >> 5) & 63;
			src[2] = (col & 31) << 1;

			uint16 dst[3];
			dst[step0] = src[0];
			dst[step1] = src[1];
			dst[step2] = src[2];

			((uint16*)buffer_)[offset + x] = ((dst[2] >> 1) << 11) | (dst[1] << 5) | (dst[0] >> 1);
		}
	}
}

void Canvas::RotateChannels24Bit(int steps) {
	if (steps > 0) {
		steps %= 3;
	} else {
		steps = 3 - ((-steps) % 3);
	}

	int step0 = (steps + 0) % 3;
	int step1 = (steps + 1) % 3;
	int step2 = (steps + 2) % 3;

	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint8* _buffer = &((uint8*)buffer_)[(offset + x) * 3];

			uint8 src[3];
			src[0] = _buffer[0];
			src[1] = _buffer[1];
			src[2] = _buffer[2];

			_buffer[step0] = src[0];
			_buffer[step1] = src[1];
			_buffer[step2] = src[2];
		}
	}
}

void Canvas::RotateChannels32Bit(int steps) {
	if (steps > 0) {
		steps &= 3;
	} else {
		steps = 3 - ((-steps) & 3);
	}

	int step0 = (steps + 0) & 3;
	int step1 = (steps + 1) & 3;
	int step2 = (steps + 2) & 3;
	int step3 = (steps + 3) & 3;

	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint8* _buffer = &((uint8*)buffer_)[(offset + x) << 2];

			uint8 src[4];
			src[0] = _buffer[0];
			src[1] = _buffer[1];
			src[2] = _buffer[2];
			src[3] = _buffer[3];

			_buffer[step0] = src[0];
			_buffer[step1] = src[1];
			_buffer[step2] = src[2];
			_buffer[step3] = src[3];
		}
	}
}

void Canvas::RotateChannels48Bit(int steps) {
	if (steps > 0) {
		steps %= 3;
	} else {
		steps = 3 - ((-steps) % 3);
	}

	int step0 = (steps + 0) % 3;
	int step1 = (steps + 1) % 3;
	int step2 = (steps + 2) % 3;

	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			uint16* _buffer = &((uint16*)buffer_)[(offset + x) * 3];

			uint16 src[3];
			src[0] = _buffer[0];
			src[1] = _buffer[1];
			src[2] = _buffer[2];

			_buffer[step0] = src[0];
			_buffer[step1] = src[1];
			_buffer[step2] = src[2];
		}
	}
}

void Canvas::RotateChannels96Bit(int steps) {
	if (steps > 0) {
		steps %= 3;
	} else {
		steps = 3 - ((-steps) % 3);
	}

	int step0 = (steps + 0) % 3;
	int step1 = (steps + 1) % 3;
	int step2 = (steps + 2) % 3;

	for (unsigned y = 0; y < height_; y++) {
		int offset = y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			float32* _buffer = &((float32*)buffer_)[(offset + x) * 3];

			float32 src[3];
			src[0] = _buffer[0];
			src[1] = _buffer[1];
			src[2] = _buffer[2];

			_buffer[step0] = src[0];
			_buffer[step1] = src[1];
			_buffer[step2] = src[2];
		}
	}
}

void Canvas::ConvertBitDepth(BitDepth bit_depth) {
	if (bit_depth_ == bit_depth) {
		return;
	}

	switch(bit_depth) {
		case kBitdepth8Bit:
			ConvertTo8();
			pixel_size_ = 1;
			break;
		case kBitdepth15Bit:
			ConvertTo15();
			pixel_size_ = 2;
			break;
		case kBitdepth16Bit:
			ConvertTo16();
			pixel_size_ = 2;
			break;
		case kBitdepth24Bit:
			ConvertTo24();
			pixel_size_ = 3;
			break;
		case kBitdepth32Bit:
			ConvertTo32();
			pixel_size_ = 4;
			break;
		case kBitdepth16BitPerChannel:
			ConvertTo48();
			pixel_size_ = 6;
			break;
		case kBitdepth32BitPerChannel:
			ConvertTo96();
			pixel_size_ = 12;
			break;
	};
}

void Canvas::ConvertToGrayscale(bool convert_to8_bit) {
	if (convert_to8_bit == true) {
		ConvertTo8BitGrayscale();
	} else {
		ConvertToGray();
	}
}

void Canvas::ConvertTo32BitWithAlpha(const Canvas& alpha_buffer) {
	if (alpha_buffer.GetBitDepth() == kBitdepth8Bit &&
	   alpha_buffer.GetWidth() == width_ &&
	   alpha_buffer.GetHeight() == height_) {
		ConvertTo32();
		pixel_size_ = 4;

		uint8* alpha = (uint8*)alpha_buffer.GetBuffer();

		for (unsigned y = 0; y < height_; y++) {
			int alpha_offset = y * alpha_buffer.GetPitch();
			uint8* _buffer = (uint8*)buffer_ + y * pitch_ * 4;

			for (unsigned x = 0; x < width_; x++) {
				_buffer[3] = alpha[alpha_offset++];
				_buffer += 4;
			}
		}
	}
}

void Canvas::GetAlphaChannel(Canvas& alpha_buffer) const {
	if (bit_depth_ != kBitdepth32Bit) {
		return;
	}

	alpha_buffer.Reset(width_, height_, kBitdepth8Bit);
	alpha_buffer.CreateBuffer();

	uint8* alpha = (uint8*)alpha_buffer.GetBuffer();

	for (unsigned y = 0; y < height_; y++) {
		int alpha_offset = y * alpha_buffer.GetPitch();
		uint8* _buffer = (uint8*)buffer_ + y * pitch_ * 4;

		for (unsigned x = 0; x < width_; x++) {
			alpha[alpha_offset++] = _buffer[3];
			_buffer += 4;
		}
	}
}

void Canvas::GetAlphaChannel(Canvas& alpha_buffer, uint8* transparent_colors, unsigned num_colors) {
	if (bit_depth_ != kBitdepth8Bit) {
		return;
	}

	alpha_buffer.Reset(width_, height_, kBitdepth8Bit);
	alpha_buffer.CreateBuffer();

	uint8* alpha = (uint8*)alpha_buffer.GetBuffer();

	for (unsigned y = 0; y < height_; y++) {
		int alpha_offset = y * alpha_buffer.GetPitch();
		uint8* _buffer = (uint8*)buffer_ + y * pitch_;

		for (unsigned x = 0; x < width_; x++) {
			bool transparent = false;

			uint8 _color = _buffer[x];

			for (unsigned i = 0; i < num_colors; i++) {
				if (_color == transparent_colors[i]) {
					transparent = true;
					break;
				}
			}

			if (transparent == true) {
				alpha[alpha_offset++] = 0;
			} else {
				alpha[alpha_offset++] = 255;
			}
		}
	}
}

void Canvas::ConvertTo15() {
	uint8* temp;
	int image_size = pitch_ * height_,
		 i;
	uint16 data;
	uint8 r, g, b;

	if (bit_depth_ == kBitdepth15Bit) {
		return;
	}

	temp = new uint8[image_size * 2];

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			uint8 col = ((uint8*)buffer_)[i];

			r = (palette_[col].red_		>> 3);
			g = (palette_[col].green_	>> 3);
			b = (palette_[col].blue_		>> 3);

			data =  ((short)r) << 10;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth15Bit;
		break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			uint16 col = ((uint16*)buffer_)[i];

			r = (uint8)(col >> 11);
			g = (uint8)(col >> 5) & 63;
			b =  (uint8)(col & 31);

			data =  ((short)r) << 10;
			data += ((short)g) << 5;
			data += (short)(b);

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth15Bit;
		break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			b = (((uint8*)buffer_)[i * 3 + 0] >> 3);
			g = (((uint8*)buffer_)[i * 3 + 1] >> 3);
			r = (((uint8*)buffer_)[i * 3 + 2] >> 3);

			data =  ((short)r) << 10;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth15Bit;
	break;

	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			b = (((uint8*)buffer_)[(i<<2) + 0] >> 3);
			g = (((uint8*)buffer_)[(i<<2) + 1] >> 3);
			r = (((uint8*)buffer_)[(i<<2) + 2] >> 3);

			data =  ((short)r) << 10;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth15Bit;
		break;

	case kBitdepth16BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 0]) >> 3);
			g = (GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 1]) >> 3);
			r = (GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 2]) >> 3);

			data =  ((short)r) << 10;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth15Bit;
		break;

	case kBitdepth32BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 0]) * 31.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 1]) * 31.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 2]) * 31.0f);

			data =  ((short)r) << 10;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth15Bit;
		break;

	default:
		delete[] temp;
	}
}

void Canvas::ConvertTo16() {
	uint8 *temp;
	int image_size = pitch_ * height_;

	uint16 data;
	uint8 r, g, b;
	int i;

	if (bit_depth_ == kBitdepth16Bit) {
		return;
	}

	temp = new uint8 [image_size * 2];

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			uint8 col = ((uint8*)buffer_)[i];

			r = (palette_[col].red_		>> 3);
			g = (palette_[col].green_	>> 2);
			b = (palette_[col].blue_		>> 3);

			data = ((short)r) << 11;
			data += ((short)g) << 5;
			data += (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16Bit;
		break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			uint16 col = ((uint16*)buffer_)[i];

			r = (uint8)(col >> 10);
			g = (uint8)(col >> 5) & 31;
			b =  (uint8)col & 31;

			data = ((short)r) << 11;
			data += ((short)g) << 6;
			data += (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16Bit;
		break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			b = (((uint8*)buffer_)[i * 3 + 0] >> 3);
			g = (((uint8*)buffer_)[i * 3 + 1] >> 2);
			r = (((uint8*)buffer_)[i * 3 + 2] >> 3);

			data = ((short)r) << 11;
			data += ((short)g) << 5;
			data += (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16Bit;
		break;

	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			b = (((uint8*)buffer_)[(i<<2) + 0] >> 3);
			g = (((uint8*)buffer_)[(i<<2) + 1] >> 2);
			r = (((uint8*)buffer_)[(i<<2) + 2] >> 3);

			data = ((short)r) << 11;
			data += ((short)g) << 5;
			data += (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16Bit;
		break;

	case kBitdepth16BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 0]) >> 3);
			g = (GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 1]) >> 2);
			r = (GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 2]) >> 3);

			data =  ((short)r) << 11;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16Bit;
		break;

	case kBitdepth32BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 0]) * 31.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 1]) * 63.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 2]) * 31.0f);

			data =  ((short)r) << 11;
			data += ((short)g) << 5;
			data +=  (short)b;

			((uint16 *)temp)[i] = data;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16Bit;
		break;

	default:
		delete[] temp;
	}
}

void Canvas::ConvertTo24() {
	uint8 *temp;
	int image_size = pitch_ * height_;

	uint16 data;
	uint8 r, g, b;
	int i;

	temp = new uint8 [image_size * 3];

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			temp[i * 3 + 0] = palette_[((uint8*)buffer_)[i]].blue_;
			temp[i * 3 + 1] = palette_[((uint8*)buffer_)[i]].green_;
			temp[i * 3 + 2] = palette_[((uint8*)buffer_)[i]].red_;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth24Bit;
		break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (uint8)((data >> 10) & 31) << 3;
			g = (uint8)((data >> 5) & 31) << 3;
			b = (uint8)((data) & 31) << 3;

			temp[i * 3 + 0] = b;
			temp[i * 3 + 1] = g;
			temp[i * 3 + 2] = r;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth24Bit;
		break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (uint8)((data >> 11) & 31) << 3;
			g = (uint8)((data >> 5) & 63) << 2;
			b = (uint8)((data) & 31) << 3;

			temp[i * 3 + 0] = b;
			temp[i * 3 + 1] = g;
			temp[i * 3 + 2] = r;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth24Bit;
		break;

	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			temp[i * 3 + 0] = ((uint8*)buffer_)[(i<<2) + 0];
			temp[i * 3 + 1] = ((uint8*)buffer_)[(i<<2) + 1];
			temp[i * 3 + 2] = ((uint8*)buffer_)[(i<<2) + 2];
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth24Bit;
		break;

	case kBitdepth16BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 0]);
			g = GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 1]);
			r = GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 2]);

			temp[i * 3 + 0] = b;
			temp[i * 3 + 1] = g;
			temp[i * 3 + 2] = r;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth24Bit;
		break;

	case kBitdepth32BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 0]) * 255.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 1]) * 255.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 2]) * 255.0f);

			temp[i * 3 + 0] = b;
			temp[i * 3 + 1] = g;
			temp[i * 3 + 2] = r;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth24Bit;
		break;

	default:
		delete[] temp;
	}
}

void Canvas::ConvertTo32() {
	uint8 *temp;
	int image_size = pitch_ * height_;

	uint16 data;
	uint8 r, g, b;
	int i;

	temp = new uint8 [image_size * 4];

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size && buffer_; i++) {
			temp[(i<<2) + 0] = palette_[((uint8*)buffer_)[i]].blue_;
			temp[(i<<2) + 1] = palette_[((uint8*)buffer_)[i]].green_;
			temp[(i<<2) + 2] = palette_[((uint8*)buffer_)[i]].red_;
			temp[(i<<2) + 3] = 255; //Alpha
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32Bit;
		break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (uint8)((data >> 10) & 31) << 3;
			g = (uint8)((data >> 5) & 63) << 3;
			b = (uint8)((data) & 31) << 3;

			temp[(i<<2) + 0] = b;
			temp[(i<<2) + 1] = g;
			temp[(i<<2) + 2] = r;
			temp[(i<<2) + 3] = 255; //Alpha
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32Bit;
		break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (uint8)((data >> 11) & 31) << 3;
			g = (uint8)((data >> 5) & 63) << 2;
			b = (uint8)((data) & 31) << 3;

			temp[(i<<2) + 0] = b;
			temp[(i<<2) + 1] = g;
			temp[(i<<2) + 2] = r;
			temp[(i<<2) + 3] = 255; //Alpha
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32Bit;
		break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			temp[(i<<2) + 0] = ((uint8*)buffer_)[i * 3 + 0];
			temp[(i<<2) + 1] = ((uint8*)buffer_)[i * 3 + 1];
			temp[(i<<2) + 2] = ((uint8*)buffer_)[i * 3 + 2];
			temp[(i<<2) + 3] = 255; //Alpha
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32Bit;
		break;

	case kBitdepth16BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 0]);
			g = GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 1]);
			r = GammaLookup::LinearToGamma(((uint16*)buffer_)[i * 3 + 2]);

			temp[(i<<2) + 0] = b;
			temp[(i<<2) + 1] = g;
			temp[(i<<2) + 2] = r;
			temp[(i<<2) + 3] = 255; //Alpha
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32Bit;
		break;

	case kBitdepth32BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 0]) * 255.0f);
			g = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 1]) * 255.0f);
			r = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[i * 3 + 2]) * 255.0f);

			temp[(i<<2) + 0] = b;
			temp[(i<<2) + 1] = g;
			temp[(i<<2) + 2] = r;
			temp[(i<<2) + 3] = 255; //Alpha
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32Bit;
		break;

	default:
		delete[] temp;
	}
}

void Canvas::ConvertTo48() {
	uint16 *temp;
	int image_size = pitch_ * height_;

	uint16 r, g, b;
	int i;

	temp = new uint16 [image_size * 3];

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			temp[(i * 3) + 0] = GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].blue_);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].green_);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].red_);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16BitPerChannel;
		break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			uint16 data = ((uint16 *)buffer_)[i];

			r = ((data >> 10) & 31) << 3;
			g = ((data >> 5) & 63) << 3;
			b = ((data) & 31) << 3;

			temp[(i * 3) + 0] = GammaLookup::GammaToLinear((uint8)b);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinear((uint8)g);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinear((uint8)r);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16BitPerChannel;
		break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			uint16 data = ((uint16 *)buffer_)[i];

			r = ((data >> 11) & 31) << 3;
			g = ((data >> 5) & 63) << 2;
			b = ((data) & 31) << 3;

			temp[(i * 3) + 0] = GammaLookup::GammaToLinear((uint8)b);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinear((uint8)g);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinear((uint8)r);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16BitPerChannel;
		break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			temp[(i * 3) + 0] = GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 0]);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 1]);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 2]);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16BitPerChannel;
		break;

	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			temp[(i * 3) + 0] = GammaLookup::GammaToLinear(((uint8*)buffer_)[(i<<2) + 0]);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinear(((uint8*)buffer_)[(i<<2) + 1]);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinear(((uint8*)buffer_)[(i<<2) + 2]);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16BitPerChannel;
		break;

	case kBitdepth32BitPerChannel:
		for (i = 0; i < image_size; i++) {
			temp[i * 3 + 0] = (uint16)floor(((float32*)buffer_)[i * 3 + 0] * 65535.0f);
			temp[i * 3 + 1] = (uint16)floor(((float32*)buffer_)[i * 3 + 1] * 65535.0f);
			temp[i * 3 + 2] = (uint16)floor(((float32*)buffer_)[i * 3 + 2] * 65535.0f);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth16BitPerChannel;
		break;

	default:
		delete[] temp;
	}
}

void Canvas::ConvertTo96() {
	float32 *temp;
	int image_size = pitch_ * height_;

	uint16 r, g, b;
	int i;

	temp = new float32 [image_size * 3];

	const float32 one_over255 = 1.0f / 255.0f;

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			temp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)palette_[((uint8*)buffer_)[i]].blue_ * one_over255);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)palette_[((uint8*)buffer_)[i]].green_ * one_over255);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)palette_[((uint8*)buffer_)[i]].red_ * one_over255);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32BitPerChannel;
		break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			uint16 data = ((uint16 *)buffer_)[i];

			r = ((data >> 10) & 31) << 3;
			g = ((data >> 5) & 63) << 3;
			b = ((data) & 31) << 3;

			temp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * one_over255);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * one_over255);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * one_over255);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32BitPerChannel;
		break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			uint16 data = ((uint16 *)buffer_)[i];

			r = ((data >> 11) & 31) << 3;
			g = ((data >> 5) & 63) << 2;
			b = ((data) & 31) << 3;

			temp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * one_over255);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * one_over255);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * one_over255);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32BitPerChannel;
		break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			b = ((uint8*)buffer_)[i * 3 + 0];
			g = ((uint8*)buffer_)[i * 3 + 1];
			r = ((uint8*)buffer_)[i * 3 + 2];

			temp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * one_over255);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * one_over255);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * one_over255);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32BitPerChannel;
		break;

	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			b = ((uint8*)buffer_)[(i<<2) + 0];
			g = ((uint8*)buffer_)[(i<<2) + 1];
			r = ((uint8*)buffer_)[(i<<2) + 2];
			temp[(i * 3) + 0] = GammaLookup::GammaToLinearF((float32)b * one_over255);
			temp[(i * 3) + 1] = GammaLookup::GammaToLinearF((float32)g * one_over255);
			temp[(i * 3) + 2] = GammaLookup::GammaToLinearF((float32)r * one_over255);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32BitPerChannel;
		break;

	case kBitdepth16BitPerChannel:
		for (i = 0; i < image_size; i++) {
			temp[i * 3 + 0] = (float32)((uint16*)buffer_)[i * 3 + 0] / 65535.0f;
			temp[i * 3 + 1] = (float32)((uint16*)buffer_)[i * 3 + 1] / 65535.0f;
			temp[i * 3 + 2] = (float32)((uint16*)buffer_)[i * 3 + 2] / 65535.0f;
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth32BitPerChannel;
		break;

	default:
		delete[] temp;
	}
}

void Canvas::ConvertTo8BitGrayscale() {
	uint8 *temp;
	int image_size = pitch_ * height_;

	uint16 data;
	float32 r, g, b;
	int i;

	temp = new uint8 [image_size];

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			r = (float32)GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].red_);
			g = (float32)GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].green_);
			b = (float32)GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].blue_);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((data >> 10) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((data >> 5) & 31) << 3);
			b = (float32)GammaLookup::GammaToLinear((uint8)((data) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((data >> 11) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((data >> 5) & 63) << 2);
			b = (float32)GammaLookup::GammaToLinear((uint8)((data) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			b = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;
	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			b = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 4 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 4 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 4 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;

	case kBitdepth16BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = (float32)((uint16*)buffer_)[i * 3 + 0];
			g = (float32)((uint16*)buffer_)[i * 3 + 1];
			r = (float32)((uint16*)buffer_)[i * 3 + 2];

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;

	case kBitdepth32BitPerChannel:
		for (i = 0; i < image_size; i++) {
			b = ((float32*)buffer_)[i * 3 + 0];
			g = ((float32*)buffer_)[i * 3 + 1];
			r = ((float32*)buffer_)[i * 3 + 2];

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			temp[i] = (uint8)floor(GammaLookup::LinearToGammaF(r * 0.3f + g * 0.59f + b * 0.11f) * 255.0f);
		}

		if (buffer_responsibility_ == true) {
			delete[] (char*)buffer_;
		}
		buffer_responsibility_ = true;
		buffer_ = temp;
		bit_depth_ = kBitdepth8Bit;
		break;

	default:
		delete[] temp;
		return;
	}

	for (i = 0; i < 256; i++) {
		palette_[i].red_   = (uint8)i;
		palette_[i].green_ = (uint8)i;
		palette_[i].blue_  = (uint8)i;
	}

	pixel_size_ = 1;
}

void Canvas::ConvertToGray() {
	int image_size = pitch_ * height_;

	uint16 data;
	float32 r, g, b;
	uint8 v;
	int i;

	switch(bit_depth_) {
	case kBitdepth8Bit:
		for (i = 0; i < image_size; i++) {
			r = (float32)GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].red_);
			g = (float32)GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].green_);
			b = (float32)GammaLookup::GammaToLinear(palette_[((uint8*)buffer_)[i]].blue_);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			((uint8*)buffer_)[i] = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));
		}

		for (i = 0; i < 256; i++) {
			palette_[i].red_ = (uint8)i;
			palette_[i].green_ = (uint8)i;
			palette_[i].blue_ = (uint8)i;
		} break;

	case kBitdepth15Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((data >> 10) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((data >> 5) & 31) << 3);
			b = (float32)GammaLookup::GammaToLinear((uint8)((data) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			data = ((uint16)GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f)) >> 3);
			((uint16 *)buffer_)[i] = (data << 10) + (data << 5) + data;
		} break;

	case kBitdepth16Bit:
		for (i = 0; i < image_size; i++) {
			data = ((uint16 *)buffer_)[i];

			r = (float32)GammaLookup::GammaToLinear((uint8)((data >> 11) & 31) << 3);
			g = (float32)GammaLookup::GammaToLinear((uint8)((data >> 5) & 63) << 2);
			b = (float32)GammaLookup::GammaToLinear((uint8)((data) & 31) << 3);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			data = ((uint16)GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f)) >> 2);
			((uint16 *)buffer_)[i] = ((data >> 1) << 11) + (data << 5) + (data >> 1);
		} break;

	case kBitdepth24Bit:
		for (i = 0; i < image_size; i++) {
			b = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 3 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			v = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));

			((uint8*)buffer_)[i * 3 + 0] = v;
			((uint8*)buffer_)[i * 3 + 1] = v;
			((uint8*)buffer_)[i * 3 + 2] = v;
		} break;
	case kBitdepth32Bit:
		for (i = 0; i < image_size; i++) {
			b = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 4 + 0]);
			g = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 4 + 1]);
			r = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i * 4 + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			v = GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));

			((uint8*)buffer_)[i * 4 + 0] = v;
			((uint8*)buffer_)[i * 4 + 1] = v;
			((uint8*)buffer_)[i * 4 + 2] = v;
		} break;
	}
}




int Canvas::GenerateOctreeAndList(ColorOctreeNode* oct_top_node, ColorOctreeNodeListNode** first_link) {
	unsigned i;
	int offset = 0;
	bool first = true;
	int _num_links = 0;
	// Loop through all pixels and store the colors in the octree
	// and store each leafnode of the octree in the linked list.

	for (i = 0; i < width_ * height_; i++) {
		uint8 blue	= ((uint8*)buffer_)[offset + 0];
		uint8 green	= ((uint8*)buffer_)[offset + 1];
		uint8 red	= ((uint8*)buffer_)[offset + 2];

		ColorOctreeNode* oct_child_node = oct_top_node->InsertColor(red, green, blue, 0);

		// Insert this node in the list.
		if (oct_child_node != 0) {
			_num_links++;

			if (first == true) {
				(*first_link)->color_octree_node_ = oct_child_node;
				first = false;
			} else {
				ColorOctreeNodeListNode* new_list_node = new ColorOctreeNodeListNode();
				new_list_node->color_octree_node_ = oct_child_node;

				new_list_node->next_ = *first_link;
				*first_link = new_list_node;
			}

		}

		offset += 3;
	}

	return _num_links;
}



void Canvas::SortLinkedList(ColorOctreeNodeListNode** first_link, int num_links) {
	ColorOctreeNodeListNode* current_node;
	ColorOctreeNodeListNode** node_array = new ColorOctreeNodeListNode*[num_links];
	ColorOctreeNodeListNode** sorted_node_array = new ColorOctreeNodeListNode*[num_links];

	// Sorting step 1: Place all list nodes in an array, and get the maximum
	//                 number of different possible values to sort.
	int max_ref_count = 0;
	int i = 0;

	current_node = *first_link;

	while (current_node != 0) {
		if (current_node->color_octree_node_->GetReferenceCount() > max_ref_count) {
			max_ref_count = current_node->color_octree_node_->GetReferenceCount();
		}

		node_array[i] = current_node;
		i++;

		current_node = current_node->next_;
	}

	// Sorting step 2: Allocate and prepare buffers...
	int* value_count = new int[max_ref_count];
	int* offset = new int[max_ref_count];

	memset(value_count, 0, max_ref_count * sizeof(int));
	memset(offset, 0, max_ref_count * sizeof(int));

	// Sorting step 3: Count how many times each value occurs in the list.
	for (i = 0; i < num_links; i++) {
		int _index = node_array[i]->color_octree_node_->GetReferenceCount() - 1;
		value_count[_index]++;
	}

	// Sorting step 4: Calculate the insert offsets for each value...
	int temp_offset = 0;
	for (i = 0; i < max_ref_count; i++) {
		offset[i] = temp_offset;
		temp_offset += value_count[i];
	}

	// Sorting step 5: Sort the array...
	for (i = 0; i < num_links; i++) {
		int _index = node_array[i]->color_octree_node_->GetReferenceCount() - 1;
		int insert_index = offset[_index];
		offset[_index]++;

		sorted_node_array[insert_index] = node_array[i];
	}

	// Sorting step 6: Put it back in a linked list.

	for (i = 0; i < num_links; i++) {
		if (i == 0) {
			*first_link = sorted_node_array[i];
			current_node = *first_link;
		} else {
			current_node->next_ = sorted_node_array[i];
			current_node = current_node->next_;
		}

		current_node->next_ = 0;
	}

	delete[] node_array;
	delete[] sorted_node_array;
}


void Canvas::CrunchOctree(ColorOctreeNode* oct_top_node, ColorOctreeNodeListNode** first_link) {
	/*
		Crunching the octree means that all the leaf nodes at the lowest level are removed
		and letting their parent node hold their mean color value instead.

		Then the parent node is stored in a linked list, the "level above" list. When all
		leaf nodes are removed from the "current level" linked list, the "current level" list
		is replaced by the "level above" list, and the procedure starts all over again.

		This continues until the number of colors goes below 257.

		This method is not as fast as how fast Photoshop does it, and the result is not as good...
		But hey! This is the first time I ever wrote something like this! And hopefully, I'm never going
		to do it again!
	*/


	// This node is holding the first node in the "level above" list.
	ColorOctreeNodeListNode* first_node_in_level_above = 0;
	int _num_links = 0;

	int _num_colors = oct_top_node->GetNumLevelXChildren(8);

	while (_num_colors > 256) {
		// Check if this list (the "current level" list) is empty.
		if (*first_link != 0) {
			ColorOctreeNodeListNode* node = *first_link;

			// Unlink the first node.
			*first_link = (*first_link)->next_;

			if (node != 0) {
				// Crunch this node and decrease the color count.
				_num_colors -= node->color_octree_node_->CrunchChildren();

				// Now put the parent to this node in the "level above" list.
				// If the tag is set to 'true', then the parent node is already in the list.
				if (node->color_octree_node_->GetParent()->tag_ == false) {
					_num_links++;

					ColorOctreeNodeListNode* new_node = new ColorOctreeNodeListNode();
					new_node->next_ = 0;
					new_node->color_octree_node_ = node->color_octree_node_->GetParent();
					new_node->color_octree_node_->tag_ = true;

					if (first_node_in_level_above == 0) {
						first_node_in_level_above = new_node;
					} else {
						new_node->next_ = first_node_in_level_above;
						first_node_in_level_above = new_node;
					}
				}

				delete node;
			}
		} else {
			// The "current level" list is empty, and we are done with this level.

			// Set the "level above" list as the "current level" list.
			*first_link = first_node_in_level_above;

			// Start all over again with the "level above" list.
			first_node_in_level_above = 0;

			// Sort the "current level" list.
			SortLinkedList(first_link, _num_links);

			_num_links = 0;
		}
	}
}



void Canvas::ConvertTo8() {
	ConvertTo24();

	// Create an octree.
	ColorOctreeNode* _oct_top_node = new ColorOctreeNode();

	// Create a linked list to hold the next to the lowest level octree nodes.
	ColorOctreeNodeListNode* first_list_node = new ColorOctreeNodeListNode();

	// Step 1: Generate the octree and the linked list.
	int _num_links = GenerateOctreeAndList(_oct_top_node, &first_list_node);

	// Step 2: Sort the linked list by the octreenodes's reference count, using a bucketsort algorithm.
	SortLinkedList(&first_list_node, _num_links);

	// Step 3: Crunch the octree until the number of colors goes below 256.
	CrunchOctree(_oct_top_node, &first_list_node);

	// Delete all nodes in the list.
	ColorOctreeNodeListNode* current_node;
	ColorOctreeNodeListNode* prev_node;

	current_node = first_list_node;
	prev_node = 0;

	while (current_node != 0) {
		prev_node = current_node;
		current_node = current_node->next_;
		delete prev_node;
	}

	// Step 4: Generate palette.
	_oct_top_node->GetPalette(palette_, 0);

	// Step 5: Generate 8 bit Image.
	uint8* image = new uint8[width_ * height_];
	unsigned i;
	int src_offset = 0;

	for (i = 0; i < width_ * height_; i++) {
		uint8 b = ((uint8*)buffer_)[src_offset + 0];
		uint8 g = ((uint8*)buffer_)[src_offset + 1];
		uint8 r = ((uint8*)buffer_)[src_offset + 2];

		image[i] = (uint8)_oct_top_node->GetPaletteEntry(r, g, b);

		src_offset += 3;
	}

	if (buffer_responsibility_ == true) {
		delete[] (char*)buffer_;
	}
	buffer_responsibility_ = true;
	buffer_ = image;

	bit_depth_ = kBitdepth8Bit;

	delete _oct_top_node;
}
/*
uint8 Canvas::GetPaletteColor(int red, int green, int blue) {
	int	lBestColorIndex;
	int	i;

	float32 lBestDist;
	float32 dist;
	float32 lDeltaRed;
	float32 lDeltaGreen;
	float32 lDeltaBlue;

	if (palette_ == 0) {
		return 0;
	}

	for (i = 0; i < 256; i++) {
		lDeltaRed   = (float32)(red   - (int)palette_[i].red_);
		lDeltaGreen = (float32)(green - (int)palette_[i].green_);
		lDeltaBlue  = (float32)(blue  - (int)palette_[i].blue_);

		dist = (float32)sqrt(lDeltaRed * lDeltaRed +
									 lDeltaGreen * lDeltaGreen +
									 lDeltaBlue * lDeltaBlue);

		if (i == 0 || dist < lBestDist) {
			lBestColorIndex = i;
			lBestDist = dist;
		}
	}

	return (uint8)lBestColorIndex;
}
*/

void Canvas::GetPixelColor(unsigned x, unsigned y, Color& color) const {
	if (x < width_ && y < height_ && buffer_ != 0) {
		switch(bit_depth_) {
		case kBitdepth8Bit: {
				uint8 _index = ((uint8*)buffer_)[y * pitch_ + x];
				color = palette_[_index];
			} break;
		case kBitdepth15Bit: {
				uint16 _color = ((uint16*)buffer_)[y * pitch_ + x];
				color.red_   = (uint8)((_color >> 10) << 3);
				color.green_ = (uint8)(((_color >> 5) & 31) << 3);
				color.blue_  = (uint8) ((_color & 31) << 3);
			} break;
		case kBitdepth16Bit: {
				uint16 _color = ((uint16*)buffer_)[y * pitch_ + x];
				color.red_   = (uint8)((_color >> 11) << 3);
				color.green_ = (uint8)(((_color >> 5) & 63) << 2);
				color.blue_  = (uint8) ((_color & 31) << 3);
			} break;
		case kBitdepth24Bit: {
				unsigned offset = (y * pitch_ + x) * 3;
				color.blue_  = ((uint8*)buffer_)[offset + 0];
				color.green_ = ((uint8*)buffer_)[offset + 1];
				color.red_   = ((uint8*)buffer_)[offset + 2];
			} break;
		case kBitdepth32Bit: {
				unsigned offset = (y * pitch_ + x) * 4;
				color.blue_  = ((uint8*)buffer_)[offset + 0];
				color.green_ = ((uint8*)buffer_)[offset + 1];
				color.red_   = ((uint8*)buffer_)[offset + 2];
				color.alpha_ = ((uint8*)buffer_)[offset + 3];
			} break;
		case kBitdepth16BitPerChannel: {
				unsigned offset = (y * pitch_ + x) * 3;
				color.blue_  = GammaLookup::LinearToGamma(((uint16*)buffer_)[offset + 0]);
				color.green_ = GammaLookup::LinearToGamma(((uint16*)buffer_)[offset + 1]);
				color.red_   = GammaLookup::LinearToGamma(((uint16*)buffer_)[offset + 2]);
			} break;
		case kBitdepth32BitPerChannel: {
				unsigned offset = (y * pitch_ + x) * 3;
				color.blue_  = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[offset + 0]) * 255.0f);
				color.green_ = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[offset + 1]) * 255.0f);
				color.red_   = (uint8)floor(GammaLookup::LinearToGammaF(((float32*)buffer_)[offset + 2]) * 255.0f);
			} break;
		default:
			break;
		}
	}
}

void Canvas::SetPixelColor(unsigned x, unsigned y, Color& color) {
	if (x < width_ && y < height_ && buffer_ != 0) {
		switch(bit_depth_) {
		case kBitdepth8Bit: {
				((uint8*)buffer_)[y * pitch_ + x] = color.alpha_;
			} break;
		case kBitdepth15Bit: {
				uint16 r = (uint16)color.red_;
				uint16 g = (uint16)color.green_;
				uint16 b = (uint16)color.blue_;
				uint16 _color = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
				((uint16*)buffer_)[y * pitch_ + x] = _color;
			} break;
		case kBitdepth16Bit: {
				uint16 r = (uint16)color.red_;
				uint16 g = (uint16)color.green_;
				uint16 b = (uint16)color.blue_;
				uint16 _color = ((r >> 3) << 11) | ((g >> 2) << 6) | (b >> 3);
				((uint16*)buffer_)[y * pitch_ + x] = _color;
			} break;
		case kBitdepth24Bit: {
				unsigned offset = (y * pitch_ + x) * 3;
				((uint8*)buffer_)[offset + 0] = color.blue_;
				((uint8*)buffer_)[offset + 1] = color.green_;
				((uint8*)buffer_)[offset + 2] = color.red_;
			} break;
		case kBitdepth32Bit: {
				unsigned offset = (y * pitch_ + x) * 4;
				((uint8*)buffer_)[offset + 0] = color.blue_;
				((uint8*)buffer_)[offset + 1] = color.green_;
				((uint8*)buffer_)[offset + 2] = color.red_;
				((uint8*)buffer_)[offset + 3] = color.alpha_;
			} break;
		case kBitdepth16BitPerChannel: {
				unsigned offset = (y * pitch_ + x) * 3;
				((uint16*)buffer_)[offset + 0] = GammaLookup::GammaToLinear(color.blue_);
				((uint16*)buffer_)[offset + 1] = GammaLookup::GammaToLinear(color.green_);
				((uint16*)buffer_)[offset + 2] = GammaLookup::GammaToLinear(color.red_);
			} break;
		case kBitdepth32BitPerChannel: {
				unsigned offset = (y * pitch_ + x) * 3;
				((float32*)buffer_)[offset + 0] = GammaLookup::GammaToLinearF((float32)color.blue_  / 255.0f);
				((float32*)buffer_)[offset + 1] = GammaLookup::GammaToLinearF((float32)color.green_ / 255.0f);
				((float32*)buffer_)[offset + 2] = GammaLookup::GammaToLinearF((float32)color.red_   / 255.0f);
			} break;
		default:
			break;
		}
	}
}

void Canvas::Clear() {
	int line_offset = pitch_ * pixel_size_;
	int line_size   = width_ * pixel_size_;
	uint8* _buffer = (uint8*)buffer_;
	for (unsigned y = 0; y < height_; y++) {
		memset(&_buffer[y * line_offset], 0, line_size);
	}
}

void Canvas::PremultiplyAlpha() {
	if (bit_depth_ == kBitdepth32Bit && buffer_responsibility_ == true) {
		int image_size = pitch_ * height_;
		for (int i = 0; i < image_size * 4; i += 4) {
			unsigned b = (unsigned)((uint8*)buffer_)[i + 0];
			unsigned g = (unsigned)((uint8*)buffer_)[i + 1];
			unsigned r = (unsigned)((uint8*)buffer_)[i + 2];
			unsigned a = (unsigned)((uint8*)buffer_)[i + 3];

			((uint8*)buffer_)[i + 0] = (uint8)((b * a) / 255);
			((uint8*)buffer_)[i + 1] = (uint8)((g * a) / 255);
			((uint8*)buffer_)[i + 2] = (uint8)((r * a) / 255);
		}
	}
}

int Canvas::GetOutputRotation() const {
	return output_rotation_;
}

void Canvas::SetOutputRotation(int rotation) {
	output_rotation_ = rotation;
}

int Canvas::GetDeviceOutputRotation() const {
	return device_rotation_ + output_rotation_;
}

void Canvas::SetDeviceRotation(int rotation) {
	device_rotation_ = rotation;
}

unsigned Canvas::GetActualWidth() const {
	return (output_rotation_%180 == 0)? width_ : height_;
}

unsigned Canvas::GetActualHeight() const {
	return (output_rotation_%180 == 0)? height_ : width_;
}

void Canvas::SetAlpha(uint8 if_true, uint8 if_false, uint8 compare_value, CmpFunc func) {
	if (bit_depth_ != kBitdepth32Bit/* || buffer_responsibility_ == false*/) {
		return;
	}

	int image_size = pitch_ * height_;
	int i;

	bool gray = false;
	int channel = 0;

	switch(func) {
	case kCmpRedGreater:
	case kCmpRedGreaterOrEqual:
	case kCmpRedLess:
	case kCmpRedLessOrEqual:
	case kCmpRedEqual:
		channel = 2;
		break;
	case kCmpGreenGreater:
	case kCmpGreenGreaterOrEqual:
	case kCmpGreenLess:
	case kCmpGreenLessOrEqual:
	case kCmpGreenEqual:
		channel = 2;
		break;
	case kCmpBlueGreater:
	case kCmpBlueGreaterOrEqual:
	case kCmpBlueLess:
	case kCmpBlueLessOrEqual:
	case kCmpBlueEqual:
		channel = 2;
		break;
	case kCmpAlphaGreater:
	case kCmpAlphaGreaterOrEqual:
	case kCmpAlphaLess:
	case kCmpAlphaLessOrEqual:
	case kCmpAlphaEqual:
		channel = 3;
		break;
	case kCmpGrayGreater:
	case kCmpGrayGreaterOrEqual:
	case kCmpGrayLess:
	case kCmpGrayLessOrEqual:
	case kCmpGrayEqual:
		gray = true;
		break;
	case kCmpAlwaysTrue:
		break;
	}

	if (gray == true) {
		for (i = 0; i < image_size * 4; i += 4) {
			float32 b = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i + 0]);
			float32 g = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i + 1]);
			float32 r = (float32)GammaLookup::GammaToLinear(((uint8*)buffer_)[i + 2]);

			// The magic formula: 30% Red, 59% Green and 11% Blue.
			uint8 ch = (uint8)GammaLookup::LinearToGamma((uint16)(r * 0.3f + g * 0.59f + b * 0.11f));

			if (Compare(func, ch, compare_value) == true)
				((uint8*)buffer_)[i + 3] = if_true;
			else
				((uint8*)buffer_)[i + 3] = if_false;
		}
	} else {
		for (i = 0; i < image_size * 4; i += 4) {
			uint8 ch = (uint8)((uint8*)buffer_)[i + channel];

			if (Compare(func, ch, compare_value) == true)
				((uint8*)buffer_)[i + 3] = if_true;
			else
				((uint8*)buffer_)[i + 3] = if_false;
		}
	}
}

bool Canvas::Compare(CmpFunc cmp_func, uint8 value1, uint8 value2) {
	switch(cmp_func) {
	case kCmpRedGreater:
	case kCmpGreenGreater:
	case kCmpBlueGreater:
	case kCmpAlphaGreater:
	case kCmpGrayGreater:
		return value1 > value2;
	case kCmpRedGreaterOrEqual:
	case kCmpGreenGreaterOrEqual:
	case kCmpBlueGreaterOrEqual:
	case kCmpAlphaGreaterOrEqual:
	case kCmpGrayGreaterOrEqual:
		return value1 >= value2;
	case kCmpRedLess:
	case kCmpGreenLess:
	case kCmpBlueLess:
	case kCmpAlphaLess:
	case kCmpGrayLess:
		return value1 < value2;
	case kCmpRedLessOrEqual:
	case kCmpGreenLessOrEqual:
	case kCmpBlueLessOrEqual:
	case kCmpAlphaLessOrEqual:
	case kCmpGrayLessOrEqual:
		return value1 <= value2;
	case kCmpRedEqual:
	case kCmpGreenEqual:
	case kCmpBlueEqual:
	case kCmpAlphaEqual:
	case kCmpGrayEqual:
		return value1 == value2;
	case kCmpAlwaysTrue:
		return true;
	default:
		return false;
	}
}

void* Canvas::GetBuffer() const {
	return buffer_;
}

Canvas::BitDepth Canvas::IntToBitDepth(unsigned bit_depth) {
	if (bit_depth <= 8) {
		return kBitdepth8Bit;
	} else if(bit_depth <= 15) {
		return kBitdepth15Bit;
	} else if(bit_depth == 16) {
		return kBitdepth16Bit;
	} else if(bit_depth <= 24) {
		return kBitdepth24Bit;
	} else if(bit_depth <= 32) {
		return kBitdepth32Bit;
	} else if(bit_depth <= 48) {
		return kBitdepth16BitPerChannel;
	} else if(bit_depth <= 96) {
		return kBitdepth32BitPerChannel;
	}

	return kBitdepth8Bit;
}

unsigned Canvas::BitDepthToInt(BitDepth bit_depth) {
	switch(bit_depth) {
	case kBitdepth8Bit:
		return 8;
	case kBitdepth15Bit:
		return 15;
	case kBitdepth16Bit:
		return 16;
	case kBitdepth24Bit:
		return 24;
	case kBitdepth32Bit:
		return 32;
	case kBitdepth16BitPerChannel:
		return 48;
	case kBitdepth32BitPerChannel:
		return 96;
	}

	return 8;
}

Canvas::BitDepth Canvas::GetBitDepth() const {
	return bit_depth_;
}

unsigned Canvas::GetWidth() const {
	return width_;
}

unsigned Canvas::GetHeight() const {
	return height_;
}

unsigned Canvas::GetPixelByteSize() const {
	return pixel_size_;
}

unsigned Canvas::GetBufferByteSize() const {
	return pitch_ * height_ * pixel_size_;
}

void Canvas::SetPitch(unsigned pitch) {
	pitch_ = pitch;
}

unsigned Canvas::GetPitch() const {
	return pitch_;
}

const Color* Canvas::GetPalette() const {
	return palette_;
}

unsigned Canvas::GetPow2Width() {
	return PowerUp(GetWidth());
}

unsigned Canvas::GetPow2Height() {
	return PowerUp(GetHeight());
}

void Canvas::operator= (const Canvas& canvas) {
	Copy(canvas);
}

Color Canvas::GetPixelColor(unsigned x, unsigned y) const {
	Color _color;
	GetPixelColor(x, y, _color);
	return _color;
}







/////////////////////////////////////////////////////
//                                                 //
//             Class ColorOctreeNode               //
//                                                 //
/////////////////////////////////////////////////////



ColorOctreeNode::ColorOctreeNode() {
	red_ = 0;
	green_ = 0;
	blue_ = 0;

	reference_count_ = 0;
	num_children_ = 0;
	level_ = -1;

	tag_ = false;

	int i;

	for (i = 0; i < 8; i++) {
		children_[i] = 0;
	}

	parent_ = 0;
}



ColorOctreeNode::~ColorOctreeNode() {
	int i;

	for (i = 0; i < 8; i++) {
		if (children_[i] != 0) {
			delete children_[i];
		}
	}
}



int ColorOctreeNode::GetNumLevelXChildren(int level) {
	if (level <= level_) {
		return 0;
	}

	if (level == (level_ + 1)) {
		return num_children_;
	}

	int i;
	int count = 0;

	for (i = 0; i < 8; i++) {
		if (children_[i] != 0) {
			count += children_[i]->GetNumLevelXChildren(level);
		}
	}

	return count;
}

int ColorOctreeNode::GetChildOffset(uint8 r, uint8 g, uint8 b, int level) {
	int bit_mask = (1 << (7 - level));
	int x = 0;
	int y = 0;
	int z = 0;

	if ((r & bit_mask) != 0) {
		x = 1;
	}
	if ((g & bit_mask) != 0) {
		y = 1;
	}
	if ((b & bit_mask) != 0) {
		z = 1;
	}

	return((z << 2) + (y << 1) + x);
}

ColorOctreeNode* ColorOctreeNode::InsertColor(uint8 r, uint8 g, uint8 b, int level) {
	level_ = level;
	reference_count_++;

	if (level < 8) {
		int child_offset = GetChildOffset(r, g, b, level);

		if (children_[child_offset] == 0) {
			children_[child_offset] = new ColorOctreeNode();
			children_[child_offset]->parent_ = this;
			num_children_++;
		}

		return children_[child_offset]->InsertColor(r, g, b, level + 1);
	} else {
		// Level is 8, which is the last level.
		red_ = r;
		green_ = g;
		blue_ = b;

		// Only return a valid value if this is the parents first reference.
		if (parent_->reference_count_ == 1) {
			return parent_;
		} else {
			return 0;
		}
	}
}

bool ColorOctreeNode::InsertUniqueColor(uint8 r, uint8 g, uint8 b, int level) {
	level_ = level;

	if (level < 8) {
		int child_offset = GetChildOffset(r, g, b, level);

		if (children_[child_offset] == 0) {
			children_[child_offset] = new ColorOctreeNode();
			children_[child_offset]->parent_ = this;
			num_children_++;
		}

		if (children_[child_offset]->InsertUniqueColor(r, g, b, level + 1) == true) {
			reference_count_++;
			return true;
		} else {
			return false;
		}
	} else {
		// Level is 8, which is the last level.
		red_ = r;
		green_ = g;
		blue_ = b;

		// Only return a valid value if this is the first reference.
		if (reference_count_ == 0) {
			reference_count_++;
			return true;
		} else {
			return 0;
		}
	}

}

bool ColorOctreeNode::RemoveColor(uint8 r, uint8 g, uint8 b) {
	if (num_children_ == 0) {
		return false;
	}

	int child_offset = GetChildOffset(r, g, b, level_);

	if (children_[child_offset] != 0) {
		if (children_[child_offset]->level_ == 8 &&
			children_[child_offset]->red_ == r &&
			children_[child_offset]->green_ == g &&
			children_[child_offset]->blue_ == b) {
			children_[child_offset]->reference_count_--;

			if (children_[child_offset]->reference_count_ <= 0) {
				delete children_[child_offset];
				children_[child_offset] = 0;
				return true;
			}
		} else {
			return children_[child_offset]->RemoveColor(r, g, b);
		}
	}

	return false;
}


int ColorOctreeNode::CrunchChildren() {
	if (num_children_ == 0) {
		return 0;
	}

	int i;
	int num_children = num_children_;
	int colors_removed = 0;

	// Step 1: Crunch recursive.
	for (i = 0; i < 8; i++) {
		if (children_[i] != 0) {
			colors_removed += children_[i]->CrunchChildren();
		}
	}

	// Step 2: Get the mean color of all child nodes and the reference sum. Delete the children.
	int r = 0;
	int g = 0;
	int b = 0;
	int ref_sum = 0;

	for (i = 0; i < 8; i++) {
		if (children_[i] != 0) {
			r += children_[i]->red_;
			g += children_[i]->green_;
			b += children_[i]->blue_;

			ref_sum += children_[i]->reference_count_;

			delete children_[i];
			children_[i] = 0;

			num_children_--;
		}
	}

	r /= num_children;
	g /= num_children;
	b /= num_children;

	red_ = (uint8)r;
	green_ = (uint8)g;
	blue_ = (uint8)b;

	// Return the number of children crunched.
	colors_removed += (num_children - 1);
	return colors_removed;
}



int ColorOctreeNode::GetPalette(Color* palette, int index) {
	int i;

	if (num_children_ > 0) {
		for (i = 0; i < 8; i++) {
			if (children_[i] != 0) {
				index = children_[i]->GetPalette(palette, index);
			}
		}

		return index;
	} else {
		palette[index].red_ = red_;
		palette[index].green_ = green_;
		palette[index].blue_ = blue_;

		// Save this index value for later use.
		palette_index_ = (uint8)index;

		return index + 1;
	}
}



int ColorOctreeNode::GetPaletteEntry(uint8 r, uint8 g, uint8 b) {
	if (num_children_ == 0) {
		return palette_index_;
	}

	int child_offset = GetChildOffset(r, g, b, level_);

	if (children_[child_offset] != 0) {
		return children_[child_offset]->GetPaletteEntry(r, g, b);
	}

	return 0;
}



/////////////////////////////////////////////////////
//                                                 //
//          Class ColorOctreeNodeListNode          //
//                                                 //
/////////////////////////////////////////////////////



ColorOctreeNodeListNode::ColorOctreeNodeListNode() {
	color_octree_node_ = 0;
	next_ = 0;
}

}
