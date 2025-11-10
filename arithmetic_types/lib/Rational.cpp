#include "lib/Rational.hpp"
#include <cmath>
//---------------------------------------------------------------------------
namespace arithmetic {
//---------------------------------------------------------------------------
long long Rational::gcd(long long a, long long b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

void Rational::simplify() {
    if (numerator == 0) {
        denominator = 1;
        return;
    }

    long long common = gcd(std::abs(numerator), std::abs(denominator));
    numerator /= common;
    denominator /= common;

    if (denominator < 0) {
        numerator = -numerator;
        denominator = -denominator;
    }
}

Rational::Rational() : numerator(0), denominator(1) {}
Rational::Rational(int num) : numerator(num), denominator(1) {}
Rational::Rational(long long wholeNumber) : numerator(wholeNumber), denominator(1) {}
Rational::Rational(int num, int denom) : numerator(num), denominator(denom) {
    simplify();
}


Rational::Rational(long long num, long long denom) : numerator(num), denominator(denom) {
    simplify();
}

long long Rational::num() const { return numerator; }

long long Rational::den() const { return denominator; }

Rational Rational::inv() const {
    return Rational(denominator, numerator);
}

Rational Rational::operator-() const {
    return Rational(-numerator, denominator);
}

Rational Rational::operator+() const {
    return *this;
}

Rational Rational::operator-(const Rational& other) const {
    return Rational(numerator * other.denominator - denominator * other.numerator,
                    denominator * other.denominator);
}

Rational Rational::operator+(const Rational& other) const {
    return Rational(numerator * other.denominator + denominator * other.numerator,
                    denominator * other.denominator);
}

Rational Rational::operator*(const Rational& other) const {
    return Rational(numerator * other.numerator, denominator * other.denominator);
}

Rational Rational::operator/(const Rational& other) const {
    return Rational(numerator * other.denominator, denominator * other.numerator);
}

bool Rational::operator==(const Rational& other) const {
    return numerator == other.numerator && denominator == other.denominator;
}

bool Rational::operator!=(const Rational& other) const {
    return !(*this == other);
}

bool Rational::operator<(const Rational& other) const {
    return numerator * other.denominator < denominator * other.numerator;
}

bool Rational::operator<=(const Rational& other) const {
    return numerator * other.denominator <= denominator * other.numerator;
}

bool Rational::operator>(const Rational& other) const {
    return numerator * other.denominator > denominator * other.numerator;
}

bool Rational::operator>=(const Rational& other) const {
    return numerator * other.denominator >= denominator * other.numerator;
}

std::strong_ordering Rational::operator<=>(const Rational& other) const {
    long long thisVal = numerator * other.denominator;
    long long otherVal = denominator * other.numerator;

    if (thisVal < otherVal) {
        return std::strong_ordering::less;
    } else if (thisVal > otherVal) {
        return std::strong_ordering::greater;
    } else {
        return std::strong_ordering::equal;
    }
}
Rational::operator double() const {
    return static_cast<double>(numerator) / static_cast<double>(denominator);
}


//---------------------------------------------------------------------------
} // namespace arithmetic
//---------------------------------------------------------------------------
