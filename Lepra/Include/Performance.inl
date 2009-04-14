
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



namespace Lepra
{



template<class _Data> SequencialPerformanceData<_Data>::SequencialPerformanceData()
{
	Clear();
}

template<class _Data> void SequencialPerformanceData<_Data>::Clear()
{
	Set(0, 0, 0, 0);
}

template<class _Data> void SequencialPerformanceData<_Data>::Set(double pMinimum, double pThis, double pMaximum, _Data pPreviousValue)
{
	mPreviousValue = pPreviousValue;
	Parent::Set(pMinimum, pThis, pMaximum);
}

template<class _Data> void SequencialPerformanceData<_Data>::Update(double pTime, _Data pValue)
{
	assert (pTime >= 0);
	if (pTime > 0)
	{
		double lPeriodValue = (pValue-mPreviousValue)/pTime;
		mPreviousValue = pValue;
		Parent::Update(lPeriodValue);
	}
}



}
