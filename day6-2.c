#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// #define FILE_NAME "example-input-day6.txt"
#define FILE_NAME "input-day6.txt"

//
// Sheet
//
typedef struct
{
    void *text;
    size_t text_len;
    size_t num_lines;
    size_t line_len;
} Sheet;

size_t get_line_width(char *data)
{
    size_t line_len = 0;
    for (; data[line_len] != '\n'; line_len += 1)
    {
    }

    return line_len;
}

void sheet_init(Sheet *self, void *text, size_t text_len)
{
    self->text = text;
    self->text_len = text_len;
    self->line_len = get_line_width(text);
    self->num_lines = text_len / self->line_len;
}

//
// Hauptbahnhof
//

void mmap_input_file(Sheet *sheet)
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

    sheet_init(sheet, map, len);
}

size_t get_col_width(char *start)
{
    char *p = start + 1;
    size_t i = 1;
    for (; p[i] == ' '; i += 1)
    {
    }
    if (p[i] == '\n')
    {
        i += 1;
    }

    return i;
}

uint64_t get_num(Sheet *sheet, size_t offset)
{
    char buf[sheet->num_lines];
    for (size_t i = 0; i < sheet->num_lines - 1; i += 1)
    {
        char c = ((char *)sheet->text)[(sheet->line_len + 1) * i + offset];
        buf[i] = c;
    }
    buf[sheet->num_lines - 1] = '\0';
    uint64_t num;
    sscanf(buf, "%ld", &num);

    return num;
}

uint64_t get_init_val(char op)
{
    switch (op)
    {
    case '*':
        return 1;
    case '+':
        return 0;
    default:
        // unexpected op
        assert(false);
    }
}

void do_op(char op, uint64_t *res, uint64_t num)
{
    switch (op)
    {
    case '*':
        *res *= num;
        break;
    case '+':
        *res += num;
        break;
    default:
        // unexpected op
        assert(false);
    }
}

uint64_t process(Sheet *sheet) // char *data, size_t data_len, size_t line_len)
{
    char *ops_start = sheet->text + sheet->text_len - sheet->line_len - 1;
    size_t offset = 0;
    uint64_t total = 0;

    while (offset < sheet->line_len)
    {
        char op = ops_start[offset];
        uint64_t res = get_init_val(op);
        ;

        size_t col_width = get_col_width(ops_start + offset);
        for (size_t i = 0; i < col_width; i += 1)
        {
            uint64_t num = get_num(sheet, offset + i);
            do_op(op, &res, num);
        }
        total += res;

        offset += col_width + 1;
    }

    return total;
}

void print_lines(Sheet *sheet) // char *data, size_t data_len, size_t line_len)
{
    size_t num_lines = sheet->num_lines;
    size_t line_len = sheet->line_len;
    printf("num_lines %ld\n", num_lines);
    char *p = sheet->text;
    for (int j = 0; j < num_lines; j += 1)
    {
        printf("'");
        for (size_t i = 0; i < line_len; i += 1)
        {
            printf("%c", p[i]);
        }
        p += line_len + 1;
        printf("'\n");
    }
}

int main()
{
    Sheet sheet;

    mmap_input_file(&sheet);
    printf("password: %ld\n", process(&sheet));
}
