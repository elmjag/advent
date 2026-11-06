#include <stddef.h>
#include <stdio.h>

typedef struct
{
    FILE *fp;
    char *line;
    size_t line_len;
} LineInput;

void line_input_init(LineInput *self, const char *file_name);
char *line_input_next(LineInput *self);
