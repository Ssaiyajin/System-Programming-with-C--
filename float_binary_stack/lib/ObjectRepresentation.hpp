#ifndef H_lib_ObjectRepresentation
#define H_lib_ObjectRepresentation
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
namespace object_representation {
//---------------------------------------------------------------------------
/// Print the binary representation of a float value
void printBinary(float value);
//---------------------------------------------------------------------------
/// Print the binary representation of a double value
void printBinary(double value);
//---------------------------------------------------------------------------
/// Push a float into buffer
void push(std::vector<unsigned char>& stack, float value);
//---------------------------------------------------------------------------
/// Push a double into buffer
void push(std::vector<unsigned char>& stack, double value);
//---------------------------------------------------------------------------
/// Pop a float from buffer
bool pop(std::vector<unsigned char>& stack, float& value);
//---------------------------------------------------------------------------
/// Pop a double from buffer
bool pop(std::vector<unsigned char>& stack, double& value);
//---------------------------------------------------------------------------
} // namespace object_representation
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
