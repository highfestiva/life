
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiIconButton.h"
#include "../Include/UiResourceManager.h"



namespace UiCure
{



IconButton::IconButton(GameUiManager* pUiManager, Cure::ResourceManager* pResourceManager,
		const str& pIconImageName, const str& pText):
	Parent(pText),
	mIconResource(new UserPainterKeepImageResource(pUiManager, PainterImageResource::RELEASE_NONE)),
	mHighlightedIconId(UiTbc::Painter::INVALID_IMAGEID)
{
	SetText(pText);
	mIconResource->Load(pResourceManager, pIconImageName,
		UserPainterKeepImageResource::TypeLoadCallback(this, &IconButton::PainterImageLoadCallback));
	GetClientRectComponent()->SetIsHollow(true);
}

IconButton::~IconButton()
{
	if (mIconResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE &&
		mIconResource->GetConstResource()->GetReferenceCount() == 1 &&
		GetImageManager()->HasImage(mIconResource->GetData()))
	{
		GetImageManager()->DropImage(mIconResource->GetData());
	}
	if (mHighlightedIconId != UiTbc::Painter::INVALID_IMAGEID)
	{
		GetImageManager()->RemoveImage(mHighlightedIconId);
	}
	delete mIconResource;
	mIconResource = 0;
}



bool IconButton::IsComplete() const
{
	return (mIconResource->GetLoadState() != Cure::RESOURCE_LOAD_IN_PROGRESS);
}

void IconButton::PainterImageLoadCallback(UserPainterKeepImageResource* pResource)
{
	assert(pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
	if (pResource->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
	{
		Canvas lCanvas(*pResource->GetRamData(), true);
		bool lHasTransparent = false;
		const unsigned w = lCanvas.GetWidth();
		const unsigned h = lCanvas.GetHeight();
		const Color lBlendColor(0, 0, 128, 128);
		for (unsigned y = 0; y < h; ++y)
		{
			for (unsigned x = 0; x < w; ++x)
			{
				Color c = lCanvas.GetPixelColor(x, y);
				if (c.mAlpha)
				{
					c = Color(c, lBlendColor, 0.5f);
					lCanvas.SetPixelColor(x, y, c);
				}
				else
				{
					lHasTransparent = true;
				}
			}
		}
		const UiTbc::GUIImageManager::BlendFunc lBlendFunc = lHasTransparent? UiTbc::GUIImageManager::ALPHABLEND : UiTbc::GUIImageManager::NO_BLEND;
		if (!GetImageManager()->HasImage(pResource->GetData()))
		{
			GetImageManager()->AddLoadedImage(*pResource->GetRamData(), pResource->GetData(), UiTbc::GUIImageManager::CENTERED, lBlendFunc, 255);
		}
		mHighlightedIconId = GetImageManager()->AddImage(lCanvas, UiTbc::GUIImageManager::CENTERED, lBlendFunc, 255);
		//lCanvas.SetBuffer(0);	// Free buffer.
		PixelCoord lPreferredSize = GetPreferredSize();
		SetIcon(pResource->GetData(), ICON_CENTER);
		SetHighlightedIcon(mHighlightedIconId);
		if (lPreferredSize.x != 0 || lPreferredSize.y != 0)
		{
			SetPreferredSize(lPreferredSize, false);
		}
		if (GetParent())
		{
			GetParent()->UpdateLayout();
		}
	}
}



}
