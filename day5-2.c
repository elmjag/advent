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
// Slice
//

typedef struct
{
    void *data;
    size_t element_size;
    size_t len;
} Slice;

void slice_init(Slice *slice, Vec *vec)
{
    slice->data = vec->data;
    slice->element_size = vec->element_size;
    slice->len = vec_len(vec);
}

size_t slice_len(Slice *slice) { return slice->len; }

void *slice_pop(Slice *slice)
{
    void *res = slice->data;

    slice->data += slice->element_size;
    slice->len -= 1;

    return res;
}

void slice_pop_into(Slice *slice, void *res)
{
    memcpy(res, slice->data, slice->element_size);

    slice->data += slice->element_size;
    slice->len -= 1;
}

void *slice_get(Slice *self, size_t i)
{
    assert(i < self->len);
    return self->data + i * self->element_size;
}

//
// Range
//

typedef struct
{
    uint64_t start;
    uint64_t end;
} Range;

void range_print(Range *r) { printf("(%ld -  %ld) ", r->start, r->end); }

void range_copy(Range *dest, Range *src) { memcpy(dest, src, sizeof(Range)); }

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

void print_range_slice(Slice *slice)
{
    size_t len = slice->len;
    for (size_t i = 0; i < len; i += 1)
    {
        Range *val = slice_get(slice, i);
        printf("%ld: %ld - %ld\n", i, val->start, val->end);
    }
}

//
// Hauptbahnhof
//

void load_data(Vec *ranges)
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

    free(line);
    fclose(fp);
}

int compare(const void *l, const void *r)
{
    const Range *r1 = l;
    const Range *r2 = r;

    if (r1->start < r2->start)
    {
        return -1;
    }

    if (r1->start > r2->start)
    {
        return 1;
    }

    return 0;
}

void sort_ranges(Vec *ranges)
{
    qsort(ranges->data, vec_len(ranges), ranges->element_size, compare);
}

bool merge_ranges(Range *r1, Range *r2, Range *res)
{
    if (r1->end >= r2->start)
    {
        if (r1->end < r2->end)
        {
            res->start = r1->start;
            res->end = r2->end;
        }
        else
        {
            res->start = r1->start;
            res->end = r1->end;
        }

        return true;
    }

    return false;
}

void squash_first(Vec *squashed, Slice *unsqushed)
{
    if (slice_len(unsqushed) == 0)
    {
        return;
    }

    Range merged;
    Range r1;
    slice_pop_into(unsqushed, &r1);
    while (slice_len(unsqushed) > 0)
    {
        bool was_merged = merge_ranges(&r1, slice_get(unsqushed, 0), &merged);
        if (was_merged)
        {
            range_copy(&r1, &merged);
            slice_pop(unsqushed);
        }
        else
        {
            break;
        }
    }

    vec_push(squashed, &r1);
    squash_first(squashed, unsqushed);
}

void squash_ranges(Vec *ranges, Vec *squashed)
{
    sort_ranges(ranges);

    vec_init(squashed, sizeof(Range));

    Slice unsqushed;
    slice_init(&unsqushed, ranges);

    squash_first(squashed, &unsqushed);
}

uint64_t count_fresh_ids(Vec *ranges)
{
    uint64_t fresh = 0;

    size_t len = vec_len(ranges);
    for (size_t i = 0; i < len; i += 1)
    {
        Range *r = vec_get(ranges, i);
        fresh += r->end - r->start + 1;
    }

    return fresh;
}

int main()
{
    Vec ranges;
    vec_init(&ranges, sizeof(Range));

    load_data(&ranges);

    Vec squashed;
    squash_ranges(&ranges, &squashed);

    printf("password: %ld\n", count_fresh_ids(&squashed));
}
