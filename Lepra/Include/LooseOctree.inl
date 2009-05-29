/*
	Class:  LooseOctree
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

TEMPLATE QUAL::LooseOctree(	_TObject pErrorObject, 
				_TVarType pTotalTreeSize, 
				_TVarType pMinimumCellSize, 
				_TVarType pK) :
	mErrorObject(pErrorObject)
{
	// Find required root node size.
	_TVarType lPowOf2 = 2;
	mMaxTreeDepth = 1;
	while ( pMinimumCellSize * lPowOf2 < pTotalTreeSize )
	{
		mMaxTreeDepth++;
		lPowOf2 *= 2;
	}

	mK = pK;
	_TVarType lFixedSizeHalf = pMinimumCellSize * lPowOf2 * 0.5;
	_TVarType lSizeHalf = lFixedSizeHalf * mK;

	mRootNode = new Node(0, 255, lFixedSizeHalf);

	// Tree center at 0,0,0.
	Vector3D<_TVarType> lSizeHalfVec(lSizeHalf, lSizeHalf, lSizeHalf);
	mRootNode->mNodeBox = AABB<_TVarType>(Vector3D<_TVarType>(0, 0, 0), lSizeHalfVec);

	mNumObjects = 0;
	mNumNodes = 1;
}




TEMPLATE QUAL::~LooseOctree()
{
	delete mRootNode;
	typename NodeList::iterator lIter;
	for (lIter = mRecycledNodeList.begin(); lIter != mRecycledNodeList.end(); ++lIter)
	{
		delete (*lIter);
	}
}




TEMPLATE void QUAL::InsertObject(_TKey pKey, LOVolume<_TVarType>* pVolume, _TObject pObject)
{
	if (mNodeTable.Find(pKey) != mNodeTable.End())
	{
		//LOG(PHYSIC, ERROR, "LooseOctree::InsertObject, object already inserted!");
		return;
	}

	typename Node::Entry lEntry(pVolume, pObject);

	InsertObject(pKey, lEntry, mRootNode, 0);
}




TEMPLATE void QUAL::InsertObject(_TKey pKey, typename Node::Entry pEntry, Node* pNode, uint16 pDepth)
{
	// Calculate the position, size and index values of the child node.
	_TVarType lChildNodeSizeHalf = pNode->GetSizeHalf() * 0.5;

	Vector3D<_TVarType> lChildNodePos;
	uint8 lChildIndex = GetChild(pEntry.mVolume->GetPosition(), pNode, lChildNodePos);

	// Check if the object fits in the child node or not.
	if (	!pEntry.mVolume->IsAABCEnclosingVolume(lChildNodePos, lChildNodeSizeHalf) ||
		(pDepth == mMaxTreeDepth))
	{
		//MATH_ASSERT((pNode->mEntryMap.Find(pKey) == pNode->mEntryMap.End()), "LooseOctree::InsertObject: pNode->mEntryMap.Find(pKey) == pNode->mEntryMap.End()");
		//MATH_ASSERT((mNodeTable.Find(pKey) == mNodeTable.End()), "LooseOctree::InsertObject: mNodeTable.Find(pKey) == mNodeTable.End()");

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
			lNewChildNode->mNodeBox.SetSize(Vector3D<_TVarType>(lChildNodeSizeHalf, lChildNodeSizeHalf, lChildNodeSizeHalf));
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




TEMPLATE inline typename QUAL::Node::EntryTable::Iterator QUAL::FindObject(_TKey pKey, Node* pObjectNode) const
{
	//MATH_ASSERT((pObjectNode != NULL), "LooseOctree::FindObject: pObjectNode != NULL");
	//MATH_ASSERT((pObjectNode->mEntryTable.Find(pKey) != pObjectNode->mEntryTable.End()), "LooseOctree::FindObject: pObjectNode->mEntryTable.Find(pKey) != pObjectNode->mEntryTable.End()");

	return pObjectNode->mEntryTable.Find(pKey);
}




TEMPLATE bool QUAL::MoveObject(_TKey pKey, LOVolume<_TVarType>* pNewVolume)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		//LOG(PHYSIC, DEBUG, "LooseOctree::MoveObject, trying to move non existing object!");
		return false;
	}
	Node* lNode = *lNodeIter;

	typename Node::EntryTable::Iterator lEntryIter = FindObject(pKey, lNode);
	(*lEntryIter).mVolume = pNewVolume;
	
	MoveObject(pKey, lNodeIter, lEntryIter);

	return true;
}




TEMPLATE _TObject QUAL::MoveObject(_TKey pKey, const Vector3D<_TVarType>& pToPos)
{
	typename NodeTable::Iterator lNodeIter = mNodeTable.Find(pKey);
	if (lNodeIter == mNodeTable.End())
	{
		//LOG(PHYSIC, DEBUG, "LooseOctree::MoveObject, trying to move non existing object!");
		return false;
	}
	Node* lNode = *lNodeIter;

	typename Node::EntryTable::Iterator lEntryIter = FindObject(pKey, lNode);
	(*lEntryIter).mVolume->SetPosition(pToPos);

	return MoveObject(pKey, lNodeIter);
}

TEMPLATE _TObject QUAL::MoveObject(_TKey pKey, typename NodeTable::Iterator& pNodeIter)
{
	typename Node::Entry lEntry = RemoveObject(pKey, pNodeIter);
	InsertObject(pKey, lEntry, mRootNode, 0);
	return lEntry.mObject;
}



TEMPLATE unsigned QUAL::GetOverlaps( const Vector3D<_TVarType>& pPosRelParent, 
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
		// Remove the four rightmost children.
		lOverlapMask &= ~(16 + 32 + 64 + 128);
		MACRO_LO_GO1_TEST_FRONT_BACK_TOP_BOTTOM
	}
	// Test the left portion of the children.
	else if( ( pPosRelParent.x + pChildNodeSize ) > lMinBoxSeparation ||
		    -( pChildNodeSize + pPosRelParent.x ) > lMinBoxSeparation)
	{
		// Remove the four leftmost children.
		lOverlapMask &= ~(1 + 2 + 4 + 8);
		MACRO_LO_GO1_TEST_FRONT_BACK_TOP_BOTTOM
	}

	return lOverlapMask;
}




TEMPLATE unsigned QUAL::GetOverlaps( const Vector3D<_TVarType>& pPosRelParent, 
				     _TVarType pSizeX, 
				     _TVarType pSizeY, 
				     _TVarType pSizeZ, 
				     _TVarType pChildNodeSize,
				     _TVarType pParentNodeSize) const
{
	// Suppose that we overlap all nodes.
	unsigned lOverlapMask	= 0xFFFFFFFF;
	_TVarType lMinBoxSeparationX = pSizeX + pParentNodeSize;
	_TVarType lMinBoxSeparationY = pSizeY + pParentNodeSize;
	_TVarType lMinBoxSeparationZ = pSizeZ + pParentNodeSize;

	// Test the right portion of the children.
	if ( ( pPosRelParent.x - pChildNodeSize ) > lMinBoxSeparationX ||
		( pChildNodeSize - pPosRelParent.x ) > lMinBoxSeparationX)
	{
		// Remove the four rightmost children.
		lOverlapMask &= ~(16 + 32 + 64 + 128);
		MACRO_LO_GO2_TEST_FRONT_BACK_TOP_BOTTOM
	}
	else if( ( pPosRelParent.x + pChildNodeSize ) > lMinBoxSeparationX ||
		    -( pChildNodeSize + pPosRelParent.x ) > lMinBoxSeparationX)
	{
		// Remove the four leftmost children.
		lOverlapMask &= ~(1 + 2 + 4 + 8);
		MACRO_LO_GO2_TEST_FRONT_BACK_TOP_BOTTOM
	}

	return lOverlapMask;
}




TEMPLATE uint8 QUAL::GetChild(const Vector3D<_TVarType>& pPos, const Node* pNode)
{
	uint8 pIndex = 0;

	if ( pPos.x > pNode->GetPosition().x )
	{
		pIndex |= 4;
	}

	if ( pPos.y > pNode->GetPosition().y )
	{
		pIndex |= 2;
	}

	if ( pPos.z > pNode->GetPosition().z )
	{
		pIndex |= 1;
	}

	return pIndex;
}




TEMPLATE uint8 QUAL::GetChild(const Vector3D<_TVarType>& pPos, const Node* pNode, Vector3D<_TVarType>& pChildPos)
{
	_TVarType lChildUnloosSizeHalf = pNode->GetFixedSizeHalf() * 0.5f;

	uint8 pIndex = 0;

	if ( pPos.x > pNode->GetPosition().x )
	{
		pChildPos.x = pNode->GetPosition().x + lChildUnloosSizeHalf;
		pIndex |= 4;
	}
	else
	{
		pChildPos.x = pNode->GetPosition().x - lChildUnloosSizeHalf;
	}

	if ( pPos.y > pNode->GetPosition().y )
	{
		pChildPos.y = pNode->GetPosition().y + lChildUnloosSizeHalf;
		pIndex |= 2;
	}
	else
	{
		pChildPos.y = pNode->GetPosition().y - lChildUnloosSizeHalf;
	}

	if ( pPos.z > pNode->GetPosition().z )
	{
		pChildPos.z = pNode->GetPosition().z + lChildUnloosSizeHalf;
		pIndex |= 1;
	}
	else
	{
		pChildPos.z = pNode->GetPosition().z - lChildUnloosSizeHalf;
	}

	return pIndex;
}








TEMPLATE void QUAL::GetObjects(ObjectList& pObjects, const Sphere<_TVarType>& pBS)
{
	GetObjectsInBS(pObjects, pBS, mRootNode);
}

TEMPLATE void QUAL::GetObjects(ObjectList& pObjects, const AABB<_TVarType>& pAABB)
{
	GetObjectsInAABB(pObjects, pAABB, mRootNode);
}

TEMPLATE void QUAL::GetObjectsInBS(ObjectList& pObjects, const Sphere<_TVarType>& pBS, Node* pCurrentNode)
{
	// Insert objects at this node into list.
	typename Node::EntryTable::Iterator lIter;
	for (lIter  = pCurrentNode->mEntryTable.First(); 
		lIter != pCurrentNode->mEntryTable.End(); 
		++lIter)
	{
		const typename Node::Entry& lEntry = *lIter;

		if (lEntry.mVolume->IsBSOverlappingVolume(pBS))
		{
			pObjects.push_back(lEntry.mObject);
		}
	}

	unsigned lOverlapMask = GetOverlaps(pBS.GetPosition() - pCurrentNode->GetPosition(), 
					    pBS.GetRadius(), 
					    pCurrentNode->GetSizeHalf() * 0.5f, 
					    pCurrentNode->GetSizeHalf());

	lOverlapMask &= pCurrentNode->mChildMask;

	if (lOverlapMask != 0)
	{
		if ((lOverlapMask & 1) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[0]);
		}
		if ((lOverlapMask & 2) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[1]);
		}
		if ((lOverlapMask & 4) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[2]);
		}
		if ((lOverlapMask & 8) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[3]);
		}
		if ((lOverlapMask & 16) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[4]);
		}
		if ((lOverlapMask & 32) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[5]);
		}
		if ((lOverlapMask & 64) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[6]);
		}
		if ((lOverlapMask & 128) != 0)
		{
			GetObjectsInBS(pObjects, pBS, pCurrentNode->mChildren[7]);
		}
	}
}




TEMPLATE void QUAL::GetObjectsInAABB(ObjectList& pObjects, const AABB<_TVarType>& pAABB, Node* pCurrentNode)
{
	typename Node::EntryTable::Iterator lIter;
	for (lIter  = pCurrentNode->mEntryTable.First(); 
		lIter != pCurrentNode->mEntryTable.End(); 
		++lIter)
	{
		const typename Node::Entry& lEntry = *lIter;

		if (lEntry.mVolume->IsAABBOverlappingVolume(pAABB))
		{
			pObjects.push_back(lEntry.mObject);
		}
	}

	unsigned lOverlapMask = GetOverlaps(pAABB.GetPosition() - pCurrentNode->GetPosition(), 
					    pAABB.GetSize().x, 
					    pAABB.GetSize().y, 
					    pAABB.GetSize().z, 
					    pCurrentNode->GetSizeHalf() * 0.5f, 
					    pCurrentNode->GetSizeHalf());
	lOverlapMask &= pCurrentNode->mChildMask;

	if (lOverlapMask != 0)
	{
		if ((lOverlapMask & 1) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[0]);
		}
		if ((lOverlapMask & 2) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[1]);
		}
		if ((lOverlapMask & 4) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[2]);
		}
		if ((lOverlapMask & 8) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[3]);
		}
		if ((lOverlapMask & 16) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[4]);
		}
		if ((lOverlapMask & 32) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[5]);
		}
		if ((lOverlapMask & 64) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[6]);
		}
		if ((lOverlapMask & 128) != 0)
		{
			GetObjectsInAABB(pObjects, pAABB, pCurrentNode->mChildren[7]);
		}
	}
}




TEMPLATE unsigned QUAL::GetFullTreeMemSize() const
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
TEMPLATE String QUAL::ToString() const
{
	std::stringstream lStrm;
	mRootNode->ToString(lStrm, 0, -1);
	return lStrm.str();
}
*/



TEMPLATE void QUAL::GetNodeBoxes(AABBList& pBoxes) const
{
	mRootNode->GetNodeBox(pBoxes);
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

/*
//---------------------------------------------------------------------------
TEMPLATE bool QUAL::FitsInNode(const Vector3D<_TVarType>& pPos, _TVarType pBoundingRadius, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize)
{
	if (	pBoundingRadius < pNodeSize )
	{
		return true;
	}

	_TVarType lSizeDiff = pNodeSize * 2.0f - pBoundingRadius;

	if ( fabs( pPos.x - pNodePos.x )  > lSizeDiff )
	{
		return false;
	}

	if ( fabs( pPos.y - pNodePos.y )  > lSizeDiff )
	{
		return false;
	}

	if ( fabs( pPos.z - pNodePos.z )  > lSizeDiff )
	{
		return false;
	}

	return true;
}

/*
//---------------------------------------------------------------------------
TEMPLATE bool QUAL::FitsInAbsoluteNode(const Vector3D<_TVarType>& pPos, _TVarType pBoundingRadius, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize)
{
	_TVarType lSizeDiff = pNodeSize * 2.0f - pBoundingRadius;

	if ( fabs( pPos.x - pNodePos.x )  > lSizeDiff )
	{
		return false;
	}

	if ( fabs( pPos.y - pNodePos.y )  > lSizeDiff )
	{
		return false;
	}

	if ( fabs( pPos.z - pNodePos.z )  > lSizeDiff )
	{
		return false;
	}

	return true;
}
*/
/*
//---------------------------------------------------------------------------
TEMPLATE bool QUAL::IsOverlapping( const Vector3D<_TVarType>& pPos, _TVarType pBoundingRadius, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize)
{
	_TVarType lMinBoxSeparation = pBoundingRadius + pNodeSize * 2.0f;

	if ( fabs( pPos.x - pNodePos.x ) > lMinBoxSeparation )
	{
		return false;
	}

	if ( fabs( pPos.y - pNodePos.y ) > lMinBoxSeparation )
	{
		return false;
	}

	if ( fabs( pPos.z - pNodePos.z ) > lMinBoxSeparation )
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
TEMPLATE bool QUAL::IsOverlapping( const Vector3D<_TVarType>& pPos, _TVarType pSizeX, _TVarType pSizeY, _TVarType pSizeZ, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize)
{
	if ( fabs( pPos.x - pNodePos.x ) > ( pSizeX + pNodeSize * 2.0f ) )
	{
		return false;
	}

	if ( fabs( pPos.y - pNodePos.y ) > ( pSizeY + pNodeSize * 2.0f ) )
	{
		return false;
	}

	if ( fabs( pPos.z - pNodePos.z ) > ( pSizeZ + pNodeSize * 2.0f ) )
	{
		return false;
	}

	return true;
}
*/

