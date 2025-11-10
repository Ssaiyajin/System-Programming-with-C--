#include "lib/OSApi.hpp"
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace osapi {
//---------------------------------------------------------------------------
void createDir(const char* path)
// Create a directory at a given path.
{
    if (mkdir(path, S_IRWXU) < 0)
        perror("createDir failed");
}
//---------------------------------------------------------------------------
void createSubdir(int parentDirFd, const char* name)
// Create a subdirectory with the given name within the directory given by the
// file descriptor.
{
    if (mkdirat(parentDirFd, name, S_IRWXU) < 0)
        perror("createSubdir failed");
}
//---------------------------------------------------------------------------
void removeDir(const char* path)
// Remove an empty directory at the given path.
{
    if (rmdir(path) < 0)
        perror("removeDir failed");
}
//---------------------------------------------------------------------------
void removeSubdir(int parentDirFd, const char* name)
// Remove a subdirectory with the given name from a directory given by the
// file descriptor.
{
    if (unlinkat(parentDirFd, name, AT_REMOVEDIR) < 0)
        perror("removeSubdir failed");
}
//---------------------------------------------------------------------------
void removeFile(int parentDirFd, const char* name)
// Remove a file with the given name from a directory given by the file
// descriptor.
{
    if (unlinkat(parentDirFd, name, 0) < 0)
        perror("removeFile failed");
}
//---------------------------------------------------------------------------
int openDir(const char* path)
// Open a directory at a given path. Returns the file descriptor of the
// directory.
{
    int fd = open(path, O_DIRECTORY);
    if (fd < 0)
        perror("openDir failed");
    return fd;
}
//---------------------------------------------------------------------------
int openSubdir(int parentDirFd, const char* name)
// Open a subdirectory of a directory given by the file descriptor with the
// given name. Returns the file descriptor of the subdirectory.
{
    int fd = openat(parentDirFd, name, O_DIRECTORY);
    if (fd < 0)
        perror("openSubdir failed");
    return fd;
}
//---------------------------------------------------------------------------
int openFile(int dirFd, const char* name)
// Create and open a file with the given name within the directory given by
// the file descriptor. Returns the file descriptor of the file.
{
    int fd = openat(dirFd, name, O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
        perror("openFile failed");
    return fd;
}
//---------------------------------------------------------------------------
void closeFd(int fd)
// Close a file descriptor of a file or a directory
{
    if (close(fd) < 0)
        perror("closeFd failed");
}
//---------------------------------------------------------------------------
} // namespace osapi
//---------------------------------------------------------------------------
