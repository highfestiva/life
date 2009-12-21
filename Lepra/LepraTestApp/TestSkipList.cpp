/*
	File:   TestSkipList.cpp
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include <assert.h>
#include "../Include/Log.h"
#include "../Include/String.h"
#include "../Include/SkipList.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);

/*bool TestSkipList(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	SkipList<int, int> lSkipList;
	SkipList<int, int>::Iterator lIter1;
	SkipList<int, int>::Iterator lIter2;
	SkipList<int, int>::Iterator lIter3;

	// Test iterator.
	lIter1 = lSkipList.End();
	lIter2 = lSkipList.Last();
	lIter3 = lSkipList.First();
	if (lIter2 != lSkipList.End())
	{
		lIter2++;
	}
	if (lIter3 != lSkipList.End())
	{
		lIter3--;
	}
	if (lTestOk)
	{
		lContext = _T("basic iterator test");
		lTestOk = !(lIter1 != lSkipList.End() || !(lIter1 == lSkipList.End()) ||
			lIter2 != lSkipList.End() || !(lIter2 == lSkipList.End()) ||
			lIter3 != lSkipList.End() || !(lIter3 == lSkipList.End()));
		assert(lTestOk);
	}

	lSkipList.Insert(1, 20);
	lSkipList.Insert(2, 19);
	lSkipList.Insert(3, 18);
	lSkipList.Insert(4, 17);
	lSkipList.Insert(5, 16);
	lSkipList.Insert(6, 15);
	lSkipList.Insert(7, 14);
	lSkipList.Insert(8, 13);
	if (lTestOk)
	{
		lContext = _T("search success test");
		lTestOk = !(lSkipList.Find(1) == lSkipList.End() || lSkipList.Find(2) == lSkipList.End() ||
			lSkipList.Find(3) == lSkipList.End() || lSkipList.Find(4) == lSkipList.End() ||
			lSkipList.Find(5) == lSkipList.End() || lSkipList.Find(6) == lSkipList.End() ||
			lSkipList.Find(7) == lSkipList.End() || lSkipList.Find(8) == lSkipList.End());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("search fail test");
		lTestOk = !(lSkipList.Find(0) != lSkipList.End() || lSkipList.Find(-123) != lSkipList.End() ||
			lSkipList.Find(123) != lSkipList.End() || lSkipList.Find(-34) != lSkipList.End() ||
			lSkipList.Find(45) != lSkipList.End() || lSkipList.Find(-56) != lSkipList.End() ||
			lSkipList.Find(67) != lSkipList.End() || lSkipList.Find(-78) != lSkipList.End());
		assert(lTestOk);
	}

	lIter2 = lSkipList.Last();
	lIter3 = lSkipList.First();
	if (lTestOk)
	{
		lContext = _T("iterator test A");
		lTestOk = !(lIter2 == lSkipList.End() || lIter3 == lSkipList.End());
		assert(lTestOk);
	}
	// These should become end pointers.
	lIter2++;
	lIter3--;
	if (lTestOk)
	{
		lContext = _T("iterator test B");
		lTestOk = !(lIter2 != lSkipList.End() || lIter3 != lSkipList.End());
		assert(lTestOk);
	}

	// Check iterating.
	int lCount = 0;
	for (lIter1 = lSkipList.First(); lIter1 != lSkipList.End(); ++lIter1)
	{
		lCount++;
	}
	// Check counting.
	if (lTestOk)
	{
		lContext = _T("iterator counting");
		lTestOk = !(lCount != 8 || lCount != lSkipList.GetCount());
		assert(lTestOk);
	}

	// Check if sorted.
	lCount = 20;
	for (lIter1 = lSkipList.First(); lIter1 != lSkipList.End(); ++lIter1, --lCount)
	{
		if (lTestOk)
		{
			lContext = _T("sorting failed");
			lTestOk = !((*lIter1) != lCount);
			assert(lTestOk);
		}
	}

	// Test remove.
	lSkipList.Remove(4);
	if (lTestOk)
	{
		lContext = _T("remove failed");
		lTestOk = !(lSkipList.Find(1) == lSkipList.End() || lSkipList.Find(2) == lSkipList.End() ||
			lSkipList.Find(3) == lSkipList.End() || lSkipList.Find(4) != lSkipList.End() ||
			lSkipList.Find(5) == lSkipList.End() || lSkipList.Find(6) == lSkipList.End() ||
			lSkipList.Find(7) == lSkipList.End() || lSkipList.Find(8) == lSkipList.End());
		assert(lTestOk);
	}

	// Now, do some random input and removal.
	lSkipList.RemoveAll();
	lCount = 0;
	int i;
	for (i = 0; i < 1024; i++)
	{
		int lRnd = rand();
		lSkipList.Insert(lRnd, lRnd);
		lSkipList.Remove(lRnd);
		if (lTestOk)
		{
			lContext = _T("random remove failed");
			lTestOk = !(lSkipList.GetCount() != 0);
			assert(lTestOk);
		}
	}

	for (i = 0; i < 1024; i++)
	{
		int lRnd = rand();
		lSkipList.Insert(lRnd, lRnd);
		lRnd = rand();
		lSkipList.Remove(lRnd);
	}

	// Try iterating.
	lCount = 0;
	for (lIter1 = lSkipList.First(); lIter1 != lSkipList.End(); ++lIter1)
	{
		lCount++;
	}
	// Check counting again.
	if (lTestOk)
	{
		lContext = _T("entries failed A");
		lTestOk = !(lCount != lSkipList.GetCount());
		assert(lTestOk);
	}

	// Remove all, and count again.
	lSkipList.RemoveAll();
	lCount = 0;
	for (lIter1 = lSkipList.First(); lIter1 != lSkipList.End(); ++lIter1)
	{
		lCount++;
	}
	if (lTestOk)
	{
		lContext = _T("entries found");
		lTestOk = !(lCount != 0 || lCount != lSkipList.GetCount());
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("SkipList"), lContext, lTestOk);
	return (lTestOk);
}*/
