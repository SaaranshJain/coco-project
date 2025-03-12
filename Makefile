CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
TARGET = compiler
SRC_FOLDER = src
SRCS = $(SRC_FOLDER)/driver.c $(SRC_FOLDER)/lexer.c $(SRC_FOLDER)/lookup.c
INCLUDES = includes

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -I$(INCLUDES)

clean:
	rm -f $(TARGET) *.o
