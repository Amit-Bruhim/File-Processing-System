#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

char buffer[516];
char c;
int i = 0;
int fp_data;
int fp_req;
int fp_result;

void clean_buffer(int i);
void process_request(char *request);

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        perror("argc != 3");
        return 1;
    }
    fp_data = open(argv[1], O_RDWR);
    if (fp_data < 0)
    {
        perror("data.txt");
        return 1;
    }
    fp_req = open(argv[2], O_RDONLY);
    if (fp_req < 0)
    {
        perror("requests.txt");
        return 1;
    }
    fp_result = open("read_results.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    while (read(fp_req, &c, 1) != 0)
    {
        if (c != '\n')
        {
            buffer[i] = c;
            i++;
        }
        else
        {
            buffer[i] = '\0';
            process_request(buffer);
            clean_buffer(i);
            i = 0;
        }
    }
    buffer[i] = '\0';
    process_request(buffer);

    close(fp_data);
    close(fp_req);
    close(fp_result);

    return 0;
}

void clean_buffer(int i)
{
    for (int j = 0; j < i; j++)
    {
        buffer[j] = '\0';
    }
}

void read_data(int start, int end)
{
    // printf("start: %d, end: %d\n", start, end);
    off_t size = lseek(fp_data, 0, SEEK_END);
    if (start < 0 || end > size)
    {
        return;
    }
    lseek(fp_data, start, SEEK_SET);
    char data[end - start + 1];
    // printf("data: ");
    read(fp_data, data, end - start + 1);
    // printf("%s", data);
    // printf("\n");
    data[end - start + 1] = '\0';
    write(fp_result, data, end - start + 1);
    write(fp_result, "\n", 1);

    
}


void write_data(int offset, char data[], int len)
{
    off_t size = lseek(fp_data, 0, SEEK_END);
    if (offset < 0 || offset > size)
    {
        return;
    }
    int tail = size - offset;
    char tail_data[tail];
    lseek(fp_data, offset, SEEK_SET);
    read(fp_data, tail_data, tail);
    // tail_data[tail] = '\0';
    lseek(fp_data, offset, SEEK_SET);
    write(fp_data, data, len - 1);
    write(fp_data, tail_data, sizeof(tail_data));
}

void process_request(char request[])
{
    char r = request[0];
    switch (r)
    {
    case 'R':
        char start[256];
        char end[256];
        int i = 2;
        while (request[i] != ' ')
        {
            start[i - 2] = request[i];
            i++;
        }
        start[i - 2] = '\0';
        i++;
        int j = 0;
        while (request[i] != '\0')
        {
            end[j] = request[i];
            i++;
            j++;
        }
        end[j] = '\0';
        int start_pos = atoi(start);
        int end_pos = atoi(end);
        read_data(start_pos, end_pos);        
        break;
    case 'W':
        char offset[256];
        char data[256];
        int k = 2;
        while (request[k] != ' ')
        {
            offset[k - 2] = request[k];
            k++;
        }
        offset[k - 2] = '\0';
        k++;
        int l = 0;
        while (request[k] != '\0')
        {
            data[l] = request[k];
            k++;
            l++;
        }
        data[l] = '\0';
        int offset_pos = atoi(offset);
        write_data(offset_pos, data, l+1);
        break;
    
    default:
        exit(0);
        break;
    }
}