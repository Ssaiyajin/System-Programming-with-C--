#include "simplevm/simplevm.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
class SetCin {
    private:
    std::streambuf* sbuf;
    std::stringstream stream;

    public:
    SetCin(std::string input) : sbuf(std::cin.rdbuf()), stream(std::move(input)) {
        std::cin.rdbuf(stream.rdbuf());
    }

    ~SetCin() {
        std::cin.rdbuf(sbuf);
    }
};
//---------------------------------------------------------------------------
class CaptureCout {
    private:
    std::streambuf* sbuf;

    public:
    std::stringstream stream;

    CaptureCout() : sbuf(std::cout.rdbuf()) {
        std::cout.rdbuf(stream.rdbuf());
    }

    ~CaptureCout() {
        std::cout.rdbuf(sbuf);
    }
};
//---------------------------------------------------------------------------
static int32_t runProgram(std::string program, std::string* output = nullptr) {
    SetCin cin{std::move(program)};
    CaptureCout cout;
    int32_t result = simplevm::runVM();
    if (output)
        *output = cout.stream.str();
    return result;
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, EmptyRegister) {
    EXPECT_EQ(runProgram("0"), 0);
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, IRegs) {
    {
        SCOPED_TRACE("movi");
        ASSERT_EQ(runProgram("10 A 123\n0"), 123);
    }
    {
        SCOPED_TRACE("movi2");
        ASSERT_EQ(runProgram("10 B 123\n0"), 0);
    }
    {
        SCOPED_TRACE("loada");
        ASSERT_EQ(runProgram("10 A 123\n20 A\n0"), 123);
        ASSERT_EQ(runProgram("10 C 123\n20 C\n0"), 123);
    }
    {
        SCOPED_TRACE("storea");
        ASSERT_EQ(runProgram("10 A 123\n21 D\n0"), 123);
        ASSERT_EQ(runProgram("10 A 123\n21 A\n0"), 123);
        ASSERT_EQ(runProgram("10 A 123\n21 D\n10 A 456\n20 D\n0"), 123);
    }
    {
        SCOPED_TRACE("swapab");
        ASSERT_EQ(runProgram("10 A 123\n10 B 456\n22\n0"), 456);
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, FRegs) {
    {
        SCOPED_TRACE("movf + ftoi");
        ASSERT_EQ(runProgram("11 X 1.234\n41\n0"), 1);
    }
    {
        SCOPED_TRACE("ftoi2");
        ASSERT_EQ(runProgram("41\n0"), 0);
    }
    {
        SCOPED_TRACE("itof");
        ASSERT_EQ(runProgram("10 A 123\n40\n10 A 456\n41\n0"), 123);
    }
    {
        SCOPED_TRACE("storex");
        ASSERT_EQ(runProgram("11 X 123\n31 X\n41\n0"), 123);
        ASSERT_EQ(runProgram("11 X 123\n31 Y\n41\n0"), 123);
    }
    {
        SCOPED_TRACE("swapxy");
        ASSERT_EQ(runProgram("11 X 123\n11 Y 456\n32\n41\n0"), 456);
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, IntArithmetic) {
    {
        SCOPED_TRACE("addi");
        EXPECT_EQ(runProgram("10 A 9\n10 B 10\n50\n0"), 19);
    }
    {
        SCOPED_TRACE("addi overflow");
        EXPECT_EQ(runProgram("10 A 2147483647\n10 B 1\n50\n0"), -2147483648);
        EXPECT_EQ(runProgram("10 A 2147483647\n10 B 2147483647\n50\n0"), -2);
    }
    {
        SCOPED_TRACE("addi underflow");
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B -1\n50\n0"), 2147483647);
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B -2147483648\n50\n0"), 0);
    }
    {
        SCOPED_TRACE("subi");
        EXPECT_EQ(runProgram("10 A 10\n10 B 9\n51\n0"), 1);
    }
    {
        SCOPED_TRACE("subi underflow");
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B 1\n51\n0"), 2147483647);
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B -2147483648\n51\n0"), 0);
    }
    {
        SCOPED_TRACE("subi overflow");
        EXPECT_EQ(runProgram("10 A 2147483647\n10 B -1\n51\n0"), -2147483648);
        EXPECT_EQ(runProgram("10 A 2147483647\n10 B -2147483647\n51\n0"), -2);
    }
    {
        SCOPED_TRACE("rsubi");
        EXPECT_EQ(runProgram("10 A 9\n10 B 10\n52\n0"), 1);
    }
    {
        SCOPED_TRACE("rsubi underflow");
        EXPECT_EQ(runProgram("10 A 1\n10 B -2147483648\n52\n0"), 2147483647);
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B -2147483648\n52\n0"), 0);
    }
    {
        SCOPED_TRACE("rsubi overflow");
        EXPECT_EQ(runProgram("10 A -1\n10 B 2147483647\n52\n0"), -2147483648);
        EXPECT_EQ(runProgram("10 A -2147483647\n10 B 2147483647\n52\n0"), -2);
    }
    {
        SCOPED_TRACE("muli");
        EXPECT_EQ(runProgram("10 A 2\n10 B 3\n53\n0"), 6);
    }
    {
        SCOPED_TRACE("muli overflow");
        EXPECT_EQ(runProgram("10 A 2147483647\n10 B 2\n53\n0"), -2);
        EXPECT_EQ(runProgram("10 A 2147483647\n10 B 2147483647\n53\n0"), 1);
    }
    {
        SCOPED_TRACE("muli underflow");
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B 2\n53\n0"), 0);
        EXPECT_EQ(runProgram("10 A -2147483648\n10 B 2147483647\n53\n0"), -2147483648);
    }
    {
        SCOPED_TRACE("divi");
        EXPECT_EQ(runProgram("10 A 5\n10 B 2\n54\n0"), 2);
        EXPECT_EQ(runProgram("10 A 5\n10 B 2\n54\n22\n0"), 1);
        EXPECT_EQ(runProgram("10 A -5\n10 B -2\n54\n0"), 2);
        EXPECT_EQ(runProgram("10 A -5\n10 B -2\n54\n22\n0"), -1);
        EXPECT_EQ(runProgram("10 A -5\n10 B 2\n54\n0"), -2);
        EXPECT_EQ(runProgram("10 A -5\n10 B 2\n54\n22\n0"), -1);
        EXPECT_EQ(runProgram("10 A 5\n10 B -2\n54\n0"), -2);
        EXPECT_EQ(runProgram("10 A 5\n10 B -2\n54\n22\n0"), 1);
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, FloatArithmetic) {
    {
        SCOPED_TRACE("addf");
        EXPECT_EQ(runProgram("11 X 1.0\n11 Y 2.0\n60\n41\n0"), 3);
    }
    {
        SCOPED_TRACE("subf");
        EXPECT_EQ(runProgram("11 X 1.0\n11 Y 2.0\n61\n41\n0"), -1);
    }
    {
        SCOPED_TRACE("mulf");
        EXPECT_EQ(runProgram("11 X 2.0\n11 Y 3.0\n62\n41\n0"), 6);
    }
    {
        SCOPED_TRACE("divf");
        EXPECT_EQ(runProgram("11 X 6.0\n11 Y 2.0\n63\n41\n0"), 3);
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, DivisionByZero) {
    {
        SCOPED_TRACE("divi");
        std::string output;
        EXPECT_EQ(runProgram("10 A 123\n54\n", &output), 123);
        EXPECT_EQ(output, "division by 0\n");
    }
    {
        SCOPED_TRACE("divf");
        std::string output;
        EXPECT_EQ(runProgram("10 A 123\n63\n", &output), 123);
        EXPECT_EQ(output, "division by 0\n");
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, Fibonacci) {
    int32_t a, b, c;
    a = 0;
    b = 1;
    c = 0;
    for (unsigned n = 0; n < 40; ++n) {
        SCOPED_TRACE(n);
        std::string fibonacciProgram;
        {
            CaptureCout cout;
            simplevm::fibonacciProgram(n);
            fibonacciProgram = cout.stream.str();
        }
        ASSERT_EQ(runProgram(fibonacciProgram), a);
        a = b + c;
        b = c;
        c = a;
    }
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
