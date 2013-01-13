DIRS = demolib player scenes

include Makefile.recursive

run: install
	cd build && LD_LIBRARY_PATH=. ./player -d libimpostor.so

.PHONY: run
