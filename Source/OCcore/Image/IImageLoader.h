// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef IIMAGELOADER_H
#define IIMAGELOADER_H

#include "Memory/StaticAllocator.h"
#include "OCImage.h"

namespace OC
{
    namespace DataProvider
    {
        // TODO: Add base methods
        class IImageLoader
        {
        public:
            virtual ~IImageLoader()
            {
            }

            virtual void Load(uint8_t* data, unsigned int size, OC::Image::OCImage& image, RawPoolAllocator& allocator) = 0;
            virtual void ProcessFrame(unsigned int frameNumber, OC::Image::OCImage& image, RawPoolAllocator& allocator) = 0;
            virtual bool CheckFormat(uint8_t* data, std::streamsize size) = 0;
        };
    }
}
#endif // IIMAGELOADER_H
