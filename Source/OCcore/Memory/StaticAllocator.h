// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef STATICALLOCATOR_H
#define STATICALLOCATOR_H

#include <vector>
#include <utility>
#include <stdint.h>
#include <unordered_map>

#include "IAllocator.h"
#include "FrameInfo.h"

#include "OCcore_export.h"

class OCCORE_EXPORT RawPoolAllocator : public IAllocator
{
    uint8_t* _mem;
    
    unsigned int _frameCount;
    int _poolBlock;
    int _totalBlock;
    
    std::unordered_map<unsigned int, FrameInfo> _frameMap; // frameNumber, frameInfo
    std::vector<unsigned int> _pointerMap; // frameNumber

public:
    RawPoolAllocator();
    ~RawPoolAllocator();
   
    void InitAllocator(std::vector<unsigned int>&, unsigned int frameSize);
    
    void* Allocate(unsigned int frameNumber, size_t size) override;
    void Deallocate(void* p) override;
    size_t allocated_size(void* p) override;
    
    void* GetData(int index, unsigned int size);
    int   GetFrameCount();
    unsigned int GetBufferIndex(unsigned int frameNumber);
    FrameState GetState(unsigned int frameNumber); 
    void SetFrameInfo(unsigned int frameNumber, FrameState state);    
};

#endif // STATICALLOCATOR_H
