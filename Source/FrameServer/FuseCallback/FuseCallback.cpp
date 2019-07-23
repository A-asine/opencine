#include "FuseCallback.h"

FuseHandler::FuseHandler(unsigned int fileSize) : 
          _fileName("sample.avi"),
          _filePath("/sample.avi"),
          _fileSize(fileSize)
{   
}

int FuseHandler::getattr_callback(const char* path, struct stat *stbuf)
{
    std::memset(stbuf, 0, sizeof(struct stat));
    
    if(strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    
        return 0;
    }

    if(strcmp(path, filepath) == 0) 
    {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = _fileSize;
         
        return 0;
    }

    return -ENOENT;
}

int FuseHandler::open_callback(const char *path, fuse_file_info *fi)
{
    return 0;
}

int FuseHandler::read_callback(const char *path, char *buf, size_t size, off_t offset, fuse_file_info *fi)
{   
    


}

int Fusehandler::readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, 
                        fuse_file_info *fi)
{
    void(path);
	void(offset);
	void(fi);

	filler(buf, ".", nullptr, 0);
	filler(buf, "..", nullptr, 0);

	if(strcmp(path, "/") == 0)
	{
		filler(buf, _fileName, nullptr, 0);		
	}

	return 0;
}

