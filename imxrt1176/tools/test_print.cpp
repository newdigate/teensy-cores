#include <string>
#include <cassert>
#include <cstdio>
#include "../Print.h"
struct StringPrint : public Print {
    std::string s;
    size_t write(uint8_t c) override { s.push_back((char)c); return 1; }
    using Print::write;
};
int main() {
    StringPrint p;
    p.print("hi"); assert(p.s == "hi"); p.s.clear();
    p.print(12345); assert(p.s == "12345"); p.s.clear();
    p.print(-42); assert(p.s == "-42"); p.s.clear();
    p.print(255, HEX); assert(p.s == "FF"); p.s.clear();
    p.println(1); assert(p.s == "1\r\n"); p.s.clear();
    p.print(3.14159, 2); assert(p.s == "3.14"); p.s.clear();
    printf("test_print OK\n");
    return 0;
}
