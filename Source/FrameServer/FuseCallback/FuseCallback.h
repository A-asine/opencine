#ifndef FUSECALLBACK_H
#define FUSECALLBACK_H
 
#include <fuse/fuse.h>
#include <string>

class FuseHandler
{
   std::string _fileName;
   std::string _filePath;
   unsigned int _fileSize;
   
public:
    
   FuseHandler();
   
   ~FuseHandler();
   
   int getattr_callback(const char* path, struct stat *stbuf);
   int read_callback(const char *path, char *buf, size_t size, off_t offset, fuse_file_info *fi);
   int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, 
                        fuse_file_info *fi);
   int open_callback(const char *path, fuse_file_info *fi); 
   
   void FrameServer();
};

#endif
