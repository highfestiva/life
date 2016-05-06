/*
	File:   TestSkipList.cpp
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../include/log.h"
#include "../include/string.h"
#include "../include/skiplist.h"

using namespace lepra;

void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);

/*bool TestSkipList(const LogDecorator& account) {
	str _context;
	bool test_ok = true;

	SkipList<int, int> lSkipList;
	SkipList<int, int>::Iterator iter1;
	SkipList<int, int>::Iterator iter2;
	SkipList<int, int>::Iterator iter3;

	// Test iterator.
	iter1 = lSkipList.End();
	iter2 = lSkipList.Last();
	iter3 = lSkipList.First();
	if (iter2 != lSkipList.End()) {
		iter2++;
	}
	if (iter3 != lSkipList.End()) {
		iter3--;
	}
	if (test_ok) {
		_context = "basic iterator test";
		test_ok = !(iter1 != lSkipList.End() || !(iter1 == lSkipList.End()) ||
			iter2 != lSkipList.End() || !(iter2 == lSkipList.End()) ||
			iter3 != lSkipList.End() || !(iter3 == lSkipList.End()));
		deb_assert(test_ok);
	}

	lSkipList.Insert(1, 20);
	lSkipList.Insert(2, 19);
	lSkipList.Insert(3, 18);
	lSkipList.Insert(4, 17);
	lSkipList.Insert(5, 16);
	lSkipList.Insert(6, 15);
	lSkipList.Insert(7, 14);
	lSkipList.Insert(8, 13);
	if (test_ok) {
		_context = "search success test";
		test_ok = !(lSkipList.Find(1) == lSkipList.End() || lSkipList.Find(2) == lSkipList.End() ||
			lSkipList.Find(3) == lSkipList.End() || lSkipList.Find(4) == lSkipList.End() ||
			lSkipList.Find(5) == lSkipList.End() || lSkipList.Find(6) == lSkipList.End() ||
			lSkipList.Find(7) == lSkipList.End() || lSkipList.Find(8) == lSkipList.End());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "search fail test";
		test_ok = !(lSkipList.Find(0) != lSkipList.End() || lSkipList.Find(-123) != lSkipList.End() ||
			lSkipList.Find(123) != lSkipList.End() || lSkipList.Find(-34) != lSkipList.End() ||
			lSkipList.Find(45) != lSkipList.End() || lSkipList.Find(-56) != lSkipList.End() ||
			lSkipList.Find(67) != lSkipList.End() || lSkipList.Find(-78) != lSkipList.End());
		deb_assert(test_ok);
	}

	iter2 = lSkipList.Last();
	iter3 = lSkipList.First();
	if (test_ok) {
		_context = "iterator test A";
		test_ok = !(iter2 == lSkipList.End() || iter3 == lSkipList.End());
		deb_assert(test_ok);
	}
	// These should become end pointers.
	iter2++;
	iter3--;
	if (test_ok) {
		_context = "iterator test B";
		test_ok = !(iter2 != lSkipList.End() || iter3 != lSkipList.End());
		deb_assert(test_ok);
	}

	// Check iterating.
	int count = 0;
	for (iter1 = lSkipList.First(); iter1 != lSkipList.End(); ++iter1) {
		count++;
	}
	// Check counting.
	if (test_ok) {
		_context = "iterator counting";
		test_ok = !(count != 8 || count != lSkipList.GetCount());
		deb_assert(test_ok);
	}

	// Check if sorted.
	count = 20;
	for (iter1 = lSkipList.First(); iter1 != lSkipList.End(); ++iter1, --count) {
		if (test_ok) {
			_context = "sorting failed";
			test_ok = !((*iter1) != count);
			deb_assert(test_ok);
		}
	}

	// Test remove.
	lSkipList.Remove(4);
	if (test_ok) {
		_context = "remove failed";
		test_ok = !(lSkipList.Find(1) == lSkipList.End() || lSkipList.Find(2) == lSkipList.End() ||
			lSkipList.Find(3) == lSkipList.End() || lSkipList.Find(4) != lSkipList.End() ||
			lSkipList.Find(5) == lSkipList.End() || lSkipList.Find(6) == lSkipList.End() ||
			lSkipList.Find(7) == lSkipList.End() || lSkipList.Find(8) == lSkipList.End());
		deb_assert(test_ok);
	}

	// Now, do some random input and removal.
	lSkipList.RemoveAll();
	count = 0;
	int i;
	for (i = 0; i < 1024; i++) {
		int rnd = rand();
		lSkipList.Insert(rnd, rnd);
		lSkipList.Remove(rnd);
		if (test_ok) {
			_context = "random remove failed";
			test_ok = !(lSkipList.GetCount() != 0);
			deb_assert(test_ok);
		}
	}

	for (i = 0; i < 1024; i++) {
		int rnd = rand();
		lSkipList.Insert(rnd, rnd);
		rnd = rand();
		lSkipList.Remove(rnd);
	}

	// Try iterating.
	count = 0;
	for (iter1 = lSkipList.First(); iter1 != lSkipList.End(); ++iter1) {
		count++;
	}
	// Check counting again.
	if (test_ok) {
		_context = "entries failed A";
		test_ok = !(count != lSkipList.GetCount());
		deb_assert(test_ok);
	}

	// Remove all, and count again.
	lSkipList.RemoveAll();
	count = 0;
	for (iter1 = lSkipList.First(); iter1 != lSkipList.End(); ++iter1) {
		count++;
	}
	if (test_ok) {
		_context = "entries found";
		test_ok = !(count != 0 || count != lSkipList.GetCount());
		deb_assert(test_ok);
	}

	ReportTestResult(account, "SkipList", _context, test_ok);
	return (test_ok);
}*/
