//clang -c -S -emit-llvm teste.c

#include <stdio.h>

int main(){

    int a = 1 + 2;
    int b = a * 4;

    printf("%d\n",a);
    printf("%d\n",b);
}
