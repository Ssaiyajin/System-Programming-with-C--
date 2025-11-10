#ifndef H_lib_CommandLine
#define H_lib_CommandLine
//---------------------------------------------------------------------------
#include "lib/TempDirectory.hpp"
#include "lib/TempFile.hpp"
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

    void run(const std::string& directory);
    void current();
    void leave();
    void enter(); // Declared with parameter
    void create();
    void list();
    void remove(int index);
    void quit();
private:
    std::string directory; // Member variable to hold directory path
    std::string currentDir;
    std::string topLevelDir; // Add topLevelDir member
    std::vector<TempDirectory> tempDirs;
    std::vector<std::string> tempFiles; // Declare tempFiles vector
     raii::CommandLine* cmdPtr;
};

} // namespace raii
//---------------------------------------------------------------------------
#endif


