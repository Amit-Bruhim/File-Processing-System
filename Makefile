# Makefile

CC = gcc
CFLAGS = -Wall
TARGET = file_processor
SRC = file_processor.c

run:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)
	./$(TARGET) data.txt requests.txt
