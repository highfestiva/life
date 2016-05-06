
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uirectcomponent.h"
#include "uibordercomponent.h"
#include "uigridlayout.h"



namespace uitbc {



class Caption;



class Window: public Component {
	typedef Component Parent;
public:

	friend class FloatingLayout;

	enum {
		kBorderResizable     = (1 << 0),
		kBorderSunken        = (1 << 1),
		kBorderLinearshading = (1 << 2),
		kBorderHalf          = (1 << 3),
	};

	Window(Layout* layout = 0);
	Window(unsigned border_style, int border_width, const Color& color, Layout* layout = 0);
	Window(unsigned border_style, int border_width, Painter::ImageID image_id, Layout* layout = 0);
	Window(const Color& color, Layout* layout = 0);
	Window(Painter::ImageID image_id, Layout* layout = 0);
	virtual ~Window();

	void Init();
	void InitBorder();

	void SetBorder(unsigned border_style, int width);
	void SetBorder(unsigned border_style,
				   Painter::ImageID top_left_id,
				   Painter::ImageID top_right_id,
				   Painter::ImageID bottom_left_id,
   				   Painter::ImageID bottom_right_id,
				   Painter::ImageID top_id,
				   Painter::ImageID bottom_id,
				   Painter::ImageID left_id,
   				   Painter::ImageID right_id);

	unsigned GetBorderStyle();
	int GetBorderWidth();
	int GetTotalBorderWidth();

	int GetRoundedRadius() const;
	void SetRoundedRadius(int radius);
	void SetRoundedRadiusMask(int mask);

	virtual bool IsOver(int screen_x, int screen_y);
	virtual bool OnMouseMove(int mouse_x, int mouse_y, int delta_x, int delta_y);

	void SetBackgroundImage(Painter::ImageID image_id);
	virtual void SetBaseColor(const Color& color);
	void SetColor(const Color& color);

	virtual void SetCaption(Caption* caption);
	Caption* GetCaption();

	Painter::ImageID GetBackgroundImage();
	const Color& GetColor();

	virtual void AddChild(Component* child, int param1 = 0, int param2 = 0, int layer = 0);
	virtual void RemoveChild(Component* child, int layer);
	virtual int GetNumChildren() const;

	virtual Type GetType() const;

	virtual bool OnChar(wchar_t c);
	virtual bool OnLButtonDown(int mouse_x, int mouse_y);
	virtual bool OnRButtonDown(int mouse_x, int mouse_y);
	virtual bool OnMButtonDown(int mouse_x, int mouse_y);

	// Returns the client rect in screen coordinates.
	virtual PixelRect GetClientRect() const;
	RectComponent* GetClientRectComponent() const;

	virtual void SetActive(bool active);

	virtual void DoSetSize(int width, int height);

	bool Check(unsigned flags, unsigned pFlag);

private:
	BorderComponent* tl_border_;
	BorderComponent* tr_border_;
	BorderComponent* br_border_;
	BorderComponent* bl_border_;
	BorderComponent* t_border_;
	BorderComponent* b_border_;
	BorderComponent* l_border_;
	BorderComponent* r_border_;

	RectComponent* center_component_;
	RectComponent* client_rect_;

	Caption* caption_;

	bool border_;
	int border_width_;
	Color body_color_;
	unsigned border_style_;
};



}
