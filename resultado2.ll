target triple = "x86_64-pc-linux-gnu"
define i32 @main() #0 {
%1 = alloca i32, align 4
%2 = alloca double, align 4
%3 = alloca double, align 4
%4 = alloca i32, align 4

%5 = alloca i32, align 4
store i32 1, i32* %5, align 4
%6 = load i32, i32*  %5, align 4

%7 = alloca i32, align 4
store i32 2, i32* %7, align 4
%8 = load i32, i32*  %7, align 4

%9 = sub nsw i32 %6, %8
store i32 %9 , i32* %1, align 4

%10 = alloca double, align 4
store double 2.300000, double* %10, align 4
%11 = load double, double*  %10, align 4
store double %11 , double* %2, align 4

%12 = alloca double, align 4
store double 1.200000, double* %12, align 4
%13 = load double, double*  %12, align 4

%14 = alloca double, align 4
store double 3.400000, double* %14, align 4
%15 = load double, double*  %14, align 4

%16 = fadd  double %13, %15

%17 = alloca double, align 4
store double 5.600000, double* %17, align 4
%18 = load double, double*  %17, align 4

%19 = fadd  double %16, %18
store double %19 , double* %3, align 4

%20 = load i32, i32*  %1, align 4

%21 = alloca i32, align 4
store i32 2, i32* %21, align 4
%22 = load i32, i32*  %21, align 4

%23 = add nsw i32 %20, %22

%24 = alloca i32, align 4
store i32 3, i32* %24, align 4
%25 = load i32, i32*  %24, align 4

%26 = add nsw i32 %23, %25

%27 = alloca i32, align 4
store i32 4, i32* %27, align 4
%28 = load i32, i32*  %27, align 4

%29 = add nsw i32 %26, %28
store i32 %29 , i32* %4, align 4

%30 = load i32, i32*  %1, align 4

%31 = load i32, i32*  %4, align 4

%32 = add nsw i32 %30, %31
%33 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %32)

%34 = load double, double*  %2, align 4
%35 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %34)

%36 = load double, double*  %3, align 4

%37 = load double, double*  %2, align 4

%38 = fadd  double %36, %37
%39 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %38)
ret i32 0
 }declare i32 @printf(i8*, ...) #1
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
