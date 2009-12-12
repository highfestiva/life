
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiRectComponent.h"
#include "UiBorderComponent.h"
#include "UiGridLayout.h"



namespace UiTbc
{



class Caption;



class Window: public Component
{
	typedef Component Parent;
public:

	friend class FloatingLayout;

	enum
	{
		BORDER_RESIZABLE     = (1 << 0),
		BORDER_SUNKEN        = (1 << 1),
		BORDER_LINEARSHADING = (1 << 2),
		BORDER_HALF          = (1 << 3),
	};

	Window(const Lepra::String& pName, Layout* pLayout = 0);
	Window(unsigned pBorderStyle, int pBorderWidth, const Lepra::Color& pColor,
		const Lepra::String& pName, Layout* pLayout = 0);
	Window(unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
		const Lepra::String& pName, Layout* pLayout = 0);
	Window(const Lepra::Color& pColor, const Lepra::String& pName, Layout* pLayout = 0);
	Window(Painter::ImageID pImageID, const Lepra::String& pName, Layout* pLayout = 0);
	virtual ~Window();

	void SetBorder(unsigned pBorderStyle, int pWidth);
	void SetBorder(unsigned pBorderStyle,
				   Painter::ImageID pTopLeftID,
				   Painter::ImageID pTopRightID,
				   Painter::ImageID pBottomLeftID,
   				   Painter::ImageID pBottomRightID,
				   Painter::ImageID pTopID,
				   Painter::ImageID pBottomID,
				   Painter::ImageID pLeftID,
   				   Painter::ImageID pRightID);

	inline unsigned GetBorderStyle();
	inline int GetBorderWidth();
	inline int GetTotalBorderWidth();

	virtual bool IsOver(int pScreenX, int pScreenY);

	inline void SetBackgroundImage(Painter::ImageID pImageID);
	inline void SetColor(const Lepra::Color& pColor);

	virtual void SetCaption(Caption* pCaption);
	inline Caption* GetCaption();

	inline Painter::ImageID GetBackgroundImage();
	inline const Lepra::Color& GetColor();

	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0);
	virtual void RemoveChild(Component* pChild, int pLayer);
	virtual Component* GetChild(const Lepra::String& pName, int pLayer);
	virtual int GetNumChildren() const;

	inline virtual Component::Type GetType();

	virtual bool OnChar(Lepra::tchar pChar);
	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnRButtonDown(int pMouseX, int pMouseY);
	virtual bool OnMButtonDown(int pMouseX, int pMouseY);

protected:
	// Returns the client rect in screen coordinates.
	virtual Lepra::PixelRect GetClientRect();

	// A convenience function.
	RectComponent* GetClientRectComponent();

	virtual void SetActive(bool pActive);

	virtual void DoSetSize(int pWidth, int pHeight);

private:
	void Init();
	inline bool Check(unsigned pFlags, unsigned pFlag);

	BorderComponent* mTLBorder;
	BorderComponent* mTRBorder;
	BorderComponent* mBRBorder;
	BorderComponent* mBLBorder;
	BorderComponent* mTBorder;
	BorderComponent* mBBorder;
	BorderComponent* mLBorder;
	BorderComponent* mRBorder;

	RectComponent* mCenterComponent;
	RectComponent* mClientRect;

	Caption* mCaption;

	bool mBorder;
	int mBorderWidth;
	Lepra::Color mBodyColor;
	unsigned mBorderStyle;
};

bool Window::Check(unsigned pFlags, unsigned pFlag)
{
	return ((pFlags & pFlag) != 0);
}

Caption* Window::GetCaption()
{
	return mCaption;
}

void Window::SetBackgroundImage(Painter::ImageID pImageID)
{
	SetNeedsRepaint(pImageID != mClientRect->GetImage());
	mClientRect->SetImage(pImageID);
}

void Window::SetColor(const Lepra::Color& pColor)
{
	SetNeedsRepaint(pColor != mClientRect->GetColor());
	mClientRect->SetColor(pColor);
}

Painter::ImageID Window::GetBackgroundImage()
{
	return mClientRect->GetImage();
}

const Lepra::Color& Window::GetColor()
{
	return mBodyColor;
}

unsigned Window::GetBorderStyle()
{
	return mBorderStyle;
}

int Window::GetBorderWidth()
{
	return mBorderWidth;
}

int Window::GetTotalBorderWidth()
{
	if (Check(mBorderStyle, BORDER_HALF) == true)
		return mBorderWidth;
	else
		return mBorderWidth * 2;
}

Component::Type Window::GetType()
{
	return Component::WINDOW;
}



}
