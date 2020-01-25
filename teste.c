//clang -c -S -emit-llvm teste.c

#include <stdio.h>

int main()
{
    double a = 1.2;
    double b = 1.2;
    double c = a + b;

    int d = 1;
    int e = 2;
    int f = d + e;
}
