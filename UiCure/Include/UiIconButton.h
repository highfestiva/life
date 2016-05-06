
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Loads icon from resource manager and shows it once loaded. Frees the icon
// when deleted.



#pragma once
#include "../../uitbc/include/gui/uibutton.h"
#include "uiresourcemanager.h"



namespace cure {
class ResourceManager;
}



namespace UiCure {



class GameUiManager;



class IconButton: public uitbc::Button {
	typedef uitbc::Button Parent;
public:
	IconButton(GameUiManager* ui_manager, cure::ResourceManager* resource_manager, const str& icon_image_name, const wstr& text);
	virtual ~IconButton();

private:
	virtual bool IsComplete() const;
	void PainterImageLoadCallback(UserPainterKeepImageResource* resource);

	UserPainterKeepImageResource* icon_resource_;
	uitbc::Painter::ImageID highlighted_icon_id_;
};



}
