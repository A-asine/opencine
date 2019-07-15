#ifndef VIDEOCLIP_H
#define VIDEOCLIP_H

#include <vector>

namespace OC
{
   namespace Image
   {    
       enum FrameState
       {
           Free,
           Allocate
       };
       
       class VideoClip
       {
           std::vector< std::pair<FrameState, unsigned int> > _frameMap;
           unsigned int _frameCount;
           
           public:
           
           VideoClip();
           ~VideoClip();
           
           void InitVideoClip(unsigned int frameCount);
          
           FrameState GetFrameState(unsigned int frameIndex);
           void SetFrameState(unsigned int frameIndex, FrameState state);
           
           unsigned int GetBufferIndex(unsigned int frameIndex);
           void SetBufferIndex(unsigned int frameIndex, unsigned int bufferIndex); 
           
           unsigned int GetFrameCount();
           void SetFrameCount(unsigned int frameCount); 
       };
   }
}

#endif
