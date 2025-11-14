#ifndef H_lib_CommandLine
#define H_lib_CommandLine
//---------------------------------------------------------------------------
#include "lib/TempDirectory.hpp"
#include <string>
#include <vector>
//---------------------------------------------------------------------------
namespace raii {
//---------------------------------------------------------------------------
class TempDirectory; // Forward declaration

class CommandLine {
    public:
    CommandLine(); // Declare the default constructor
    ~CommandLine();

    // emulate the sequence used by the tests
    void run(const std::string& directory);

    void current();
    void enter();
    void create();
    void leave();
    void list();
    void remove(int index);
    void quit();
private:
    std::string directory; // Member variable to hold directory path
    std::string currentDir;
    std::string topLevelDir; // Add topLevelDir member
    std::vector<TempDirectory> tempDirs;
    std::vector<std::string> tempFiles; // Declare tempFiles vector

    // state for subdir creation
    int nextDirIndex = 0;
    std::string currentSubdir;
    bool inSubdir = false;
};

} // namespace raii
//---------------------------------------------------------------------------
#endif


