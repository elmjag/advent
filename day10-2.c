#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#include "line_input.h"
#include "vec.h"

// #define FILE_NAME "example-input-day10.txt"
#define FILE_NAME "input-day10.txt"

//
// Machine
//

typedef struct
{
    Vec buttons;
    Vec joltages;
} Machine;

void machine_init(Machine *self)
{
    vec_init(&(self->buttons), sizeof(Vec));
    vec_init(&(self->joltages), sizeof(uint32_t));
}

Vec *machine_get_buttons(Machine *self) { return &(self->buttons); }

Vec *machine_get_joltages(Machine *self) { return &(self->joltages); }

void machine_reset(Machine *self)
{
    size_t len = vec_len(&(self->buttons));
    for (size_t i = 0; i < len; i += 1)
    {
        Vec **v = vec_get(&(self->buttons), i);
        vec_free(*v);
    }

    vec_clear(&(self->buttons));
    vec_clear(&(self->joltages));
}

void machine_button_print(Vec *button)
{
    size_t len = vec_len(button);
    printf("| ");
    for (size_t j = 0; j < len; j += 1)
    {
        uint32_t *n = vec_get(button, j);
        printf("%d ", *n);
    }
    printf("|");
}

void machine_print(Machine *self)
{
    size_t len;

    printf("Machine\n");

    printf("  buttons(");
    len = vec_len(&(self->buttons));
    for (size_t i = 0; i < len; i += 1)
    {
        Vec **button = vec_get(&(self->buttons), i);
        machine_button_print(*button);
    }
    printf(")\n");

    printf("  joltages(");
    len = vec_len(&(self->joltages));
    for (size_t i = 0; i < len; i += 1)
    {
        int32_t *j = vec_get(&(self->joltages), i);
        printf("%d ", *j);
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
// Combo
//
typedef struct
{
    Vec buttons;
    bool *parity;
    size_t parity_len;
} Combo;

Combo *combo_new(size_t parity_len)
{
    Combo *self = malloc(sizeof(Combo));

    vec_init(&(self->buttons), sizeof(Vec *));
    self->parity = calloc(parity_len, sizeof(bool));
    self->parity_len = parity_len;

    return self;
}

Vec *combo_get_buttons(Combo *self) { return &(self->buttons); }

void combo_add_button(Combo *self, Vec *button)
{
    vec_push(combo_get_buttons(self), &button);
}

bool combo_same_parity(Combo *self, bool *parity)
{
    for (size_t i = 0; i < self->parity_len; i += 1)
    {
        if (parity[i] != self->parity[i])
        {
            return false;
        }
    }

    return true;
}

size_t combo_len(Combo *self) { return vec_len(combo_get_buttons(self)); }

void combo_set_parity(Combo *self)
{
    Vec *buttons = combo_get_buttons(self);

    for (size_t i = 0; i < vec_len(buttons); i += 1)
    {
        Vec **button = vec_get(buttons, i);
        for (size_t j = 0; j < vec_len(*button); j += 1)
        {
            uint32_t *idx = vec_get(*button, j);
            self->parity[*idx] = !self->parity[*idx];
        }
    }
}

void combo_print(Combo *self)
{
    printf("combo <");

    Vec *buttons = combo_get_buttons(self);

    for (size_t j = 0; j < vec_len(buttons); j += 1)
    {
        Vec **button = vec_get(buttons, j);
        machine_button_print(*button);
    }

    printf("> (%ld) |", combo_len(self));

    for (size_t i = 0; i < self->parity_len; i += 1)
    {
        printf("%d", self->parity[i]);
    }
    printf("|\n");
}

//
// Utils
//
void print_parity(bool *parity, size_t len)
{
    printf("parity(");
    for (size_t i = 0; i < len; i += 1)
    {
        printf("%d", parity[i]);
    }
    printf(")\n");
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

void parse_joltages(Vec *joltages, StrSlice *slice)
{
    assert(slice->str[0] == '{');

    Splitter split;
    splitter_init(&split, slice->str + 1, slice->len - 2, ',');

    StrSlice nums;
    while (splitter_next(&split, &nums))
    {
        int32_t n;
        sscanf(nums.str, "%d", &n);
        vec_push(joltages, &n);
    }
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
            /* lights are ignored */
            break;
        case '(':
            Vec *buttons = parse_buttons(&slice);
            vec_push(&(machine->buttons), &buttons);
            break;
        case '{':
            parse_joltages(&(machine->joltages), &slice);
            break;
        default:
            assert(false); /* unexpected first character */
        }
    }

    return true;
}

void get_joltages_parity(int32_t *joltages, bool *parity, size_t len)
{
    for (size_t i = 0; i < len; i += 1)
    {
        parity[i] = (joltages[i] % 2) != 0;
    }
}

void get_button_combinations(Vec *buttons, Vec *combinations,
                             size_t joltages_len)
{
    size_t num_buttons = vec_len(buttons);
    uint32_t num_combos = 1 << num_buttons;

    for (uint32_t combo_bits = 0; combo_bits < num_combos; combo_bits += 1)
    {
        Combo *combo = combo_new(joltages_len);
        for (size_t button_idx = 0; button_idx < num_buttons; button_idx += 1)
        {
            if (combo_bits & 1 << button_idx)
            {
                Vec **button = vec_get(buttons, button_idx);
                combo_add_button(combo, *button);
            }
        }
        // printf("pushed %p\n", combo);
        combo_set_parity(combo);
        vec_push(combinations, &combo);
        // get_combo_parity(combo, joltages_len);
    }
}

void print_button_combinations(Vec *combinations)
{
    size_t len = vec_len(combinations);
    for (size_t i = 0; i < len; i += 1)
    {
        Combo **combo = vec_get(combinations, i);
        combo_print(*combo);
    }
}

bool is_zero_joltages(int32_t *joltages, size_t len)
{
    for (size_t i = 0; i < len; i += 1)
    {
        if (joltages[i] != 0)
        {
            return false;
        }
    }

    return true;
}

bool is_negative_joltages(int32_t *joltages, size_t len)
{
    for (size_t i = 0; i < len; i += 1)
    {
        if (joltages[i] < 0)
        {
            return true;
        }
    }

    return false;
}

void half_joltages(int32_t *joltages, size_t len)
{
    for (size_t i = 0; i < len; i += 1)
    {
        joltages[i] = joltages[i] / 2;
    }
}

void print_joltages(int32_t *joltages, size_t len)
{
    printf("joltages( ");
    for (size_t i = 0; i < len; i += 1)
    {
        printf("%d ", joltages[i]);
    }
    printf(")\n");
}

void apply(Combo *combo, int32_t *joltages, int32_t *new_joltages,
           size_t joltages_len)
{
    // copy joltages to new_joltages
    for (size_t i = 0; i < joltages_len; i += 1)
    {
        new_joltages[i] = joltages[i];
    }

    Vec *buttons = combo_get_buttons(combo);
    for (size_t i = 0; i < vec_len(buttons); i += 1)
    {
        Vec **button = vec_get(buttons, i);
        for (size_t j = 0; j < vec_len(*button); j += 1)
        {
            uint32_t *idx = vec_get(*button, j);
            new_joltages[*idx] -= 1;
        }
    }
}

int solve(Vec *button_combinations, int32_t *joltages, size_t joltages_len)
{
    bool parity[joltages_len];

    if (is_zero_joltages(joltages, joltages_len))
    {
        return 0;
    }

    get_joltages_parity(joltages, parity, joltages_len);

    int presses = 1000000;

    size_t num_combos = vec_len(button_combinations);
    int32_t new_joltages[joltages_len];

    for (size_t i = 0; i < num_combos; i += 1)
    {
        Combo **combo = vec_get(button_combinations, i);
        if (!combo_same_parity(*combo, parity))
        {
            continue;
        }

        apply(*combo, joltages, new_joltages, joltages_len);

        if (is_negative_joltages(new_joltages, joltages_len))
        {
            continue;
        }
        half_joltages(new_joltages, joltages_len);

        presses =
            MIN(presses,
                combo_len(*combo) +
                    solve(button_combinations, new_joltages, joltages_len) * 2);
    }

    return presses;
}

int find_solution(Machine *machine)
{
    Vec button_combinations;
    vec_init(&button_combinations, sizeof(Combo *));

    get_button_combinations(machine_get_buttons(machine), &button_combinations,
                            vec_len(machine_get_joltages(machine)));

    Vec *vec_joltages = machine_get_joltages(machine);
    size_t joltages_len = vec_len(vec_joltages);
    int32_t joltages[joltages_len];
    for (size_t i = 0; i < joltages_len; i += 1)
    {
        joltages[i] = *(int32_t *)vec_get(vec_joltages, i);
    }

    return solve(&button_combinations, joltages, joltages_len);
}

int main()
{
    LineInput li;
    line_input_init(&li, FILE_NAME);

    Machine mach;
    machine_init(&mach);

    int password = 0;
    while (load_data(&li, &mach))
    {
        password += find_solution(&mach);
        machine_reset(&mach);
        printf(".");
        fflush(stdout);
    }

    printf("password: %d\n", password);
}
