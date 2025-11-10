#include "lib/ObjectRepresentation.hpp"
#include <iostream>
#include <bitset>
#include <cstring>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace object_representation {
//---------------------------------------------------------------------------
void printBinary(float value)
// Print the binary representation of a float value
{
    unsigned int* ptr = reinterpret_cast<unsigned int*>(&value);
    unsigned int v = *ptr;

    int sign = (v >> 31) & 1;
    int exponent = (v >> 23) & ((1 << 8) - 1);
    int mantissa = v & ((1 << 23) - 1);

    // Formatting the binary representation as strings
    std::string signStr = std::to_string(sign);
    std::string exponentStr = std::bitset<8>(exponent).to_string();
    std::string mantissaStr = std::bitset<23>(mantissa).to_string();

    // Printing the formatted output
    std::cout << "sign: " << signStr << "\n"
              << "exponent: " << exponentStr << "\n"
              << "mantissa: " << mantissaStr << std::endl;
}
//---------------------------------------------------------------------------
void printBinary(double value)
// Print the binary representation of a double value
{
    uint64_t* ptr = reinterpret_cast<uint64_t*>(&value);
    uint64_t v = *ptr;

    int sign = (v >> 63) & 1;
    int exponent = (v >> 52) & ((1ULL << 11) - 1);
    uint64_t mantissa = v & ((1ULL << 52) - 1);

    // Formatting the binary representation as strings
    std::string signStr = std::to_string(sign);
    std::string exponentStr = std::bitset<11>(exponent).to_string();
    std::string mantissaStr = std::bitset<52>(mantissa).to_string();

    // Printing the formatted output exactly as expected by the test cases
    std::cout << "sign: " << signStr << "\n"
              << "exponent: " << exponentStr << "\n"
              << "mantissa: " << mantissaStr << std::endl;
}
//---------------------------------------------------------------------------
void push(vector<unsigned char>& stack, float value)
// Push a float onto buffer
{
    unsigned char* ptr = reinterpret_cast<unsigned char*>(&value);

    // Push the value bytes onto the stack
    for (size_t i = 0; i < sizeof(float); ++i) {
        stack.push_back(ptr[i]);
    }

    // Push the number of bytes used for the object representation
    stack.push_back(static_cast<unsigned char>(sizeof(float)));
}
//---------------------------------------------------------------------------
void push(vector<unsigned char>& stack, double value)
// Push a float onto buffer
{
   unsigned char* ptr = reinterpret_cast<unsigned char*>(&value);

    // Push the value bytes onto the stack
    for (size_t i = 0; i < sizeof(double); ++i) {
        stack.push_back(ptr[i]);
    }

    // Push the number of bytes used for the object representation
    stack.push_back(static_cast<unsigned char>(sizeof(double)));
    }
//---------------------------------------------------------------------------
bool pop(vector<unsigned char>& stack, float& value)
// Pop a float from buffer
{
    if (stack.size() < sizeof(float)) {
        return false;
    }

    unsigned int intValue = 0;
    for (unsigned i = 0; i < sizeof(float); ++i) {
        intValue |= static_cast<unsigned int>(stack.back()) << (8 * i);
        stack.pop_back();
    }

    std::memcpy(&value, &intValue, sizeof(float));

    return true;
    }
//---------------------------------------------------------------------------
bool pop(vector<unsigned char>& stack, double& value){
    if (stack.size() < sizeof(double)) {
        return false;
    }

    unsigned long long longValue = 0;
    for (unsigned i = 0; i < sizeof(double); ++i) {
        longValue |= static_cast<unsigned long long>(stack.back()) << (8 * i);
        stack.pop_back();
    }

    std::memcpy(&value, &longValue, sizeof(double));

    return true;
}
//---------------------------------------------------------------------------
} // namespace object_representation
//---------------------------------------------------------------------------
