#ifndef H_lib_CommandLine
#define H_lib_CommandLine
//---------------------------------------------------------------------------
#include "lib/TempDirectory.hpp"
#include <string>
#include <vector>
#include <filesystem>
//---------------------------------------------------------------------------
namespace raii {
//---------------------------------------------------------------------------

class TempDirectory; // Forward declaration

class CommandLine {
public:
    CommandLine();
    ~CommandLine();

    // run reads commands from stdin and executes them (enter/create/leave/remove/list/quit)
    void run(const std::string& directory);

    void current();
    void enter();
    void create();
    void leave();
    void list();
    void remove(int index);
    void quit();

private:
    std::string directory;       // the path passed into run(), e.g. "/tmp/raii_test"
    std::string currentDir;
    std::string topLevelDirName; // basename like "raii_test"
    std::vector<TempDirectory> tempDirs; // first element will be base temp dir
    std::vector<std::string> tempFiles;  // tracked files (file paths)

    // state for subdir creation
    int nextDirIndex = 0;
    std::string currentSubdir;
    bool inSubdir = false;

    // create the base TempDirectory on demand (so the test's inotify watcher can be set up first)
    void ensureBaseDirectory();
};

} // namespace raii
//---------------------------------------------------------------------------
#endif


