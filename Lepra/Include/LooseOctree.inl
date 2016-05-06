/*
	Class:  LooseOctree
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

TEMPLATE QUAL::LooseOctree(	_TObject error_object,
				_TVarType total_tree_size,
				_TVarType minimum_cell_size,
				_TVarType k) :
	error_object_(error_object) {
	// Find required root node size.
	_TVarType pow_of2 = 2;
	max_tree_depth_ = 1;
	while ( minimum_cell_size * pow_of2 < total_tree_size ) {
		max_tree_depth_++;
		pow_of2 *= 2;
	}

	k_ = k;
	_TVarType _fixed_size_half = minimum_cell_size * pow_of2 * 0.5;
	_TVarType size_half = _fixed_size_half * k_;

	root_node_ = new Node(0, 255, _fixed_size_half);

	// Tree center at 0,0,0.
	Vector3D<_TVarType> size_half_vec(size_half, size_half, size_half);
	root_node_->node_box_ = AABB<_TVarType>(Vector3D<_TVarType>(0, 0, 0), size_half_vec);

	num_objects_ = 0;
	num_nodes_ = 1;
}




TEMPLATE QUAL::~LooseOctree() {
	delete root_node_;
	typename NodeList::iterator iter;
	for (iter = recycled_node_list_.begin(); iter != recycled_node_list_.end(); ++iter) {
		delete (*iter);
	}
}




TEMPLATE void QUAL::InsertObject(_TKey key, LOVolume<_TVarType>* volume, _TObject object) {
	if (node_table_.Find(key) != node_table_.End()) {
		//LOG(PHYSIC, ERROR, "LooseOctree::InsertObject, object already inserted!");
		return;
	}

	typename Node::Entry _entry(volume, object);

	InsertObject(key, _entry, root_node_, 0);
}




TEMPLATE void QUAL::InsertObject(_TKey key, typename Node::Entry entry, Node* node, uint16 depth) {
	// Calculate the position, size and index values of the child node.
	_TVarType child_node_size_half = node->GetSizeHalf() * 0.5;

	Vector3D<_TVarType> child_node_pos;
	uint8 child_index = GetChild(entry.volume_->GetPosition(), node, child_node_pos);

	// Check if the object fits in the child node or not.
	if (	!entry.volume_->IsAABCEnclosingVolume(child_node_pos, child_node_size_half) ||
		(depth == max_tree_depth_)) {
		//MATH_ASSERT((node->mEntryMap.Find(key) == node->mEntryMap.End()), "LooseOctree::InsertObject: node->mEntryMap.Find(key) == node->mEntryMap.End()");
		//MATH_ASSERT((node_table_.Find(key) == node_table_.End()), "LooseOctree::InsertObject: node_table_.Find(key) == node_table_.End()");

		// The object doesn't fit in the child node, so put it in the current node...
		node->entry_table_.Insert(key, entry);
		node->object_count_++;
		node_table_.Insert(key, node);
		num_objects_++;
	} else {
		// Insert object in the child node
		if (node->children_[child_index] == 0) {
			Node* new_child_node = NewNode(node, child_index, node->GetFixedSizeHalf() * 0.5);
			new_child_node->node_box_.SetPosition(child_node_pos);
			new_child_node->node_box_.SetSize(Vector3D<_TVarType>(child_node_size_half, child_node_size_half, child_node_size_half));
			node->child_mask_ |= (1 << child_index);
			num_nodes_++;
			node->children_[child_index] = new_child_node;
		}

		InsertObject(key, entry, node->children_[child_index], depth + 1);
	}
}



TEMPLATE _TObject QUAL::RemoveObject(_TKey key) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		return error_object_;
	}

	return RemoveObject(key, _node_iter).object_;
}




TEMPLATE typename QUAL::Node::Entry QUAL::RemoveObject(_TKey key, typename NodeTable::Iterator& node_iter) {
	// Removes object from the octree and deletes the node if empty.
	Node* _current_node = *node_iter;

	typename Node::EntryTable::Iterator iter = FindObject(key, _current_node);
	typename Node::Entry _entry = *iter;

	_current_node->entry_table_.Remove(iter);
	node_table_.Remove(node_iter);
	num_objects_--;

	while (_current_node != 0 && _current_node->IsEmpty() == true) {
		Node* _parent = _current_node->parent_;

		if (_parent != 0) {
			RecycleNode(_parent->children_[_current_node->index_]);
			_parent->children_[_current_node->index_] = 0;

			_parent->child_mask_ &= (0xFFFFFFFF ^ (1 << _current_node->index_));
			num_nodes_--;
		}

		_current_node = _parent;
	}

	return _entry;
}




TEMPLATE _TObject QUAL::FindObject(_TKey key) const {
	typename NodeTable::ConstIterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		return error_object_;
	}

	return (*FindObject(key, *_node_iter)).object_;
}




TEMPLATE inline typename QUAL::Node::EntryTable::Iterator QUAL::FindObject(_TKey key, Node* object_node) const {
	//MATH_ASSERT((object_node != NULL), "LooseOctree::FindObject: object_node != NULL");
	//MATH_ASSERT((object_node->entry_table_.Find(key) != object_node->entry_table_.End()), "LooseOctree::FindObject: object_node->entry_table_.Find(key) != object_node->entry_table_.End()");

	return object_node->entry_table_.Find(key);
}




TEMPLATE bool QUAL::MoveObject(_TKey key, LOVolume<_TVarType>* new_volume) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		//LOG(PHYSIC, DEBUG, "LooseOctree::MoveObject, trying to move non existing object!");
		return false;
	}
	Node* _node = *_node_iter;

	typename Node::EntryTable::Iterator entry_iter = FindObject(key, _node);
	(*entry_iter).volume_ = new_volume;

	MoveObject(key, _node_iter, entry_iter);

	return true;
}




TEMPLATE _TObject QUAL::MoveObject(_TKey key, const Vector3D<_TVarType>& to_pos) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		//LOG(PHYSIC, DEBUG, "LooseOctree::MoveObject, trying to move non existing object!");
		return _TObject();
	}
	Node* _node = *_node_iter;

	typename Node::EntryTable::Iterator entry_iter = FindObject(key, _node);
	(*entry_iter).volume_->SetPosition(to_pos);

	return MoveObject(key, _node_iter);
}

TEMPLATE _TObject QUAL::MoveObject(_TKey key, typename NodeTable::Iterator& node_iter) {
	typename Node::Entry _entry = RemoveObject(key, node_iter);
	InsertObject(key, _entry, root_node_, 0);
	return _entry.object_;
}



TEMPLATE unsigned QUAL::GetOverlaps( const Vector3D<_TVarType>& pos_rel_parent,
				     _TVarType bounding_radius,
				     _TVarType child_node_size,
				     _TVarType parent_node_size) const {
	// Suppose that we overlap all nodes.
	unsigned overlap_mask	= 0xFFFFFFFF;
	_TVarType min_box_separation = bounding_radius + parent_node_size;

	// Test the right portion of the children.
	if ( ( pos_rel_parent.x - child_node_size ) > min_box_separation ||
		( child_node_size - pos_rel_parent.x ) > min_box_separation) {
		// Remove the four rightmost children.
		overlap_mask &= ~(16 + 32 + 64 + 128);
		kMacroLoGo1TestFrontBackTopBottom
	}
	// Test the left portion of the children.
	else if( ( pos_rel_parent.x + child_node_size ) > min_box_separation ||
		    -( child_node_size + pos_rel_parent.x ) > min_box_separation) {
		// Remove the four leftmost children.
		overlap_mask &= ~(1 + 2 + 4 + 8);
		kMacroLoGo1TestFrontBackTopBottom
	}

	return overlap_mask;
}




TEMPLATE unsigned QUAL::GetOverlaps( const Vector3D<_TVarType>& pos_rel_parent,
				     _TVarType size_x,
				     _TVarType size_y,
				     _TVarType size_z,
				     _TVarType child_node_size,
				     _TVarType parent_node_size) const {
	// Suppose that we overlap all nodes.
	unsigned overlap_mask	= 0xFFFFFFFF;
	_TVarType min_box_separation_x = size_x + parent_node_size;
	_TVarType min_box_separation_y = size_y + parent_node_size;
	_TVarType min_box_separation_z = size_z + parent_node_size;

	// Test the right portion of the children.
	if ( ( pos_rel_parent.x - child_node_size ) > min_box_separation_x ||
		( child_node_size - pos_rel_parent.x ) > min_box_separation_x) {
		// Remove the four rightmost children.
		overlap_mask &= ~(16 + 32 + 64 + 128);
		kMacroLoGo2TestFrontBackTopBottom
	} else if( ( pos_rel_parent.x + child_node_size ) > min_box_separation_x ||
		    -( child_node_size + pos_rel_parent.x ) > min_box_separation_x) {
		// Remove the four leftmost children.
		overlap_mask &= ~(1 + 2 + 4 + 8);
		kMacroLoGo2TestFrontBackTopBottom
	}

	return overlap_mask;
}




TEMPLATE uint8 QUAL::GetChild(const Vector3D<_TVarType>& pos, const Node* node) {
	uint8 index = 0;

	if ( pos.x > node->GetPosition().x ) {
		index |= 4;
	}

	if ( pos.y > node->GetPosition().y ) {
		index |= 2;
	}

	if ( pos.z > node->GetPosition().z ) {
		index |= 1;
	}

	return index;
}




TEMPLATE uint8 QUAL::GetChild(const Vector3D<_TVarType>& pos, const Node* node, Vector3D<_TVarType>& child_pos) {
	_TVarType child_unloos_size_half = node->GetFixedSizeHalf() * 0.5f;

	uint8 index = 0;

	if ( pos.x > node->GetPosition().x ) {
		child_pos.x = node->GetPosition().x + child_unloos_size_half;
		index |= 4;
	} else {
		child_pos.x = node->GetPosition().x - child_unloos_size_half;
	}

	if ( pos.y > node->GetPosition().y ) {
		child_pos.y = node->GetPosition().y + child_unloos_size_half;
		index |= 2;
	} else {
		child_pos.y = node->GetPosition().y - child_unloos_size_half;
	}

	if ( pos.z > node->GetPosition().z ) {
		child_pos.z = node->GetPosition().z + child_unloos_size_half;
		index |= 1;
	} else {
		child_pos.z = node->GetPosition().z - child_unloos_size_half;
	}

	return index;
}








TEMPLATE void QUAL::GetObjects(ObjectList& objects, const Sphere<_TVarType>& bs) {
	GetObjectsInBS(objects, bs, root_node_);
}

TEMPLATE void QUAL::GetObjects(ObjectList& objects, const AABB<_TVarType>& aabb) {
	GetObjectsInAABB(objects, aabb, root_node_);
}

TEMPLATE void QUAL::GetObjectsInBS(ObjectList& objects, const Sphere<_TVarType>& bs, Node* current_node) {
	// Insert objects at this node into list.
	typename Node::EntryTable::Iterator iter;
	for (iter  = current_node->entry_table_.First();
		iter != current_node->entry_table_.End();
		++iter) {
		const typename Node::Entry& _entry = *iter;

		if (_entry.volume_->IsBSOverlappingVolume(bs)) {
			objects.push_back(_entry.object_);
		}
	}

	unsigned overlap_mask = GetOverlaps(bs.GetPosition() - current_node->GetPosition(),
					    bs.GetRadius(),
					    current_node->GetSizeHalf() * 0.5f,
					    current_node->GetSizeHalf());

	overlap_mask &= current_node->child_mask_;

	if (overlap_mask != 0) {
		if ((overlap_mask & 1) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[0]);
		}
		if ((overlap_mask & 2) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[1]);
		}
		if ((overlap_mask & 4) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[2]);
		}
		if ((overlap_mask & 8) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[3]);
		}
		if ((overlap_mask & 16) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[4]);
		}
		if ((overlap_mask & 32) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[5]);
		}
		if ((overlap_mask & 64) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[6]);
		}
		if ((overlap_mask & 128) != 0) {
			GetObjectsInBS(objects, bs, current_node->children_[7]);
		}
	}
}




TEMPLATE void QUAL::GetObjectsInAABB(ObjectList& objects, const AABB<_TVarType>& aabb, Node* current_node) {
	typename Node::EntryTable::Iterator iter;
	for (iter  = current_node->entry_table_.First();
		iter != current_node->entry_table_.End();
		++iter) {
		const typename Node::Entry& _entry = *iter;

		if (_entry.volume_->IsAABBOverlappingVolume(aabb)) {
			objects.push_back(_entry.object_);
		}
	}

	unsigned overlap_mask = GetOverlaps(aabb.GetPosition() - current_node->GetPosition(),
					    aabb.GetSize().x,
					    aabb.GetSize().y,
					    aabb.GetSize().z,
					    current_node->GetSizeHalf() * 0.5f,
					    current_node->GetSizeHalf());
	overlap_mask &= current_node->child_mask_;

	if (overlap_mask != 0) {
		if ((overlap_mask & 1) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[0]);
		}
		if ((overlap_mask & 2) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[1]);
		}
		if ((overlap_mask & 4) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[2]);
		}
		if ((overlap_mask & 8) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[3]);
		}
		if ((overlap_mask & 16) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[4]);
		}
		if ((overlap_mask & 32) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[5]);
		}
		if ((overlap_mask & 64) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[6]);
		}
		if ((overlap_mask & 128) != 0) {
			GetObjectsInAABB(objects, aabb, current_node->children_[7]);
		}
	}
}




TEMPLATE unsigned QUAL::GetFullTreeMemSize() const {
	unsigned num_nodes = 0;
	int i;

	for (i = 1; i <= max_tree_depth_; i++) {
		num_nodes += (unsigned)pow(8.0f, i);
	}

	return num_nodes * sizeof(Node);
}



/*
TEMPLATE str QUAL::ToString() const {
	std::stringstream lStrm;
	root_node_->ToString(lStrm, 0, -1);
	return lStrm.str();
}
*/



TEMPLATE void QUAL::GetNodeBoxes(AABBList& boxes) const {
	root_node_->GetNodeBox(boxes);
}




TEMPLATE void QUAL::RecycleNode(Node* node) {
	if (recycled_node_list_.size() < kMaxRecycledNodes) {
		node->DeleteChildren(this);
		recycled_node_list_.push_back(node);
	} else {
		delete node;
	}
}

TEMPLATE typename QUAL::Node* QUAL::NewNode(Node* parent, uint8 index, _TVarType fixed_size_half) {
	Node* _node = 0;

	if (recycled_node_list_.empty()) {
		_node = new Node(parent, index, fixed_size_half);
	} else {
		_node = recycled_node_list_.front();
		recycled_node_list_.pop_front();
		_node->Init(parent, index, fixed_size_half);
	}
	return _node;
}

/*
//---------------------------------------------------------------------------
TEMPLATE bool QUAL::FitsInNode(const Vector3D<_TVarType>& pos, _TVarType bounding_radius, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize) {
	if (	bounding_radius < pNodeSize ) {
		return true;
	}

	_TVarType size_diff = pNodeSize * 2.0f - bounding_radius;

	if ( fabs( pos.x - pNodePos.x )  > size_diff ) {
		return false;
	}

	if ( fabs( pos.y - pNodePos.y )  > size_diff ) {
		return false;
	}

	if ( fabs( pos.z - pNodePos.z )  > size_diff ) {
		return false;
	}

	return true;
}

/*
//---------------------------------------------------------------------------
TEMPLATE bool QUAL::FitsInAbsoluteNode(const Vector3D<_TVarType>& pos, _TVarType bounding_radius, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize) {
	_TVarType size_diff = pNodeSize * 2.0f - bounding_radius;

	if ( fabs( pos.x - pNodePos.x )  > size_diff ) {
		return false;
	}

	if ( fabs( pos.y - pNodePos.y )  > size_diff ) {
		return false;
	}

	if ( fabs( pos.z - pNodePos.z )  > size_diff ) {
		return false;
	}

	return true;
}
*/
/*
//---------------------------------------------------------------------------
TEMPLATE bool QUAL::IsOverlapping( const Vector3D<_TVarType>& pos, _TVarType bounding_radius, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize) {
	_TVarType min_box_separation = bounding_radius + pNodeSize * 2.0f;

	if ( fabs( pos.x - pNodePos.x ) > min_box_separation ) {
		return false;
	}

	if ( fabs( pos.y - pNodePos.y ) > min_box_separation ) {
		return false;
	}

	if ( fabs( pos.z - pNodePos.z ) > min_box_separation ) {
		return false;
	}

	return true;
}

//---------------------------------------------------------------------------
TEMPLATE bool QUAL::IsOverlapping( const Vector3D<_TVarType>& pos, _TVarType size_x, _TVarType size_y, _TVarType size_z, const Vector3D<_TVarType>& pNodePos, _TVarType pNodeSize) {
	if ( fabs( pos.x - pNodePos.x ) > ( size_x + pNodeSize * 2.0f ) ) {
		return false;
	}

	if ( fabs( pos.y - pNodePos.y ) > ( size_y + pNodeSize * 2.0f ) ) {
		return false;
	}

	if ( fabs( pos.z - pNodePos.z ) > ( size_z + pNodeSize * 2.0f ) ) {
		return false;
	}

	return true;
}
*/

