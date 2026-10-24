#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// #define FILE_NAME "example-input-day6.txt"
#define FILE_NAME "input-day6.txt"

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
// Sheet
//
typedef struct
{
    Vec numbers;
    Vec operations;
} Sheet;

void sheet_init(Sheet *self)
{
    vec_init(&(self->numbers), sizeof(uint64_t));
    vec_init(&(self->operations), sizeof(char));
}

char _sheet_get_op(Sheet *self, size_t column)
{
    char *op = vec_get(&(self->operations), column);
    return *op;
}

uint64_t sheet_get_res(Sheet *self)
{
    size_t cols = vec_len(&(self->operations));
    uint64_t res[cols];

    // initial result vals
    for (size_t i = 0; i < cols; i += 1)
    {
        switch (_sheet_get_op(self, i))
        {
        case '*':
            res[i] = 1;
            break;
        case '+':
            res[i] = 0;
            break;
        default:
            assert(false);
        }
    }

    size_t nums_len = vec_len(&(self->numbers));

    for (size_t i = 0; i < nums_len; i += 1)
    {
        size_t curr_col = i % cols;
        uint64_t *num = vec_get(&(self->numbers), i);

        switch (_sheet_get_op(self, curr_col))
        {
        case '*':
            res[curr_col] *= *num;
            break;
        case '+':
            res[curr_col] += *num;
            break;
        default:
            assert(false);
        }
    }

    uint64_t grand_total = 0;
    for (size_t i = 0; i < cols; i += 1)
    {
        grand_total += res[i];
    }

    return grand_total;
}

void sheet_print(Sheet *self)
{
    size_t cols = vec_len(&(self->operations));
    size_t nums_len = vec_len(&(self->numbers));

    for (size_t i = 0; i < nums_len; i += 1)
    {
        uint64_t *num = vec_get(&(self->numbers), i);
        printf("%ld ", *num);
        if ((i + 1) % cols == 0)
        {
            printf("\n");
        }
    }

    for (size_t i = 0; i < cols; i += 1)
    {
        char *op = vec_get(&(self->operations), i);
        printf("%c ", *op);
    }
    printf("\n");
}

//
// Slice
//

typedef struct
{
    void *data;
    size_t element_size;
    size_t len;
} Slice;

void slice_init(Slice *self, void *data, size_t element_size, size_t len)
{
    self->data = data;
    self->element_size = element_size;
    self->len = len;
}

// pop first num elements
void slice_pop_elements(Slice *self, size_t num_elements)
{
    self->data = self->data + self->element_size * num_elements;
    self->len -= num_elements;
}

size_t slice_len(Slice *slice) { return slice->len; }

void *slice_get(Slice *self, size_t i)
{
    assert(i < self->len);
    return self->data + i * self->element_size;
}

char slice_get_char(Slice *self, size_t i)
{
    assert(self->element_size == sizeof(char));
    char *c = slice_get(self, i);

    return *c;
}

char slice_peek_char(Slice *self) { return slice_get_char(self, 0); }

//
// Utilities
//

void print_char_slice(Slice *slice)
{
    size_t len = slice_len(slice);
    printf("'");
    for (size_t i = 0; i < len; i += 1)
    {
        char *c = (char *)slice_get(slice, i);
        printf("%c", *c);
    }
    printf("'\n");
}

//
// Hauptbahnhof
//

void mmap_input_file(char **data, size_t *file_len)
{
    /* Open the file read/write */
    int fd = open(FILE_NAME, O_RDONLY);
    assert(fd != -1);

    /* Find out how big it is */
    struct stat st;
    int res = fstat(fd, &st);
    assert(res != -1);
    size_t len = st.st_size;
    assert(len > 0);

    /* Map the whole file into memory */
    char *map = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    assert(map != MAP_FAILED);

    /* The fd is no longer needed once mapped */
    close(fd);

    *data = map;
    *file_len = len;
}

bool get_line(Slice *data, Slice *line)
{
    size_t data_len = slice_len(data);
    if (data_len == 0)
    {
        // end-of-data
        return false;
    }

    char *start = data->data;
    size_t i = 0;
    for (; start[i] != '\n' && i < data_len; i += 1)
    {
    }

    slice_init(line, start, sizeof(char), i);
    if (i < data_len)
    {
        /* move past '\n' */
        i += 1;
    }

    slice_pop_elements(data, i);

    return true;
}

bool get_next_token(Slice *line, Slice *token)
{
    if (slice_len(line) == 0)
    {
        return false;
    }

    /* skip all leading spaces */
    size_t i = 0;
    size_t line_len = slice_len(line);
    for (; i < line_len && slice_get_char(line, i) == ' '; i += 1)
    {
    }
    slice_pop_elements(line, i);
    if (i >= line_len)
    {
        return false;
    }

    /* figure out token end */
    i = 0;
    line_len = slice_len(line);
    for (; i < line_len && slice_get_char(line, i) != ' '; i += 1)
    {
    }

    slice_init(token, line->data, sizeof(char), i);
    slice_pop_elements(line, i);
    return true;
}

void parse_num_line(Slice *line, Vec *numbers)
{
    Slice token;

    while (get_next_token(line, &token))
    {
        uint64_t num;
        int res = sscanf(token.data, "%ld", &num);
        assert(res == 1);
        vec_push(numbers, &num);
    }
}

void parse_ops_line(Slice *line, Vec *ops)
{
    Slice token;

    while (get_next_token(line, &token))
    {
        vec_push(ops, (char *)slice_get(&token, 0));
    }
}

void load_data(Sheet *sheet)
{
    char *data;
    size_t len;

    mmap_input_file(&data, &len);

    Slice remaining;
    slice_init(&remaining, data, sizeof(char), len);

    Slice line;
    while (get_line(&remaining, &line))
    {
        char first_char = slice_peek_char(&line);
        if (first_char == '*' || first_char == '+')
        {
            parse_ops_line(&line, &(sheet->operations));
        }
        else
        {
            parse_num_line(&line, &(sheet->numbers));
        }
    }
}

int main()
{
    Sheet sheet;
    sheet_init(&sheet);

    load_data(&sheet);

    uint64_t password = sheet_get_res(&sheet);
    printf("password: %ld\n", password);
}
