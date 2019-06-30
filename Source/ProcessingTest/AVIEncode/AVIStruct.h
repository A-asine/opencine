#ifndef AVISTRUCT_H
#define AVISTRUCT_H

#include <stdint.h>

struct List
{
   uint32_t FourCC;
   uint32_t size;
   uint32_t type;
};

struct Chunk
{
   uint32_t Ckid;
   uint32_t CkSize;
};

struct AVIMainHeader
{
   uint32_t fcc;
   uint32_t cb;
   uint32_t dwMicroSecPerFrame;
   uint32_t dwMaxBytesPerSec;
   uint32_t dwPaddingGranularity;
   uint32_t dwFlags;
   uint32_t dwTotalFrames;
   uint32_t dwInitialFrames;
   uint32_t dwStreams;
   uint32_t dwSuggestedBufferSize;
   uint32_t dwWidth;
   uint32_t dwHeight;
   uint32_t dwReserved[4];
};

struct AVIStreamHeader
{
   uint32_t FourCCType;
   uint32_t cb;
   uint32_t fccType; 
   uint32_t fccHandler;
   uint32_t Flags;
   uint16_t Priority;
   uint16_t Language;
   uint32_t InitialFrames;
   uint32_t Scale;
   uint32_t Rate;
   uint32_t Start;
   uint32_t Length;
   uint32_t SuggestedBufferSize;
   uint32_t Quality;
   uint32_t SampleSize;

   struct
   {
      uint8_t left;
      uint8_t top;
      uint8_t right;
      uint8_t bottom;
   } rcFrame;
};

struct BitmapInfoHeader
{     
   uint32_t Size;
   uint32_t Width;
   uint32_t Height;
   uint16_t Planes;
   uint16_t BitCount;
   uint32_t Compression;
   uint32_t SizeImage;
   uint32_t XPelsPerMeter;
   uint32_t YPelsPerMeter;
   uint32_t ClrUsed;
   uint32_t ClrImportant;
};

#endif  // AVISTRUCT_H
