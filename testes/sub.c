//clang -c -S -emit-llvm teste.c

#include <stdio.h>

int main(){

    int a = 1 - 2;
    int b = a - 4;
    int c = a - b;

    float a = 1 - 2;
    float b = a - 4;
    float c = a - b;

}
