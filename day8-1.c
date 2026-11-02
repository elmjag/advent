#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define FILE_NAME "example-input-day8.txt"
#define FILE_NAME "input-day8.txt"

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
// PtrSet (set of unique pointers)
//
typedef struct
{
    Vec elements;
} PtrSet;

void ptr_set_init(PtrSet *self) { vec_init(&(self->elements), sizeof(void *)); }

void ptr_set_add(PtrSet *self, void *ptr)
{
    size_t len = vec_len(&(self->elements));
    for (size_t i = 0; i < len; i += 1)
    {
        void **e = vec_get(&(self->elements), i);
        if (*e == ptr)
        {
            // already present
            return;
        }
    }

    vec_push(&(self->elements), &ptr);
}

Vec *ptr_set_elements(PtrSet *self) { return &(self->elements); }

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
// Box
//
typedef struct
{
    int32_t x;
    int32_t y;
    int32_t z;
    Vec *connections;
} Box;

void box_init(Box *self, int32_t x, int32_t y, int32_t z)
{
    self->x = x;
    self->y = y;
    self->z = z;
    self->connections = vec_new(sizeof(Box *));
}

void box_self_connect(Box *self) { vec_push(self->connections, &self); }

void box_print(Box *self)
{
    printf("Box(%p %d %d %d con %p)\n", self, self->x, self->y, self->z,
           self->connections);

    printf("[ ");
    for (size_t i = 0; i < vec_len(self->connections); i += 1)
    {
        Box **b = vec_get(self->connections, i);
        printf("%p ", *b);
    }
    printf(" ]\n");
}

//
// BoxPair
//

typedef struct
{
    float distance;
    Box *box1;
    Box *box2;
} BoxPair;

void box_pair_init(BoxPair *self, float distance, Box *box1, Box *box2)
{
    self->distance = distance;
    self->box1 = box1;
    self->box2 = box2;
}

int box_pair_compare(const void *l, const void *r)
{
    const BoxPair *p1 = l;
    const BoxPair *p2 = r;

    if (p1->distance < p2->distance)
    {
        return -1;
    }

    if (p1->distance > p2->distance)
    {
        return 1;
    }

    return 0;
}

void box_pair_print(Vec *pairs)
{
    size_t len = vec_len(pairs);
    for (size_t i = 0; i < len; i += 1)
    {
        BoxPair *p = vec_get(pairs, i);
        printf("BoxPair[\n%f\n", p->distance);
        box_print(p->box1);
        box_print(p->box2);
        printf("]\n");
    }
}

//
// Utilities
//

void print_boxes(Vec *boxes)
{
    size_t len = vec_len(boxes);
    for (size_t i = 0; i < len; i += 1)
    {
        box_print(vec_get(boxes, i));
    }
}

size_t get_connections_num()
{
    if (FILE_NAME[0] == 'e')
    {
        return 10;
    }

    return 1000;
}

//
// Hauptbahnhof
//

void load_data(Input *input, Vec *boxes)
{
    char *line;
    int res;
    Box box;
    int32_t x, y, z;

    while ((line = input_next_line(input)) != NULL)
    {
        res = sscanf(line, "%d,%d,%d", &x, &y, &z);
        assert(res == 3);

        box_init(&box, x, y, z);
        vec_push(boxes, &box);
    }

    for (size_t i = 0; i < vec_len(boxes); i += 1)
    {
        Box *box = vec_get(boxes, i);
        box_self_connect(box);
    }
}

float box_distance(Box *b1, Box *b2)
{
    int64_t dx = b1->x - b2->x;
    int64_t dy = b1->y - b2->y;
    int64_t dz = b1->z - b2->z;

    return sqrtf((float)(dx * dx + dy * dy + dz * dz));
}

void calc_distances(Vec *boxes, Vec *box_pairs)
{
    BoxPair box_pair;
    size_t len = vec_len(boxes);
    for (size_t i = 0; i < len; i += 1)
    {
        Box *b1 = vec_get(boxes, i);
        for (size_t j = i + 1; j < len; j += 1)
        {
            Box *b2 = vec_get(boxes, j);
            float distance = box_distance(b1, b2);

            box_pair_init(&box_pair, distance, b1, b2);
            vec_push(box_pairs, &box_pair);
        }
    }

    qsort(box_pairs->data, vec_len(box_pairs), box_pairs->element_size,
          box_pair_compare);
}

void connect_box_pair(Box *b1, Box *b2)
{

    if (b1->connections == b2->connections)
    {
        // already connected
        return;
    }

    size_t len;
    Vec *cons = vec_new(sizeof(Box *));

    //
    // merge connections
    //
    len = vec_len(b1->connections);
    for (size_t i = 0; i < len; i += 1)
    {
        Box **box = vec_get(b1->connections, i);
        vec_push(cons, box);
    }

    len = vec_len(b2->connections);
    for (size_t i = 0; i < len; i += 1)
    {
        Box **box = vec_get(b2->connections, i);
        vec_push(cons, box);
    }

    //
    // attach new connections to all boxes
    //
    len = vec_len(cons);
    for (size_t i = 0; i < len; i += 1)
    {
        Box **box = vec_get(cons, i);
        (*box)->connections = cons;
    }
}

void connect_boxes(Vec *pairs)
{
    for (size_t i = 0; i < get_connections_num(); i += 1)
    {
        BoxPair *p = vec_get(pairs, i);
        connect_box_pair(p->box1, p->box2);
    }
}

int compare_vec_len(const void *l, const void *r)
{
    Vec *const *vl = l;
    Vec *const *vr = r;

    size_t len_l = vec_len(*vl);
    size_t len_r = vec_len(*vr);

    // we want to sort in 'reversed' order
    if (len_l < len_r)
    {
        return 1;
    }

    if (len_l > len_r)
    {
        return -1;
    }

    return 0;
}

int get_password(Vec *boxes)
{
    PtrSet uniq_cons;
    ptr_set_init(&uniq_cons);

    for (size_t i = 0; i < vec_len(boxes); i += 1)
    {
        Box *box = vec_get(boxes, i);
        Vec *cons = box->connections;

        ptr_set_add(&uniq_cons, cons);
    }

    Vec *cons = ptr_set_elements(&uniq_cons);

    qsort(cons->data, vec_len(cons), cons->element_size, compare_vec_len);

    int password = 1;
    for (size_t i = 0; i < 3; i += 1)
    {
        Vec **con = vec_get(cons, i);
        password *= vec_len(*con);
    }

    return password;
}

int main()
{
    Input input;
    input_init(&input);

    Vec boxes;
    vec_init(&boxes, sizeof(Box));

    load_data(&input, &boxes);

    Vec box_pairs;
    vec_init(&box_pairs, sizeof(BoxPair));

    calc_distances(&boxes, &box_pairs);

    connect_boxes(&box_pairs);

    int passowrd = get_password(&boxes);
    printf("password: %d\n", passowrd);
}
