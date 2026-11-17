#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vec.h"

#define FILE_NAME "input-day12.txt"

void mmap_input_file(char **data, size_t *len)
{
    /* Open the file read/write */
    int fd = open(FILE_NAME, O_RDONLY);
    assert(fd != -1);

    /* Find out how big it is */
    struct stat st;
    int res = fstat(fd, &st);
    assert(res != -1);
    *len = st.st_size;
    assert(len > 0);

    /* Map the whole file into memory */
    *data = mmap(NULL, *len, PROT_READ, MAP_SHARED, fd, 0);
    assert(*data != MAP_FAILED);

    /* The fd is no longer needed once mapped */
    close(fd);
}

#define SHAPE_LEN 0x10

void parse_shapes(Vec *shapes, char *data)
{
    for (size_t i = 0; i < 6; i += 1)
    {
        int n = 0;
        for (size_t j = 0; j < SHAPE_LEN; j += 1)
        {
            if (data[i * SHAPE_LEN + j] == '#')
            {
                n += 1;
            }
        }
        vec_push(shapes, &n);
    }
}

#define NUM_SHAPES 6
#define REG_LEN 25

bool check_region(Vec *shapes, int w, int h, int *shape_num)
{
    int footprint = 0;
    for (size_t i = 0; i < NUM_SHAPES; i += 1)
    {
        int shape_footprint = *(int *)vec_get(shapes, i);
        footprint += shape_footprint * shape_num[i];
    }

    return footprint <= w * h;
}

int count_regions(Vec *shapes, char *data, size_t len)
{
    int res, w, h;
    int num[NUM_SHAPES];
    int password = 0;

    for (char *p = data; p < data + len; p += REG_LEN)
    {
        res = sscanf(p, "%dx%d: %d %d %d %d %d %d", &w, &h, num, num + 1,
                     num + 2, num + 3, num + 4, num + 5);
        assert(res == 8);
        if (check_region(shapes, w, h, num))
        {
            password += 1;
        }
    }

    return password;
}

int main()
{
    Vec shapes;
    vec_init(&shapes, sizeof(int));
    char *data;
    size_t len;

    mmap_input_file(&data, &len);

    parse_shapes(&shapes, data);
    int password =
        count_regions(&shapes, data + (SHAPE_LEN * 6), len - (SHAPE_LEN * 6));

    printf("password: %d\n", password);
}

