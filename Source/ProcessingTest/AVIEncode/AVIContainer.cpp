#include "AVIStruct.h"
#include "AVIContainer.h"

#define AVISize                 sizeof(List) - 2 * sizeof(uint32_t) + FindSize("hdrl") + FindSize("movi")
#define hdrlSize                sizeof(List) - 2 * sizeof(uint32_t) + FindSize("avih") + FindSize("strl")      
#define strlSize                sizeof(List) - 2 * sizeof(uint32_t) + FindSize("strh") + FindSize("strf")            
#define AVIStreamHeaderSize     sizeof(AVIStreamHeader)
#define BitmapInfoHeaderSize    sizeof(Chunk) - sizeof(uint32_t) + sizeof(BitmapInfoHeader)
#define AVIMainHeaderSize       sizeof(AVIMainHeader)
#define MoviSize                sizeof(List) - 2 * sizeof(uint32_t) + _frameCount * sizeof(Chunk)


AVIContainer::AVIContainer(unsigned int height, unsigned int width, uint16_t frameCount)
{
    _height     = height;
    _width      = width;
    _frameCount = frameCount;
    
    _aviFile    = new uint8_t[220];  //  220 byte : size of avi headers 
    
    FindSize("avi ");
    
    int offset = 0;
    CreateRIFFheader(offset);
    std::cout << offset << std::endl;
    WriteToFile(offset);
}

AVIContainer::~AVIContainer()
{  
    delete []_aviFile;
    
    avi.close();
}

uint32_t AVIContainer::CreateFourCC(std::string type)
{
    uint32_t fourCC = (uint32_t)type[3] << 24 |
                      (uint32_t)type[2] << 16 |
                      (uint32_t)type[1] << 8  |
                      (uint32_t)type[0];
      
    return fourCC;
}

unsigned long int AVIContainer::FindSize(std::string type)
{
    if(type == "avi ")
    {
      headerSize["avi "] = AVISize; 
      return (headerSize["avi "] + 2 * sizeof(uint32_t));
    }
   
    if(type == "hdrl")
    {
      headerSize["hdrl"] = hdrlSize;
      return ( headerSize["hdrl"] + 2 * sizeof(uint32_t));
    }
    
    if(type == "strl")
    {
      headerSize["strl"] = strlSize;                                 
      return (headerSize["strl"]  + 2 * sizeof(uint32_t));
    }
    
    if(type == "strh") 
    {
      headerSize["strh"] = AVIStreamHeaderSize;
      return headerSize["strh"]; 
    }
  
    if(type == "strf")
    {
      headerSize["strf"] = BitmapInfoHeaderSize;
      return headerSize["strf"] + sizeof(uint32_t);
    }
  
    if(type == "avih")
    {
      headerSize["avih"] = AVIMainHeaderSize;
      return headerSize["avih"];
    }
  
    if(type == "movi")
    {
      unsigned long int value = _frameCount * (_height * _width * 3);
      headerSize["movi"]       = MoviSize + value;
      return headerSize["movi"] + 2 * sizeof(uint32_t);
    }

    return 0;
}

void AVIContainer::WriteToFile(int &offset)
{  
    avi.open("sample.avi", std::ios::binary);
    avi.write(reinterpret_cast<char*>(_aviFile), offset);
}

void AVIContainer::CreateRIFFheader(int &offset)
{
    List list;
    
    list.FourCC = CreateFourCC("RIFF");
    list.size   = headerSize["avi "];
    list.type   = CreateFourCC("AVI ");
    
    std::memcpy(&_aviFile[offset], (const void*)&list , sizeof(List));
    offset += sizeof(List);
     
    CreateHDRLheader(offset);
    CreateMOVIheader(offset);   
}

void AVIContainer::CreateHDRLheader(int &offset)
{
    List list;
    
    list.FourCC = CreateFourCC("LIST");
    list.size   = headerSize["hdrl"];
    list.type   = CreateFourCC("hdrl");
    
    std::memcpy(&_aviFile[offset], (const void*)&list, sizeof(List));
    offset += sizeof(List);
    
    CreateAVIHheader(offset);
    CreateSTRLheader(offset);
}

void AVIContainer::CreateAVIHheader(int &offset)
{
    AVIMainHeader avih;
    
    avih.fcc                  = CreateFourCC("avih");
    avih.cb                   = headerSize["avih"] - 8;
    avih.dwMicroSecPerFrame   = 100;
    avih.dwMaxBytesPerSec     = 1000;
    avih.dwPaddingGranularity = 0;
    avih.dwFlags              = 1;
    avih.dwTotalFrames        = _frameCount;  
    avih.dwInitialFrames      = 0;
    avih.dwStreams            = 1;
    avih.dwSuggestedBufferSize= 100000;
    avih.dwWidth              = _width;
    avih.dwHeight             = _height;
    avih.dwReserved[0]        = 0;
    avih.dwReserved[1]        = 0;
    avih.dwReserved[2]        = 0;
    avih.dwReserved[3]        = 0;
    
    std::memcpy(&_aviFile[offset], (const void*)&avih, sizeof(AVIMainHeader));
    offset += sizeof(AVIMainHeader); 
}

void AVIContainer::CreateSTRLheader(int &offset)
{
    List list;
    
    list.FourCC = CreateFourCC("LIST");
    list.size   = headerSize["strl"];
    list.type   = CreateFourCC("strl");
    
    std::memcpy(&_aviFile[offset], (const void*)&list, sizeof(List));
    offset += sizeof(List);
    
    CreateSTRHheader(offset);
    CreateSTRFheader(offset);    
}  

void AVIContainer::CreateSTRHheader(int &offset)
{
    AVIStreamHeader strh;
    
    strh.FourCCType          = CreateFourCC("strh");
    strh.cb                  = headerSize["strh"] - 8;
    strh.fccType             = CreateFourCC("vids");
    strh.fccHandler          = 0;
    strh.Flags               = 0;
    strh.Priority            = 0;
    strh.Language            = 0;
    strh.InitialFrames       = 0;
    strh.Scale               = 1;
    strh.Rate                = 30;
    strh.Start               = 0;
    strh.Length              = strh.Rate / strh.Scale;
    strh.SuggestedBufferSize = 10000000;
    strh.Quality             = 0;
    strh.SampleSize          = 120;

    strh.rcFrame.left        = 0;
    strh.rcFrame.right       = _width;
    strh.rcFrame.top         = 0;
    strh.rcFrame.bottom      = _height;
   
    std::memcpy(&_aviFile[offset], (const void *)&strh, sizeof(AVIStreamHeader));
    offset += sizeof(AVIStreamHeader);
}

void AVIContainer::CreateSTRFheader(int &offset)
{
    Chunk strf;
          
    strf.Ckid   = CreateFourCC("strf");
    strf.CkSize = headerSize["strf"]; 
    
    std::memcpy(&_aviFile[offset], (const void*)&strf, sizeof(Chunk));
    offset += sizeof(Chunk);
    
    CreateBITheader(offset);
}

void AVIContainer::CreateBITheader(int &offset)
{
    BitmapInfoHeader vidf;
  
    vidf.Size          = sizeof(BitmapInfoHeader);
    vidf.Width         = _width;
    vidf.Height        = _height;
    vidf.Planes        = 1;
    vidf.BitCount      = 24;
    vidf.Compression   = 0;
    vidf.SizeImage     = 0;
    vidf.XPelsPerMeter = 2835;
    vidf.YPelsPerMeter = 2835;
    vidf.ClrUsed       = 0;
    vidf.ClrImportant  = 0;
 
    std::memcpy(&_aviFile[offset], (const void *)&vidf, sizeof(BitmapInfoHeader));
    offset += sizeof(BitmapInfoHeader);
}

void AVIContainer::CreateMOVIheader(int &offset)
{
    List movi;
    
    movi.FourCC = CreateFourCC("LIST");
    movi.size   = headerSize["movi"];
    movi.type   = CreateFourCC("movi");
    
    std::memcpy(&_aviFile[offset], (const void*)&movi, sizeof(List));
    offset += sizeof(List);
}

void AVIContainer::AddFrames(OC::Image::OCImage &image)
{
    unsigned int dataLength = _width * _height;
    unsigned char* interleavedArray = new unsigned char[dataLength * 3];

    for(int i = 0; i < dataLength; i++)
    {   
        interleavedArray[i * 3 + 0] = (static_cast<unsigned short*>(image.RedChannel())[i] - 2052 / 3) >> 6;
        interleavedArray[i * 3 + 1] = (static_cast<unsigned short*>(image.GreenChannel())[i] - 2052 / 3) >> 6;
        interleavedArray[i * 3 + 2] = (static_cast<unsigned short*>(image.BlueChannel())[i] - 2052 / 3) >> 6;
    }
    
    Chunk vidf;
    vidf.Ckid   = CreateFourCC("00db");
    vidf.CkSize = _height * _width * 3; 
    
    avi.write(reinterpret_cast<char*>(&vidf), sizeof(Chunk));    
    avi.write(reinterpret_cast<char*>(interleavedArray), dataLength * 3);
}

