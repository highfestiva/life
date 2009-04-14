
// Author: Alexander Hugestrand, Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once

#include "UiTBC.h"
#include "UiPainter.h"
#include "../../Lepra/Include/String.h"



namespace UiTbc
{



class SystemPainter: public Painter
{
public:
	enum // FontFlags
	{
		BOLD      = (1 << 0),
		ITALIC    = (1 << 1),
		UNDERLINE = (1 << 2),
		STRIKEOUT = (1 << 3),
	};

	enum CharacterSet
	{
		NATIVE = 0, 
		ANSI,
	};

	SystemPainter();
	virtual ~SystemPainter();

	virtual Painter::FontID AddSystemFont(const Lepra::String& pFont, double pSize, Lepra::uint32 pFlags, CharacterSet pCharSet) = 0;

	virtual bool RenderGlyph(Lepra::tchar pChar, Lepra::Canvas& pImage, const Lepra::PixelRect& pRect) = 0;
};



}
