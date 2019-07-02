// Copyright (c) 2017 apertus° Association & contributors
// Project: OpenCine / ProcesseingTest
// License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

#ifndef AVIContainer_H
#define AVIContainer_H

#include <fstream>
#include "SimpleTree.h"
#include <Image/OCImage.h>

// Reference: https://github.com/BAndiT1983/OC_FrameServer

class AVIContainer
{
private:
	Node* _rootNode{};	// Usually RIFF header
	unsigned int _fileSize;

	unsigned int CreateRIFFHeader(void* buffer, unsigned offset);
	unsigned int CreateHDRLHeader(void* buffer, unsigned offset);
	unsigned int CreateAVIMainHeader(void* buffer, unsigned offset);
	unsigned int CreateStreamHeader(void* buffer, unsigned offset);
	unsigned CreateMOVIHeader(void* buffer, unsigned offset);
	unsigned int CreateAVIStreamHeader(void* buffer, unsigned offset);
	unsigned CreateBitmapInfoHeader(void* buffer, unsigned offset);

	int WriteHeaderSequence();

	uint8_t* _dataBuffer;

	unsigned int _width;
	unsigned int _height;
	unsigned int _framesPerSecond;
	unsigned int _frameCount;

	unsigned int _frameSize;   // Pre-calculated sizes
	OC::Image::OCImage _image;
	Node* _temporaryNode;

public:
	AVIContainer(int width, int height, int framesPerSecond, int frameCount);
	~AVIContainer();

	void SetFourCC(uint32_t* fourCC, const char value[]);
	unsigned AddFrame(void* dataBuffer, unsigned offset);
	void AddMoviChild(unsigned int frameNumber, OC::Image::OCImage image);
	bool BuildAVI(uint8_t* dataBuffer, Node* node);
};

#endif //AVIContainer_H
