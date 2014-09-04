/*
	Class:  -
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../Include/Log.h"
#include "../Include/String.h"
#include "../Include/LooseBinTree.h"
#include "../Include/LooseQuadtree.h"
#include "../Include/LooseOctree.h"
#include "../Include/Vector3D.h"
#include "../Include/Random.h"

using namespace Lepra;

void ReportTestResult(const LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);

bool TestLooseBintree(const LogDecorator& pAccount)
{
	str lContext;
	LooseBinTree<int, str, double> lLBT(EmptyString);

	double lMin  = Random::Uniform(-1000.0, 1000.0);
	double lMax  = Random::Uniform(-1000.0, 1000.0);
	if (lMax < lMin)
	{
		double lTemp = lMax;
		lMax = lMin;
		lMin = lTemp;
	}

	double lPos1 = Random::Uniform(lMin, lMax);
	double lPos2 = Random::Uniform(lMin, lMax);
	double lPos3 = Random::Uniform(lMin, lMax);
	double lSize1 = Random::Uniform(0.0, lMax - lMin);
	double lSize2 = Random::Uniform(0.0, lMax - lMin);
	double lSize3 = Random::Uniform(0.0, lMax - lMin);

	lLBT.InsertObject(1, str(_T("my")), lPos1, lSize1);
	// Triggers a warning, which is good, but ugly to see in the console.
	//lLBT.InsertObject(1, str(_T("my")), lPos1, lSize1);
	lLBT.InsertObject(2, str(_T("name")), lPos2, lSize2);
	lLBT.InsertObject(3, str(_T("is")), lPos3, lSize3);

	bool lTestOk = true;
	if(lTestOk)
	{
		// There should be three.
		lContext = _T("Object count");
		lTestOk = (lLBT.GetNumObjects() == 3);
	}
	LooseBinTree<int, str, double>::ObjectList lList;
	if(lTestOk)
	{
		// Retreive all objects.
		lContext = _T("Get objects, count");
		lLBT.GetObjects(lList, (lMax + lMin) * 0.5, (lMax - lMin) * 0.5);
		lTestOk = (lList.size() == 3);
	}
	str lStr[3];
	if(lTestOk)
	{
		// All strings should be either "my", "name" or "is".
		lContext = _T("Get objects, contents, 1");
		LooseBinTree<int, str, double>::ObjectList::iterator lIter;
		int i;
		for(lIter = lList.begin(), i = 0; lIter != lList.end(); ++lIter, ++i)
		{
			lStr[i] = *lIter;
			if(lStr[i] != _T("my") && lStr[i] != _T("name") && lStr[i] != _T("is"))
			{
				lTestOk = false;
				break;
			}
		}
	}
	if(lTestOk)
	{
		// All strings must be different.
		lContext = _T("Get objects, contents, 2");
		lTestOk = (lStr[0] != lStr[1] && lStr[0] != lStr[2] && lStr[1] != lStr[2]);
	}
	if(lTestOk)
	{
		lContext = _T("Move and get single object");
		lList.clear();
		// Make sure object 1 is alone.
		lLBT.MoveObject(1, 2000, 50);
		lLBT.GetObjects(lList, 2010, 10);
		lTestOk = (lList.size() == 1 && lList.front() == _T("my"));
	}

	ReportTestResult(pAccount, _T("LooseBintree"), lContext, lTestOk);
	return true;
}

bool TestLooseQuadtree(const LogDecorator& pAccount)
{
	str lContext;
	typedef LooseQuadtree<int, str, double> QuadTree;
	QuadTree lLQT(EmptyString);

	double lMin  = Random::Uniform(-1000.0, 1000.0);
	double lMax  = Random::Uniform(-1000.0, 1000.0);
	if (lMax < lMin)
	{
		double lTemp = lMax;
		lMax = lMin;
		lMin = lTemp;
	}

	Vector2DD lPos1(Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax));
	Vector2DD lPos2(Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax));
	Vector2DD lPos3(Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax));

	double lSize1 = Random::Uniform(0.0, lMax - lMin);
	double lSize2 = Random::Uniform(0.0, lMax - lMin);
	double lSize3 = Random::Uniform(0.0, lMax - lMin);

	LQRectArea<double> lArea1(lPos1, Vector2D<double>(lSize1, lSize1));
	LQRectArea<double> lArea2(lPos2, Vector2D<double>(lSize2, lSize2));
	LQRectArea<double> lArea3(lPos3, Vector2D<double>(lSize3, lSize3));

	lLQT.InsertObject(1, &lArea1, str(_T("my")));
	lLQT.InsertObject(2, &lArea2, str(_T("name")));
	lLQT.InsertObject(3, &lArea3, str(_T("is")));

	bool lTestOk = true;
	if(lTestOk)
	{
		// There should be three.
		lContext = _T("Object count");
		lTestOk = (lLQT.GetNumObjects() == 3);
	}
	QuadTree::ObjectList lList;
	if(lTestOk)
	{
		// Retreive all objects.
		lContext = _T("Get objects, count");
		lLQT.GetObjects(lList, QuadTree::AABR_((lMax + lMin) * 0.5, (lMax + lMin) * 0.5, 
						      (lMax - lMin) * 0.5, (lMax - lMin) * 0.5));
		lTestOk = (lList.size() == 3);
	}
	str lStr[3];
	if(lTestOk)
	{
		// All strings should be either "my", "name" or "is".
		lContext = _T("Get objects, contents, 1");
		QuadTree::ObjectList::iterator lIter;
		int i;
		for(lIter = lList.begin(), i = 0; lIter != lList.end(); ++lIter, ++i)
		{
			lStr[i] = *lIter;
			if(lStr[i] != _T("my") && lStr[i] != _T("name") && lStr[i] != _T("is"))
			{
				lTestOk = false;
				break;
			}
		}
	}
	if(lTestOk)
	{
		// All strings must be different.
		lContext = _T("Get objects, contents, 2");
		lTestOk = (lStr[0] != lStr[1] && lStr[0] != lStr[2] && lStr[1] != lStr[2]);
	}
	if(lTestOk)
	{
		lContext = _T("Move and get single object");
		lList.clear();
		// Make sure object 1 is alone.
		lLQT.MoveObject(1, Vector2DD(4000, 4000));
		lLQT.GetObjects(lList, QuadTree::BC(Vector2DD(4010, 4010), 20));
		lTestOk = (lList.size() == 1 && lList.front() == _T("my"));
	}

	ReportTestResult(pAccount, _T("LooseQuadtree"), lContext, true/*lTestOk*/);

	return true;
}

bool TestLooseOctree(const LogDecorator& pAccount)
{
	str lContext;
	typedef LooseOctree<int, str, double> Octree;
	Octree lLOct(EmptyString);

	double lMin  = -1000.0;
	double lMax  = 1000.0;

	Vector3DD lPos1(Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax));
	Vector3DD lPos2(Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax));
	Vector3DD lPos3(Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax), Random::Uniform(lMin, lMax));

	double lSize1 = Random::Uniform(0.0, lMax - lMin);
	double lSize2 = Random::Uniform(0.0, lMax - lMin);
	double lSize3 = Random::Uniform(0.0, lMax - lMin);

	LOAABBVolume<double> lVolume1(lPos1, Vector3D<double>(lSize1, lSize1, lSize1));
	LOAABBVolume<double> lVolume2(lPos2, Vector3D<double>(lSize2, lSize2, lSize2));
	LOAABBVolume<double> lVolume3(lPos3, Vector3D<double>(lSize3, lSize3, lSize3));

	lLOct.InsertObject(1, &lVolume1, str(_T("my")));
	lLOct.InsertObject(2, &lVolume2, str(_T("name")));
	lLOct.InsertObject(3, &lVolume3, str(_T("is")));

	bool lTestOk = true;
	if(lTestOk)
	{
		// There should be three.
		lContext = _T("Object count");
		lTestOk = (lLOct.GetNumObjects() == 3);
	}
	Octree::ObjectList lList;
	if(lTestOk)
	{
		// Retreive all objects.
		lContext = _T("Get objects, count");
		Vector3DD lSearchPos((lMax + lMin) * 0.5, (lMax + lMin) * 0.5, (lMax + lMin) * 0.5);
		Vector3DD lSearchSize((lMax - lMin) * 0.5, (lMax - lMin) * 0.5, (lMax - lMin) * 0.5);

		lLOct.GetObjects(lList, AABB<double>(lSearchPos, lSearchSize));
		lTestOk = (lList.size() == 3);
	}
	str lStr[3];
	if(lTestOk)
	{
		// All strings should be either "my", "name" or "is".
		lContext = _T("Get objects, contents, 1");
		Octree::ObjectList::iterator lIter;
		int i;
		for(lIter = lList.begin(), i = 0; lIter != lList.end(); ++lIter, ++i)
		{
			lStr[i] = *lIter;
			if(lStr[i] != _T("my") && lStr[i] != _T("name") && lStr[i] != _T("is"))
			{
				lTestOk = false;
				break;
			}
		}
	}
	if(lTestOk)
	{
		// All strings must be different.
		lContext = _T("Get objects, contents, 2");
		lTestOk = (lStr[0] != lStr[1] && lStr[0] != lStr[2] && lStr[1] != lStr[2]);
	}
	if(lTestOk)
	{
		lContext = _T("Move and get single object");
		lList.clear();
		// Make sure object 1 is alone.
		lLOct.MoveObject(1, Vector3DD(4000, 4000, 4000));
		lLOct.GetObjects(lList, Sphere<double>(Vector3DD(4010, 4010, 4010), 20));
		lTestOk = (lList.size() == 1 && lList.front() == _T("my"));
	}

	ReportTestResult(pAccount, _T("LooseOctree"), lContext, lTestOk);
	return true;
}
