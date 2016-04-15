
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../../Lepra/Include/CyclicArray.h"
#include "../../../Lepra/Include/DiskFile.h"
#include "../../../Lepra/Include/Path.h"
#include "../../Include/X11/UiX11FontManager.h"



namespace UiTbc
{



FontManager* FontManager::Create(UiLepra::DisplayManager* pDisplayManager)
{
	return new X11FontManager;
}



X11FontManager::X11Font::X11Font():
	mX11LoadedCharFace(0)
{
	::memset(&mCharWidthOffset[0], 0, sizeof(mCharWidthOffset));
}

bool X11FontManager::X11Font::GetCharWidth(wchar_t pChar, int& pWidth, int& pOffset)
{
	if (pChar < LEPRA_ARRAY_COUNT(mCharWidthOffset))
	{
		pWidth  = mCharWidthOffset[pChar].mWidth;
		pOffset = mCharWidthOffset[pChar].mOffset;
		return mCharWidthOffset[pChar].mSet;
	}
	CharPlacementMap::const_iterator x = mCharPlacements.find(pChar);
	if (x != mCharPlacements.end())
	{
		pWidth  = x->second.mWidth;
		pOffset = x->second.mOffset;
		return true;
	}
	return false;
}

void X11FontManager::X11Font::PutCharWidth(wchar_t pChar, int pWidth, int pOffset)
{
	if (pChar < LEPRA_ARRAY_COUNT(mCharWidthOffset))
	{
		mCharWidthOffset[pChar].mSet = true;
		mCharWidthOffset[pChar].mWidth = pWidth;
		mCharWidthOffset[pChar].mOffset = pOffset;
	}
	else
	{
		mCharPlacements[pChar] = {true, pWidth, pOffset};
	}
}



X11FontManager::X11FontManager()
{
	FT_Init_FreeType(&mLibrary);
}

X11FontManager::~X11FontManager()
{
	FontTable::iterator x = mFontTable.begin();
	if (x != mFontTable.end())
	{
		X11Font* lFont = (X11Font*)x->second;
		FT_Done_Face(lFont->mX11Face);
		delete lFont;
	}
	mFontTable.clear();

	FT_Done_FreeType(mLibrary);
}




X11FontManager::FontId X11FontManager::AddFont(const str& pFontName, double pSize, int pFlags)
{
	if (mFontFiles.empty())
	{
		strutil::Append(mFontFiles, FindAllFontFiles("/usr/share/fonts"));
		strutil::Append(mFontFiles, FindAllFontFiles("/usr/local/share/fonts"));
	}
	strutil::strvec lSuffixes;
	if (pFlags & (BOLD|ITALIC))
	{
		lSuffixes.push_back("-BoldItalic");
		lSuffixes.push_back("bi.");
		lSuffixes.push_back("z.");
		lSuffixes.push_back("-Italic");
		lSuffixes.push_back("i.");
		lSuffixes.push_back("-Bold");
		lSuffixes.push_back("b.");
		lSuffixes.push_back("b");
		lSuffixes.push_back("");
	}
	else if (pFlags & BOLD)
	{
		lSuffixes.push_back("-Bold.");
		lSuffixes.push_back("b.");
		lSuffixes.push_back("Bold");
		lSuffixes.push_back("b");
		lSuffixes.push_back("");
	}
	else if (pFlags & ITALIC)
	{
		lSuffixes.push_back("-Italic.");
		lSuffixes.push_back("i.");
		lSuffixes.push_back("Italic");
		lSuffixes.push_back("i");
		lSuffixes.push_back("");
	}
	lSuffixes.push_back("-Regular.");
	lSuffixes.push_back(".");
	lSuffixes.push_back("Regular");
	lSuffixes.push_back("Light");
	lSuffixes.push_back("");
	const str lFontFile = GetFontFile(pFontName, lSuffixes);
	FT_Face lFace;
	if (lFontFile.empty() || FT_New_Face(mLibrary, lFontFile.c_str(), 0, &lFace) != 0)
	{
		mLog.Debugf("Unable to find font %s.", pFontName.c_str());
		return INVALID_FONTID;
	}
	FT_Set_Char_Size(lFace, 0, int(pSize*0.7*64), 90, 90);
	X11Font* lFont = new X11Font;
	lFont->mX11Face = lFace;
	lFont->mX11LoadedCharFace = 0;
	lFont->mName = pFontName;
	lFont->mSize = pSize;
	lFont->mFlags = pFlags;
	if (!InternalAddFont(lFont))
	{
		delete (lFont);
		FT_Done_Face(lFace);
		return INVALID_FONTID;
	}
	return lFont->mFontId;
}

bool X11FontManager::RenderGlyph(wchar_t pChar, Canvas& pImage, const PixelRect& pRect)
{
	bool lOk = (mCurrentFont != 0);
	if (lOk)
	{
		lOk = (pRect.mTop >= 0 && pRect.mLeft >= 0 &&
			pRect.GetWidth() >= 1 && pRect.GetHeight() >= 1 &&
			pRect.mRight <= (int)pImage.GetWidth() && pRect.mBottom <= (int)pImage.GetHeight());
	}
	if (lOk)
	{
		X11Font* lFont = (X11Font*)mCurrentFont;
		if (lFont->mX11LoadedCharFace != pChar)
		{
			FT_Load_Char(lFont->mX11Face, pChar, FT_LOAD_RENDER|FT_LOAD_TARGET_LCD|FT_LOAD_PEDANTIC|FT_LOAD_FORCE_AUTOHINT);
			lFont->mX11LoadedCharFace = pChar;
		}
		const FT_GlyphSlot lGlyph = lFont->mX11Face->glyph;
		const FT_Bitmap* lFaceBitmap = &lGlyph->bitmap;
		const uint8* lRawFaceBitmap = lGlyph->bitmap.buffer;
		const int lWidth = lFaceBitmap->width/3;
		const int lHeight = lFaceBitmap->rows;
		const int lBaseLine = int(lFont->mSize * lFont->mX11Face->bbox.yMax / float(lFont->mX11Face->bbox.yMax - lFont->mX11Face->bbox.yMin)) + 1;
		const int lYOffset = lBaseLine - lGlyph->metrics.horiBearingY / 64;
		deb_assert(lWidth <= pRect.GetWidth());
		deb_assert(lYOffset >= 0);
		deb_assert(lYOffset+lHeight <= lFont->mSize);
		const int lBytePitch = std::abs(lFaceBitmap->pitch);
		uint8* lBitmap = new uint8[3*pRect.GetWidth()*pRect.GetHeight()];
		::memset(lBitmap, 0, 3*pRect.GetWidth()*pRect.GetHeight());
		for (int y = 0; y < lHeight; ++y)
		{
			const uint8* s = &lRawFaceBitmap[y*lBytePitch];
			uint8* d = &lBitmap[(lYOffset + y) * 3 * pImage.GetWidth()];
			for (int x = 0; x < 3*lWidth; ++x)
			{
				*d++ = *s++;
			}
		}

		Canvas::BitDepth lSourceBitDepth = Canvas::BITDEPTH_24_BIT;
		Canvas::BitDepth lTargetBitDepth = pImage.GetBitDepth();
		pImage.Reset(pRect.GetWidth(), pRect.GetHeight(), lSourceBitDepth);
		pImage.SetBuffer(lBitmap, false, true);
		lBitmap = 0;	// Don't delete it, ownership now taken by canvas.
		pImage.ConvertBitDepth(lTargetBitDepth);
		if (lFaceBitmap->pitch > 0)
		{
			pImage.FlipVertical();	// Glyph is upside down...
		}
		pImage.SwapRGBOrder();	// FreeType fonts are always BGR.
		for (int y = 0; y < pRect.GetHeight(); ++y)
		{
			for (int x = 0; x < pRect.GetWidth(); ++x)
			{
				Color lColor;
				pImage.GetPixelColor(x, y, lColor);
				int lIntensity = lColor.SumRgb();
				// Sum controls part of light intensity, average controls part.
				lIntensity = (lIntensity*20 + (lIntensity/3)*80) / 100;
				lIntensity = (lIntensity > 255)? 255 : lIntensity;
				lColor.mRed = lColor.mGreen = lColor.mBlue = 255;
				lColor.mAlpha = (uint8)(lIntensity);
				pImage.SetPixelColor(x, y, lColor);
			}
		}
	}
	return (lOk);
}



int X11FontManager::GetCharWidth(wchar_t pChar) const
{
	if (!mCurrentFont)
	{
		deb_assert(false);
		return 10;
	}

	X11Font* lFont = (X11Font*)mCurrentFont;
	int lWidth;
	int lOffset;
	if (lFont->GetCharWidth(pChar, lWidth, lOffset))
	{
		return lWidth;
	}
	if (lFont->mX11LoadedCharFace != pChar)
	{
		FT_Load_Char(lFont->mX11Face, pChar, FT_LOAD_RENDER|FT_LOAD_TARGET_LCD|FT_LOAD_PEDANTIC|FT_LOAD_FORCE_AUTOHINT);
		lFont->mX11LoadedCharFace = pChar;
	}
	lWidth = (lFont->mX11Face->glyph->metrics.horiAdvance - lFont->mX11Face->glyph->metrics.horiBearingX) / 64 + 1;
	lOffset = (lFont->mX11Face->glyph->metrics.horiBearingX - 63) / 64;
	lFont->PutCharWidth(pChar, lWidth, lOffset);
	return lWidth;
}

int X11FontManager::GetCharOffset(wchar_t pChar) const
{
	X11Font* lFont = (X11Font*)mCurrentFont;
	int lWidth;
	int lOffset;
	if (!lFont->GetCharWidth(pChar, lWidth, lOffset))
	{
		GetCharWidth(pChar);
		lFont->GetCharWidth(pChar, lWidth, lOffset);
	}
	return std::min(0, lOffset);
}



strutil::strvec X11FontManager::FindAllFontFiles(const str& pPath)
{
	strutil::strvec lFiles;
	DiskFile::FindData lData;
	if (!DiskFile::FindFirst(Path::JoinPath(pPath, "*"), lData))
	{
		return lFiles;
	}
	do
	{
		if (lData.IsSubDir())
		{
			strutil::Append(lFiles, FindAllFontFiles(lData.GetName()));
		}
		else if (strutil::EndsWith(lData.GetName(), ".ttf") || strutil::EndsWith(lData.GetName(), ".otf"))
		{
			lFiles.push_back(lData.GetName());
		}
	}
	while (DiskFile::FindNext(lData));
	return lFiles;
}

str X11FontManager::GetFontFile(const str& pFontName, const strutil::strvec& pSuffixes) const
{
	strutil::strvec lNames = strutil::Split(pFontName, " ");
	str lName0 = lNames[0];
	str lFullName = strutil::Join(lNames, "");
	strutil::ToLower(lName0);
	strutil::ToLower(lFullName);
	for (auto lSuffix: pSuffixes)
	{
		strutil::ToLower(lSuffix);
		for (auto lFontFile: mFontFiles)
		{
			str lLowerFontFile = lFontFile;
			strutil::ToLower(lLowerFontFile);
			//mLog.Infof("%s == %s || %s", lLowerFontFile.c_str(), (lFullName+lSuffix).c_str(), lName0.c_str());
			if (lLowerFontFile.find(lFullName+lSuffix) != str::npos)
			{
				return lFontFile;
			}
			if (lLowerFontFile.find(lName0+lSuffix) != str::npos)
			{
				return lFontFile;
			}
		}
	}
	return str();
}



loginstance(UI_GFX_2D, X11FontManager);



}
