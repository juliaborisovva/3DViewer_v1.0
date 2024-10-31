CC = gcc
CFLAGS =-Wall -Wextra -Werror -std=c11
CFLAGS_GCOV =-fprofile-arcs --coverage

OS=$(shell uname)
ifeq ($(OS), Linux)
  LIBS=-lcheck -lsubunit -lpthread -lrt -lm -g
  CHECK_LEAKS=CK_FORK=no valgrind --leak-check=full -s --log-file=log.txt
else
  LIBS=-lcheck
  CHECK_LEAKS=no leaks -atExit --
endif

all: install

install:
	rm -rf build
	mkdir build
	cd build && qmake6 ../frontend/3DViewer_v1.0.pro && make > /dev/null 2>&1
	mkdir -p ~/Applications; cp -r build/3DViewer_v1.app ~/Applications/3DViewer_v1.app

uninstall: clean
	rm -rf build
	rm -rf ~/Applications/3DViewer_v1.app

dist:
	cd .. && tar -zcvf ne_smart_biber3D.tar.gz src

tests:
	checkmk clean_mode=1 parser_tests.check > parser_tests.c
	gcc $(CFLAGS) $(CFLAGS_GCOV) *.c $(LIBS)
	./a.out

style:
	cp ../materials/linters/.clang-format .
	clang-format -n *.c *.h
	clang-format -i *.c *.h frontend/*.cpp frontend/*.h
	rm .clang-format

gcov_report: tests
	gcovr --html-details coverage.html
	open coverage.html

clean:
	rm -rf ./lcov_report ./report
	rm -rf app *.o *.a *.gcno *.gcda coverage*.html *.css *.out *.gch *.cfg coverage *.out *.txt parser_tests.c test.info build*
