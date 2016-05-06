/*
	Class:  ImageLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A class used to load an image into a Canvas, by using all
	file-format specific image loaders in lepra. With other words,
	you can load images stored using the bmp-, jpeg-, tga-, tiff-
	and png-format.

	The save functions let you save the contents of a Canvas in any of
	the previously mentioned file formats.
*/

#pragma once

#include "string.h"



namespace lepra {



class Canvas;
class Reader;
class Writer;



class ImageLoader {
public:

	enum FileType {
		kUnknown = -1,
		kBmp,
		kTga,
		kTif,
		kJpg,
		PNG
	};

	ImageLoader();
	virtual ~ImageLoader();

	static FileType GetFileTypeFromName(const str& filename);

	bool Load(const str& file_name, Canvas& canvas);
	bool Save(const str& file_name, const Canvas& canvas);

	bool Load(FileType file_type, Reader& reader, Canvas& canvas);
	bool Save(FileType file_type, Writer& writer, const Canvas& canvas);
};



}
