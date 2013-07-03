
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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

	Window(const str& pName, Layout* pLayout = 0);
	Window(unsigned pBorderStyle, int pBorderWidth, const Color& pColor,
		const str& pName, Layout* pLayout = 0);
	Window(unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
		const str& pName, Layout* pLayout = 0);
	Window(const Color& pColor, const str& pName, Layout* pLayout = 0);
	Window(Painter::ImageID pImageID, const str& pName, Layout* pLayout = 0);
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

	unsigned GetBorderStyle();
	int GetBorderWidth();
	int GetTotalBorderWidth();

	void SetRoundedStyle(int pRadius);

	virtual bool IsOver(int pScreenX, int pScreenY);
	virtual bool OnMouseMove(int pMouseX, int pMouseY, int pDeltaX, int pDeltaY);

	void SetBackgroundImage(Painter::ImageID pImageID);
	virtual void SetBaseColor(const Color& pColor);
	void SetColor(const Color& pColor);

	virtual void SetCaption(Caption* pCaption);
	Caption* GetCaption();

	Painter::ImageID GetBackgroundImage();
	const Color& GetColor();

	virtual void AddChild(Component* pChild, int pParam1 = 0, int pParam2 = 0);
	virtual void RemoveChild(Component* pChild, int pLayer);
	virtual Component* GetChild(const str& pName, int pLayer);
	virtual int GetNumChildren() const;

	virtual Type GetType() const;

	virtual bool OnChar(tchar pChar);
	virtual bool OnLButtonDown(int pMouseX, int pMouseY);
	virtual bool OnRButtonDown(int pMouseX, int pMouseY);
	virtual bool OnMButtonDown(int pMouseX, int pMouseY);

	// Returns the client rect in screen coordinates.
	virtual PixelRect GetClientRect() const;
	RectComponent* GetClientRectComponent() const;

protected:
	virtual void SetActive(bool pActive);

	virtual void DoSetSize(int pWidth, int pHeight);

private:
	void Init();
	void InitBorder();
	bool Check(unsigned pFlags, unsigned pFlag);

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
	Color mBodyColor;
	unsigned mBorderStyle;
};



}
