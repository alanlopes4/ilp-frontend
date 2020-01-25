target triple = "x86_64-pc-linux-gnu"
define i32 @main() #0 {
%1 = alloca i32, align 4
%2 = alloca double, align 4
%3 = alloca i32, align 4
%4 = alloca i32, align 4

%5 = alloca i32, align 4
store i32 2, i32* %5, align 4
%6 = load i32, i32*  %5, align 4

%7 = alloca i32, align 4
store i32 1, i32* %7, align 4
%8 = load i32, i32*  %7, align 4

%9 = sub nsw i32 %8, %6
store i32 %9 , i32* %1, align 4

%10 = alloca double, align 4
store double 2.300000, double* %10, align 4
%11 = load double, double*  %10, align 4
store double %11 , double* %2, align 4

%12 = load i32, i32*  %1, align 4
%13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), i32 %12)

%14 = load double, double*  %2, align 4
%15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i32 0, i32 0), double %14)
ret i32 0
 }declare i32 @printf(i8*, ...) #1
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
