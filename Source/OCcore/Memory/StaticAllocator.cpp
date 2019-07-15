// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#include <Log/Logger.h>
#include <string>

#include "StaticAllocator.h"

#define SIZE_LIMIT 512 * 1024 * 1024
#define FRAME_ABSENT -1

RawPoolAllocator::RawPoolAllocator() :   
    _mem(nullptr),
    _chunkSize(0),
    _totalBlock(0),
    _poolBlock(0)
{
}

RawPoolAllocator::~RawPoolAllocator()
{  
	if(_mem != nullptr)
	{
		delete [] _mem;
	}
}

void RawPoolAllocator::InitAllocator(unsigned int frameSize, OC::Image::VideoClip &videoClip)
{
    _chunkSize = frameSize;
    
    _mem = new uint8_t[SIZE_LIMIT - SIZE_LIMIT % _chunkSize];
    _totalBlock = (SIZE_LIMIT / _chunkSize) * 3;
    
    _pointerMap.resize(_totalBlock, FRAME_ABSENT);
    _videoClip = videoClip;
    
    OC_LOG_INFO("Total size of buffer: " + std::to_string(SIZE_LIMIT - SIZE_LIMIT % _chunkSize) 
                 + " | total number of blocks: " + std::to_string(_totalBlock)); 
}

void* RawPoolAllocator::Allocate(unsigned int frameNumber)
{   
    if(_pointerMap[_poolBlock] != FRAME_ABSENT) // Free the Block
    { 
        unsigned int presentFrame = _pointerMap[_poolBlock]; 
    
        _videoClip.SetBufferIndex(presentFrame, FRAME_ABSENT);
        _videoClip.SetFrameState(presentFrame, OC::Image::FrameState::Free);
    }
    
    CheckBufferSize();
    
	_pointerMap[_poolBlock] = frameNumber;
	void* ptr = &_mem[_poolBlock * _chunkSize];
	
	if(_poolBlock % 3 == 0)   // Allocate a new Block 
	{
	    _videoClip.SetBufferIndex(frameNumber, _poolBlock);
	    _videoClip.SetFrameState(frameNumber, OC::Image::FrameState::Allocate);   
	}
	
    _poolBlock += 1; 
	return ptr;
}

void RawPoolAllocator::CheckBufferSize()
{
    if(_poolBlock == _totalBlock)
    {  
       _poolBlock = 0;
    }   
}

void* RawPoolAllocator::GetBlockData(unsigned int poolBlock)
{
    void* ptr = &_mem[poolBlock * _chunkSize];
    return ptr;
}

void RawPoolAllocator::Deallocate(void* p)
{

}

size_t RawPoolAllocator::allocated_size(void* p)
{
	return 0;
}
