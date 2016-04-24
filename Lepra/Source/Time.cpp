

// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/HiResTimer.h"
#include "../Include/Time.h"
#include <time.h>
#pragma warning(disable: 4996)	// Deprecated functions are not to be used, as they may be removed in future versions. Circumvent problem instead.



namespace Lepra
{



Time::Time()
{
	UpdateTime();
}

Time::Time(const Time& pTime)
{
	mYear        = pTime.mYear;
	mMonth       = pTime.mMonth;
	mDay         = pTime.mDay;
	mWeekDay     = pTime.mWeekDay;
	mHour        = pTime.mHour;
	mMinute      = pTime.mMinute;
	mSecond      = pTime.mSecond;
	mMillis      = pTime.mMillis;
	mDaylightSavingTime = pTime.mDaylightSavingTime;
}

int Time::GetDaysInMonth() const
{
	switch(mMonth)
	{
	case 0:		// January
	case 2:		// March
	case 4:		// May
	case 6:		// July
	case 7:		// August
	case 9:		// October
	case 11:	// December
		return 31;
	case 1:		// February
		if (IsLeapYear())
		{
			return 29;
		}
		else
		{
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

void Time::UpdateTime()
{
	time_t lTime = time(NULL);
	tm* lTM = localtime(&lTime);
	const double s = HiResTimer(false).GetTime();

	mYear        = (int)lTM->tm_year + 1900;
	mMonth       = (int)lTM->tm_mon + 1;
	mDay         = (int)lTM->tm_mday;
	mWeekDay     = (int)lTM->tm_wday;
	mHour        = (int)lTM->tm_hour;
	mMinute      = (int)lTM->tm_min;
	mSecond      = (int)lTM->tm_sec;
	mMillis      = int((s-int(s))*1000);

	mDaylightSavingTime = ((int)lTM->tm_isdst > 0);
}

Time& Time::operator += (const Time& pTime)
{
	mYear        += pTime.mYear;
	mMonth       += pTime.mMonth;
	mDay         += pTime.mDay;
	mWeekDay     += pTime.mWeekDay;
	mHour        += pTime.mHour;
	mMinute      += pTime.mMinute;
	mSecond      += pTime.mSecond;
	mMillis      += pTime.mMillis;

	mSecond += mMillis / 1000;
	mMillis %= 1000;

	mMinute += mSecond / 60;
	mSecond %= 60;

	mHour += mMinute / 60;
	mMinute %= 60;

	mDay += mHour / 24;
	mWeekDay = (mWeekDay + mHour / 24) % 7;
	mHour %= 24;

	int lDaysInMonth = GetDaysInMonth();
	while (mDay > lDaysInMonth)
	{
		mMonth++;

		if (mMonth > 12)
		{
			mYear++;
			mMonth -= 12;
		}

		mDay -= lDaysInMonth;
		lDaysInMonth = GetDaysInMonth();
	}

	return *this;
}

Time& Time::operator -= (const Time& pTime)
{
	mYear        -= pTime.mYear;
	mMonth       -= pTime.mMonth;
	mDay         -= pTime.mDay;
	mWeekDay     -= pTime.mWeekDay;
	mHour        -= pTime.mHour;
	mMinute      -= pTime.mMinute;
	mSecond      -= pTime.mSecond;
	mMillis      -= pTime.mMillis;

	if (mMillis < 0)
	{
		mSecond += (mMillis-1000-1) / 1000;
		mMillis = (1000+mMillis) % 1000;
	}
	if (mSecond < 0)
	{
		mMinute += (mSecond-60-1) / 60;
		mSecond  = (60+mSecond) % 60;
	}
	if (mMinute < 0)
	{
		mHour  += (mMinute-60-1) / 60;
		mMinute = (60+mMinute) % 60;
	}
	if (mHour < 0)
	{
		mDay += (mHour-24-1) / 24;
		mHour = (24+mHour) % 24;
	}

	int lDaysInMonth = GetDaysInMonth();
	while (mDay <= 0)
	{
		mMonth--;

		if (mMonth <= 0)
		{
			mYear--;
			mMonth += 12;
		}

		mDay += lDaysInMonth;
		lDaysInMonth = GetDaysInMonth();
	}
	return *this;
}

str Time::GetDateTimeAsString() const
{
	return strutil::Format("%i-%.2i-%.2i %.2i:%.2i:%.2i,%.3i", GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond(), GetMillisecond());
}

bool Time::operator < (const Time& pTime) const
{
	if (mYear != pTime.mYear)
	{
		return mYear < pTime.mYear;
	}
	if (mMonth != pTime.mMonth)
	{
		return mMonth < pTime.mMonth;
	}
	if (mDay != pTime.mDay)
	{
		return mDay < pTime.mDay;
	}
	if (mHour != pTime.mHour)
	{
		return mHour < pTime.mHour;
	}
	if (mMinute != pTime.mMinute)
	{
		return mMinute < pTime.mMinute;
	}
	if (mSecond != pTime.mSecond)
	{
		return mSecond < pTime.mSecond;
	}
	if (mMillis != pTime.mMillis)
	{
		return mMillis < pTime.mMillis;
	}
	return false;
}

bool Time::operator >  (const Time& pTime) const
{
	if (mYear != pTime.mYear)
	{
		return mYear > pTime.mYear;
	}
	if (mMonth != pTime.mMonth)
	{
		return mMonth > pTime.mMonth;
	}
	if (mDay != pTime.mDay)
	{
		return mDay > pTime.mDay;
	}
	if (mHour != pTime.mHour)
	{
		return mHour > pTime.mHour;
	}
	if (mMinute != pTime.mMinute)
	{
		return mMinute > pTime.mMinute;
	}
	if (mSecond != pTime.mSecond)
	{
		return mSecond > pTime.mSecond;
	}
	if (mMillis != pTime.mMillis)
	{
		return mMillis > pTime.mMillis;
	}
	return false;
}

const Time& Time::operator = (const Time& pTime)
{
	mYear        = pTime.mYear;
	mMonth       = pTime.mMonth;
	mDay         = pTime.mDay;
	mWeekDay     = pTime.mWeekDay;
	mHour        = pTime.mHour;
	mMinute      = pTime.mMinute;
	mSecond      = pTime.mSecond;
	mMillis      = pTime.mMillis;
	mDaylightSavingTime = pTime.mDaylightSavingTime;
	return *this;
}


int Time::GetYear() const
{
	return mYear;
}

int Time::GetMonth() const
{
	return mMonth;
}

int Time::GetDay() const
{
	return mDay;
}

int Time::GetWeekDay() const
{
	return mWeekDay;
}

int Time::GetHour() const
{
	return mHour;
}

int Time::GetMinute() const
{
	return mMinute;
}

int Time::GetSecond() const
{
	return mSecond;
}

int Time::GetMillisecond() const
{
	return mMillis;
}

bool Time::IsLeapYear() const
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
	   mSecond      == pTime.mSecond &&
	   mMillis      == pTime.mMillis)
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



}
