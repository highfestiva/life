/*
	Class:  ImageLoader
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A class used to load an image into a Canvas, by using all
	file-format specific image loaders in Lepra. With other words,
	you can load images stored using the bmp-, jpeg-, tga-, tiff-
	and png-format.

	The save functions let you save the contents of a Canvas in any of
	the previously mentioned file formats.
*/

#pragma once

#include "String.h"



namespace Lepra
{



class Canvas;
class Reader;
class Writer;



class ImageLoader
{
public:

	enum FileType
	{
		UNKNOWN = -1,
		BMP,
		TGA,
		TIF,
		JPG,
		PNG
	};

	ImageLoader();
	virtual ~ImageLoader();

	static FileType GetFileTypeFromName(const str& pFilename);

	bool Load(const str& pFileName, Canvas& pCanvas);
	bool Save(const str& pFileName, const Canvas& pCanvas);

	bool Load(FileType pFileType, Reader& pReader, Canvas& pCanvas);
	bool Save(FileType pFileType, Writer& pWriter, const Canvas& pCanvas);
};



}
