#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// #define FILE_NAME "example-input-day4.txt"
#define FILE_NAME "input-day4.txt"

//
// Vec
//

#define VEC_INC 1024

typedef struct
{
    size_t size;
    size_t next_free;
    bool *data;

} Vec;

void vec_init(Vec *self)
{
    self->size = VEC_INC;
    self->next_free = 0;
    self->data = malloc(self->size);
}

void vec_push(Vec *self, bool data)
{
    if (self->next_free >= self->size)
    {
        self->size += VEC_INC;
        self->data = realloc(self->data, self->size);
    }

    self->data[self->next_free] = data;
    self->next_free += 1;
}

bool vec_at(Vec *self, size_t i)
{
    assert(i < self->next_free);
    return self->data[i];
}

void vec_print(Vec *self)
{
    printf("[ ");
    for (size_t i = 0; i < self->next_free; i += 1)
    {
        printf("%d ", self->data[i]);
    }

    printf(" ] size %ld next_free %ld\n", self->size, self->next_free);
}

//
// load grid from file
//

void load_grid_data(int *width, int *height, Vec *cells)
{
    char *line = NULL;
    size_t len = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    assert(fp != NULL);

    ssize_t read = 0;
    *width = 0;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        *width = read - 1;
        *height += 1;
        for (size_t i = 0; line[i] != '\n'; i += 1)
        {
            char cell = line[i];
            assert(cell == '.' || cell == '@');
            vec_push(cells, cell == '@');
        }
    }

    free(line);
    fclose(fp);
}

//
// Grid
//

typedef struct
{
    int width;
    int height;
    Vec cells;
} Grid;

void grid_init(Grid *self)
{
    vec_init(&self->cells);
    load_grid_data(&self->width, &self->height, &self->cells);
}

bool grid_is_cell_occupied(Grid *self, int x, int y)
{
    int pos = y * self->width + x;
    return vec_at(&self->cells, pos);
}

bool grid_valid_cell_coord(Grid *self, int x, int y)
{
    return 0 <= x && x < self->width && 0 <= y && y < self->height;
}

int grid_count_occupied_neighbors(Grid *self, int x, int y)
{
    int occupied = 0;

    for (int ny = y - 1; ny <= y + 1; ny += 1)
    {
        for (int nx = x - 1; nx <= x + 1; nx += 1)
        {
            if ((nx == x && ny == y) || !grid_valid_cell_coord(self, nx, ny))
            {
                continue;
            }
            if (grid_is_cell_occupied(self, nx, ny))
            {
                occupied += 1;
            }
        }
    }

    return occupied;
}

int grid_count_accessable_cells(Grid *self)
{
    int accessable = 0;

    for (int y = 0; y < self->width; y += 1)
    {
        for (int x = 0; x < self->height; x += 1)
        {
            if (!grid_is_cell_occupied(self, x, y))
            {
                continue;
            }

            if (grid_count_occupied_neighbors(self, x, y) < 4)
            {
                accessable += 1;
            }
        }
    }

    return accessable;
}

void grid_print(Grid *self)
{
    for (int y = 0; y < self->width; y += 1)
    {
        for (int x = 0; x < self->height; x += 1)
        {
            char c = grid_is_cell_occupied(self, x, y) ? '@' : '.';
            printf("%c", c);
        }
        printf("\n");
    }
}

//
// Hauptbahnhof
//

void main()
{
    Grid grid;

    grid_init(&grid);

    printf("password: %d\n", grid_count_accessable_cells(&grid));
}
