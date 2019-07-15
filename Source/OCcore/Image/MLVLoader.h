// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef MLVLOADER_H
#define MLVLOADER_H

#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

#include <Memory/StaticAllocator.h>
#include "BayerFrameDownscaler.h"

#include "IImageLoader.h"
#include "OCImage.h"
#include "VideoClip.h"
#include "mlv_structure_mod.h"

typedef std::function < void(uint8_t*, unsigned int&, mlv_hdr_t& )> MLVFunc;
// void is return type, uint8_t* is file data, unsigned int is offset to filedata, mlv_hdr_t is block of MLV 

namespace OC
{
    namespace DataProvider
    {
        class MLVLoader : public IImageLoader
        {   
            #define MLVI_MAGIC 0x49564c4d
        
            mlv_rawi_hdr_t blockRAWI;
            mlv_vidf_hdr_t blockVIDF;
            // more blocks, when required
            
            uint8_t* _targetData;
            std::vector<unsigned int> _sourceData;
            std::unordered_map<std::string, MLVFunc> mlvFunc; 
            
            BayerFrameDownscaler* _frameProcessor = new BayerFrameDownscaler();
            
            mlv_file_hdr_t ReadHeader(uint8_t* buffer, unsigned int& bufferPosition);
            mlv_hdr_t ReadBlockHeader(uint8_t* buffer, unsigned int& bufferPosition);
            raw_info ReadRawInfo(uint8_t* buffer, unsigned int& bufferPosition);
            
            void ReadRAWI(uint8_t* buffer, unsigned int& bufferPosition, mlv_hdr_t& blockHeader);
            void ReadVIDF(uint8_t* buffer, unsigned int& bufferPosition, mlv_hdr_t& blockHeader);
            
            void ProcessTags();
            
            void InitOCImage(Image::OCImage& image, uint16_t width, uint16_t height, uint32_t bits_per_pixel,
                              unsigned int& imageDataSize, Image::ImageFormat& imageFormat);
        public:
            MLVLoader();
            ~MLVLoader();
            void Load(uint8_t* data, unsigned int size, Image::OCImage& image, Image::VideoClip &videoClip, 
                     RawPoolAllocator& allocator) override;
                     
            void ProcessFrame(unsigned int frameNumber, Image::OCImage& image, Image::VideoClip &videoClip,
                              RawPoolAllocator& allocator) override;
                              
            bool CheckFormat(uint8_t* data, std::streamsize size);
        };
    }
}
#endif // MLVLOADER_H
