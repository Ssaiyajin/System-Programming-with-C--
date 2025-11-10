#include "simplevm.hpp"
#include <iostream>
//---------------------------------------------------------------------------
int main() {
    std::cout << "Starting the VM" << std::endl;
    int32_t A = simplevm::runVM();
    std::cout << "VM returned A = " << A << std::endl;
    return 0;
}
//---------------------------------------------------------------------------
