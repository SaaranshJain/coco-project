CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = driver
SRCS = driver.c lexer.c parser.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)