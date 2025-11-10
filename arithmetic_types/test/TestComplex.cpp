#include "lib/Complex.hpp"
#include <cmath>
#include <sstream>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace arithmetic;
using namespace std;
//---------------------------------------------------------------------------
TEST(TestComplex, realImag) {
    Complex a;
    Complex b = 42.0;
    Complex c(1.0, 2.0);
 
    ASSERT_FLOAT_EQ(a.real(), 0); // for testing i changed the test case a bit just changed from a.real to a.realValue
    ASSERT_FLOAT_EQ(a.imag(), 0); // for testing i changed the test case a bit just changed from a.imag to a.imagValue

    ASSERT_FLOAT_EQ(b.real(), 42.0);
    ASSERT_FLOAT_EQ(b.imag(), 0);

    ASSERT_FLOAT_EQ(c.real(), 1.0);
    ASSERT_FLOAT_EQ(c.imag(), 2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, abs) {
    Complex a(3.0, 4.0);

    ASSERT_FLOAT_EQ(a.abs(), 5.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, norm) {
    Complex a(3.0, 4.0);

    ASSERT_FLOAT_EQ(a.norm(), 25.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, arg) {
    Complex a(1.0, 0.0);
    Complex b(0.0, 1.0);
    Complex c(-1.0, 0.0);
    Complex d(0.0, -1.0);

    ASSERT_FLOAT_EQ(a.arg(), 0.0);
    ASSERT_FLOAT_EQ(b.arg(), M_PI / 2);
    ASSERT_FLOAT_EQ(c.arg(), M_PI);
    ASSERT_FLOAT_EQ(d.arg(), -M_PI / 2);
}
//---------------------------------------------------------------------------
TEST(TestComplex, conj) {
    Complex a(1.0, 2.0);
    Complex c = a.conj();

    ASSERT_FLOAT_EQ(c.real(), 1.0);
    ASSERT_FLOAT_EQ(c.imag(), -2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, unaryMinus) {
    Complex a(1.0, 2.0);
    Complex b = -a;

    ASSERT_FLOAT_EQ(b.real(), -1.0);
    ASSERT_FLOAT_EQ(b.imag(), -2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, unaryPlus) {
    Complex a(1.0, 2.0);
    Complex b = +a;

    ASSERT_FLOAT_EQ(b.real(), 1.0);
    ASSERT_FLOAT_EQ(b.imag(), 2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, binaryPlus) {
    Complex a(1.0, 2.0);
    Complex b(3.0, 4.0);
    Complex c = a + b;

    ASSERT_FLOAT_EQ(c.real(), 4.0);
    ASSERT_FLOAT_EQ(c.imag(), 6.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, binaryMinus) {
    Complex a(1.0, 2.0);
    Complex b(3.0, 4.0);
    Complex c = a - b;

    ASSERT_FLOAT_EQ(c.real(), -2.0);
    ASSERT_FLOAT_EQ(c.imag(), -2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, multiplication) {
    Complex a(1.0, 2.0);
    Complex b(3.0, 4.0);
    Complex c = a * b;

    ASSERT_FLOAT_EQ(c.real(), -5.0);
    ASSERT_FLOAT_EQ(c.imag(), 10.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, division) {
    Complex a(1.0, 2.0);
    Complex b(3.0, 4.0);
    Complex c = a / b;

    ASSERT_FLOAT_EQ(c.real(), 11.0 / 25.0);
    ASSERT_FLOAT_EQ(c.imag(), 2.0 / 25.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, comparison) {
    Complex a(1.0, 2.0);
    Complex b(3.0, 4.0);

    ASSERT_TRUE(a == a);
    ASSERT_TRUE(b == b);
    ASSERT_FALSE(a == b);

    ASSERT_FALSE(a != a);
    ASSERT_FALSE(b != b);
    ASSERT_TRUE(a != b);
}
//---------------------------------------------------------------------------
