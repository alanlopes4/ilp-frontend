target triple = "x86_64-pc-linux-gnu"
define i32 @main() #0 {
%1 = alloca i32, align 4
%2 = alloca i32, align 4
%3 = alloca i32, align 4
%4 = alloca double, align 4
%5 = alloca double, align 4

%6 = alloca i32, align 4
store i32 2, i32* %6, align 4
%7 = load i32, i32*  %6, align 4

%8 = alloca i32, align 4
store i32 1, i32* %8, align 4
%9 = load i32, i32*  %8, align 4

%10 = sub nsw i32 %9, %7
store i32 %10 , i32* %1, align 4

%11 = alloca i32, align 4
store i32 2, i32* %11, align 4
%12 = load i32, i32*  %11, align 4

%13 = alloca i32, align 4
store i32 1, i32* %13, align 4
%14 = load i32, i32*  %13, align 4

%15 = mul nsw i32 %14, %12

%16 = alloca i32, align 4
store i32 2, i32* %16, align 4
%17 = load i32, i32*  %16, align 4

%18 = load i32, i32*  %1, align 4

%19 = add nsw i32 %18, %17

%20 = sub nsw i32 %19, %15
store i32 %20 , i32* %2, align 4

%21 = alloca i32, align 4
store i32 2, i32* %21, align 4
%22 = load i32, i32*  %21, align 4

%23 = alloca i32, align 4
store i32 2, i32* %23, align 4
%24 = load i32, i32*  %23, align 4
%25 = sitofp i32 %24 to double
%26 = sitofp i32 %22 to double
%27 = call double @pow(double %26, double %27)
%28 = fptosi double %27 to i32
store i32 %28 , i32* %3, align 4

%29 = alloca double, align 4
store double 1.200000, double* %29, align 4
%30 = load double, double*  %29, align 4

%31 = alloca double, align 4
store double 2.400000, double* %31, align 4
%32 = load double, double*  %31, align 4

%33 = fadd  double %32, %30
store double %33 , double* %4, align 4

%34 = alloca double, align 4
store double 2.500000, double* %34, align 4
%35 = load double, double*  %34, align 4

%36 = alloca double, align 4
store double 1.200000, double* %36, align 4
%37 = load double, double*  %36, align 4

%38 = fdiv  double %37, %35
store double %38 , double* %5, align 4
ret i32 0
 }declare double @pow(double, double)
