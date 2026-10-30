#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define FILE_NAME "example-input-day7.txt"
#define FILE_NAME "input-day7.txt"

//
// Set
//

#define SET_NUM_ELEMENTS 32
#define SET_MAX_VAL (SET_NUM_ELEMENTS * 8) - 1

typedef struct
{
    uint8_t elements[SET_NUM_ELEMENTS];
} Set;

void set_init(Set *self) { memset(self->elements, 0, SET_NUM_ELEMENTS); }

void _set_get_byte_num_mask(uint8_t val, size_t *byte_num, uint8_t *byte_mask)
{
    *byte_num = val / 8;
    *byte_mask = 1 << (val % 8);
}

void set_add(Set *self, uint8_t val)
{
    size_t byte_num;
    uint8_t byte_mask;
    _set_get_byte_num_mask(val, &byte_num, &byte_mask);

    self->elements[byte_num] |= byte_mask;
}

void set_remove(Set *self, uint8_t val)
{
    size_t byte_num;
    uint8_t byte_mask;
    _set_get_byte_num_mask(val, &byte_num, &byte_mask);

    self->elements[byte_num] &= ~(byte_mask);
}

bool set_includes(Set *self, uint8_t val)
{
    size_t byte_num;
    uint8_t byte_mask;

    _set_get_byte_num_mask(val, &byte_num, &byte_mask);

    uint8_t res = self->elements[byte_num] & byte_mask;
    return res != 0;
}

void set_print(Set *self)
{
    for (uint8_t i = 0; i < SET_MAX_VAL; i += 1)
    {
        if (!set_includes(self, i))
        {
            continue;
        }
        printf("%d ", i);
    }
    printf("\n");
}

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
// Hauptbahnhof
//

void find_start_beam(Set *beams, char *line)
{
    for (size_t i = 0; line[i] != '\n'; i += 1)
    {
        if (line[i] != 'S')
        {
            continue;
        }
        set_add(beams, i);
        return;
    }

    // 'S' not found
    assert(false);
}

void process_splits(Set *beams, char *line, int *splits)
{
    for (uint8_t i = 0; line[i] != '\n'; i += 1)
    {
        if (line[i] == '.')
        {
            continue;
        }
        assert(line[i] == '^');

        if (!set_includes(beams, i))
        {
            continue;
        }

        *splits += 1;

        set_remove(beams, i);

        set_add(beams, i - 1);
        set_add(beams, i + 1);
    }
    return;
}

int process()
{
    Set beams;
    set_init(&beams);

    Input input;
    input_init(&input);

    char *line = input_next_line(&input);
    find_start_beam(&beams, line);

    int splits = 0;
    while ((line = input_next_line(&input)) != NULL)
    {
        process_splits(&beams, line, &splits);
    }

    return splits;
}

int main()
{
    int password = process();
    printf("password: %d\n", password);
}
