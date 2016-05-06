
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "hirestimer.h"



namespace lepra {



class GameTimer {
public:
	inline GameTimer();

	inline double PopTimeDiff();
	// May only be used once per frame, or couting will be off!
	inline double QueryTimeDiff();
	inline double GetTimeDiff() const;
	inline void ReduceTimeDiff(double seconds);

	inline bool TryStart();
	inline void Start(double offset = 0);
	inline bool ResumeFromLapTime();
	inline bool ResumeFromStop();
	inline void Stop();
	inline bool IsStarted() const;
	// May only be used once per frame, or couting will be off!
	inline double QuerySplitTime();
	inline int GetStartCount() const;

	inline static double GetRealTimeRatio();
	inline static void SetRealTimeRatio(double real_time_ratio);

protected:
	StopWatch timer_;
	double game_time_;
	static double real_time_ratio_;
};

GameTimer::GameTimer():
	game_time_(0) {
}

double GameTimer::PopTimeDiff() {
	double game_time = game_time_ + timer_.PopTimeDiff()*real_time_ratio_;
	game_time_ = 0;
	return game_time;
}

double GameTimer::QueryTimeDiff() {
	game_time_ += timer_.PopTimeDiff()*real_time_ratio_;
	return game_time_;
}

double GameTimer::GetTimeDiff() const {
	return game_time_;
}

void GameTimer::ReduceTimeDiff(double seconds) {
	game_time_ -= seconds;
}

bool GameTimer::TryStart() {
	if (timer_.TryStart()) {
		game_time_ = 0;
		return true;
	}
	return false;
}

void GameTimer::Start(double offset) {
	timer_.Start();
	game_time_ = offset;
}

bool GameTimer::ResumeFromLapTime() {
	return timer_.ResumeFromLapTime();
}

bool GameTimer::ResumeFromStop() {
	return timer_.ResumeFromStop();
}

void GameTimer::Stop() {
	timer_.Stop();
}

bool GameTimer::IsStarted() const {
	return timer_.IsStarted();
}

double GameTimer::QuerySplitTime() {
	return IsStarted()? QueryTimeDiff() : game_time_;
}

int GameTimer::GetStartCount() const {
	return timer_.GetStartCount();
}

double GameTimer::GetRealTimeRatio() {
	return real_time_ratio_;
}

void GameTimer::SetRealTimeRatio(double real_time_ratio) {
	real_time_ratio_ = real_time_ratio;
}



}
