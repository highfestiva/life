/*
	Class:  LooseBinTree
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

TEMPLATE QUAL::LooseBinTree(_TObject pErrorObject,				
							_TVarType pTotalTreeSize,
							_TVarType pMinimumCellSize,
							_TVarType pK) :
	mErrorObject(pErrorObject)
{
	// Find required root node size.
	_TVarType lPowOf2 = 2;
	mMaxTreeDepth = 1;
	while (pMinimumCellSize * lPowOf2 < pTotalTreeSize)
	{
		mMaxTreeDepth++;
		lPowOf2 *= 2;
	}

	mK = pK;
	_TVarType lFixedSizeHalf = pMinimumCellSize * lPowOf2 * 0.5;
	_TVarType lSizeHalf = lFixedSizeHalf * mK;

	mRootNode = new Node(0, 255, lFixedSizeHalf);

	// Tree center at 0.
	mRootNode->mPos = 0;
	mRootNode->mSizeHalf = lSizeHalf;

	mNumObjects = 0;
	mNumNodes = 1;
}

TEMPLATE QUAL::~LooseBinTree()
{
	delete mRootNode;
	typename NodeList::iterator lIter;
	for (lIter = mRecycledNodeList.begin(); lIter != mRecycledNodeList.end(); ++lIter)
	{
		delete (*lIter);
	}
}

TEMPLATE void QUAL::InsertObject(_TKey pKey, _TObject pObject, _TVarType pPos, _TVarType pSizeHalf)
{
	if (mNodeTable.Find(pKey) != mNodeTable.End())
	{
		mLog.AWarning("Object already inserted.");
		return;
	}

	typename Node::Entry lEntry;
	lEntry.mSizeHalf = pSizeHalf;
	lEntry.mPos      = pPos;
	lEntry.mObject   = pObject;

	InsertObject(pKey, lEntry, mRootNode, 0);
}

TEMPLATE void QUAL::InsertObject(_TKey pKey, typename Node::Entry pEntry, Node* pNode, unsigned pDepth)
{
	// Calculate the position, size and index values of the child node.
	_TVarType lChildNodeSizeHalf = pNode->GetSizeHalf() * 0.5;

	_TVarType lChildNodePos;
	uint8 lChildIndex = GetChild(pEntry.mPos, pNode, lChildNodePos);

	// Check if the object fits in the child node or not.
	if (	!pEntry.IsEnclosed(lChildNodePos, lChildNodeSizeHalf) ||
		(pDepth == mMaxTreeDepth))
	{
		// The object doesn't fit in the child node, so put it in the current node...
		pNode->mEntryTable.Insert(pKey, pEntry);
		pNode->mObjectCount++;
		mNodeTable.Insert(pKey, pNode);
		mNumObjects++;
	}
	else
	{
		// Insert object in the child node
		if (pNode->mChildren[lChildIndex] == 0)
		{
			Node* lNewChildNode = NewNode(pNode, lChildIndex, pNode->mFixedSizeHalf * 0.5);
			lNewChildNode->mPos = lChildNodePos;
			lNewChildNode->mSizeHalf = lChildNodeSizeHalf;
			pNode->mChildMask |= (1 << lChildIndex);
			mNumNodes++;
			pNode->mChildren[lChildIndex] = lNewChildNode;
		}

		InsertObject(pKey, pEntry, pNode->mChildren[lChildIndex], pDepth + 1);
	}
}

TEMPLATE _TObject QUAL::RemoveObject(_TKey pKey)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		return mErrorObject;
	}

	return RemoveObject(pKey, lNodeIter).mObject;
}

TEMPLATE typename QUAL::Node::Entry QUAL::RemoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter)
{
	// Removes object from the octree and deletes the node if empty.
	Node* lCurrentNode = *pNodeIter;

	typename Node::EntryTable::Iterator lIter = FindObject(pKey, lCurrentNode);
	typename Node::Entry lEntry = *lIter;

	lCurrentNode->mEntryTable.Remove(lIter);
	mNodeTable.Remove(pNodeIter);
	mNumObjects--;

	while (lCurrentNode != 0 && lCurrentNode->IsEmpty() == true)
	{
		Node* lParent = lCurrentNode->mParent;

		if (lParent != 0)
		{
			RecycleNode(lParent->mChildren[lCurrentNode->mIndex]);
			lParent->mChildren[lCurrentNode->mIndex] = 0;
			
			lParent->mChildMask &= (0xFFFFFFFF ^ (1 << lCurrentNode->mIndex));
			mNumNodes--;
		}

		lCurrentNode = lParent;
	}

	return lEntry;
}


TEMPLATE _TObject QUAL::FindObject(_TKey pKey) const
{
	typename NodeTable::ConstIterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		return mErrorObject;
	}

	return (*FindObject(pKey, *lNodeIter)).mObject;
}

TEMPLATE typename QUAL::Node::EntryTable::Iterator QUAL::FindObject(_TKey pKey, Node* pObjectNode) const
{
	return pObjectNode->mEntryTable.Find(pKey);
}


TEMPLATE bool QUAL::MoveObject(_TKey pKey, _TVarType pNewPos, _TVarType pNewSizeHalf)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		mLog.AWarning("Trying to move non existing object.");
		return false;
	}
	Node* lNode = *lNodeIter;

	typename Node::EntryTable::Iterator lEntryIter = FindObject(pKey, lNode);
	(*lEntryIter).mPos      = pNewPos;
	(*lEntryIter).mSizeHalf = pNewSizeHalf;
	
	MoveObject(pKey, lNodeIter, lEntryIter);

	return true;
}

TEMPLATE _TObject QUAL::MoveObject(_TKey pKey, _TVarType pNewPos)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		mLog.AWarning("Trying to move non existing object.");
		return false;
	}
	Node* lNode = *lNodeIter;

	typename Node::EntryTable::Iterator lEntryIter = FindObject(pKey, lNode);
	(*lEntryIter).mPos = pNewPos;

	return MoveObject(pKey, lNodeIter, lEntryIter);
}

TEMPLATE _TObject QUAL::MoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter,
	typename Node::EntryTable::Iterator& /*pObjectIter*/)
{
	typename Node::Entry lEntry = RemoveObject(pKey, pNodeIter);
	InsertObject(pKey, lEntry, mRootNode, 0);
	return lEntry.mObject;
}

TEMPLATE bool QUAL::GetObjectSizeAndPos(_TKey pKey, _TVarType& pPos, _TVarType& pSizeHalf)
{
	bool lOk = true;

	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		mLog.AWarning("Trying to fetch non existing object.");
		lOk = false;
	}

	typename Node::EntryTable::Iterator lEntryIter;
	if (lOk)
	{
		Node* lNode = *lNodeIter;
		lEntryIter = FindObject(pKey, lNode);
		lOk = (lEntryIter != lNode->mEntryTable.End());
	}

	if (lOk)
	{
		pPos = (*lEntryIter).mPos;
		pSizeHalf = (*lEntryIter).mSizeHalf;
	}

	return lOk;
}


TEMPLATE unsigned QUAL::GetOverlaps(_TVarType pPosRelParent, 
				    _TVarType pSizeHalf, 
				    _TVarType pChildNodeSize,
				    _TVarType pParentNodeSize) const
{
	// Suppose that we overlap all nodes.
	unsigned lOverlapMask = 3;
	_TVarType lMinSeparation = pSizeHalf + pParentNodeSize;

	// Test the right child.
	if ( ( pPosRelParent  - pChildNodeSize ) > lMinSeparation ||
		( pChildNodeSize - pPosRelParent  ) > lMinSeparation)
	{
		// Remove the right child.
		lOverlapMask &= ~2;
	}
	// Test the left portion of the children.
	else if( ( pPosRelParent  + pChildNodeSize) > lMinSeparation ||
		    -( pChildNodeSize + pPosRelParent ) > lMinSeparation)
	{
		// Remove the left child.
		lOverlapMask &= ~1;
	}

	return lOverlapMask;
}

TEMPLATE uint8 QUAL::GetChild(_TVarType pPos, const Node* pNode)
{
	return (pPos > pNode->mPos) ? 1 : 0;
}

TEMPLATE uint8 QUAL::GetChild(_TVarType pPos, const Node* pNode, _TVarType& pChildPos)
{
	_TVarType lChildUnloosSizeHalf = pNode->mFixedSizeHalf * 0.5f;

	if (pPos > pNode->mPos)
	{
		pChildPos = pNode->mPos + lChildUnloosSizeHalf;
		return 1;
	}
	else
	{
		pChildPos = pNode->mPos - lChildUnloosSizeHalf;
		return 0;
	}
}


TEMPLATE void QUAL::GetObjects(ObjectList& pObjects, _TVarType pPos, _TVarType pSizeHalf)
{
	GetObjects(pObjects, pPos, pSizeHalf, mRootNode);
}

TEMPLATE void QUAL::GetObjects(ObjectList& pObjects, _TVarType pPos, _TVarType pSizeHalf, Node* pNode)
{
	typename Node::EntryTable::Iterator lIter;
	for (lIter  = pNode->mEntryTable.First(); 
		lIter != pNode->mEntryTable.End(); 
		++lIter)
	{
		const typename Node::Entry& lEntry = *lIter;

		if (lEntry.IsOverlapping(pPos, pSizeHalf) == true)
		{
			pObjects.push_back(lEntry.mObject);
		}
	}

	unsigned lOverlapMask = GetOverlaps(pPos - pNode->mPos, 
					       pSizeHalf, 
					       pNode->mSizeHalf * 0.5f, 
					       pNode->mSizeHalf);
	lOverlapMask &= pNode->mChildMask;

	if (lOverlapMask != 0)
	{
		if ((lOverlapMask & 1) != 0)
		{
			GetObjects(pObjects, pPos, pSizeHalf, pNode->mChildren[0]);
		}
		if ((lOverlapMask & 2) != 0)
		{
			GetObjects(pObjects, pPos, pSizeHalf, pNode->mChildren[1]);
		}
	}
}


TEMPLATE unsigned QUAL::GetFullTreeMemSize() const
{
	unsigned lNumNodes = 0;
	int i;

	for (i = 1; i <= mMaxTreeDepth; i++)
	{
		lNumNodes += (unsigned)(1 << i);
	}

	return lNumNodes * sizeof(Node);
}

TEMPLATE void QUAL::RecycleNode(Node* pNode)
{
	if (mRecycledNodeList.size() < MAX_RECYCLED_NODES)
	{
		pNode->DeleteChildren(this);
		mRecycledNodeList.push_back(pNode);
	}
	else
	{
		delete pNode;
	}
}

TEMPLATE typename QUAL::Node* QUAL::NewNode(Node* pParent, uint8 pIndex, _TVarType pFixedSizeHalf)
{
	Node* lNode = 0;

	if (mRecycledNodeList.empty())
	{
		lNode = new Node(pParent, pIndex, pFixedSizeHalf);
	}
	else
	{
		lNode = mRecycledNodeList.front();
		mRecycledNodeList.pop_front();
		lNode->Init(pParent, pIndex, pFixedSizeHalf);
	}
	return lNode;
}

TEMPLATE LogDecorator LooseBinTree<_TKey, _TObject, _TVarType, _THashFunc>::mLog(LogType::GetLog(LogType::SUB_GENERAL), typeid(LooseBinTree));
