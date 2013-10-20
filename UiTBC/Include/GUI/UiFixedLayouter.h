
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



namespace UiTbc
{



class Button;
class Component;
class Dialog;
class Window;



class FixedLayouter
{
public:
	FixedLayouter(Dialog* pParent);
	virtual ~FixedLayouter();

	void SetContentWidthPart(float pContentWidthPart);
	void SetContentHeightPart(float pContentHeightPart);
	void SetContentMargin(int pContentMargin);
	void SetContentXMargin(int pContentXMargin);
	void SetContentYMargin(int pContentYMargin);

	void AddComponent(Component* pComponent, int r, int rc, int c, int cc);
	void AddWindow(Window* pWindow, int r, int rc, int c, int cc);
	void AddButton(Button* pButton, int pTag, int r, int rc, int c, int cc, bool pAutoDismiss);
	void AddCornerButton(Button* pButton, int pTag);

protected:
	Dialog* mDialog;
	float mContentWidthPart;
	float mContentHeightPart;
	int mMaxRowCount;
	int mContentXMargin;
	int mContentYMargin;
};



}
