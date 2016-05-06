
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/lepraassert.h"
#include "../include/logger.h"



namespace lepra {



void LogType::Init() {
	if (initialized_) {
		return;
	}
	initialized_ = true;

	::memset(logger_array_, 0, sizeof(logger_array_));
	logger_array_[kRoot] = new Logger("Root", 0);
	logger_array_[kGeneral] = new Logger("Data", GetLogger(kRoot));
	logger_array_[kGeneralResources] = new Logger("Resources", GetLogger(kGeneral));
	logger_array_[kNetwork] = new Logger("Network", GetLogger(kRoot));
	logger_array_[kNetworkClient] = new Logger("NetClient", GetLogger(kNetwork));
	logger_array_[kNetworkServer] = new Logger("NetServer", GetLogger(kNetwork));
	logger_array_[kConsole] = new Logger("Console", GetLogger(kRoot));
	logger_array_[kPhysics] = new Logger("Physics", GetLogger(kRoot));
	logger_array_[kUi] = new Logger("kUi", GetLogger(kRoot));
	logger_array_[kUiInput] = new Logger("Input", GetLogger(kUi));
	logger_array_[kUiSound] = new Logger("Sound", GetLogger(kUi));
	logger_array_[kUiGfx] = new Logger("Graphics", GetLogger(kUi));
	logger_array_[kUiGfx2D] = new Logger("Gfx2D", GetLogger(kUiGfx));
	logger_array_[kUiGfx3D] = new Logger("Gfx3D", GetLogger(kUiGfx));
	logger_array_[kGame] = new Logger("Game", GetLogger(kRoot));
	logger_array_[kGameContext] = new Logger("Context", GetLogger(kGame));
	logger_array_[kGameContextCpp] = new Logger("C++Obj", GetLogger(kGameContext));
	logger_array_[kTest] = new Logger("Test", GetLogger(kRoot));
}

void LogType::Close() {
	for (int x = kLowestType; x < kTypeCount; ++x) {
		delete (logger_array_[x]);
		logger_array_[x] = 0;
	}
	initialized_ = false;
}



Logger* LogType::GetLogger(Subsystem subsystem) {
	Init();

	if (subsystem >= kLowestType && subsystem < kTypeCount) {
		deb_assert(logger_array_[subsystem]);
		return (logger_array_[subsystem]);
	}
	deb_assert(false);
	return (logger_array_[0]);
}

Logger* LogType::GetLogger(const str& name) {
	Init();

	Logger* found_log = 0;
	for (int x = kLowestType; !found_log && x < kTypeCount; ++x) {
		if (logger_array_[x] && logger_array_[x]->GetName() == name) {
			found_log = logger_array_[x];
		}
	}
	return (found_log);
}

const std::vector<Logger*> LogType::GetLoggers() {
	std::vector<Logger*> log_array;
	for (int x = kLowestType; x < kTypeCount; ++x) {
		log_array.push_back(logger_array_[x]);
	}
	return log_array;
}

void LogType::SetLogLevel(LogLevel level) {
	for (int x = kLowestType; x < kTypeCount; ++x) {
		logger_array_[x]->SetLevelThreashold(level);
	}
}



bool LogType::initialized_ = false;
Logger* LogType::logger_array_[kTypeCount];



}
