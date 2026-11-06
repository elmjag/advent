#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

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
