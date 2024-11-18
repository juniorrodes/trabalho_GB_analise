build:
	mkdir -p bin
	gcc main.c -o ./bin/solution -g

run: build
	./bin/solution $(file_name)
