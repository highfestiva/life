
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiRectComponent.h"
#include "UiTextComponent.h"



namespace UiTbc
{



class Label: public RectComponent, public TextComponent
{
	typedef RectComponent Parent;
public:

	//
	// The following constructors can be used with labels that are
	// static (not part of a list, tree or menu etc).
	//

	// A hollow label (transparent background).
	Label(bool pSelectable = false,
	      const str& pName = _T("Label"));

	// Single background color.
	Label(const Color& pColor,
	      const str& pName = _T("Label"));

	// Interpolated background color ("gradients").
	Label(const Color& pTopLeftColor,
	      const Color& pTopRightColor,
	      const Color& pBottomRightColor,
	      const Color& pBottomLeftColor,
	      const str& pName = _T("Label"));

	// Image as background.
	Label(Painter::ImageID pImageID,
	      const str& pName = _T("Label"));

	//
	// The following constructors can be used with labels that can
	// be "selected" (list, tree or menu items etc). The last selected
	// label (LSL, in a list for instance) is marked in a special way.
	//

	// Single background color. LSL is marked with a dotted line.
	Label(const Color& pColor,
	      const Color& pSelectedColor,
	      const str& pName = _T("Label"));

	// Interpolated background color ("gradients").
	// LSL is marked with a dotted line.
	Label(const Color& pTopLeftColor,
	      const Color& pTopRightColor,
	      const Color& pBottomRightColor,
	      const Color& pBottomLeftColor,
	      const Color& pSelectedTopLeftColor,
	      const Color& pSelectedTopRightColor,
	      const Color& pSelectedBottomRightColor,
	      const Color& pSelectedBottomLeftColor,
	      const str& pName = _T("Label"));

	// LCL = Last Clicked Label. If these are set to INVALID_IMAGEID
	// pImageID and pSelectedImageID will be used with the default
	// dotted border.
	Label(Painter::ImageID pImageID,
	      Painter::ImageID pSelectedImageID,
	      Painter::ImageID pLCLImageID,
	      Painter::ImageID pLCLSelectedImageID,
	      const str& pName = _T("Label"));

	virtual ~Label();

	void SetIcon(Painter::ImageID pIconID);
	void SetText(const str& pText,
		     const Color& pTextColor,
		     const Color& pBackgColor,
		     Painter* pPainter = 0);
	void SetText(const str& pText,
		     const Color& pTextColor,
		     const Color& pBackgColor,
		     const Color& pSelectedTextColor,
		     const Color& pSelectedBackgColor,
		     Painter* pPainter = 0);

	const str& GetText() const;

	virtual void Repaint(Painter* pPainter);

	virtual PixelCoord GetPreferredSize(bool pForceAdaptive);

	virtual void OnConnectedToDesktopWindow();

	virtual Type GetType() const;

	virtual void SetSelected(bool pSelected);
	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(Component::RecurseDir pDir = Component::RECURSE_UP, Component* pFocusedComponent = 0);

protected:
	void UpdateBackground();
	void ForceRepaint();

	Painter::ImageID mIconID;

	str mText;
	Color mTextBackgColor;
	Color mSelectedTextColor;
	Color mSelectedTextBackgColor;

	int mTextWidth;
	int mTextHeight;

	// The following is only used if the label is selectable.
	bool mSelectable;
	Color mColor[4];
	Color mSelectedColor[4];
	Painter::ImageID mImageID;
	Painter::ImageID mSelectedImageID;
	Painter::ImageID mLCLImageID;
	Painter::ImageID mLCLSelectedImageID;
};



}
