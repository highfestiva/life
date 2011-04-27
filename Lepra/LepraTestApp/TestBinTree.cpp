/*
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include <assert.h>
#include "../Include/Log.h"
#include "../Include/String.h"
#include "../Include/BinTree.h"

using namespace Lepra;

void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);

bool TestBinTree(const LogDecorator& pAccount)
{
	str lContext;
	bool lTestOk = true;

	BinTree<int, int> lBinTree;
	BinTree<int, int>::Iterator lIter1;
	BinTree<int, int>::Iterator lIter2;
	BinTree<int, int>::Iterator lIter3;

	// Test iterator.
	lIter1 = lBinTree.End();
	lIter2 = lBinTree.Last();
	lIter3 = lBinTree.First();
	if (lIter2 != lBinTree.End())
	{
		lIter2++;
	}
	if (lIter3 != lBinTree.End())
	{
		lIter3--;
	}
	if (lTestOk)
	{
		lContext = _T("basic iterator test");
		lTestOk = !(lIter1 != lBinTree.End() || !(lIter1 == lBinTree.End()) ||
			lIter2 != lBinTree.End() || !(lIter2 == lBinTree.End()) ||
			lIter3 != lBinTree.End() || !(lIter3 == lBinTree.End()));
		assert(lTestOk);
	}

	lBinTree.Insert(1, 20);
	lBinTree.Insert(2, 19);
	lBinTree.Insert(3, 18);
	lBinTree.Insert(4, 17);
	lBinTree.Insert(5, 16);
	lBinTree.Insert(6, 15);
	lBinTree.Insert(7, 14);
	lBinTree.Insert(8, 13);
	if (lTestOk)
	{
		lContext = _T("search success test");
		lTestOk = !(lBinTree.Find(1) == lBinTree.End() || lBinTree.Find(2) == lBinTree.End() ||
			lBinTree.Find(3) == lBinTree.End() || lBinTree.Find(4) == lBinTree.End() ||
			lBinTree.Find(5) == lBinTree.End() || lBinTree.Find(6) == lBinTree.End() ||
			lBinTree.Find(7) == lBinTree.End() || lBinTree.Find(8) == lBinTree.End());
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("search fail test");
		lTestOk = !(lBinTree.Find(0) != lBinTree.End() || lBinTree.Find(-123) != lBinTree.End() ||
			lBinTree.Find(123)  != lBinTree.End() || lBinTree.Find(-34)  != lBinTree.End() ||
			lBinTree.Find(45)   != lBinTree.End() || lBinTree.Find(-56)  != lBinTree.End() ||
			lBinTree.Find(67)   != lBinTree.End() || lBinTree.Find(-78)  != lBinTree.End());
		assert(lTestOk);
	}

	lIter2 = lBinTree.Last();
	lIter3 = lBinTree.First();
	if (lTestOk)
	{
		lContext = _T("iterator test A");
		lTestOk = !(lIter2 == lBinTree.End() || lIter3 == lBinTree.End());
		assert(lTestOk);
	}
	// These should become end pointers.
	lIter2++;
	lIter3--;
	if (lTestOk)
	{
		lContext = _T("iterator test B");
		lTestOk = !(lIter2 != lBinTree.End() || lIter3 != lBinTree.End());
		assert(lTestOk);
	}

	// Check iterating.
	int lCount = 0;
	for (lIter1 = lBinTree.First(); lIter1 != lBinTree.End(); ++lIter1)
	{
		lCount++;
	}
	// Check counting.
	if (lTestOk)
	{
		lContext = _T("iterator counting");
		lTestOk = !(lCount != 8 || lCount != lBinTree.GetCount());
		assert(lTestOk);
	}

	// Check if sorted.
	lCount = 20;
	for (lIter1 = lBinTree.First(); lIter1 != lBinTree.End(); ++lIter1, --lCount)
	{
		if (lTestOk)
		{
			lContext = _T("sorting failed");
			lTestOk = !((*lIter1) != lCount);
			assert(lTestOk);
		}
	}

	// Test remove.
	lBinTree.Remove(4);
	if (lTestOk)
	{
		lContext = _T("remove failed");
		lTestOk = !(lBinTree.Find(1) == lBinTree.End() || lBinTree.Find(2) == lBinTree.End() ||
			lBinTree.Find(3) == lBinTree.End() || lBinTree.Find(4) != lBinTree.End() ||
			lBinTree.Find(5) == lBinTree.End() || lBinTree.Find(6) == lBinTree.End() ||
			lBinTree.Find(7) == lBinTree.End() || lBinTree.Find(8) == lBinTree.End());
		assert(lTestOk);
	}

	// Now, do some random input and removal.
	lBinTree.RemoveAll();
	lCount = 0;
	int i;
	for (i = 0; i < 1024; i++)
	{
		int lRnd = rand();
		lBinTree.Insert(lRnd, lRnd);
		lBinTree.Remove(lRnd);
		if (lTestOk)
		{
			lContext = _T("random remove failed");
			lTestOk = !(lBinTree.GetCount() != 0);
			assert(lTestOk);
		}
	}

	for (i = 0; i < 1024; i++)
	{
		int lRnd = rand();
		lBinTree.Insert(lRnd, lRnd);
		lRnd = rand();
		lBinTree.Remove(lRnd);
	}
	// Try iterating.
	lCount = 0;
	for (lIter1 = lBinTree.First(); lIter1 != lBinTree.End(); ++lIter1)
	{
		lCount++;
	}
	// Check counting again.
	if (lTestOk)
	{
		lContext = _T("entries failed A");
		lTestOk = !(lCount != lBinTree.GetCount());
		assert(lTestOk);
	}

	// Balance the tree.
	lBinTree.Balance();
	// Try iterating.
	lCount = 0;
	for (lIter1 = lBinTree.First(); lIter1 != lBinTree.End(); ++lIter1)
	{
		lCount++;
	}
	// Check counting again.
	if (lTestOk)
	{
		lContext = _T("entries failed B");
		lTestOk = !(lCount != lBinTree.GetCount());
		assert(lTestOk);
	}

	// Remove all, and count again.
	lBinTree.RemoveAll();
	lCount = 0;
	for (lIter1 = lBinTree.First(); lIter1 != lBinTree.End(); ++lIter1)
	{
		lCount++;
	}
	if (lTestOk)
	{
		lContext = _T("entries found");
		lTestOk = !(lCount != 0 || lCount != lBinTree.GetCount());
		assert(lTestOk);
	}

	ReportTestResult(pAccount, _T("BinTree"), lContext, lTestOk);
	return (lTestOk);
}
