//clang -c -S -emit-llvm teste.c

#include <stdio.h>

int main()
{

    int a = 2 + 1;
    int b = a * 1;
    int c = b / 1;
    int d = c ^ 1;
    int e = d - 1;
    int f = -e;
}
