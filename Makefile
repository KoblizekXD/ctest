.PHONY: build example all run_example clean

all: clean build example

build/ctest: src/runner/main.c
	gcc -g -lelf src/runner/main.c -L build/ -o build/ctest

build/libctest.so: src/ctest.c
	gcc -g -fPIC -shared -o build/libctest.so src/ctest.c

build: build/ctest build/libctest.so

build/tests.so: src/tests/unit_tests.c build/libctest.so
	gcc -g -fPIC -L build/ -l ctest -shared -o build/tests.so src/tests/unit_tests.c

example: build/tests.so

run_example: build/ctest build/tests.so
	build/ctest build/tests.so

clean:
	rm -rf build/
	mkdir build/
