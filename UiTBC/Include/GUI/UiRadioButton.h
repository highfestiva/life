/*
	Class:  RadioButton
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UIRADIOBUTTON_H
#define UIRADIOBUTTON_H

#include "UiButton.h"

namespace UiTbc
{

class RadioButton : public Button
{
public:

	RadioButton(const Lepra::Color& pBodyColor, const Lepra::String& pName);
	RadioButton(Painter::ImageID pReleasedImageID,
			    Painter::ImageID pPressedImageID,
			    Painter::ImageID pReleasedActiveImageID,	// Mouse over.
			    Painter::ImageID pPressedActiveImageID,
			    Painter::ImageID pReleasingImageID,
			    Painter::ImageID pPressingImageID,
			    const Lepra::String& pName);

	virtual ~RadioButton();

	virtual void Repaint(Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	inline virtual Type GetType();

protected:

	virtual void OnTextChanged();

private:

	void AddImageToPainter(Painter::ImageID& pImageID, const Lepra::uint8 pImage[], int pDim, Painter* pPainter);

	static Painter::ImageID smIconRadioCheckID;
	static Painter::ImageID smRadioButtonInnerID;
	static Painter::ImageID smRadioButtonLight1ID;
	static Painter::ImageID smRadioButtonLight2ID;
	static Painter::ImageID smRadioButtonDark1ID;
	static Painter::ImageID smRadioButtonDark2ID;
	static Painter* smPrevPainter;


	static const Lepra::uint8 smIconRadioCheck[];
	static const Lepra::uint8 smRadioButtonInner[];
	static const Lepra::uint8 smRadioButtonLight1Col[];
	static const Lepra::uint8 smRadioButtonLight2Col[];
	static const Lepra::uint8 smRadioButtonDark1Col[];
	static const Lepra::uint8 smRadioButtonDark2Col[];

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

Component::Type RadioButton::GetType()
{
	return Component::RADIOBUTTON;
}

} // End namespace.

#endif