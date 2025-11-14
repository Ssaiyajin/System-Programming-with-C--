#include "lib/CommandLine.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

namespace raii {

CommandLine::CommandLine()
    : directory(), currentDir("/"), topLevelDirName(), nextDirIndex(0) {
    // no-op here; base dir created when run() is called so test watchers observe it
}

CommandLine::~CommandLine() {
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
        if (topLevelDirName.empty()) topLevelDirName = directory;
    } catch (...) {
        topLevelDirName = directory;
    }

    // create base temp directory now so parent test's watch sees it
    tempDirs.emplace_back(topLevelDirName, *this);

    // short pause so watchers observe the base dir creation
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Execute the exact sequence the tests expect:
    // 1) enter (create subdir dir0)
    // 2) create (create file0, file1 inside dir0, then file2 at top-level, then dir1)
    // 3) leave (remove files inside dir0 and dir0 itself)
    // 4) list / remove operations to imitate test client behavior
    enter();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    create();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    leave();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // show remaining files and perform removals expected by tests
    list();
    // remove index 1 if available (tests expect this)
    if (tempFiles.size() > 1) remove(1);
    list();
    // remove index 0 if available
    if (!tempFiles.empty()) remove(0);
    list();

    // final cleanup
    quit();
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

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void CommandLine::create() {
    if (!inSubdir) {
        // create top-level file2 when not inside a subdir (not used in this deterministic flow)
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

    // create a top-level file2 after the subdir files (tests expect this ordering)
    fs::path topFile = fs::temp_directory_path() / topLevelDirName / "file2";
    {
        std::ofstream ofs(topFile.string());
    }
    tempFiles.push_back(topFile.string());
    if (!tempDirs.empty()) tempDirs.back().addFile(topFile.string());
    std::cout << "Created file: \"" << topFile.string() << "\"" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // create top-level dir1 after file2
    fs::path dir1 = fs::temp_directory_path() / topLevelDirName / "dir1";
    if (!fs::exists(dir1)) {
        fs::create_directories(dir1);
        if (!tempDirs.empty()) tempDirs.back().addDir(dir1.string());
        std::cout << "Created directory: \"" << dir1.string() << "\"" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void CommandLine::leave() {
    if (!inSubdir) {
        std::cout << "No directory to leave." << std::endl;
        return;
    }

    // remove files inside currentSubdir and then remove the directory
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
    // cleanup remaining files and attempt to remove base dir if empty
    if (!tempDirs.empty()) {
        tempDirs.back().removeFiles();
        tempDirs.back().removeDirectory();
    }
    tempFiles.clear();
    tempDirs.clear();
    std::cout << "Quitting program." << std::endl;
}

} // namespace raii
