#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <stdint.h>
#include <iostream>

enum FrameState
{
    Free,
    Allocated 
};

class FrameInfo
{
    FrameState _frameState;
    unsigned int _indexInBuffer;
    unsigned int _offsetInFile;
    unsigned int _frameSize;
    // more Info;
 
public:
   
    FrameInfo()
    {
      
    }
    
    FrameInfo(unsigned int fileOffset, unsigned int bufferIndex, unsigned int frameSize)
    {
      _frameState     = FrameState::Free;
      _indexInBuffer  = bufferIndex;
      _offsetInFile   = fileOffset;
      _frameSize      = frameSize;
    }
    
    unsigned int BufferIndex()
    {
      return _indexInBuffer;
    }      
    
    void SetBufferIndex(unsigned int index)
    {
      _indexInBuffer = index;   
    }
    
    void SetFrameState(FrameState state)
    {
      _frameState = state;  
    }
  
    FrameState State()
    {
      return _frameState;
    }
    
};

#endif
