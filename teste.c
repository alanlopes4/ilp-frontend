//clang -c -S -emit-llvm teste.c

#include <stdio.h>

int main()
{
    int a = 2;
    printf("%d\n", a);
    double b = 2.3;
    printf("%f\n", b);
}
