#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define NOTCHES 100

bool next_move(FILE *fp, int *steps)
{
    char *line = NULL;
    size_t len = 0;

    ssize_t read = getline(&line, &len, fp);
    if (read == -1)
    {
        return false;
    }

    *steps = atoi(&line[1]);
    if (line[0] == 'L')
    {
        *steps = -(*steps);
    }

    free(line);

    return true;
}

void move_dial(int *dial_pos, int *password, int steps)
{
    int pos = *dial_pos;
    int dir = steps < 0 ? -1 : 1;
    int remaining_steps = abs(steps);
    int zero_passed = 0;

    while (remaining_steps > 0)
    {
        pos = (pos + dir) % NOTCHES;
        if (pos == 0)
        {
            zero_passed += 1;
        }

        remaining_steps -= 1;
    }

    *dial_pos = pos;
    *password += zero_passed;
}

void main()
{
    FILE *fp = fopen("input-day1.txt", "r");
    assert(fp != NULL);

    int dial_pos = 50;
    int password = 0;
    int steps;
    while (next_move(fp, &steps))
    {
        move_dial(&dial_pos, &password, steps);
    }

    printf("password: %d\n", password);

    fclose(fp);
}
