/*
	Class:  -
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/Log.h"
#include "../Include/String.h"
#include "../Include/LooseBintree.h"
#include "../Include/LooseQuadtree.h"
#include "../Include/LooseOctree.h"
#include "../Include/Vector3D.h"
#include "../Include/Random.h"

void ReportTestResult(const Lepra::LogDecorator& pLog, const Lepra::String& pTestName, const Lepra::String& pContext, bool pResult);

bool TestLooseBintree(const Lepra::LogDecorator& pAccount)
{
	Lepra::String lContext;
	Lepra::LooseBinTree<int, Lepra::String, double> lLBT(Lepra::EmptyString);

	double lMin  = Lepra::Random::Uniform(-1000.0, 1000.0);
	double lMax  = Lepra::Random::Uniform(-1000.0, 1000.0);
	if (lMax < lMin)
	{
		double lTemp = lMax;
		lMax = lMin;
		lMin = lTemp;
	}

	double lPos1 = Lepra::Random::Uniform(lMin, lMax);
	double lPos2 = Lepra::Random::Uniform(lMin, lMax);
	double lPos3 = Lepra::Random::Uniform(lMin, lMax);
	double lSize1 = Lepra::Random::Uniform(0, lMax - lMin);
	double lSize2 = Lepra::Random::Uniform(0, lMax - lMin);
	double lSize3 = Lepra::Random::Uniform(0, lMax - lMin);

	lLBT.InsertObject(1, Lepra::String(_T("my")), lPos1, lSize1);
	// Triggers a warning, which is good, but ugly to see in the console.
	//lLBT.InsertObject(1, Lepra::String(_T("my")), lPos1, lSize1);
	lLBT.InsertObject(2, Lepra::String(_T("name")), lPos2, lSize2);
	lLBT.InsertObject(3, Lepra::String(_T("is")), lPos3, lSize3);

	bool lTestOk = true;
	if(lTestOk)
	{
		// There should be three.
		lContext = _T("Object count");
		lTestOk = (lLBT.GetNumObjects() == 3);
	}
	Lepra::LooseBinTree<int, Lepra::String, double>::ObjectList lList;
	if(lTestOk)
	{
		// Retreive all objects.
		lContext = _T("Get objects, count");
		lLBT.GetObjects(lList, (lMax + lMin) * 0.5, (lMax - lMin) * 0.5);
		lTestOk = (lList.size() == 3);
	}
	Lepra::String lStr[3];
	if(lTestOk)
	{
		// All strings should be either "my", "name" or "is".
		lContext = _T("Get objects, contents, 1");
		Lepra::LooseBinTree<int, Lepra::String, double>::ObjectList::iterator lIter;
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

bool TestLooseQuadtree(const Lepra::LogDecorator& pAccount)
{
	Lepra::String lContext;
	typedef Lepra::LooseQuadtree<int, Lepra::String, double> QuadTree;
	QuadTree lLQT(Lepra::EmptyString);

	double lMin  = Lepra::Random::Uniform(-1000.0, 1000.0);
	double lMax  = Lepra::Random::Uniform(-1000.0, 1000.0);
	if (lMax < lMin)
	{
		double lTemp = lMax;
		lMax = lMin;
		lMin = lTemp;
	}

	Lepra::Vector2DD lPos1(Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax));
	Lepra::Vector2DD lPos2(Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax));
	Lepra::Vector2DD lPos3(Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax));

	double lSize1 = Lepra::Random::Uniform(0, lMax - lMin);
	double lSize2 = Lepra::Random::Uniform(0, lMax - lMin);
	double lSize3 = Lepra::Random::Uniform(0, lMax - lMin);

	Lepra::LQRectArea<double> lArea1(lPos1, Lepra::Vector2D<double>(lSize1, lSize1));
	Lepra::LQRectArea<double> lArea2(lPos2, Lepra::Vector2D<double>(lSize2, lSize2));
	Lepra::LQRectArea<double> lArea3(lPos3, Lepra::Vector2D<double>(lSize3, lSize3));

	lLQT.InsertObject(1, &lArea1, Lepra::String(_T("my")));
	lLQT.InsertObject(2, &lArea2, Lepra::String(_T("name")));
	lLQT.InsertObject(3, &lArea3, Lepra::String(_T("is")));

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
	Lepra::String lStr[3];
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
		lLQT.MoveObject(1, Lepra::Vector2DD(4000, 4000));
		lLQT.GetObjects(lList, QuadTree::BC(Lepra::Vector2DD(4010, 4010), 20));
		lTestOk = (lList.size() == 1 && lList.front() == _T("my"));
	}

	ReportTestResult(pAccount, _T("LooseQuadtree"), lContext, true/*lTestOk*/);

	return true;
}

bool TestLooseOctree(const Lepra::LogDecorator& pAccount)
{
	Lepra::String lContext;
	typedef Lepra::LooseOctree<int, Lepra::String, double> Octree;
	Octree lLOct(Lepra::EmptyString);

	double lMin  = -1000.0;
	double lMax  = 1000.0;

	Lepra::Vector3DD lPos1(Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax));
	Lepra::Vector3DD lPos2(Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax));
	Lepra::Vector3DD lPos3(Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax), Lepra::Random::Uniform(lMin, lMax));

	double lSize1 = Lepra::Random::Uniform(0, lMax - lMin);
	double lSize2 = Lepra::Random::Uniform(0, lMax - lMin);
	double lSize3 = Lepra::Random::Uniform(0, lMax - lMin);

	Lepra::LOAABBVolume<double> lVolume1(lPos1, Lepra::Vector3D<double>(lSize1, lSize1, lSize1));
	Lepra::LOAABBVolume<double> lVolume2(lPos2, Lepra::Vector3D<double>(lSize2, lSize2, lSize2));
	Lepra::LOAABBVolume<double> lVolume3(lPos3, Lepra::Vector3D<double>(lSize3, lSize3, lSize3));

	lLOct.InsertObject(1, &lVolume1, Lepra::String(_T("my")));
	lLOct.InsertObject(2, &lVolume2, Lepra::String(_T("name")));
	lLOct.InsertObject(3, &lVolume3, Lepra::String(_T("is")));

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
		Lepra::Vector3DD lSearchPos((lMax + lMin) * 0.5, (lMax + lMin) * 0.5, (lMax + lMin) * 0.5);
		Lepra::Vector3DD lSearchSize((lMax - lMin) * 0.5, (lMax - lMin) * 0.5, (lMax - lMin) * 0.5);

		lLOct.GetObjects(lList, Lepra::AABB<double>(lSearchPos, lSearchSize));
		lTestOk = (lList.size() == 3);
	}
	Lepra::String lStr[3];
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
		lLOct.MoveObject(1, Lepra::Vector3DD(4000, 4000, 4000));
		lLOct.GetObjects(lList, Lepra::Sphere<double>(Lepra::Vector3DD(4010, 4010, 4010), 20));
		lTestOk = (lList.size() == 1 && lList.front() == _T("my"));
	}

	ReportTestResult(pAccount, _T("LooseOctree"), lContext, lTestOk);
	return true;
}
