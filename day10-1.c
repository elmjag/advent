#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "line_input.h"
#include "vec.h"

// #define FILE_NAME "example-input-day10.txt"
#define FILE_NAME "input-day10.txt"

//
// Machine
//

typedef struct
{
    Vec lights;
    Vec buttons;
} Machine;

void machine_init(Machine *self)
{
    vec_init(&(self->lights), sizeof(bool));
    vec_init(&(self->buttons), sizeof(Vec));
}

Vec *machine_get_buttons(Machine *self) { return &(self->buttons); }

Vec *machine_get_lights(Machine *self) { return &(self->lights); }

void machine_reset(Machine *self)
{
    vec_clear(&(self->lights));

    size_t len = vec_len(&(self->buttons));
    for (size_t i = 0; i < len; i += 1)
    {
        Vec **v = vec_get(&(self->buttons), i);
        vec_free(*v);
    }

    vec_clear(&(self->buttons));
}

void machine_print(Machine *self)
{
    size_t len;

    printf("Machine\n");
    printf("  lights(");

    len = vec_len(&(self->lights));
    for (size_t i = 0; i < len; i += 1)
    {
        bool *l = vec_get(&(self->lights), i);
        printf("%d", *l);
    }
    printf(")\n");

    printf("  buttons(");

    len = vec_len(&(self->buttons));
    for (size_t i = 0; i < len; i += 1)
    {
        Vec **btns = vec_get(&(self->buttons), i);
        size_t num_btns = vec_len(*btns);
        printf("|");
        for (size_t j = 0; j < num_btns; j += 1)
        {
            uint32_t *n = vec_get(*btns, j);
            printf("%d ", *n);
        }
        printf("|");
    }
    printf(")\n");
}

//
// StrSlice
//
typedef struct
{
    char *str;
    size_t len;
} StrSlice;

void strslice_init(StrSlice *self, char *str, size_t len)
{
    self->str = str;
    self->len = len;
}

void strslice_print(StrSlice *self)
{
    printf("StrSlice('");
    for (size_t i = 0; i < self->len; i += 1)
    {
        printf("%c", self->str[i]);
    }
    printf("' len=%ld)\n", self->len);
}

//
// Splitter
//
typedef struct
{
    char *text;
    size_t len;
    char split_on;
} Splitter;

void splitter_init(Splitter *self, char *text, size_t len, char split_on)
{
    self->text = text;
    self->len = len;
    self->split_on = split_on;
}

bool splitter_next(Splitter *self, StrSlice *slice)
{
    if (self->len == 0)
    {
        return false;
    }

    char *start = self->text;
    size_t i = 0;
    for (; i < self->len && self->text[i] != self->split_on; i += 1)
    {
    }

    strslice_init(slice, start, i);

    if (i < self->len)
    {
        self->text += i + 1;
        self->len -= i + 1;
    }
    else
    {
        self->text += i;
        self->len = 0;
    }

    return true;
}

//
// Hauptbahnhof
//

void parse_lights(Vec *lights, StrSlice *slice)
{
    assert(slice->str[0] == '[');

    size_t start = 1;
    size_t end = 2;
    for (; slice->str[end] != ']'; end += 1)
    {
    }

    for (size_t i = start; i < end; i += 1)
    {
        bool res = slice->str[i] == '#';
        vec_push(lights, &res);
    }
}

Vec *parse_buttons(StrSlice *input)
{
    assert(input->str[0] == '(');
    assert(input->str[input->len - 1] == ')');

    Splitter split;
    splitter_init(&split, input->str + 1, input->len - 2, ',');

    StrSlice nums;
    Vec *buttons = vec_new(sizeof(uint32_t));

    while (splitter_next(&split, &nums))
    {
        uint32_t n;
        sscanf(nums.str, "%d", &n);
        vec_push(buttons, &n);
    }

    return buttons;
}

bool load_data(LineInput *li, Machine *machine)
{
    char *line = line_input_next(li);
    if (line == NULL)
    {
        return false;
    }

    Splitter split;
    splitter_init(&split, line, strlen(line), ' ');

    StrSlice slice;
    while (splitter_next(&split, &slice))
    {
        switch (slice.str[0])
        {
        case '[':
            parse_lights(&(machine->lights), &slice);
            break;
        case '(':
            Vec *buttons = parse_buttons(&slice);
            vec_push(&(machine->buttons), &buttons);
            break;
        case '{':
            /* joltages are ignored */
            break;
        default:
            assert(false); /* unexpected first character */
        }
    }

    return true;
}

// void dump(bool lights[], size_t len)
// {
//     for (size_t i = 0; i < len; i += 1)
//     {
//         printf("%ld -> %d\n", i, lights[i]);
//     }
// }

bool matches(Vec *expected, bool got[])
{
    size_t len = vec_len(expected);
    for (size_t i = 0; i < len; i += 1)
    {
        bool *e = vec_get(expected, i);
        if (*e != got[i])
        {
            return false;
        }
    }

    return true;
}

bool check_permutation(Machine *machine, Vec *perm)
{
    Vec *lights = machine_get_lights(machine);
    bool toggly_lights[vec_len(lights)];
    memset(toggly_lights, false, sizeof(toggly_lights));

    size_t len = vec_len(perm);
    for (size_t i = 0; i < len; i += 1)
    {
        Vec **btns = vec_get(perm, i);
        size_t btns_len = vec_len(*btns);
        for (size_t j = 0; j < btns_len; j += 1)
        {
            uint32_t *button = vec_get(*btns, j);
            toggly_lights[*button] = !toggly_lights[*button];
        }
    }

    return matches(lights, toggly_lights);
}

void free_perms(Vec *perms)
{
    for (size_t i = 0; i < vec_len(perms); i += 1)
    {
        Vec **p = vec_get(perms, i);
        vec_free(*p);
    }
    vec_free(perms);
}

size_t permutations(Machine *machine)
{
    size_t result = 0;

    Vec *buttons = machine_get_buttons(machine);
    Vec *perms = vec_new(sizeof(Vec *));

    size_t len = vec_len(buttons);
    for (size_t i = 0; i < len; i += 1)
    {
        Vec *btns = vec_get(buttons, i);
        Vec *p = vec_new(sizeof(Vec *));
        vec_push(p, btns);
        vec_push(perms, &p);
    }

    len = vec_len(perms);
    for (size_t i = 0; i < len; i += 1)
    {
        Vec **p = vec_get(perms, i);
        if (check_permutation(machine, *p))
        {
            result = vec_len(*p);
            free_perms(perms);

            return result;
        }
    }

    while (true)
    {
        Vec *new_perms = vec_new(sizeof(Vec *));

        for (size_t i = 0; i < vec_len(perms); i += 1)
        {
            Vec **p = vec_get(perms, i);
            for (size_t j = 0; j < vec_len(buttons); j += 1)
            {
                Vec *np = vec_copy(*p);
                Vec *btns = vec_get(buttons, j);
                vec_push(np, btns);
                vec_push(new_perms, &np);

                if (check_permutation(machine, np))
                {
                    result = vec_len(np);
                    free_perms(perms);
                    free_perms(new_perms);

                    return result;
                }
            }
        }

        free_perms(perms);
        perms = new_perms;
    }
}

int main()
{
    LineInput li;
    line_input_init(&li, FILE_NAME);

    Machine mach;
    machine_init(&mach);

    size_t password = 0;
    while (load_data(&li, &mach))
    {
        password += permutations(&mach);
        machine_reset(&mach);
        printf(".");
        fflush(stdout);
    }

    printf("\npassword: %ld\n", password);
}