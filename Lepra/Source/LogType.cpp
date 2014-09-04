
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/LepraAssert.h"
#include "../Include/Logger.h"



namespace Lepra
{



void LogType::Init()
{
	if (mInitialized)
	{
		return;
	}
	mInitialized = true;

	::memset(mLoggerArray, 0, sizeof(mLoggerArray));
	mLoggerArray[SUB_ROOT] = new Logger(_T("Root"), 0);
	mLoggerArray[SUB_GENERAL] = new Logger(_T("Data"), GetLogger(SUB_ROOT));
	mLoggerArray[SUB_GENERAL_RESOURCES] = new Logger(_T("Resources"), GetLogger(SUB_GENERAL));
	mLoggerArray[SUB_NETWORK] = new Logger(_T("Network"), GetLogger(SUB_ROOT));
	mLoggerArray[SUB_NETWORK_CLIENT] = new Logger(_T("NetClient"), GetLogger(SUB_NETWORK));
	mLoggerArray[SUB_NETWORK_SERVER] = new Logger(_T("NetServer"), GetLogger(SUB_NETWORK));
	mLoggerArray[SUB_CONSOLE] = new Logger(_T("Console"), GetLogger(SUB_ROOT));
	mLoggerArray[SUB_PHYSICS] = new Logger(_T("Physics"), GetLogger(SUB_ROOT));
	mLoggerArray[SUB_UI] = new Logger(_T("UI"), GetLogger(SUB_ROOT));
	mLoggerArray[SUB_UI_INPUT] = new Logger(_T("Input"), GetLogger(SUB_UI));
	mLoggerArray[SUB_UI_SOUND] = new Logger(_T("Sound"), GetLogger(SUB_UI));
	mLoggerArray[SUB_UI_GFX] = new Logger(_T("Graphics"), GetLogger(SUB_UI));
	mLoggerArray[SUB_UI_GFX_2D] = new Logger(_T("Gfx2D"), GetLogger(SUB_UI_GFX));
	mLoggerArray[SUB_UI_GFX_3D] = new Logger(_T("Gfx3D"), GetLogger(SUB_UI_GFX));
	mLoggerArray[SUB_GAME] = new Logger(_T("Game"), GetLogger(SUB_ROOT));
	mLoggerArray[SUB_GAME_CONTEXT] = new Logger(_T("Context"), GetLogger(SUB_GAME));
	mLoggerArray[SUB_GAME_CONTEXT_CPP] = new Logger(_T("C++Obj"), GetLogger(SUB_GAME_CONTEXT));
	mLoggerArray[SUB_TEST] = new Logger(_T("Test"), GetLogger(SUB_ROOT));
}

void LogType::Close()
{
	for (int x = SUB_LOWEST_TYPE; x < SUB_TYPE_COUNT; ++x)
	{
		delete (mLoggerArray[x]);
		mLoggerArray[x] = 0;
	}
	mInitialized = false;
}



Logger* LogType::GetLogger(Subsystem pSubsystem)
{
	Init();

	if (pSubsystem >= SUB_LOWEST_TYPE && pSubsystem < SUB_TYPE_COUNT)
	{
		deb_assert(mLoggerArray[pSubsystem]);
		return (mLoggerArray[pSubsystem]);
	}
	deb_assert(false);
	return (mLoggerArray[0]);
}

Logger* LogType::GetLogger(const str& pName)
{
	Init();

	Logger* lFoundLog = 0;
	for (int x = SUB_LOWEST_TYPE; !lFoundLog && x < SUB_TYPE_COUNT; ++x)
	{
		if (mLoggerArray[x] && mLoggerArray[x]->GetName() == pName)
		{
			lFoundLog = mLoggerArray[x];
		}
	}
	return (lFoundLog);
}

const std::vector<Logger*> LogType::GetLoggers()
{
	std::vector<Logger*> lLogArray;
	for (int x = SUB_LOWEST_TYPE; x < SUB_TYPE_COUNT; ++x)
	{
		lLogArray.push_back(mLoggerArray[x]);
	}
	return lLogArray;
}

void LogType::SetLogLevel(LogLevel pLevel)
{
	for (int x = SUB_LOWEST_TYPE; x < SUB_TYPE_COUNT; ++x)
	{
		mLoggerArray[x]->SetLevelThreashold(pLevel);
	}
}



bool LogType::mInitialized = false;
Logger* LogType::mLoggerArray[SUB_TYPE_COUNT];



}
