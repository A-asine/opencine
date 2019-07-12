// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#include <Log/Logger.h>
#include <string>

#include "StaticAllocator.h"

#define SzLimit 15 * 1024 * 1024

RawPoolAllocator::RawPoolAllocator() :   
    _mem(nullptr),
    _frameCount(0),
    _poolBlock(0),
    _totalBlock(0)
{
}

RawPoolAllocator::~RawPoolAllocator()
{  
	if(_mem != nullptr)
	{
		delete [] _mem;
	}
}

void RawPoolAllocator::InitAllocator(std::vector<unsigned int>& frameOffset, unsigned int frameSize)
{
    _frameCount = frameOffset.size();  
 
    for(unsigned int frameNumber = 1; frameNumber <= _frameCount; frameNumber++)
    {  
       FrameInfo frameInfo(frameOffset[frameNumber - 1], -1, frameSize);
       _frameMap.insert(std::make_pair(frameNumber, frameInfo));
    } 
    
    int numBlock = 0;
    // bufferSize will in multiple of frameSize
    numBlock = (frameSize * _frameCount) <= SzLimit ?  _frameCount : (SzLimit / frameSize);
    
    _totalBlock = numBlock * 3;
    // Each Frame will contain three channels
    _mem = new uint8_t[numBlock * frameSize * 3];
    _pointerMap.resize(_totalBlock);  
      
    OC_LOG_INFO("Total size of buffer: " + std::to_string(numBlock * frameSize * 3) 
                 + " | total number of blocks: " + std::to_string(_totalBlock)); 
}

void* RawPoolAllocator::Allocate(unsigned int frameNumber, size_t size)
{   
    if(_pointerMap[_poolBlock] != 0)
    { 
      unsigned int framePresent = _pointerMap[_poolBlock]; 
      _frameMap[framePresent].SetBufferIndex(-1);
      _frameMap[framePresent].SetFrameState(FrameState::Free);
    }
    
    _pointerMap[_poolBlock] = frameNumber;
    void* ptr = &_mem[_poolBlock * size];
    _poolBlock += 1; 
    
    return ptr;
}

void RawPoolAllocator::SetFrameInfo(unsigned int frameNumber, FrameState state)
{   
    if(_poolBlock == _totalBlock)
    {        
      std::cout << std::endl <<"Buffer has No space left, start from PoolBlock 0 again" << std::endl;
      _poolBlock = 0;              // for looping purpose
    }
     
    std::string redPool   = std::to_string(_poolBlock);
    std::string greenPool = std::to_string(_poolBlock + 1);
    std::string bluePool  = std::to_string(_poolBlock + 2);
    
    std::string poolBlock = "PoolBlock :" + redPool + "," + greenPool + "," + bluePool + "," 
                             + "are now occupied with R,G,B channel of frame :" + std::to_string(frameNumber);
                                 
    OC_LOG_INFO(poolBlock);
     
    _frameMap[frameNumber].SetBufferIndex(_poolBlock); // set index of red channel only
    _frameMap[frameNumber].SetFrameState(state); 
}


unsigned int RawPoolAllocator::GetBufferIndex(unsigned int frameNumber)
{   
    return _frameMap[frameNumber].GetBufferIndex();
}

FrameState RawPoolAllocator::GetState(unsigned int frameNumber)
{
    return _frameMap[frameNumber].GetFrameState();
} 

void* RawPoolAllocator::GetData(int index, unsigned int size)
{
    void* ptr = &_mem[index * size];
    return ptr;
}
 
int RawPoolAllocator::GetFrameCount()
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

