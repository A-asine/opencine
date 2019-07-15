#include "VideoClip.h"
#include <utility>

using namespace OC::Image;

VideoClip::VideoClip() : 
           _frameCount(0)
{
}           

VideoClip::~VideoClip()
{
}

void VideoClip::InitVideoClip(unsigned int frameCount)
{   
     _frameCount = frameCount;
     _frameMap.resize(frameCount + 1); 
     
     std::fill(_frameMap.begin() + 1, _frameMap.end(), std::make_pair(FrameState::Free, -1));
}           

void VideoClip::SetFrameState(unsigned int frameIndex, FrameState state)
{
     _frameMap[frameIndex].first = state;
}

FrameState VideoClip::GetFrameState(unsigned int frameIndex)
{
     return _frameMap[frameIndex].first;
}

void VideoClip::SetBufferIndex(unsigned int frameIndex, unsigned int BufferIndex)
{
     _frameMap[frameIndex].second = BufferIndex;
}

unsigned int VideoClip::GetBufferIndex(unsigned frameIndex)
{
     return _frameMap[frameIndex].second;
}

void VideoClip::SetFrameCount(unsigned int frameCount)
{
     _frameCount = frameCount;
} 

unsigned int VideoClip::GetFrameCount()
{
     return _frameCount;
}


