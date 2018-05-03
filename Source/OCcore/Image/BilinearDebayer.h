// Copyright (c) 2018 apertus° Association & contributors
// Project: OpenCine / OCcore
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef BILINEARDEBAYER_H
#define BILINEARDEBAYER_H

#include "IFrameProcessor.h"
#include "OCImage.h"

using namespace OC::DataProvider;

class BilinearDebayer
{
private:
    // Color Channels.
    uint16_t* _redChannel;
    uint16_t* _greenChannel;
    uint16_t* _blueChannel;

    // Image Dimensions.
    uint32_t _width;
    uint32_t _height;
    uint32_t _size;

    // TODO
    // _patternOffsets

public:
    BilinearDebayer(OCImage& image);

    ~BilinearDebayer();

    // Processors for each color channel.
    void ProcessRed();
    void ProcessBlue();
    void ProcessGreen();

    // Main Processor.
    void Process();

};

#endif //BilinearDebayer_H
