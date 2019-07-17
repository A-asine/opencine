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
    _poolBlock(0),
    _frameCount(0)
{
}

RawPoolAllocator::~RawPoolAllocator()
{  
	if(_mem != nullptr)
	{
		delete [] _mem;
	}
}

void RawPoolAllocator::InitAllocator(unsigned int frameSize, unsigned int frameCount)
{
    _chunkSize  = frameSize;
    _frameCount = frameCount; 
    // allocating 512 MB space (approx) and also keeping it as multiple of frameSize.
    _mem = new uint8_t[SIZE_LIMIT - SIZE_LIMIT % _chunkSize];  
    
    // Break the whole space into chunks having size each equal to frameSize
    _totalBlock = (SIZE_LIMIT / _chunkSize) * 3;
    
    // keep all info related to blocks in _pointerMap(set to FRAME_ABSENT initally) 
    _pointerMap.resize(_totalBlock, FRAME_ABSENT);
    
    OC_LOG_INFO("Total size of buffer: " + std::to_string(SIZE_LIMIT - SIZE_LIMIT % _chunkSize) 
                 + " | total number of blocks: " + std::to_string(_totalBlock)); 
}

void* RawPoolAllocator::Allocate(unsigned int frameNumber)
{    
 	_pointerMap[_poolBlock] = frameNumber;
	void* ptr = &_mem[_poolBlock * _chunkSize];

    _poolBlock += 1; 
	return ptr;
}

// The method checks, if the particular block is allocated by a frame
// if not allocated , we can set related info(index in Buffer) in videoclip class
// if already allocated, We have to free that Frame from there, and place new frame

void RawPoolAllocator::SetFrameInfo(unsigned int frameIndex, OC::Image::VideoClip &videoClip)
{
    if(_pointerMap[_poolBlock] != FRAME_ABSENT)     
    {    
         unsigned int presentFrame = _pointerMap[_poolBlock];
         videoClip.SetBufferIndex(presentFrame, _poolBlock);
         videoClip.SetFrameState(presentFrame, OC::Image::FrameState::Free);  // Free the frame
    }    
    
    videoClip.SetBufferIndex(frameIndex, _poolBlock);    
    videoClip.SetFrameState(frameIndex, OC::Image::FrameState::Allocate);    // allocate new frame 
}

// if size of buffer is empty, we can always loop from first block in buffer
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

unsigned int RawPoolAllocator::GetFrameCount()
{
    return _frameCount;
}

void RawPoolAllocator::Deallocate(void* p)
{

}

size_t RawPoolAllocator::allocated_size(void* p)
{
	return 0;
}
