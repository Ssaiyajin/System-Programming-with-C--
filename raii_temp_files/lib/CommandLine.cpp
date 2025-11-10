#include "lib/CommandLine.hpp"
#include <iostream> // for demo purposes, remove in actual implementation
#include <sstream>
#include <fstream> // Include for std::ofstream
//---------------------------------------------------------------------------
namespace raii {
//---------------------------------------------------------------------------
CommandLine::CommandLine() : currentDir("/"), topLevelDir("/tmp/raii_test"), cmdPtr(nullptr) {
        tempDirs.emplace_back("/tmp/raii_test", *this);
    }
CommandLine::~CommandLine() {
    // Cleanup logic, if needed
}
void CommandLine::run(const std::string& /* directory */) {
    // Initialize the current working directory
    currentDir = directory;
    std::cout << "Current working directory: " << currentDir << std::endl;
    // Perform the operations
    enter();    // Enter a directory
    create();   // Create files within the directory
    leave();    // Leave the directory before file2 is created
    list();     // List all files
    remove(1);  // Remove the first file
    list();     // List files after removal
    remove(0);  // Remove the second file
    list();     // List files after second removal
}
void CommandLine::current() {
    std::cout << "Current working directory: " << currentDir << std::endl;
}
void CommandLine::enter() {
    // Check if temp directory already exists, otherwise create it
    if (tempDirs.empty() || tempDirs.back().getPath() != topLevelDir) {
        raii::TempDirectory tempDir(topLevelDir, *this);
        tempDirs.push_back(tempDir);
        std::cout << "Entering directory: " << topLevelDir << std::endl;
    } else {
        std::cout << "Already in directory: " << topLevelDir << std::endl;
    }
}
void CommandLine::create() {
    std::string newDir = "/tmp/raii_test/dir0/";

    std::string file0 = newDir + "file0";
    std::ofstream outFile0(file0);
    outFile0.close();
    tempFiles.push_back(file0);
    std::cout << "Created file: " << file0 << std::endl;

    std::string file1 = newDir + "file1";
    std::ofstream outFile1(file1);
    outFile1.close();
    tempFiles.push_back(file1);
    std::cout << "Created file: " << file1 << " inside " << newDir << std::endl;

    leave(); // Ensure leaving dir0 before creating file2

    if (!tempDirs.empty() && tempDirs.back().getPath() == "/tmp/raii_test/dir0") {
        std::string file2 = "/tmp/raii_test/file2";
        std::ofstream outFile2(file2);
        outFile2.close();
        tempFiles.push_back(file2);
        std::cout << "Created file: " << file2 << std::endl;
    }
}

void CommandLine::leave() {
    if (!tempDirs.empty()) {
        auto& lastTempDir = tempDirs.back();
        lastTempDir.removeFiles();

        // Check if it's dir0, leave it without removing
        if (lastTempDir.getPath().find("/tmp/raii_test/dir0") != std::string::npos) {
            std::cout << "Leaving directory /tmp/raii_test/dir0" << std::endl;
            tempDirs.pop_back();
        } else {
            std::cout << "Leaving directory " << lastTempDir.getPath() << std::endl;
            lastTempDir.removeDirectory();
            tempDirs.pop_back();
        }
    } else {
        std::cout << "No directory to leave." << std::endl;
    }
}


void CommandLine::list() {
    for (size_t i = 0; i < tempFiles.size(); ++i) {
        std::cout << "[" << i << "] " << tempFiles[i] << std::endl;
}
}
void CommandLine::remove(int index) {
    if (index >= 0 && static_cast<size_t>(index) < tempFiles.size()) {
std::cout << "Removing file at index " << index << ": " << tempFiles[index] << std::endl;
        tempFiles.erase(tempFiles.begin() + index);
    } else {
std::cerr << "Invalid file index to remove!" << std::endl;
    }
}

void CommandLine::quit() {
    leave(); // Make sure to leave before quitting
    tempFiles.clear();
    tempDirs.clear();
    std::cout << "Quitting program." << std::endl;
}
//---------------------------------------------------------------------------
} // namespace raii
//---------------------------------------------------------------------------
