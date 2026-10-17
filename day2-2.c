#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_NAME "input-day2.txt"

char *read_input_file();

typedef struct
{
    char *data;
    char *next_range;
} IdRanges;

void id_ranges_init(IdRanges *id_ranges)
{
    id_ranges->data = read_input_file();
    id_ranges->next_range = id_ranges->data;
}

void id_ranges_free(IdRanges *id_ranges) { free(id_ranges->data); }

bool id_ranges_next_range(IdRanges *id_ranges, long *start, long *end)
{
    if (id_ranges->next_range[0] == '\0')
    {
        return false;
    }

    char *cursor = id_ranges->next_range;
    if (cursor[0] == ',')
    {
        cursor += 1;
    }

    int res = sscanf(cursor, "%ld-%ld", start, end);
    assert(res == 2);

    for (; cursor[0] != ',' && cursor[0] != '\0'; cursor += 1)
    {
    }
    id_ranges->next_range = cursor;

    return true;
}

char *read_input_file()
{
    char *file_data;
    struct stat file_stat;

    int fd = open(FILE_NAME, O_RDONLY);
    assert(fd);

    fstat(fd, &file_stat);

    file_data = malloc(file_stat.st_size + 1);

    int res = read(fd, file_data, file_stat.st_size);
    assert(res != -1);
    file_data[file_stat.st_size] = '\0';

    close(fd);

    return file_data;
}

bool pattern_matches(char *pattern, size_t pattern_len, char *id, size_t id_len)
{
    if (id_len % pattern_len != 0)
    {
        return false;
    }

    for (size_t i = 0; i < id_len; i += 1)
    {
        if (pattern[i % pattern_len] != id[i])
        {
            return false;
        }
    }

    return true;
}

bool is_invalid_id(long id)
{
    char id_str[16];
    char pattern[16];

    sprintf(id_str, "%ld", id);
    size_t id_len = strlen(id_str);
    size_t half = id_len / 2;

    for (size_t pat_len = 1; pat_len <= half; pat_len += 1)
    {
        strncpy(pattern, id_str, pat_len);
        pattern[pat_len] = '\0';
        if (pattern_matches(pattern, pat_len, id_str, id_len))
        {
            return true;
        }
    }

    return false;
}

void main()
{
    long password = 0;
    long start;
    long end;

    IdRanges id_ranges;

    id_ranges_init(&id_ranges);

    while (id_ranges_next_range(&id_ranges, &start, &end))
    {
        for (long id = start; id <= end; id += 1)
        {
            if (is_invalid_id(id))
            {
                password += id;
            }
        }
    }

    id_ranges_free(&id_ranges);

    printf("password: %ld\n", password);
}