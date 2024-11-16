build:
	mkdir -p bin
	gcc main.c -o ./bin/solution

run: build
	./bin/solution
