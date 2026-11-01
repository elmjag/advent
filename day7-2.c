#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define FILE_NAME "example-input-day7.txt"
#define FILE_NAME "input-day7.txt"

//
// Input
//
typedef struct
{
    FILE *fp;
    char *line;
    size_t line_len;
} Input;

void input_init(Input *self)
{
    self->line = NULL;
    self->line_len = 0;

    self->fp = fopen(FILE_NAME, "r");
    assert(self->fp != NULL);
}

char *input_next_line(Input *self)
{
    int res = getline(&(self->line), &(self->line_len), self->fp);
    if (res == -1)
    {
        return NULL;
    }
    return self->line;
}

//
// Utils
//
size_t get_line_len(char *line)
{
    size_t i = 0;
    for (; line[i] != '\n'; i += 1)
    {
    }

    return i;
}

void print_timeline(uint64_t *timeline, size_t len)
{
    for (size_t i = 0; i < len; i += 1)
    {
        printf("%ld ", timeline[i]);
    }
    printf("\n");
}

//
// Hauptbahnhof
//

void initial_timeline(char *line, uint64_t *timeline)
{
    size_t i = 0;
    for (; line[i] != 'S'; i += 1)
    {
    }

    timeline[i] = 1;
}

void update_timeline(char *line, uint64_t *timeline, size_t len)
{
    uint64_t res[len];
    memcpy(res, timeline, sizeof(res));

    for (size_t i = 0; i < len; i += 1)
    {
        if (timeline[i] == 0 || line[i] != '^')
        {
            continue;
        }

        res[i - 1] += timeline[i];
        res[i] = 0;
        res[i + 1] += timeline[i];
    }

    memcpy(timeline, res, sizeof(res));
}

uint64_t process(Input *input)
{
    char *line = input_next_line(input);
    size_t line_len = get_line_len(line);
    uint64_t timeline[line_len];
    memset(timeline, 0, sizeof(timeline));

    initial_timeline(line, timeline);

    while ((line = input_next_line(input)) != NULL)
    {
        update_timeline(line, timeline, line_len);
    }

    uint64_t res = 0;
    for (size_t i = 0; i < line_len; i += 1)
    {
        res += timeline[i];
    }

    return res;
}

int main()
{
    Input input;
    input_init(&input);

    printf("password: %ld\n", process(&input));
}
