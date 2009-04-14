/*
	Class:  Time
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef LEPRA_TIME_H
#define LEPRA_TIME_H

#include "LepraTypes.h"

namespace Lepra
{

class Time
{
public:

	Time();
	Time(const Time& pTime);

	inline int GetYear();	        // Returns the current year.
	inline int GetMonth();          // The current month [0,11].
	inline int GetDay();	        // The current day in month [0,30].
	inline int GetWeekDay();        // The current day after last sunday [0, 6].
	inline int GetHour();           // Hour since midnight [0, 23].
	inline int GetMinute();         // Minute since hour [0, 59].
	inline int GetSecond();         // Second since minute [0, 59].

	int GetDaysInMonth();

	inline bool IsLeapYear();

	// Updates to the current time.
	void UpdateTime();

	// Operators.
	const Time& operator = (const Time& pTime);

	bool operator <  (const Time& pTime) const;
	bool operator >  (const Time& pTime) const;
	inline bool operator == (const Time& pTime) const;
	inline bool operator != (const Time& pTime) const;
	inline bool operator <= (const Time& pTime) const;
	inline bool operator >= (const Time& pTime) const;

	inline friend Time operator + (const Time& pLeft, const Time& pRight);
	inline friend Time operator - (const Time& pLeft, const Time& pRight);
	Time& operator += (const Time& pTime);
	Time& operator -= (const Time& pTime);

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

int Time::GetYear()
{
	return mYear;
}

int Time::GetMonth()
{
	return mMonth;
}

int Time::GetDay()
{
	return mDay;
}

int Time::GetWeekDay()
{
	return mWeekDay;
}

int Time::GetHour()
{
	return mHour;
}

int Time::GetMinute()
{
	return mMinute;
}

int Time::GetSecond()
{
	return mSecond;
}

bool Time::IsLeapYear()
{
	return (mYear % 4 == 0 && (mYear % 100 != 0 || mYear % 400 == 0));
}

bool Time::operator == (const Time& pTime) const
{
	if (mYear        == pTime.mYear   &&
	   mMonth       == pTime.mMonth  &&
	   mDay         == pTime.mDay    &&
	   mHour        == pTime.mHour   &&
	   mMinute      == pTime.mMinute &&
	   mSecond      == pTime.mSecond)
	{
		return true;
	}

	return false;
}

bool Time::operator != (const Time& pTime) const
{
	return !((*this) == pTime);
}

bool Time::operator <= (const Time& pTime) const
{
	if ((*this) == pTime)
	{
		return true;
	}

	return (*this) < pTime;
}

bool Time::operator >= (const Time& pTime) const
{
	if ((*this) == pTime)
	{
		return true;
	}

	return (*this) > pTime;
}

Time operator + (const Time& pLeft, const Time& pRight)
{
	Time lTime(pLeft);
	lTime += pRight;
	return lTime;
}

Time operator - (const Time& pLeft, const Time& pRight)
{
	Time lTime(pLeft);
	lTime -= pRight;
	return lTime;
}

} // End namespace.

#endif // LEPRA_TIME_H
