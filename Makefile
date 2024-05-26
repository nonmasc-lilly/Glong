CFLAGS = -std=c99 -O3
LDFLAGS = -lglfw -lGL -lglad

All: Build Test

Build: src/*.c
	gcc src/*.c -o build/glong $(CFLAGS) $(LDFLAGS)

Test: build/glong
	build/glong
