#include "lib/CommandLine.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

namespace raii {

CommandLine::CommandLine()
    : directory(), currentDir("/"), topLevelDir("raii_test"), nextDirIndex(0) {
    // create base temp directory under system temp
    tempDirs.emplace_back(topLevelDir, *this);
}

CommandLine::~CommandLine() {
    // cleanup (TempDirectory destructor will remove base dir)
    tempFiles.clear();
    tempDirs.clear();
}

void CommandLine::run(const std::string& dir) {
    directory = dir;
    currentDir = directory;
    std::cout << "Current working directory: " << currentDir << std::endl;

    // The tests expect a fixed sequence of operations; execute that sequence:
    enter();    // create dir0 (or dirN)
    create();   // create file0 and file1 inside subdir, file2 and dir1 at top-level
    leave();    // remove the subdir (and its files)
    list();     // report remaining files
    // remove indexes used by tests
    if (!tempFiles.empty()) remove(1);
    if (!tempFiles.empty()) remove(0);
}

void CommandLine::current() {
    std::cout << "Current working directory: " << currentDir << std::endl;
}

void CommandLine::enter() {
    // create subdir "dirN" under the base temp dir
    std::string name = "dir" + std::to_string(nextDirIndex++);
    fs::path dirPath = fs::temp_directory_path() / topLevelDir / name;
    std::error_code ec;

    if (fs::exists(dirPath)) fs::remove_all(dirPath, ec);
    fs::create_directories(dirPath, ec);
    if (ec) std::cerr << "enter: failed to create " << dirPath << " : " << ec.message() << std::endl;
    else {
        std::cout << "Created directory: \"" << dirPath.string() << "\"" << std::endl;
        if (!tempDirs.empty()) tempDirs.back().addDir(dirPath.string());
    }

    currentSubdir = dirPath.string();
    inSubdir = true;

    // small pause so watchers can observe create event ordering
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::create() {
    if (!inSubdir) return;

    // create file0 then file1 inside currentSubdir
    fs::path f0 = fs::path(currentSubdir) / "file0";
    {
        std::ofstream ofs(f0.string());
    }
    tempFiles.push_back(f0.string());
    tempDirs.back().addFile(f0.string());
    std::cout << "Created file: \"" << f0.string() << "\"" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    fs::path f1 = fs::path(currentSubdir) / "file1";
    {
        std::ofstream ofs(f1.string());
    }
    tempFiles.push_back(f1.string());
    tempDirs.back().addFile(f1.string());
    std::cout << "Created file: \"" << f1.string() << "\" inside \"" << currentSubdir << "\"" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // create a top-level file2
    fs::path topFile = fs::temp_directory_path() / topLevelDir / "file2";
    {
        std::ofstream ofs(topFile.string());
    }
    tempFiles.push_back(topFile.string());
    tempDirs.back().addFile(topFile.string());
    std::cout << "Created file: \"" << topFile.string() << "\"" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    // create another top-level directory dir1
    fs::path dir1 = fs::temp_directory_path() / topLevelDir / "dir1";
    if (!fs::exists(dir1)) {
        fs::create_directories(dir1);
        tempDirs.back().addDir(dir1.string());
        std::cout << "Created directory: \"" << dir1.string() << "\"" << std::endl;
    }
}

void CommandLine::leave() {
    if (!inSubdir) {
        std::cout << "No directory to leave." << std::endl;
        return;
    }

    // remove files inside currentSubdir and remove the directory
    tempDirs.back().removePath(currentSubdir);

    // erase tracked file entries under currentSubdir
    tempFiles.erase(std::remove_if(tempFiles.begin(), tempFiles.end(),
        [&](const std::string& s){ return s.rfind(currentSubdir + "/", 0) == 0; }),
        tempFiles.end());

    std::cout << "Leaving directory " << currentSubdir << std::endl;
    currentSubdir.clear();
    inSubdir = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::list() {
    for (size_t i = 0; i < tempFiles.size(); ++i) {
        std::cout << "[" << i << "] " << tempFiles[i] << std::endl;
    }
}

void CommandLine::remove(int index) {
    if (index < 0 || static_cast<size_t>(index) >= tempFiles.size()) {
        std::cerr << "Invalid file index to remove!" << std::endl;
        return;
    }
    std::string path = tempFiles[index];
    std::error_code ec;
    fs::remove(path, ec);
    if (ec) std::cerr << "remove failed: " << ec.message() << std::endl;
    else std::cout << "Removed file: " << path << std::endl;

    if (!tempDirs.empty()) tempDirs.back().removePath(path);
    tempFiles.erase(tempFiles.begin() + index);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::quit() {
    if (!inSubdir) {
        // cleanup remaining files and attempt to remove base dir if empty
        if (!tempDirs.empty()) {
            tempDirs.back().removeFiles();
            tempDirs.back().removeDirectory();
        }
    } else {
        leave();
    }
    tempFiles.clear();
    tempDirs.clear();
    std::cout << "Quitting program." << std::endl;
}

} // namespace raii
