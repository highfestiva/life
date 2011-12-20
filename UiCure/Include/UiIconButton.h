
// Author: Jonas Byström
// Copyright (c) 2002-2011, Righteous Games

// Loads icon from resource manager and shows it once loaded. Frees the icon
// when deleted.



#pragma once
#include "../../UiTBC/Include/GUI/UiCustomButton.h"
#include "UiResourceManager.h"



namespace Cure
{
class ResourceManager;
}



namespace UiCure
{



class GameUiManager;



class IconButton: public UiTbc::CustomButton
{
	typedef UiTbc::CustomButton Parent;
public:
	IconButton(GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager, const str& pIconImageName, const str& pText);
	virtual ~IconButton();

private:
	virtual bool IsComplete() const;
	void PainterImageLoadCallback(UserPainterKeepImageResource* pResource);

	UserPainterKeepImageResource* mIconResource;
	UiTbc::Painter::ImageID mHighlightedIconId;
};



}
