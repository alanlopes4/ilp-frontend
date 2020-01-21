target triple = "x86_64-pc-linux-gnu"
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 3, i32* %1, align 4
  %3 = load i32, i32* %1, align 4
  %4 = mul nsw i32 %3, 4
  store i32 %4, i32* %2, align 4
  %5 = load i32, i32* %1, align 4
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %5)
  %7 = load i32, i32* %2, align 4
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %7)
  ret i32 0
}

declare i32 @printf(i8*, ...) #1

 int a = 1 + 2;
    int b = a * 4;
