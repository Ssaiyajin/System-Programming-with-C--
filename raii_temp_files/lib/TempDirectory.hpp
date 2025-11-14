#ifndef H_lib_TempDirectory
#define H_lib_TempDirectory

#include <string>
#include <vector>
#include <filesystem>

namespace raii {

class CommandLine; // forward

class TempDirectory {
public:
    TempDirectory(const std::string& path, CommandLine& cmd);
    ~TempDirectory();

    std::string getPath() const noexcept { return directoryPath; }

    void addFile(const std::string& filePath);
    void addDir(const std::string& dirPath);

    // remove all tracked files (nested first), try to remove tracked dirs,
    // but leave base dir removal to removeDirectory()
    void removeFiles();

    // remove base directory if empty
    bool removeDirectory() noexcept;

    // remove a specific tracked path (file or directory)
    void removePath(const std::string& path) noexcept;

private:
    std::string directoryPath;
    CommandLine* cmdPtr = nullptr;
    std::vector<std::string> createdFiles;
    std::vector<std::string> createdDirs;
};

} // namespace raii

#endif

