
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/Math.h"
#include "../Include/Performance.h"



namespace Lepra
{



PerformanceData::PerformanceData()
{
	Clear();
}

void PerformanceData::Clear()
{
	mFirstTime = true;
	mTimeOfLastMeasure = 0;
	Set(0, 0, 0);
}

void PerformanceData::Set(double pMinimum, double pThis, double pMaximum)
{
	ResetHitCount();
	mMinimum = pMinimum;
	mLast = pThis;
	mSlidingAverage = pThis;
	mMaximum = pMaximum;
}

void PerformanceData::Append(double pPeriodValue, double pTimeOfLastMeasure)
{
	++mHitCount;
	mTimeOfLastMeasure = pTimeOfLastMeasure;
	if (mFirstTime)
	{
		mFirstTime = false;
		Set(pPeriodValue, pPeriodValue, pPeriodValue);
	}
	else
	{
		mMinimum = (pPeriodValue < mMinimum)? pPeriodValue : mMinimum;
		mLast = pPeriodValue;
		mSlidingAverage = Math::Lerp(mSlidingAverage, mLast, 0.05);
		mMaximum = (pPeriodValue > mMaximum)? pPeriodValue : mMaximum;
	}
}

void PerformanceData::ResetHitCount()
{
	mHitCount = 0;
}

int PerformanceData::GetHitCount() const
{
	return mHitCount;
}

double PerformanceData::GetTimeOfLastMeasure() const
{
	return (mTimeOfLastMeasure);
}

double PerformanceData::GetMinimum() const
{
	return (mMinimum);
}

double PerformanceData::GetLast() const
{
	return (mLast);
}

double PerformanceData::GetSlidingAverage() const
{
	return (mSlidingAverage);
}

double PerformanceData::GetMaximum() const
{
	return (mMaximum);
}

double PerformanceData::GetRangeFactor() const
{
	if (mMinimum)
	{
		return ((mMaximum-mMinimum)/mMinimum);
	}
	return (0);
}



ScopePerformanceData* ScopePerformanceData::Insert(const str& pName, size_t pHash)
{
	ScopePerformanceData* lParent = GetActive();
	if (!lParent)
	{
		ScopePerformanceData* lRoot = new ScopePerformanceData(0, pName, pHash);
		AddRoot(lRoot);
		SetActive(lRoot);
		return (lRoot);
	}
	// Check if we're root and being reactivated, otherwise it's plain ol' recursion.
	if (lParent->mHash == pHash && lParent->mParent == 0)
	{
		SetActive(lParent);
		return (lParent);
	}

	ScopePerformanceData* lNode;
	{
		ScopeSpinLock lLock(&mRootLock);
		// Find self.
		lNode = lParent->FindChild(/*pName,*/ pHash);
		if (!lNode)
		{
			// Not listed, so beam us up Scotty.
			lNode = new ScopePerformanceData(lParent, pName, pHash);
		}
		else
		{
			assert(lNode->GetName() == pName);
		}
	}
	SetActive(lNode);
	return (lNode);
}

ScopePerformanceData::ScopePerformanceData(ScopePerformanceData* pParent, const str& pName, size_t pHash):
	mName(pName),
	mHash(pHash),
	mParent(pParent)
{
	if (mParent)
	{
		mParent->mChildArray.push_back(this);
	}
}

void ScopePerformanceData::ClearAll()
{
	ScopeSpinLock lLock(&mRootLock);
	NodeArray::iterator x = mRoots.begin();
	while (x != mRoots.end())
	{
		if ((*x)->GetMaximum() == 0)
		{
			mRoots.erase(x++);
		}
		else
		{
			++x;
		}
	}
	ClearAll(mRoots);
}

void ScopePerformanceData::Append(double pPeriodValue, double pTimeOfLastMeasure)
{
	Parent::Append(pPeriodValue, pTimeOfLastMeasure);
	if (mParent)
	{
		SetActive(mParent);
	}
}

ScopePerformanceData::NodeArray ScopePerformanceData::GetRoots()
{
	ScopeSpinLock lLock(&mRootLock);
	NodeArray lRootsCopy(mRoots);
	return (lRootsCopy);
}

const str& ScopePerformanceData::GetName() const
{
	return (mName);
}

ScopePerformanceData::NodeArray ScopePerformanceData::GetChildren() const
{
	ScopeSpinLock lLock(&mRootLock);
	NodeArray lChildrenCopy(mChildArray);
	return (lChildrenCopy);
}

void ScopePerformanceData::ClearAll(NodeArray& pNodes)
{
	NodeArray::iterator x = pNodes.begin();
	for (; x != pNodes.end(); ++x)
	{
		(*x)->Clear();
		ClearAll((*x)->mChildArray);
	}
}

ScopePerformanceData* ScopePerformanceData::FindChild(/*const str& pName,*/ size_t pHash) const
{
	NodeArray::const_iterator x = mChildArray.begin();
	for (; x != mChildArray.end(); ++x)
	{
		if ((*x)->mHash == pHash)
		{
			return (*x);
		}
	}
	return (0);
}

void ScopePerformanceData::AddRoot(ScopePerformanceData* pNode)
{
	ScopeSpinLock lLock(&mRootLock);
	mRoots.push_back(pNode);
}

void ScopePerformanceData::SetActive(ScopePerformanceData* pNode)
{
	Thread::SetExtraData(pNode);
}

ScopePerformanceData* ScopePerformanceData::GetActive()
{
	return ((ScopePerformanceData*)Thread::GetExtraData());
}

ScopePerformanceData::NodeArray ScopePerformanceData::mRoots;
SpinLock ScopePerformanceData::mRootLock;



}
