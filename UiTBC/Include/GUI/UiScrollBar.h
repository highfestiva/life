
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/Timer.h"
#include "UiRectComponent.h"
#include "UiButton.h"
#include "UiCleaner.h"



namespace UiTbc
{



class ScrollBar: public RectComponent
{
	typedef RectComponent Parent;
public:

	enum Style
	{
		HORIZONTAL = 0,
		VERTICAL,
	};

	ScrollBar(Style pStyle = HORIZONTAL,
		  int pSize = 16, int pButtonSize = 16,
		  const Lepra::Color& pBodyColor = Lepra::Color(192, 192, 192),
		  const Lepra::Color& pBackgColor = Lepra::Color(192, 192, 192),
		  BorderComponent::BorderShadeFunc pBorderShadeFunc = BorderComponent::LINEAR,
		  int pBorderWidth = 3,
		  const Lepra::String& pName = _T("ScrollBar"));

	ScrollBar(Style pStyle,
		  Painter::ImageID pBackgImageID,
		  Button* pTopLeftButton,
		  Button* pBottomRightButton,
		  Button* pScrollerButton,
		  const Lepra::String& pName = _T("ScrollBar"));

	// 0 <= visible <= max.
	void SetScrollRatio(Lepra::float64 pVisible, Lepra::float64 pMax);
	Lepra::float64 GetScrollRatioVisible() const;
	Lepra::float64 GetScrollRatioMax() const;

	// Makes the scroller width/height static with the given size.
	inline void SetStaticScrollerSize(int pScrollerSize);

	// Scroll position is in the range [0, 1].
	inline Lepra::float64 GetScrollPos();
	void SetScrollPos(Lepra::float64 pPos);

	inline void SetScrollDelay(Lepra::float64 pFirstDelay, Lepra::float64 pDelay);

	virtual void Repaint(Painter* pPainter);

	virtual void DoSetSize(int pWidth, int pHeight);

	virtual void OnConnectedToDesktopWindow();
	virtual void OnIdle();

	inline virtual Component::Type GetType();

	inline void SetOwner(Component* pOwner);

protected:

	GridLayout* CreateLayout(Style pStyle);
	void LoadIcons();
	void LoadButtons();
	void CheckAndSetSize();
	void CheckButtonSize(Button* pButton);
	void InitPreferredSize();
	void DoLayout();

	void OnScrollTL(Button* pButton);
	void OnScrollBR(Button* pButton);
	void OnStopScroll(Button* pButton);
	void OnScrollerDown(Button* pButton);
	void OnScrollerDragged(Button* pButton, int pDeltaX, int pDeltaY);

private:

	void AddImage(Painter::ImageID& pImageID, Lepra::uint8 pImage[], int pDim);

	// Cleans up among the static variables.
	class ScrollbarCleaner : public Cleaner
	{
	public:
		~ScrollbarCleaner()
		{
			ScrollBar::smIconLeftID  = Painter::INVALID_IMAGEID;
			ScrollBar::smIconRightID = Painter::INVALID_IMAGEID;
			ScrollBar::smIconUpID    = Painter::INVALID_IMAGEID;
			ScrollBar::smIconDownID  = Painter::INVALID_IMAGEID;
			ScrollBar::smPrevPainter = 0;
			ScrollBar::smCleaner = 0;
		}
	};

	static Lepra::uint8 smIconArrowLeft[];
	static Lepra::uint8 smIconArrowRight[];
	static Lepra::uint8 smIconArrowUp[];
	static Lepra::uint8 smIconArrowDown[];

	static Painter::ImageID smIconLeftID;
	static Painter::ImageID smIconRightID;
	static Painter::ImageID smIconUpID;
	static Painter::ImageID smIconDownID;

	static Painter* smPrevPainter;
	static ScrollbarCleaner* smCleaner;

	Style mStyle;

	bool mUserDefinedGfx;

	Painter::ImageID mBackgImageID;
	Button* mTLButton;
	Button* mBRButton;
	Button* mScrollerButton;
	RectComponent* mTLRect;
	RectComponent* mBRRect;

	Lepra::Color mBodyColor;

	Lepra::float64 mVisible;
	Lepra::float64 mMax;
	Lepra::float64 mPos;
	Lepra::float64 mScrollSpeed;
	Lepra::float64 mFirstDelay;
	Lepra::float64 mDelay;

	Lepra::Timer mTimer;
	bool mFirstDelayDone;

	int mSize;
	int mButtonSize;
	int mScrollerSize;
	BorderComponent::BorderShadeFunc mBorderShadeFunc;
	int mBorderWidth;

	Component* mOwner;
};

Lepra::float64 ScrollBar::GetScrollPos()
{
	return mPos;
}

void ScrollBar::SetScrollDelay(Lepra::float64 pFirstDelay, Lepra::float64 pDelay)
{
	mFirstDelay = pFirstDelay;
	mDelay = pDelay;
}

void ScrollBar::SetStaticScrollerSize(int pScrollerSize)
{
	mScrollerSize = pScrollerSize;
	SetNeedsRepaint(true);
}

Component::Type ScrollBar::GetType()
{
	return Component::SCROLLBAR;
}

void ScrollBar::SetOwner(Component* pOwner)
{
	mOwner = pOwner;
}



}
