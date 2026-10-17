#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

//
// Slice
//

typedef struct
{
    int64_t *data;
    size_t len;
} Slice;

void slice_init(Slice *self, int64_t *data, size_t len)
{
    self->data = data;
    self->len = len;
}

void slice_subslice(Slice *self, size_t start, size_t end, Slice *dest)
{
    assert(start <= end);
    assert(start <= self->len);
    assert(end <= self->len);

    dest->data = self->data + start;
    dest->len = end - start;
}

uint64_t slice_at(Slice *self, size_t index)
{
    assert(self->data + index < self->data + self->len);
    return self->data[index];
}

void slice_print(Slice *self)
{
    printf("[ ");
    for (size_t i = 0; i < self->len; i += 1)
    {
        printf("%ld ", self->data[i]);
    }

    printf("] len: %ld\n", self->len);
}

//
// Banks
//

#define MAX_BANK_LEN 128

typedef struct
{
    FILE *fp;
    int64_t cur_bank[MAX_BANK_LEN];
} Banks;

void banks_init(Banks *self)
{
    self->fp = fopen("input-day3.txt", "r");
    assert(self->fp != NULL);
}

int64_t as_int(char c) { return (int64_t)c - '0'; }

bool banks_next(Banks *self, Slice *next_bank)
{
    char *line = NULL;
    size_t len = 0;

    ssize_t read = getline(&line, &len, self->fp);
    if (read == -1)
    {
        return false;
    }

    size_t p = 0;
    for (; line[p] != '\n'; p += 1)
    {
        assert(p < MAX_BANK_LEN);
        self->cur_bank[p] = as_int(line[p]);
    }

    slice_init(next_bank, self->cur_bank, p);

    return true;
}

void banks_drop(Banks *self) { fclose(self->fp); }

//
// der Algorithmus
//

void find_biggest_joltage(Slice *joltages, int64_t *val, size_t *pos)
{
    *val = -1;

    for (size_t i = 0; i < joltages->len; i += 1)
    {
        int64_t joltage = slice_at(joltages, i);
        if (joltage > *val)
        {
            *val = joltage;
            *pos = i;
        }
    }
    assert(*val != -1);
}

uint64_t ten_to_powerof(size_t p)
{
    uint64_t res = 1;
    for (size_t i = 0; i < p; i += 1)
    {
        res *= 10;
    }

    return res;
}

uint64_t get_max_joltage(Slice *joltages, size_t on_len)
{
    if (on_len == 0)
    {
        return 0;
    }

    /* cut out search part */
    Slice part;
    if (on_len == 1)
    {
        slice_subslice(joltages, 0, joltages->len, &part);
    }
    else
    {
        slice_subslice(joltages, 0, joltages->len - (on_len - 1), &part);
    }

    /* find max joltage */
    uint64_t max_joltage;
    size_t max_joltage_pos;
    find_biggest_joltage(&part, &max_joltage, &max_joltage_pos);

    /* cut out remaing slice */
    Slice remaining;
    slice_subslice(joltages, max_joltage_pos + 1, joltages->len, &remaining);

    return max_joltage * ten_to_powerof(on_len - 1) +
           get_max_joltage(&remaining, on_len - 1);
}

void main(void)
{
    Banks banks;
    banks_init(&banks);

    Slice bank;
    int64_t password = 0;
    while (banks_next(&banks, &bank))
    {
        password += get_max_joltage(&bank, 2);
    }

    printf("password: %ld\n", password);
}
