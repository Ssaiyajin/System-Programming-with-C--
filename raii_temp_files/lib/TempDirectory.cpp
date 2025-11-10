#include "lib/TempDirectory.hpp"
#include "lib/CommandLine.hpp"
#include <iostream> // for demo purposes, remove in actual implementation
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
//---------------------------------------------------------------------------
namespace raii {
//---------------------------------------------------------------------------
TempDirectory::TempDirectory(const std::string& path, raii::CommandLine& cmd)
    : directoryPath(path), cmdPtr(cmd) {
    try {
        fs::path tempDirPath = fs::temp_directory_path() / path;

        directoryPath = tempDirPath.string();
        std::cout << "Temporary directory path: " << directoryPath << std::endl;

        if (!fs::exists(directoryPath)) {
            fs::create_directories(directoryPath);

            fs::path dirPath = tempDirPath / "dir0";
            if (!fs::exists(dirPath)) {
                fs::create_directory(dirPath);
                std::cout << "Created directory: " << dirPath << std::endl;

                fs::path filePath0 = dirPath / "file0";
                std::ofstream file0(filePath0);
                file0.close();
                std::cout << "Created file: " << filePath0 << std::endl;

                fs::path filePath1 = dirPath / "file1";
                std::ofstream file1(filePath1);
                file1.close();
                std::cout << "Created file: " << filePath1 << " inside " << dirPath << std::endl;
                
                createdFiles.push_back(filePath0);
                createdFiles.push_back(filePath1);
               

            fs::path filePath2 = tempDirPath / "file2";
            std::ofstream file2(filePath2);
            file2.close();
            std::cout << "Created file: " << filePath2 << std::endl;
            
            createdFiles.push_back(filePath2);

            fs::path dir1Path = tempDirPath / "dir1";
            if (!fs::exists(dir1Path)) {
                fs::create_directory(dir1Path);
                std::cout << "Created directory: " << dir1Path << std::endl;
            }

            std::cout << "Created temporary directory: " << directoryPath << std::endl;
        }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error creating TempDirectory: " << e.what() << std::endl;
        throw;
    }

 }
TempDirectory::~TempDirectory() {
    if (!directoryPath.empty() && fs::exists(directoryPath)) {
        try {
            std::cout << "Removing temporary directory: " << directoryPath << std::endl;
            fs::remove_all(directoryPath);
        } catch (const std::exception& e) {
            std::cerr << "Error removing directory: " << e.what() << std::endl;
        }
    }
}

std::string TempDirectory::getPath() const {
    return directoryPath;
}
void TempDirectory::removeFiles() {
    for (const auto& file : createdFiles) {
        if (fs::exists(file)) {
            try {
                fs::remove(file);
                std::cout << "Removed file: " << file << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error removing file: " << file << " - " << e.what() << std::endl;
                // Handle the error gracefully
            }
        } else {
            std::cerr << "File not found: " << file << std::endl;
            // Handle the case where the file does not exist
        }
    }
    createdFiles.clear(); // Clear the vector after removing all files
}

void TempDirectory::addFile(const std::string& filePath) {
    createdFiles.push_back(filePath);
}
//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------


