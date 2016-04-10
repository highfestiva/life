
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// Loads icon from resource manager and shows it once loaded. Frees the icon
// when deleted.



#pragma once
#include "../../UiTbc/Include/GUI/UiButton.h"
#include "UiResourceManager.h"



namespace Cure
{
class ResourceManager;
}



namespace UiCure
{



class GameUiManager;



class IconButton: public UiTbc::Button
{
	typedef UiTbc::Button Parent;
public:
	IconButton(GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, const str& pIconImageName, const wstr& pText);
	virtual ~IconButton();

private:
	virtual bool IsComplete() const;
	void PainterImageLoadCallback(UserPainterKeepImageResource* pResource);

	UserPainterKeepImageResource* mIconResource;
	UiTbc::Painter::ImageID mHighlightedIconId;
};



}
