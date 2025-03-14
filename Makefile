CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
TARGET = stage1exe
SRCS = driver.c lexer.c lookup.c parser.c firstAndFollow.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) *.o
