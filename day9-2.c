#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include "line_input.h"
#include "vec.h"

// #define FILE_NAME "example-input-day9.txt"
#define FILE_NAME "input-day9.txt"

//
// Point
//

typedef struct
{
    int32_t x;
    int32_t y;
} Point;

//
// Line
//

typedef struct
{
    Point *start;
    Point *end;
    bool is_horizontal;
} Line;

void line_init(Line *self, Point *start, Point *end)
{
    self->start = start;
    self->end = end;

    if (start->x == end->x)
    {
        self->is_horizontal = false;
    }
    else
    {
        self->is_horizontal = true;
    }
}

bool line_is_crossing(Line *self, Line *line)
{
    assert(self->is_horizontal != line->is_horizontal);
    if (self->is_horizontal)
    {
        int a = MIN(self->start->x, self->end->x);
        int b = MAX(self->start->x, self->end->x);

        int c = MIN(line->start->y, line->end->y) + 1;
        int d = MAX(line->start->y, line->end->y) - 1;

        return a <= line->start->x && line->start->x <= b &&
               c <= self->start->y && self->start->y <= d;
    }
    else
    {
        int a = MIN(self->start->y, self->end->y);
        int b = MAX(self->start->y, self->end->y);

        int c = MIN(line->start->x, line->end->x) + 1;
        int d = MAX(line->start->x, line->end->x) - 1;

        return a <= line->start->y && line->start->y <= b &&
               c <= self->start->x && self->start->x <= d;
    }
}

//
// Floor
//

typedef struct
{
    Vec tiles;
    Vec hlines;
    Vec vlines;
} Floor;

void floor_init(Floor *self)
{
    vec_init(&(self->tiles), sizeof(Point));
    vec_init(&(self->hlines), sizeof(Line));
    vec_init(&(self->vlines), sizeof(Line));
}

void floor_add_tile(Floor *self, Point *tile)
{
    vec_push(&(self->tiles), tile);
}

size_t floor_num_tiles(Floor *self) { return vec_len(&(self->tiles)); }

void floor_add_line(Floor *self, Line *line)
{
    if (line->is_horizontal)
    {
        vec_push(&(self->hlines), line);
    }
    else
    {
        vec_push(&(self->vlines), line);
    }
}

void floor_print(Floor *self)
{
    printf("--- tiles ---\n");
    size_t len = vec_len(&(self->tiles));
    for (size_t i = 0; i < len; i += 1)
    {
        Point *p = vec_get(&(self->tiles), i);
        printf("(%d %d)\n", p->x, p->y);
    }
}

bool floor_crosses_outline(Floor *self, Line *line)
{
    Vec *lines = NULL;
    if (line->is_horizontal)
    {
        lines = &(self->vlines);
    }
    else
    {
        lines = &(self->hlines);
    }

    size_t len = vec_len(lines);
    for (size_t i = 0; i < len; i += 1)
    {
        if (line_is_crossing(vec_get(lines, i), line))
        {
            return true;
        }
    }

    return false;
}

//
// Rect
//

typedef struct
{
    uint64_t area;
    Point *t1;
    Point *t2;
} Rect;

void rect_init(Rect *self, Point *t1, Point *t2)
{
    uint64_t dx = abs(t2->x - t1->x) + 1;
    uint64_t dy = abs(t2->y - t1->y) + 1;

    self->area = dx * dy;
    self->t1 = t1;
    self->t2 = t2;
}

int rect_compare(const void *l, const void *r)
{
    uint64_t l_area = ((Rect *)l)->area;
    uint64_t r_area = ((Rect *)r)->area;

    if (l_area > r_area)
    {
        return -1;
    }

    if (l_area < r_area)
    {
        return 1;
    }

    return 0;
}

//
// Utils
//

void lines_print(Vec *lines)
{
    Line *l = NULL;
    size_t len = vec_len(lines);
    for (size_t i = 0; i < len; i += 1)
    {
        l = vec_get(lines, i);
        printf("(%d %d) -> (%d %d)\n", l->start->x, l->start->y, l->end->x,
               l->end->y);
    }
}

void rects_print(Vec *rects)
{
    Rect *r = NULL;
    size_t len = vec_len(rects);
    for (size_t i = 0; i < len; i += 1)
    {
        r = vec_get(rects, i);
        printf("%ld (%d %d) (%d %d)\n", r->area, r->t1->x, r->t1->y, r->t2->x,
               r->t2->y);
    }
}

//
// Hauptbahnhof
//

void load_data(Floor *floor)
{
    LineInput li;
    line_input_init(&li, FILE_NAME);
    char *line;
    Point p;

    while ((line = line_input_next(&li)) != NULL)
    {
        int res = sscanf(line, "%d,%d", &p.x, &p.y);
        assert(res == 2);
        floor_add_tile(floor, &p);
    }
}

void make_lines(Floor *floor)
{
    Point *t1 = vec_get(&(floor->tiles), 0);
    Point *t2 = NULL;

    Line line;

    size_t len = floor_num_tiles(floor);
    for (size_t i = 1; i < len; i += 1)
    {
        t2 = vec_get(&(floor->tiles), i);

        line_init(&line, t1, t2);
        floor_add_line(floor, &line);

        t1 = t2;
    }

    line_init(&line, t2, vec_get(&(floor->tiles), 0));
    floor_add_line(floor, &line);
}

void make_rects(Floor *floor, Vec *rects)
{
    Rect rect;

    size_t len = floor_num_tiles(floor);
    for (size_t i = 0; i < len; i += 1)
    {
        Point *t1 = vec_get(&(floor->tiles), i);
        for (size_t j = i + 1; j < len; j += 1)
        {
            Point *t2 = vec_get(&(floor->tiles), j);
            rect_init(&rect, t1, t2);
            vec_push(rects, &rect);
        }
    }

    qsort(rects->data, vec_len(rects), rects->element_size, rect_compare);
}

bool valid_rect(Floor *floor, Point *t1, Point *t2)
{
    Point p1 = *t1;
    Point p2 = {x : t2->x, y : t1->y};
    Point p3 = *t2;
    Point p4 = {x : t1->x, y : t2->y};

    Line line;

    line_init(&line, &p1, &p2);
    if (floor_crosses_outline(floor, &line))
    {
        return false;
    }

    line_init(&line, &p2, &p3);
    if (floor_crosses_outline(floor, &line))
    {
        return false;
    }

    line_init(&line, &p3, &p4);
    if (floor_crosses_outline(floor, &line))
    {
        return false;
    }

    line_init(&line, &p4, &p1);
    if (floor_crosses_outline(floor, &line))
    {
        return false;
    }

    return true;
}

uint64_t first_valid_rect(Floor *floor, Vec *rects)
{
    Rect *rect = NULL;
    size_t len = vec_len(rects);
    for (size_t i = 0; i < len; i += 1)
    {
        rect = vec_get(rects, i);
        if (valid_rect(floor, rect->t1, rect->t2))
        {
            return rect->area;
        }
    }

    assert(false); // no valid rectangle found
}

int main()
{
    Floor floor;
    floor_init(&floor);

    load_data(&floor);
    make_lines(&floor);

    Vec rects;
    vec_init(&rects, sizeof(Rect));

    make_rects(&floor, &rects);
    uint64_t password = first_valid_rect(&floor, &rects);
    printf("password: %ld\n", password);
}
