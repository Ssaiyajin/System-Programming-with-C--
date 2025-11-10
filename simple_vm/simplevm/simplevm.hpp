#ifndef H_simplevm_simplevm
#define H_simplevm_simplevm
//---------------------------------------------------------------------------
#include <cstdint>
//---------------------------------------------------------------------------
namespace simplevm {
//---------------------------------------------------------------------------
/// Run the VM. Return the value from register A when the execution finished.
int32_t runVM();
//---------------------------------------------------------------------------
/// Print a VM program that calculates the nth fibonacci number.
void fibonacciProgram(unsigned n);
//---------------------------------------------------------------------------
} // namespace simplevm
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
