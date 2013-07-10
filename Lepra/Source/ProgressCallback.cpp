/*
	Class:  ProgressCallback
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "../Include/ProgressCallback.h"

namespace Lepra
{

ProgressCallback::ProgressCallback():
	mContinue(true),
	mPause(false)
{
}



ProgressCallback::~ProgressCallback()
{
}

	

void ProgressCallback::SignalCancel()
{
	mContinue = false;
}



bool ProgressCallback::GetContinue() const
{
	return mContinue;
}



void ProgressCallback::SetPause(bool pPause)
{
	mPause = pPause;
}



bool ProgressCallback::GetPause() const
{
	return mPause;
}

} // End namespace.
