#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
    *dial_pos = (*dial_pos + steps) % 100;
    if (*dial_pos == 0)
    {
        *password += 1;
    }
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
