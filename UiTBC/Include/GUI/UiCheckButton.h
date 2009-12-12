/*
	Class:  CheckButton
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UICHECKBUTTON_H
#define UICHECKBUTTON_H

#include "UiButton.h"

namespace UiTbc
{

class CheckButton : public Button
{
public:

	CheckButton(const Lepra::Color& pBodyColor, const Lepra::String& pName);
	CheckButton(Painter::ImageID pReleasedImageID,
		    Painter::ImageID pPressedImageID,
		    Painter::ImageID pReleasedActiveImageID,	// Mouse over.
		    Painter::ImageID pPressedActiveImageID,
		    Painter::ImageID pReleasingImageID,
		    Painter::ImageID pPressingImageID,
		    const Lepra::String& pName);

	virtual ~CheckButton();

	virtual void Repaint(Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

protected:

	virtual void OnTextChanged();

private:

	static Painter::ImageID smCheckIconID;
	static const Lepra::uint8 smIconCheck[];
	static Painter* smPrevPainter;

	bool mUserDefinedGfx;
	bool mUpdateSize;

	Lepra::Color mBodyColor;
	Lepra::Color mLightColor;
	Lepra::Color mDarkColor1;
	Lepra::Color mDarkColor2;

	Painter::ImageID mReleasedID;
	Painter::ImageID mReleasedActiveID;
	Painter::ImageID mReleasingID;
	Painter::ImageID mPressedID;
	Painter::ImageID mPressedActiveID;
	Painter::ImageID mPressingID;
};

} // End namespace.

#endif