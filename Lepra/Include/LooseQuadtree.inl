
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



LQ_TEMPLATE LQ_QUAL::LooseQuadtree(_TObject error_object, _TVarType total_tree_size,_TVarType minimum_cell_size, _TVarType k):
	error_object_(error_object) {
	// Find required root node size.
	_TVarType pow_of2 = 2;
	max_tree_depth_ = 1;
	while (minimum_cell_size * pow_of2 < total_tree_size) {
		max_tree_depth_++;
		pow_of2 *= 2;
	}

	k_ = k;
	_TVarType _fixed_size_half = minimum_cell_size * pow_of2 * (_TVarType)0.5;
	_TVarType size_half = _fixed_size_half * k_;

	root_node_ = new Node(0, 255, _fixed_size_half);

	// Tree center at 0,0,0.
	Vector2D<_TVarType> size_half_vec(size_half, size_half);
	root_node_->node_box_ = AABR_(Vector2D<_TVarType>(0, 0), size_half_vec);

	num_objects_ = 0;
	num_nodes_ = 1;
}




LQ_TEMPLATE LQ_QUAL::~LooseQuadtree() {
	delete root_node_;
	typename NodeList::iterator iter;
	for (iter = recycled_node_list_.begin(); iter != recycled_node_list_.end(); ++iter) {
		delete (*iter);
	}
}




LQ_TEMPLATE void LQ_QUAL::InsertObject(_TKey key, LQArea<_TVarType>* area, _TObject object) {
	if (node_table_.Find(key) != node_table_.End()) {
		//LOG(PHYSIC, ERROR, "LooseQuadtree::InsertObject, object already inserted!");
		return;
	}

	InsertObject(key, typename Node::Entry(area, object), root_node_, 0);
}




LQ_TEMPLATE void LQ_QUAL::InsertObject(_TKey key, typename Node::Entry entry, Node* node, uint16 depth) {
	// Calculate the position, size and index values of the child node.
	_TVarType child_node_size_half = node->GetSizeHalf() * 0.5;

	Vector2D<_TVarType> child_node_pos;
	uint8 child_index = GetChild(entry.area_->GetPosition(), node, child_node_pos);

	// Check if the object fits in the child node or not.
	if (	!entry.area_->IsAABSQEnclosingArea(child_node_pos, child_node_size_half) ||
		(depth == max_tree_depth_)) {
		//MATH_ASSERT((node->mEntryMap.Find(key) == node->mEntryMap.End()), "LooseQuadtree::InsertObject: node->mEntryMap.Find(key) == node->mEntryMap.End()");
		//MATH_ASSERT((node_table_.Find(key) == node_table_.End()), "LooseQuadtree::InsertObject: node_table_.Find(key) == node_table_.End()");

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
			new_child_node->node_box_.SetSize(Vector2D<_TVarType>(child_node_size_half, child_node_size_half));
			node->child_mask_ |= (1 << child_index);
			num_nodes_++;
			node->children_[child_index] = new_child_node;
		}

		InsertObject(key, entry, node->children_[child_index], depth + 1);
	}
}



LQ_TEMPLATE _TObject LQ_QUAL::RemoveObject(_TKey key) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		return error_object_;
	}

	return RemoveObject(key, _node_iter).object_;
}




LQ_TEMPLATE typename LQ_QUAL::Node::Entry LQ_QUAL::RemoveObject(_TKey key, typename NodeTable::Iterator& node_iter) {
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




LQ_TEMPLATE _TObject LQ_QUAL::FindObject(_TKey key) const {
	typename NodeTable::ConstIterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		return error_object_;
	}

	return (*FindObject(key, *_node_iter)).object_;
}




LQ_TEMPLATE inline typename LQ_QUAL::Node::EntryTable::Iterator LQ_QUAL::FindObject(_TKey key, Node* object_node) const {
	//MATH_ASSERT((object_node != NULL), "LooseQuadtree::FindObject: object_node != NULL");
	//MATH_ASSERT((object_node->entry_table_.Find(key) != object_node->entry_table_.End()), "LooseQuadtree::FindObject: object_node->entry_table_.Find(key) != object_node->entry_table_.End()");

	return object_node->entry_table_.Find(key);
}




LQ_TEMPLATE bool LQ_QUAL::MoveObject(_TKey key, LQArea<_TVarType>* new_area) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		//LOG(PHYSIC, DEBUG, "LooseQuadtree::MoveObject, trying to move non existing object!");
		return false;
	}
	Node* _node = *_node_iter;

	typename Node::EntryTable::Iterator entry_iter = FindObject(key, _node);
	(*entry_iter).area_ = new_area;

	MoveObject(key, _node_iter, entry_iter);

	return true;
}




LQ_TEMPLATE _TObject LQ_QUAL::MoveObject(_TKey key, const Vector2D<_TVarType>& to_pos) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		//LOG(PHYSIC, DEBUG, "LooseQuadtree::MoveObject, trying to move non existing object!");
		return _TObject();
	}
	Node* _node = *_node_iter;

	typename Node::EntryTable::Iterator entry_iter = FindObject(key, _node);
	(*entry_iter).area_->SetPosition(to_pos);

	return MoveObject(key, _node_iter);
}

LQ_TEMPLATE _TObject LQ_QUAL::MoveObject(_TKey key, typename NodeTable::Iterator& node_iter) {
	typename Node::Entry _entry = RemoveObject(key, node_iter);
	InsertObject(key, _entry, root_node_, 0);
	return _entry.object_;
}



LQ_TEMPLATE unsigned LQ_QUAL::GetOverlaps(const Vector2D<_TVarType>& pos_rel_parent,
				    _TVarType bounding_radius,
				    _TVarType child_node_size,
				    _TVarType parent_node_size) const {
	// Suppose that we overlap all nodes.
	unsigned overlap_mask	= 0xFFFFFFFF;
	_TVarType min_box_separation = bounding_radius + parent_node_size;

	// Test the right portion of the children.
	if ( ( pos_rel_parent.x - child_node_size ) > min_box_separation ||
	    ( child_node_size - pos_rel_parent.x ) > min_box_separation) {
		// Remove the two rightmost children.
		overlap_mask &= ~(4 + 8);
		kMacroLoGo1TestTopAndBottom
	}
	// Test the left portion of the children.
	else if( ( pos_rel_parent.x + child_node_size ) > min_box_separation ||
		-( child_node_size + pos_rel_parent.x ) > min_box_separation) {
		// Remove the two leftmost children.
		overlap_mask &= ~(1 + 2);
		kMacroLoGo1TestTopAndBottom
	}

	return overlap_mask;
}




LQ_TEMPLATE unsigned LQ_QUAL::GetOverlaps( const Vector2D<_TVarType>& pos_rel_parent,
				     _TVarType size_x,
				     _TVarType size_y,
				     _TVarType child_node_size,
				     _TVarType parent_node_size) const {
	// Suppose that we overlap all nodes.
	unsigned overlap_mask	= 0xFFFFFFFF;
	_TVarType min_box_separation_x = size_x + parent_node_size;
	_TVarType min_box_separation_y = size_y + parent_node_size;

	// Test the right portion of the children.
	if ( ( pos_rel_parent.x - child_node_size ) > min_box_separation_x ||
		( child_node_size - pos_rel_parent.x ) > min_box_separation_x) {
		// Remove the two rightmost children.
		overlap_mask &= ~(4 + 8);
		kMacroLoGo2TestTopAndBottom
	} else if( ( pos_rel_parent.x + child_node_size ) > min_box_separation_x ||
		    -( child_node_size + pos_rel_parent.x ) > min_box_separation_x) {
		// Remove the two leftmost children.
		overlap_mask &= ~(1 + 2);
		kMacroLoGo2TestTopAndBottom
	}

	return overlap_mask;
}




LQ_TEMPLATE uint8 LQ_QUAL::GetChild(const Vector2D<_TVarType>& pos, const Node* node) {
	uint8 index = 0;

	if ( pos.x > node->GetPosition().x ) {
		index |= 2;
	}

	if ( pos.y > node->GetPosition().y ) {
		index |= 1;
	}

	return index;
}




LQ_TEMPLATE uint8 LQ_QUAL::GetChild(const Vector2D<_TVarType>& pos, const Node* node, Vector2D<_TVarType>& child_pos) {
	_TVarType child_unloos_size_half = node->GetFixedSizeHalf() * 0.5f;

	uint8 index = 0;

	if ( pos.x > node->GetPosition().x ) {
		child_pos.x = node->GetPosition().x + child_unloos_size_half;
		index |= 2;
	} else {
		child_pos.x = node->GetPosition().x - child_unloos_size_half;
	}

	if ( pos.y > node->GetPosition().y ) {
		child_pos.y = node->GetPosition().y + child_unloos_size_half;
		index |= 1;
	} else {
		child_pos.y = node->GetPosition().y - child_unloos_size_half;
	}

	return index;
}








LQ_TEMPLATE void LQ_QUAL::GetObjects(ObjectList& objects, const BC& bc) {
	GetObjectsInBC(root_node_, objects, bc);
}

LQ_TEMPLATE void LQ_QUAL::GetObjects(ObjectList& objects, const AABR_& aabr) {
	GetObjectsInAABR(root_node_, objects, aabr);
}

LQ_TEMPLATE void LQ_QUAL::GetObjectsInBC(Node* current_node, ObjectList& objects, const BC& bc) {
	// Insert objects at this node into list.
	typename Node::EntryTable::Iterator iter;
	for (iter  = current_node->entry_table_.First();
		iter != current_node->entry_table_.End();
		++iter) {
		const typename Node::Entry& _entry = *iter;

		if (_entry.area_->IsBCOverlappingArea(bc)) {
			objects.push_back(_entry.object_);
		}
	}

	unsigned overlap_mask = GetOverlaps(bc.GetPosition() - current_node->GetPosition(),
					       bc.GetRadius(),
					       current_node->GetSizeHalf() * 0.5f,
					       current_node->GetSizeHalf());

	overlap_mask &= current_node->child_mask_;

	if (overlap_mask != 0) {
		if ((overlap_mask & 1) != 0) {
			GetObjectsInBC(current_node->children_[0], objects, bc);
		}
		if ((overlap_mask & 2) != 0) {
			GetObjectsInBC(current_node->children_[1], objects, bc);
		}
		if ((overlap_mask & 4) != 0) {
			GetObjectsInBC(current_node->children_[2], objects, bc);
		}
		if ((overlap_mask & 8) != 0) {
			GetObjectsInBC(current_node->children_[3], objects, bc);
		}
	}
}




LQ_TEMPLATE void LQ_QUAL::GetObjectsInAABR(Node* current_node, ObjectList& objects, const AABR_& aabr) {
	typename Node::EntryTable::Iterator iter;
	for (iter = current_node->entry_table_.First();
		iter != current_node->entry_table_.End();
		++iter) {
		const typename Node::Entry& _entry = *iter;

		if (_entry.area_->IsAABROverlappingArea(aabr)) {
			objects.push_back(_entry.object_);
		}
	}

	unsigned overlap_mask = GetOverlaps(	aabr.GetPosition() - current_node->GetPosition(),
						aabr.GetSize().x,
						aabr.GetSize().y,
						current_node->GetSizeHalf() * 0.5f,
						current_node->GetSizeHalf());
	overlap_mask &= current_node->child_mask_;

	if (overlap_mask != 0) {
		if ((overlap_mask & 1) != 0) {
			GetObjectsInAABR(current_node->children_[0], objects, aabr);
		}
		if ((overlap_mask & 2) != 0) {
			GetObjectsInAABR(current_node->children_[1], objects, aabr);
		}
		if ((overlap_mask & 4) != 0) {
			GetObjectsInAABR(current_node->children_[2], objects, aabr);
		}
		if ((overlap_mask & 8) != 0) {
			GetObjectsInAABR(current_node->children_[3], objects, aabr);
		}
	}
}




LQ_TEMPLATE unsigned LQ_QUAL::GetFullTreeMemSize() const {
	unsigned num_nodes = 0;
	int i;

	for (i = 1; i <= max_tree_depth_; i++) {
		num_nodes += (unsigned)pow(8.0f, i);
	}

	return num_nodes * sizeof(Node);
}



/*
LQ_TEMPLATE str LQ_QUAL::ToString() const {
	std::stringstream lStrm;
	root_node_->ToString(lStrm, 0, -1);
	return lStrm.str();
}
*/



/*LQ_TEMPLATE void LQ_QUAL::GetNodeBoxes(AABRList& boxes) const {
	root_node_->GetNodeBox(boxes);
}*/




LQ_TEMPLATE void LQ_QUAL::RecycleNode(Node* node) {
	if (recycled_node_list_.size() < kMaxRecycledNodes) {
		node->DeleteChildren(this);
		recycled_node_list_.push_back(node);
	} else {
		delete node;
	}
}

LQ_TEMPLATE typename LQ_QUAL::Node* LQ_QUAL::NewNode(Node* parent, uint8 index, _TVarType fixed_size_half) {
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
