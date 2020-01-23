fb3: fb3-2.y fb3-2.l
	bison -d fb3-2.y
	flex -o fb3-2.lex.c fb3-2.l
	gcc  -Wall -g -o fb3 fb3-2funcs.c fb3-2.tab.c fb3-2.lex.c -lfl -lm

clean:
	rm -rf fb3
