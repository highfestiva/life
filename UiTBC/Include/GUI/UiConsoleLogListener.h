
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/LogListener.h"



namespace UiTbc
{



class TextArea;



class ConsoleLogListener: public Lepra::InteractiveConsoleLogListener
{
public:
	ConsoleLogListener(OutputFormat pFormat = FORMAT_PLAIN);

	void SetOutputComponent(TextArea* pOutputComponent);

protected:
	void WriteLog(const Lepra::String& pFullMessage, Lepra::Log::LogLevel pLevel);
	void StepPage(int pPageCount);
	void OnLogRawMessage(const Lepra::String& pText);

	TextArea* mOutputComponent;
	bool mLineFeed;
};



}
