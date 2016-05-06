/*
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../lepra/include/lepraassert.h"
#include "../include/log.h"
#include "../include/string.h"
#include "../include/bintree.h"

using namespace lepra;

void ReportTestResult(const lepra::LogDecorator& log, const str& test_name, const str& context, bool result);

bool TestBinTree(const LogDecorator& account) {
	str _context;
	bool test_ok = true;

	BinTree<int, int> bin_tree;
	BinTree<int, int>::Iterator iter1;
	BinTree<int, int>::Iterator iter2;
	BinTree<int, int>::Iterator iter3;

	// Test iterator.
	iter1 = bin_tree.End();
	iter2 = bin_tree.Last();
	iter3 = bin_tree.First();
	if (iter2 != bin_tree.End()) {
		iter2++;
	}
	if (iter3 != bin_tree.End()) {
		iter3--;
	}
	if (test_ok) {
		_context = "basic iterator test";
		test_ok = !(iter1 != bin_tree.End() || !(iter1 == bin_tree.End()) ||
			iter2 != bin_tree.End() || !(iter2 == bin_tree.End()) ||
			iter3 != bin_tree.End() || !(iter3 == bin_tree.End()));
		deb_assert(test_ok);
	}

	bin_tree.Insert(1, 20);
	bin_tree.Insert(2, 19);
	bin_tree.Insert(3, 18);
	bin_tree.Insert(4, 17);
	bin_tree.Insert(5, 16);
	bin_tree.Insert(6, 15);
	bin_tree.Insert(7, 14);
	bin_tree.Insert(8, 13);
	if (test_ok) {
		_context = "search success test";
		test_ok = !(bin_tree.Find(1) == bin_tree.End() || bin_tree.Find(2) == bin_tree.End() ||
			bin_tree.Find(3) == bin_tree.End() || bin_tree.Find(4) == bin_tree.End() ||
			bin_tree.Find(5) == bin_tree.End() || bin_tree.Find(6) == bin_tree.End() ||
			bin_tree.Find(7) == bin_tree.End() || bin_tree.Find(8) == bin_tree.End());
		deb_assert(test_ok);
	}
	if (test_ok) {
		_context = "search fail test";
		test_ok = !(bin_tree.Find(0) != bin_tree.End() || bin_tree.Find(-123) != bin_tree.End() ||
			bin_tree.Find(123)  != bin_tree.End() || bin_tree.Find(-34)  != bin_tree.End() ||
			bin_tree.Find(45)   != bin_tree.End() || bin_tree.Find(-56)  != bin_tree.End() ||
			bin_tree.Find(67)   != bin_tree.End() || bin_tree.Find(-78)  != bin_tree.End());
		deb_assert(test_ok);
	}

	iter2 = bin_tree.Last();
	iter3 = bin_tree.First();
	if (test_ok) {
		_context = "iterator test A";
		test_ok = !(iter2 == bin_tree.End() || iter3 == bin_tree.End());
		deb_assert(test_ok);
	}
	// These should become end pointers.
	iter2++;
	iter3--;
	if (test_ok) {
		_context = "iterator test B";
		test_ok = !(iter2 != bin_tree.End() || iter3 != bin_tree.End());
		deb_assert(test_ok);
	}

	// Check iterating.
	int count = 0;
	for (iter1 = bin_tree.First(); iter1 != bin_tree.End(); ++iter1) {
		count++;
	}
	// Check counting.
	if (test_ok) {
		_context = "iterator counting";
		test_ok = !(count != 8 || count != bin_tree.GetCount());
		deb_assert(test_ok);
	}

	// Check if sorted.
	count = 20;
	for (iter1 = bin_tree.First(); iter1 != bin_tree.End(); ++iter1, --count) {
		if (test_ok) {
			_context = "sorting failed";
			test_ok = !((*iter1) != count);
			deb_assert(test_ok);
		}
	}

	// Test remove.
	bin_tree.Remove(4);
	if (test_ok) {
		_context = "remove failed";
		test_ok = !(bin_tree.Find(1) == bin_tree.End() || bin_tree.Find(2) == bin_tree.End() ||
			bin_tree.Find(3) == bin_tree.End() || bin_tree.Find(4) != bin_tree.End() ||
			bin_tree.Find(5) == bin_tree.End() || bin_tree.Find(6) == bin_tree.End() ||
			bin_tree.Find(7) == bin_tree.End() || bin_tree.Find(8) == bin_tree.End());
		deb_assert(test_ok);
	}

	// Now, do some random input and removal.
	bin_tree.RemoveAll();
	count = 0;
	int i;
	for (i = 0; i < 1024; i++) {
		int rnd = rand();
		bin_tree.Insert(rnd, rnd);
		bin_tree.Remove(rnd);
		if (test_ok) {
			_context = "random remove failed";
			test_ok = !(bin_tree.GetCount() != 0);
			deb_assert(test_ok);
		}
	}

	for (i = 0; i < 1024; i++) {
		int rnd = rand();
		bin_tree.Insert(rnd, rnd);
		rnd = rand();
		bin_tree.Remove(rnd);
	}
	// Try iterating.
	count = 0;
	for (iter1 = bin_tree.First(); iter1 != bin_tree.End(); ++iter1) {
		count++;
	}
	// Check counting again.
	if (test_ok) {
		_context = "entries failed A";
		test_ok = !(count != bin_tree.GetCount());
		deb_assert(test_ok);
	}

	// Balance the tree.
	bin_tree.Balance();
	// Try iterating.
	count = 0;
	for (iter1 = bin_tree.First(); iter1 != bin_tree.End(); ++iter1) {
		count++;
	}
	// Check counting again.
	if (test_ok) {
		_context = "entries failed B";
		test_ok = !(count != bin_tree.GetCount());
		deb_assert(test_ok);
	}

	// Remove all, and count again.
	bin_tree.RemoveAll();
	count = 0;
	for (iter1 = bin_tree.First(); iter1 != bin_tree.End(); ++iter1) {
		count++;
	}
	if (test_ok) {
		_context = "entries found";
		test_ok = !(count != 0 || count != bin_tree.GetCount());
		deb_assert(test_ok);
	}

	ReportTestResult(account, "BinTree", _context, test_ok);
	return (test_ok);
}
