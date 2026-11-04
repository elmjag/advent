#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

// #define FILE_NAME "example-input-day9.txt"
#define FILE_NAME "input-day9.txt"

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
// Tile
//

typedef struct
{
    int32_t x;
    int32_t y;
} Tile;

void tile_print(Tile *self) { printf("Tile(%d, %d)\n", self->x, self->y); }

//
// Vec
//

#define VEC_INC 1024 * 4

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

Vec *vec_new(size_t element_size)
{
    Vec *v = malloc(sizeof(Vec));

    vec_init(v, element_size);

    return v;
}

void _vec_maybe_grow(Vec *self)
{
    if (self->next_free_element * (self->element_size + 1) <
        self->bytes_allocated)
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
    void *dest = self->data + self->next_free_element * self->element_size;
    memcpy(dest, data, self->element_size);
    self->next_free_element += 1;
}

void *vec_get(Vec *self, size_t i)
{
    assert(i < self->next_free_element);
    return self->data + i * self->element_size;
}

size_t vec_len(const Vec *self) { return self->next_free_element; }

//
// Hauptbahnhof
//

void load_data(Input *input, Vec *tiles)
{
    int res;
    char *line;
    Tile tile;

    while ((line = input_next_line(input)) != NULL)
    {
        res = sscanf(line, "%d,%d", &tile.x, &tile.y);
        assert(res == 2);
        //        tile_print(&tile);
        vec_push(tiles, &tile);
    }
}

uint64_t calc_area(Tile *t1, Tile *t2)
{
    uint64_t dx = abs(t2->x - t1->x) + 1;
    uint64_t dy = abs(t2->y - t1->y) + 1;

    return dx * dy;
}

uint64_t find_max_area(Vec *tiles)
{
    uint64_t max_area = 0;
    Tile *t1, *t2;
    size_t len = vec_len(tiles);

    for (size_t i = 0; i < len; i += 1)
    {
        t1 = vec_get(tiles, i);
        for (size_t j = i + 1; j < len; j += 1)
        {
            t2 = vec_get(tiles, j);
            max_area = MAX(calc_area(t1, t2), max_area);
        }
    }

    return max_area;
}

int main()
{
    Input input;
    input_init(&input);

    Vec tiles;
    vec_init(&tiles, sizeof(Tile));

    load_data(&input, &tiles);

    uint64_t password = find_max_area(&tiles);
    printf("passward: %ld\n", password);
}
