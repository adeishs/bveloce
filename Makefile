# preferred compiler
CC = cc
CFLAGS = -std=c17 -W -Wall -pedantic
TEST_FLAG = -DBVELOCE_TEST__
TEST_BIN = ./bveloce-test
SRC = bveloce.c

obj = bveloce.o

all: $(obj)

${TEST_BIN}: ${SRC}
	${CC} ${CFLAGS} ${TEST_FLAG} -o ${TEST_BIN} ${SRC} && ${TEST_BIN}

test: ${TEST_BIN}

.PHONY: all clean test

$(obj): %.o: %.c

clean:
	rm -f $(obj)
