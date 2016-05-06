

// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/hirestimer.h"
#include "../include/time.h"
#include <time.h>
#pragma warning(disable: 4996)	// Deprecated functions are not to be used, as they may be removed in future versions. Circumvent problem instead.



namespace lepra {



Time::Time() {
	UpdateTime();
}

Time::Time(const Time& _time) {
	year_        = _time.year_;
	month_       = _time.month_;
	day_         = _time.day_;
	week_day_     = _time.week_day_;
	hour_        = _time.hour_;
	minute_      = _time.minute_;
	second_      = _time.second_;
	millis_      = _time.millis_;
	daylight_saving_time_ = _time.daylight_saving_time_;
}

int Time::GetDaysInMonth() const {
	switch(month_) {
	case 0:		// January
	case 2:		// March
	case 4:		// May
	case 6:		// July
	case 7:		// August
	case 9:		// October
	case 11:	// December
		return 31;
	case 1:		// February
		if (IsLeapYear()) {
			return 29;
		} else {
			return 28;
		}
	case 3:		// April
	case 5:		// June
	case 8:		// September
	case 10:	// November
		return 30;
	default:
		return 0;
	}
}

void Time::UpdateTime() {
	time_t __time = time(NULL);
	tm* __tm = localtime(&__time);
	const double s = HiResTimer(false).GetTime();

	year_        = (int)__tm->tm_year + 1900;
	month_       = (int)__tm->tm_mon + 1;
	day_         = (int)__tm->tm_mday;
	week_day_     = (int)__tm->tm_wday;
	hour_        = (int)__tm->tm_hour;
	minute_      = (int)__tm->tm_min;
	second_      = (int)__tm->tm_sec;
	millis_      = int((s-int(s))*1000);

	daylight_saving_time_ = ((int)__tm->tm_isdst > 0);
}

Time& Time::operator += (const Time& _time) {
	year_        += _time.year_;
	month_       += _time.month_;
	day_         += _time.day_;
	week_day_     += _time.week_day_;
	hour_        += _time.hour_;
	minute_      += _time.minute_;
	second_      += _time.second_;
	millis_      += _time.millis_;

	second_ += millis_ / 1000;
	millis_ %= 1000;

	minute_ += second_ / 60;
	second_ %= 60;

	hour_ += minute_ / 60;
	minute_ %= 60;

	day_ += hour_ / 24;
	week_day_ = (week_day_ + hour_ / 24) % 7;
	hour_ %= 24;

	int days_in_month = GetDaysInMonth();
	while (day_ > days_in_month) {
		month_++;

		if (month_ > 12) {
			year_++;
			month_ -= 12;
		}

		day_ -= days_in_month;
		days_in_month = GetDaysInMonth();
	}

	return *this;
}

Time& Time::operator -= (const Time& _time) {
	year_        -= _time.year_;
	month_       -= _time.month_;
	day_         -= _time.day_;
	week_day_     -= _time.week_day_;
	hour_        -= _time.hour_;
	minute_      -= _time.minute_;
	second_      -= _time.second_;
	millis_      -= _time.millis_;

	if (millis_ < 0) {
		second_ += (millis_-1000-1) / 1000;
		millis_ = (1000+millis_) % 1000;
	}
	if (second_ < 0) {
		minute_ += (second_-60-1) / 60;
		second_  = (60+second_) % 60;
	}
	if (minute_ < 0) {
		hour_  += (minute_-60-1) / 60;
		minute_ = (60+minute_) % 60;
	}
	if (hour_ < 0) {
		day_ += (hour_-24-1) / 24;
		hour_ = (24+hour_) % 24;
	}

	int days_in_month = GetDaysInMonth();
	while (day_ <= 0) {
		month_--;

		if (month_ <= 0) {
			year_--;
			month_ += 12;
		}

		day_ += days_in_month;
		days_in_month = GetDaysInMonth();
	}
	return *this;
}

str Time::GetDateTimeAsString() const {
	return strutil::Format("%i-%.2i-%.2i %.2i:%.2i:%.2i,%.3i", GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond(), GetMillisecond());
}

bool Time::operator < (const Time& _time) const {
	if (year_ != _time.year_) {
		return year_ < _time.year_;
	}
	if (month_ != _time.month_) {
		return month_ < _time.month_;
	}
	if (day_ != _time.day_) {
		return day_ < _time.day_;
	}
	if (hour_ != _time.hour_) {
		return hour_ < _time.hour_;
	}
	if (minute_ != _time.minute_) {
		return minute_ < _time.minute_;
	}
	if (second_ != _time.second_) {
		return second_ < _time.second_;
	}
	if (millis_ != _time.millis_) {
		return millis_ < _time.millis_;
	}
	return false;
}

bool Time::operator >  (const Time& _time) const {
	if (year_ != _time.year_) {
		return year_ > _time.year_;
	}
	if (month_ != _time.month_) {
		return month_ > _time.month_;
	}
	if (day_ != _time.day_) {
		return day_ > _time.day_;
	}
	if (hour_ != _time.hour_) {
		return hour_ > _time.hour_;
	}
	if (minute_ != _time.minute_) {
		return minute_ > _time.minute_;
	}
	if (second_ != _time.second_) {
		return second_ > _time.second_;
	}
	if (millis_ != _time.millis_) {
		return millis_ > _time.millis_;
	}
	return false;
}

const Time& Time::operator = (const Time& _time) {
	year_        = _time.year_;
	month_       = _time.month_;
	day_         = _time.day_;
	week_day_     = _time.week_day_;
	hour_        = _time.hour_;
	minute_      = _time.minute_;
	second_      = _time.second_;
	millis_      = _time.millis_;
	daylight_saving_time_ = _time.daylight_saving_time_;
	return *this;
}


int Time::GetYear() const {
	return year_;
}

int Time::GetMonth() const {
	return month_;
}

int Time::GetDay() const {
	return day_;
}

int Time::GetWeekDay() const {
	return week_day_;
}

int Time::GetHour() const {
	return hour_;
}

int Time::GetMinute() const {
	return minute_;
}

int Time::GetSecond() const {
	return second_;
}

int Time::GetMillisecond() const {
	return millis_;
}

bool Time::IsLeapYear() const {
	return (year_ % 4 == 0 && (year_ % 100 != 0 || year_ % 400 == 0));
}

bool Time::operator == (const Time& _time) const {
	if (year_        == _time.year_   &&
	   month_       == _time.month_  &&
	   day_         == _time.day_    &&
	   hour_        == _time.hour_   &&
	   minute_      == _time.minute_ &&
	   second_      == _time.second_ &&
	   millis_      == _time.millis_) {
		return true;
	}

	return false;
}

bool Time::operator != (const Time& _time) const {
	return !((*this) == _time);
}

bool Time::operator <= (const Time& _time) const {
	if ((*this) == _time) {
		return true;
	}

	return (*this) < _time;
}

bool Time::operator >= (const Time& _time) const {
	if ((*this) == _time) {
		return true;
	}

	return (*this) > _time;
}

Time operator + (const Time& left, const Time& right) {
	Time __time(left);
	__time += right;
	return __time;
}

Time operator - (const Time& left, const Time& right) {
	Time __time(left);
	__time -= right;
	return __time;
}



}
