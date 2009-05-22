/*
	Author: Jonas Byström
	Copyright (c) 2002-2006, Righteous Games

	*** Note that this class is not thread-safe! ***

	The PerformanceScope class is implemented to measure
	performance over a scope and it's underlying sub-scopes.
	The measured time data is stored in a tree. To add
	measurement of a scope, use the following syntax:

	{
		LEPRA_PERFORMANCE_SCOPE("MyTestScope");
		...
	}

	LEPRA_PERFORMANCE_SCOPE may be replaced by
	LEPRA_INFO_PERFORMANCE_SCOPE if the time is not to be
	measured in the final release (only measured in debug
	and release candidate versions).

	The stored frequency of the time is unspecified and may
	vary from implementation to implementation. However,
	the important thing, which is the distribution between
	the scopes, remain the same.
*/



#ifndef PERFORMANCESCOPE_H
#define PERFORMANCESCOPE_H



#include "String.h"
#include "../Include/SystemManager.h"



namespace Lepra
{



class PerformanceScope
{
public:
	class Node
	{
	public:
		inline Node(const AnsiString& pName, size_t pNameHash, Node* pParent, Node* pNext);
		~Node();

		// Subtracts the time spent in children with the time of this node.
		uint64 GetTimeInSelf() const;

		const AnsiString mName;
		size_t mNameHash;
		unsigned mHits;
		uint64 mTime;
		Node* mParent;
		Node* mNext;
		Node* mChildren[4];

	private:
		void operator=(const Node&);
	};

	inline PerformanceScope(const AnsiString& pName);
	inline ~PerformanceScope();

	static Node* GetRootNode();
	static void UpdateRootTime();
	static void Clear(Node* pNode = &smRoot);
	static void ResetTime(Node* pNode = smCurrentNode);

	static void Log(const LogDecorator& pLog);

private:
	static void LogNode(const LogDecorator& pLog, PerformanceScope::Node* pNode, int pDepth);

	uint64 mStartTime;

	static Node smRoot;
	static Node* smCurrentNode;
};

inline PerformanceScope::Node::Node(const AnsiString& pName, size_t pNameHash, Node* pParent, Node* pNext):
	mName(pName),
	mNameHash(pNameHash),
	mHits(1),
	mTime(0),
	mParent(pParent),
	mNext(pNext)
{
	mChildren[0] = 0;
	mChildren[1] = 0;
	mChildren[2] = 0;
	mChildren[3] = 0;
}

inline PerformanceScope::PerformanceScope(const AnsiString& pName)
{
	mStartTime = SystemManager::GetCpuTick();

	std::LEPRA_STD_HASHER<const char*> lHasher;
	size_t lNameHash = lHasher.operator()(pName.c_str());
	Node* lNode;
	for (lNode = smCurrentNode->mChildren[lNameHash&3]; lNode && lNode->mNameHash != lNameHash; lNode = lNode->mNext)
		;	// For loop does the searching.
	if (lNode)
	{
		++lNode->mHits;
		smCurrentNode = lNode;
	}
	else
	{
		lNode = new Node(pName, lNameHash, smCurrentNode, smCurrentNode->mChildren[lNameHash&3]);
		smCurrentNode->mChildren[lNameHash&3] = lNode;
		smCurrentNode = lNode;
	}
}

inline PerformanceScope::~PerformanceScope()
{
	smCurrentNode->mTime += SystemManager::GetCpuTick()-mStartTime;
	smCurrentNode = smCurrentNode->mParent;
}



#define LEPRA_PERFORMANCE_SCOPE(name)	static Lepra::AnsiString _lPerformanceName(name); Lepra::PerformanceScope _lPerformanceInstance(_lPerformanceName)

#ifdef LEPRA_INFO_PERFORMANCE
#define LEPRA_INFO_PERFORMANCE_SCOPE(name)	LEPRA_PERFORMANCE_SCOPE(name)
#else	// !LEPRA_INFO_PERFORMANCE
#define LEPRA_INFO_PERFORMANCE_SCOPE(name)
#endif	// LEPRA_INFO_PERFORMANCE/!LEPRA_INFO_PERFORMANCE



}



#endif // !PERFORMANCESCOPE_H
