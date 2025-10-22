#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

// ------------------------------
// Global variables
// ------------------------------

// Buffer to store each line from the requests file
char buffer[516];

// Single character for reading from file
char c;

// Index for buffer
int i = 0;

// File descriptors
int fp_data;
int fp_req;
int fp_result;

// ------------------------------
// Function declarations
// ------------------------------
void clean_buffer(int i);
void process_request(char *request);
void read_data(int start, int end);
void write_data(int offset, char data[], int len);

// ------------------------------
// Main function
// ------------------------------
int main(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc != 3)
    {
        perror("argc != 3");
        return 1;
    }

    // Open the data file for reading and writing
    fp_data = open(argv[1], O_RDWR);
    if (fp_data < 0)
    {
        perror("data.txt");
        return 1;
    }

    // Open the requests file for reading
    fp_req = open(argv[2], O_RDONLY);
    if (fp_req < 0)
    {
        perror("requests.txt");
        return 1;
    }

    // Open the results file for writing (truncate if exists)
    fp_result = open("files/read_results.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Read the requests file character by character
    while (read(fp_req, &c, 1) != 0)
    {
        if (c != '\n')
        {
            // Append character to buffer
            buffer[i] = c;
            i++;
        }
        else
        {
            // End of line, process the request
            buffer[i] = '\0';
            process_request(buffer);

            // Clear buffer for next line
            clean_buffer(i);
            i = 0;
        }
    }

    // Process the last line if file doesn't end with newline
    buffer[i] = '\0';
    process_request(buffer);

    // Close all file descriptors
    close(fp_data);
    close(fp_req);
    close(fp_result);

    return 0;
}

// ------------------------------
// Clear the buffer
// ------------------------------
void clean_buffer(int i)
{
    for (int j = 0; j < i; j++)
    {
        buffer[j] = '\0';
    }
}

// ------------------------------
// Read data from the data file between start and end offsets
// Write result to read_results.txt
// ------------------------------
void read_data(int start, int end)
{
    off_t size = lseek(fp_data, 0, SEEK_END);

    // Return if offsets are invalid
    if (start < 0 || end > size)
    {
        return;
    }

    // Move file pointer to start position
    lseek(fp_data, start, SEEK_SET);

    // Read data into buffer
    char data[end - start + 1];
    read(fp_data, data, end - start + 1);

    // Null terminate (for safety)
    data[end - start + 1] = '\0';

    // Write data to results file with newline
    write(fp_result, data, end - start + 1);
    write(fp_result, "\n", 1);
}

// ------------------------------
// Write data to the data file at given offset
// Shift existing content forward
// ------------------------------
void write_data(int offset, char data[], int len)
{
    off_t size = lseek(fp_data, 0, SEEK_END);

    // Return if offset is invalid
    if (offset < 0 || offset > size)
    {
        return;
    }

    // Calculate how much data is after the offset
    int tail = size - offset;
    char tail_data[tail];

    // Read the tail of the file into temporary buffer
    lseek(fp_data, offset, SEEK_SET);
    read(fp_data, tail_data, tail);

    // Move file pointer back to offset and write new data
    lseek(fp_data, offset, SEEK_SET);
    write(fp_data, data, len - 1);

    // Write the old tail back to file
    write(fp_data, tail_data, sizeof(tail_data));
}

// ------------------------------
// Process a single request line
// ------------------------------
void process_request(char request[])
{
    char r = request[0];

    switch (r)
    {
    case 'R':
        // --------------------------
        // Read request
        // Format: R <start> <end>
        // --------------------------
        char start[256];
        char end[256];
        int i = 2;

        // Parse start offset
        while (request[i] != ' ')
        {
            start[i - 2] = request[i];
            i++;
        }
        start[i - 2] = '\0';
        i++;

        // Parse end offset
        int j = 0;
        while (request[i] != '\0')
        {
            end[j] = request[i];
            i++;
            j++;
        }
        end[j] = '\0';

        // Convert to integers and read
        int start_pos = atoi(start);
        int end_pos = atoi(end);
        read_data(start_pos, end_pos);
        break;

    case 'W':
        // --------------------------
        // Write request
        // Format: W <offset> <data>
        // --------------------------
        char offset[256];
        char data[256];
        int k = 2;

        // Parse offset
        while (request[k] != ' ')
        {
            offset[k - 2] = request[k];
            k++;
        }
        offset[k - 2] = '\0';
        k++;

        // Parse data to write
        int l = 0;
        while (request[k] != '\0')
        {
            data[l] = request[k];
            k++;
            l++;
        }
        data[l] = '\0';

        // Convert offset to integer and write
        int offset_pos = atoi(offset);
        write_data(offset_pos, data, l + 1);
        break;

    default:
        // Quit on any other character (including 'Q')
        exit(0);
        break;
    }
}
