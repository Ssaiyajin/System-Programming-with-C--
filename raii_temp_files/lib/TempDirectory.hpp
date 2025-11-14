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
        void removeFiles(); // Declaration3
        bool isEmpty() const {
            return std::filesystem::is_empty(directoryPath);
        }

    // removeDirectory removes the temp directory if it is empty and returns true on success
    bool removeDirectory();

    private:
        std::string directoryPath;
        raii::CommandLine& cmdPtr;
        std::vector<std::string> createdFiles; // Move the declaration inside the class
        std::vector<std::string> createdDirs;  // track created directories so they can be removed
    };

//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------
#endif

