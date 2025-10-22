# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Target binary
TARGET = bin/file_processor

# Source files
SRC = src/file_processor.c

# Input files
DATA_FILE = files/data.txt
REQ_FILE = files/requests.txt

# Output file (inside files/)
RESULT_FILE = files/read_results.txt

# Ensure bin directory exists
$(shell mkdir -p bin)

# Default build
all: $(TARGET)

# Compile the program into bin/
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	./$(TARGET) $(DATA_FILE) $(REQ_FILE)

# Clean build files and output
clean:
	rm -f $(TARGET) $(RESULT_FILE)
