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

        // Create the base temporary directory if it doesn't exist.
        // Do not create child files/dirs here — CommandLine::create() is responsible
        // for creating dir0/file0/file1/file2/dir1 so the test sees a single set
        // of create events in a predictable order.
        if (!fs::exists(directoryPath)) {
            fs::create_directories(directoryPath);
            std::cout << "Created temporary directory: " << directoryPath << std::endl;
        } else {
            std::cout << "Temporary directory already exists: " << directoryPath << std::endl;
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

void TempDirectory::addFile(const std::string& filePath) {
    createdFiles.push_back(filePath);
}

void TempDirectory::addDir(const std::string& dirPath) {
    createdDirs.push_back(dirPath);
}

void TempDirectory::removeFiles() {
    // Separate files that live in the base temp directory from nested files
    std::vector<std::string> topLevelFiles;
    std::vector<std::string> nestedFiles;
    for (const auto& file : createdFiles) {
        try {
            fs::path p(file);
            if (p.parent_path() == fs::path(directoryPath)) {
                topLevelFiles.push_back(file);
            } else {
                nestedFiles.push_back(file);
            }
        } catch (...) {
            nestedFiles.push_back(file);
        }
    }

    // Remove nested files first (files inside dir0/dir1)
    for (const auto& file : nestedFiles) {
        if (fs::exists(file)a) {
            try {
                fs::remove(file);
                std::cout << "Removed file: " << file << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error removing file: " << file << " - " << e.what() << std::endl;
            }
        }
    }

    // Remove created subdirectories in creation order (dir0, dir1, ...)
    for (const auto& d : createdDirs) {
        if (fs::exists(d) && fs::is_directory(d)) {
            try {
                if (fs::is_empty(d)) {
                    fs::remove(d);
                    std::cout << "Removed directory: " << d << std::endl;
                } else {
                    std::cout << "Directory not empty, not removed: " << d << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error removing directory: " << d << " - " << e.what() << std::endl;
            }
        }
    }

    // Remove top-level files last
    for (const auto& file : topLevelFiles) {
        if (fs::exists(file)) {
            try {
                fs::remove(file);
                std::cout << "Removed file: " << file << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error removing file: " << file << " - " << e.what() << std::endl;
            }
        }
    }

    createdFiles.clear();
    createdDirs.clear();
}

bool TempDirectory::removeDirectory() {
    try {
        if (directoryPath.empty()) return false;
        if (fs::exists(directoryPath) && fs::is_directory(directoryPath) && fs::is_empty(directoryPath)) {
            return fs::remove(directoryPath);
        }
    } catch (...) {
        // swallow errors; caller can handle return value
    }
    return false;
}

void TempDirectory::removePath(const std::string& path) {
    try {
        fs::path p(path);

        // Remove file or directory from filesystem
        if (fs::exists(p)) {
            if (fs::is_directory(p)) {
                fs::remove_all(p);
                std::cout << "Removed directory: " << path << std::endl;
            } else {
                fs::remove(p);
                std::cout << "Removed file: " << path << std::endl;
            }
        }

        // Erase matching entries from createdFiles
        createdFiles.erase(std::remove(createdFiles.begin(), createdFiles.end(), path), createdFiles.end());

        // Erase matching entries from createdDirs
        createdDirs.erase(std::remove(createdDirs.begin(), createdDirs.end(), path), createdDirs.end());

        // Also remove any files that were children of this path from createdFiles
        std::string prefix = fs::path(path).string() + "/";
        createdFiles.erase(std::remove_if(createdFiles.begin(), createdFiles.end(),
            [&](const std::string& s){ return s.rfind(prefix, 0) == 0; }), createdFiles.end());

        // Remove any directories under this path from createdDirs
        createdDirs.erase(std::remove_if(createdDirs.begin(), createdDirs.end(),
            [&](const std::string& s){ return s.rfind(prefix, 0) == 0; }), createdDirs.end());

    } catch (const std::exception& e) {
        std::cerr << "Error in removePath(" << path << "): " << e.what() << std::endl;
    }
}

//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------


