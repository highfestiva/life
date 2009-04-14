/*
	Class:  ProgressBar
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#ifndef UIPROGRESSBAR_H
#define UIPROGRESSBAR_H

#include "UiWindow.h"
#include "../../../Lepra/Include/ProgressCallback.h"

namespace UiTbc
{

class ProgressBar: public Window, public Lepra::ProgressCallback
{
	typedef Window Parent;
public:

	enum Style
	{
		STYLE_NORMAL = 0,
		STYLE_BOXES,
	};


	ProgressBar(Style pStyle = STYLE_NORMAL,
				const Lepra::Color& pBodyColor = Lepra::Color(192, 192, 192),
				const Lepra::Color& pProgressColorLeft  = Lepra::Color(0, 0, 255),
				const Lepra::Color& pProgressColorRight = Lepra::Color(0, 0, 255),
				const Lepra::String& pName = _T("ProgressBar"));

	ProgressBar(Style pStyle,
				Painter::ImageID pBackgroundImageID,
				Painter::ImageID pProgressImageID,
				const Lepra::String& pName = _T("ProgressBar"));

	inline Style GetStyle();

	inline int GetBoxSize();
	inline void SetBoxSize(int pBoxSize);

	virtual void   SetProgressMax(int pMax);
	virtual int  GetProgressMax();
	virtual void   SetProgressPos(int pPos);
	virtual int  GetProgressPos();
	virtual int  Step();
	virtual int  Step(int pSteps);

	virtual void Repaint(Painter* pPainter);

protected:
private:

	Style mStyle;
	int mBoxSize;
	int mMax;
	int mPos;

	bool mUserDefinedGfx;

	Lepra::Color mProgressColorLeft;
	Lepra::Color mProgressColorRight;

	Painter::ImageID mProgressImageID;
};

ProgressBar::Style ProgressBar::GetStyle()
{
	return mStyle;
}

int ProgressBar::GetBoxSize()
{
	return mBoxSize;
}

void ProgressBar::SetBoxSize(int pBoxSize)
{
	mBoxSize = pBoxSize;
}

} // End namespace.

#endif