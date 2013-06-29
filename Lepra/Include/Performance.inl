
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



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

template<class _Data> void SequencialPerformanceData<_Data>::Append(double pTime, double pStartTime, _Data pValue)
{
	assert (pTime >= 0);
	if (pTime > 0)
	{
		double lPeriodValue = (pValue-mPreviousValue)/pTime;
		mPreviousValue = pValue;
		Parent::Append(lPeriodValue, pStartTime);
	}
}



template<class _T> BasicScopeTimer<_T>::BasicScopeTimer():
	mTime(false)
{
}

template<class _T> BasicScopeTimer<_T>::BasicScopeTimer(_T* pData):
	mTime(false),
	mData(pData)
{
}

template<class _T> BasicScopeTimer<_T>::~BasicScopeTimer()
{
	const double lStart = mTime.GetTime();
	const double lDelta = mTime.PopTimeDiff();
	mData->Append(lDelta, lStart);
	mData = 0;
}

template<class _T> void BasicScopeTimer<_T>::Attach(_T* pData)
{
	mData = pData;
}



}
