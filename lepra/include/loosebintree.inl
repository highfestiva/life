
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

TEMPLATE QUAL::LooseBinTree(_TObject error_object,
							_TVarType total_tree_size,
							_TVarType minimum_cell_size,
							_TVarType k) :
	error_object_(error_object) {
	// Find required root node size.
	_TVarType pow_of2 = 2;
	max_tree_depth_ = 1;
	while (minimum_cell_size * pow_of2 < total_tree_size) {
		max_tree_depth_++;
		pow_of2 *= 2;
	}

	k_ = k;
	_TVarType _fixed_size_half = minimum_cell_size * pow_of2 * 0.5;
	_TVarType _size_half = _fixed_size_half * k_;

	root_node_ = new Node(0, 255, _fixed_size_half);

	// Tree center at 0.
	root_node_->pos_ = 0;
	root_node_->size_half_ = _size_half;

	num_objects_ = 0;
	num_nodes_ = 1;
}

TEMPLATE QUAL::~LooseBinTree() {
	delete root_node_;
	typename NodeList::iterator iter;
	for (iter = recycled_node_list_.begin(); iter != recycled_node_list_.end(); ++iter) {
		delete (*iter);
	}
}

TEMPLATE void QUAL::InsertObject(_TKey key, _TObject object, _TVarType pos, _TVarType size_half) {
	if (node_table_.Find(key) != node_table_.End()) {
		log_.Warning("Object already inserted.");
		return;
	}

	typename Node::Entry _entry;
	_entry.size_half_ = size_half;
	_entry.pos_      = pos;
	_entry.object_   = object;

	InsertObject(key, _entry, root_node_, 0);
}

TEMPLATE void QUAL::InsertObject(_TKey key, typename Node::Entry entry, Node* node, unsigned depth) {
	// Calculate the position, size and index values of the child node.
	_TVarType child_node_size_half = node->GetSizeHalf() * 0.5;

	_TVarType child_node_pos;
	uint8 child_index = GetChild(entry.pos_, node, child_node_pos);

	// Check if the object fits in the child node or not.
	if (	!entry.IsEnclosed(child_node_pos, child_node_size_half) ||
		(depth == max_tree_depth_)) {
		// The object doesn't fit in the child node, so put it in the current node...
		node->entry_table_.Insert(key, entry);
		node->object_count_++;
		node_table_.Insert(key, node);
		num_objects_++;
	} else {
		// Insert object in the child node
		if (node->children_[child_index] == 0) {
			Node* new_child_node = NewNode(node, child_index, node->fixed_size_half_ * 0.5);
			new_child_node->pos_ = child_node_pos;
			new_child_node->size_half_ = child_node_size_half;
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
	Node* current_node = *node_iter;

	typename Node::EntryTable::Iterator iter = FindObject(key, current_node);
	typename Node::Entry _entry = *iter;

	current_node->entry_table_.Remove(iter);
	node_table_.Remove(node_iter);
	num_objects_--;

	while (current_node != 0 && current_node->IsEmpty() == true) {
		Node* _parent = current_node->parent_;

		if (_parent != 0) {
			RecycleNode(_parent->children_[current_node->index_]);
			_parent->children_[current_node->index_] = 0;

			_parent->child_mask_ &= (0xFFFFFFFF ^ (1 << current_node->index_));
			num_nodes_--;
		}

		current_node = _parent;
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

TEMPLATE typename QUAL::Node::EntryTable::Iterator QUAL::FindObject(_TKey key, Node* object_node) const {
	return object_node->entry_table_.Find(key);
}


TEMPLATE bool QUAL::MoveObject(_TKey key, _TVarType new_pos, _TVarType new_size_half) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		log_.Warning("Trying to move non existing object.");
		return false;
	}
	Node* _node = *_node_iter;

	typename Node::EntryTable::Iterator entry_iter = FindObject(key, _node);
	(*entry_iter).pos_      = new_pos;
	(*entry_iter).size_half_ = new_size_half;

	MoveObject(key, _node_iter, entry_iter);

	return true;
}

TEMPLATE _TObject QUAL::MoveObject(_TKey key, _TVarType new_pos) {
	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		log_.Warning("Trying to move non existing object.");
		return false;
	}
	Node* _node = *_node_iter;

	typename Node::EntryTable::Iterator entry_iter = FindObject(key, _node);
	(*entry_iter).pos_ = new_pos;

	return MoveObject(key, _node_iter, entry_iter);
}

TEMPLATE _TObject QUAL::MoveObject(_TKey key, typename NodeTable::Iterator& node_iter,
	typename Node::EntryTable::Iterator& /*object_iter*/) {
	typename Node::Entry _entry = RemoveObject(key, node_iter);
	InsertObject(key, _entry, root_node_, 0);
	return _entry.object_;
}

TEMPLATE bool QUAL::GetObjectSizeAndPos(_TKey key, _TVarType& pos, _TVarType& size_half) {
	bool ok = true;

	typename NodeTable::Iterator _node_iter = node_table_.Find(key);
	if (_node_iter == node_table_.End()) {
		log_.Warning("Trying to fetch non existing object.");
		ok = false;
	}

	typename Node::EntryTable::Iterator entry_iter;
	if (ok) {
		Node* _node = *_node_iter;
		entry_iter = FindObject(key, _node);
		ok = (entry_iter != _node->entry_table_.End());
	}

	if (ok) {
		pos = (*entry_iter).pos_;
		size_half = (*entry_iter).size_half_;
	}

	return ok;
}


TEMPLATE unsigned QUAL::GetOverlaps(_TVarType pos_rel_parent,
				    _TVarType size_half,
				    _TVarType child_node_size,
				    _TVarType parent_node_size) const {
	// Suppose that we overlap all nodes.
	unsigned overlap_mask = 3;
	_TVarType min_separation = size_half + parent_node_size;

	// Test the right child.
	if ( ( pos_rel_parent  - child_node_size ) > min_separation ||
		( child_node_size - pos_rel_parent  ) > min_separation) {
		// Remove the right child.
		overlap_mask &= ~2;
	}
	// Test the left portion of the children.
	else if( ( pos_rel_parent  + child_node_size) > min_separation ||
		    -( child_node_size + pos_rel_parent ) > min_separation) {
		// Remove the left child.
		overlap_mask &= ~1;
	}

	return overlap_mask;
}

TEMPLATE uint8 QUAL::GetChild(_TVarType pos, const Node* node) {
	return (pos > node->pos_) ? 1 : 0;
}

TEMPLATE uint8 QUAL::GetChild(_TVarType pos, const Node* node, _TVarType& child_pos) {
	_TVarType child_unloos_size_half = node->fixed_size_half_ * 0.5f;

	if (pos > node->pos_) {
		child_pos = node->pos_ + child_unloos_size_half;
		return 1;
	} else {
		child_pos = node->pos_ - child_unloos_size_half;
		return 0;
	}
}


TEMPLATE void QUAL::GetObjects(ObjectList& objects, _TVarType pos, _TVarType size_half) {
	GetObjects(objects, pos, size_half, root_node_);
}

TEMPLATE void QUAL::GetObjects(ObjectList& objects, _TVarType pos, _TVarType size_half, Node* node) {
	typename Node::EntryTable::Iterator iter;
	for (iter  = node->entry_table_.First();
		iter != node->entry_table_.End();
		++iter) {
		const typename Node::Entry& _entry = *iter;

		if (_entry.IsOverlapping(pos, size_half) == true) {
			objects.push_back(_entry.object_);
		}
	}

	unsigned overlap_mask = GetOverlaps(pos - node->pos_,
					       size_half,
					       node->size_half_ * 0.5f,
					       node->size_half_);
	overlap_mask &= node->child_mask_;

	if (overlap_mask != 0) {
		if ((overlap_mask & 1) != 0) {
			GetObjects(objects, pos, size_half, node->children_[0]);
		}
		if ((overlap_mask & 2) != 0) {
			GetObjects(objects, pos, size_half, node->children_[1]);
		}
	}
}


TEMPLATE unsigned QUAL::GetFullTreeMemSize() const {
	unsigned num_nodes = 0;
	int i;

	for (i = 1; i <= max_tree_depth_; i++) {
		num_nodes += (unsigned)(1 << i);
	}

	return num_nodes * sizeof(Node);
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

TEMPLATE LogDecorator LooseBinTree<_TKey, _TObject, _TVarType, _THashFunc>::log_(LogType::GetLogger(LogType::kGeneral), typeid(LooseBinTree));
