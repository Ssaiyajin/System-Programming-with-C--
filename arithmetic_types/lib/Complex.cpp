#include "lib/Complex.hpp"
#include <cmath>
//---------------------------------------------------------------------------
namespace arithmetic {
//---------------------------------------------------------------------------
// Implementations of the constructors
    Complex::Complex() : realPart(0.0), imagPart(0.0) {}
    Complex::Complex(double r) : realPart(r), imagPart(0.0) {}
    Complex::Complex(double r, double i) : realPart(r), imagPart(i) {}

    // Implementations of member functions and operators
    double Complex::real() const { return realPart; }
    double Complex::imag() const { return imagPart; }
    double Complex::abs() const { return std::sqrt(realPart * realPart + imagPart * imagPart); }
    double Complex::norm() const { return realPart * realPart + imagPart * imagPart; }
    double Complex::arg() const { return std::atan2(imagPart, realPart); }
    Complex Complex::conj() const { return Complex(realPart, -imagPart); }
    Complex Complex::operator-() const { return Complex(-realPart, -imagPart); }
    Complex Complex::operator+() const { return Complex(realPart, imagPart); }
    Complex Complex::operator-(const Complex& other) const {
        return Complex(realPart - other.realPart, imagPart - other.imagPart);
    }
    Complex Complex::operator+(const Complex& other) const {
        return Complex(realPart + other.realPart, imagPart + other.imagPart);
    }
    Complex Complex::operator*(const Complex& other) const {
        return Complex(realPart * other.realPart - imagPart * other.imagPart,
                       realPart * other.imagPart + imagPart * other.realPart);
    }
    Complex Complex::operator/(const Complex& other) const {
        double denominator = other.realPart * other.realPart + other.imagPart * other.imagPart;
        return Complex((realPart * other.realPart + imagPart * other.imagPart) / denominator,
                       (imagPart * other.realPart - realPart * other.imagPart) / denominator);
    }
    bool Complex::operator==(const Complex& other) const {
        return realPart == other.realPart && imagPart == other.imagPart;
    }
    bool Complex::operator!=(const Complex& other) const {
        return !(*this == other);
    }
//----------------------------------------------------------------------
} // namespace arithmetic
//---------------------------------------------------------------------------
