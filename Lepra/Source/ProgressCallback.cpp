/*
	Class:  ProgressCallback
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/progresscallback.h"

namespace lepra {

ProgressCallback::ProgressCallback():
	do_continue_(true),
	pause_(false) {
}



ProgressCallback::~ProgressCallback() {
}



void ProgressCallback::SignalCancel() {
	do_continue_ = false;
}



bool ProgressCallback::GetContinue() const {
	return do_continue_;
}



void ProgressCallback::SetPause(bool pause) {
	pause_ = pause;
}



bool ProgressCallback::GetPause() const {
	return pause_;
}

}
