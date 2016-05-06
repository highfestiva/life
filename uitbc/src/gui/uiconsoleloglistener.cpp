
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../include/gui/uiconsoleloglistener.h"
#include "../../../lepra/include/math.h"
#include "../../include/gui/uitextarea.h"



namespace uitbc {



ConsoleLogListener::ConsoleLogListener(OutputFormat format):
	InteractiveConsoleLogListener(format),
	output_component_(0),
	line_feed_(false) {
}

ConsoleLogListener::~ConsoleLogListener() {
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
}



void ConsoleLogListener::SetOutputComponent(TextArea* output_component) {
	output_component_ = output_component;
}



void ConsoleLogListener::WriteLog(const str& full_message, LogLevel level) {
	if (output_component_) {
		Color color;
		switch(level) {
			case kLevelTrace:	// Fall through.
			case kLevelDebug:		color.Set(0.2f, 0.2f, 0.7f, 1.0f);	break;
			case kLevelInfo:		color.Set(0.7f, 0.7f, 0.7f, 1.0f);	break;
			case kLevelPerformance:	color.Set(0.0f, 1.0f, 0.0f, 1.0f);	break;
			case kLevelHeadline:	color.Set(1.0f, 1.0f, 1.0f, 1.0f);	break;
			case kLevelWarning:	color.Set(1.0f, 1.0f, 0.0f, 1.0f);	break;
			case kLevelError:		color.Set(1.0f, 0.0f, 0.0f, 1.0f);	break;
			default:	// Fall through.
			case kLevelFatal:		color.Set(1.0f, 0.5f, 0.0f, 1.0f);	break;
		}
		str _text(full_message);
		if (line_feed_) {
			_text = "\n"+_text;
			line_feed_ = false;
		}
		if (_text.length() > 0 && _text[_text.length()-1] == '\n') {
			_text.resize(_text.length()-1);
			line_feed_ = true;
		}
		output_component_->AddText(wstrutil::Encode(_text), &color);
	}
}

void ConsoleLogListener::StepPage(int page_count) {
	int first_visible_line = output_component_->GetFirstVisibleLineIndex();
	const int visible_line_count = output_component_->GetVisibleLineCount();
	const int line_count = output_component_->GetLineCount();
	const int step_sign = Math::Clamp(page_count, -1, 1);
	first_visible_line = Math::Clamp(first_visible_line+visible_line_count*page_count-step_sign, 0, line_count);
	output_component_->SetFirstVisibleLineIndex(first_visible_line);
}

void ConsoleLogListener::OnLogRawMessage(const str& text) {
	WriteLog(text, kLevelInfo);
}



}
