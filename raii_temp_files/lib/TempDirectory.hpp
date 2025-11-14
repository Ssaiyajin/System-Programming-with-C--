#ifndef H_lib_TempDirectory
#define H_lib_TempDirectory
#include <string>
#include "lib/CommandLine.hpp" // Include CommandLine header
#include <iostream> // Added for debugging purposes
#include <filesystem>
#include <vector>
//---------------------------------------------------------------------------
namespace raii {
//---------------------------------------------------------------------------

    class CommandLine; // Forward declaration

    class TempDirectory {
    public:
        TempDirectory(const std::string& path, raii::CommandLine& cmd);
        ~TempDirectory();

        std::string getPath() const;
        void addFile(const std::string& filePath);
        void addDir(const std::string& dirPath);
        void removePath(const std::string& path);
        void removeFiles(); // remove created files and attempt to remove created subdirs
        bool isEmpty() const {
            return std::filesystem::is_empty(directoryPath);
        }

    // remove base directory if it's empty
    bool removeDirectory();

    private:
        std::string directoryPath;
        raii::CommandLine& cmdPtr;
        std::vector<std::string> createdFiles; // track created files
        std::vector<std::string> createdDirs;  // track created subdirectories (dir0, dir1, ...)
    };

//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------
#endif

