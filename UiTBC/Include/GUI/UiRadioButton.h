/*
	Class:  RadioButton
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#ifndef UIRADIOBUTTON_H
#define UIRADIOBUTTON_H

#include "UiButton.h"

namespace UiTbc
{

class RadioButton : public Button
{
public:

	RadioButton(const Color& pBodyColor, const str& pName);
	RadioButton(Painter::ImageID pReleasedImageID,
			    Painter::ImageID pPressedImageID,
			    Painter::ImageID pReleasedActiveImageID,	// Mouse over.
			    Painter::ImageID pPressedActiveImageID,
			    Painter::ImageID pReleasingImageID,
			    Painter::ImageID pPressingImageID,
			    const str& pName);

	virtual ~RadioButton();

	virtual void Repaint(Painter* pPainter);
	virtual bool IsOver(int pScreenX, int pScreenY);

	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	inline virtual Type GetType() const;

protected:

	virtual void OnTextChanged();

private:

	void AddImageToPainter(Painter::ImageID& pImageID, const uint8 pImage[], int pDim, Painter* pPainter);

	static Painter::ImageID smIconRadioCheckID;
	static Painter::ImageID smRadioButtonInnerID;
	static Painter::ImageID smRadioButtonLight1ID;
	static Painter::ImageID smRadioButtonLight2ID;
	static Painter::ImageID smRadioButtonDark1ID;
	static Painter::ImageID smRadioButtonDark2ID;
	static Painter* smPrevPainter;


	static const uint8 smIconRadioCheck[];
	static const uint8 smRadioButtonInner[];
	static const uint8 smRadioButtonLight1Col[];
	static const uint8 smRadioButtonLight2Col[];
	static const uint8 smRadioButtonDark1Col[];
	static const uint8 smRadioButtonDark2Col[];

	bool mUserDefinedGfx;
	bool mUpdateSize;

	Color mBodyColor;
	Color mLightColor;
	Color mDarkColor1;
	Color mDarkColor2;

	Painter::ImageID mReleasedID;
	Painter::ImageID mReleasedActiveID;
	Painter::ImageID mReleasingID;
	Painter::ImageID mPressedID;
	Painter::ImageID mPressedActiveID;
	Painter::ImageID mPressingID;
};

Component::Type RadioButton::GetType() const
{
	return Component::RADIOBUTTON;
}

} // End namespace.

#endif