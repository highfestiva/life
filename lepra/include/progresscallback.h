/*
	Class:  ProgressCallback
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine

	NOTES:

	A simple callback class with the possibility for derived classes
	to handle progress notification.
*/

#pragma once

#include "lepratypes.h"

namespace lepra {

class ProgressCallback {
public:
	ProgressCallback();
	virtual ~ProgressCallback();

	virtual void   SetProgressMax(int /*max*/){}
	virtual int  GetProgressMax(){ return 0; }
	virtual void   SetProgressPos(int /*pos*/){}
	virtual int  GetProgressPos(){ return 0; }
	virtual int  Step(){ return 0; }
	virtual int  Step(int /*steps*/) { return 0; }

	void SignalCancel();
	bool GetContinue() const;

	void SetPause(bool pause);
	bool GetPause() const;

private:
	bool do_continue_;
	bool pause_;
};

}
