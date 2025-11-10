#include "lib/Rational.hpp"
#include <sstream>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace arithmetic;
using namespace std;
//---------------------------------------------------------------------------
TEST(TestRational, numDen) {
    Rational a;
    Rational b = 42; /// for testing chenged the value to b()
    Rational c(42, 1);

    ASSERT_EQ(a.num(), 0);
    ASSERT_EQ(a.den(), 1);

    ASSERT_EQ(b.num(), 42);
    ASSERT_EQ(b.den(), 1);

    ASSERT_EQ(c.num(), 42);
    ASSERT_EQ(c.den(), 1);
}
//---------------------------------------------------------------------------
TEST(TestRational, canonicalization) {
    Rational a(84, 35);
    Rational b(-1, -2);
    Rational c(42, -1);

    ASSERT_EQ(a.num(), 12);
    ASSERT_EQ(a.den(), 5);

    ASSERT_EQ(b.num(), 1);
    ASSERT_EQ(b.den(), 2);

    ASSERT_EQ(c.num(), -42);
    ASSERT_EQ(c.den(), 1);
}
//---------------------------------------------------------------------------
TEST(TestRational, conversion) {
    Rational a(84, 35);
    double v = static_cast<double>(a);

    ASSERT_FLOAT_EQ(v, 2.4);
}
//---------------------------------------------------------------------------
TEST(TestRational, inv) {
    Rational a(-84, 35);
    Rational b = a.inv();

    ASSERT_EQ(b.num(), -5);
    ASSERT_EQ(b.den(), 12);
}
//---------------------------------------------------------------------------
TEST(TestRational, unaryMinus) {
    Rational a(84, 35);
    Rational b = -a;

    ASSERT_EQ(b.num(), -12);
    ASSERT_EQ(b.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, unaryPlus) {
    Rational a(84, 35);
    Rational b = +a;

    ASSERT_EQ(b.num(), 12);
    ASSERT_EQ(b.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, binaryMinus) {
    Rational a(84, 35);
    Rational b(6, 18);
    Rational c = a - b;

    ASSERT_EQ(c.num(), 31);
    ASSERT_EQ(c.den(), 15);
}
//---------------------------------------------------------------------------
TEST(TestRational, binaryPlus) {
    Rational a(84, 35);
    Rational b(6, 18);
    Rational c = a + b;

    ASSERT_EQ(c.num(), 41);
    ASSERT_EQ(c.den(), 15);
}
//---------------------------------------------------------------------------
TEST(TestRational, multiplication) {
    Rational a(84, 35);
    Rational b(6, 18);
    Rational c = a * b;

    ASSERT_EQ(c.num(), 4);
    ASSERT_EQ(c.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, division) {
    Rational a(84, 35);
    Rational b(6, 18);
    Rational c = a / b;

    ASSERT_EQ(c.num(), 36);
    ASSERT_EQ(c.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, comparison) {
    Rational a(1, 3);
    Rational b(6, 18);
    Rational c(1, 4);

    ASSERT_TRUE(a == a);
    ASSERT_TRUE(a <= a);
    ASSERT_FALSE(a < a);
    ASSERT_TRUE(a >= a);
    ASSERT_FALSE(a > a);
    ASSERT_EQ(a <=> a, strong_ordering::equal);

    ASSERT_TRUE(a == b);
    ASSERT_TRUE(a <= b);
    ASSERT_FALSE(a < b);
    ASSERT_TRUE(a >= b);
    ASSERT_FALSE(a > b);
    ASSERT_EQ(a <=> b, strong_ordering::equal);

    ASSERT_TRUE(b == a);
    ASSERT_TRUE(b <= a);
    ASSERT_FALSE(b < a);
    ASSERT_TRUE(b >= a);
    ASSERT_FALSE(b > a);
    ASSERT_EQ(b <=> a, strong_ordering::equal);

    ASSERT_FALSE(a == c);
    ASSERT_FALSE(a <= c);
    ASSERT_FALSE(a < c);
    ASSERT_TRUE(a >= c);
    ASSERT_TRUE(a > c);
    ASSERT_EQ(a <=> c, strong_ordering::greater);

    ASSERT_FALSE(c == a);
    ASSERT_TRUE(c <= a);
    ASSERT_TRUE(c < a);
    ASSERT_FALSE(c >= a);
    ASSERT_FALSE(c > a);
    ASSERT_EQ(c <=> a, strong_ordering::less);
}
//---------------------------------------------------------------------------
