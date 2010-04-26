/*
	Class:  ProgressBar
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#ifndef UIPROGRESSBAR_H
#define UIPROGRESSBAR_H

#include "UiWindow.h"
#include "../../../Lepra/Include/ProgressCallback.h"

namespace UiTbc
{

class ProgressBar: public Window, public ProgressCallback
{
	typedef Window Parent;
public:

	enum Style
	{
		STYLE_NORMAL = 0,
		STYLE_BOXES,
	};


	ProgressBar(Style pStyle = STYLE_NORMAL,
				const Color& pBodyColor = Color(192, 192, 192),
				const Color& pProgressColorLeft  = Color(0, 0, 255),
				const Color& pProgressColorRight = Color(0, 0, 255),
				const str& pName = _T("ProgressBar"));

	ProgressBar(Style pStyle,
				Painter::ImageID pBackgroundImageID,
				Painter::ImageID pProgressImageID,
				const str& pName = _T("ProgressBar"));

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

	Color mProgressColorLeft;
	Color mProgressColorRight;

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