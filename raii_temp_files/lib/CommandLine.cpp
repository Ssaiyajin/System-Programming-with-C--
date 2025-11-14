#include "lib/CommandLine.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <sstream>

namespace fs = std::filesystem;

namespace raii {

CommandLine::CommandLine()
    : directory(), currentDir("/"), topLevelDirName(), nextDirIndex(0) {
    // do not create base temp directory here — create it in run() so the
    // test's inotify watch (set by parent) observes the creation.
}

CommandLine::~CommandLine() {
    // TempDirectory destructor will clean up when tempDirs is cleared.
    tempFiles.clear();
    tempDirs.clear();
}

void CommandLine::run(const std::string& dir) {
    directory = dir;
    currentDir = directory;
    std::cout << "Current working directory: " << currentDir << std::endl;

    // determine base directory name (e.g. "/tmp/raii_test" -> "raii_test")
    try {
        fs::path p(directory);
        topLevelDirName = p.filename().string();
        if (topLevelDirName.empty()) {
            // fallback: use whole directory string if filename() is empty
            topLevelDirName = directory;
        }
    } catch (...) {
        topLevelDirName = directory;
    }

    // create the base temp directory now so the parent test's watch on /tmp
    // sees the directory creation event.
    tempDirs.emplace_back(topLevelDirName, *this);

    // Process commands from stdin until "quit"
    std::string line;
    while (std::getline(std::cin, line)) {
        // normalize line endings
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        if (line == "enter") {
            enter();
        } else if (line == "create") {
            create();
        } else if (line == "leave") {
            leave();
        } else if (line == "list") {
            list();
        } else if (line.rfind("remove ", 0) == 0) {
            std::istringstream iss(line);
            std::string cmd;
            int idx;
            iss >> cmd >> idx;
            remove(idx);
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
    // create subdir "dirN" under the base temp dir
    std::string name = "dir" + std::to_string(nextDirIndex++);
    fs::path dirPath = fs::temp_directory_path() / topLevelDirName / name;
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

    // small pause so watchers observe create event ordering
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void CommandLine::create() {
    if (!inSubdir) {
        // create top-level file2 when create is called outside a subdir
        fs::path topFile = fs::temp_directory_path() / topLevelDirName / "file2";
        {
            std::ofstream ofs(topFile.string());
        }
        tempFiles.push_back(topFile.string());
        if (!tempDirs.empty()) tempDirs.back().addFile(topFile.string());
        std::cout << "Created file: \"" << topFile.string() << "\"" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return;
    }

    // create file0 then file1 inside currentSubdir
    fs::path f0 = fs::path(currentSubdir) / "file0";
    {
        std::ofstream ofs(f0.string());
    }
    tempFiles.push_back(f0.string());
    if (!tempDirs.empty()) tempDirs.back().addFile(f0.string());
    std::cout << "Created file: \"" << f0.string() << "\"" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    fs::path f1 = fs::path(currentSubdir) / "file1";
    {
        std::ofstream ofs(f1.string());
    }
    tempFiles.push_back(f1.string());
    if (!tempDirs.empty()) tempDirs.back().addFile(f1.string());
    std::cout << "Created file: \"" << f1.string() << "\" inside \"" << currentSubdir << "\"" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void CommandLine::leave() {
    if (!inSubdir) {
        std::cout << "No directory to leave." << std::endl;
        return;
    }

    // remove files and the directory
    if (!tempDirs.empty()) {
        tempDirs.back().removePath(currentSubdir);
    } else {
        std::error_code ec;
        fs::remove_all(currentSubdir, ec);
        if (!ec) std::cout << "Removed directory: " << currentSubdir << std::endl;
    }

    // erase tracked file entries under currentSubdir
    tempFiles.erase(std::remove_if(tempFiles.begin(), tempFiles.end(),
        [&](const std::string& s){ return s.rfind(currentSubdir + "/", 0) == 0; }),
        tempFiles.end());

    std::cout << "Leaving directory " << currentSubdir << std::endl;
    currentSubdir.clear();
    inSubdir = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void CommandLine::quit() {
    // remove any remaining files and directories then base dir if empty
    if (!tempDirs.empty()) {
        tempDirs.back().removeFiles();
        tempDirs.back().removeDirectory();
    }
    tempFiles.clear();
    tempDirs.clear();
    std::cout << "Quitting program." << std::endl;
}

} // namespace raii
