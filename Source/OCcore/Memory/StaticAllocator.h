// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef STATICALLOCATOR_H
#define STATICALLOCATOR_H

#include <vector>

#include "IAllocator.h" 
#include "OCcore_export.h"
#include "../Image/VideoClip.h"

class OCCORE_EXPORT RawPoolAllocator : public IAllocator
{
    uint8_t* _mem;
    unsigned int _chunkSize;
    // will contain frameIndex of loaded frames at any block
    std::vector<unsigned int> _pointerMap; 
    
    unsigned int _totalBlock;
    unsigned int _poolBlock;
    
    // placed to get frameCount to processingTest, remove with  better alternative
    unsigned int _frameCount;  
    
public:
    RawPoolAllocator();
    ~RawPoolAllocator();
   
    void InitAllocator(unsigned int frameSize, unsigned int frameCount);
    void* GetBlockData(unsigned int poolBlock);
    void CheckBufferSize();
    void SetFrameInfo(unsigned int frameIndex, OC::Image::VideoClip &videoClip);
    unsigned int GetFrameCount();
    
    void* Allocate(unsigned int frameNumber) override;
    void Deallocate(void* p) override;
    size_t allocated_size(void* p) override;
};

#endif // STATICALLOCATOR_H
