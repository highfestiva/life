
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "stopwatch.h"
#include "../../cure/include/contextmanager.h"
#include "../../uicure/include/uigameuimanager.h"
#include "../../uitbc/include/gui/uidesktopwindow.h"
#include "../../uitbc/include/gui/uifloatinglayout.h"
#include "../racescore.h"
#include "screenpart.h"



namespace life {



StopWatch::StopWatch(ScreenPart* screen_part, UiCure::GameUiManager* ui_manager, const str& class_resource_name,
	const str& attribute_name):
	Parent(0, class_resource_name, ui_manager),
	screen_part_(screen_part),
	attribute_name_(attribute_name),
	label_(RED, L"") {
	label_.SetPreferredSize(50, 20);
	label_.SetMinSize(30, 15);
	label_.SetPos(0, 0);
	GetUiManager()->AssertDesktopLayout(new uitbc::FloatingLayout, 0);
	GetUiManager()->GetDesktopWindow()->AddChild(&label_, 0, 0, 0);
}

StopWatch::~StopWatch() {
	GetUiManager()->GetDesktopWindow()->RemoveChild(&label_, 0);
}

void StopWatch::Start(cure::ContextObject* parent) {
	parent->GetManager()->AddLocalObject(this);
	parent->AddChild(this);
	GetManager()->EnableTickCallback(this);
}



void StopWatch::OnTick() {
	if (!GetManager()) {
		return;
	}

	const RaceScore* score = (RaceScore*)parent_->GetAttribute(attribute_name_);
	if (!score) {
		GetManager()->PostKillObject(GetInstanceId());
		return;
	}
	PixelRect area = screen_part_->GetRenderArea();
	label_.SetPos(area.right_-65, area.top_+16);
	const double time = score->GetTime();
	const int minute = (int)(time / 60);
	const int second = (int)::fmod(time, 60);
	const int centi_second = (int)(100 * (time - minute*60 - second));
	label_.SetText(wstrutil::Format(L"%i:%2.2i.%2.2i", minute, second, centi_second));
}



loginstance(kGameContextCpp, StopWatch);



}
