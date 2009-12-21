
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include <assert.h>
#include "../Include/Log.h"



namespace Lepra
{



void LogType::Init()
{
	if (mInitialized)
	{
		return;
	}
	mInitialized = true;

	::memset(mLogArray, 0, sizeof(mLogArray));
	mLogArray[SUB_ROOT] = new Log(_T("Root"), 0, Log::LEVEL_INFO);
	mLogArray[SUB_GENERAL] = new Log(_T("Data"), GetLog(SUB_ROOT));
	mLogArray[SUB_GENERAL_RESOURCES] = new Log(_T("Resources"), GetLog(SUB_GENERAL));
	mLogArray[SUB_NETWORK] = new Log(_T("Network"), GetLog(SUB_ROOT));
	mLogArray[SUB_NETWORK_CLIENT] = new Log(_T("NetClient"), GetLog(SUB_NETWORK));
	mLogArray[SUB_NETWORK_SERVER] = new Log(_T("NetServer"), GetLog(SUB_NETWORK));
	mLogArray[SUB_CONSOLE] = new Log(_T("Console"), GetLog(SUB_ROOT));
	mLogArray[SUB_PHYSICS] = new Log(_T("Physics"), GetLog(SUB_ROOT));
	mLogArray[SUB_UI] = new Log(_T("UI"), GetLog(SUB_ROOT));
	mLogArray[SUB_UI_INPUT] = new Log(_T("Input"), GetLog(SUB_UI));
	mLogArray[SUB_UI_SOUND] = new Log(_T("Sound"), GetLog(SUB_UI));
	mLogArray[SUB_UI_GFX] = new Log(_T("Graphics"), GetLog(SUB_UI));
	mLogArray[SUB_UI_GFX_2D] = new Log(_T("Gfx2D"), GetLog(SUB_UI_GFX));
	mLogArray[SUB_UI_GFX_3D] = new Log(_T("Gfx3D"), GetLog(SUB_UI_GFX));
	mLogArray[SUB_GAME] = new Log(_T("Game"), GetLog(SUB_ROOT));
	mLogArray[SUB_GAME_CONTEXT] = new Log(_T("Context"), GetLog(SUB_GAME));
	mLogArray[SUB_GAME_CONTEXT_CPP] = new Log(_T("C++ Obj"), GetLog(SUB_GAME_CONTEXT));
	mLogArray[SUB_TEST] = new Log(_T("Test"), GetLog(SUB_ROOT));
}

void LogType::Close()
{
	for (int x = SUB_LOWEST_TYPE; x < SUB_TYPE_COUNT; ++x)
	{
		delete (mLogArray[x]);
		mLogArray[x] = 0;
	}
	mInitialized = false;
}



Log* LogType::GetLog(Subsystem pSubsystem)
{
	Init();

	if (pSubsystem >= SUB_LOWEST_TYPE && pSubsystem < SUB_TYPE_COUNT)
	{
		assert(mLogArray[pSubsystem]);
		return (mLogArray[pSubsystem]);
	}
	assert(false);
	return (mLogArray[0]);
}

Log* LogType::GetLog(const str& pName)
{
	Init();

	Log* lFoundLog = 0;
	for (int x = SUB_LOWEST_TYPE; !lFoundLog && x < SUB_TYPE_COUNT; ++x)
	{
		if (mLogArray[x] && mLogArray[x]->GetName() == pName)
		{
			lFoundLog = mLogArray[x];
		}
	}
	return (lFoundLog);
}



bool LogType::mInitialized = false;
Log* LogType::mLogArray[SUB_TYPE_COUNT];



}
