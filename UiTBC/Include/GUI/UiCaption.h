/*
	Class:  Caption
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UICAPTION_H
#define UICAPTION_H

#include "UiRectComponent.h"
#include "UiButton.h"
#include "UiLabel.h"
#include "UiTextComponent.h"

namespace UiTbc
{

class Caption: public RectComponent, public TextComponent
{
public:

	friend class Window;

	enum
	{
		MINIMIZE_BUTTON = (1 << 0),
		MAXIMIZE_BUTTON = (1 << 1),
		CLOSE_BUTTON    = (1 << 2),
	};

	Caption(const Lepra::Color& pActiveColor, const Lepra::Color& pInactiveColor, int pHeight);
	Caption(const Lepra::Color& pActiveTopLeftColor,
			const Lepra::Color& pActiveTopRightColor,
			const Lepra::Color& pActiveBottomLeftColor,
			const Lepra::Color& pActiveBottomRightColor,
			const Lepra::Color& pInactiveTopLeftColor,
			const Lepra::Color& pInactiveTopRightColor,
			const Lepra::Color& pInactiveBottomLeftColor,
			const Lepra::Color& pInactiveBottomRightColor,
			int pHeight);
	Caption(Painter::ImageID pActiveLeftImageID,
			Painter::ImageID pActiveRightImageID,
			Painter::ImageID pActiveCenterImageID,
			Painter::ImageID pInactiveLeftImageID,
			Painter::ImageID pInactiveRightImageID,
			Painter::ImageID pInactiveCenterImageID,
			int pHeight);

	virtual ~Caption();

	Button* SetLeftButton(Button* pButton);
	Button* SetRightButton(Button* pButton);
	Button* SetMiddleButton(Button* pButton);

	inline void SetIcon(Painter::ImageID pIconID);
	inline void SetText(const Lepra::String& pText,
						Painter::FontID pFontID,
						const Lepra::Color& pActiveTextColor,
						const Lepra::Color& pActiveBackgColor,
						const Lepra::Color& pInactiveTextColor,
						const Lepra::Color& pInactiveBackgColor,
						BlendFunc pBlendFunc = ALPHATEST,
						Lepra::uint8 pAlphaTreshold = 128);

	void SetActive(bool pActive);
	inline bool GetActive() const;

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);

	inline virtual Component::Type GetType();

protected:
	Button* SetButton(Button*& pMemberButton, Button* pNewButton);
	void ForceRepaint();

private:

	enum Style
	{
		SINGLECOLOR_STYLE = 0,
		MULTICOLOR_STYLE,
		IMAGE_STYLE,
	};

	void InitCaption();

	inline bool Check(unsigned pFlags, unsigned pFlag);

	Label* mLabel;
	RectComponent* mCaptionRect;
	RectComponent* mButtonRect;
	RectComponent* mLeftImageRect;
	RectComponent* mCenterImageRect;
	RectComponent* mRightImageRect;

	Button* mLeftButton;
	Button* mRightButton;
	Button* mMiddleButton;

	Lepra::Color mActiveTLColor;
	Lepra::Color mActiveTRColor;
	Lepra::Color mActiveBLColor;
	Lepra::Color mActiveBRColor;

	Lepra::Color mInactiveTLColor;
	Lepra::Color mInactiveTRColor;
	Lepra::Color mInactiveBLColor;
	Lepra::Color mInactiveBRColor;

	Painter::ImageID mActiveLeftImageID;
	Painter::ImageID mActiveRightImageID;
	Painter::ImageID mActiveCenterImageID;
	Painter::ImageID mInactiveLeftImageID;
	Painter::ImageID mInactiveRightImageID;
	Painter::ImageID mInactiveCenterImageID;

	Lepra::String mText;
	Lepra::Color mActiveTextBackgColor;
	Lepra::Color mInactiveTextColor;
	Lepra::Color mInactiveTextBackgColor;

	bool mMovingWindow;
	bool mActive;
	Style mStyle;
};

void Caption::SetIcon(Painter::ImageID pIconID)
{
	mLabel->SetIcon(pIconID);
}

void Caption::SetText(const Lepra::String& pText,
					  Painter::FontID pFontID,
					  const Lepra::Color& pActiveTextColor,
					  const Lepra::Color& pActiveBackgColor,
					  const Lepra::Color& pInactiveTextColor,
					  const Lepra::Color& pInactiveBackgColor,
					  BlendFunc pBlendFunc,
					  Lepra::uint8 pAlphaTreshold)
{
	mText                   = pText;
	mActiveTextBackgColor   = pActiveBackgColor;
	mInactiveTextColor      = pInactiveTextColor;
	mInactiveTextBackgColor = pInactiveBackgColor;
	SetFont(pFontID, pActiveTextColor, pBlendFunc, pAlphaTreshold);

	mLabel->SetText(mText, GetFontId(), GetTextColor(), mActiveTextBackgColor, GetTextBlendFunc(), GetTextAlpha());
}

bool Caption::Check(unsigned pFlags, unsigned pFlag)
{
	return ((pFlags & pFlag) != 0);
}

Component::Type Caption::GetType()
{
	return Component::CAPTION;
}

bool Caption::GetActive() const
{
	return mActive;
}

} // End namespace.

#endif
