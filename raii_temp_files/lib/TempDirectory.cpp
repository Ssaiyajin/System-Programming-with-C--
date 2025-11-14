#include "lib/TempDirectory.hpp"
#include "lib/CommandLine.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

namespace raii {

TempDirectory::TempDirectory(const std::string& path, CommandLine& cmd)
    : directoryPath(path), cmdPtr(&cmd) {
    try {
        fs::path base = fs::temp_directory_path() / path;
        directoryPath = base.string();
        if (!fs::exists(base)) {
            fs::create_directories(base);
            std::cout << "Created temporary directory: " << directoryPath << std::endl;
        } else {
            std::cout << "Temporary directory already exists: " << directoryPath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "TempDirectory ctor error: " << e.what() << std::endl;
        throw;
    }
}

TempDirectory::~TempDirectory() {
    try {
        if (!directoryPath.empty() && fs::exists(directoryPath)) {
            fs::remove_all(directoryPath);
            std::cout << "Removing temporary directory: " << directoryPath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "TempDirectory dtor error: " << e.what() << std::endl;
    }
}

void TempDirectory::addFile(const std::string& filePath) {
    createdFiles.push_back(filePath);
}

void TempDirectory::addDir(const std::string& dirPath) {
    createdDirs.push_back(dirPath);
}

void TempDirectory::removeFiles() {
    // remove nested files first (files whose parent != base)
    std::string base = fs::path(directoryPath).string();
    std::vector<std::string> nested, top;
    for (auto const& f : createdFiles) {
        if (fs::path(f).parent_path() == fs::path(base)) top.push_back(f);
        else nested.push_back(f);
    }

    for (auto const& f : nested) {
        if (fs::exists(f)) {
            try { fs::remove(f); std::cout << "Removed file: " << f << std::endl; }
            catch (const std::exception& e) { std::cerr << "removeFiles error: " << e.what() << std::endl; }
        }
    }

    // remove tracked dirs (attempt in creation order)
    for (auto const& d : createdDirs) {
        if (fs::exists(d) && fs::is_directory(d)) {
            try {
                if (fs::is_empty(d)) {
                    fs::remove(d);
                    std::cout << "Removed directory: " << d << std::endl;
                } else {
                    // if not empty, don't remove here
                    std::cout << "Directory not empty, skipping remove: " << d << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "removeFiles dir error: " << e.what() << std::endl;
            }
        }
    }

    // remove top-level files last
    for (auto const& f : top) {
        if (fs::exists(f)) {
            try { fs::remove(f); std::cout << "Removed file: " << f << std::endl; }
            catch (const std::exception& e) { std::cerr << "removeFiles top error: " << e.what() << std::endl; }
        }
    }

    createdFiles.clear();
    createdDirs.clear();
}

bool TempDirectory::removeDirectory() noexcept {
    try {
        if (directoryPath.empty()) return false;
        fs::path p(directoryPath);
        if (fs::exists(p) && fs::is_directory(p) && fs::is_empty(p)) {
            return fs::remove(p);
        }
    } catch (...) {}
    return false;
}

void TempDirectory::removePath(const std::string& path) noexcept {
    try {
        fs::path p(path);
        if (fs::exists(p)) {
            if (fs::is_directory(p)) fs::remove_all(p);
            else fs::remove(p);
        }
        createdFiles.erase(std::remove(createdFiles.begin(), createdFiles.end(), path), createdFiles.end());
        createdDirs.erase(std::remove(createdDirs.begin(), createdDirs.end(), path), createdDirs.end());

        // also remove children from createdFiles / createdDirs
        std::string prefix = p.string() + "/";
        createdFiles.erase(std::remove_if(createdFiles.begin(), createdFiles.end(),
            [&](const std::string& s){ return s.rfind(prefix, 0) == 0; }), createdFiles.end());
        createdDirs.erase(std::remove_if(createdDirs.begin(), createdDirs.end(),
            [&](const std::string& s){ return s.rfind(prefix, 0) == 0; }), createdDirs.end());
    } catch (const std::exception& e) {
        std::cerr << "removePath error: " << e.what() << std::endl;
    }
}

} // namespace raii
//---------------------------------------------------------------------------


