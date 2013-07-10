
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



LQ_TEMPLATE LQ_QUAL::LooseQuadtree(_TObject pErrorObject, _TVarType pTotalTreeSize,_TVarType pMinimumCellSize, _TVarType pK):
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
	_TVarType lFixedSizeHalf = pMinimumCellSize * lPowOf2 * (_TVarType)0.5;
	_TVarType lSizeHalf = lFixedSizeHalf * mK;

	mRootNode = new Node(0, 255, lFixedSizeHalf);

	// Tree center at 0,0,0.
	Vector2D<_TVarType> lSizeHalfVec(lSizeHalf, lSizeHalf);
	mRootNode->mNodeBox = AABR_(Vector2D<_TVarType>(0, 0), lSizeHalfVec);

	mNumObjects = 0;
	mNumNodes = 1;
}




LQ_TEMPLATE LQ_QUAL::~LooseQuadtree()
{
	delete mRootNode;
	typename NodeList::iterator lIter;
	for (lIter = mRecycledNodeList.begin(); lIter != mRecycledNodeList.end(); ++lIter)
	{
		delete (*lIter);
	}
}




LQ_TEMPLATE void LQ_QUAL::InsertObject(_TKey pKey, LQArea<_TVarType>* pArea, _TObject pObject)
{
	if (mNodeTable.Find(pKey) != mNodeTable.End())
	{
		//LOG(PHYSIC, ERROR, "LooseQuadtree::InsertObject, object already inserted!");
		return;
	}

	InsertObject(pKey, typename Node::Entry(pArea, pObject), mRootNode, 0);
}




LQ_TEMPLATE void LQ_QUAL::InsertObject(_TKey pKey, typename Node::Entry pEntry, Node* pNode, uint16 pDepth)
{
	// Calculate the position, size and index values of the child node.
	_TVarType lChildNodeSizeHalf = pNode->GetSizeHalf() * 0.5;

	Vector2D<_TVarType> lChildNodePos;
	uint8 lChildIndex = GetChild(pEntry.mArea->GetPosition(), pNode, lChildNodePos);

	// Check if the object fits in the child node or not.
	if (	!pEntry.mArea->IsAABSQEnclosingArea(lChildNodePos, lChildNodeSizeHalf) ||
		(pDepth == mMaxTreeDepth))
	{
		//MATH_ASSERT((pNode->mEntryMap.Find(pKey) == pNode->mEntryMap.End()), "LooseQuadtree::InsertObject: pNode->mEntryMap.Find(pKey) == pNode->mEntryMap.End()");
		//MATH_ASSERT((mNodeTable.Find(pKey) == mNodeTable.End()), "LooseQuadtree::InsertObject: mNodeTable.Find(pKey) == mNodeTable.End()");

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
			Node* lNewChildNode = NewNode(pNode, lChildIndex, pNode->GetFixedSizeHalf() * 0.5);
			lNewChildNode->mNodeBox.SetPosition(lChildNodePos);
			lNewChildNode->mNodeBox.SetSize(Vector2D<_TVarType>(lChildNodeSizeHalf, lChildNodeSizeHalf));
			pNode->mChildMask |= (1 << lChildIndex);
			mNumNodes++;
			pNode->mChildren[lChildIndex] = lNewChildNode;
		}

		InsertObject(pKey, pEntry, pNode->mChildren[lChildIndex], pDepth + 1);
	}
}



LQ_TEMPLATE _TObject LQ_QUAL::RemoveObject(_TKey pKey)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		return mErrorObject;
	}

	return RemoveObject(pKey, lNodeIter).mObject;
}




LQ_TEMPLATE typename LQ_QUAL::Node::Entry LQ_QUAL::RemoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter)
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




LQ_TEMPLATE _TObject LQ_QUAL::FindObject(_TKey pKey) const
{
	typename NodeTable::ConstIterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		return mErrorObject;
	}

	return (*FindObject(pKey, *lNodeIter)).mObject;
}




LQ_TEMPLATE inline typename LQ_QUAL::Node::EntryTable::Iterator LQ_QUAL::FindObject(_TKey pKey, Node* pObjectNode) const
{
	//MATH_ASSERT((pObjectNode != NULL), "LooseQuadtree::FindObject: pObjectNode != NULL");
	//MATH_ASSERT((pObjectNode->mEntryTable.Find(pKey) != pObjectNode->mEntryTable.End()), "LooseQuadtree::FindObject: pObjectNode->mEntryTable.Find(pKey) != pObjectNode->mEntryTable.End()");

	return pObjectNode->mEntryTable.Find(pKey);
}




LQ_TEMPLATE bool LQ_QUAL::MoveObject(_TKey pKey, LQArea<_TVarType>* pNewArea)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		//LOG(PHYSIC, DEBUG, "LooseQuadtree::MoveObject, trying to move non existing object!");
		return false;
	}
	Node* lNode = *lNodeIter;

	typename Node::EntryTable::Iterator lEntryIter = FindObject(pKey, lNode);
	(*lEntryIter).mArea = pNewArea;
	
	MoveObject(pKey, lNodeIter, lEntryIter);

	return true;
}




LQ_TEMPLATE _TObject LQ_QUAL::MoveObject(_TKey pKey, const Vector2D<_TVarType>& pToPos)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		//LOG(PHYSIC, DEBUG, "LooseQuadtree::MoveObject, trying to move non existing object!");
		return false;
	}
	Node* lNode = *lNodeIter;

	typename Node::EntryTable::Iterator lEntryIter = FindObject(pKey, lNode);
	(*lEntryIter).mArea->SetPosition(pToPos);

	return MoveObject(pKey, lNodeIter);
}

LQ_TEMPLATE _TObject LQ_QUAL::MoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter)
{
	typename Node::Entry lEntry = RemoveObject(pKey, pNodeIter);
	InsertObject(pKey, lEntry, mRootNode, 0);
	return lEntry.mObject;
}



LQ_TEMPLATE unsigned LQ_QUAL::GetOverlaps(const Vector2D<_TVarType>& pPosRelParent, 
				    _TVarType pBoundingRadius, 
				    _TVarType pChildNodeSize,
				    _TVarType pParentNodeSize) const
{
	// Suppose that we overlap all nodes.
	unsigned lOverlapMask	= 0xFFFFFFFF;
	_TVarType lMinBoxSeparation = pBoundingRadius + pParentNodeSize;

	// Test the right portion of the children.
	if ( ( pPosRelParent.x - pChildNodeSize ) > lMinBoxSeparation ||
	    ( pChildNodeSize - pPosRelParent.x ) > lMinBoxSeparation)
	{
		// Remove the two rightmost children.
		lOverlapMask &= ~(4 + 8);
		MACRO_LO_GO1_TEST_TOP_AND_BOTTOM
	}
	// Test the left portion of the children.
	else if( ( pPosRelParent.x + pChildNodeSize ) > lMinBoxSeparation ||
		-( pChildNodeSize + pPosRelParent.x ) > lMinBoxSeparation)
	{
		// Remove the two leftmost children.
		lOverlapMask &= ~(1 + 2);
		MACRO_LO_GO1_TEST_TOP_AND_BOTTOM
	}

	return lOverlapMask;
}




LQ_TEMPLATE unsigned LQ_QUAL::GetOverlaps( const Vector2D<_TVarType>& pPosRelParent, 
				     _TVarType pSizeX, 
				     _TVarType pSizeY, 
				     _TVarType pChildNodeSize,
				     _TVarType pParentNodeSize) const
{
	// Suppose that we overlap all nodes.
	unsigned lOverlapMask	= 0xFFFFFFFF;
	_TVarType lMinBoxSeparationX = pSizeX + pParentNodeSize;
	_TVarType lMinBoxSeparationY = pSizeY + pParentNodeSize;

	// Test the right portion of the children.
	if ( ( pPosRelParent.x - pChildNodeSize ) > lMinBoxSeparationX ||
		( pChildNodeSize - pPosRelParent.x ) > lMinBoxSeparationX)
	{
		// Remove the two rightmost children.
		lOverlapMask &= ~(4 + 8);
		MACRO_LO_GO2_TEST_TOP_AND_BOTTOM
	}
	else if( ( pPosRelParent.x + pChildNodeSize ) > lMinBoxSeparationX ||
		    -( pChildNodeSize + pPosRelParent.x ) > lMinBoxSeparationX)
	{
		// Remove the two leftmost children.
		lOverlapMask &= ~(1 + 2);
		MACRO_LO_GO2_TEST_TOP_AND_BOTTOM
	}

	return lOverlapMask;
}




LQ_TEMPLATE uint8 LQ_QUAL::GetChild(const Vector2D<_TVarType>& pPos, const Node* pNode)
{
	uint8 pIndex = 0;

	if ( pPos.x > pNode->GetPosition().x )
	{
		pIndex |= 2;
	}

	if ( pPos.y > pNode->GetPosition().y )
	{
		pIndex |= 1;
	}

	return pIndex;
}




LQ_TEMPLATE uint8 LQ_QUAL::GetChild(const Vector2D<_TVarType>& pPos, const Node* pNode, Vector2D<_TVarType>& pChildPos)
{
	_TVarType lChildUnloosSizeHalf = pNode->GetFixedSizeHalf() * 0.5f;

	uint8 pIndex = 0;

	if ( pPos.x > pNode->GetPosition().x )
	{
		pChildPos.x = pNode->GetPosition().x + lChildUnloosSizeHalf;
		pIndex |= 2;
	}
	else
	{
		pChildPos.x = pNode->GetPosition().x - lChildUnloosSizeHalf;
	}

	if ( pPos.y > pNode->GetPosition().y )
	{
		pChildPos.y = pNode->GetPosition().y + lChildUnloosSizeHalf;
		pIndex |= 1;
	}
	else
	{
		pChildPos.y = pNode->GetPosition().y - lChildUnloosSizeHalf;
	}

	return pIndex;
}








LQ_TEMPLATE void LQ_QUAL::GetObjects(ObjectList& pObjects, const BC& pBC)
{
	GetObjectsInBC(mRootNode, pObjects, pBC);
}

LQ_TEMPLATE void LQ_QUAL::GetObjects(ObjectList& pObjects, const AABR_& pAABR)
{
	GetObjectsInAABR(mRootNode, pObjects, pAABR);
}

LQ_TEMPLATE void LQ_QUAL::GetObjectsInBC(Node* pCurrentNode, ObjectList& pObjects, const BC& pBC)
{
	// Insert objects at this node into list.
	typename Node::EntryTable::Iterator lIter;
	for (lIter  = pCurrentNode->mEntryTable.First(); 
		lIter != pCurrentNode->mEntryTable.End(); 
		++lIter)
	{
		const typename Node::Entry& lEntry = *lIter;

		if (lEntry.mArea->IsBCOverlappingArea(pBC))
		{
			pObjects.push_back(lEntry.mObject);
		}
	}

	unsigned lOverlapMask = GetOverlaps(pBC.GetPosition() - pCurrentNode->GetPosition(), 
					       pBC.GetRadius(), 
					       pCurrentNode->GetSizeHalf() * 0.5f, 
					       pCurrentNode->GetSizeHalf());

	lOverlapMask &= pCurrentNode->mChildMask;

	if (lOverlapMask != 0)
	{
		if ((lOverlapMask & 1) != 0)
		{
			GetObjectsInBC(pCurrentNode->mChildren[0], pObjects, pBC);
		}
		if ((lOverlapMask & 2) != 0)
		{
			GetObjectsInBC(pCurrentNode->mChildren[1], pObjects, pBC);
		}
		if ((lOverlapMask & 4) != 0)
		{
			GetObjectsInBC(pCurrentNode->mChildren[2], pObjects, pBC);
		}
		if ((lOverlapMask & 8) != 0)
		{
			GetObjectsInBC(pCurrentNode->mChildren[3], pObjects, pBC);
		}
	}
}




LQ_TEMPLATE void LQ_QUAL::GetObjectsInAABR(Node* pCurrentNode, ObjectList& pObjects, const AABR_& pAABR)
{
	typename Node::EntryTable::Iterator lIter;
	for (lIter = pCurrentNode->mEntryTable.First(); 
		lIter != pCurrentNode->mEntryTable.End(); 
		++lIter)
	{
		const typename Node::Entry& lEntry = *lIter;

		if (lEntry.mArea->IsAABROverlappingArea(pAABR))
		{
			pObjects.push_back(lEntry.mObject);
		}
	}

	unsigned lOverlapMask = GetOverlaps(	pAABR.GetPosition() - pCurrentNode->GetPosition(),
						pAABR.GetSize().x,
						pAABR.GetSize().y,
						pCurrentNode->GetSizeHalf() * 0.5f,
						pCurrentNode->GetSizeHalf());
	lOverlapMask &= pCurrentNode->mChildMask;

	if (lOverlapMask != 0)
	{
		if ((lOverlapMask & 1) != 0)
		{
			GetObjectsInAABR(pCurrentNode->mChildren[0], pObjects, pAABR);
		}
		if ((lOverlapMask & 2) != 0)
		{
			GetObjectsInAABR(pCurrentNode->mChildren[1], pObjects, pAABR);
		}
		if ((lOverlapMask & 4) != 0)
		{
			GetObjectsInAABR(pCurrentNode->mChildren[2], pObjects, pAABR);
		}
		if ((lOverlapMask & 8) != 0)
		{
			GetObjectsInAABR(pCurrentNode->mChildren[3], pObjects, pAABR);
		}
	}
}




LQ_TEMPLATE unsigned LQ_QUAL::GetFullTreeMemSize() const
{
	unsigned lNumNodes = 0;
	int i;

	for (i = 1; i <= mMaxTreeDepth; i++)
	{
		lNumNodes += (unsigned)pow(8.0f, i);
	}

	return lNumNodes * sizeof(Node);
}



/*
LQ_TEMPLATE str LQ_QUAL::ToString() const
{
	std::stringstream lStrm;
	mRootNode->ToString(lStrm, 0, -1);
	return lStrm.str();
}
*/



/*LQ_TEMPLATE void LQ_QUAL::GetNodeBoxes(AABRList& pBoxes) const
{
	mRootNode->GetNodeBox(pBoxes);
}*/




LQ_TEMPLATE void LQ_QUAL::RecycleNode(Node* pNode)
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

LQ_TEMPLATE typename LQ_QUAL::Node* LQ_QUAL::NewNode(Node* pParent, uint8 pIndex, _TVarType pFixedSizeHalf)
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
