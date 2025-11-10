#ifndef H_lib_OSApi
#define H_lib_OSApi
//---------------------------------------------------------------------------
namespace osapi {
//---------------------------------------------------------------------------
/// Create a directory at a given path.
void createDir(const char* path);
//---------------------------------------------------------------------------
/// Create a subdirectory with the given name within the directory given by the
/// file descriptor.
void createSubdir(int parentDirFd, const char* name);
//---------------------------------------------------------------------------
/// Remove an empty directory at the given path.
void removeDir(const char* path);
//---------------------------------------------------------------------------
/// Remove a subdirectory with the given name from a directory given by the
/// file descriptor.
void removeSubdir(int parentDirFd, const char* name);
//---------------------------------------------------------------------------
/// Remove a file with the given name from a directory given by the file
/// descriptor.
void removeFile(int parentDirFd, const char* name);
//---------------------------------------------------------------------------
/// Open a directory at a given path. Returns the file descriptor of the
/// directory.
int openDir(const char* path);
//---------------------------------------------------------------------------
/// Open a subdirectory of a directory given by the file descriptor with the
/// given name. Returns the file descriptor of the subdirectory.
int openSubdir(int parentDirFd, const char* name);
//---------------------------------------------------------------------------
/// Create and open a file with the given name within the directory given by
/// the file descriptor. Returns the file descriptor of the file.
int openFile(int dirFd, const char* name);
//---------------------------------------------------------------------------
/// Close a file descriptor of a file or a directory
void closeFd(int fd);
//---------------------------------------------------------------------------
} // namespace osapi
//---------------------------------------------------------------------------
#endif
