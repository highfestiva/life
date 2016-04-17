/*
	Class:  ProgressCallback
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A simple callback class with the possibility for derived classes
	to handle progress notification.
*/

#pragma once

#include "LepraTypes.h"

namespace Lepra
{

class ProgressCallback
{
public:
	ProgressCallback();
	virtual ~ProgressCallback();

	virtual void   SetProgressMax(int /*pMax*/){}
	virtual int  GetProgressMax(){ return 0; }
	virtual void   SetProgressPos(int /*pPos*/){}
	virtual int  GetProgressPos(){ return 0; }
	virtual int  Step(){ return 0; }
	virtual int  Step(int /*pSteps*/) { return 0; }

	void SignalCancel();
	bool GetContinue() const;

	void SetPause(bool pPause);
	bool GetPause() const;

private:
	bool mContinue;
	bool mPause;
};

}
