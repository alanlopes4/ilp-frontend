target triple = 'x86_64-pc-linux-gnu'
define i32 @main() #0 {
%1 = alloca i32, align 4
%2 = alloca i32, align 4
%3 = alloca i32, align 4
%4 = alloca i32, align 4
store i32 2, i32* %4, align 4
%5 = load i32, i32*  %4, align 4
%6 = alloca i32, align 4
store i32 1, i32* %6, align 4
%7 = load i32, i32*  %6, align 4
%8 = sub nsw i32 %7, %5
store i32 %8 , i32* %1, align 4
%10 = alloca i32, align 4
store i32 2, i32* %10, align 4
%11 = load i32, i32*  %10, align 4
%12 = alloca i32, align 4
store i32 1, i32* %12, align 4
%13 = load i32, i32*  %12, align 4
%14 = mul nsw i32 %13, %11
store i32 %14 , i32* %2, align 4
%15 = alloca i32, align 4
store i32 2, i32* %15, align 4
%16 = load i32, i32*  %15, align 4
%17 = load i32, i32*  %1, align 4
18 = add 1, 16
store i32 %18 , i32* %3, align 4
%19 = sub nsw i32 %18, %14
store i32 %19 , i32* %4, align 4
%21 = alloca i32, align 4
store i32 2, i32* %21, align 4
%22 = load i32, i32*  %21, align 4
%23 = alloca i32, align 4
store i32 2, i32* %23, align 4
%24 = load i32, i32*  %23, align 4
%25 = call double @pow(double %24, double %22)
%26 = fptosi double %24 to i32
store i32 %26 , i32* %5, align 4
ret i32 0
 }