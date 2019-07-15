// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef STATICALLOCATOR_H
#define STATICALLOCATOR_H

#include <vector>
#include "../Image/VideoClip.h"

#include "IAllocator.h" 
#include "OCcore_export.h"

class OCCORE_EXPORT RawPoolAllocator : public IAllocator
{
    uint8_t* _mem;
    unsigned int _chunkSize;
    std::vector<unsigned int> _pointerMap; // frameNumber
    
    unsigned int _totalBlock;
    unsigned int _poolBlock;
    
    OC::Image::VideoClip _videoClip;
    
public:
    RawPoolAllocator();
    ~RawPoolAllocator();
   
    void InitAllocator(unsigned int frameSize, OC::Image::VideoClip& videoClip);
    void* GetBlockData(unsigned int poolBlock);
    void CheckBufferSize();
    
    void* Allocate(unsigned int frameNumber) override;
    void Deallocate(void* p) override;
    size_t allocated_size(void* p) override;
};

#endif // STATICALLOCATOR_H
