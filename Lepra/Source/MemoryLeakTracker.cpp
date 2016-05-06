
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/memoryleaktracker.h"

//#define MEMOVERWRITE_DETECT	1



#if defined(_DEBUG) && defined(LEPRA_WINDOWS) && defined(MEMLEAK_DETECT)

// Remove the definition...
#undef new

#pragma warning(disable: 4996)	// Deprecated functions are not to be used, as they may be removed in future versions. Circumvent problem for example in strings by using str instead.

#include <new>
#include <exception>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <crtdbg.h>
#include "../include/spinlock.h"



namespace lepra {

class MemoryLeakTracker {
public:

	friend class MemoryLeakDetector;

	static void AddTrack(void* addr, unsigned long size, const char* f_name, unsigned long num);
	static bool RemoveTrack(void* addr);
	static void DumpLeaks();

	static int max_allocated_;
	static int currently_allocated_;
	static SpinLock spin_lock_;
	static bool leaks_dumped;
};

}



void* operator new(size_t size, const char* file_name, int line) {
	void* _pointer = (void*)malloc(size);
	if (_pointer == 0)
		throw std::bad_alloc(); // ANSI/ISO compliant behavior

	lepra::MemoryLeakTracker::AddTrack(_pointer, (unsigned long)size, file_name, line);

	return _pointer;
}

/*void* operator new(size_t size) {
	void* _pointer = (void*)malloc(size);
	if (_pointer == 0)
		throw std::bad_alloc(); // ANSI/ISO compliant behavior

	lepra::MemoryLeakTracker::AddTrack(_pointer, size, "<Unknown file - make sure to include lepra.h>", 0);

	return _pointer;
}
*/
void operator delete(void* pointer, const char*, int) {
	//_ASSERT(pointer != 0);
	if (pointer != 0) {
		lepra::MemoryLeakTracker::RemoveTrack(pointer);
		free(pointer);
	}
}

void operator delete(void* pointer) {
	//_ASSERT(pointer != 0);
	if (pointer != 0) {
		lepra::MemoryLeakTracker::RemoveTrack(pointer);
		free(pointer);
	}
}

void operator delete[](void* pointer) {
	//_ASSERT(pointer != 0);
	if (pointer != 0) {
		lepra::MemoryLeakTracker::RemoveTrack(pointer);
		free(pointer);
	}
}



namespace lepra {

struct ALLOC_INFO {
	void*		address_;
	unsigned long	size_;
	char		file_[256];
	unsigned long	line_;
	ALLOC_INFO*	next_alloc_info_;
};

ALLOC_INFO* g_first_info = 0;
ALLOC_INFO* g_last_info = 0;
int g_num_false_leaks = 0;

int MemoryLeakTracker::max_allocated_ = 0;
int MemoryLeakTracker::currently_allocated_ = 0;
SpinLock MemoryLeakTracker::spin_lock_;
bool MemoryLeakTracker::leaks_dumped = false;

void MemoryLeakTracker::AddTrack(void* addr, unsigned long size, const char* f_name, unsigned long num) {
	// TODO: optimize HARD!!!

	spin_lock_.UncheckedAcquire();

	ALLOC_INFO* info;

	info = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));
	info->address_ = addr;
	strncpy(info->file_, f_name, 255);
	info->line_ = num;
	info->size_ = size;
	info->next_alloc_info_ = 0;

	if (g_first_info == 0) {
		g_first_info = info;
		g_last_info  = info;
	} else {
		g_last_info->next_alloc_info_ = info;
		g_last_info = info;
	}

	currently_allocated_ += size;
	if (currently_allocated_ > max_allocated_)
		max_allocated_ = currently_allocated_;

	spin_lock_.Release();
};

bool MemoryLeakTracker::RemoveTrack(void* addr) {
	// TODO: optimize HARD!!!

	spin_lock_.UncheckedAcquire();

	ALLOC_INFO* current_info = g_first_info;
	ALLOC_INFO* prev_info = 0;

	bool track_found = false;

	while (current_info != 0) {
		if (current_info->address_ == addr) {
			currently_allocated_ -= current_info->size_;

			if (g_first_info == current_info)
				g_first_info = current_info->next_alloc_info_;
			if (g_last_info == current_info)
				g_last_info = prev_info;

			if (prev_info != 0) {
				prev_info->next_alloc_info_ = current_info->next_alloc_info_;
			}

			free(current_info);

			track_found = true;
			break;
		}

		prev_info = current_info;
		current_info = current_info->next_alloc_info_;
	}

	/*if (track_found == false) {
		char s[256];
		if (leaks_dumped == true) {
			g_num_false_leaks++;
			sprintf(s, "False memory leak reported at address 0x%p, (Count: %d)\n", addr, g_num_false_leaks);
		} else {
			sprintf(s, "Released unknown memory address 0x%p (allocated in BSS = global object)\n", addr);
		}
		OutputDebugStringA(s);
	}*/

	spin_lock_.Release();

	return track_found;
};

void MemoryLeakTracker::DumpLeaks() {
	spin_lock_.UncheckedAcquire();

	unsigned long total_size = 0;
	char buf[1024];

	ALLOC_INFO* current_info = g_first_info;

	// Get the maximum file name length in order to pad the output
	// nice.
	int max_length = 0;
	int max_length2 = 0;
	int max_line_digits = 0;
	while (current_info != 0) {
		sprintf(buf, "%s()", current_info->file_);
		int length = (int)::strlen(buf);

		sprintf(buf, "%d", current_info->line_);
		int line_digits = (int)::strlen(buf);

		length += line_digits;

		if (length > max_length)
			max_length = length;
		if (line_digits > max_line_digits)
			max_line_digits = line_digits;

		current_info = current_info->next_alloc_info_;
	}
	max_length += 4;

	// 22 is the length of "ADDRESS 0x00000000    ".
	max_length2 = max_length + 22 + max_line_digits + 1;

	current_info = g_first_info;
	g_first_info = 0;
	g_last_info = 0;

	int num_unfreed = 0;

	while (current_info != 0) {
		sprintf(buf, "%s(%d)", current_info->file_, current_info->line_);
		int length = (int)::strlen(buf);

		// Pad with whitespace.
		int i;
		for (i = length; i < max_length; i++) {
			buf[i] = ' ';
		}
		buf[max_length] = 0;

		sprintf(&buf[max_length], "ADDRESS 0x%p    %d", current_info->address_,
			current_info->size_);

		length = (int)::strlen(buf);

		// Pad with more whitespace.
		for (i = length; i < max_length2; i++) {
			buf[i] = ' ';
		}
		buf[max_length2] = 0;

		sprintf(&buf[max_length2], "bytes unfreed\n");

		OutputDebugStringA(buf);
		total_size += current_info->size_;

		ALLOC_INFO* info = current_info;
		current_info = current_info->next_alloc_info_;
		free(info);

		num_unfreed++;
	}

	sprintf(buf, "-----------------------------------------------------------\n");
	OutputDebugStringA(buf);
	sprintf(buf, "Total Unfreed: %d bytes in %d allocations.\n", total_size, num_unfreed);
	OutputDebugStringA(buf);
	sprintf(buf, "Peak alloc: %d bytes.\n", max_allocated_);
	OutputDebugStringA(buf);

	leaks_dumped = true;

	spin_lock_.Release();
};

/*
void MemoryLeakTracker::InitMemoryLeakDetection() {
	// Clear all records.
	ALLOC_INFO* current_info = g_first_info;

	g_first_info = 0;
	g_last_info = 0;

	while (current_info != 0) {
		ALLOC_INFO* info = current_info;
		current_info = current_info->next_alloc_info_;
		free(info);
	}
}
*/

// TODO: place this in a systemcall so that we're the last function to
// execute (not as in this case, when we're just another global object
// being destroyed.
class MemoryLeakDetector {
public:
	MemoryLeakDetector(){}

	~MemoryLeakDetector() {
		MemoryLeakTracker::DumpLeaks();
	}
};

MemoryLeakDetector g_leak_detector;

}

#else // !<Memleak detection>

#include <new>
#include <exception>
#include <cstdlib>
#include "../include/lepraassert.h"

void* operator new(size_t size) {
#if !defined(MEMOVERWRITE_DETECT)	// Normal operation, no overwrite detection
	void* _pointer = malloc(size);
	if (_pointer == 0) throw std::bad_alloc(); // ANSI/ISO compliant behavior
	return _pointer;
#else // Overwrite detection - YEAH!
	char* _pointer = (char*)malloc(size+12);
	if (_pointer == 0) throw std::bad_alloc(); // ANSI/ISO compliant behavior
	*(int*)_pointer = 0x5AF00FA5;
	*(int*)(_pointer+4) = (int)size;
	*(int*)(_pointer+size+8) = 0x55C3F0A9;
	return _pointer+8;
#endif // Normal alloc / With overwrite detection.
}

void operator delete(void* pointer) {
#if !defined(MEMOVERWRITE_DETECT)	// Normal operation, no overwrite detection
	free(pointer);
#else // Overwrite detection - YEAH!
	if (!pointer) return;
	char* _pointer = (char*)pointer - 8;
	deb_assert(*(int*)_pointer == 0x5AF00FA5);
	int _size = *(int*)(_pointer+4);
	deb_assert(*(int*)(_pointer+_size+8) == 0x55C3F0A9);
	free(_pointer);
#endif // Normal alloc / With overwrite detection.
}

void operator delete[](void* pointer) {
#if !defined(MEMOVERWRITE_DETECT)	// Normal operation, no overwrite detection
	free(pointer);
#else // Overwrite detection - YEAH!
	if (!pointer) return;
	char* _pointer = (char*)pointer - 8;
	deb_assert(*(int*)_pointer == 0x5AF00FA5);
	int _size = *(int*)(_pointer+4);
	deb_assert(*(int*)(_pointer+_size+8) == 0x55C3F0A9);
	free(_pointer);
#endif // Normal alloc / With overwrite detection.
}

#endif // <Memleak detection>/!<Memleak detection>
