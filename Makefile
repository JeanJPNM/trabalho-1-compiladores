all: lexer parser
	g++ main.cpp lex.yy.c parser.tab.c -g -o main -lfl

lexer:
	flex lexer.l

parser:
	bison -d parser.y -v