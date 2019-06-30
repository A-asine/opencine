#ifndef AVICONTAINER_H
#define AVICONTAINER_H

#include <unordered_map>
#include <string>

#include <Memory/StaticAllocator.h>
#include <Image/BilinearDebayer.h>
#include <Image/OCImage.h>

#include "AVIStruct.h"

class AVIContainer
{
   unsigned int _height;
   unsigned int _width;
   uint16_t _frameCount;
   
   uint8_t* _aviFile;
   std::ofstream avi;
   
   std::unordered_map<std::string, unsigned long> headerSize;
   // more Info 
   
public:
   
   AVIContainer(unsigned int height, unsigned int width, uint16_t frameCount);
   
   ~AVIContainer();
   
   void CreateRIFFheader(int& offset);
   void CreateHDRLheader(int& offset);
   void CreateAVIHheader(int& offset);
   void CreateSTRLheader(int& offset);
   void CreateSTRHheader(int& offset);
   void CreateSTRFheader(int& offset);
   void CreateBITheader(int& offset);
   void CreateMOVIheader(int& offset);
   
   uint32_t CreateFourCC(std::string);
   unsigned long int FindSize(std::string);
   
   void AddFrames(OC::Image::OCImage &image);  // temp function
   void WriteToFile(int &offset);
   
};

#endif
