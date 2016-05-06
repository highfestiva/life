
// Author:      Jonas Byström
// Copyright (c) Pixel Doctrine

// NOTES:

// This class is only intended for performance gain when frequently
// allocating objects of the same type.

// Warning: When deleting an object of this class, all memory that's
// allocated using it will be freed!

// Description of the algorithm:

// This class first allocates an array of chunk pointers, with room
// for 10 chunks by default. The first time you call Alloc(), the first
// chunk will be initialized with 8 objects. When you've allocated all
// objects, another chunk will be allocated with exponentially more
// objects (27 objects in the next level). How many objects are
// allocated per chunk is defined in the function GetNumNodes().

// This procedure goes on until all chunks are used, at which point
// the FastAllocator allocates even more chunks, and so on.

// The chunks are never deleted, which means that at some point in the
// program excecution, the	maximum peak will be reached and no more
// chunks will ever be allocated. At that point, the allocator will
// be performing at its best.

// How many chunks that will be allocated at first, and at every
// reallocation of new chunks, can be configured through the parameter
// num_chunks_per_alloc in the constructor.



#pragma once

#include "lepratypes.h"
#include <stddef.h>
#ifndef LEPRA_WINDOWS
#include <stdint.h>
#endif // Windows

#define FA_TEMPLATE template<class _Obj, class _LockType>
#define FA_QUAL FastAllocator<_Obj, _LockType>



namespace lepra {



class DummyLock {
public:
	inline void Acquire()    {}
	inline void TryAcquire() {}
	inline void Release() {}
};

template<class _Obj, class _LockType = DummyLock>
class FastAllocator {
public:

	FastAllocator(int num_chunks_per_alloc = 10);
	FastAllocator(FastAllocator& take_over);
	virtual ~FastAllocator();

	_Obj* Alloc();
	void  Free(_Obj* object);
	void  FreeAll();

protected:
private:

	void AllocateMoreChunks();
	void AllocateNextChunk();
	void ResetChunk(int chunk);

	// Returns the number of nodes in chunk chunk.
	inline int GetNumNodes(int chunk) {
		int x = chunk + 2;
		return x * x;
	}

	class Node {
	public:

		Node() :
			next_node_(0) {
		}

		inline static Node* GetNode(_Obj* object) {
			// Doing some magic to find the correct Node address.
			uint8* raw_node = (uint8*)object;
			Node* node = (Node*)0;
			raw_node -= (intptr_t)(&node->object_);
			//raw_node -= offsetof(Node, object_);
			return (Node*)raw_node;
		}

		Node* next_node_;
		_Obj  object_;
	};

	_LockType lock_;

	int num_chunks_;
	int current_chunk_;
	int num_chunks_per_alloc_;
	Node** chunk_;
	Node* first_free_node_;
};

FA_TEMPLATE FA_QUAL::FastAllocator(int num_chunks_per_alloc) :
	num_chunks_(0),
	current_chunk_(-1),
	num_chunks_per_alloc_(num_chunks_per_alloc),
	chunk_(0),
	first_free_node_(0) {
	AllocateMoreChunks();
}

FA_TEMPLATE FA_QUAL::FastAllocator(FastAllocator& take_over) :
	num_chunks_(take_over.num_chunks_),
	current_chunk_(take_over.current_chunk_),
	num_chunks_per_alloc_(take_over.num_chunks_per_alloc_),
	chunk_(take_over.chunk_),
	first_free_node_(take_over.first_free_node_) {
	// Reset take_over.
	take_over.num_chunks_ = 0;
	take_over.current_chunk_ = -1;
	take_over.chunk_ = 0;
	take_over.first_free_node_ = 0;
	take_over.AllocateMoreChunks();
}

FA_TEMPLATE FA_QUAL::~FastAllocator() {
	if (chunk_ != 0) {
//		FreeAll();

		for (int i = 0; i < num_chunks_; i++) {
			if (chunk_[i] != 0) {
				delete[] chunk_[i];
			}
		}

		delete[] chunk_;
		chunk_ = 0;
	}
}

FA_TEMPLATE _Obj* FA_QUAL::Alloc() {
	lock_.Acquire();

	if (!first_free_node_) {
		AllocateNextChunk();
	}

	_Obj* _object = &first_free_node_->object_;
	first_free_node_ = first_free_node_->next_node_;

	lock_.Release();

	return _object;
}

FA_TEMPLATE void FA_QUAL::Free(_Obj* object) {
	lock_.Acquire();

	Node* node = Node::GetNode(object);
	node->next_node_ = first_free_node_;
	first_free_node_ = node;

	lock_.Release();
}

FA_TEMPLATE void FA_QUAL::FreeAll() {
	lock_.Acquire();

	for (int i = 0; i < num_chunks_; i++) {
		ResetChunk(i);
	}

	if (current_chunk_ > 0) {
		current_chunk_ = 0;
		first_free_node_ = chunk_[0];
	}

	lock_.Release();
}

FA_TEMPLATE void FA_QUAL::AllocateMoreChunks() {
	Node** _chunk = new Node*[num_chunks_ + num_chunks_per_alloc_];

	int i;
	for (i = 0; i < num_chunks_; i++) {
		_chunk[i] = chunk_[i];
	}

	for (; i < num_chunks_ + num_chunks_per_alloc_; i++) {
		_chunk[i] = 0;
	}

	if (chunk_ != 0) {
		delete[] chunk_;
	}

	chunk_ = _chunk;
	num_chunks_ += num_chunks_per_alloc_;
}

FA_TEMPLATE void FA_QUAL::AllocateNextChunk() {
	current_chunk_++;
	if (current_chunk_ >= num_chunks_) {
		AllocateMoreChunks();
	}

	// We need to check wether the chunk is already allocated or not,
	// because FreeAll() doesn't free this memory.
	if (chunk_[current_chunk_] == 0) {
		chunk_[current_chunk_] = new Node[GetNumNodes(current_chunk_)];
	}

	// Since this function is only called if first_free_node_
	// is null, we can safely set it without further checking.
	first_free_node_ = chunk_[current_chunk_];

	ResetChunk(current_chunk_);
}

FA_TEMPLATE void FA_QUAL::ResetChunk(int chunk) {
	Node* node = chunk_[chunk];

	// Check needed, since we may call this function on a chunk
	// with no allocated nodes.
	if (node) {
		int loop_max = GetNumNodes(chunk) - 1;
		int i;
		for (i = 0; i < loop_max; i++) {
			node[i].next_node_ = &node[i + 1];
		}
		node[i].next_node_ = 0;
	}
}



}



#undef FA_TEMPLATE
#undef FA_QUAL
