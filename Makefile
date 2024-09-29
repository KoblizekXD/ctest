.PHONY: build example all run_example clean

all: clean build example

build/ctest: src/runner/main.c
	gcc -lelf src/runner/main.c -o build/ctest

build: build/ctest

build/tests.so: src/tests/unit_tests.c
	gcc -g -fPIC -shared -o build/tests.so src/tests/unit_tests.c

example: build/tests.so

run_example: build/ctest build/tests.so
	build/ctest build/tests.so

clean:
	rm -rf build/
	mkdir build/
