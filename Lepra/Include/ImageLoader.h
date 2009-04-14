/*
	Class:  ImageLoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	A class used to load an image into a Canvas, by using all
	file-format specific image loaders in Lepra. With other words,
	you can load images stored using the bmp-, jpeg-, tga-, tiff-
	and png-format.

	The save functions let you save the contents of a Canvas in any of
	the previously mentioned file formats.
*/
#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include "String.h"
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
		BMP = 0,
		TGA,
		TIF,
		JPG,
		PNG
	};

	ImageLoader();
	virtual ~ImageLoader();

	bool Load(const String& pFileName, Canvas& pCanvas);
	bool Save(const String& pFileName, const Canvas& pCanvas);

	bool Load(const String& pArchiveName, const String& pFileName, Canvas& pCanvas);
	bool Save(const String& pArchiveName, const String& pFileName, const Canvas& pCanvas);

	bool Load(FileType pFileType, Reader& pReader, Canvas& pCanvas);
	bool Save(FileType pFileType, Writer& pWriter, const Canvas& pCanvas);

protected:
private:
};

} // End namespace.

#endif
