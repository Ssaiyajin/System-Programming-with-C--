#include <iostream>
// ---------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------
unsigned foo(unsigned a, unsigned b) {
    unsigned c = a ^ b;
    unsigned d = a & b;
    while (d != 0) {
        d <<= 1;
        unsigned e = c ^ d;
        d = c & d;
        c = e;
    }
    return c;
}
// ---------------------------------------------------------------------------
unsigned bar(unsigned a, unsigned b) {
    if (b == 0)
        return 0;
    else if (b == 1)
        return a;
    else
        return foo(a, bar(a, b - 1));
}
// ---------------------------------------------------------------------------
} // namespace
// ---------------------------------------------------------------------------
int main() {
    unsigned a;
    unsigned b;
    std::cout << "Input first number: " << std::flush;
    std::cin >> a;
    std::cout << "Input second number: " << std::flush;
    std::cin >> b;
    unsigned c = bar(a, b);
    std::cout << c << std::endl;
}
// ---------------------------------------------------------------------------