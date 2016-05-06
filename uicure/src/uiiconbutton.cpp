
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uiiconbutton.h"
#include "../include/uiresourcemanager.h"



namespace UiCure {



IconButton::IconButton(GameUiManager* ui_manager, cure::ResourceManager* resource_manager,
		const str& icon_image_name, const wstr& text):
	Parent(text),
	icon_resource_(new UserPainterKeepImageResource(ui_manager, PainterImageResource::kReleaseNone)),
	highlighted_icon_id_(uitbc::Painter::kInvalidImageid) {
	SetText(text);
	icon_resource_->Load(resource_manager, icon_image_name,
		UserPainterKeepImageResource::TypeLoadCallback(this, &IconButton::PainterImageLoadCallback));
	GetClientRectComponent()->SetIsHollow(true);
}

IconButton::~IconButton() {
	if (icon_resource_->GetLoadState() == cure::kResourceLoadComplete &&
		icon_resource_->GetConstResource()->GetReferenceCount() == 1 &&
		GetImageManager()->HasImage(icon_resource_->GetData())) {
		GetImageManager()->DropImage(icon_resource_->GetData());
	}
	if (highlighted_icon_id_ != uitbc::Painter::kInvalidImageid) {
		GetImageManager()->RemoveImage(highlighted_icon_id_);
	}
	delete icon_resource_;
	icon_resource_ = 0;
}



bool IconButton::IsComplete() const {
	return (icon_resource_->GetLoadState() != cure::kResourceLoadInProgress);
}

void IconButton::PainterImageLoadCallback(UserPainterKeepImageResource* resource) {
	deb_assert(resource->GetLoadState() == cure::kResourceLoadComplete);
	if (resource->GetLoadState() == cure::kResourceLoadComplete) {
		Canvas canvas(*resource->GetRamData(), true);
		bool has_transparent = false;
		const unsigned w = canvas.GetWidth();
		const unsigned h = canvas.GetHeight();
		const Color blend_color(0, 0, 128, 128);
		for (unsigned y = 0; y < h; ++y) {
			for (unsigned x = 0; x < w; ++x) {
				Color c = canvas.GetPixelColor(x, y);
				if (c.alpha_) {
					c = Color(c, blend_color, 0.5f);
					canvas.SetPixelColor(x, y, c);
				} else {
					has_transparent = true;
				}
			}
		}
		const uitbc::GUIImageManager::BlendFunc blend_func = has_transparent? uitbc::GUIImageManager::kAlphablend : uitbc::GUIImageManager::kNoBlend;
		if (!GetImageManager()->HasImage(resource->GetData())) {
			GetImageManager()->AddLoadedImage(*resource->GetRamData(), resource->GetData(), uitbc::GUIImageManager::kCentered, blend_func, 255);
		}
		highlighted_icon_id_ = GetImageManager()->AddImage(canvas, uitbc::GUIImageManager::kCentered, blend_func, 255);
		//canvas.SetBuffer(0);	// Free buffer.
		PixelCoord preferred_size = GetPreferredSize();
		SetIcon(resource->GetData(), kIconCenter);
		SetHighlightedIcon(highlighted_icon_id_);
		if (preferred_size.x != 0 || preferred_size.y != 0) {
			SetPreferredSize(preferred_size, false);
		}
		if (GetParent()) {
			GetParent()->UpdateLayout();
		}
	}
}



}
