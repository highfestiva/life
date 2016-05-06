
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "lepratypes.h"
#include "string.h"



namespace lepra {



class Time {
public:

	Time();
	Time(const Time& time);

	int GetYear() const;	// Returns the current year.
	int GetMonth() const;	// The current month [0,11].
	int GetDay() const;	// The current day in month [0,30].
	int GetWeekDay() const;	// The current day after last sunday [0, 6].
	int GetHour() const;	// Hour since midnight [0, 23].
	int GetMinute() const;	// Minute since hour [0, 59].
	int GetSecond() const;	// Second since minute [0, 59].
	int GetMillisecond() const;

	int GetDaysInMonth() const;

	bool IsLeapYear() const;

	// Updates to the current time.
	void UpdateTime();

	// Operators.
	const Time& operator = (const Time& time);

	bool operator <  (const Time& time) const;
	bool operator >  (const Time& time) const;
	bool operator == (const Time& time) const;
	bool operator != (const Time& time) const;
	bool operator <= (const Time& time) const;
	bool operator >= (const Time& time) const;

	friend Time operator + (const Time& left, const Time& right);
	friend Time operator - (const Time& left, const Time& right);
	Time& operator += (const Time& time);
	Time& operator -= (const Time& time);

	str GetDateTimeAsString() const;

private:
	int year_;
	int month_;
	int day_;
	int week_day_;
	int hour_;
	int minute_;
	int second_;
	int millis_;

	// This value is only valid as long as no - or + operators are used.
	bool daylight_saving_time_;
};



}
