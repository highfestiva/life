/*
	Class:  PerformanceScope
	Author: Jonas Byström
	Copyright (c) 2002-2006, Righteous Games
*/



#include <assert.h>
#include "../Include/PerformanceScope.h"



namespace Lepra
{



PerformanceScope::Node::~Node()
{
	if (mNext)
	{
		delete (mNext);
		mNext = 0;
	}
	for (unsigned x = 0; x < 4; ++x)
	{
		if (mChildren[x])
		{
			delete (mChildren[x]);
			mChildren[x] = 0;
		}
	}
}



uint64 PerformanceScope::Node::GetTimeInSelf() const
{
	uint64 lTimeInSelf = mTime;
	for (unsigned x = 0; x < 4; ++x)
	{
		for (Node* lChild = mChildren[x]; lChild; lChild = lChild->mNext)
		{
			lTimeInSelf -= lChild->mTime;
		}
	}
	return (lTimeInSelf);
}



void PerformanceScope::Node::operator=(const Node&)
{
	assert(false);
}



PerformanceScope::Node* PerformanceScope::GetRootNode()
{
	return (&smRoot);
}

void PerformanceScope::UpdateRootTime()
{
	uint64 lRootTime = 0;
	for (unsigned x = 0; x < 4; ++x)
	{
		for (Node* lChild = smRoot.mChildren[x]; lChild; lChild = lChild->mNext)
		{
			lRootTime += lChild->mTime;
		}
	}
	smRoot.mTime = lRootTime;
}

void PerformanceScope::Clear(Node* pNode)
{
	for (unsigned x = 0; x < 4; ++x)
	{
		if (pNode->mChildren[x])
		{
			Clear(pNode->mChildren[x]);
			pNode->mChildren[x] = 0;
		}
	}
	if (pNode->mNext)
	{
		Clear(pNode->mNext);
		pNode->mNext = 0;
	}
	if (pNode != &smRoot)
	{
		delete (pNode);
		if (pNode == smCurrentNode)
		{
			smCurrentNode = &smRoot;
		}
	}
}

void PerformanceScope::ResetTime(Node* pNode)
{
	pNode->mTime = 0;
	for (unsigned x = 0; x < 4; ++x)
	{
		if (pNode->mChildren[x])
		{
			ResetTime(pNode->mChildren[x]);
		}
	}
	if (pNode->mNext)
	{
		ResetTime(pNode->mNext);
	}
}

void PerformanceScope::Log(const LogDecorator& pLog)
{
	PerformanceScope::UpdateRootTime();
	LogNode(pLog, &smRoot, 0);
}



void PerformanceScope::LogNode(const LogDecorator& pLog, PerformanceScope::Node* pNode, int pDepth)
{
	if (pDepth > 0)
	{
		AnsiString lString;
		for (int x = pDepth-2; x >= 0; --x)
		{
			lString += "  |  ";
		}
		uint64 lSelf = pNode->GetTimeInSelf();
		uint64 lTotalTime = PerformanceScope::GetRootNode()->mTime;
		double lSelfPercent = ((double)(lSelf*100))/lTotalTime;
		double lSelfPercentPerHit = lSelfPercent/pNode->mHits;
		lString += AnsiStringUtility::Format("  +-- %s: self=%f %%, hits=%u, single=%f %%",
			pNode->mName.c_str(), lSelfPercent,
			pNode->mHits, lSelfPercentPerHit);
		pLog.Performance(AnsiStringUtility::ToCurrentCode(lString));
	}
	for (int x = 0; x < 4; ++x)
	{
		if (pNode->mChildren[x])
		{
			LogNode(pLog, pNode->mChildren[x], pDepth+1);
		}
	}
	if (pNode->mNext)
	{
		LogNode(pLog, pNode->mNext, pDepth);
	}
}

PerformanceScope::Node PerformanceScope::smRoot("<Root>", 0, 0, 0);
PerformanceScope::Node* PerformanceScope::smCurrentNode = &smRoot;



}
