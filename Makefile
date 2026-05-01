all: mkgen parser lexer
	g++ src/*.cpp src/gen/*.cpp -I src/gen -I src -g -o main -lfl

mkgen:
	mkdir -p src/gen

lexer:
	flex -o src/gen/lexer.cpp src/lexer.l

parser:
	bison -d src/parser.y -v -o src/gen/parser.cpp