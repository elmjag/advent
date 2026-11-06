#include <assert.h>

#include "line_input.h"

void line_input_init(LineInput *self, const char *file_name)
{
    self->line = NULL;
    self->line_len = 0;

    self->fp = fopen(file_name, "r");
    assert(self->fp != NULL);
}

char *line_input_next(LineInput *self)
{
    int res = getline(&(self->line), &(self->line_len), self->fp);
    if (res == -1)
    {
        return NULL;
    }
    return self->line;
}
