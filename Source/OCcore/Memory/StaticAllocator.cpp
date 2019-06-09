// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#include "StaticAllocator.h"

RawPoolAllocator::RawPoolAllocator(size_t pageSize) :
	_mem(nullptr),
	_currentOffset(0)
{
    _mem = new uint8_t[pageSize];
}

RawPoolAllocator::~RawPoolAllocator()
{
	if(_mem != nullptr)
	{
		delete [] _mem;
	}
}

void* RawPoolAllocator::Allocate(size_t size/*, size_t align*/)
{
	_pointerMap.push_back(std::make_pair(_currentOffset, size));
	
	void* ptr = _mem + _currentOffset;
	_currentOffset += size;

	return ptr;
}

void* RawPoolAllocator::GetData(int index)
{
    void* ptr = &_mem[_pointerMap[index].first];
    return ptr;
}
 
int RawPoolAllocator::GetFrameCount()
{
    return (_pointerMap.size());
}

void RawPoolAllocator::Deallocate(void* p)
{

}

size_t RawPoolAllocator::allocated_size(void* p)
{
	return 0;
}

