
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

#include "../../../Lepra/Include/HashTable.h"
#include "../UiPainter.h"
#include "../UiTBC.h"




namespace UiTbc
{



class GUIImageManager
{
public:
	enum ImageStyle
	{
		TILED = 0,
		CENTERED,
		STRETCHED,
	};

	enum BlendFunc
	{
		NO_BLEND = 0,
		ALPHATEST,
		ALPHABLEND
	};

	GUIImageManager();
	~GUIImageManager();

	void SetPainter(Painter* pPainter);
	Painter* GetPainter() const;

	// TODO: come up with a way to avoid using an image loader directly in a
	// void LoadImages(const tchar* pImageDefinitionFile, const tchar* pArchive = 0);
	Painter::ImageID AddImage(const Canvas& pImage, ImageStyle pStyle, BlendFunc pBlendFunc, uint8 pAlphaValue);
	void AddLoadedImage(const Canvas& pImage, Painter::ImageID pImageId, ImageStyle pStyle, BlendFunc pBlendFunc, uint8 pAlphaValue);
	bool RemoveImage(Painter::ImageID pImageId);
	bool DropImage(Painter::ImageID pImageId);
	bool HasImage(Painter::ImageID pImageId) const;

	void SetImageOffset(Painter::ImageID pImageID, int pXOffset, int pYOffset);

	// Translates the name of the image to the corresponding ImageID.
	// The name is the name of the structure in the image definition file.
	Painter::ImageID GetImageID(const str& pImageName);

	void DrawImage(Painter::ImageID pImageID, int x, int y);
	void DrawImage(Painter::ImageID pImageID, const PixelRect& pRect);

	PixelCoord GetImageSize(Painter::ImageID pImageID);
	bool IsOverImage(Painter::ImageID pImageID, 
			 int pScreenX, int pScreenY, 
			 const PixelRect& pScreenRect);

	void SwapRGB();

private:
	void ClearImageTable();

	class Image
	{
	public:
		inline Image(Painter::ImageID pID,
					 const Canvas& pCanvas,
					 ImageStyle pStyle,
					 BlendFunc pBlendFunc,
					 uint8 pAlphaValue)
		{
			mID = pID;
			mCanvas.Copy(pCanvas);
			mStyle = pStyle;
			mBlendFunc = pBlendFunc;
			mAlphaValue = pAlphaValue;
			mXOffset = 0;
			mYOffset = 0;
		}

		Painter::ImageID mID;
		Canvas mCanvas;
		ImageStyle mStyle;
		BlendFunc mBlendFunc;
		uint8 mAlphaValue;

		int mXOffset;
		int mYOffset;
	};

	// An image table used by all components that need to store icons and 
	// other common images.
	typedef HashTable<Painter::ImageID, Image*, std::hash<int> > ImageTable;
	typedef HashTable<str, Painter::ImageID> IDTable;
	ImageTable mImageTable;
	IDTable mIDTable;

	Painter* mPainter;

	bool mSwapRGB;
};



}
