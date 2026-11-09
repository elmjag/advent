#include <stddef.h>

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

void vec_init(Vec *self, size_t element_size);
Vec *vec_new(size_t element_size);
void vec_free(Vec *self);
void vec_push(Vec *self, void *data);
void *vec_get(Vec *self, size_t i);
size_t vec_len(const Vec *self);
void vec_clear(Vec *self);
Vec *vec_copy(Vec *source);
