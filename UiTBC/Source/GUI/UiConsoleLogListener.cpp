
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#include "../../../Lepra/Include/Math.h"
#include "../../Include/GUI/UiConsoleLogListener.h"
#include "../../Include/GUI/UiTextArea.h"



namespace UiTbc
{



ConsoleLogListener::ConsoleLogListener(OutputFormat pFormat):
	InteractiveConsoleLogListener(pFormat),
	mOutputComponent(0),
	mLineFeed(false)
{
}



void ConsoleLogListener::SetOutputComponent(TextArea* pOutputComponent)
{
	mOutputComponent = pOutputComponent;
}



void ConsoleLogListener::WriteLog(const Lepra::String& pFullMessage, Lepra::Log::LogLevel pLevel)
{
	if (mOutputComponent)
	{
		Lepra::Color lColor;
		switch(pLevel)
		{
			case Lepra::Log::LEVEL_TRACE:	// Fall through.
			case Lepra::Log::LEVEL_DEBUG:	// Fall through.
			case Lepra::Log::LEVEL_INFO:		lColor.Set(0.7, 0.7, 0.7, 1.0);	break;
			case Lepra::Log::LEVEL_PERFORMANCE:	lColor.Set(0.0, 1.0, 0.0, 1.0);	break;
			case Lepra::Log::LEVEL_HEADLINE:	lColor.Set(1.0, 1.0, 1.0, 1.0);	break;
			case Lepra::Log::LEVEL_WARNING:		lColor.Set(1.0, 1.0, 0.0, 1.0);	break;
			case Lepra::Log::LEVEL_ERROR:		lColor.Set(1.0, 0.0, 0.0, 1.0);	break;
			default:	// Fall through.
			case Lepra::Log::LEVEL_FATAL:		lColor.Set(1.0, 0.5, 0.0, 1.0);	break;
		}
		Lepra::String lText(pFullMessage);
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
	const int lStepSign = Lepra::Math::Clamp(pPageCount, -1, 1);
	lFirstVisibleLine = Lepra::Math::Clamp(lFirstVisibleLine+lVisibleLineCount*pPageCount-lStepSign, 0, lLineCount);
	mOutputComponent->SetFirstVisibleLineIndex(lFirstVisibleLine);
}

void ConsoleLogListener::OnLogRawMessage(const Lepra::String& pText)
{
	WriteLog(pText, Lepra::Log::LEVEL_INFO);
}



}
