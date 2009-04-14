
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



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
	      const Lepra::String& pName = _T("Label"));

	// Single background color.
	Label(const Lepra::Color& pColor,
	      const Lepra::String& pName = _T("Label"));

	// Interpolated background color ("gradients").
	Label(const Lepra::Color& pTopLeftColor,
	      const Lepra::Color& pTopRightColor,
	      const Lepra::Color& pBottomRightColor,
	      const Lepra::Color& pBottomLeftColor,
	      const Lepra::String& pName = _T("Label"));

	// Image as background.
	Label(Painter::ImageID pImageID,
	      const Lepra::String& pName = _T("Label"));

	//
	// The following constructors can be used with labels that can
	// be "selected" (list, tree or menu items etc). The last selected
	// label (LSL, in a list for instance) is marked in a special way.
	//

	// Single background color. LSL is marked with a dotted line.
	Label(const Lepra::Color& pColor,
	      const Lepra::Color& pSelectedColor,
	      const Lepra::String& pName = _T("Label"));

	// Interpolated background color ("gradients").
	// LSL is marked with a dotted line.
	Label(const Lepra::Color& pTopLeftColor,
	      const Lepra::Color& pTopRightColor,
	      const Lepra::Color& pBottomRightColor,
	      const Lepra::Color& pBottomLeftColor,
	      const Lepra::Color& pSelectedTopLeftColor,
	      const Lepra::Color& pSelectedTopRightColor,
	      const Lepra::Color& pSelectedBottomRightColor,
	      const Lepra::Color& pSelectedBottomLeftColor,
	      const Lepra::String& pName = _T("Label"));

	// LCL = Last Clicked Label. If these are set to INVALID_IMAGEID
	// pImageID and pSelectedImageID will be used with the default
	// dotted border.
	Label(Painter::ImageID pImageID,
	      Painter::ImageID pSelectedImageID,
	      Painter::ImageID pLCLImageID,
	      Painter::ImageID pLCLSelectedImageID,
	      const Lepra::String& pName = _T("Label"));

	virtual ~Label();

	inline void SetIcon(Painter::ImageID pIconID);
	void SetText(const Lepra::String& pText,
		     Painter::FontID pFontID,
		     const Lepra::Color& pTextColor,
		     const Lepra::Color& pBackgColor,
		     BlendFunc pBlendFunc = ALPHATEST,
		     Lepra::uint8 pAlphaTreshold = 128,
		     Painter* pPainter = 0);
	void SetText(const Lepra::String& pText,
		     Painter::FontID pFontID,
		     const Lepra::Color& pTextColor,
		     const Lepra::Color& pBackgColor,
		     const Lepra::Color& pSelectedTextColor,
		     const Lepra::Color& pSelectedBackgColor,
		     BlendFunc pBlendFunc = ALPHATEST,
		     Lepra::uint8 pAlphaTreshold = 128,
		     Painter* pPainter = 0);

	const Lepra::String& GetText() const;

	virtual void Repaint(Painter* pPainter);

	virtual Lepra::PixelCoords GetPreferredSize(bool pForceAdaptive);

	virtual void OnConnectedToDesktopWindow();

	inline virtual Component::Type GetType();

	virtual void SetSelected(bool pSelected);
	virtual void SetKeyboardFocus();
	virtual void ReleaseKeyboardFocus(Component::RecurseDir pDir = Component::RECURSE_UP, Component* pFocusedComponent = 0);

protected:
	void UpdateBackground();
	void ForceRepaint();

	Painter::ImageID mIconID;

	Lepra::String mText;
	Lepra::Color mTextBackgColor;
	Lepra::Color mSelectedTextColor;
	Lepra::Color mSelectedTextBackgColor;

	int mTextWidth;
	int mTextHeight;

	// The following is only used if the label is selectable.
	bool mSelectable;
	Lepra::Color mColor[4];
	Lepra::Color mSelectedColor[4];
	Painter::ImageID mImageID;
	Painter::ImageID mSelectedImageID;
	Painter::ImageID mLCLImageID;
	Painter::ImageID mLCLSelectedImageID;
};

void Label::SetIcon(Painter::ImageID pIconID)
{
	mIconID = pIconID;
}

Component::Type Label::GetType()
{
	return Component::LABEL;
}

}
