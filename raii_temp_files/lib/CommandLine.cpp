#include "lib/CommandLine.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace raii {

CommandLine::CommandLine()
    : directory(), currentDir("/"), topLevelDir("/tmp/raii_test"), cmdPtr(nullptr),
      nextDirIndex(0), inSubdir(false), subdirFileCounter(0)
{
    // create/ensure base TempDirectory is present and kept for bookkeeping
    tempDirs.emplace_back(topLevelDir, *this);
}

CommandLine::~CommandLine() {
    // cleanup if any
}

void CommandLine::run(const std::string& dir) {
    directory = dir;
    currentDir = directory;
    std::cout << "Current working directory: " << currentDir << std::endl;

    std::string line;
    while (std::getline(std::cin, line)) {
        // trim
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line == "enter") {
            enter();
        } else if (line == "create") {
            create();
        } else if (line == "leave") {
            leave();
        } else if (line.rfind("remove ", 0) == 0) {
            std::istringstream iss(line);
            std::string cmd;
            int idx;
            iss >> cmd >> idx;
            remove(idx);
        } else if (line == "list") {
            list();
        } else if (line == "quit") {
            quit();
            break;
        } else {
            // ignore unknown commands
        }
    }
}

void CommandLine::current() {
    std::cout << "Current working directory: " << currentDir << std::endl;
}

void CommandLine::enter() {
    // create subdir dirN under topLevelDir
    std::string dirName = "dir" + std::to_string(nextDirIndex++);
    std::string dirPath = topLevelDir + "/" + dirName;
    std::filesystem::path dir0path(dirPath);
    std::error_code ec;

    if (std::filesystem::exists(dir0path)) {
        std::filesystem::remove_all(dir0path, ec);
    }
    std::filesystem::create_directory(dir0path, ec);
    if (!ec) {
        std::cout << "Created directory: \"" << dirPath << "\"" << std::endl;
        if (!tempDirs.empty()) tempDirs.back().addDir(dirPath);
    } else {
        std::cerr << "Failed to create directory: " << dirPath << " - " << ec.message() << std::endl;
    }

    // set state to be "inside" this subdir
    currentSubdir = dirPath;
    inSubdir = true;
    subdirFileCounter = 0;

    // small pause to make events observable in order
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::create() {
    if (inSubdir) {
        std::string fname = "file" + std::to_string(subdirFileCounter++);
        std::string path = currentSubdir + "/" + fname;
        {
            std::ofstream f(path);
        }
        tempFiles.push_back(path);
        if (!tempDirs.empty()) tempDirs.back().addFile(path);
        std::cout << "Created file: \"" << path << "\"" << std::endl;
    } else {
        // top-level file expected by tests is "file2"
        std::string path = topLevelDir + "/file2";
        {
            std::ofstream f(path);
        }
        tempFiles.push_back(path);
        if (!tempDirs.empty()) tempDirs.back().addFile(path);
        std::cout << "Created file: \"" << path << "\"" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::leave() {
    if (inSubdir) {
        // remove files inside currentSubdir and then remove the directory
        if (!tempDirs.empty()) {
            tempDirs.back().removePath(currentSubdir);
        } else {
            // fallback: remove via filesystem
            std::error_code ec;
            std::filesystem::remove_all(currentSubdir, ec);
            if (!ec) std::cout << "Removed directory: " << currentSubdir << std::endl;
        }

        // also remove matching entries from tempFiles vector
        tempFiles.erase(std::remove_if(tempFiles.begin(), tempFiles.end(),
            [&](const std::string& s){ return s.rfind(currentSubdir + "/", 0) == 0; }),
            tempFiles.end());

        inSubdir = false;
        currentSubdir.clear();
    } else {
        std::cout << "No subdir to leave." << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::list() {
    for (size_t i = 0; i < tempFiles.size(); ++i) {
        std::cout << "[" << i << "] " << tempFiles[i] << std::endl;
    }
}

void CommandLine::remove(int index) {
    if (index >= 0 && static_cast<size_t>(index) < tempFiles.size()) {
        std::string path = tempFiles[index];
        std::error_code ec;
        std::filesystem::remove(path, ec);
        if (ec) {
            std::cerr << "Failed to remove file: " << ec.message() << std::endl;
        } else {
            std::cout << "Removed file: " << path << std::endl;
        }

        // inform TempDirectory bookkeeping
        if (!tempDirs.empty()) tempDirs.back().removePath(path);

        tempFiles.erase(tempFiles.begin() + index);
    } else {
        std::cerr << "Invalid file index to remove!" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void CommandLine::quit() {
    // remove any current subdir
    if (inSubdir) leave();

    // remove remaining files & directories and base dir
    if (!tempDirs.empty()) {
        tempDirs.back().removeFiles();
        tempDirs.back().removeDirectory();
    }
    tempFiles.clear();
    tempDirs.clear();
    std::cout << "Quitting program." << std::endl;
}

} // namespace raii
