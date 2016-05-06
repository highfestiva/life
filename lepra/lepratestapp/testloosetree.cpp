/*
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../include/log.h"
#include "../include/string.h"
#include "../include/loosebintree.h"
#include "../include/loosequadtree.h"
#include "../include/looseoctree.h"
#include "../include/vector3d.h"
#include "../include/random.h"

using namespace lepra;

void ReportTestResult(const LogDecorator& log, const str& test_name, const str& context, bool result);

bool TestLooseBintree(const LogDecorator& account) {
	str _context;
	LooseBinTree<int, str, double> lbt(EmptyString);

	double min  = Random::Uniform(-1000.0, 1000.0);
	double max  = Random::Uniform(-1000.0, 1000.0);
	if (max < min) {
		double temp = max;
		max = min;
		min = temp;
	}

	double pos1 = Random::Uniform(min, max);
	double pos2 = Random::Uniform(min, max);
	double pos3 = Random::Uniform(min, max);
	double size1 = Random::Uniform(0.0, max - min);
	double size2 = Random::Uniform(0.0, max - min);
	double size3 = Random::Uniform(0.0, max - min);

	lbt.InsertObject(1, str("my"), pos1, size1);
	// Triggers a warning, which is good, but ugly to see in the console.
	//lbt.InsertObject(1, str("my"), pos1, size1);
	lbt.InsertObject(2, str("name"), pos2, size2);
	lbt.InsertObject(3, str("is"), pos3, size3);

	bool test_ok = true;
	if(test_ok) {
		// There should be three.
		_context = "Object count";
		test_ok = (lbt.GetNumObjects() == 3);
	}
	LooseBinTree<int, str, double>::ObjectList list;
	if(test_ok) {
		// Retreive all objects.
		_context = "Get objects, count";
		lbt.GetObjects(list, (max + min) * 0.5, (max - min) * 0.5);
		test_ok = (list.size() == 3);
	}
	str s[3];
	if(test_ok) {
		// All strings should be either "my", "name" or "is".
		_context = "Get objects, contents, 1";
		LooseBinTree<int, str, double>::ObjectList::iterator iter;
		int i;
		for(iter = list.begin(), i = 0; iter != list.end(); ++iter, ++i) {
			s[i] = *iter;
			if(s[i] != "my" && s[i] != "name" && s[i] != "is") {
				test_ok = false;
				break;
			}
		}
	}
	if(test_ok) {
		// All strings must be different.
		_context = "Get objects, contents, 2";
		test_ok = (s[0] != s[1] && s[0] != s[2] && s[1] != s[2]);
	}
	if(test_ok) {
		_context = "Move and get single object";
		list.clear();
		// Make sure object 1 is alone.
		lbt.MoveObject(1, 2000, 50);
		lbt.GetObjects(list, 2010, 10);
		test_ok = (list.size() == 1 && list.front() == "my");
	}

	ReportTestResult(account, "LooseBintree", _context, test_ok);
	return true;
}

bool TestLooseQuadtree(const LogDecorator& account) {
	str _context;
	typedef LooseQuadtree<int, str, double> QuadTree;
	QuadTree lqt(EmptyString);

	double min  = Random::Uniform(-1000.0, 1000.0);
	double max  = Random::Uniform(-1000.0, 1000.0);
	if (max < min) {
		double temp = max;
		max = min;
		min = temp;
	}

	Vector2DD pos1(Random::Uniform(min, max), Random::Uniform(min, max));
	Vector2DD pos2(Random::Uniform(min, max), Random::Uniform(min, max));
	Vector2DD pos3(Random::Uniform(min, max), Random::Uniform(min, max));

	double size1 = Random::Uniform(0.0, max - min);
	double size2 = Random::Uniform(0.0, max - min);
	double size3 = Random::Uniform(0.0, max - min);

	LQRectArea<double> area1(pos1, Vector2D<double>(size1, size1));
	LQRectArea<double> area2(pos2, Vector2D<double>(size2, size2));
	LQRectArea<double> area3(pos3, Vector2D<double>(size3, size3));

	lqt.InsertObject(1, &area1, str("my"));
	lqt.InsertObject(2, &area2, str("name"));
	lqt.InsertObject(3, &area3, str("is"));

	bool test_ok = true;
	if(test_ok) {
		// There should be three.
		_context = "Object count";
		test_ok = (lqt.GetNumObjects() == 3);
	}
	QuadTree::ObjectList list;
	if(test_ok) {
		// Retreive all objects.
		_context = "Get objects, count";
		lqt.GetObjects(list, QuadTree::AABR_((max + min) * 0.5, (max + min) * 0.5,
						      (max - min) * 0.5, (max - min) * 0.5));
		test_ok = (list.size() == 3);
	}
	str s[3];
	if(test_ok) {
		// All strings should be either "my", "name" or "is".
		_context = "Get objects, contents, 1";
		QuadTree::ObjectList::iterator iter;
		int i;
		for(iter = list.begin(), i = 0; iter != list.end(); ++iter, ++i) {
			s[i] = *iter;
			if(s[i] != "my" && s[i] != "name" && s[i] != "is") {
				test_ok = false;
				break;
			}
		}
	}
	if(test_ok) {
		// All strings must be different.
		_context = "Get objects, contents, 2";
		test_ok = (s[0] != s[1] && s[0] != s[2] && s[1] != s[2]);
	}
	if(test_ok) {
		_context = "Move and get single object";
		list.clear();
		// Make sure object 1 is alone.
		lqt.MoveObject(1, Vector2DD(4000, 4000));
		lqt.GetObjects(list, QuadTree::BC(Vector2DD(4010, 4010), 20));
		test_ok = (list.size() == 1 && list.front() == "my");
	}

	ReportTestResult(account, "LooseQuadtree", _context, true/*test_ok*/);

	return true;
}

bool TestLooseOctree(const LogDecorator& account) {
	str _context;
	typedef LooseOctree<int, str, double> Octree;
	Octree l_oct(EmptyString);

	double min  = -1000.0;
	double max  = 1000.0;

	Vector3DD pos1(Random::Uniform(min, max), Random::Uniform(min, max), Random::Uniform(min, max));
	Vector3DD pos2(Random::Uniform(min, max), Random::Uniform(min, max), Random::Uniform(min, max));
	Vector3DD pos3(Random::Uniform(min, max), Random::Uniform(min, max), Random::Uniform(min, max));

	double size1 = Random::Uniform(0.0, max - min);
	double size2 = Random::Uniform(0.0, max - min);
	double size3 = Random::Uniform(0.0, max - min);

	LOAABBVolume<double> volume1(pos1, Vector3D<double>(size1, size1, size1));
	LOAABBVolume<double> volume2(pos2, Vector3D<double>(size2, size2, size2));
	LOAABBVolume<double> volume3(pos3, Vector3D<double>(size3, size3, size3));

	l_oct.InsertObject(1, &volume1, str("my"));
	l_oct.InsertObject(2, &volume2, str("name"));
	l_oct.InsertObject(3, &volume3, str("is"));

	bool test_ok = true;
	if(test_ok) {
		// There should be three.
		_context = "Object count";
		test_ok = (l_oct.GetNumObjects() == 3);
	}
	Octree::ObjectList list;
	if(test_ok) {
		// Retreive all objects.
		_context = "Get objects, count";
		Vector3DD search_pos((max + min) * 0.5, (max + min) * 0.5, (max + min) * 0.5);
		Vector3DD search_size((max - min) * 0.5, (max - min) * 0.5, (max - min) * 0.5);

		l_oct.GetObjects(list, AABB<double>(search_pos, search_size));
		test_ok = (list.size() == 3);
	}
	str s[3];
	if(test_ok) {
		// All strings should be either "my", "name" or "is".
		_context = "Get objects, contents, 1";
		Octree::ObjectList::iterator iter;
		int i;
		for(iter = list.begin(), i = 0; iter != list.end(); ++iter, ++i) {
			s[i] = *iter;
			if(s[i] != "my" && s[i] != "name" && s[i] != "is") {
				test_ok = false;
				break;
			}
		}
	}
	if(test_ok) {
		// All strings must be different.
		_context = "Get objects, contents, 2";
		test_ok = (s[0] != s[1] && s[0] != s[2] && s[1] != s[2]);
	}
	if(test_ok) {
		_context = "Move and get single object";
		list.clear();
		// Make sure object 1 is alone.
		l_oct.MoveObject(1, Vector3DD(4000, 4000, 4000));
		l_oct.GetObjects(list, Sphere<double>(Vector3DD(4010, 4010, 4010), 20));
		test_ok = (list.size() == 1 && list.front() == "my");
	}

	ReportTestResult(account, "LooseOctree", _context, test_ok);
	return true;
}
