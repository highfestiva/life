
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../../lepra/include/loglistener.h"
#include "../../include/uitbc.h"



namespace uitbc {



class TextArea;



class ConsoleLogListener: public InteractiveConsoleLogListener {
public:
	ConsoleLogListener(OutputFormat format = kFormatPlain);
	~ConsoleLogListener();

	void SetOutputComponent(TextArea* output_component);

protected:
	void WriteLog(const str& full_message, LogLevel level);
	void StepPage(int page_count);
	void OnLogRawMessage(const str& text);

	TextArea* output_component_;
	bool line_feed_;
};



}
