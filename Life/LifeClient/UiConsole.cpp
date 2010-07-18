
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "UiConsole.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTBC/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTBC/Include/GUI/UiConsolePrompt.h"
#include "../../UiTBC/Include/GUI/UiDesktopWindow.h"
#include "../../UiTBC/Include/GUI/UiFileNameField.h"
#include "../../UiTBC/Include/GUI/UiTextArea.h"
#include "../ConsoleManager.h"
#include "RtVar.h"



namespace Life
{



UiConsole::UiConsole(ConsoleManager* pManager, UiCure::GameUiManager* pUiManager, const PixelRect& pArea):
	mManager(pManager),
	mUiManager(pUiManager),
	mArea(pArea),
	mColor(10, 30, 20, 160),
	mConsoleComponent(0),
	mConsoleOutput(0),
	mConsoleInput(0),
	mIsConsoleVisible(false),
	mIsFirstConsoleUse(true),
	mConsoleTargetPosition(0)
{
	if (mManager->GetConsoleLogger())
	{
#ifdef NO_LOG_DEBUG_INFO
		const Log::LogLevel lAllowedLevel = Log::LEVEL_INFO;
#else // Allow debug logging.
		const Log::LogLevel lAllowedLevel = Log::LEVEL_LOWEST_TYPE;
#endif // Disallow/allow debug logging.
		for (int x = lAllowedLevel; x < Log::LEVEL_TYPE_COUNT; ++x)
		{
			LogType::GetLog(LogType::SUB_ROOT)->AddListener(mManager->GetConsoleLogger(), (Log::LogLevel)x);
		}
	}
}

UiConsole::~UiConsole()
{
	Close();
	mUiManager = 0;
	mManager = 0;
}



void UiConsole::Open()
{
	InitGraphics();

	((UiTbc::ConsoleLogListener*)mManager->GetConsoleLogger())->SetOutputComponent(mConsoleOutput);
	((UiTbc::ConsolePrompt*)mManager->GetConsolePrompt())->SetInputComponent(mConsoleInput);

	OnConsoleChange();
}

void UiConsole::Close()
{
	((UiTbc::ConsoleLogListener*)mManager->GetConsoleLogger())->SetOutputComponent(0);
	((UiTbc::ConsolePrompt*)mManager->GetConsolePrompt())->SetInputComponent(0);
	CloseGraphics();
}



void UiConsole::SetRenderArea(const PixelRect& pRenderArea)
{
	mArea = pRenderArea;

	if (mConsoleComponent)
	{
		PixelCoord lSize = mArea.GetSize();
		lSize.y = (int)(lSize.y*0.6);	// TODO: use setting for how high console should be.
		mConsoleComponent->SetPreferredSize(lSize);
		int lInputHeight = mUiManager->GetPainter()->GetFontHeight()+4;
		lSize.y -= lInputHeight;
		mConsoleOutput->SetPreferredSize(lSize);
		lSize.y = lInputHeight;
		mConsoleInput->SetPreferredSize(lSize);
	}
}

void UiConsole::SetColor(const Color& pColor)
{
	mColor = pColor;
}

bool UiConsole::ToggleVisible()
{
	SetVisible(!mIsConsoleVisible);
	return (mIsConsoleVisible);
}

void UiConsole::SetVisible(bool pVisible)
{
	mIsConsoleVisible = pVisible;
	OnConsoleChange();
}

void UiConsole::Tick()
{
	const float lFrameTime = mManager->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime();
	const float lConsoleSpeed = Math::GetIterateLerpTime(0.9f, lFrameTime);
	if (mIsConsoleVisible)
	{
		if (mArea.mTop == 0)	// Slide down.
		{
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)mArea.mTop, lConsoleSpeed);
			mConsoleComponent->SetPos(mArea.mLeft, (int)mConsoleTargetPosition);
		}
		else	// Slide sideways.
		{
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)mArea.mLeft, lConsoleSpeed);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
		}
	}
	else
	{
		const int lMargin = 3;
		if (mArea.mTop == 0)	// Slide out top.
		{
			const int lTarget = -mConsoleComponent->GetSize().y-lMargin;
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)lTarget, lConsoleSpeed);
			mConsoleComponent->SetPos(mArea.mLeft, (int)mConsoleTargetPosition);
			if (mConsoleComponent->GetPos().y <= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
		else if (mArea.mLeft == 0)	// Slide out left.
		{
			const int lTarget = -mConsoleComponent->GetSize().x-lMargin;
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)lTarget, lConsoleSpeed);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
			if (mConsoleComponent->GetPos().x <= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
		else	// Slide out right.
		{
			const int lTarget = mUiManager->GetDisplayManager()->GetWidth()+lMargin;
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)lTarget, lConsoleSpeed);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
			if (mConsoleComponent->GetPos().x >= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
	}
}



void UiConsole::InitGraphics()
{
	CloseGraphics();

	mConsoleComponent = new UiTbc::Component(_T("CON:"), new UiTbc::ListLayout());
	mConsoleOutput = new UiTbc::TextArea(mColor);
	Color lInputColor = mColor - mColor * 0.3f;
	lInputColor.mAlpha = mColor.mAlpha;
	mConsoleInput = new UiTbc::TextField(mConsoleComponent, lInputColor, _T("CONI:"));

	SetRenderArea(mArea);

	mConsoleOutput->SetFocusAnchor(UiTbc::TextArea::ANCHOR_BOTTOM_LINE);
	mConsoleOutput->SetFontColor(WHITE);
	mConsoleInput->SetFontColor(WHITE);

	mConsoleComponent->AddChild(mConsoleOutput);
	mConsoleComponent->AddChild(mConsoleInput);

	mUiManager->GetDesktopWindow()->AddChild(mConsoleComponent, 0, 0, 1);
	mConsoleComponent->SetPos(mArea.mLeft, mArea.mTop);
	mConsoleComponent->SetVisible(false);

	// This is just for getting some basic metrics (such as font height).
	mConsoleComponent->Repaint(mUiManager->GetPainter());
}

void UiConsole::CloseGraphics()
{
	if (mUiManager && mConsoleComponent)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mConsoleComponent, 1);
		mConsoleComponent->RemoveChild(mConsoleOutput, 0);
		mConsoleComponent->RemoveChild(mConsoleInput, 0);
		mConsoleComponent->SetVisible(false);
	}

	delete (mConsoleComponent);
	mConsoleComponent = 0;
	delete (mConsoleOutput);
	mConsoleOutput = 0;
	delete (mConsoleInput);
	mConsoleInput = 0;
}

void UiConsole::OnConsoleChange()
{
	if (mIsConsoleVisible)
	{
		mConsoleComponent->SetVisible(true);
		mUiManager->GetDesktopWindow()->UpdateLayout();
		mManager->GetConsolePrompt()->SetFocus(true);
		if (mIsFirstConsoleUse)
		{
			mIsFirstConsoleUse = false;
			PrintHelp();
		}
	}
	else
	{
		mManager->GetConsolePrompt()->SetFocus(false);
	}
}

void UiConsole::PrintHelp()
{
	str lKeys;
	CURE_RTVAR_GET(lKeys, =, mManager->GetVariableScope(), RTVAR_CTRL_UI_CONTOGGLE, _T("???"));
	typedef strutil::strvec SV;
	SV lKeyArray = strutil::Split(lKeys, _T(", \t"));
	SV lNiceKeys;
	for (SV::iterator x = lKeyArray.begin(); x != lKeyArray.end(); ++x)
	{
		const str lKey = strutil::ReplaceAll(*x, _T("Key."), _T(""));
		lNiceKeys.push_back(lKey);
	}
	str lKeyInfo;
	if (lKeyArray.size() == 1)
	{
		lKeyInfo = _T("key ");
	}
	else
	{
		lKeyInfo = _T("any of the following keys: ");
	}
	lKeyInfo += strutil::Join(lNiceKeys, _T(", "));
	mLog.Infof(_T("To bring this console up again press %s."), lKeyInfo.c_str());
}



LOG_CLASS_DEFINE(CONSOLE, UiConsole);



}
