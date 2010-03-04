
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

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

	Caption(const Color& pActiveColor, const Color& pInactiveColor, int pHeight);
	Caption(const Color& pActiveTopLeftColor,
			const Color& pActiveTopRightColor,
			const Color& pActiveBottomLeftColor,
			const Color& pActiveBottomRightColor,
			const Color& pInactiveTopLeftColor,
			const Color& pInactiveTopRightColor,
			const Color& pInactiveBottomLeftColor,
			const Color& pInactiveBottomRightColor,
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

	void SetIcon(Painter::ImageID pIconID);
	void SetText(const str& pText,
						const Color& pActiveTextColor,
						const Color& pActiveBackgColor,
						const Color& pInactiveTextColor,
						const Color& pInactiveBackgColor);

	void SetActive(bool pActive);
	bool GetActive() const;

	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnLButtonUp(int pMouseX, int pMouseY);

	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);

	virtual Type GetType() const;

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

	bool Check(unsigned pFlags, unsigned pFlag);

	Label* mLabel;
	RectComponent* mCaptionRect;
	RectComponent* mButtonRect;
	RectComponent* mLeftImageRect;
	RectComponent* mCenterImageRect;
	RectComponent* mRightImageRect;

	Button* mLeftButton;
	Button* mRightButton;
	Button* mMiddleButton;

	Color mActiveTLColor;
	Color mActiveTRColor;
	Color mActiveBLColor;
	Color mActiveBRColor;

	Color mInactiveTLColor;
	Color mInactiveTRColor;
	Color mInactiveBLColor;
	Color mInactiveBRColor;

	Painter::ImageID mActiveLeftImageID;
	Painter::ImageID mActiveRightImageID;
	Painter::ImageID mActiveCenterImageID;
	Painter::ImageID mInactiveLeftImageID;
	Painter::ImageID mInactiveRightImageID;
	Painter::ImageID mInactiveCenterImageID;

	str mText;
	Color mActiveTextBackgColor;
	Color mInactiveTextColor;
	Color mInactiveTextBackgColor;

	bool mMovingWindow;
	bool mActive;
	Style mStyle;
};



}
