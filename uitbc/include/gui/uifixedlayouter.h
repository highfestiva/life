
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



namespace uitbc {



class Button;
class Component;
class Dialog;
class Window;



class FixedLayouter {
public:
	FixedLayouter(Dialog* parent);
	virtual ~FixedLayouter();

	void SetContentWidthPart(float content_width_part);
	void SetContentHeightPart(float content_height_part);
	void SetContentMargin(int content_margin);
	int GetContentXMargin() const;
	void SetContentXMargin(int content_x_margin);
	int GetContentYMargin() const;
	void SetContentYMargin(int content_y_margin);

	void AddComponent(Component* component, int r, int rc, int c, int cw, int cc);
	void AddWindow(Window* window, int r, int rc, int c, int cw, int cc);
	void AddButton(Button* button, int tag, int r, int rc, int c, int cw, int cc, bool auto_dismiss);
	void AddCornerButton(Button* button, int tag);

protected:
	Dialog* dialog_;
	float content_width_part_;
	float content_height_part_;
	int max_row_count_;
	int content_x_margin_;
	int content_y_margin_;
};



}
