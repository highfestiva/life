
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../../Lepra/Include/LogListener.h"
#include "../../Include/UiTBC.h"



namespace UiTbc
{



class TextArea;



class ConsoleLogListener: public InteractiveConsoleLogListener
{
public:
	ConsoleLogListener(OutputFormat pFormat = FORMAT_PLAIN);
	~ConsoleLogListener();

	void SetOutputComponent(TextArea* pOutputComponent);

protected:
	void WriteLog(const str& pFullMessage, LogLevel pLevel);
	void StepPage(int pPageCount);
	void OnLogRawMessage(const str& pText);

	TextArea* mOutputComponent;
	bool mLineFeed;
};



}
