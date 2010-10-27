
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../../Include/GUI/UiConsoleLogListener.h"
#include "../../../Lepra/Include/Math.h"
#include "../../Include/GUI/UiTextArea.h"



namespace UiTbc
{



ConsoleLogListener::ConsoleLogListener(OutputFormat pFormat):
	InteractiveConsoleLogListener(pFormat),
	mOutputComponent(0),
	mLineFeed(false)
{
}

ConsoleLogListener::~ConsoleLogListener()
{
	KillSelf();	// TRICKY: has to be done in own destructor to avoid purecalls.
}



void ConsoleLogListener::SetOutputComponent(TextArea* pOutputComponent)
{
	mOutputComponent = pOutputComponent;
}



void ConsoleLogListener::WriteLog(const str& pFullMessage, Log::LogLevel pLevel)
{
	if (mOutputComponent)
	{
		Color lColor;
		switch(pLevel)
		{
			case Log::LEVEL_TRACE:	// Fall through.
			case Log::LEVEL_DEBUG:		lColor.Set(0.2f, 0.2f, 0.7f, 1.0f);	break;
			case Log::LEVEL_INFO:		lColor.Set(0.7f, 0.7f, 0.7f, 1.0f);	break;
			case Log::LEVEL_PERFORMANCE:	lColor.Set(0.0f, 1.0f, 0.0f, 1.0f);	break;
			case Log::LEVEL_HEADLINE:	lColor.Set(1.0f, 1.0f, 1.0f, 1.0f);	break;
			case Log::LEVEL_WARNING:		lColor.Set(1.0f, 1.0f, 0.0f, 1.0f);	break;
			case Log::LEVEL_ERROR:		lColor.Set(1.0f, 0.0f, 0.0f, 1.0f);	break;
			default:	// Fall through.
			case Log::LEVEL_FATAL:		lColor.Set(1.0f, 0.5f, 0.0f, 1.0f);	break;
		}
		str lText(pFullMessage);
		if (mLineFeed)
		{
			lText = _T("\n")+lText;
			mLineFeed = false;
		}
		if (lText.length() > 0 && lText[lText.length()-1] == _T('\n'))
		{
			lText.resize(lText.length()-1);
			mLineFeed = true;
		}
		mOutputComponent->AddText(lText, &lColor);
	}
}

void ConsoleLogListener::StepPage(int pPageCount)
{
	int lFirstVisibleLine = mOutputComponent->GetFirstVisibleLineIndex();
	const int lVisibleLineCount = mOutputComponent->GetVisibleLineCount();
	const int lLineCount = mOutputComponent->GetLineCount();
	const int lStepSign = Math::Clamp(pPageCount, -1, 1);
	lFirstVisibleLine = Math::Clamp(lFirstVisibleLine+lVisibleLineCount*pPageCount-lStepSign, 0, lLineCount);
	mOutputComponent->SetFirstVisibleLineIndex(lFirstVisibleLine);
}

void ConsoleLogListener::OnLogRawMessage(const str& pText)
{
	WriteLog(pText, Log::LEVEL_INFO);
}



}
