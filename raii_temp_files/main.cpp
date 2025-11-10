#include "lib/CommandLine.hpp"
//---------------------------------------------------------------------------
using namespace raii;
//---------------------------------------------------------------------------
int main() {
    std::string directory = "/tmp/raii";
    CommandLine cli;
    cli.run(directory); // Pass the directory path here
}
//---------------------------------------------------------------------------

