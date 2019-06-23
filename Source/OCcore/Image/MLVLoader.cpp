// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#include "MLVLoader.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include "EndianHelper.h"

#include "BayerFrameDownscaler.h"

using namespace OC::Image;
using namespace OC::DataProvider;

#define BIND_FUNC(x) std::bind(&MLVLoader::x, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

mlv_file_hdr_t MLVLoader::ReadHeader(uint8_t* buffer, unsigned int& bufferPosition)
{
    mlv_file_hdr_t header;

    header.fileMagic       = GetUInt32(&buffer[bufferPosition], bufferPosition);
    header.blockSize       = GetUInt32(&buffer[bufferPosition], bufferPosition);
    std::copy(&buffer[bufferPosition], &buffer[bufferPosition + 8], header.versionString);
    bufferPosition  += 8;
  
    header.fileGuid        = GetUInt64(&buffer[bufferPosition], bufferPosition);
    header.fileNum         = GetUInt16(&buffer[bufferPosition], bufferPosition);
    header.fileCount       = GetUInt16(&buffer[bufferPosition], bufferPosition);
    header.fileFlags       = GetUInt32(&buffer[bufferPosition], bufferPosition);
    header.videoClass      = GetUInt16(&buffer[bufferPosition], bufferPosition);
    header.audioClass      = GetUInt16(&buffer[bufferPosition], bufferPosition);
    header.videoFrameCount = GetUInt32(&buffer[bufferPosition], bufferPosition);
    header.audioFrameCount = GetUInt32(&buffer[bufferPosition], bufferPosition);
    header.sourceFpsNom    = GetUInt32(&buffer[bufferPosition], bufferPosition);
    header.sourceFpsDenom  = GetUInt32(&buffer[bufferPosition], bufferPosition);

    return header;
}

mlv_hdr_t MLVLoader::ReadBlockHeader(uint8_t* buffer, unsigned int& bufferPosition)
{
    mlv_hdr_t block;

    block.blockType = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.blockSize = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.timestamp = GetUInt64(&buffer[bufferPosition], bufferPosition);
    return block;
}

raw_info MLVLoader::ReadRawInfo(uint8_t* buffer, unsigned int& bufferPosition)
{
    raw_info block;

    block.api_version      = GetUInt32(&buffer[bufferPosition], bufferPosition);
    // block.do_not_use_this = GetUInt32(&buffer[bufferPosition], bufferPosition);
    bufferPosition         += 4; // step over some obsolete struct member
    block.height           = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.width            = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.pitch            = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.frame_size       = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.bits_per_pixel   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.black_level      = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.white_level      = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.crop.origin[0]   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.crop.origin[1]   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.crop.size[0]     = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.crop.size[1]     = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.active_area.y1   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.active_area.x1   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.active_area.y2   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.active_area.x2   = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.exposure_bias[0] = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.exposure_bias[1] = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.cfa_pattern      = GetUInt32(&buffer[bufferPosition], bufferPosition);
    block.calibration_illuminant1 = GetUInt32(&buffer[bufferPosition], bufferPosition);
    
    unsigned int tempLength = 18 * sizeof(int32_t);
    // TODO: Check the SIGABRT when the next line is active
    // std::copy(&buffer[bufferPosition], &buffer[bufferPosition] + tempLength, block.color_matrix1); // 4 * 18 -> uint32_t * 18
    bufferPosition += tempLength;
    block.dynamic_range = GetUInt32(&buffer[bufferPosition], bufferPosition);

    return block;
}

void MLVLoader::ReadRAWI(uint8_t* buffer, unsigned int& bufferPosition, mlv_hdr_t& blockHeader)
{
    blockRAWI.xRes    = GetUInt16(&buffer[bufferPosition], bufferPosition);
    blockRAWI.yRes    = GetUInt16(&buffer[bufferPosition], bufferPosition);
    // block.rawInfo  = &buffer[bufferPosition];
    blockRAWI.rawInfo = ReadRawInfo(buffer, bufferPosition);
}

void MLVLoader::ReadVIDF(uint8_t* buffer, unsigned int& bufferPosition, mlv_hdr_t& blockHeader)
{
    blockVIDF.frameNumber = GetUInt32(&buffer[bufferPosition], bufferPosition);
    blockVIDF.cropPosX    = GetUInt16(&buffer[bufferPosition], bufferPosition);
    blockVIDF.cropPosY    = GetUInt16(&buffer[bufferPosition], bufferPosition);
    blockVIDF.panPosX     = GetUInt16(&buffer[bufferPosition], bufferPosition);
    blockVIDF.panPosY     = GetUInt16(&buffer[bufferPosition], bufferPosition);
    blockVIDF.frameSpace  = GetUInt32(&buffer[bufferPosition], bufferPosition);
    // uint8_t	frameData

    _sourceData.push_back(bufferPosition + blockVIDF.frameSpace);
   
    bufferPosition += blockHeader.blockSize - (sizeof(mlv_vidf_hdr_t) + sizeof(mlv_hdr_t));
}

MLVLoader::MLVLoader()
{
    mlvFunc.insert(std::make_pair("RAWI", BIND_FUNC(ReadRAWI)));
    mlvFunc.insert(std::make_pair("VIDF", BIND_FUNC(ReadVIDF)));
    // more blocks, when required 
}

bool MLVLoader::CheckFormat(uint8_t* data, std::streamsize size)
{
    bool result = false;

    uint32_t magicBytes = static_cast<uint32_t>((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
    if(magicBytes == MLVI_MAGIC)
    {
        result = true;
    }

    return result;
}

void SwapEndianess16BitArray(uint8_t* array, unsigned int size)
{
    for(unsigned int i = 0; i < size - 1; i += 2)
    {
        std::swap(array[i], array[i + 1]);
    }
}

void MLVLoader::InitOCImage(Image::OCImage& image, uint16_t width, uint16_t height, uint32_t bits_per_pixel, 
                            unsigned int& imageDataSize, ImageFormat& imageFormat)
{  
    unsigned int dataSize = width * height;
   
    image.SetWidth(width);
    image.SetHeight(height);
    image.SetBayerPattern(BayerPattern::RGGB);
    
    switch(bits_per_pixel)
    {
       case 14:
           imageFormat = ImageFormat::Integer14;
           image.SetFormat(static_cast<ImageFormat>(bits_per_pixel));
           imageDataSize = static_cast<unsigned int>(dataSize * 1.75f); // 14bit / 8 bit
           break;
    }
   
}

void MLVLoader::Load(uint8_t* data, unsigned int size, Image::OCImage& image, RawPoolAllocator& allocator)
{
    // TODO: Add handlng of endianess 
    unsigned int bufferPosition = 0;
    mlv_file_hdr_t mlvHeader = ReadHeader(data, bufferPosition);
    _targetData = data;
    
    unsigned int blockHeaderSize = sizeof(mlv_hdr_t);
  
    while(bufferPosition < size)
    {
        mlv_hdr_t blockHeader = ReadBlockHeader(data, bufferPosition);
        std::string s(reinterpret_cast<char const*>(&blockHeader.blockType), 4);
        
        if(mlvFunc.find(s) != mlvFunc.end())
        {   
            MLVFunc callable = mlvFunc[s];
            callable(data, bufferPosition, blockHeader); 
        }
        else
        {   
            if(s != "NULL")
            std::cout << "No processing implemented for block. Type: " << s << " Size: " << blockHeader.blockSize << " bytes" 
            << std::endl;
            
            bufferPosition += blockHeader.blockSize - blockHeaderSize;
        }
    }
    
    unsigned int frameSize = blockRAWI.xRes * blockRAWI.yRes;
        
    allocator.InitAllocator(_sourceData, frameSize);
}

void MLVLoader::ProcessFrame(unsigned int frameNumber , Image::OCImage& image, RawPoolAllocator& allocator)
{          
     if(allocator.GetState(frameNumber) == FrameState::Allocated)
     {  
         std::cout << "frame :" << frameNumber << "already present in Buffer" << std::endl; 
      
         unsigned int index = allocator.GetBufferIndex(frameNumber);
         image.SetRedChannel(allocator.GetData(index));
         image.SetGreenChannel(allocator.GetData(index + 1));
         image.SetBlueChannel(allocator.GetData(index + 2));
         
         return;
     }
     
     std::unique_ptr<BayerFrameDownscaler> frameProcessor(new BayerFrameDownscaler());
     unsigned int dataSize = blockRAWI.xRes * blockRAWI.yRes; 
     
     unsigned int imageDataSize = 0;
     ImageFormat imageFormat = ImageFormat::Integer12;
      
     InitOCImage(image, blockRAWI.xRes, blockRAWI.yRes, blockRAWI.rawInfo.bits_per_pixel, imageDataSize, imageFormat);
     
     allocator.SetFrameInfo(frameNumber, FrameState::Allocated);
       
     image.SetRedChannel(allocator.Allocate(frameNumber, dataSize));
     image.SetGreenChannel(allocator.Allocate(frameNumber, dataSize));
     image.SetBlueChannel(allocator.Allocate( frameNumber, dataSize)); 
    
     unsigned int offset = _sourceData[frameNumber - 1];       

     SwapEndianess16BitArray(&_targetData[offset], imageDataSize);

     frameProcessor->SetData(&_targetData[offset], image, imageFormat);
    //frameProcessor->SetLinearizationData(linearizationTable, linearizationLength);
     frameProcessor->Process();           
}


