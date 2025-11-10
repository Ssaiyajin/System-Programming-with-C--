#ifndef H_lib_Complex
#define H_lib_Complex
#include <cmath>
//---------------------------------------------------------------------------
namespace arithmetic {
//---------------------------------------------------------------------------

    class Complex {
    private:
        double realPart;
        double imagPart;

    public:
        // Constructors
        Complex();
        Complex(double r);
        Complex(double r, double i);

        // Member functions
        double real() const;
        double imag() const;
        double abs() const;
        double norm() const;
        double arg() const;
        Complex conj() const;

        // Access functions for private members
        double getReal() const { return realPart; }
        double getImag() const { return imagPart; }

        // Operators
        Complex operator-() const;
        Complex operator+() const;
        Complex operator-(const Complex& other) const;
        Complex operator+(const Complex& other) const;
        Complex operator*(const Complex& other) const;
        Complex operator/(const Complex& other) const;
        bool operator==(const Complex& other) const;
        bool operator!=(const Complex& other) const;
    };
//----------------------------------------------------------------
} // namespace arithmetic
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
