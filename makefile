HEADERS := lockeless-queue.h circullar-buffer-array.h circullar-buffer.h exception.h makefile

LDFLAGS := -pedantic
CFLAGS := -pedantic -ggdb -Wall -Wextra -std=c++14

%.o: %.cpp $(HEADERS)
	g++ $(CFLAGS) -c $< -o $@

test: main.o
	g++ $(LDFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *.o
	rm -f ./test
