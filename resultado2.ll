target triple = "x86_64-pc-linux-gnu"
define i32 @main() #0 {
%1 = alloca i32, align 4
%2 = alloca i32, align 4
%3 = alloca i32, align 4
%4 = alloca double, align 4
%5 = alloca double, align 4

%6 = alloca i32, align 4
store i32 1, i32* %6, align 4
%7 = load i32, i32*  %6, align 4
store i32 %7 , i32* %1, align 4

%8 = alloca i32, align 4
store i32 2, i32* %8, align 4
%9 = load i32, i32*  %8, align 4
store i32 %9 , i32* %2, align 4

%10 = load i32, i32*  %1, align 4

%11 = load i32, i32*  %2, align 4

%12 = add nsw i32 %10, %11

%13 = load i32, i32*  %1, align 4

%14 = sub nsw i32 %12, %13
store i32 %14 , i32* %3, align 4

%15 = alloca double, align 4
store double 1.200000, double* %15, align 4
%16 = load double, double*  %15, align 4

%17 = alloca double, align 4
store double 2.200000, double* %17, align 4
%18 = load double, double*  %17, align 4

%19 = fadd  double %16, %18

%20 = alloca double, align 4
store double 4.500000, double* %20, align 4
%21 = load double, double*  %20, align 4

%22 = fadd  double %19, %21
store double %22 , double* %4, align 4

%23 = alloca double, align 4
store double 2.100000, double* %23, align 4
%24 = load double, double*  %23, align 4

%25 = alloca double, align 4
store double 2.400000, double* %25, align 4
%26 = load double, double*  %25, align 4
%27 = call double @pow(double %24, double %26)
store double %27 , double* %5, align 4

%28 = load double, double*  %4, align 4
%29 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %28)

%30 = load i32, i32*  %1, align 4
%31 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %30)
ret i32 0
 }declare i32 @printf(i8*, ...) #1
@.str.1 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
declare double @pow(double, double)
