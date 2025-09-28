files := src/main.c
target := build/index.html

.PHONY: build test all clean

build:
	emcc $(files) -o $(target) -I. \
	-s USE_SDL=2
	
	emrun --no-browser build