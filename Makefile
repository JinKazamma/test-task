CC = gcc
CFLAGS = -Wall -Wextra -O3 -g
TARGET = statdata_util
TEST_TARGET = test_util

all: $(TARGET) $(TEST_TARGET)

$(TARGET): main.o statdata.o
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_TARGET): test_util.o statdata.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c statdata.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(TARGET) $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: all clean test