#ifndef H_lib_Rational
#define H_lib_Rational

//---------------------------------------------------------------------------
#include <compare>
#include <cmath>
//---------------------------------------------------------------------------
namespace arithmetic {
//---------------------------------------------------------------------------
class Rational {
private:
    long long numerator;
    long long denominator;

    long long gcd(long long a, long long b);
    void simplify();

public:
    Rational();
    Rational(int numerator);
    explicit Rational(long long wholeNumber);
    Rational(long long num, long long denom);
    Rational(int numerator, int denominator);

    long long num() const;
    long long den() const;

    Rational inv() const;

    Rational operator-() const;
    Rational operator+() const;
    Rational operator-(const Rational& other) const;
    Rational operator+(const Rational& other) const;
    Rational operator*(const Rational& other) const;
    Rational operator/(const Rational& other) const;
    bool operator==(const Rational& other) const;
    bool operator!=(const Rational& other) const;
    bool operator<(const Rational& other) const;
    bool operator<=(const Rational& other) const;
    bool operator>(const Rational& other) const;
    bool operator>=(const Rational& other) const;

    std::strong_ordering operator<=>(const Rational& other) const;

    explicit operator double() const;
};
//---------------------------------------------------------------------------
} // namespace arithmetic
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
