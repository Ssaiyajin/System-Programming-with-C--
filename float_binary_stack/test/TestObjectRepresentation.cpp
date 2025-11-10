#include "lib/ObjectRepresentation.hpp"
#include <array>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace object_representation;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
class CaptureCout {
    private:
    streambuf* sbuf;

    public:
    stringstream stream;

    CaptureCout() : sbuf(cout.rdbuf()) {
        cout.rdbuf(stream.rdbuf());
    }

    ~CaptureCout() {
        cout.rdbuf(sbuf);
    }
};
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(TestObjectRepresentation, printBinaryFloat) {
    array tests = {
        pair(
            +42.125f,
            string(
                "sign: 0\n"
                "exponent: 10000100\n"
                "mantissa: 01010001000000000000000\n")),
        pair(
            -42.125f,
            string(
                "sign: 1\n"
                "exponent: 10000100\n"
                "mantissa: 01010001000000000000000\n")),
        pair(
            +0.0f,
            string(
                "sign: 0\n"
                "exponent: 00000000\n"
                "mantissa: 00000000000000000000000\n")),
        pair(
            -0.0f,
            string(
                "sign: 1\n"
                "exponent: 00000000\n"
                "mantissa: 00000000000000000000000\n")),
        pair(
            +numeric_limits<float>::infinity(),
            string(
                "sign: 0\n"
                "exponent: 11111111\n"
                "mantissa: 00000000000000000000000\n")),
        pair(
            -numeric_limits<float>::infinity(),
            string(
                "sign: 1\n"
                "exponent: 11111111\n"
                "mantissa: 00000000000000000000000\n")),
    };
    for (auto& test : tests) {
        auto value = test.first;
        auto& expectedOutput = test.second;
        SCOPED_TRACE(value);
        {
            CaptureCout cout;
            printBinary(value);
            EXPECT_EQ(cout.stream.str(), expectedOutput);
        }
    }
}
//---------------------------------------------------------------------------
TEST(TestObjectRepresentation, printBinaryDouble) {
    array tests = {
        pair(
            +42.125,
            string(
                "sign: 0\n"
                "exponent: 10000000100\n"
                "mantissa: 0101000100000000000000000000000000000000000000000000\n")),
        pair(
            -42.125,
            string(
                "sign: 1\n"
                "exponent: 10000000100\n"
                "mantissa: 0101000100000000000000000000000000000000000000000000\n")),
        pair(
            +0.0,
            string(
                "sign: 0\n"
                "exponent: 00000000000\n"
                "mantissa: 0000000000000000000000000000000000000000000000000000\n")),
        pair(
            -0.0,
            string(
                "sign: 1\n"
                "exponent: 00000000000\n"
                "mantissa: 0000000000000000000000000000000000000000000000000000\n")),
        pair(
            +numeric_limits<double>::infinity(),
            string(
                "sign: 0\n"
                "exponent: 11111111111\n"
                "mantissa: 0000000000000000000000000000000000000000000000000000\n")),
        pair(
            -numeric_limits<double>::infinity(),
            string(
                "sign: 1\n"
                "exponent: 11111111111\n"
                "mantissa: 0000000000000000000000000000000000000000000000000000\n")),
    };
    for (auto& test : tests) {
        auto value = test.first;
        auto& expectedOutput = test.second;
        SCOPED_TRACE(value);
        {
            CaptureCout cout;
            printBinary(value);
            EXPECT_EQ(cout.stream.str(), expectedOutput);
        }
    }
}
//---------------------------------------------------------------------------
TEST(TestObjectRepresentation, pushPopFloat) {
    vector<unsigned char> buffer;
    push(buffer, 1.0f);

    double dValue;
    float fValue;

    ASSERT_FALSE(pop(buffer, dValue));
    ASSERT_TRUE(pop(buffer, fValue));
    ASSERT_EQ(fValue, 1.0f);
}
//---------------------------------------------------------------------------
TEST(TestObjectRepresentation, pushPopDouble) {
    vector<unsigned char> buffer;
    push(buffer, 1.0);

    double dValue;
    float fValue;

    ASSERT_FALSE(pop(buffer, fValue));
    ASSERT_TRUE(pop(buffer, dValue));
    ASSERT_EQ(dValue, 1.0);
}
//---------------------------------------------------------------------------
TEST(TestObjectRepresentation, pushPopMixed) {
    vector<unsigned char> buffer;
    push(buffer, 123.456);
    push(buffer, 42.24f);
    push(buffer, 245e123);

    double dValue = 0.0;
    float fValue = 0.0f;

    ASSERT_FALSE(pop(buffer, fValue));
    ASSERT_EQ(fValue, 0.0f);
    ASSERT_TRUE(pop(buffer, dValue));
    ASSERT_EQ(dValue, 245e123);
    ASSERT_FALSE(pop(buffer, dValue));
    ASSERT_EQ(dValue, 245e123);
    ASSERT_TRUE(pop(buffer, fValue));
    ASSERT_EQ(fValue, 42.24f);
    ASSERT_FALSE(pop(buffer, fValue));
    ASSERT_EQ(fValue, 42.24f);
    ASSERT_TRUE(pop(buffer, dValue));
    ASSERT_EQ(dValue, 123.456);
    ASSERT_FALSE(pop(buffer, fValue));
    ASSERT_EQ(fValue, 42.24f);
    ASSERT_FALSE(pop(buffer, dValue));
    ASSERT_EQ(dValue, 123.456);
}
//---------------------------------------------------------------------------
