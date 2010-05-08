
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "LepraTypes.h"
#include "String.h"



namespace Lepra
{



class Time
{
public:

	Time();
	Time(const Time& pTime);

	int GetYear() const;	// Returns the current year.
	int GetMonth() const;	// The current month [0,11].
	int GetDay() const;	// The current day in month [0,30].
	int GetWeekDay() const;	// The current day after last sunday [0, 6].
	int GetHour() const;	// Hour since midnight [0, 23].
	int GetMinute() const;	// Minute since hour [0, 59].
	int GetSecond() const;	// Second since minute [0, 59].

	int GetDaysInMonth() const;

	bool IsLeapYear() const;

	// Updates to the current time.
	void UpdateTime();

	// Operators.
	const Time& operator = (const Time& pTime);

	bool operator <  (const Time& pTime) const;
	bool operator >  (const Time& pTime) const;
	bool operator == (const Time& pTime) const;
	bool operator != (const Time& pTime) const;
	bool operator <= (const Time& pTime) const;
	bool operator >= (const Time& pTime) const;

	friend Time operator + (const Time& pLeft, const Time& pRight);
	friend Time operator - (const Time& pLeft, const Time& pRight);
	Time& operator += (const Time& pTime);
	Time& operator -= (const Time& pTime);

	str GetDateTimeAsString() const;

private:
	int mYear;
	int mMonth;
	int mDay;
	int mWeekDay;
	int mHour;
	int mMinute;
	int mSecond;

	// This value is only valid as long as no - or + operators are used. 
	bool mDaylightSavingTime;
};



}
