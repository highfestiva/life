
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
	mLoggerArray[ROOT] = new Logger("Root", 0);
	mLoggerArray[GENERAL] = new Logger("Data", GetLogger(ROOT));
	mLoggerArray[GENERAL_RESOURCES] = new Logger("Resources", GetLogger(GENERAL));
	mLoggerArray[NETWORK] = new Logger("Network", GetLogger(ROOT));
	mLoggerArray[NETWORK_CLIENT] = new Logger("NetClient", GetLogger(NETWORK));
	mLoggerArray[NETWORK_SERVER] = new Logger("NetServer", GetLogger(NETWORK));
	mLoggerArray[CONSOLE] = new Logger("Console", GetLogger(ROOT));
	mLoggerArray[PHYSICS] = new Logger("Physics", GetLogger(ROOT));
	mLoggerArray[UI] = new Logger("UI", GetLogger(ROOT));
	mLoggerArray[UI_INPUT] = new Logger("Input", GetLogger(UI));
	mLoggerArray[UI_SOUND] = new Logger("Sound", GetLogger(UI));
	mLoggerArray[UI_GFX] = new Logger("Graphics", GetLogger(UI));
	mLoggerArray[UI_GFX_2D] = new Logger("Gfx2D", GetLogger(UI_GFX));
	mLoggerArray[UI_GFX_3D] = new Logger("Gfx3D", GetLogger(UI_GFX));
	mLoggerArray[GAME] = new Logger("Game", GetLogger(ROOT));
	mLoggerArray[GAME_CONTEXT] = new Logger("Context", GetLogger(GAME));
	mLoggerArray[GAME_CONTEXT_CPP] = new Logger("C++Obj", GetLogger(GAME_CONTEXT));
	mLoggerArray[TEST] = new Logger("Test", GetLogger(ROOT));
}

void LogType::Close()
{
	for (int x = LOWEST_TYPE; x < TYPE_COUNT; ++x)
	{
		delete (mLoggerArray[x]);
		mLoggerArray[x] = 0;
	}
	mInitialized = false;
}



Logger* LogType::GetLogger(Subsystem pSubsystem)
{
	Init();

	if (pSubsystem >= LOWEST_TYPE && pSubsystem < TYPE_COUNT)
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
	for (int x = LOWEST_TYPE; !lFoundLog && x < TYPE_COUNT; ++x)
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
	for (int x = LOWEST_TYPE; x < TYPE_COUNT; ++x)
	{
		lLogArray.push_back(mLoggerArray[x]);
	}
	return lLogArray;
}

void LogType::SetLogLevel(LogLevel pLevel)
{
	for (int x = LOWEST_TYPE; x < TYPE_COUNT; ++x)
	{
		mLoggerArray[x]->SetLevelThreashold(pLevel);
	}
}



bool LogType::mInitialized = false;
Logger* LogType::mLoggerArray[TYPE_COUNT];



}
