target triple = "x86_64-pc-linux-gnu"
define i32 @main() #0 {
%1 = alloca i32, align 4
%2 = alloca double, align 4
%3 = alloca double, align 4
%4 = alloca i32, align 4
%5 = alloca i32, align 4
%6 = alloca i32, align 4

%7 = alloca i32, align 4
store i32 2, i32* %7, align 4
%8 = load i32, i32*  %7, align 4

%9 = alloca i32, align 4
store i32 1, i32* %9, align 4
%10 = load i32, i32*  %9, align 4

%11 = sub nsw i32 %10, %8
store i32 %11 , i32* %1, align 4

%12 = alloca double, align 4
store double 2.300000, double* %12, align 4
%13 = load double, double*  %12, align 4
store double %13 , double* %2, align 4

%14 = alloca double, align 4
store double 5.600000, double* %14, align 4
%15 = load double, double*  %14, align 4

%16 = alloca double, align 4
store double 3.400000, double* %16, align 4
%17 = load double, double*  %16, align 4

%18 = alloca double, align 4
store double 1.200000, double* %18, align 4
%19 = load double, double*  %18, align 4

%20 = fadd  double %19, %17

%21 = fadd  double %20, %15
store double %21 , double* %3, align 4

%22 = load i32, i32*  %1, align 4
%23 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %22)

%24 = load double, double*  %2, align 4
%25 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %24)

%26 = load double, double*  %2, align 4

%27 = load double, double*  %3, align 4

%28 = fadd  double %27, %26
%29 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %28)
ret i32 0
 }declare i32 @printf(i8*, ...) #1
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
