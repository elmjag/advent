#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define FILE_NAME "example-input-day5.txt"
#define FILE_NAME "input-day5.txt"

//
// Vec
//

#define VEC_INC 1024

typedef struct
{
    size_t bytes_allocated;
    size_t element_size;
    size_t next_free_element;
    void *data;

} Vec;

void vec_init(Vec *self, size_t element_size)
{
    self->element_size = element_size;
    self->next_free_element = 0;

    self->bytes_allocated = VEC_INC;
    self->data = malloc(self->bytes_allocated);
}

void _vec_maybe_grow(Vec *self)
{
    if (self->next_free_element * self->element_size < self->bytes_allocated)
    {
        /* no need to grow */
        return;
    }

    self->bytes_allocated += VEC_INC;
    self->data = realloc(self->data, self->bytes_allocated);
}

void vec_push(Vec *self, void *data)
{
    _vec_maybe_grow(self);
    memcpy(self->data + self->next_free_element * self->element_size, data,
           self->element_size);
    self->next_free_element += 1;
}

void *vec_get(Vec *self, size_t i)
{
    assert(i < self->next_free_element);
    return self->data + i * self->element_size;
}

size_t vec_len(Vec *self) { return self->next_free_element; }

//
// Range
//

typedef struct
{
    uint64_t start;
    uint64_t end;
} Range;

//
// Utilities
//

void print_int_vec(Vec *v)
{
    size_t len = vec_len(v);
    for (size_t i = 0; i < len; i += 1)
    {
        uint64_t *val = vec_get(v, i);
        printf("%ld\n", *val);
    }
}

void print_range_vec(Vec *v)
{
    size_t len = vec_len(v);
    for (size_t i = 0; i < len; i += 1)
    {
        Range *val = vec_get(v, i);
        printf("%ld: %ld - %ld\n", i, val->start, val->end);
    }
}

//
// Hauptbahnhof
//

void load_data(Vec *ranges, Vec *ids)
{
    char *line = NULL;
    size_t len = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    assert(fp != NULL);

    while (getline(&line, &len, fp) != -1 && line[0] != '\n')
    {
        Range range;
        sscanf(line, "%ld-%ld", &(range.start), &(range.end));
        vec_push(ranges, &range);
    }

    while (getline(&line, &len, fp) != -1)
    {
        uint64_t id;
        sscanf(line, "%ld", &id);
        vec_push(ids, &id);
    }

    free(line);
    fclose(fp);
}

bool is_id_fresh(Vec *ranges, uint64_t id)
{
    size_t num_ranges = vec_len(ranges);
    for (size_t i = 0; i < num_ranges; i += 1)
    {
        Range *range = vec_get(ranges, i);
        if (range->start <= id && id <= range->end)
        {
            return true;
        }
    }

    return false;
}

int count_fresh_ids(Vec *ranges, Vec *ids)
{
    size_t num_ids = vec_len(ids);
    int fresh_ids = 0;

    for (size_t i = 0; i < num_ids; i += 1)
    {
        uint64_t *id = vec_get(ids, i);
        if (is_id_fresh(ranges, *id))
        {
            fresh_ids += 1;
        }
    }

    return fresh_ids;
}

int main()
{
    Vec ranges;
    vec_init(&ranges, sizeof(Range));

    Vec ids;
    vec_init(&ids, sizeof(uint64_t));

    load_data(&ranges, &ids);

    int password = count_fresh_ids(&ranges, &ids);
    printf("password: %d\n", password);
}
