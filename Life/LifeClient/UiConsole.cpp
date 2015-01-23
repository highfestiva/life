
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "UiConsole.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../UiCure/Include/UiGameUiManager.h"
#include "../../UiTbc/Include/GUI/UiConsoleLogListener.h"
#include "../../UiTbc/Include/GUI/UiConsolePrompt.h"
#include "../../UiTbc/Include/GUI/UiDesktopWindow.h"
#include "../../UiTbc/Include/GUI/UiFileNameField.h"
#include "../../UiTbc/Include/GUI/UiFloatingLayout.h"
#include "../../UiTbc/Include/GUI/UiTextArea.h"
#include "../ConsoleManager.h"
#include "RtVar.h"



namespace Life
{



UiConsole::UiConsole(ConsoleManager* pManager, UiCure::GameUiManager* pUiManager, const PixelRect& pArea):
	mManager(pManager),
	mUiManager(pUiManager),
	mArea(pArea),
	mColor(10, 20, 30, 230),
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
		const LogLevel lAllowedLevel = LEVEL_INFO;
#else // Allow debug logging.
		const LogLevel lAllowedLevel = LEVEL_LOWEST_TYPE;
#endif // Disallow/allow debug logging.
		for (int x = lAllowedLevel; x < LEVEL_TYPE_COUNT; ++x)
		{
			LogType::GetLogger(LogType::SUB_ROOT)->AddListener(mManager->GetConsoleLogger(), (LogLevel)x);
		}
	}

	UiTbc::FontManager::FontId lFontId = mUiManager->GetFontManager()->GetActiveFontId();
	mFontId = mUiManager->GetFontManager()->QueryAddFont(_T("Courier New"), 14.0f);
	mUiManager->GetFontManager()->SetActiveFont(lFontId);
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
	if (mManager->GetConsoleLogger())
	{
		((UiTbc::ConsoleLogListener*)mManager->GetConsoleLogger())->SetOutputComponent(0);
	}
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
		mConsoleInput->ActivateFont(mUiManager->GetPainter());
		int lInputHeight = mUiManager->GetPainter()->GetFontHeight()+4;
		mConsoleInput->DeactivateFont(mUiManager->GetPainter());
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
	if (!mConsoleComponent)
	{
		return;
	}

	const float lFrameTime = mManager->GetGameManager()->GetTimeManager()->GetRealNormalFrameTime();
	float lConsoleSpeed;
	v_get(lConsoleSpeed, =(float), mManager->GetVariableScope(), RTVAR_CTRL_UI_CONSPEED, 2.7);
	const float lFrameConsoleSpeed = std::min(1.0f, Math::GetIterateLerpTime(lConsoleSpeed, lFrameTime));
	if (mIsConsoleVisible)
	{
		if (mArea.mTop == 0)	// Slide down.
		{
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)mArea.mTop, lFrameConsoleSpeed);
			mConsoleComponent->SetPos(mArea.mLeft, (int)mConsoleTargetPosition);
		}
		else	// Slide sideways.
		{
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)mArea.mLeft, lFrameConsoleSpeed);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
		}
	}
	else
	{
		const int lMargin = 3;
		if (mArea.mTop == 0)	// Slide out top.
		{
			const int lTarget = -mConsoleComponent->GetSize().y-lMargin;
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)lTarget, lFrameConsoleSpeed);
			mConsoleComponent->SetPos(mArea.mLeft, (int)mConsoleTargetPosition);
			if (mConsoleComponent->GetPos().y <= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
		else if (mArea.mLeft == 0)	// Slide out left.
		{
			const int lTarget = -mConsoleComponent->GetSize().x-lMargin;
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)lTarget, lFrameConsoleSpeed);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
			if (mConsoleComponent->GetPos().x <= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
		else	// Slide out right.
		{
			const int lTarget = mUiManager->GetDisplayManager()->GetWidth()+lMargin;
			mConsoleTargetPosition = Math::Lerp(mConsoleTargetPosition, (float)lTarget, lFrameConsoleSpeed);
			mConsoleComponent->SetPos((int)mConsoleTargetPosition, mArea.mTop);
			if (mConsoleComponent->GetPos().x >= lTarget+lMargin)
			{
				mConsoleComponent->SetVisible(false);
			}
		}
	}
}

UiCure::GameUiManager* UiConsole::GetUiManager() const
{
	return mUiManager;
}

UiTbc::FontManager::FontId UiConsole::GetFontId() const
{
	return mFontId;
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

	mConsoleOutput->SetHorizontalMargin(3);
	mConsoleOutput->SetFocusAnchor(UiTbc::TextArea::ANCHOR_BOTTOM_LINE);
	mConsoleOutput->SetFontId(mFontId);
	mConsoleOutput->SetFontColor(WHITE);
	mConsoleInput->SetFontId(mFontId);
	mConsoleInput->SetFontColor(WHITE);

	mConsoleComponent->AddChild(mConsoleOutput);
	mConsoleComponent->AddChild(mConsoleInput);

	mUiManager->AssertDesktopLayout(new UiTbc::FloatingLayout, 0);
	mUiManager->GetDesktopWindow()->AddChild(mConsoleComponent, 0, 0, 0);
	mConsoleComponent->SetPos(mArea.mLeft, mArea.mTop);
	mConsoleComponent->SetVisible(false);

	// This is just for getting some basic metrics (such as font height).
	mConsoleComponent->Repaint(mUiManager->GetPainter());
}

void UiConsole::CloseGraphics()
{
	if (mUiManager && mConsoleComponent)
	{
		mUiManager->GetDesktopWindow()->RemoveChild(mConsoleComponent, 0);
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
	if (!mConsoleComponent)
	{
		return;
	}

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
	v_get(lKeys, =, mManager->GetVariableScope(), RTVAR_CTRL_UI_CONTOGGLE, _T("???"));
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



loginstance(CONSOLE, UiConsole);



}
