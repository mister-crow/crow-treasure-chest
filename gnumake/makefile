INC := ../include
SRC := ../src
OBJ := ./obj
BIN := ./bin

HEADERS := \
	lockeless-queue.h \
	circullar-buffer-array.h \
	circullar-buffer.h \
	automata.h \
	automata-string-check.h \
	exception.h

HEADERS := $(addprefix $(INC)/, $(HEADERS))
#$(info VAR=$(HEADERS))

LDFLAGS := -pedantic
CFLAGS := -pedantic -ggdb -Wall -Wextra -std=c++14

.PHONY:
default: $(BIN)/test

$(BIN):
	mkdir $(BIN)

$(OBJ):
	mkdir $(OBJ)

$(OBJ)/%.o: $(SRC)/%.cpp $(DEPS) $(HEADERS) $(OBJ) makefile
	g++ $(CFLAGS) -c $< -I$(INC) -o $@

$(BIN)/test: $(OBJ)/main.o $(BIN)
	g++ $(LDFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ)/*.o
	rm -f $(BIN)/test
