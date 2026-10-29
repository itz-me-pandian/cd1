all: compiler optimizer backend
parser.tab.c parser.tab.h: parser.y
	bison -d -o parser.tab.c parser.y
lex.yy.c: lexer.l parser.tab.h
	flex -o lex.yy.c lexer.l
compiler: parser.tab.c lex.yy.c
	gcc -Wall -Wextra -o compiler parser.tab.c lex.yy.c
optimizer: ../shared/optimizer.c
	gcc -Wall -Wextra -o optimizer ../shared/optimizer.c
backend: ../shared/backend.c
	gcc -Wall -Wextra -o backend ../shared/backend.c
clean:
	rm -f compiler optimizer backend parser.tab.c parser.tab.h lex.yy.c tac.txt optimized.txt target.txt
.PHONY: all clean
